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
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Data_Structures/list_decl.h>
#include <Data_Structures/list.h>
/*------------- Module related inclusion -------------*/
#include "common.ph"//for the common macro def
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation

//for local scope
    #include <Definitions/threadspecific.h> //for the thread specific attributes
    #include <Utils/localmem_decl.h> //for RF_LocalMemoryStack
    #include <stdint.h> //for UINT32_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>
/*------------- libc includes -------------*/
#include <string.h> //for memcpy and memcp
/*------------- End of includes -------------*/


RF_List*  rfList_Create(
    /* @omit start */
    uint32_t elSize,
    void (*ptr2Destroy)(void*),
    char (*ptr2Copy)(void*, void*),
    char (*ptr2Equal)(void*, void*)
    /* @omit end */
)
{
    RF_List* ret;
    //allocate the list
    RF_MALLOC(ret, sizeof(RF_List), NULL);
    if(rfList_Init(ret) == false)
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}


char rfList_Init(
    RF_List* l
    /* @omit start */
    ,uint32_t elSize,
    void (*ptr2Destroy)(void*),
    char (*ptr2Copy)(void*, void*),
    char (*ptr2Equal)(void*, void*)
    /* @omit end */
)
{
    /* @omit start */
    if(elSize == 0)
    {
        RF_ERROR(0,"Tried to create an array of objects with zero sized "
                 "elements");
        return false;
    }
    /* @omit end */
    //allocate the first node
    RF_MALLOC(l->start, sizeof(RF_L_Node), false);
    /* @mutate 0 INITIALIZE_TO_ZERO */
    l->start->data = 0;
    l->start->next = 0;
    l->end = l->start;
    /* @omit start */
    l->element_size = elSize;
    l->ptr2Destroy = ptr2Destroy;
    l->ptr2Copy = ptr2Copy;
    l->ptr2Equal = ptr2Equal;
    /* @omit end */
    return true;
}



char rfList_Copy_IN(RF_List* dst,
                    RF_List* src)
{
    RF_L_Node* n = src->start->next, *c, *pr = 0;
    /* @omit start */
    dst->element_size = src->element_size;
    dst->ptr2Destroy = src->ptr2Destroy;
    dst->ptr2Copy = src->ptr2Copy;
    dst->ptr2Equal = src->ptr2Equal;
    /* @omit end */

    //allocate first node
    RF_MALLOC(c, sizeof(RF_L_Node), false);
    /* @mutate 0 INITIALIZE_TO_ZERO */
    c->data = 0;
    c->next = 0;
    dst->start = c;
    pr = c;
    while(n != 0)
    {
        //allocate a node
        RF_MALLOC(c, sizeof(RF_L_Node), false);
        //copy its data
        /* @omit next */
        RF_MALLOC(c->data, src->element_size, false);
        /* @mutate ptr2Copyrr ASSIGN IFBLOCK return false; */
        src->ptr2Copy(c->data, n->data);
        c->next = 0;
        //connect it to the previous one
        pr->next = c;
        pr = c;
        //keep reading the src
        n = n->next;
    }
    dst->end = c;
    return true;
}

RF_List* rfList_Copy_OUT(RF_List* src)
{
    RF_List* dst;
    RF_MALLOC(dst, sizeof(RF_List), NULL);
    if(rfList_Copy_IN(dst, src) == false)
    {
        free(dst);
        dst = NULL;
    }
    return dst;
}


void rfList_Destroy(RF_List* a)
{
    RF_L_Node* n = a->start->next, *p;
    while(n != 0)
    {
        p = n;
        n = n->next;
        //free this node
        /* @mutate ptr2Destroyr DESTROY */
        a->ptr2Destroy(p->data);
        free(p);
    }
    //finally free start node and the whole list
    free(a->start);
    free(a);
}


void rfList_Deinit(RF_List* a)
{
    RF_L_Node* n = a->start->next, *p;
    while(n != 0)
    {
        p = n;
        n = n->next;
        //free this node
        /* @mutate ptr2Destroyr DESTROY */
        a->ptr2Destroy(p->data);
        free(p);
    }
    //finally free start node
    free(a->start);
}

/* @mutate void* TYPEPTR_OBJ_ONLY */
char rfList_Add(RF_List* a, void* e)
{
    RF_L_Node* n;
    char ret = true;
    /* @omitcond NONLMS */
    RF_ENTER_LOCAL_SCOPE();
    /* @omit end */
    RF_MALLOC_JMP(n, sizeof(RF_L_Node), ret = false, cleanup);
    n->next = 0;
    //add the data to the node
    /* @omit next */
    RF_MALLOC(n->data, sizeof(a->element_size), ret = false, cleanup);

    /* @mutate ptr2Copyrp ASSIGN IFBLOCK ret=false; goto cleanup; */
    a->ptr2Copy(n->data, e);


    //connect the new node to the linked list
    a->end->next = n;
    a->end = n;
/* @omitcond OBJECT */
#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION
/* @omit end */
  cleanup:
/* @omitcond OBJECT */
#endif
/* @omit end */
    /* @omitcond NONLMS */
    RF_EXIT_LOCAL_SCOPE();
    /* @omit end */
    return ret;
}

/* @mutate void* TYPEPTR_OBJ_ONLY */
char rfList_Delete(RF_List* a, void* e)
{
    RF_L_Node* n = a->start->next;
    RF_L_Node* pr = a->start;
    /* @omitcond NONLMS */
    RF_ENTER_LOCAL_SCOPE();
    /* @omit end */
    while(n != 0)
    {
        /* @mutate ptr2Equalpr COMPARE */
        if(a->ptr2Equal(e, n->data))
        {
            if(n == a->end)
            {//if this is the last node
                /* @mutate ptr2Destroyr DESTROY */
                a->ptr2Destroy(n->data);
                pr->next = 0;
                a->end = pr;
                free(n);
                /* @omitcond NONLMS */
                RF_EXIT_LOCAL_SCOPE();
                /* @omit end */
                return true;
            }
            //else it's somewhere in between
            /* @mutate ptr2Destroyr DESTROY */
            a->ptr2Destroy(n->data);
            pr->next = n->next;
            free(n);
            /* @omitcond NONLMS */
            RF_EXIT_LOCAL_SCOPE();
            /* @omit end */
            return true;
        }
        pr = n;
        n = n->next;
    }
    /* @omitcond NONLMS */
    RF_EXIT_LOCAL_SCOPE();
    /* @omit end */
    return false;
}

/* @mutate void* TYPEPTR_OBJ_ONLY */
char rfList_Has(RF_List* a, void* e)
{
    RF_L_Node* n = a->start->next;
    /* @omitcond NONLMS */
    RF_ENTER_LOCAL_SCOPE();
    /* @omit end */
    while(n != 0)
    {
        /* @mutate ptr2Equalpr COMPARE */
        if(a->ptr2Equal(e, n->data))
        {
            /* @omitcond NONLMS */
            RF_EXIT_LOCAL_SCOPE();
            /* @omit end */
            return true;
        }
        n = n->next;
    }while(n != 0);
    /* @omitcond NONLMS */
    RF_EXIT_LOCAL_SCOPE();
    /* @omit end */
    return false;
}
