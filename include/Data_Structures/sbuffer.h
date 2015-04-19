/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
 */

#ifndef RF_DATA_STRUCTURES_SBUFFER_H
#define RF_DATA_STRUCTURES_SBUFFER_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for bool
#include <Utils/memory.h> //for alloc realloc
/*------------- libc inclusion --------------*/
#include <string.h> //for memcpy
/*------------- End of includes -------------*/

struct RFsbuffer;
struct RFsbuffer_stack;
typedef bool (*rf_sbuffer_realloc_cb)(struct RFsbuffer*);
/**
 * A buffer of a single block. If you allocate beyond the current size
 * a realloc will happen. This will invalidate any pointers that were
 * pointing to the buffer. If this is not a concern then this is a good
 * buffer to use. If it is refer to @ref struct RFmbuffer
 */
struct RFsbuffer {
    char *buff;
    size_t size;
    size_t index;
    struct RFsbuffer_stack *stack;
    rf_sbuffer_realloc_cb realloc_cb;
};

bool rf_sbuffer_init(struct RFsbuffer *b, size_t initial_size, rf_sbuffer_realloc_cb cb);
void rf_sbuffer_deinit(struct RFsbuffer *b);


/**
 * Allocate @a size bytes from the buffer and return pointer to their beginning
 *
 * @param b            The buffer from which to allocate
 * @param added_size   The size in bytes to allocate
 * @return             A pointer to the beginning of the allocated memory
 */
void *rf_sbuffer_alloc(struct RFsbuffer *b, size_t size);

/**
 * Extends the last buffer allocation by @a added_size
 *
 * @param b            The buffer whose size to extend
 * @param added_size   The size in bytes to add to the last allocation
 * @return             A pointer the last allocated block, since a realloc may
 *                     happen after this function and we need to be able to
 *                     refresh the pointer.
 */
void *rf_sbuffer_extend(struct RFsbuffer *b, size_t added_size);

void rf_sbuffer_push(struct RFsbuffer *b);
void rf_sbuffer_pop(struct RFsbuffer *b);
#endif
