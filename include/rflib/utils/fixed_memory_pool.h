/**
 * An implementation of a fixed size memory pool.
 * Algorithm inspired by the paper: Fast Efficient Fixed-Size Memory Pool
 * by Ben Kenwright
 */
#ifndef RF_FIXED_MEMORY_POOL_H
#define RF_FIXED_MEMORY_POOL_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

struct rf_fixed_memorypool_chunk {
    uint8_t *blocks;
    uint32_t next;
    uint32_t free_blocks;
};

struct rf_fixed_memorypool {
    size_t element_size;
    size_t chunk_size;
    size_t chunks_num;
    struct rf_fixed_memorypool_chunk *active_chunk;
    struct rf_fixed_memorypool_chunk **chunks;
};


bool rf_fixed_memorypool_init(struct rf_fixed_memorypool *pool,
                              size_t element_size,
                              size_t chunk_size);

struct rf_fixed_memorypool *rf_fixed_memorypool_create(size_t element_size,
                                                       size_t chunk_size);

void rf_fixed_memorypool_deinit(struct rf_fixed_memorypool *pool);
void rf_fixed_memorypool_destroy(struct rf_fixed_memorypool *pool);

void *rf_fixed_memorypool_alloc_element(struct rf_fixed_memorypool *pool);
bool rf_fixed_memorypool_free_element(struct rf_fixed_memorypool *pool,
                                      void *element);
#endif
