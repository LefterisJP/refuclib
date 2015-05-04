/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */

/* @omit start */

/*
 * This file is a TEMPLATE. It does not actually get compiled at all
 * anywhere inside the library. Depending on the options you provide
 * in scons though various specializations of this file will get 
 * generated during building the library
 */

/* @omit end */

#ifndef RF_BINARYARRAY_DECL_H
#define RF_BINARYARRAY_DECL_H

/**
** @internal
** @author Lefteris
** @date 22/02/2012
** @endinternal
** @brief An array that stores only binary values inside
**
** This is a special array which only holds binary values
** 0 or 1. Instead of using a byte for each value this data
** structure saves each value in an individual bit.
**
** Functions to set/get values and also to reallocate the array
** are provided
*/
typedef struct RFbinary_array
{
    //! The size of the array in values (bits)
    uint32_t size;
    //! The data
    char* data;
}RFbinary_array;

#endif//include guards end
