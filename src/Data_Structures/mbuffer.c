#include <Data_Structures/mbuffer.h>
#include <Data_Structures/darray.h>
#include <Utils/memory.h>

/* -- RFmbuffer_stack functions -- */
struct RFmbuffer_stack {
    struct {darray(size_t);} block_stack;
    struct {darray(size_t);} block_index_stack;
};

static struct RFmbuffer_stack *rf_mbuffer_stack_create()
{
    struct RFmbuffer_stack *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    darray_init(ret->block_stack);
    darray_init(ret->block_index_stack);
    return ret;
}

static void rf_mbuffer_stack_destroy(struct RFmbuffer_stack *b)
{
    darray_free(b->block_index_stack);
    darray_free(b->block_stack);
    free(b);
}

static inline void rf_mbuffer_stack_push(struct RFmbuffer_stack *b,
                                         size_t curr_block,
                                         size_t curr_idx)
{
    darray_append(b->block_stack, curr_block);
    darray_append(b->block_index_stack, curr_idx);
}

static inline void rf_mbuffer_stack_pop(struct RFmbuffer_stack *b,
                                        size_t *saved_block,
                                        size_t *saved_idx)
{
    RF_ASSERT_OR_EXIT(!darray_empty(b->block_stack) && !darray_empty(b->block_index_stack),
                      "Tried to pop empty buffer");
    *saved_block = darray_pop(b->block_stack);
    *saved_idx = darray_pop(b->block_index_stack);
}

/* -- RFmbuffer_block functions -- */

struct RFmbuffer_block {
    char *data;
    size_t size;
    size_t index;
};

static bool rf_mbuffer_block_init(struct RFmbuffer_block *b, size_t size)
{
    RF_MALLOC(b->data, size, return false);
    b->size = size;
    b->index = 0;
    return true;
}

static struct RFmbuffer_block *rf_mbuffer_block_create(size_t size)
{
    struct RFmbuffer_block *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    return rf_mbuffer_block_init(ret, size) ? ret : NULL;
}

static void rf_mbuffer_block_deinit(struct RFmbuffer_block *b)
{
    free(b->data);
}

static void rf_mbuffer_block_destroy(struct RFmbuffer_block *b)
{
    rf_mbuffer_block_deinit(b);
    free(b);
}

static inline size_t rf_mbuffer_block_remsize(const struct RFmbuffer_block *b)
{
    return b->size - b->index;
}

static char *rf_mbuffer_block_tryalloc(struct RFmbuffer_block *b, size_t size)
{
    if (size > rf_mbuffer_block_remsize(b)) {
        return NULL;
    }
    size_t old_index = b->index;
    b->index += size;
    return b->data + old_index;
}

static inline char *rf_mbuffer_block_currptr(const struct RFmbuffer_block *b)
{
    return b->data + b->index;
}

/* -- RFmbuffer functions -- */

bool rf_mbuffer_init(struct RFmbuffer *b, size_t initial_buffer_size)
{
    b->blocks_num = 1;
    b->curr_block_idx = 0;
    RF_MALLOC(b->blocks, sizeof(*b->blocks), return false);
    b->blocks[0] = rf_mbuffer_block_create(initial_buffer_size);
    if (!b->blocks[0]) {
        free(b->blocks);
        return false;
    }
    b->stack = rf_mbuffer_stack_create();
    return b->stack;
}

void rf_mbuffer_deinit(struct RFmbuffer *b)
{
    rf_mbuffer_stack_destroy(b->stack);
    unsigned int i = 0;
    for (i = 0; i < b->blocks_num; ++i) {
        rf_mbuffer_block_destroy(b->blocks[i]);
    }
    free(b->blocks);
}

void *rf_mbuffer_alloc(struct RFmbuffer *b, size_t size)
{
    char *ret;
    unsigned int i;
    // try to allocate the size in any of the existing blocks
    for (i = b->curr_block_idx; i < b->blocks_num; ++i) {
        ret = rf_mbuffer_block_tryalloc(b->blocks[i], size);
        if (ret) {
            b->curr_block_idx = i;
            return ret;
        }
    }

    // no luck, we have to create a new block
    size_t new_size = b->blocks[b->curr_block_idx]->size > size
                    ? b->blocks[b->curr_block_idx]->size * 2
                    : size * 2;
    RF_REALLOC(b->blocks,
               struct RFmbuffer_block* ,
               (b->blocks_num + 1) * sizeof(*b->blocks),
               return NULL);
    b->blocks_num += 1;
    b->curr_block_idx += 1;
    b->blocks[b->curr_block_idx] = rf_mbuffer_block_create(new_size);
    if (!b->blocks[b->curr_block_idx]) {
        return NULL;
    }
    return rf_mbuffer_block_tryalloc(b->blocks[b->curr_block_idx], size);
}

bool rf_mbuffer_extend(struct RFmbuffer *b, size_t added_size)
{
    if (added_size > rf_mbuffer_currblock_remsize(b)) {
        return false;
    }
    b->blocks[b->curr_block_idx]->index += added_size;
    return true;
}

void rf_mbuffer_shrink(struct RFmbuffer *b, size_t shrink_size)
{
    if (b->blocks[b->curr_block_idx]->index > shrink_size) {
        b->blocks[b->curr_block_idx]->index -= shrink_size;
    } else {
        b->blocks[b->curr_block_idx]->index = 0;
    }
}

size_t rf_mbuffer_currblock_remsize(const struct RFmbuffer *b)
{
    return rf_mbuffer_block_remsize(b->blocks[b->curr_block_idx]);
}

void *rf_mbuffer_currblock_currptr(const struct RFmbuffer *b)
{
    return rf_mbuffer_block_currptr(b->blocks[b->curr_block_idx]);
}

void rf_mbuffer_push(struct RFmbuffer *b)
{
    rf_mbuffer_stack_push(b->stack,
                          b->curr_block_idx,
                          b->blocks[b->curr_block_idx]->index);
}

void rf_mbuffer_pop(struct RFmbuffer *b)
{
    size_t saved_block_idx;
    size_t saved_idx;
    unsigned int i;
    rf_mbuffer_stack_pop(b->stack, &saved_block_idx, &saved_idx);
    // just a sanity check
    RF_ASSERT(saved_block_idx <= b->curr_block_idx, "Popping greater block index?");
    // for all in between blocks make sure their indexes are also reset
    for (i = b->curr_block_idx; i > saved_block_idx; --i) {
        b->blocks[b->curr_block_idx]->index = 0;
    }
    // set block and block index
    b->curr_block_idx = saved_block_idx;
    b->blocks[b->curr_block_idx]->index = saved_idx;
}
