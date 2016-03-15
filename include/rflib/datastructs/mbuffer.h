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
 * single block buffer. A single block buffer implementation can be found
 * at @ref struct RFsbuffer
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
 * @return          A pointer to the allocated memory. Once @ref rf_mbuffer_pop()
 *                  is called this pointer will be invalidated.
 */
void *rf_mbuffer_alloc(struct RFmbuffer *b, size_t size);

/**
 * Extends the last buffer allocation by @a added_size bytes
 *
 * If the mbuffer does not have enough size on its current block this call will
 * faill
 *
 * @param b            The buffer from which to extend last allocation
 * @param added_size   The added size in bytes
 * @return             True if there was enough space in the current block and
 *                     we were succesfull and false otherwise
 */
bool rf_mbuffer_extend(struct RFmbuffer *b, size_t added_size);

/**
 * Shrinks the last buffer allocation by @a rem_size bytes
 *
 * @param b            The buffer from which to extend the last allocation
 * @param shrink_size  The size in bytes to shrink the last allocation by. If
 *                     more than the current allocated size then the whole block
 *                     is simply freed.
 */
void rf_mbuffer_shrink(struct RFmbuffer *b, size_t shrink_size);

/**
 * Retrieve the remaining size of the current memory block
 *
 * @param b         The buffer in question.
 * @return          the remaining size of the current memory block
 */
size_t rf_mbuffer_currblock_remsize(const struct RFmbuffer *b);

/**
 * Retrieve the pointer where next memory block would be allocated.
 *
 * Do not use unless you know what you are doing.
 * It's basically curr_block + curr_block->index
 *
 * @param b         The buffer in question.
 * @return          Pointer to where next allocation will start
 */
void *rf_mbuffer_currblock_currptr(const struct RFmbuffer *b);

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
 * Pop the last remembered buffer position and properly free memory
 *
 * Call in combination with @ref rf_mbuffer_push() in order to safely allocate
 * memory from an mbuffer.
 *
 * @param b         The buffer whose position to pop
 */
void rf_mbuffer_pop(struct RFmbuffer *b);

#endif
