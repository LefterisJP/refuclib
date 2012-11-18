/**
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include <stdlib.h>//for malloc,free and other stdlib functions
#include <string.h> //for memcpy

#include <rf_setup.h>
#include <Data_Structures/rfc_array.h>
#include "datastructures_private.h"


/***********************************************************************RF_ArrayV Functions**************************************************************************************/

// Allocates and returns an array of objects
RF_ArrayV*  i_rfArrayV_Create(uint32_t elSize,uint32_t size,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*))
{
    RF_ArrayV* ret;
    //if the given size is 0
    if(elSize == 0)
    {
        LOG_ERROR("Tried to create an array of objects with zero sized elements",RE_ARRAY_INIT_FAILURE);
        return 0;
    }
    //allocate the new array
    RF_MALLOC(ret,sizeof(RF_ArrayV))
    //allocate the data buffer
    ret->elementSize = elSize;
    ret->size = size;
    RF_MALLOC(ret->data,elSize*size);
    ret->ptr2Destroy = ptr2Destroy;
    ret->ptr2Copy = ptr2Copy;
    //initialize the binary array
    rfBinaryArray_Init(&ret->map,size);
    return ret;
}
// Initialises an array of objects
char  i_rfArrayV_Init(RF_ArrayV* arr,uint32_t elSize,uint32_t size,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*))
{
    //if the given size is 0
    if(elSize == 0)
    {
        LOG_ERROR("Tried to initialize an array of objects with zero sized elements",RE_ARRAY_INIT_FAILURE);
        return false;
    }
    //allocate the data buffer
    arr->elementSize = elSize;
    arr->size = size;
    RF_MALLOC(arr->data,elSize*size);
    arr->ptr2Destroy = ptr2Destroy;
    arr->ptr2Copy = ptr2Copy;
    //initialize the binary array
    rfBinaryArray_Init(&arr->map,size);
    return true;
}

//Copies RF_ArrayV @c src into RF_ArrayV @c dst
void rfArrayV_Copy_IN(RF_ArrayV* dst,RF_ArrayV* src)
{
    uint32_t i;
    dst->elementSize = src->elementSize;
    dst->size = src->size;
    RF_MALLOC(dst->data,dst->elementSize*dst->size);
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    rfBinaryArray_Copy_IN(&dst->map,&src->map);
    //if a copy function has not been given just blindy copy the whole data buffer (dangerous)
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->size*dst->elementSize);
    else//if a copy function has been given
    {   //for each existing item of the array
        for(i=0;i<dst->size;i ++)
        {
            if(rfBinaryArray_Get_NC(&dst->map,i)==true)
            {
                //copy the new element in to the data
                dst->ptr2Copy(dst->data+(dst->elementSize*i),src->data+(src->elementSize*i));
            }
        }
    }
}
//Creates and returns an allocated copy of the given RF_ArrayV
RF_ArrayV* rfArrayV_Copy_OUT(RF_ArrayV* src)
{
    RF_ArrayV* dst;
    RF_MALLOC(dst,sizeof(RF_ArrayV));
    uint32_t i;
    dst->elementSize = src->elementSize;
    dst->size = src->size;
    RF_MALLOC(dst->data,dst->elementSize*dst->size);
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    rfBinaryArray_Copy_IN(&dst->map,&src->map);
    //if a copy function has not been given just blindy copy the whole data buffer (dangerous)
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->size*dst->elementSize);
    else//if a copy function has been given
    {   //for each existing item of the array
        for(i=0;i<dst->size;i ++)
        {
            if(rfBinaryArray_Get_NC(&dst->map,i)==true)
            {
                //copy the new element in to the data
                dst->ptr2Copy(dst->data+(dst->elementSize*i),src->data+(src->elementSize*i));
            }
        }
    }
    return dst;
}

// Destroys an array of objects releasing their memory and calling the destructing function if it exists, use with arrays made by _Create
void rfArrayV_Destroy(RF_ArrayV* a)
{
    uint32_t i;
    //if we have a function prototype for deletion use it to delete the individual objects
    if(a->ptr2Destroy != 0)
    {
        for(i = 0; i < a->size; i ++)
        {
            //if the element exists
            if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds because of the for condition
            {
                a->ptr2Destroy(a->data+ (i*a->elementSize));

            }
        }
    }
    //finally just free the  whole data buffer, the binary array and the array itself
    free(a->data);
    rfBinaryArray_Destroy(&a->map);
    free(a);
}
// Destroys an array of objects releasing their memory without calling the destructing function if it exists, use with arrays made by _Create
void rfArrayV_Destroy_nofree(RF_ArrayV* a)
{
    //finally just free the  whole data buffer, the binary array and the array itself
    free(a->data);
    rfBinaryArray_Destroy(&a->map);
    free(a);
}

