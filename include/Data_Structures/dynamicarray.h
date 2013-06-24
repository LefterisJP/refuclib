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
** --Data_Structures/dynamicarray.h
** This header declares functions to operate on RF_DynamicArray
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Data_Structures/dynamicarray_decl.h>
#include <Data_Structures/dynamicarray.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_DYNAMICARRAY_H
#define RF_DYNAMICARRAY_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
 ** @memberof RF_DynamicArray
 ** @brief Allocates and returns a dynamic array
 **
 ** @return The allocated array
 **/
i_DECLIMEX_ RF_DynamicArray*  rfDynamicArray_Create(
    uint32_t numElements
    /* @omit start */
    , uint32_t elementSize,
    void (*ptr2Destroy)(void*),
    void (*ptr2Copy)(void*,void*)
    /* @omit end */
);
 

/**
 ** @memberof RF_DynamicArray
 ** @brief Initializes a dynamic array
 **
 ** @param array The array to initialize
 ** @return true in case of succesfull initialization and false otherwise
 **
 **/
i_DECLIMEX_ char  rfDynamicArray_Init(
    RF_DynamicArray* array,
    uint32_t numElements
    /* @omit start */
    , uint32_t elSize,
    void (*ptr2Destroy)(void*),
    void (*ptr2Copy)(void*,void*)
    /* @omit end */
);

/**
 ** @memberof RF_DynamicArray
 ** @brief Copies RF_DynamicArray @c src into RF_DynamicArray @c dst
 **
 ** @param dst The new array copy to create
 ** @param src The array to copy from
 **
 **/
i_DECLIMEX_ void rfDynamicArray_Copy_IN(RF_DynamicArray* dst,
                                        RF_DynamicArray* src);
/**
 ** @memberof RF_DynamicArray
 ** @brief Creates and returns an allocated copy of the given 
 ** RF_DynamicArray
 **
 ** @param src The array to copy from
 ** @return An allocated RF_DynamicArray copy of the given array
 **
 **/
i_DECLIMEX_ RF_DynamicArray* rfDynamicArray_Copy_OUT(RF_DynamicArray* src);

/**
 ** @memberof RF_DynamicArray
 ** @brief Destroys a array of objects releasing their memory and calling
 ** the destructing function if it exists
 **
 ** If for some reason you do not want to call the destruction
 ** function of the given objects at array destruction use
 ** @ref rfDynamicArray_Destroy_nofree()
 ** @param l The array to destroy
 **
 **/
i_DECLIMEX_ void rfDynamicArray_Destroy(RF_DynamicArray* l);
/**
 ** @memberof RF_DynamicArray
 ** @brief Destroys a array of objects without calling individual object 
 ** destructing function
 **
 ** For a function that deletes the array and calls the appropriate
 ** destruction function for each individual object refer to
 ** @ref rfDynamicArray_Destroy()
 ** @param l The array to destroy
 **
 **/
i_DECLIMEX_ void rfDynamicArray_Destroy_nofree(RF_DynamicArray* l);
/**
 ** @memberof RF_DynamicArray
 ** @brief Deinitializes a array of objects releasing their memory and 
 ** calling the destructing function if it exists
 **
 ** If for some reason you do not want to call the destruction
 ** function of the given objects at array deinitialization use
 ** @ref rfDynamicArray_Deinit_nofree()
 ** @param l The array to deinitialize
 **
 **/
i_DECLIMEX_ void rfDynamicArray_Deinit(RF_DynamicArray* l);
/**
 ** @memberof RF_DynamicArray
 ** @brief Deinitializes a array of objects without calling individual object destructing function
 **
 ** For a function that deinitializes the array and calls the appropriate
 ** destruction function for each individual object refer to
 ** @ref rfDynamicArray_Deinit()
 ** @param l The array to deinitialize
 **
 **/
i_DECLIMEX_ void rfDynamicArray_Deinit_nofree(RF_DynamicArray* l);

