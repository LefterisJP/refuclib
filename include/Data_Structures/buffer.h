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
 *
 * RFbuffer is a simple buffer with a given size and an index. Functions that
 * use it should remember how many elements they have written in the buffer.
 * The index points to the position in the buffer the last function
 * started using.
 *
 * When a function is done with the buffer it returns the index to where it
 * was before so that all functions withing a single thread can use the same
 * buffer.
 *
 * Functions can interpret the contents of the buffer as any type they need
 * via the use of the provided macros
*/

#ifndef RF_BUFFER_H
#define RF_BUFFER_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for bool
#include <Definitions/inline.h> //for inline
#include <Utils/memory.h> //for alloc realloc
/*------------- libc inclusion --------------*/
#include <string.h> //for memcpy
/*------------- End of includes -------------*/

struct RFbuffer;
typedef bool (*rf_buffer_realloc_cb)(struct RFbuffer*);
struct RFbuffer {
    char* buff;
    size_t size;
    unsigned int index;
    rf_buffer_realloc_cb realloc_cb;
};

/*
 * RFbuffer API. These macros allow for buffer
 * manipulation of different sizes
 */

#define RF_BUFFER_SHALLOW_INIT(buff_, size_) \
    {.buff = (char*)buff_, .size = size_, .index = 0, .realloc_cb = NULL }

/*
 * Returns remaining size in @c type_ of @c I_BUFF
 */
#define rf_buffer_remaining_size(i_BUFF_, type_) \
    (rf_buffer_remaining_size_(i_BUFF_) / sizeof(type_))

/*
 * Returns a pointer to the current buffer position
 * having it interpreted as a buffer of @c type_
 */
#define rf_buffer_current_ptr(i_BUFF_, type_)   \
    ((type_*)rf_buffer_current_ptr_(i_BUFF_))

/*
 * Return/set the value of element at @c i_IND of
 * the buffer, starting from the current pointer and
 * having its contents interpreted as  @c type_
 */
#define rf_buffer_from_current_at(i_BUFF_, i_IND_, type_) \
    ((type_*)rf_buffer_current_ptr_(i_BUFF_))[i_IND_]

/*
 * Return/set the value of element at @c i_IND of
 * the buffer, starting from the beginning of the buffer
 * having its contents interpreted as  @c type_
 */
#define rf_buffer_from_start_at(i_BUFF_, i_IND_, type_) \
    ((type_*)(i_BUFF_)->buff)[i_IND_]

/*
 * Move buffer index by a number of elements
 * with its contents interpreted as @c type_
 *
 * @warning Must have checked before calling that there is no overflow
 */
#define rf_buffer_move_index(i_BUFF_, i_ELEMENTS, type_)  \
    (i_BUFF_)->index += (i_ELEMENTS) * sizeof(type_)

/*
 * Set buffer index at a specific element
 * with the contents interpreted as @c type_
 */
#define rf_buffer_set_index(i_BUFF_, i_IND_, type_) \
    rf_buffer_set_index_(i_BUFF_, (i_IND_) * sizeof(type_))


i_INLINE_DECL unsigned int rf_buffer_index(struct RFbuffer* b)
{
    return b->index;
}

bool rf_buffer_init(struct RFbuffer* b, size_t size, rf_buffer_realloc_cb cb);
i_INLINE_DECL void rf_buffer_deinit(struct RFbuffer* b)
{
    free(b->buff);
}

i_INLINE_DECL size_t rf_buffer_size(struct RFbuffer* b)
{
    return b->size;
}

i_INLINE_DECL uint64_t rf_buffer_get_offset(struct RFbuffer *b, char *byte)
{
    return byte - b->buff;
}

/**
 * Increase the buffer size and execute realloc callback if existing
 *
 * @param b            The buffer whose size to increase
 * @param added_size   The size in bytes to add. The actual new size will be
 *                     double the current size + added_size
 * @return             true in success and false in failure
 */
bool rf_buffer_increase_size(struct RFbuffer *b, size_t added_size);

/*
 * Buffer internal functions start - Do not use them
 */
i_INLINE_DECL void  *rf_buffer_current_ptr_(struct RFbuffer *b)
{
    return b->buff + b->index;
}

i_INLINE_DECL size_t rf_buffer_remaining_size_(struct RFbuffer *b)
{
    return b->size - b->index;
}

i_INLINE_DECL bool rf_buffer_assert_remaining_size(struct RFbuffer *b, size_t size)
{
    if (rf_buffer_remaining_size_(b) > size) {
        return true;
    }
    return rf_buffer_increase_size(b, size);
}

i_INLINE_DECL bool rf_buffer_assert_remaining_size_detect_realloc(struct RFbuffer *b,
                                                                  size_t size,
                                                                  bool *had_realloc)
{
    if (rf_buffer_remaining_size_(b) > size) {
        *had_realloc = false;
        return true;
    }
    *had_realloc = true;
    return rf_buffer_increase_size(b, size);
}

i_INLINE_DECL void rf_buffer_set_index_(struct RFbuffer* b, unsigned int index)
{
    b->index = index;
}

/*
 * Buffer internal functions end
 */

/**
 * Allocates a number of bytes from buffer. Reallocs if needed.
 */
i_INLINE_DECL void *rf_buffer_malloc(struct RFbuffer *b, size_t len_bytes)
{
    char *ret;
    if (!rf_buffer_assert_remaining_size(b, len_bytes)) {
        return NULL;
    }

    ret = rf_buffer_current_ptr(b, char);
    rf_buffer_move_index(b, len_bytes, char);
    return ret;
}

/**
 * Allocates a number of bytes from buffer. If realloc happens then it does
 * not actually "malloc" anything on the buffer but instead sets had_realloc to true
 * and returns NULL
 */
i_INLINE_DECL void *rf_buffer_malloc_or_detect_realloc(struct RFbuffer *b,
                                                       size_t len_bytes,
                                                       bool *had_realloc)
{
    char *ret;
    if (!rf_buffer_assert_remaining_size_detect_realloc(b, len_bytes, had_realloc)) {
        return NULL;
    }
    if (*had_realloc) {
        return NULL;
    }

    ret = rf_buffer_current_ptr(b, char);
    rf_buffer_move_index(b, len_bytes, char);
    return ret;
}
#endif
