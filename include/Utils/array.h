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
 * RFarray is a way to provide a checkable array to a function. It is a more
 * lightweight form of @ref RFbuffer in the sense that there is no index inside
 * the buffer to point where a specific data section is located.
 *
 * Also in contrast to RFbuffer the array is fixed and can't be reallocated.
*/

#ifndef RF_ARRAY_H
#define RF_ARRAY_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for bool
#include <Definitions/inline.h> //for inline
#include <Utils/memory.h> //for alloc realloc
/*------------- libc inclusion --------------*/
#include <string.h> //for memcpy
/*------------- End of includes -------------*/


struct RFarray {
    size_t size;
    bool buff_allocated;
    char *buff;
};

/*
 * Initialize a temporary RFarray only for use on this stack
 * frame and below
 */
#define RF_ARRAY_TEMP_INIT(arr_, type_, size_)      \
    type_ i_temporary_buffer_[size_];               \
    do {                                            \
        arr_ = {.buff = (char*)i_temporary_buffer_, \
                .buff_allocated = false,            \
                .size = size_};                     \
    } while(0)

i_INLINE_DECL bool rf_array_init(struct RFarray *a, size_t size)
{
    RF_MALLOC(a->buff, size, false);
    a->size = size;
    a->buff_allocated = true;

    return true;
}

i_INLINE_DECL void rf_array_deinit(struct RFarray *a)
{
    if (a->buff_allocated) {
        free(a->buff);
    }
}

//TODO: somehow check for bounds here
#define rf_array_at(arr_, ind_, type_)          \
    rf_array_at_(arr_, ind_, sizeof(type_))

#define rf_array_size(arr_, type_)              \
    rf_array_size_(arr) / sizeof(type_)


i_INLINE_DECL void *rf_array_at_(struct RFarray *a, size_t i, size_t typesize)
{
    size_t index = i * typesize;
    if (index > a->size - 1) {
        //index out of bounds
        return NULL;
    }
    
    return a->buff + index;
}

i_INLINE_DECL size_t rf_array_size_(struct RFarray *a)
{
    return a->size;
}


#endif
