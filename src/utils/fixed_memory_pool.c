#include <rflib/utils/fixed_memory_pool.h>

#include <rflib/utils/memory.h>
#include <rflib/utils/sanity.h>

#include <math.h>

/* this meta data is stored in all of the unused blocks of the pool */
struct rf_fixed_memorypool_meta {
    //! index number of the next memory block
    uint32_t idx;
};
#define POOL_META_SIZE sizeof(struct rf_fixed_memorypool_meta)


/* -- pool chunk functions start -- */
static inline void rf_fixed_memorypool_chunk_meta_set_idx(
    struct rf_fixed_memorypool_chunk *chunk,
    uint32_t element_size,
    uint32_t block_idx,
    uint32_t next_idx);

static inline bool rf_fixed_memorypool_chunk_init(
    struct rf_fixed_memorypool_chunk *c,
    size_t chunk_size,
    size_t element_size)
{
    unsigned int i;
    RF_MALLOC(c->blocks, chunk_size, return false);
    c->next = 0;
    c->free_blocks = chunk_size / element_size;

    /* create the linked list of unused blocks */
    for (i = 0; i < c->free_blocks; ++i) {
        rf_fixed_memorypool_chunk_meta_set_idx(c,
                                               element_size,
                                               i,
                                               i + 1);
    }
    return true;
}

static inline struct rf_fixed_memorypool_chunk *
rf_fixed_memorypool_chunk_create(size_t chunk_size, size_t element_size)
{
    struct rf_fixed_memorypool_chunk *ret;
    RF_MALLOC(ret, sizeof(struct rf_fixed_memorypool_chunk), return NULL);
    if (!rf_fixed_memorypool_chunk_init(ret, chunk_size, element_size)) {
        free(ret);
        return NULL;
    }
    return ret;
}

static inline void rf_fixed_memorypool_chunk_destroy(
    struct rf_fixed_memorypool_chunk *c)
{
    free(c->blocks);
    free(c);
}

static inline void* rf_fixed_memorypool_chunk_addr_from_index(
    struct rf_fixed_memorypool_chunk *chunk,
    uint32_t element_size,
    uint32_t idx)
{
    return chunk->blocks + (idx * element_size);
}

static inline void rf_fixed_memorypool_chunk_calculate_next_idx(
    struct rf_fixed_memorypool_chunk *chunk,
    uint32_t element_size,
    uint32_t idx)
{
    struct rf_fixed_memorypool_meta *meta;
    meta = rf_fixed_memorypool_chunk_addr_from_index(chunk, element_size, idx);
    chunk->next = meta->idx;
}

static inline void rf_fixed_memorypool_chunk_meta_set_idx(
    struct rf_fixed_memorypool_chunk *chunk,
    uint32_t element_size,
    uint32_t block_idx,
    uint32_t next_idx)
{
    struct rf_fixed_memorypool_meta *meta;
    meta = rf_fixed_memorypool_chunk_addr_from_index(chunk, element_size, block_idx);
    meta->idx = next_idx;
}

static inline uint32_t rf_fixed_memorypool_chunk_index_from_addr(
    struct rf_fixed_memorypool_chunk *chunk,
    uint32_t element_size,
    uint8_t *p)
{
    return (p - chunk->blocks) / element_size;
}

/* -- pool functions start -- */

bool rf_fixed_memorypool_init(struct rf_fixed_memorypool *pool,
                              size_t element_size,
                              size_t chunk_size)
{
    pool->element_size = element_size;
    if (element_size < POOL_META_SIZE) {
        pool->element_size = POOL_META_SIZE;
        RF_WARNING("A fixed memory pool element_size should have at least %zu "
                   "size in bytes. Wasting %zu bytes per element",
                   POOL_META_SIZE,
                   POOL_META_SIZE - element_size);
    }

    /* allocate the chunks array, with only 1 chunk to begin with */
    RF_MALLOC(pool->chunks, sizeof(*pool->chunks), return false);
    pool->chunk_size = chunk_size;
    pool->chunks[0] = rf_fixed_memorypool_chunk_create(chunk_size, element_size);
    if (!pool->chunks[0]) {
        RF_ERROR("Failed to allocate the initial chunk for a fixed size memory pool");
        return false;
    }
    pool->active_chunk = pool->chunks[0];
    pool->chunks_num = 1;

    return true;
}