// Deinitializes an array of objects releasing their memory and calling the destructing function if it exists
void rfArrayV_Deinit(RF_ArrayV* a)
{
    uint32_t i;
    //if we have a function prototype for deletion use it to delete the individual objects
    if(a->ptr2Destroy != 0)
    {
        for(i = 0; i < a->size; i ++)
        {
            //if the element exists
            if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds because of the for condition
            {
                a->ptr2Destroy(a->data+ (i*a->elementSize) );
            }
        }
    }
    //finally just free the  whole data buffer, the binary array and the array itself
    free(a->data);
    rfBinaryArray_Destroy(&a->map);
}
// Deinitializes an array of objects releasing their memory without calling the destructing function if it exists
void rfArrayV_Deinit_nofree(RF_ArrayV* a)
{
    //finally just free the  whole data buffer, the binary array and the array itself
    free(a->data);
    rfBinaryArray_Destroy(&a->map);
}


// Sets an object of the array to the given pointer value. If there was already an object in the previous location it is removed and its destructor is called.
char rfArrayV_Set(RF_ArrayV* a,uint32_t i,void* e,char free)
{
    uint32_t index;
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to set an object in an Array of Objects in an out of bounds index position",RE_INDEX_OUT_OF_BOUNDS);
        return false;
    }
    index = a->elementSize*i;
    //if there is an object there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    {
        if(free==true)
            if(a->ptr2Destroy != 0)
            a->ptr2Destroy(a->data+index);
        //copy the new element in to the data
        memcpy(a->data+(index),e,a->elementSize);
    }
    else//no object was there before
    {
        //mark the object as existing
        rfBinaryArray_Set_NC(&a->map,i,1);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
        //copy the new element in to the array
        memcpy(a->data+index,e,a->elementSize);
    }
    return true;
}
//Sets a copy of object @c e at the @c i index of the Array
char rfArrayV_Set_copy(RF_ArrayV* a,uint32_t i,void* e,char free)
{
    uint32_t index;
    //checks
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to set an object in an Array of Objects in an out of bounds index position",RE_INDEX_OUT_OF_BOUNDS);
        return false;
    }
    if(a->ptr2Copy==0)
        return false;
    index = a->elementSize*i;
    //if there is an object there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    {
        if(free==true)
            if(a->ptr2Destroy != 0)
            a->ptr2Destroy(a->data+index);
        //insert a copy of the element in to the array
        a->ptr2Copy(a->data+index,e);
    }
    else//no object was there before
    {
        //mark the object as existing
        rfBinaryArray_Set_NC(&a->map,i,1);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
        //insert a copy of the element in to the array
        a->ptr2Copy(a->data+index,e);
    }
    return true;
}

// Retrieves an object from the array and saves it in the given pointer. Pointer must be allocated
int32_t rfArrayV_Get(RF_ArrayV* a,uint32_t i,void* ret)
{
    //check for index being out of bounds
    if(i >= a->size)
        RETURN_LOG_ERROR("Attempted to retrieve an object from an Array of Objects from an index that is out of bounds",RE_INDEX_OUT_OF_BOUNDS)

    //check if the requested elements exists
    if(rfBinaryArray_Get_NC(&a->map,i)==false)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
        RETURN_LOG_ERROR("Attempted to retrieve an object from an Array of Objects from an uninitialized position",RE_ARRAY_UNINITIALIZED_OBJECT)

    //now get the element
    memcpy(ret,a->data+ (a->elementSize*i) ,a->elementSize);
    return RF_SUCCESS;
}


// Removes an object from the array
void rfArrayV_Remove(RF_ArrayV* a, uint32_t i)
{
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to remove an element from an array of objects with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }

    uint32_t index = a->elementSize*i;
    //if there is an object there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)// the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    {
        if(a->ptr2Destroy != 0)
            a->ptr2Destroy(a->data+index);

        //also mark it as no longer existin
        rfBinaryArray_Set_NC(&a->map,i,0);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    }
    //if there is no object there, well no need to do anything
}
// Removes an object from the array without calling its destructing function
void rfArrayV_Remove_nofree(RF_ArrayV* a, uint32_t i)
{
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to remove an element from an array of objects with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }
    //if there is an object there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)// the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
        //also mark it as no longer existin
        rfBinaryArray_Set_NC(&a->map,i,0);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
}

