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
**/
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Data_Structures/dynamicarray_decl.h> 
#include <Data_Structures/dynamicarray.h>
//*---------------------Module related inclusion----------------------------------------
#include "common.ph"//for the common macro definitions
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <string.h> //for memcpy e.t.c.
//*----------------------------End of Includes------------------------------------------


//Allocates and returns an array of objects
RF_DynamicArray* rfDynamicArray_Create(
    uint32_t numElements
    /* @omit start */
    ,uint32_t elSize,
    void (*ptr2Destroy)(void*),
    void (*ptr2Copy)(void*,void*)
    /* @omit end */
)
{
    RF_DynamicArray* ret;
    /* @omit start */
    //if the given size is 0
    if(elSize == 0)
    {
        LOG_ERROR("Tried to create a list of objects which have zero size",RE_LIST_INIT_FAILURE);
        return 0;
    }
    /* @omit end */
    //allocate the new array
    RF_MALLOC(ret,sizeof(RF_DynamicArray));
    //allocate the data buffer
    /* @mutate elSize SIZE */
    ret->bufferCapacity = elSize*numElements;
    RF_MALLOC(ret->data,ret->bufferCapacity);
    ret->size = 0;
    /* @omit start */
    ret->elementSize = elSize;
    ret->ptr2Destroy = ptr2Destroy;
    ret->ptr2Copy = ptr2Copy;
    /* @omit end */
    return ret;
}
//Initialises an array of objects
char rfDynamicArray_Init(
    RF_DynamicArray* array,
    uint32_t numElements
    /* @omit start */
    ,uint32_t elSize,
    void (*ptr2Destroy)(void*),
    void (*ptr2Copy)(void*,void*)
    /* @omit end */
)
{
    /* @omit start */
    //if the given size is 0
    if(elSize == 0)
    {
        LOG_ERROR("Tried to initialize a list of objects which have zero size",RE_LIST_INIT_FAILURE);
        return false;
    }
    /* @omit end */

    //allocate the data buffer
    /* @mutate elSize SIZE */
    array->bufferCapacity = elSize * numElements;
    RF_MALLOC(array->data, array->bufferCapacity);
    array->size = 0;
    /* @omit start */
    array->elementSize = elSize;
    array->ptr2Destroy = ptr2Destroy;
    array->ptr2Copy = ptr2Copy;
    /* @omit end */
    return true;
}

// Copies RF_DynamicArray @c src into RF_DynamicArray @c dst
void rfDynamicArray_Copy_IN(RF_DynamicArray* dst,RF_DynamicArray* src)
{
    uint32_t i;
    dst->bufferCapacity = src->bufferCapacity;
    dst->size = src->size;
    /* @omit start */
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->elementSize = src->elementSize;
    dst->ptr2Copy = src->ptr2Copy;
    /* @omit end */
    //allocate the data buffer
    RF_MALLOC(dst->data,dst->bufferCapacity)
    //copy each element using the copy function
    for(i=0;i<dst->size;i++)
    {
        /* @mutate ptr2Copypp ASSIGN_NOSIZE */
        dst->ptr2Copy(dst->data+(i*dst->elementSize),src->data+(i*src->elementSize));
    }
    
}
// Creates and returns an allocated copy of the given RF_DynamicArray
RF_DynamicArray* rfDynamicArray_Copy_OUT(RF_DynamicArray* src)
{
    uint32_t i;
    RF_DynamicArray* dst;
    RF_MALLOC(dst,sizeof(RF_DynamicArray));
    dst->bufferCapacity = src->bufferCapacity;
    dst->size = src->size;
    /* @omit start */
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->elementSize = src->elementSize;
    dst->ptr2Copy = src->ptr2Copy;
    /* @omit end */
    //allocate the data buffer
    RF_MALLOC(dst->data,dst->bufferCapacity)

    //copy each element using the copy function
    for(i=0;i<dst->size;i++)
    {
        /* @mutate ptr2Copypp ASSIGN NOSIZE */
        dst->ptr2Copy(dst->data+(i*dst->elementSize),src->data+(i*src->elementSize));
    }

    return dst;
}

