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
/*------------- Corrensponding Header inclusion -------------*/
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>//for fixed size data types
#include <Data_Structures/binaryarray_decl.h> //for RF_BinarryArray
#include <Data_Structures/binaryarray.h>
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <Threads/common.h> //for rfThread_GetID()
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation
/*------------- libc inclusion --------------*/
#include <string.h> //for memcpy
/*------------- End of includes -------------*/

// Allocates and returns a binary array
RF_BinaryArray* rfBinaryArray_Create(uint32_t size)
{
    RF_BinaryArray* ret;
    RF_MALLOC(ret, sizeof(RF_BinaryArray), NULL);
    if(rfBinaryArray_Init(ret, size) == false)
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}
// Initializes a binary array
char rfBinaryArray_Init(RF_BinaryArray* arr,uint32_t size)
{
    //allocate enough bytes to encompass all of the values. Initializing everything to 0
    RF_CALLOC(arr->data, size/8+1, 1, false);
    arr->size = size;
    return true;
}

//Copies RF_BinarryArray @c src into RF_BinaryArray @c dst
char rfBinaryArray_Copy_IN(RF_BinaryArray* dst, RF_BinaryArray* src)
{
    dst->size = src->size;
    RF_CALLOC(dst->data, dst->size/8+1, 1, false);
    memcpy(dst->data, src->data, dst->size/8+1);
    return true;
}
//Creates and returns an allocated copy of the given RF_BinarryArray
RF_BinaryArray* rfBinaryArray_Copy_OUT(RF_BinaryArray* src)
{
    RF_BinaryArray* dst;
    RF_MALLOC(dst, sizeof(RF_BinaryArray), NULL);
    if(rfBinaryArray_Copy_IN(dst, src) == false)
    {
        free(dst);
        dst = NULL;
    }
    return dst;
}

// Destroys a binary array freeing its memory
void rfBinaryArray_Destroy(RF_BinaryArray* a)
{
    free(a->data);
    free(a);
}
// Destroys a binary array but without freeing its memory
void rfBinaryArray_Deinit(RF_BinaryArray* a)
{
    free(a->data);
}

// Gets a specific value of the array
char rfBinaryArray_Get(RF_BinaryArray* a, uint32_t i, char* val)
{
    char b;
    //check for out of bounds index
    if(i >= a->size)
    {
        RF_ERROR(0,
                 "Attempted to retrieve a value from a BinaryArray with an "
                 "index out of bounds");
        return false;
    }

    //the specific bit of the byte we wanna check for
    b = 0x1 << (i%8);
    //check if the bit is set or not
    *val = (b == (a->data[i/8] & b));
    //success
    return true;
}

// Gets a specific value of the array, without checking for array index out of bounds. If the index IS out of bounds the value can not be trusted. For a safer function look at #rfBinaryArray_Get
char rfBinaryArray_Get_NC(RF_BinaryArray* a, uint32_t i)
{
    //the specific bit of the byte we wanna check for
    char b = 0x1 << (i%8);
    //return the value of the bit
    return (b == (a->data[i/8] & b));
}

// Sets a specific value of the binary array.
char rfBinaryArray_Set(RF_BinaryArray* a, uint32_t i, char val)
{
   //check for out of bounds index
   if(i >= a->size)
   {
       RF_ERROR(0,
                "Attempted to set a value of a BinaryArray with an index out "
                "of bounds");
       return false;
   }

    //if the given value is true set the bit
   if(val == true)
   {
      a->data[i/8] |= (0x1 << (i%8));
      return true;
   }

    //else unset the bit
    a->data[i/8] &= ~(0x1 << (i%8));
    return true;
}

// Sets a specific value of the binary array without making an index out of bounds check. Use only if you are certain that the given index is not out of bounds!!!!
void rfBinaryArray_Set_NC(RF_BinaryArray* a, uint32_t i, char val)
{
   //if the given value is true set the bit
   if(val == true)
   {
      a->data[i/8] |= (0x1 << (i%8));
      return ;
   }

    //else unset the bit
    a->data[i/8] &= ~(0x1 << (i%8));
}


// Increases the size of a binary array
char rfBinaryArray_Reallocate(RF_BinaryArray* a, uint32_t newSize)
{
    //attempt to realloc
    RF_REALLOC(a->data, char, newSize/8 + 1, false);
    //check to see if the array got reallocated upwards
    if(newSize > a->size)
    {
        ///if it did we have to initialize all the new values to 0
        unsigned char j;
        unsigned char sBit = a->size%8;
        //get the difference in size
        uint32_t diff = newSize-a->size;

        //set the remaining bits of the byte to 0 (if any)
        for(j=sBit ;j < 8; j ++)
        {
            a->data[a->size/8] &= ~(0x1 << (j%8));
        }

        //find the remaining bits of the byte and check if the difference in size exceeds them
        if(diff > (unsigned char)(8-sBit))
        {
            //then we also have additional bytes which we need to make sure are equal to 0
            for(j = a->size/8 + 1; j <= newSize/8; j++ )
            {
                a->data[j] = 0;
            }
        }
    }

    //finally save the new size
    a->size = newSize;

    return true;
}
