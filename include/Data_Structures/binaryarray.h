/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
**
** --Data_Structures/binaryarray.h
**
** Declares functions that operate on RF_BinaryArray
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>//for fixed size data types
#include <Data_Structures/binaryarray_decl.h> //for RF_BinarryArray
#include <Data_Structures/binaryarray.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_BINARY_ARRAY_H
#define RF_BINARY_ARRAY_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/**
 ** @memberof RF_BinaryArray
 ** @brief Allocates and returns a binary array. All initial values are 0 (false)
 **
 ** @param size The size of the array in values
 ** @return Returns the initialized binary array
 **
 **/
i_DECLIMEX_ RF_BinaryArray* rfBinaryArray_Create(uint32_t size);
/**
 ** @memberof RF_BinaryArray
 ** @brief Initializes a binary array. All initial values are 0 (false)
 **
 ** @param arr The array to initialize
 ** @param size The size of the array in values
 ** @return Returns true in success and false for failure
 **
 **/
i_DECLIMEX_ char rfBinaryArray_Init(RF_BinaryArray* arr,uint32_t size);

/**
 ** @memberof RF_BinaryArray
 ** @brief Copies RF_BinarryArray @c src into RF_BinaryArray @c dst
 **
 ** @param dst The new binarry array copy to create
 ** @param src The binary array to copy from
 **
 **/
i_DECLIMEX_ void rfBinaryArray_Copy_IN(RF_BinaryArray* dst,RF_BinaryArray* src);
/**
 ** @memberof RF_BinaryArray
 ** @brief Creates and returns an allocated copy of the given RF_BinarryArray
 **
 ** @param src The Binary array to copy
 ** @return An allocated RF_BinaryArray copy of the given binary array
 **
 **/
i_DECLIMEX_ RF_BinaryArray* rfBinaryArray_Copy_OUT(RF_BinaryArray* src);

/**
 ** @memberof RF_BinaryArray
 ** @brief Destroys a binary array freeing its memory
 **
 ** Call this for arrays initialized with _Create
 ** @param a The binary array to destroy
 **
 **/
i_DECLIMEX_ void rfBinaryArray_Destroy(RF_BinaryArray* a);
/**
 ** @memberof RF_BinaryArray
 ** @brief Destroys a binary array but not freeing own memory
 **
 ** Call this for arrays initialized with _Init
 ** @param a The binary array to destroy
 **
 **/
i_DECLIMEX_ void rfBinaryArray_Deinit(RF_BinaryArray* a);

/**
 ** @memberof RF_BinaryArray
 ** @brief Gets a specific value of the array
 ** @param[in] a The binary array from where to get the value
 ** @param[in] i The index of the value to get
 ** @param[out] val Pass a reference to a char to get the value here
 ** @return Returns true if the value exists and false if the requested index is out of bounds
 **
 **/
i_DECLIMEX_ char rfBinaryArray_Get(RF_BinaryArray* a,uint32_t i,char* val);

/**
 ** @memberof RF_BinaryArray
 ** @brief Gets a specific value of the array, no checking.
 **
 ** This function gets a value without checking for array index out of bounds. If the index IS out of bounds the value can not be trusted and
 ** a segmentation fault can also happen. For a safer function look at #rfBinaryArray_Get that performs the check.
 ** @warning  NEVER use this function if you are not sure wether the index is out of bounds or not
 ** @param a The binary array from where to get the value
 ** @param i The index of the value to get
 ** @return Returns true if the value is set and false if it is not set
 **
 **/
i_DECLIMEX_ char rfBinaryArray_Get_NC(RF_BinaryArray* a,uint32_t i);

/**
 ** @memberof RF_BinaryArray
 ** @brief Sets a specific value of the binary array.
 ** @param a The binary array from where to get the value
 ** @param i The index of the value to set
 ** @param val The value to give. Either 1 or 0.
 ** @return Returns true if the value was succesfully set or false if the given index was out of bounds
 **
 **/
i_DECLIMEX_ char rfBinaryArray_Set(RF_BinaryArray* a,uint32_t i,char val);

/**
 ** @memberof RF_BinaryArray
 ** @brief Sets a specific value of the binary array, no checking.
 **
 ** This function sets a value without making an index out of bounds check. Use only if you are certain that the given index is not out of bounds!!!!
 ** @warning  NEVER use this function if you are not sure wether the index is out of bounds or not
 ** @param a The binary array from where to get the value
 ** @param i The index of the value to set
 ** @param val The value to give. Either 1 or 0.
 **
 **/
i_DECLIMEX_ void rfBinaryArray_Set_NC(RF_BinaryArray* a,uint32_t i,char val);

/**
 ** @memberof RF_BinaryArray
 ** @brief Increases or decreases the size of a binary array.
 ** @param a The binary array to reallocate
 ** @param newSize The new size in values that the array should have.
 ** @return True for succesfull reallocation and false otherwise.
 **
 **/
i_DECLIMEX_ char rfBinaryArray_Reallocate(RF_BinaryArray* a,uint32_t newSize);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end
