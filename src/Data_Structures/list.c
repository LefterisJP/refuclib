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
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Data_Structures/list_decl.h> //for RF_ListP and RF_ListV
#include <Data_Structures/list.h>
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

/****************************************RF_ListV Functions************************************************************************************/

//Allocates and returns a list of objects
RF_ListV*  i_rfListV_Create(uint32_t elSize,uint32_t numElements,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*))
{
    RF_ListV* ret;
    //if the given size is 0
    if(elSize == 0)
    {
        LOG_ERROR("Tried to create a list of objects which have zero size",RE_LIST_INIT_FAILURE);
        return 0;
    }
    //allocate the new group
    RF_MALLOC(ret,sizeof(RF_ListV));
    //allocate the data buffer
    ret->elementSize = elSize;
    ret->bufferCapacity = elSize*numElements;
    RF_MALLOC(ret->data,ret->bufferCapacity);
    ret->size = 0;
    ret->ptr2Destroy = ptr2Destroy;
    ret->ptr2Copy = ptr2Copy;

    return ret;
}
//Initialises a list of objects
char  i_rfListV_Init(RF_ListV* list,uint32_t elSize,uint32_t numElements,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*))
{
    //if the given size is 0
    if(elSize == 0)
    {
        LOG_ERROR("Tried to initialize a list of objects which have zero size",RE_LIST_INIT_FAILURE);
        return false;
    }

    //allocate the data buffer
    list->elementSize = elSize;
    list->bufferCapacity = elSize*numElements;
    RF_MALLOC(list->data,list->bufferCapacity);
    list->size = 0;
    list->ptr2Destroy = ptr2Destroy;
    list->ptr2Copy = ptr2Copy;
    return true;
}

//Copies RF_ListV @c src into RF_ListV @c dst
void rfListV_Copy_IN(RF_ListV* dst,RF_ListV* src)
{
    uint32_t i;
    dst->bufferCapacity = src->bufferCapacity;
    dst->size = src->size;
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->elementSize = src->elementSize;
    dst->ptr2Copy = src->ptr2Copy;
    //allocate the data buffer
    RF_MALLOC(dst->data,dst->bufferCapacity)
    //if we don't have a copy function just copy the whole data buffer (dangerous)
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->bufferCapacity);
    else
    {   //else copy each element using the copy function
        for(i=0;i<dst->size;i++)
        {
            dst->ptr2Copy( dst->data+(i*dst->elementSize),src->data+(i*src->elementSize));
        }
    }
}
//brief Creates and returns an allocated copy of the given RF_ListV
RF_ListV* rfListV_Copy_OUT(RF_ListV* src)
{
    uint32_t i;
    RF_ListV* dst;
    RF_MALLOC(dst,sizeof(RF_ListV));
    dst->bufferCapacity = src->bufferCapacity;
    dst->size = src->size;
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->elementSize = src->elementSize;
    dst->ptr2Copy = src->ptr2Copy;
    //allocate the data buffer
    RF_MALLOC(dst->data,dst->bufferCapacity)
    //if we don't have a copy function just copy the whole data buffer (dangerous)
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->bufferCapacity);
    else
    {   //else copy each element using the copy function
        for(i=0;i<dst->size;i++)
        {
           dst->ptr2Copy( dst->data+(i*dst->elementSize),src->data+(i*src->elementSize));
        }
    }
    return dst;
}

// Destroys a list of objects releasing their memory and calling the destructing function if it exists
void rfListV_Destroy(RF_ListV* l)
{
    uint32_t i;
    uint32_t index = 0;
    //if we have a function prototype for deletion use it to delete the individual objects
    if(l->ptr2Destroy != 0)
    {
        for(i = 0; i < l->size; i ++)
        {
            l->ptr2Destroy(l->data+index);
            index += l->elementSize;
        }
    }
    //finally just free the  whole data buffer and the list object
    free(l->data);
    free(l);
}

