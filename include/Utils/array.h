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
    size_t size; /*!< Size of the array in BYTES */
    bool buff_allocated;
    char *buff;
};

/*
 * Returns true if an index is out of bounds for the array
 * of a given element size
 */
#define i_RF_ARRAY_IOB(arr_, ind_, elemsize_)   \
    (((ind_) * (elemsize_)) >= (arr_)->size)


/**
 * Initialize a temporary RFarray only for use on this stack
 * frame and below
 *
 * @param arr_     The array to initialzie
 * @param type_    The type of elements that will reside in the array
 * @param size_    The number of elements of @c type_ to reside in the array
 */
#define RF_ARRAY_TEMP_INIT(arr_, type_, size_)      \
    type_ i_temporary_buffer_[size_];               \
    do {                                            \
        (arr_)->buff = (char*)i_temporary_buffer_;  \
        (arr_)->buff_allocated = false;             \
        (arr_)->size = size_ * sizeof(type_);       \
    } while(0)

/**
 * Initialize an RFarray by pointing it to an already
 * existing fixed size array on the stack.
 *
 * @param other_ The existing fixed size array to point to
 * @param type_  The type of the array
 */
#define RF_ARRAY_SHALLOW_INIT(other_, type_)        \
    {.buff = (char*)other_,                         \
        .buff_allocated = false,                    \
        .size = sizeof(other_)}

/**
 * Initialize an RFarray in the heap
 *
 * @param a     The array to initialize
 * @param size  The size of the array in bytes
 *
 * @return      True for succes and false for no memory failure.
 */
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


/**
 * Access an array element in an unsafe way.
 * Interpret the array as containing elements of
 * type_. If index is out of bounds, behaviour is undefined.
 */
#define rf_array_at_unsafe(arr_, ind_, type_)   \
    ((type_*)((arr_)->buff))[ind_]

/**
 * Access an array element with bounds checking
 *
 * @param arr_        The array
 * @param ind_        The index in elements of @c type_ to access
 * @param type_       The type of elements the array holds
 * @param getvar_     The variable in which to get the element's value.
 *                    Not a pointer to the variable.
 * @param fail_       Action to do in case of failure
 */
#define rf_array_get(arr_, ind_, type_, getvar_, fail_)               \
    do {                                                              \
        void *i_temp_val_ = rf_array_at_(arr_, ind_, sizeof(type_));  \
        if (!i_temp_val_) {                                           \
            fail_;                                                    \
        }                                                             \
        getvar_ = *((type_*)i_temp_val_);                             \
    } while (0)


/**
 * Get size of array in elements of @c type_
 */
#define rf_array_size(arr_, type_)              \
    rf_array_size_(arr) / sizeof(type_)

/**
 * Sets index of an array. If the index is out of bounds
 * it will reallocate the array so that it has enough space.
 * If it was pointing to a fixed size array then, a new buffer
 * will be allocated on the heap.
 *
 * @param arr_         The array
 * @param ind_         The index to the array in elements of @c type_
 * @param type_        The type of elements that the array holds
 * @param val_         The value to set
 * @param fail_        Action to do in case of failure (no memory)
 */
#define rf_array_set(arr_, ind_, type_, val_, fail_)                    \
    do {                                                                \
        void *i_temp_val_ = rf_array_ptrto_index_(arr_, i, sizeof(type_)); \
        if (!i_temp_val_) {                                             \
            fail_;                                                      \
        }                                                               \
        *((type_*)i_temp_val_) = val_;                                  \
    } while(0)

/**
 * Sets index of an array. If the index is out of bounds
 * you can provide an action to follow with @c fail_
 *
 * @param arr_         The array
 * @param ind_         The index to the array in elements of @c type_
 * @param type_        The type of elements that the array holds
 * @param val_         The value to set
 * @param fail_        Action to do in case of failure (index out of bounds)
 */
#define rf_array_set_norealloc(arr_, ind_, type_, val_, fail_)  \
    do {                                                        \
        if (i_RF_ARRAY_IOB(arr_, ind_, sizeof(type_))) {        \
            fail_;                                              \
        }                                                       \
        ((type_*)(arr_)->buff)[ind_] = val_;                    \
    } while(0)

i_INLINE_DECL void *rf_array_ptrto_index_(struct RFarray *a,
                                          size_t i,
                                          size_t typesize)
{
    size_t index = i * typesize;
    if (i_RF_ARRAY_IOB(a, i, typesize)) {
        if (a->buff_allocated) {
            RF_REALLOC(a->buff, char, a->size * 2, NULL);
        } else {
            RF_MALLOC(a->buff, a->size * 2, NULL);
            a->buff_allocated = true;
        }
        a->size *= 2;
    }

    return a->buff + index;
}

i_INLINE_DECL void *rf_array_at_(struct RFarray *a, size_t i, size_t typesize)
{
    size_t index = i * typesize;
    if (i_RF_ARRAY_IOB(a, i, typesize)) {
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
