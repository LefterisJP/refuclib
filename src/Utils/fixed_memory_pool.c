#include <Utils/fixed_memory_pool.h>

#include <Utils/memory.h>

#include <math.h>

/* this meta data is stored in all of the unused blocks of the pool */
struct rf_fixed_memorypool_meta {
    //! index number of the next memory block
    uint32_t idx;
};
#define POOL_META_SIZE sizeof(struct rf_fixed_memorypool_meta)

bool rf_fixed_memorypool_init(struct rf_fixed_memorypool *pool,
                              size_t element_size,
                              size_t initial_capacity)
{
    unsigned int i;

    pool->element_size = element_size;
    if (element_size < POOL_META_SIZE) {
        pool->element_size = POOL_META_SIZE;
        RF_WARNING("A fixed memory pool element_size should have at least %zu "
                   "size in bytes. Wasting %zu bytes per element",
                   POOL_META_SIZE,
                   POOL_META_SIZE - element_size);
    }

    pool->buffer_size = element_size * initial_capacity;
    RF_MALLOC(pool->blocks, pool->buffer_size , return false);
    pool->next = 0;

    return true;
}

struct rf_fixed_memorypool *rf_fixed_memorypool_create(size_t element_size,
                                                       size_t initial_capacity)
{
    struct rf_fixed_memorypool *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);

    if (!rf_fixed_memorypool_init(ret, element_size, initial_capacity)) {
        free(ret);
        return NULL;
    }

    return ret;
}

void rf_fixed_memorypool_deinit(struct rf_fixed_memorypool *pool)
{
    free(pool->blocks);
}

void rf_fixed_memorypool_destroy(struct rf_fixed_memorypool *pool)
{
    rf_fixed_memorypool_deinit(pool);
    free(pool);
}

static inline void* rf_fixed_memorypool_addr_from_index(
    struct rf_fixed_memorypool *pool,
    uint32_t idx)
{
    return pool->blocks + (idx * pool->element_size);
}

static inline void rf_fixed_memorypool_calculate_next_idx(
    struct rf_fixed_memorypool *pool,
    uint32_t idx)
{
    struct rf_fixed_memorypool_meta *meta;
    meta = rf_fixed_memorypool_addr_from_index(pool, idx);
    pool->next = meta->idx;
}

static inline void rf_fixed_memorypool_meta_set_idx(
    struct rf_fixed_memorypool *pool,
    uint32_t block_idx,
    uint32_t next_idx)
{
    struct rf_fixed_memorypool_meta *meta;
    meta = rf_fixed_memorypool_addr_from_index(pool, block_idx);
    meta->idx = next_idx;
}

static inline uint32_t rf_fixed_memorypool_index_from_addr(
    struct rf_fixed_memorypool *pool,
    uint8_t *p)
{
    return (p - pool->blocks) / pool->element_size;
}

void *rf_fixed_memorypool_alloc_element(struct rf_fixed_memorypool *pool)
{
    struct rf_fixed_memory_meta *meta;
    uint32_t idx;
    void *allocated_element;

    /* if we are running out of space reallocate all blocks */
    if ((pool->next + 1) * pool->element_size >= pool->buffer_size) {
        pool->buffer_size *= 1.75;
        RF_REALLOC(pool->blocks, uint8_t, pool->buffer_size, goto err);
    }

    /* set the index of the next block since we will be neededing it */
    rf_fixed_memorypool_meta_set_idx(pool, pool->next, pool->next + 1);

    /* get this block's element to return and also set pool's next index */
    allocated_element = rf_fixed_memorypool_addr_from_index(pool, pool->next);
    rf_fixed_memorypool_calculate_next_idx(pool, pool->next);

    return allocated_element;

  err:
    RF_ERROR("Failed to allocate a memory pool element");
    return NULL;
}

void rf_fixed_memorypool_free_element(struct rf_fixed_memorypool *pool,
                                      void *element)
{
    uint32_t previous_next = pool->next;
    /* get index of freed element */
    uint32_t idx = rf_fixed_memorypool_index_from_addr(pool, element);
    /*
     * pool->next should point to it and also set the (now) unused block's index
     * to point to the previous next thus adding to the linked list
     */
    pool->next = idx;
    rf_fixed_memorypool_meta_set_idx(pool, pool->next, previous_next);
}
