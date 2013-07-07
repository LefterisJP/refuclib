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
** --Data_Structures/hashmap.h
** This header declares functions that operate on RF_Hashmap, a simple
** separate chaining with linked list hash map implementation
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <String/string_decl.h> //for RF_String
#include <Data_Structures/hashmap_decl.h> //for the struct declarations
#include <Data_Structures/hashmap.h>
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

#ifndef RF_HASHMAP_H
#define RF_HASHMAP_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/**
 ** Initializes a hashmap data structure
 ** @param m The hashmap to initialize
 ** @param s The initial size of the map in "buckets"
 ** @return @c true for success and @c false otherwise
 */
i_DECLIMEX_ char rfHashmap_Init(RF_Hashmap* m, uint32_t s);

/**
 ** Allocates and returns a hashmap data structure
 ** @param s The initial size of the map in "buckets"
 ** @return The allocated @ref RF_Hashmap or @c NULL if there is an error
 */
i_DECLIMEX_ RF_Hashmap* rfHashmap_Create(uint32_t s);

/**
 ** Deinitializes a hashmap
 ** @param m The map to deinitialize
 */
i_DECLIMEX_ void rfHashmap_Deinit(RF_Hashmap* m);

/**
 ** Destroys a hashmap
 ** @param m The map to destroy
 */
i_DECLIMEX_ void rfHashmap_Destroy(RF_Hashmap* m);

/**
 ** Insert a value into the Hashmap
 ** @param m The map to insert the @c value into
 ** @param key An @ref RF_String representing the key to associate with
 ** the particular value
 ** @param value The value to insert into the hashmap
 ** @return @c true if all is fine and @c false otherwise
 */
i_DECLIMEX_ char rfHashmap_Insert(RF_Hashmap* m, RF_String* key,
                                  /* @mutate void* TYPEPTR_OBJ_ONLY */
                                  void* value);

/**
 ** Query a value for a key of the hashmap
 ** @param m The map to query
 ** @param key An @ref RF_String with the key to query the map for
 ** @param value If @c key exists in the map then @c value will contain the
 ** value associated with this key.
 ** @return If the @c key exists in the map then @c true will be returned.
 ** Otherwise @c false will be returned
 */
i_DECLIMEX_ char rfHashmap_Get(RF_Hashmap* m, RF_String* key,
                               /* @mutate void* TYPEPTR */
                               void* value);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
