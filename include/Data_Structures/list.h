/**
**      ==Start OF REFU LICENSE==
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
** --Data_Structures/list.h
** This header declares functions that operate on RF_List, a single
** linked list data structure
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Data_Structures/linkedlist_decl.h>
#include <Data_Structures/linkedlist.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_LIST_H
#define RF_LIST_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/* @omit start */
/**
 ** @memberof RF_List
 ** @brief Allocates and returns a single linked list of objects
 **
 ** @param ptr2Destroy \rfoptional{The object's Deinit() functions} A
 ** pointer to the destruction function for the objects stored inside the
 ** group. Used for object deletion.
 ** Can be @c 0 which means no destructor is given, so no freeing will
 ** happen when members of the list are removed
 ** @warning Be careful this must be a destruction function that DOES NOT
 ** free the pointer itself since there are only values saved in here.
 ** So it must only be destroying the value of the object. 
 ** Else undefined behaviour is caused.
 ** If omitted then the appropriate  @c objectType_Deinit()
 ** function will be called. If you are making a list of non Refu object 
 ** and omit this argument it will lead to a compilation error.
 ** @param ptr2Copy \rfoptional{The object's Copy_IN function} A pointer
 ** to the copying function for the objects stored inside the list. It is
 ** used for copying the List and also for making copies of the objects
 ** themselves. Can be @c 0 which means that at List copying the copied
 ** list contains a blind data buffer copy of the to copy list. Can be
 ** dangerous for any type of stored object that stores data on the heap.
 ** It is recommended to always provide a copy function. This function
 ** must be in the form of all the functions of the library that Copy
 ** an object from a destination to a src type. For reference look at
 ** @ref rfString_Copy_IN() or rfListV_Copy_IN()
 ** For storing objects of the Refu library this argument can be omitted
 ** and will be given a pointer to the object's @c objectType_Copy_IN()
 ** function. If you are making a list of non Refu object and omit this
 ** argument it will lead to a compilation error.
 ** @return Returns true in succesfull initialization and false otherwise
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
RF_List* rfList_Create(Type objectType,
                                   void (*ptr2Destroy)(void*),
                                   void (*ptr2Copy)(void*, void*),
                                   char (*ptr2Equal)(void*, void*)
);
#else
/* @omit end */

i_DECLIMEX_ RF_List*  rfList_Create(
    /* @omit start */
    void (*ptr2Destroy)(void*),
    void (*ptr2Copy)(void*, void*),
    char (*ptr2Equal)(void*, void*)
    /* @omit end */
);
/* @omit start */
#endif
/**
 ** @memberof RF_List
 ** @brief Initializes a single linked list of objects
 **
 ** @param l the list to initialize
 ** @param ptr2Destroy \rfoptional{The object's Deinit() functions} A
 ** pointer to the destruction function for the objects stored inside the
 ** group. Used for object deletion.
 ** Can be @c 0 which means no destructor is given, so no freeing will
 ** happen when members of the list are removed
 ** @warning Be careful this must be a destruction function that DOES NOT
 ** free the pointer itself since there are only values saved in here.
 ** So it must only be destroying the value of the object. 
 ** Else undefined behaviour is caused.
 ** If omitted then the appropriate  @c objectType_Deinit()
 ** function will be called. If you are making a list of non Refu object 
 ** and omit this argument it will lead to a compilation error.
 ** @param ptr2Copy \rfoptional{The object's Copy_IN function} A pointer
 ** to the copying function for the objects stored inside the list. It is
 ** used for copying the List and also for making copies of the objects
 ** themselves. Can be @c 0 which means that at List copying the copied
 ** list contains a blind data buffer copy of the to copy list. Can be
 ** dangerous for any type of stored object that stores data on the heap.
 ** It is recommended to always provide a copy function. This function
 ** must be in the form of all the functions of the library that Copy
 ** an object from a destination to a src type. For reference look at
 ** @ref rfString_Copy_IN() or rfListV_Copy_IN()
 ** For storing objects of the Refu library this argument can be omitted
 ** and will be given a pointer to the object's @c objectType_Copy_IN()
 ** function. If you are making a list of non Refu object and omit this
 ** argument it will lead to a compilation error.
 ** @return Returns true in succesfull initialization and false otherwise
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
char  rfList_Init(RF_List* l,
                        Type objectType,
                        void (*ptr2Destroy)(void*),
                        void (*ptr2Copy)(void*,void*),
                        char (*ptr2Equal)(void*, void*)
);
#else
/* @omit end */
i_DECLIMEX_ char  rfList_Init(
    RF_List* l
    /* @omit start */
    ,void (*ptr2Destroy)(void*),
    void (*ptr2Copy)(void*, void*),
    char (*ptr2Equal)(void*, void*)
    /* @omit end */
);
/* @omit next */
#endif
/**
 ** @memberof RF_List
 ** @brief Copies RF_List @c src into RF_List @c dst
 **
 ** @param dst The new list copy to create
 ** @param src The list to copy from
 **
 **/