// Reallocates the array changing its size
char rfArrayV_Reallocate(RF_ArrayV* a,uint32_t newSize)
{
    void* testptr = realloc(a->data,newSize*a->elementSize);
    //if reallocation fails
    if(testptr == 0)
    {
        LOG_ERROR("Reallocation of an array of objects has failed",RE_REALLOC_FAILURE);
        return false;
    }
    //now reallocate the binary map too
    if(rfBinaryArray_Reallocate(&a->map,newSize)==false)
        return false;
    //success
    a->data = testptr;
    a->size = newSize;
    return true;
}

/***********************************************************************RF_ArrayP Functions**************************************************************************************/


// Allocates and returns an array of pointers to objects
RF_ArrayP* i_rfArrayP_Create(uint32_t size,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*))
{
    RF_ArrayP* ret;
    //if the given size is 0
    if(size == 0)
    {
        LOG_ERROR("Tried to create an array of pointers to objects with zero sized elements",RE_ARRAY_INIT_FAILURE);
        return 0;
    }

    //allocate the new array
    RF_MALLOC(ret,sizeof(RF_ArrayP));
    //allocate the data buffer
    ret->size= size;
    RF_MALLOC(ret->data,RF_PTRMEMORYLOC_SIZE*size);
    ret->ptr2Destroy = ptr2Destroy;
    ret->ptr2Copy = ptr2Copy;
    //initialize the binary array
    rfBinaryArray_Init(&ret->map,size);

    return ret;
}
// Initializes an array of pointers to objects
char i_rfArrayP_Init(RF_ArrayP* arr,uint32_t size,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*))
{
    //if the given size is 0
    if(size == 0)
    {
        LOG_ERROR("Tried to initialize an array of pointers to objects with zero sized elements",RE_ARRAY_INIT_FAILURE);
        return false;
    }
    //allocate the data buffer
    arr->size= size;
    RF_MALLOC(arr->data,RF_PTRMEMORYLOC_SIZE*size);
    arr->ptr2Destroy = ptr2Destroy;
    arr->ptr2Copy = ptr2Copy;
    //initialize the binary array
    rfBinaryArray_Init(&arr->map,size);
    return true;
}


//Copies RF_ArrayP @c src into RF_ArrayP @c dst
void rfArrayP_Copy_IN(RF_ArrayP* dst,RF_ArrayP* src)
{
    uint32_t i;
    dst->size = src->size;
    RF_MALLOC(dst->data,RF_PTRMEMORYLOC_SIZE*dst->size);
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    rfBinaryArray_Copy_IN(&dst->map,&src->map);
    //if a copy function has not been given just blindy copy the whole data buffer (dangerous)
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->size*RF_PTRMEMORYLOC_SIZE);
    else//if a copy function has been given
    {   //for each existing item of the array
        for(i=0;i<dst->size;i ++)
        {
            if(rfBinaryArray_Get_NC(&dst->map,i)==true)
            {
                //copy the element into the new array
                *(dst->data+i) = dst->ptr2Copy(*(src->data+i));
            }
        }
    }
}
//Creates and returns an allocated copy of the given RF_ArrayP
RF_ArrayP* rfArrayP_Copy_OUT(RF_ArrayP* src)
{
    RF_ArrayP* dst;
    uint32_t i;
    RF_MALLOC(dst,sizeof(RF_ArrayP));
    dst->size = src->size;
    RF_MALLOC(dst->data,RF_PTRMEMORYLOC_SIZE*dst->size);
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    rfBinaryArray_Copy_IN(&dst->map,&src->map);
    //if a copy function has not been given just blindy copy the whole data buffer (dangerous)
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->size*RF_PTRMEMORYLOC_SIZE);
    else//if a copy function has been given
    {   //for each existing item of the array
        for(i=0;i<dst->size;i ++)
        {
            if(rfBinaryArray_Get_NC(&dst->map,i)==true)
            {
                //copy the element into the new array
                *(dst->data+i) = dst->ptr2Copy(*(src->data+i));
            }
        }
    }
    return dst;
}