// Destroys an array of objects releasing their memory and calling the 
// destructing function if it exists
void rfDynamicArray_Destroy(RF_DynamicArray* l)
{
    uint32_t i;
    
    //delete the individual objects
    /* @omitcond POD */
    for(i = 0; i < l->size; i ++)
    {
        /* @mutate ptr2Destroyp DESTROY NOSIZE */
        l->ptr2Destroy(l->data+(i*l->elementSize));
    }
    /* @omit end */
    //finally just free the  whole data buffer and the list object
    free(l->data);
    free(l);
}

// Destroys an array of objects without calling individual object 
// destructing function
void rfDynamicArray_Destroy_nofree(RF_DynamicArray* l)
{
    //just free the  whole data buffer and the list object
    free(l->data);
    free(l);
}

// Destroys an array of objects but not the list memory itself releasing
// their memory and calling the destructing function if it exists
void rfDynamicArray_Deinit(RF_DynamicArray* l)
{
    uint32_t i;
    /* @omitcond POD */
    for(i = 0; i < l->size; i ++)
    {
        /* @mutate ptr2Destroyp DESTROY NOSIZE */
        l->ptr2Destroy(l->data+(i*l->elementSize));
    }
    /* @omit end */
    //finally just free the  whole data buffer
    free(l->data);
}
// Deinitializes an array of objects without calling individual object
//  destructing function
void rfDynamicArray_Deinit_nofree(RF_DynamicArray* l)
{
    //just free the  whole data buffer
    free(l->data);
}



// Adds a new object to the list by taking it from a pointer address.
// The object is added right after the last element and if it does not fit
// in the buffer a reallocation happens
/* @mutate void* TYPEPTR */
void rfDynamicArray_Add(RF_DynamicArray* l, void* object)
{
    //check if there is enough space in the list
    /* @mutate l->elementSize SIZE */
    if(l->size*l->elementSize >= l->bufferCapacity)
    {
        //if not reallocate to double capacity
        l->bufferCapacity*= RF_OPTION_DYNAMICARRAY_CAPACITY_M;
        /* @mutate void* TYPEPTR */
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in adding an object to an array of "
                      "Objects during an attempt to increase buffer"
                      " capacity. Aborting program", RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }//end of capacity increase case
    //copy the new element in to the data and increase the index
    /* @mutate l->elementSize SIZE */
    memcpy(l->data+(l->size*l->elementSize),object,l->elementSize);
    l->size++;
}

// Adds a a copy of an object to the array
/* @mutate void* TYPEPTR */
char rfDynamicArray_AddCopy(RF_DynamicArray* l, void* object)
{
    //check if there is enough space in the list
    /* @mutate l->elementSize SIZE */
    if(l->size*l->elementSize >= l->bufferCapacity)
    {
        //if not reallocate to double capacity
        l->bufferCapacity*= RF_OPTION_DYNAMICARRAY_CAPACITY_M;
        /* @mutate void* TYPEPTR */
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in adding an object to an array of "
                      "Objects during an attempt to increase buffer"
                      " capacity. Aborting program", RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }//end of capacity increase case
    //copy the new element in to the data and increase the index
    /* @mutate ptr2Copypp NOSIZE */
    l->ptr2Copy(l->data+(l->size*l->elementSize),object);
    l->size++;
    return true;
}

// Retrieves an object from the group and saves it in the given pointer.
// Pointer must be allocated
/* @mutate void* TYPEPTR */
int32_t rfDynamicArray_Get(RF_DynamicArray* l, uint32_t i, void* ret)
{
    //if the index goes over the buffer capacity
    if( i >= l->size)
    {
        LOG_ERROR("Attempted to retrieve an element from an array with an"
                  " index that is out of bounds", RE_INDEX_OUT_OF_BOUNDS);
        return RF_FAILURE;
    }
    /* @mutate l->elementSize SIZE */
    memcpy(ret, l->data+(i*l->elementSize), l->elementSize);
    return RF_SUCCESS;
}

