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
struct RFbuffer {
    char* buff;
    size_t size;
    unsigned int index;
};

/*
 * RFbuffer API. These macros allow for buffer
 * manipulation of different sizes
 */

#define RF_BUFFER_SHALLOW_INIT(buff_, size_) \
    {.buff = (char*)buff_, .size = size_, .index = 0 }

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
 */
#define rf_buffer_move_index(i_BUFF_, i_ELEMENTS, type_)  \
    (i_BUFF_)->index += (i_ELEMENTS) * sizeof(type_)

/*
 * Set buffer index at a specific element
 * with the contents interpreted as @c type_
 */
#define rf_buffer_set_index(i_BUFF_, i_IND_, type_) \
    rf_buffer_set_index_(i_BUFF_, (i_IND_) * sizeof(type_))

/*
 * Increase the size of the buffer by @c type_ number of
 * @c i_ELEMENTS. If the remaining size fits it, then do
 * nothing.
 */
#define rf_buffer_increase_size(i_BUFF_, i_ELEMENTS_, type_)      \
    rf_buffer_increase_size_(i_BUFF_, (i_ELEMENTS_) * sizeof(type_))


i_INLINE_DECL bool rf_buffer_init(struct RFbuffer* b, size_t size)
{
    b->size = size;
    b->index = 0;
    RF_CALLOC(b->buff, size, 1, return false);
    return true;
}

i_INLINE_DECL void rf_buffer_deinit(struct RFbuffer* b)
{
    free(b->buff);
}

i_INLINE_DECL unsigned int rf_buffer_index(struct RFbuffer* b)
{
    return b->index;
}

i_INLINE_DECL size_t rf_buffer_size(struct RFbuffer* b)
{
    return b->size;
}

/*
 * Buffer internal functions start - Do not use them
 */
i_INLINE_DECL void  *rf_buffer_current_ptr_(struct RFbuffer *b)
{
    return b->buff + b->index;
}

i_INLINE_DECL size_t rf_buffer_remaining_size_(struct RFbuffer* b)
{
    return b->size - b->index;
}

i_INLINE_DECL bool rf_buffer_increase_size_(struct RFbuffer* b,
                                            size_t added_size)
{

    if (rf_buffer_remaining_size_(b) > added_size) {
        return true;
    }

    RF_REALLOC(b->buff, char, b->size + added_size, return false);
    b->size += added_size;
    return true;
}

i_INLINE_DECL void rf_buffer_set_index_(struct RFbuffer* b, unsigned int index)
{
    b->index = index;
}

/*
 * Buffer internal functions end
 */

/*
 * Copies @c len bytes from @c src into the current position
 * of buffer. If there is not enough size in the buffer, it
 * reallocates
 */
i_INLINE_DECL bool rf_buffer_copy_at_current(struct RFbuffer* b,
                                             const char* src,
                                             size_t len)
{
    if(!rf_buffer_increase_size(b, len, char))
    {
        return false;
    }

    memcpy(rf_buffer_current_ptr(b, char), src, len);
    return true;
}
#endif
