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
** --Data_Structures/list.h
** This header declares functions that operate on RF_HashMap, a hash 
** map/table data structure
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <String/string_decl.h> //for RF_String
#include <Data_Structures/hashmap_decl.h>
#include <Data_Structures/hasmap.h>
---------------------For internal library include make sure to have----------------------------
*/

/* @omit start */

/*
 * This file is a TEMPLATE. It does not actually get compiled at all
 * anywhere inside the library. Depending on the options you provide
 * in scons though various specializations of this file will get
 * generated during building the library
 */

/* @omit end */

#ifndef RF_HASHMAP_DECL_H
#define RF_HASHMAP_DECL_H

typedef struct RF_Hashslot
{
    RF_String key;
    /* @mutate void* TYPE */
    void*  value;
    struct RF_Hashslot* next;
}RF_Hashslot;

typedef struct RF_Hashmap
{
    RF_Hashslot** slots;
    uint32_t size;
    uint32_t occupied;
    /* @omit start */
    //! The destruction (deinit) function for the values of the map
    void (*ptr2Destroy)(void*);
    //! The copy function for the values of the map
    char (*ptr2Copy)(void*, void*);
    //! The equality function for the value of the map
    char (*ptr2Equal)(void*, void*);
    /* @omit end */
}RF_Hashmap;




#endif//include guards end
