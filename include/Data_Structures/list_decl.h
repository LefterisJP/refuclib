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
#ifndef RF_LIST_DECL_H
#define RF_LIST_DECL_H



/**
** @internal
** @author Lefteris
** @date 10/07/2011
** @endinternal
** @brief A List of  objects (not pointers) of any DATA type that can be created dynamically
**
** Functions to add, remove and retrieve
** object from the list exist.
**
** Normal usage is to set an initial capacity of the list and then whenever more objects
** are added the capacity gets increased if it goes over the buffer and it also gets decreased if
** many objects are removed. So this is a datastructure to use if you need something that grows bigger
** and/or shrinks when needed. For a well managed but immutable(should not change its size dynamically unless specified) data structure
** take a look at @ref RF_ArrayV and @ref RF_ArrayP
**
** @note To store a list of pointers use the @ref RF_ListP structure
**/
typedef struct RF_ListV
{
    //! The data of the group for when we save values
    void* data;
    //! An element's size
    uint32_t elementSize;
    //! The current buffer capacity in bytes
    uint32_t bufferCapacity;
    //! The size of the list in elements (how many elements are actually occupying the list right now). So the next element will go to index = size
    uint32_t size;
    //! A Function pointer for the destruction of the objects
    void (*ptr2Destroy)(void*);
    //! A Function pointer for the copying of the objects
    void (*ptr2Copy)(void*,void*);
}RF_ListV;


/**
** @internal
** @author Lefteris
** @date 10/07/2011
** @endinternal
** @brief A List of pointers to any object DATA type that can be created
** dynamically
**
** Functions to add, remove and retrieve
** object from the group exist. Only the pointers to the objects
** are stored in this type of group.
**
** Normal usage is to set an initial capacity of the list and then whenever more objects
** are added the capacity gets increased if it goes over the buffer and it also gets decreased if
** many objects are removed. So this is a datastructure to use if you need something that grows bigger
** and/or shrinks when needed. For a well managed but immutable(should not change its size dynamically unless specified) data structure
** take a look at @ref RF_ArrayV and @ref RF_ArrayP
**
** @note To store a list of object and not pointers use the @ref RF_ListV structure
**/
typedef struct RF_ListP
{
    //! The data of the group for when we save pointers
    void** data;
    //! The current buffer capacity in bytes
    uint32_t bufferCapacity;
    //! The size of the list in pointers (how many pointers are actually occupying the list right now). So the next pointer will go to index = size
    uint32_t size;
    //! A Function pointer for the destruction of the objects
    void (*ptr2Destroy)(void*);
    //! A Function pointer for the copying of the objects
    void* (*ptr2Copy)(void*);
}RF_ListP;


#endif//include guards end
