/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_ARRAY_H
#define RF_ARRAY_H

#include <rflib/defs/retcodes.h>
#include <rflib/defs/inline.h>
#include <rflib/utils/memory.h>
#include <rflib/string/rf_str_common.h>

#include <string.h>
#include <stdio.h>

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
 *               Be very careful so that the array IS NOT
 *               const. If it is the conversion  will be silently accepted
 *               by gcc but can cause a lot of problems. (undefined behaviour)
 */
#define RF_ARRAY_SHALLOW_INIT(other_)               \
    {.buff = (char*)other_,                         \
        .buff_allocated = false,                    \
        .size = sizeof(other_)}

/**
 * Initialize an RFarray in the heap
 *
 * @param arr_             The array to initialize
 * @param elements_num_    The size of the array in elements
 * @param type_            The type of elements to store
 *
 * @return      True for succes and false for no memory failure.
 */
#define rf_array_init(arr_, elements_num_, type_)             \
    rf_array_init_(arr_, sizeof(type_), elements_num_)

i_INLINE_DECL bool rf_array_init_(struct RFarray *a,
                                  size_t element_size,
                                  size_t size)
{
    a->size = size * element_size;
    RF_MALLOC(a->buff, a->size, return false);
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
        } else {                                                      \
            getvar_ = *((type_*)i_temp_val_);                         \
        }                                                             \
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
        void *i_temp_val_ = rf_array_ptrto_index_(arr_, ind_, sizeof(type_)); \
        if (!i_temp_val_) {                                             \
            fail_;                                                      \
        } else {                                                        \
            *((type_*)i_temp_val_) = val_;                              \
        }                                                               \
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
        } else {                                                \
            ((type_*)(arr_)->buff)[ind_] = val_;                \
        }                                                       \
    } while(0)

i_INLINE_DECL void *rf_array_ptrto_index_(struct RFarray *a,
                                          size_t i,
                                          size_t typesize)
{
    size_t index = i * typesize;
    if (i_RF_ARRAY_IOB(a, i, typesize)) {
        if (a->buff_allocated) {
            RF_REALLOC(a->buff, char, a->size * 2, return NULL);
        } else {
            char *old_buffer = a->buff;
            RF_MALLOC(a->buff, a->size * 2, return NULL);
            memcpy(a->buff, old_buffer, a->size);
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