// Destroys a list of objects without calling individual object destructing function
void rfListV_Destroy_nofree(RF_ListV* l)
{
    //just free the  whole data buffer and the list object
    free(l->data);
    free(l);
}
// Destroys a list of objects but not the list memory itself releasing their memory and calling the destructing function if it exists
void rfListV_Deinit(RF_ListV* l)
{
    uint32_t i;
    uint32_t index = 0;
    //if we have a function prototype for deletion use it to delete the individual objects
    if(l->ptr2Destroy != 0)
    {
        for(i = 0; i < l->size; i ++)
        {
            l->ptr2Destroy(l->data+index);
            index += l->elementSize;
        }
    }
    //finally just free the  whole data buffer
    free(l->data);
}
// Deinitializes a list of objects without calling individual object destructing function
void rfListV_Deinit_nofree(RF_ListV* l)
{
    //just free the  whole data buffer
    free(l->data);
}



// Adds a new object to the list by taking it from a pointer address. The object is added right after the last element and if it does not fit in the buffer a reallocation happens
void rfListV_Add(RF_ListV* l,void* object)
{
    //check if there is enough space in the list
    if(l->size*l->elementSize >= l->bufferCapacity)
    {
        //if not reallocate to double capacity
        l->bufferCapacity*= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in adding an object to a List of Objects during an attempt to increase buffer capacity. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }//end of capacity increase case
    //copy the new element in to the data and increase the index
    memcpy(l->data+(l->size*l->elementSize),object,l->elementSize);
    l->size++;
}
// Adds a a copy of an object to the list
char rfListV_Add_copy(RF_ListV* l,void* object)
{
    //check for existence of a copy function
    if(l->ptr2Copy == 0)
        return false;
    //check if there is enough space in the list
    if(l->size*l->elementSize >= l->bufferCapacity)
    {
        //if not reallocate to double capacity
        l->bufferCapacity*= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in adding an object to a List of Objects during an attempt to increase buffer capacity. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }//end of capacity increase case
    //copy the new element in to the data and increase the index
    l->ptr2Copy(l->data+(l->size*l->elementSize),object);
    l->size++;
    return true;
}

// Retrieves an object from the group and saves it in the given pointer. Pointer must be allocated
int32_t rfListV_Get(RF_ListV* l,uint32_t i,void* ret)
{
    uint32_t index = i*l->elementSize;
    //if the index goes over the buffer capacity
    if( i >= l->size)
    {
        LOG_ERROR("Attempted to retrieve an element from a list with an index that is out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return RF_FAILURE;
    }

    memcpy(ret,l->data+index,l->elementSize);
    return RF_SUCCESS;
}

// Sets an object of the list to the given pointer value. If there was already an object in the previous location it is removed and its destructor is called.
char rfListV_Set(RF_ListV* l,uint32_t i,void* e,char free)
{
    uint32_t index = i*l->elementSize;
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to set an element in a list with an index out of bounds. If you want to add objects to the list use the Add function.",RE_INDEX_OUT_OF_BOUNDS)
        return false;
    }
    //and if we have a function prototype for deletion use it to delete the object
    if(free==true)
        if(l->ptr2Destroy != 0)
        {
            l->ptr2Destroy(l->data+index);
        }
    //set the new object
    memcpy(l->data+index,e,l->elementSize);
    return true;
}

// Sets a copy of the given objects to a specific index in the list
char rfListV_Set_copy(RF_ListV* l,uint32_t i,void* e,char free)
{
    uint32_t index = i*l->elementSize;
    if(l->ptr2Copy == 0)
        return false;
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to set an element in a list with an index out of bounds. If you want to add objects to the list use the Add function.",RE_INDEX_OUT_OF_BOUNDS)
        return false;
    }
    //and if we have a function prototype for deletion use it to delete the object
    if(free==true)
        if(l->ptr2Destroy != 0)
        {
            l->ptr2Destroy(l->data+index);
        }
    //set the new object as the copy of e
    l->ptr2Copy(l->data+index,e);
    return true;
}