// Destroys an array of pointers to objects releasing their memory and calling the destructing function if it exists
void rfArrayP_Destroy(RF_ArrayP* a)
{
    uint32_t i;
    //if we have a function prototype for deletion use it to delete the individual objects
    if(a->ptr2Destroy != 0)
    {
        for(i = 0; i < a->size; i ++)
        {
            //if the element exists
            if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds because of the for condition
            {
                a->ptr2Destroy( *(a->data+i) );
            }
        }
    }

    //finally just free the  whole data buffer, the binary array and the array itself
    free(a->data);
    rfBinaryArray_Deinit(&a->map);
    free(a);
}
// Destroys an array of pointers to objects without calling the destructing function
void rfArrayP_Destroy_nofree(RF_ArrayP* a)
{
    //finally just free the  whole data buffer, the binary array and the array itself
    free(a->data);
    rfBinaryArray_Deinit(&a->map);
    free(a);
}
// Deinitializes an array of pointers to objects releasing their memory and calling the destructing function if it exists
void rfArrayP_Deinit(RF_ArrayP* a)
{
    uint32_t i;
    //if we have a function prototype for deletion use it to delete the individual objects
    if(a->ptr2Destroy != 0)
    {
        for(i = 0; i < a->size; i ++)
        {
            //if the element exists
            if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds because of the for condition
            {
                a->ptr2Destroy(*(a->data+i));
            }
        }
    }

    //finally just free the  whole data buffer, the binary array
    free(a->data);
    rfBinaryArray_Deinit(&a->map);
}
// Deinitializes an array of pointers to objects without calling the destructing function
void rfArrayP_Deinit_nofree(RF_ArrayP* a)
{
    //finally just free the  whole data buffer, the binary array
    free(a->data);
    rfBinaryArray_Deinit(&a->map);
}

//Sets the pointer at index @c i of the array to point to the object pointed to by @c e
char rfArrayP_Set(RF_ArrayP* a,uint32_t i,void* e,char free)
{
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to set a pointer in an Array of Pointers in an out of bounds index position",RE_INDEX_OUT_OF_BOUNDS);
        return false;
    }
    //if there is an object there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    {
        if(free == true)
            if(a->ptr2Destroy != 0)
                a->ptr2Destroy( *(a->data+i));

        //now make the pointer point to the location of the new element
        *(a->data+i) =  e;
    }
    else//no object was there before
    {
        //mark the object as existing
        rfBinaryArray_Set_NC(&a->map,i,1);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
        //and make the pointer point to the location of the new element
        *(a->data+i) =  e;
    }

    return true;
}
//Sets the pointer at index @c i of the array to point to a copy of the object pointed to by @c
char rfArrayP_Set_copy(RF_ArrayP* a,uint32_t i,void* e,char free)
{
    if(a->ptr2Copy == 0)
        return false;
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to set a pointer in an Array of Pointers in an out of bounds index position",RE_INDEX_OUT_OF_BOUNDS);
        return false;
    }
    //if there is an object there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    {
        if(free == true)
            if(a->ptr2Destroy != 0)
                a->ptr2Destroy( *(a->data+i));
        //and make the pointer point to a copy of the new element
        *(a->data+i) = a->ptr2Copy(e);
    }
    else//no object was there before
    {
        //mark the object as existing
        rfBinaryArray_Set_NC(&a->map,i,1);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
        //and make the pointer point to a copy of the new element
        *(a->data+i) = a->ptr2Copy(e);

    }
    return true;
}

// Retrieves a pointer to an object from the array
void* rfArrayP_Get(RF_ArrayP* a,uint32_t i)
{
    if( i >= a->size)
    {
        LOG_ERROR("Attempted to retrieve a pointer from an Array of Pointers with an out of bounds index",RE_INDEX_OUT_OF_BOUNDS);
        return 0;
    }
    if(rfBinaryArray_Get_NC(&a->map,i)==false)
    {
        LOG_ERROR("Attempted to retrieve a pointer from an Array of Pointers' position that has not been set",RE_ARRAY_UNINITIALIZED_POINTER);
        return 0;
    }

    //return the memory location of the pointer
    return *(a->data+i);
}