struct rf_fixed_memorypool *rf_fixed_memorypool_create(size_t element_size,
                                                       size_t chunk_size)
{
    struct rf_fixed_memorypool *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);

    if (!rf_fixed_memorypool_init(ret, element_size, chunk_size)) {
        free(ret);
        return NULL;
    }

    return ret;
}

void rf_fixed_memorypool_deinit(struct rf_fixed_memorypool *pool)
{
    unsigned int i;
    for (i = 0; i < pool->chunks_num; i++) {
        rf_fixed_memorypool_chunk_destroy(pool->chunks[i]);
    }
    free(pool->chunks);
}

void rf_fixed_memorypool_destroy(struct rf_fixed_memorypool *pool)
{
    rf_fixed_memorypool_deinit(pool);
    free(pool);
}

void *rf_fixed_memorypool_alloc_element(struct rf_fixed_memorypool *pool)
{
    unsigned int i;
    bool found_chunk = false;
    void *allocated_element;

    /* if we are running out of space in the current chunk */
    if (pool->active_chunk->free_blocks == 0) {
        /* find a chunk with free blocks */
        for (i = 0; i < pool->chunks_num; i ++) {
            if (pool->chunks[i]->free_blocks != 0) {
                pool->active_chunk = pool->chunks[i];
                found_chunk = true;
                break;
            }
        }

        if (!found_chunk) {
            /* if we have no free blocks in any of the existing chunks */

            pool->chunks_num +=1;
            RF_REALLOC(pool->chunks,
                       struct rf_fixed_memorypool_chunk*,
                       sizeof(*pool->chunks) * pool->chunks_num,
                       goto err);

            pool->chunks[pool->chunks_num - 1] = rf_fixed_memorypool_chunk_create(
                pool->chunk_size, pool->element_size);
            if (!pool->chunks[pool->chunks_num - 1]) {
                RF_ERROR("Failed to allocate a new chunk for a fixed size "
                         "memory pool");
                goto err;
            }
            pool->active_chunk = pool->chunks[pool->chunks_num - 1];
        }
    }
    pool->active_chunk->free_blocks -= 1;

    /* get this block's element to return and also set pool's next index */
    allocated_element = rf_fixed_memorypool_chunk_addr_from_index(
        pool->active_chunk,
        pool->element_size,
        pool->active_chunk->next);
    rf_fixed_memorypool_chunk_calculate_next_idx(pool->active_chunk,
                                                 pool->element_size,
                                                 pool->active_chunk->next);

    return allocated_element;

  err:
    RF_ERROR("Failed to allocate a memory pool element");
    return NULL;
}

bool rf_fixed_memorypool_free_element(struct rf_fixed_memorypool *pool,
                                      void *element)
{
    unsigned int i;
    int chunk_idx = -1;

    /* find which chunk it belongs to */
    for (i = 0; i < pool->chunks_num; i++) {
        if ((uint8_t*)element >= pool->chunks[i]->blocks &&
            (uint8_t*)element < pool->chunks[i]->blocks + pool->chunk_size) {
            chunk_idx = i;
            break;
        }
    }

    if (RF_CRITICAL_TEST(chunk_idx == -1,
                         "Attempted to free an element which does not belong "
                         "to any of the pool's chunks")) {
        return false;
    }

    pool->active_chunk = pool->chunks[chunk_idx];
    uint32_t previous_next = pool->active_chunk->next;
    /* get index of freed element */
    uint32_t idx = rf_fixed_memorypool_chunk_index_from_addr(pool->active_chunk,
                                                             pool->element_size,
                                                             element);

    // just a sanity check
    RF_ASSERT(idx != previous_next, "Attempted to free an element at which the"
              " chunk's next pointer was pointing to");

    /*
     * chunk->next should point to it and also set the (now) unused block's index
     * to point to the previous next thus adding to the linked list
     */
    pool->active_chunk->next = idx;
    rf_fixed_memorypool_chunk_meta_set_idx(
        pool->active_chunk,
        pool->element_size,
        pool->active_chunk->next,
        previous_next);
    pool->active_chunk->free_blocks += 1;

    return true;
}