// Removes an object from the list. All objects get moved to make up for the freed up space and if there is too much space left the list gets reallocated downwards. The destructor of the element is called if given.
void rfListV_Remove(RF_ListV* l, uint32_t i)
{
    uint32_t index;
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to remove an element from a list with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS)
        return;
    }

    index = i*l->elementSize;
    //if we have a function prototype for deletion use it to delete the object
    if(l->ptr2Destroy != 0)
    {
        l->ptr2Destroy(l->data+index);
    }

    //Move the whole buffer back to cover up for the removed element
    memmove(l->data+index,l->data+index+l->elementSize,l->elementSize*(l->size-i-1));
    //and reduce size
    l->size--;

    //also check if we can reallocate downwards. Condition is if the current size is less than the fraction of the capacity
    if( l->size*l->elementSize < l->bufferCapacity/RF_OPTION_LIST_CAPACITY_M)
    {
        l->bufferCapacity/= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in removing an object from a List during an attempt to shrink the buffer. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }
}
// Removes an object from the list without calling its destroy function
void rfListV_Remove_nofree(RF_ListV* l, uint32_t i)
{
    uint32_t index;
    //if the index points to a non-yet set element
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to remove an element from a list with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS)
        return;
    }

    index = i*l->elementSize;
    //Move the whole buffer back to cover up for the removed element
    memmove(l->data+index,l->data+index+l->elementSize,l->elementSize*(l->size-i-1));
    //and reduce size
    l->size--;

    //also check if we can reallocate downwards. Condition is if the current size is less than the fraction of the capacity
    if( l->size*l->elementSize < l->bufferCapacity/RF_OPTION_LIST_CAPACITY_M)
    {
        l->bufferCapacity/= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void* testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in removing an object from a List during an attempt to shrink the buffer. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }
}



/*****************************************************************RF_ListP Functions **************************************************************************/

// Allocates and returns a list of pointers to objects
RF_ListP*  i_rfListP_Create(uint32_t numElements,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*))
{
    RF_ListP* ret;
    if(numElements<=0)
    {
        LOG_ERROR("Attempted to create a list of pointers with a zero number of initial elements",RE_LIST_INIT_FAILURE);
        return 0;
    }

    //allocate the new list
    RF_MALLOC(ret,sizeof(RF_ListP));
    //allocate the data buffer and keep its size
    RF_CALLOC(ret->data,numElements*RF_OPTION_LIST_CAPACITY_M,RF_PTRMEMORYLOC_SIZE);
    ret->bufferCapacity = numElements*RF_OPTION_LIST_CAPACITY_M*RF_PTRMEMORYLOC_SIZE;
    ret->size = 0;
    ret->ptr2Destroy = ptr2Destroy;
    ret->ptr2Copy = ptr2Copy;

    return ret;
}
// Initializes a list of pointers to objects
char  i_rfListP_Init(RF_ListP* list,uint32_t numElements,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*))
{
    if(numElements<=0)
    {
        LOG_ERROR("Attempted to create a list of pointers with a zero number of initial elements",RE_LIST_INIT_FAILURE);
        return false;
    }

    //allocate the data buffer and keep its size
    RF_CALLOC(list->data,numElements*RF_OPTION_LIST_CAPACITY_M,RF_PTRMEMORYLOC_SIZE);
    list->bufferCapacity = numElements*RF_OPTION_LIST_CAPACITY_M*RF_PTRMEMORYLOC_SIZE;
    list->size = 0;
    list->ptr2Destroy = ptr2Destroy;
    list->ptr2Copy = ptr2Copy;

    return true;
}


