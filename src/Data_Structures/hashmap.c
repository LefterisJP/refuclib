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

/* @omit start */

/*
 * This file is a TEMPLATE. It does not actually get compiled at all
 * anywhere inside the library. Depending on the options you provide
 * in scons though various specializations of this file will get
 * generated during building the library
 */

/* @omit end */

/*------------- Corrensponding Header inclusion -------------*/
/* @omit cond STRING */
#include <Definitions/imex.h> //for import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Definitions/types.h> //for fixed size data types
#include <String/string_decl.h> //for RF_String
/* @omit end */
#include <Definitions/retcodes.h> //for error codes, bool
#include <Data_Structures/hashmap_decl.h> //for the struct declarations
#include <Data_Structures/hashmap.h> 
/*------------- Module related inclusion -------------*/

/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Utils/memory.h> //for refu memory allocation

//for local scope
    #include <string.h> //for memset used by RF_ENTER_LOCAL_SCOPE
    #include <Definitions/threadspecific.h> //for the thread specific attributes
    #include <Utils/localmem_decl.h> //for RF_LocalMemoryStack
    #include <stdint.h> //for UINT32_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>
// for FLCMP macros
    #include <float.h>
    #include <Utils/constcmp.h>
// String module inclusions
/* @omit cond STRING */
    #include <String/core.h> //for rfString_Equal
/* @omit end */
    #include <String/conversion.h> //for rfString_Cstr() 
    #include <String/retrieval.h> //for rfString_Length()
/*------------- libc includes -------------*/
#include <stdint.h> //used by hsieh's hash
/*------------- End of includes -------------*/

// This hash function is taken from here
// http://www.azillionmonkeys.com/qed/hash.html
static inline uint32_t HsiehHash(const char* data, int len)
{
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
    || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                      +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}


static char i_InsertValue_Copy(RF_Hashslot** slots, RF_String* key,
                                       bool* exists,
/* @mutate void* TYPEPTR_OBJ_ONLY */
                          void* value, uint32_t size
/* @omit start */
                          ,uint32_t object_size
                          ,char (*ptr2Copy)(void*, void*),
                          void (*ptr2Destroy)(void*)
                          
/* @omit end */
    )
{
    uint32_t i;
    RF_Hashslot* s, *pr;

    *exists = false;
    i = HsiehHash(rfString_Cstr(key), rfString_Length(key));
    i = i % size;
    s = slots[i];

    //if the slot is null initialize it
    if(s == NULL)
    {
        RF_MALLOC(s, sizeof(RF_Hashslot), false);
        slots[i] = s;
/* @omit start */
        // we have to allocate the value for the generic case
        RF_MALLOC(s->value, object_size, false);
/* @omit end */
        s->next = NULL;
    }
    else
    {
        while(s != NULL)
        {
            // if key already exist in the table
            if(rfString_Equal(&s->key, key))
            {
                *exists = true;
                return false;
            }
            pr = s;
            s = s->next;
        }
        //if we get here it means we need to allocate a new slot
        RF_MALLOC(s, sizeof(RF_Hashslot), false);
        pr->next = s;
/* @omit start */
        // we have to allocate the value for the generic case
        RF_MALLOC(s->value, object_size, false);
/* @omit end */
        s->next = NULL;
    }
    //finaly insert the key-value combo
    //copy key
    if(rfString_Copy_IN(&s->key, key) == false)
    {
        return false;
    }
    //copy value
    /* @mutate ptr2Copyrp ASSIGN IFBLOCK return false; */
    ptr2Copy(s->value, value);
    return true;
}


static char i_InsertValue(RF_Hashslot** slots, RF_String* key,
                          bool* exists,
/* @mutate void* TYPEPTR_OBJ_ONLY */
                          void* value, uint32_t size
/* @omit start */
                          ,uint32_t object_size
/* @omit end */
    )
{
    uint32_t i;
    RF_Hashslot* s, *pr;
    
    *exists = false;
    i = HsiehHash(rfString_Cstr(key), rfString_Length(key));
    i = i % size;
    s = slots[i];

    //if the slot is null initialize it
    if(s == NULL)
    {
        RF_MALLOC(s, sizeof(RF_Hashslot), false);
        slots[i] = s;
/* @omit start */
        // we have to allocate the value for the generic case
        RF_MALLOC(s->value, object_size, false);
/* @omit end */
        s->next = NULL;
    }
    else
    {
        while(s != NULL)
        {
            // if key already exists in the table
            if(rfString_Equal(&s->key, key))
            {
                *exists = true;
                return false;
            }
            pr = s;
            s = s->next;
        }
        //if we get here it means we need to allocate a new slot
        RF_MALLOC(s, sizeof(RF_Hashslot), false);
        pr->next = s;
/* @omit start */
        // we have to allocate the value for the generic case
        RF_MALLOC(s->value, object_size, false);
/* @omit end */
        s->next = NULL;
    }
    //finaly insert the key-value combo
    //copy key
    if(rfString_Copy_IN(&s->key, key) == false)
    {
        return false;
    }
    //shallow copy
    /* @mutate memcpy MEMCPY */
    memcpy(&s->value, value, object_size);
    return true;
}

