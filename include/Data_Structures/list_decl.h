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
*/

/* @omit start */

/*
 * This file is a TEMPLATE. It does not actually get compiled at all
 * anywhere inside the library. Depending on the options you provide
 * in scons though various specializations of this file will get
 * generated during building the library
 */

/* @omit end */

#ifndef RF_LIST_DECL_H
#define RF_LIST_DECL_H


typedef struct RF_L_Node
{
    struct RF_L_Node* next;
    /* @mutate void* TYPE */
    void* data;
}RF_L_Node;

typedef struct RF_List
{
    //! The first node of the linked list
    struct RF_L_Node* start;
    //! The last node of the linked list
    struct RF_L_Node* end;
    /* @omit start */

    //! An element's size
    uint32_t element_size;
    //! The destruction (deinit) function for the elements of the list
    void (*ptr2Destroy)(void*);
    //! The copy function for the elements of the list
    void (*ptr2Copy)(void*, void*);
    //! The equality function for the elements of the list
    char (*ptr2Equal)(void*, void*);
    /* @omit end */
}RF_List;

#endif//include guards end