//Copies RF_ListP @c src into RF_ListP @c dst
void rfListP_Copy_IN(RF_ListP* dst,RF_ListP* src)
{
    uint32_t i;
    dst->bufferCapacity = src->bufferCapacity;
    dst->size = src->size;
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    //allocate the data buffer
    RF_CALLOC(dst->data,dst->bufferCapacity,RF_PTRMEMORYLOC_SIZE)
    //if there is no copy function, jsut copy the whole data buffer
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->bufferCapacity);
    else
    {   //else utilize the copy function to make copies of each saved pointer to object
        for(i=0;i<dst->size;i++)
        {
            *(dst->data+i) = dst->ptr2Copy(*(src->data+i));
        }
    }
}
// Creates and returns an allocated copy of the given RF_ListP
RF_ListP* rfListP_Copy_OUT(RF_ListP* src)
{
    uint32_t i;
    RF_ListP* dst;
    RF_MALLOC(dst,sizeof(RF_ListP));
    dst->bufferCapacity = src->bufferCapacity;
    dst->size = src->size;
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    //allocate the data buffer
    RF_CALLOC(dst->data,dst->bufferCapacity,RF_PTRMEMORYLOC_SIZE)
    //if there is no copy function, jsut copy the whole data buffer
    if(dst->ptr2Copy == 0)
        memcpy(dst->data,src->data,dst->bufferCapacity);
    else
    {   //else utilize the copy function to make copies of each saved pointer to object
        for(i=0;i<dst->size;i++)
        {
            *(dst->data+i) = dst->ptr2Copy(*(src->data+i));
        }
    }
    return dst;
}

// Destroys a list of pointers and its memory, calling the destructors of the objects they point to at the same time. If you don't want to call the destructor of the object itself call the _nofree version of the function
void rfListP_Destroy(RF_ListP* l)
{
    uint32_t i;
    //if we have a function prototype for deletion use it to delete the objects the pointers point to
    if(l->ptr2Destroy != 0)
    {
        for(i = 0; i < l->size; i ++)
        {
            l->ptr2Destroy( *(l->data+i));
        }
    }

    //finally just free the  whole data buffer and the list object
    free(l->data);
    free(l);
}
// Destroys a list of pointers and its memory. The objects the pointers point to are not disturbed.
void rfListP_Destroy_nofree(RF_ListP* l)
{
    free(l->data);
    free(l);
}
// Destroys a list of pointers but not its memory, calling the destructors of the objects they point to at the same time. If you don't want to call the destructor of the object itself call the _nofree version of the function
void rfListP_Deinit(RF_ListP* l)
{
    uint32_t i;
    //if we have a function prototype for deletion use it to delete the objects the pointers point to
    if(l->ptr2Destroy != 0)
    {
        for(i = 0; i < l->size; i ++)
        {
            l->ptr2Destroy(*(l->data+i));
        }
    }

    //finally just free the  whole data buffer and the list object
    free(l->data);
}
// Destroys a list of pointers but not its memory. The objects the pointers point to are not disturbed.
void rfListP_Deinit_nofree(RF_ListP* l)
{
    free(l->data);
}

// Adds a new pointer to the list. The pointer is added right after the last pointer and if it does not fit in the buffer a reallocation happens
void rfListP_Add(RF_ListP* l,void* object)
{
    //check if there is enough space in the list
    if(l->size*RF_PTRMEMORYLOC_SIZE >= l->bufferCapacity)
    {
        //if not reallocate to double capacity
        l->bufferCapacity*= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void** testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in adding a pointer to a List of pointers during an attempt to increase buffer capacity. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }//end of capacity increase case

    //give the value of the pointer's memory location to the appropriate member of the list
    *(l->data+l->size) = object;

    //finally increase the size of the list
    l->size++;
}
// Adds a pointer to the copy of the object pointed to by @c object
char rfListP_Add_copy(RF_ListP* l,void* object)
{
    if(l->ptr2Copy == 0)
        return false;
    //check if there is enough space in the list
    if(l->size*RF_PTRMEMORYLOC_SIZE >= l->bufferCapacity)
    {
        //if not reallocate to double capacity
        l->bufferCapacity*= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void** testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in adding a pointer to a List of pointers during an attempt to increase buffer capacity. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }//end of capacity increase case

    //give the value of the pointer's memory location to the appropriate member of the list
    *(l->data+l->size) = l->ptr2Copy(object);
    //finally increase the size of the list
    l->size++;
    return true;
}