//Removes a pointer from the array
void rfArrayP_Remove(RF_ArrayP* a, uint32_t i)
{
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to remove a pointer from an array of pointers with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }
    //if there is an pointer there before make sure to destroy it
    if(rfBinaryArray_Get_NC(&a->map,i) == true)// the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    {
        if(a->ptr2Destroy != 0)
            a->ptr2Destroy(*(a->data+i));

        //also mark it as no longer existing
        rfBinaryArray_Set_NC(&a->map,i,0);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
    }
    //if there is no object there, well no need to do anything
}
//Removes a pointer from the array without calling the destructing functiong on the object it points to
void rfArrayP_Remove_nofree(RF_ArrayP* a, uint32_t i)
{
    if(i >= a->size)
    {
        LOG_ERROR("Attempted to remove a pointer from an array of pointers with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }

    //mark it as no longer existing
    rfBinaryArray_Set_NC(&a->map,i,0);//the _NC (no check) version is used here since we are sure i can't be an index out of bounds since it's checked for in the start of the function
}


// Reallocates the array of pointers changing its size
char rfArrayP_Reallocate(RF_ArrayP* a,uint32_t newSize)
{
    void** testptr = realloc(a->data,newSize*RF_PTRMEMORYLOC_SIZE);
    //if reallocation fails
    if(testptr == 0)
    {
        LOG_ERROR("Reallocation of an array of pointers has failed",RE_REALLOC_FAILURE);
        return false;
    }

    //now reallocate the binary map too
    if(rfBinaryArray_Reallocate(&a->map,newSize)==false)
        return false;

    //success
    a->data = testptr;
    a->size = newSize;
    return true;
}


/*******************************************************************RF Func Group Functions************************************************************************************/

// Allocates and returns a group of function pointers
RF_FuncGroup*   i_rfFuncGroup_Create(uint32_t elSize,uint32_t numElements)
{
    RF_FuncGroup* ret;
    if(elSize == 0)
    {
        LOG_ERROR("Tried to create a function group with zero sized elements",RE_FUNCGROUP_INIT_FAILURE);
        return 0;
    }

    //allocate the new group
    RF_MALLOC(ret,sizeof(RF_FuncGroup));
    ret->elementSize = elSize;
    RF_MALLOC(ret->data,sizeof(genericfp)*numElements);
    ret->bufferCapacity = numElements;
    ret->size = 0;

    return ret;
}
// Initialisesa group of function pointers
char   i_rfFuncGroup_Init(RF_FuncGroup* g,uint32_t elSize,uint32_t numElements)
{
    if(elSize == 0)
    {
        LOG_ERROR("Tried to initialize a function group with zero sized elements",RE_FUNCGROUP_INIT_FAILURE);
        return false;
    }

    //initialize the group
    g->elementSize = elSize;
    RF_MALLOC(g->data,sizeof(genericfp)*numElements);
    g->bufferCapacity = numElements;
    g->size = 0;

    return true;
}

//Destroys a group of functions
void rfFuncGroup_Destroy(RF_FuncGroup* g)
{
    //just free the  whole data buffer and the function group object. Can't free individual function poitners
    free(g->data);
    free(g);
}
//Deinitializes a group of functions
void rfFuncGroup_Deinit(RF_FuncGroup* g)
{
    free(g->data);
}


//Adds a new function to the group. Element must be an object with a  value. Not a pointer. Values like 8,3,6.16
void rfFuncGroup_Add(RF_FuncGroup* g,genericfp fp)
{
    //check if we are getting same sized element
    if(sizeof(fp) != g->elementSize)
    {
        LOG_ERROR("Tried to add a different function to the  Group than the one used in initialiation",RE_FUNCGROUP_ADD_FAILURE);
        return;
    }

    //check if there is enough space in the group
    if(g->size==g->bufferCapacity)
    {
        //if not reallcoate to double capacity
        //not using macro here cause no type is known and gotta do it manually
        void* testPtr = realloc(g->data,g->elementSize*g->bufferCapacity*2);
        if(testPtr) g->data = testPtr;
        else
        {
            DLOG_ERROR("Realloc Failed",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
        //also double capacity
        g->bufferCapacity*=2;

    }

    //add the function pointer
    g->data[g->size] = fp;
    g->size++;

}


// Retrieves function from the group
genericfp rfFuncGroup_Get(RF_FuncGroup* g,uint32_t i)
{
    if( i >= g->size)
    {
        LOG_ERROR("Attempted to retrieve a function from a group with an out of bounds index",RE_INDEX_OUT_OF_BOUNDS);
        return 0;
    }

    return g->data[i];
}


// Removes an element from the group
void rfFuncGroup_Remove(RF_FuncGroup* g, uint32_t i)
{
    uint32_t j;
    if( i >= g->size)
    {
        LOG_ERROR("Attempted to remove an element from a group with an out of bounds index",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }
    //move the pointers one position back
    for(j = i; j < g->size; j ++)
    {
       g->data[j] = g->data[j+1];
    }
    //and reduce size by 1
    g->size--;
}


// Empties a function group
void rfFuncGroup_Empty(RF_FuncGroup* g)
{
    g->size = 0;
}





