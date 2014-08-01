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

#define RF_BUFFER_SHALLOW_INIT(buff_, size_) \
    {.buff = (char*)buff_, .size = size_, .index = 0 }

i_INLINE_DECL bool rf_buffer_init(struct RFbuffer* b, size_t size)
{
    b->size = size;
    b->index = 0;
    RF_MALLOC(b->buff, size, false);
    return true;
}

i_INLINE_DECL void rf_buffer_deinit(struct RFbuffer* b)
{
    free(b->buff);
}

i_INLINE_DECL char* rf_buffer_ptr(struct RFbuffer* b)
{
    return b->buff + b->index;
}

i_INLINE_DECL bool rf_buffer_increase(struct RFbuffer* b, size_t size)
{
    if(size <= b->index + b->size)
    {
        return true;
    }

    RF_REALLOC(b->buff, char, size, false);
    b->size = size;
    return true;
}

i_INLINE_DECL bool rf_buffer_copy(struct RFbuffer* b, const char* src, size_t len)
{
    if(!rf_buffer_increase(b, len))
    {
        return false;
    }

    memcpy(rf_buffer_ptr(b), src, len);
    return true;
}

i_INLINE_DECL size_t rf_buffer_size(struct RFbuffer* b)
{
    return b->size - b->index;
}


i_INLINE_DECL void rf_buffer_set_index(struct RFbuffer* b, unsigned int index)
{
    b->index = index;
}

i_INLINE_DECL unsigned int rf_buffer_index(struct RFbuffer* b)
{
    return b->index;
}

#define rf_buffer_size_u32(i_BUFF_) \
    (rf_buffer_size(i_BUFF_) / sizeof(uint32_t))

#define rf_buffer_ptr_u32(i_BUFF_, i_IND_)       \
    ((uint32_t*)rf_buffer_ptr(i_BUFF_))[i_IND_]

#define rf_buffer_atindex_u32(i_BUFF_, i_IND_)       \
    ((uint32_t*)(i_BUFF_)->buff)[i_IND_]

#define rf_buffer_increase_u32(i_BUFF_, i_SIZE_) \
    rf_buffer_increase(i_BUFF_, (i_SIZE_) * sizeof(uint32_t))


#endif
