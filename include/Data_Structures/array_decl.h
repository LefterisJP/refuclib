/*
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
*/
#ifndef RF_ARRAY_DECL_H
#define RF_ARRAY_DECL_H


/**
** @internal
** @author Lefteris
** @date 10/07/2011
** @endinternal
** @brief An array of  objects (not pointers) of any DATA type.
**
** The array has a certain size, and elements are set or not set.
** Functions to set, remove and retrieve
** objects from the array exist.
**
** Normal usage is to set an initial size of the array and then set which objects you want.
** If there is a need to increase/decrease the size of the array there is a reallocation method but
** has to be called explicitly. For a data structure that does all these dynamically look
** at RF_ListP and RF_ListV
**
** To store an array of pointers use the #RF_ArrayP structure
**/
typedef struct RF_ArrayV
{
    //! The data of the group for when we save values
    void* data;
    //! An element's size
    uint32_t elementSize;
    //! The size of the array in elements
    uint32_t size;
    //! A binary array to keep which elements are set and which are not
    RF_BinaryArray map;
    //! A Function pointer for the destruction of the objects
    void (*ptr2Destroy)(void*);
    //! A Function pointer for the copying of the objects
    void (*ptr2Copy)(void*,void*);
}RF_ArrayV;


/**
** @internal
** @author Lefteris
** @date 10/07/2011
** @endinternal
** @brief An array of  pointers to objects of any DATA type.
**
** The array has a certain size, and elements are set or not set.
** Functions to set, remove and retrieve
** pointers from the array exist.
**
** Normal usage is to set an initial size of the array and then set which pointers you want.
** If there is a need to increase/decrease the size of the array there is a reallocation method but
** has to be called explicitly. For a data structure that does all these dynamically look
** at RF_ListP and RF_ListV
**
** @note To store an array of objects and not pointers use the #RF_ArrayV structure
**/
typedef struct RF_ArrayP
{
    //! The data of the group for when we save pointers
    void** data;
    //! The current size of the array in Element pointers
    uint32_t size;
    //! A binary array to keep which pointers are set and which are not
    RF_BinaryArray map;
    //! A Function pointer for the destruction of the objects
    void (*ptr2Destroy)(void*);
    //! A Function pointer for the copying of the objects
    void* (*ptr2Copy)(void*);
}RF_ArrayP;


#endif//include guards end