i_DECLIMEX_ void rfList_Copy_IN(RF_List* dst,
                                       RF_List* src);
/**
 ** @memberof RF_List
 ** @brief Creates and returns an allocated copy of the given 
 ** RF_List
 **
 ** @param src The list to copy from
 ** @return An allocated RF_List copy of the given list
 **
 **/
i_DECLIMEX_ RF_List* rfList_Copy_OUT(RF_List* src);

/**
 ** @memberof RF_List
 ** @brief Destroys a single linked list of objects, calling the 
 ** destructing function if it exists
 **
 ** @param a The list to destroy
 **
 **/
i_DECLIMEX_ void rfList_Destroy(RF_List* a);

/**
 ** @memberof RF_List
 ** @brief Deinitializes a single linked list of objects, calling the
 ** destructing function if it exists
 **
 ** @param a The list to destroy
 **
 **/
i_DECLIMEX_ void rfList_Deinit(RF_List* a);


/**
 ** @memberof RF_List
 ** @brief Adds an element to a single linked list
 **
 ** The element gets a deep copy using the copy function passed to the
 ** list.
 **
 ** @param a The list to add to
 ** @param e A pointer to the element to add
 **
 **/
/* @mutate void* TYPEPTR */
i_DECLIMEX_ char rfList_Add(RF_List* a, void* e);

/**
 ** @memberof RF_List
 ** @brief Deletes an element from a single linked list by calling
 ** the destruction and the comparison functions.
 **
 ** @param a The list to delete the element from
 ** @param e A pointer to the element to search for in the list and
 ** if found then delete
 ** @return Returns @c true if the element is found and deleted from the
 ** list and @c false otherwise
 **
 **/
/* @mutate void* TYPEPTR */
i_DECLIMEX_ char rfList_Delete(RF_List* a, void* e);



/**
 ** @memberof RF_List
 ** @brief Searches if an element exists in the list by a deep check
 **
 ** The check used in this function is a deep object comparison using
 ** the given comparison function
 ** @param a The list in which to search for the element
 ** @param e A pointer to the element to search for in the list
 ** @return Returns @c true if the element is found and false otherwise
 **
 **/
/* @mutate void* TYPEPTR */
i_DECLIMEX_ char rfList_Has(RF_List* a, void* ret);

/**
 ** These 2 macros here are used to iterate a linked list from the 
 ** starting node to its end node.
 ** @param i_LIST_ A pointer to the linked list to iterate
 ** @param i_DATA_PTR_ A pointer of the type of data you have stored in
 ** the linked list. In every iteration this pointer will point to the 
 ** current data.
 **/
#define RF_LIST_ITERATE_START(i_LIST_, i_DATA_PTR_)   \
    RF_L_Node* i_anode_ptr_ = i_LIST_->start->next;\
    while(i_anode_ptr_ != 0)\
    {\
    /* @mutate (i_anode_ptr_ REFERENCE */ \
    i_DATA_PTR_ = (i_anode_ptr_->data);

#define RF_LIST_ITERATE_END() \
    i_anode_ptr_ = i_anode_ptr_->next;}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif// end of include guards