static void i_DestroySlots_Copy(RF_Hashslot** slots, uint32_t size 
                           /* @omit start */
                           ,void (*ptr2Destroy)(void*)
                           /* @omit end */
)
{
    int i;
    RF_Hashslot* s,*tmp;
    for(i = 0;i < size;i++)
    {
        s = slots[i];
        while(s != NULL)
        {
            tmp = s;
            s = s->next;
            rfString_Deinit(&tmp->key);
            /* @mutate ptr2Destroyr DESTROY */
            ptr2Destroy(tmp->value);
            free(tmp);
        }
    }
}

static void i_DestroySlots(RF_Hashslot** slots, uint32_t size)
{
    int i;
    RF_Hashslot* s,*tmp;
    for(i = 0;i < size;i++)
    {
        s = slots[i];
        while(s != NULL)
        {
            tmp = s;
            s = s->next;
            rfString_Deinit(&tmp->key);
            free(tmp);
        }
    }
}

// Initializes a hashmap
char rfHashmap_Init(RF_Hashmap* m, uint32_t s
/* @omit start */
                    ,uint32_t object_size
                    ,char (*ptr2Copy)(void*, void*)
                    ,void (*ptr2Destroy)(void*)
/* @omit end */
)
{
    int i;
    m->size = s;
    m->occupied = 0;
/* @omit start */
    m->object_size = object_size;
    m->ptr2Copy = ptr2Copy;
    m->ptr2Destroy = ptr2Destroy;
/* @omit end */
    RF_MALLOC(m->slots, sizeof(RF_Hashslot*)*m->size, false);
    for(i = 0; i < m->size; i++)
    {
        m->slots[i] = NULL;
    }
    return true;
}

// Allocates and returns a hashmap
RF_Hashmap* rfHashmap_Create(uint32_t s
/* @omit start */
                             ,uint32_t object_size
                             ,char (*ptr2Copy)(void*, void*)
                             ,void (*ptr2Destroy)(void*)
/* @omit end */
)
{
    RF_Hashmap* m;
    RF_MALLOC(m, sizeof(RF_Hashmap), NULL);
    if(! rfHashmap_Init(m, s
/* @omit start */
                        ,object_size
                        ,ptr2Copy
                        ,ptr2Destroy
/* @omit end */
))
    {
        free(m);
        return NULL;
    }
    return m;
}


// Deinitializes a hashmap
void rfHashmap_Deinit_Copy(RF_Hashmap* m)
{
    i_DestroySlots_Copy(m->slots, m->size
/* @omit start */
                   ,m->ptr2Destroy
/* @omit end */
);
}
// Destroys a hashmap
void rfHashmap_Destroy_Copy(RF_Hashmap* m)
{
    rfHashmap_Deinit_Copy(m);
    free(m);
}

// Deinitializes a hashmap
void rfHashmap_Deinit(RF_Hashmap* m)
{
    i_DestroySlots(m->slots, m->size);
}
// Destroys a hashmap
void rfHashmap_Destroy(RF_Hashmap* m)
{
    rfHashmap_Deinit(m);
    free(m);
}