// Retrieves a pointer to an object from the list
void* rfListP_Get(RF_ListP* l,uint32_t i)
{
    if( i >= l->size)
    {
        LOG_ERROR("Attempted to retrieve an element from a pointer list with an out of bounds index",RE_INDEX_OUT_OF_BOUNDS);
        return 0;
    }

    //return the memory location of the pointer
    return  *(l->data+i);
}


//Sets the pointer of an already existing specific index of the list to the given pointer
char rfListP_Set(RF_ListP* l,uint32_t i,void* e,char free)
{
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to set a list of pointers element with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return false;
    }
    //if we have a function prototype for deletion use it to delete the object
    if(free == true)
        if(l->ptr2Destroy != 0)
        {
            l->ptr2Destroy(*(l->data+i));
        }
    //now make the location point to the address of the new element
    *(l->data+i) = e;
    return true;
}

//Sets the pointer of an already existing specific index of the list to the copy of the given pointer
char rfListP_Set_copy(RF_ListP* l,uint32_t i,void* e,char free)
{
    if(l->ptr2Copy == 0)
        return false;
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to set a list of pointers element with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return false;
    }
    //if we have a function prototype for deletion use it to delete the object
    if(free == true)
        if(l->ptr2Destroy != 0)
        {
            l->ptr2Destroy(*(l->data+i));
        }
    //now make the location point to a copy of e
    *(l->data+i) = l->ptr2Copy(e);
    return true;
}

//Removes a pointer to an object from the list, calling the its destructing function.
void rfListP_Remove(RF_ListP* l, uint32_t i)
{
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to remove a pointer from a pointer list with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }

    //if we have a function prototype for deletion use it to delete the object
    if(l->ptr2Destroy != 0)
    {
        l->ptr2Destroy( *(l->data+i));
    }

    //Move the whole buffer back to cover up for the removed element
    memmove(l->data+i,l->data+i+1,RF_PTRMEMORYLOC_SIZE*(l->size-i-1));
    //decrease size
    l->size--;

    //also check if we can reallocate downwards. Condition is if the current size is less than the fraction of the capacity
    if( l->size*RF_PTRMEMORYLOC_SIZE < l->bufferCapacity/RF_OPTION_LIST_CAPACITY_M)
    {
        l->bufferCapacity/= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void** testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in removing a pointer from a List during an attempt to shrink the buffer. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }
}
//Removes a pointer to an object from the list,without calling the its destructing function.
void rfListP_Remove_nofree(RF_ListP* l, uint32_t i)
{
    if(i >= l->size)
    {
        LOG_ERROR("Attempted to remove a pointer from a pointer list with an index out of bounds",RE_INDEX_OUT_OF_BOUNDS);
        return;
    }

    //Move the whole buffer back to cover up for the removed element
    memmove(l->data+i,l->data+i+1,RF_PTRMEMORYLOC_SIZE*(l->size-i-1));
    //decrease size
    l->size--;

    //also check if we can reallocate downwards. Condition is if the current size is less than the fraction of the capacity
    if( l->size*RF_PTRMEMORYLOC_SIZE < l->bufferCapacity/RF_OPTION_LIST_CAPACITY_M)
    {
        l->bufferCapacity/= RF_OPTION_LIST_CAPACITY_M;
        //not using macro here cause no type is known and gotta do it manually
        void** testPtr = realloc(l->data,l->bufferCapacity);
        if(testPtr)
            l->data = testPtr;
        else
        {
            LOG_ERROR("Realloc failed in removing a pointer from a List during an attempt to shrink the buffer. Aborting program",RE_REALLOC_FAILURE);
            exit(RE_REALLOC_FAILURE);
        }
    }
}