/**
 ** @memberof RF_DynamicArray
 ** @brief Adds a new object to the array
 **
 ** The @c object is added in the array right after the last element and
 **  if it does not fit in the buffer, a reallocation happens.
 ** If the @c object is Plain old Data then it's simply appended to the 
 ** array while if it's any other type created by the template its copy
 ** function is called.
 ** @param l The array in which to add the object
 ** @param object The object to add. For plain old data types simply add
 ** it here. For objects make sure to pass a pointer to the object
 **
 ** @return Returns @c RF_SUCCESS in all cases except for when there is 
 ** insufficient memory in which case the function will return @c 
 ** RE_REALLOC_FAILURE
 **/
/* @mutate void* TYPEPTR_OBJ_ONLY */
i_DECLIMEX_ int32_t rfDynamicArray_Add(RF_DynamicArray* l,void* object);


/**
 ** @memberof RF_DynamicArray
 ** @brief Retrieves an object from the array
 **
 ** The object will be placed in the passed parameter @c ret. It must
 ** be a reference of the appropriate type.
 ** @param l The array from which to retrieve
 ** @param i The index of the element to retrieve
 ** @param ret A pointer to the object you need to populate with the
 ** retrieved data
 ** @return @c RF_SUCCESS if the object was retrieved succesfully and 
 ** @c RE_INDEX_OUT_OF_BOUNDS if the index is out of bounds
 **
 **/
i_DECLIMEX_ int32_t rfDynamicArray_Get_IN(
    RF_DynamicArray* l,
    uint32_t i,
/* @mutate void* TYPEPTR */
    void* ret
);

/**
 ** @memberof RF_DynamicArray
 ** @brief Returns an object from the array
 **
 ** @param l The array from which to retrieve
 ** @param i The index of the element to retrieve
 ** @param code Contains @c RF_SUCCESS if all went well with the object
 ** retrieval and @c RE_INDEX_OUT_OF_BOUNDS if there was an index out of bounds
 ** @return If @c code contains @c RF_SUCCESS then it returns the 
 ** value of the array at @c i. If there is a failure this will return @c 0
 **/
/* @mutate void* TYPEPTR_OBJ_ONLY */
i_DECLIMEX_ void* rfDynamicArray_Get_OUT(
    RF_DynamicArray* l,
    uint32_t i,
    int32_t* code
);

/**
 ** @memberof RF_DynamicArray
 ** @brief Sets a copy of the given object to the specified array index
 **
 ** @c object is copied into the @c i index of the array. If the object is
 ** plain old data type then it is simply placed there while if it is an 
 ** object type its copy function is called.
 ** @param l The array for which to set the object
 ** @param i The index of the object to set
 ** @param e A pointer to the object to add.
 ** previous object at @c i or @c false if you don't
 ** @return True if the element got set succesfully and false if index @c i
 ** is over the capacity of the array
 **
 **/
i_DECLIMEX_ char rfDynamicArray_Set(
    RF_DynamicArray* l,
    uint32_t i,
/* @mutate void* TYPEPTR_OBJ_ONLY */
    void* e
);

/**
 ** @memberof RF_DynamicArray
 ** @brief Removes an object from the array.
 **
 ** All objects get moved to make up for the freed up space and if there
 ** is too much space left the array gets reallocated downwards.
 ** Also the destructor of the object at index @c i is called if given.
 ** For a function
 ** that removes the object without calling the destructor take a look at
 ** @ref rfDynamicArray_Remove_nofree()
 ** @param l The array from which to remove
 ** @param i The index of the element to remove
 ** @return Returns RF_SUCCESS if the object at @c i was succesfully removed
 ** and @c RE_INDEX_OUT_OF_BOUNDS if there is no element at @c i. Additionally
 ** if during reallocation downwards there is an error this function can 
 ** return @c RE_REALLOC_FAILURE
 **/
i_DECLIMEX_ int32_t rfDynamicArray_Remove(RF_DynamicArray* l, uint32_t i);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif //end of include guards
