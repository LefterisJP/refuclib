/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */

#include <rflib/datastructs/sbuffer.h>
#include <rflib/datastructs/darray.h>
#include <rflib/utils/memory.h>

struct RFsbuffer_stack {
    struct {darray(size_t);} index_stack;
};

struct RFsbuffer_stack *rf_sbuffer_stack_create()
{
    struct RFsbuffer_stack *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    darray_init(ret->index_stack);
    return ret;
}

void rf_sbuffer_stack_destroy(struct RFsbuffer_stack *b)
{
    darray_free(b->index_stack);
    free(b);
}


bool rf_sbuffer_init(struct RFsbuffer *b, size_t size, rf_sbuffer_realloc_cb cb)
{
    b->size = size;
    b->index = 0;
    b->realloc_cb = cb;
    RF_CALLOC(b->buff, size, 1, return false);
    b->stack = rf_sbuffer_stack_create();
    return b->stack;
}

void rf_sbuffer_deinit(struct RFsbuffer *b)
{
    rf_sbuffer_stack_destroy(b->stack);
    free(b->buff);
}

static inline size_t rf_sbuffer_remsize(const struct RFsbuffer *b)
{
    return b->size - b->index;
}

static inline bool rf_sbuffer_realloc(struct RFsbuffer *b, size_t new_size)
{
    // else we need to realloc
    RF_REALLOC(b->buff, char, new_size, return false);
    return b->realloc_cb ? b->realloc_cb(b) : true;
}

void *rf_sbuffer_alloc(struct RFsbuffer *b, size_t size)
{
    char *ret = b->buff + b->index;
    if (rf_sbuffer_remsize(b) >= size) {
        b->index += size;
        return ret;
    }

    size_t new_size = b->size > size ? b->size : size;
    new_size *= 2;
    // else we need to realloc
    if (!rf_sbuffer_realloc(b, new_size)) {
        return NULL;
    }
    // assignment needs to happen again due to realloc
    ret = b->buff + b->index;
    b->index += size;
    b->size = new_size;
    return ret;
}

void *rf_sbuffer_extend(struct RFsbuffer *b, size_t added_size)
{
    char *ret = b->buff + darray_top(b->stack->index_stack);
    if (rf_sbuffer_remsize(b) >= added_size) {
        b->index += added_size;
        return ret;
    }
    size_t new_size = b->size + added_size;
    new_size *= 2;
    if (!rf_sbuffer_realloc(b, new_size)) {
        return NULL;
    }
    // assignment needs to happen again due to realloc
    ret = b->buff + darray_top(b->stack->index_stack);
    b->index += added_size;
    b->size = new_size;
    return ret;
}

void rf_sbuffer_push(struct RFsbuffer *b)
{
    darray_append(b->stack->index_stack, b->index);
}

void rf_sbuffer_pop(struct RFsbuffer *b)
{
    RF_ASSERT_OR_EXIT(!darray_empty(b->stack->index_stack), "Tried to pop empty buffer");
    b->index = darray_pop(b->stack->index_stack);
}