// Insert a copy of a value into the Hashmap
char rfHashmap_Insert_Copy(RF_Hashmap* m, RF_String* key,
                           /* @mutate void* TYPEPTR_OBJ_ONLY */
                           void* value,
                           bool* exists)
{
    uint32_t i;
    RF_Hashslot* s;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();

    if(i_InsertValue_Copy(m->slots, key, exists, value, m->size
/* @omit start */
                     ,m->object_size, m->ptr2Copy, m->ptr2Destroy
/* @omit end */
) == false)
    {
        ret = false;
        goto cleanup;
    }
    m->occupied ++;

    //if the ratio of occupied over size is greaten than the load factor
    if( FLCMP_G((float)m->occupied / (float)m->size,
            RF_OPTION_HASHMAP_LOAD_FACTOR))
    {
        uint32_t prsize;
        RF_Hashslot** tmp;
        //make a new hash map with double the size
        prsize = m->size;
        m->size *= 2;
        RF_MALLOC_JMP(tmp, sizeof(RF_Hashslot*)*m->size,
                      ret=false, cleanup);
        for(i = 0; i < m->size; i ++)
        {
            tmp[i] = NULL;
        }
        //rehash all the values
        for(i = 0; i < prsize; i ++)
        {
            if(m->slots[i] != 0)
            {   //if there is value in the slot
                //enter the value and all of its linked descendants
                s = m->slots[i];
                while(s != 0)
                {
                    /* @mutate s->value REFERENCE */
                    if(!i_InsertValue_Copy(tmp, &s->key, exists, s->value,
                                           m->size
/* @omit start */
                                      ,m->object_size, m->ptr2Copy, m->ptr2Destroy
/* @omit end */
                           ))
                    {
                        ret = false;
                        goto cleanup;
                    }
                    s = s->next;
                }
            }
        }
        //replace the current slots array with the rehashed one and free
        i_DestroySlots_Copy(m->slots, prsize
/* @omit start */
                       ,m->ptr2Destroy
/* @omit end */
);
        free(m->slots);
        m->slots = tmp;
    }    

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


// Insert a value into the Hashmap
char rfHashmap_Insert(RF_Hashmap* m, RF_String* key,
                      /* @mutate void* TYPEPTR_OBJ_ONLY */
                      void* value,
                      bool* exists)
{
    uint32_t i;
    RF_Hashslot* s;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();

    if(i_InsertValue(m->slots, key, exists, value, m->size
/* @omit start */
                     ,m->object_size
/* @omit end */
) == false)
    {
        ret = false;
        goto cleanup;
    }
    m->occupied ++;

    //if the ratio of occupied over size is greaten than the load factor
    if( FLCMP_G((float)m->occupied / (float)m->size,
            RF_OPTION_HASHMAP_LOAD_FACTOR))
    {
        uint32_t prsize;
        RF_Hashslot** tmp;
        //make a new hash map with double the size
        prsize = m->size;
        m->size *= 2;
        RF_MALLOC_JMP(tmp, sizeof(RF_Hashslot*)*m->size,
                      ret=false, cleanup);
        for(i = 0; i < m->size; i ++)
        {
            tmp[i] = NULL;
        }
        //rehash all the values
        for(i = 0; i < prsize; i ++)
        {
            if(m->slots[i] != 0)
            {   //if there is value in the slot
                //enter the value and all of its linked descendants
                s = m->slots[i];
                while(s != 0)
                {
                    /* @mutate s->value REFERENCE */
                    if(!i_InsertValue(tmp, &s->key, exists, s->value,
                                      m->size
/* @omit start */
                                      ,m->object_size
/* @omit end */
                           ))
                    {
                        ret = false;
                        goto cleanup;
                    }
                    s = s->next;
                }
            }
        }
        //replace the current slots array with the rehashed one and free
        i_DestroySlots(m->slots, prsize);
        free(m->slots);
        m->slots = tmp;
    }    

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


char rfHashmap_Get(RF_Hashmap* m, RF_String* key,
                   /* @mutate void* TYPEPTR */
                   void* value)
{
    uint32_t i;
    RF_Hashslot* s;
    char ret = false;
    RF_ENTER_LOCAL_SCOPE();
    i = HsiehHash(rfString_Cstr(key), rfString_Length(key));
    i = i % m->size;
    s = m->slots[i];

    while(s != NULL)
    {
        if(rfString_Equal(&s->key, key))
        {//if key is found get it

            /* @mutate m->object_size SIZE */
            memcpy(value, &s->value, m->object_size);
            ret = true;
            goto cleanup;
        }
        //else proceed in the linked list chain
        s = s->next;
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

char rfHashmap_Get_Copy(RF_Hashmap* m, RF_String* key,
                        /* @mutate void* TYPEPTR */
                        void* value)
{
    uint32_t i;
    RF_Hashslot* s;
    char ret = false;
    RF_ENTER_LOCAL_SCOPE();
    i = HsiehHash(rfString_Cstr(key), rfString_Length(key));
    i = i % m->size;
    s = m->slots[i];

    while(s != NULL)
    {
        if(rfString_Equal(&s->key, key))
        {//if key is found get it

            /* @mutate ptr2Copypr ASSIGN PODPTR POD_DL IFBLOCK goto cleanup; */
            m->ptr2Copy(value, s->value);

            ret = true;
            goto cleanup;
        }
        //else proceed in the linked list chain
        s = s->next;
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