// Sets an object of the list to the given pointer value. If there was 
// already an object in the previous location it is removed and its
// destructor is called.
char rfDynamicArray_Set(RF_DynamicArray* l,
                        uint32_t i,
                        /* @mutate void* TYPEPTR */
                        void* e)
{
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to set an element in an array with an index "
                  "out of bounds. If you want to add objects to the list"
                  "use the Add function.", RE_INDEX_OUT_OF_BOUNDS)
        return false;
    }
    /* @mutate ptr2Destroyp NOSIZE */
    l->ptr2Destroy(l->data + (i*l->elementSize));
    //set the new object
    /* @mutate l->elementSize SIZE */
    memcpy(l->data+(i*l->elementSize), e, l->elementSize);
    return true;
}

// Sets a copy of the given objects to a specific index in the array
char rfDynamicArray_Set_copy(
    RF_DynamicArray* l,
    uint32_t i,
    /* @mutate void* TYPEPTR */
    void* e)
{
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to set an element in an array with an index "
                  "out of bounds. If you want to add objects to the list"
                  "use the Add function.", RE_INDEX_OUT_OF_BOUNDS)
        return false;
    }
    /* @mutate ptr2Destroyp NOSIZE */
    l->ptr2Destroy(l->data + (i*l->elementSize));
    //set the new object as the copy of e
    /* @mutate ptr2Copypp ASSIGN NOSIZE*/
    l->ptr2Copy(l->data+(i*l->elementSize), e);
    return true;
}


// Removes an object from the list. All objects get moved to make up for
// the freed up space and if there is too much space left the list gets 
// reallocated downwards. The destructor of the element is called if given
void rfDynamicArray_Remove(RF_DynamicArray* l, uint32_t i)
{
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to remove an element from an array with an"
                  " index out of bounds",RE_INDEX_OUT_OF_BOUNDS)
        return;
    }

    /* @mutate ptr2Destroyp DESTROY NOSIZE */
    l->ptr2Destroy(l->data+(i*l->elementSize));

    //Move the whole buffer back to cover up for the removed element
    /* @mutate l->elementSize SIZE */
    memmove(l->data+(i*l->elementSize), l->data+(i*l->elementSize)+l->elementSize,l->elementSize*(l->size-i-1));
    //and reduce size
    l->size--;

    //also check if we can reallocate downwards. Condition is if the 
    //current size is less than the fraction of the capacity
    /* @mutate l->elementsize SIZE */
    if( l->size*l->elementSize < 
        l->bufferCapacity/RF_OPTION_DYNAMICARRAY_CAPACITY_M)
    {
        l->bufferCapacity /= RF_OPTION_DYNAMICARRAY_CAPACITY_M;
        //reallocate
        /* @mutate void* TYPEPTR */
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in removing an object from an array"
                      " during an attempt to shrink the buffer. Aborting "
                      "program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }
}

// Removes an object from the list without calling its destroy function
void rfDynamicArray_Remove_nofree(RF_DynamicArray* l, uint32_t i)
{
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to remove an element from an array with an"
                  " index out of bounds",RE_INDEX_OUT_OF_BOUNDS)
        return;
    }

    //Move the whole buffer back to cover up for the removed element
    /* @mutate l->elementSize SIZE */
    memmove(l->data+(i*l->elementSize), l->data+(i*l->elementSize)+l->elementSize,l->elementSize*(l->size-i-1));
    //and reduce size
    l->size--;

    //also check if we can reallocate downwards. Condition is if the 
    //current size is less than the fraction of the capacity
    /* @mutate l->elementsize SIZE */
    if( l->size*l->elementSize < 
        l->bufferCapacity/RF_OPTION_DYNAMICARRAY_CAPACITY_M)
    {
        l->bufferCapacity /= RF_OPTION_DYNAMICARRAY_CAPACITY_M;
        //reallocate
        /* @mutate void* TYPEPTR */
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in removing an object from an array"
                      " during an attempt to shrink the buffer. Aborting "
                      "program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }
}



