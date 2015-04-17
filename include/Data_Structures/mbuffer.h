#ifndef RF_DATA_STRUCTURES_MBUFFER_H
#define RF_DATA_STRUCTURES_MBUFFER_H

#include <stdbool.h>
#include <stddef.h>

struct RFmbuffer_block;
struct RFmbuffer_stack;

/**
 * A buffer of multiple blocks. When you allocate something
 * that does not fit in one block another one of double size is allocated.
 * This avoid pointer invalidation due to reallocing that can happen with a 
 * single block buffer
 */
struct RFmbuffer {
    struct RFmbuffer_block **blocks;
    size_t blocks_num;
    size_t curr_block_idx;
    struct RFmbuffer_stack *stack;
};

bool rf_mbuffer_init(struct RFmbuffer *b, size_t initial_buffer_size);
void rf_mbuffer_deinit(struct RFmbuffer *b);

/**
 * Request allocation of @a size bytes from an mbuffer.
 *
 * If the mbuffer does not have enough size on its current block a new block
 * of double the size will be allocated.
 *
 * @param b         The buffer from which to allocate
 * @param size      The size in bytes to allocate
 *
 * @return          A pointer to the allocated memory. Once @ref rf_mbuffer_pop()
 *                  is called this pointer will be invalidated.
 */
void *rf_mbuffer_alloc(struct RFmbuffer *b, size_t size);
/**
 * Remember the buffer position at call
 *
 * Call in combination with @ref rf_mbuffer_pop() in order to safely allocate
 * memory from an mbuffer.
 *
 * @param b         The buffer whose position to remember
 */
void rf_mbuffer_push(struct RFmbuffer *b);
/**
 * Pop the last remembered buffer position
 *
 * Call in combination with @ref rf_mbuffer_push() in order to safely allocate
 * memory from an mbuffer.
 *
 * @param b         The buffer whose position to pop
 */
void rf_mbuffer_pop(struct RFmbuffer *b);

#endif
