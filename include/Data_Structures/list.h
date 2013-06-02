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
** --Data_Structures/list.h
** This header declares functions to operate on RF_List
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <Data_Structures/list_decl.h> //for RF_ListP and RF_ListV
#include <Data_Structures/list.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_LIST_H
#define RF_LIST_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
 ** The multiplier by which to increase the size of the buffer of a list if more memory is needed
 **
 **/
#define RF_LIST_CAPACITY_MULTIPLIER  2

/**
 ** @memberof RF_ListV
 ** @brief Allocates and returns a list of objects
 **
 ** @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
 ** @param numElements The number of elements to initially allocate for the list
 ** @param ptr2Destroy \rfoptional{The object's Deinit() functions} A pointer to the destruction function for the objects stored inside the group. Used for object deletion.
 ** Can be @c 0 which means no destructor is given, so no freeing will happen when members of the list are removed
 ** @warning Be careful this must be a destruction function that DOES NOT free the pointer itself since there are
 ** only values saved in here. So it must only be destroying the value of the object. Else undefined behaviour is caused.
 ** If omitted then the appropriate  @c objectType_Deinit()
 ** function will be called. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @param ptr2Copy \rfoptional{The object's Copy_IN function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
 ** List and also for making copies of the objects themselves. Can be @c 0 which means that at List copying the copied
 ** list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
 ** that stores data on the heap. It is recommended to always provide a copy function.
 ** This function must be in the form of all the functions of the library that Copy
 ** an object from a destination to a src type. For reference look at @ref rfString_Copy_IN() or rfListV_Copy_IN()
 ** For storing objects of the Refu library this argument can be omitted and will be given a pointer
 ** to the object's @c objectType_Copy_IN() functio. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @return The newly initialized list or null pointer if the object size was 0
 **
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_ListV*  rfListV_Create(Type objectType,uint32_t numElements,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
#else
i_DECLIMEX_ RF_ListV*  i_rfListV_Create(uint32_t elSize,uint32_t numElements,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
    #define rfListV_Create(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_LISTV_CREATE,4,__VA_ARGS__)
    #define i_NPSELECT_RF_LISTV_CREATE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Create() accepts from 2 to 4 arguments\"")
    #define i_NPSELECT_RF_LISTV_CREATE0(...) RF_SELECT_FUNC(i_SELECT_RF_LISTV_CREATE,__VA_ARGS__)
    #define i_SELECT_RF_LISTV_CREATE4(i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY) \
        i_rfListV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY)
    #define i_SELECT_RF_LISTV_CREATE3(i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_) \
        i_rfListV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
    #define i_SELECT_RF_LISTV_CREATE2(i_TYPE_ ,i_NUMELEMENTS_) \
        i_rfListV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_RF_GET_DEINIT(i_TYPE_),(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
    #define i_SELECT_RF_LISTV_CREATE1(...)  RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Create() accepts from 2 to 4 arguments\"")
    #define i_SELECT_RF_LISTV_CREATE0(...)  RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Create() accepts from 2 to 4 arguments\"")
    #else
        #define rfListV_Create(i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY) \
            i_rfListV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY)
    #endif
#endif


/**
 ** @memberof RF_ListV
 ** @brief Initializes a list of objects
 **
 ** @param list The list to initialize
 ** @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
 ** @param numElements The number of elements to initially allocate for the list
 ** @param ptr2Destroy \rfoptional{The object's Deinit() functions} A pointer to the destruction function for the objects stored inside the group. Used for object deletion.
 ** Can be @c 0 which means no destructor is given, so no freeing will happen when members of the list are removed
 ** @warning Be careful this must be a destruction function that DOES NOT free the pointer itself since there are
 ** only values saved in here. So it must only be destroying the value of the object. Else undefined behaviour is caused.
 ** If omitted then the appropriate  @c objectType_Deinit()
 ** function will be called. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @param ptr2Copy \rfoptional{The object's Copy_IN function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
 ** List and also for making copies of the objects themselves. Can be @c 0 which means that at List copying the copied
 ** list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
 ** that stores data on the heap. It is recommended to always provide a copy function.
 ** This function must be in the form of all the functions of the library that Copy
 ** an object from a destination to a src type. For reference look at @ref rfString_Copy_IN() or rfListV_Copy_IN()
 ** For storing objects of the Refu library this argument can be omitted and will be given a pointer
 ** to the object's @c objectType_Copy_IN() functio. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @return true in case of succesfull initialization and false otherwise
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char  rfListV_Init(RF_ListV* list,Type objectType,uint32_t numElements,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
#else
i_DECLIMEX_ char  i_rfListV_Init(RF_ListV* list,uint32_t elSize,uint32_t numElements,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfListV_Init(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_LISTV_INIT,5,__VA_ARGS__)
        #define i_NPSELECT_RF_LISTV_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Init() accepts from 3 to 5 arguments\"")
        #define i_NPSELECT_RF_LISTV_INIT0(...) RF_SELECT_FUNC(i_SELECT_RF_LISTV_INIT,__VA_ARGS__)
        #define i_SELECT_RF_LISTV_INIT5(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY) \
            i_rfListV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY)
        #define i_SELECT_RF_LISTV_INIT4(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_) \
            i_rfListV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
        #define i_SELECT_RF_LISTV_INIT3(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_) \
            i_rfListV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_RF_GET_DEINIT(i_TYPE_),(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
        #define i_SELECT_RF_LISTV_INIT2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Init() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_LISTV_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Init() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_LISTV_INIT0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListV_Init() accepts from 3 to 5 arguments\"")
    #else
        #define rfListV_Init(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY)  \
            i_rfListV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY)
    #endif
#endif


/**
 ** @memberof RF_ListV
 ** @brief Copies RF_ListV @c src into RF_ListV @c dst
 **
 ** @param dst The new list copy to create
 ** @param src The list to copy from
 **
 **/
i_DECLIMEX_ void rfListV_Copy_IN(RF_ListV* dst,RF_ListV* src);
/**
 ** @memberof RF_ListV
 ** @brief Creates and returns an allocated copy of the given RF_ListV
 **
 ** @param src The list to copy from
 ** @return An allocated RF_ListV copy of the given list
 **
 **/
i_DECLIMEX_ RF_ListV* rfListV_Copy_OUT(RF_ListV* src);

/**
 ** @memberof RF_ListV
 ** @brief Destroys a list of objects releasing their memory and calling the destructing function if it exists
 **
 ** If for some reason you do not want to call the destruction
 ** function of the given objects at list destruction use
 ** @ref rfListV_Destroy_nofree()
 ** @param l The list to destroy
 **
 **/
i_DECLIMEX_ void rfListV_Destroy(RF_ListV* l);
/**
 ** @memberof RF_ListV
 ** @brief Destroys a list of objects without calling individual object destructing function
 **
 ** For a function that deletes the list and calls the appropriate
 ** destruction function for each individual object refer to
 ** @ref rfListV_Destroy()
 ** @param l The list to destroy
 **
 **/
i_DECLIMEX_ void rfListV_Destroy_nofree(RF_ListV* l);
/**
 ** @memberof RF_ListV
 ** @brief Deinitializes a list of objects releasing their memory and calling the destructing function if it exists
 **
 ** If for some reason you do not want to call the destruction
 ** function of the given objects at list deinitialization use
 ** @ref rfListV_Deinit_nofree()
 ** @param l The list to deinitialize
 **
 **/
i_DECLIMEX_ void rfListV_Deinit(RF_ListV* l);
/**
 ** @memberof RF_ListV
 ** @brief Deinitializes a list of objects without calling individual object destructing function
 **
 ** For a function that deinitializes the list and calls the appropriate
 ** destruction function for each individual object refer to
 ** @ref rfListV_Deinit()
 ** @param l The list to deinitialize
 **
 **/
i_DECLIMEX_ void rfListV_Deinit_nofree(RF_ListV* l);

/**
 ** @memberof RF_ListV
 ** @brief Adds a new object to the list
 **
 ** The @c object is added in the list right after the last element and if it does not fit in the buffer, a reallocation happens.
 ** The @c object is just copied as it is inside the already allocated space of @c sizeof(objectType) of the data structure
 ** So basically the object itself is added into the List. No copy is made.
 ** For a version of this function that first creates a copy use @ref rfListV_Add_copy()
 ** @param l The list in which to add the object
 ** @param object The object to add. Should be a pointer to the object
 ** so if you need to add an object on the stack do not forget to use the @c & operator.
 **
 **/
i_DECLIMEX_ void rfListV_Add(RF_ListV* l,void* object);

/**
 ** @memberof RF_ListV
 ** @brief Adds a a copy of an object to the list
 **
 ** A copy of @c object is made using the equivalent copy function
 ** given at list initialization. If no copy function was given the
 ** function returns false. For a simple addition without copying
 ** refer to @ref rfListV_Add()
 ** @param l The list in which to add the object
 ** @param object The object to add. Should be a pointer to the object
 ** so if you need to add an object on the stack do not forget to use the @c & operator.
 **
 **/
i_DECLIMEX_ char rfListV_Add_copy(RF_ListV* l,void* object);

/**
 ** @memberof RF_ListV
 ** @brief Retrieves an object from the list
 **
 ** The object will be placed in the passed parameter @c ret. It must be allocated properly.
 ** Passing incorrect data to the position @c ret position will cause undefined behaviour, probably segmentation fault.
 ** @param l The list from which to retrieve
 ** @param i The index of the element to retrieve
 ** @param ret A pointer to the object you need to populate with the retrieved data
 ** @return RF_SUCCESS if the object was retrieved succesfully and RF_FAILURE if the index is out of bounds
 **
 **/
i_DECLIMEX_ int32_t rfListV_Get(RF_ListV* l,uint32_t i,void* ret);

/**
 ** @memberof RF_ListV
 ** @brief Sets the given objects to a specific index in the list
 **
 ** The @c object is just copied as it is inside the already allocated space of @c sizeof(objectType) of the data structure
 ** So basically the object itself is added into the List. No copy is made.
 ** If you need a function that first makes a copy of @c object refer to @ref rfListV_Set_copy().
 ** This function can't set the location of an unitialized object. This is not how this data structure works.
 ** For a data structure with such functionality that achieves this take a look at @ref RF_ArrayV.
 ** @param l The list for which to set the object
 ** @param i The index of the object to set
 ** @param e A pointer to the object to add.
 ** @param free A boolean value. Give @c true if you want to call the destructing function of the
 ** previous object at @c i or @c false if you don't
 ** @return True if the element got set succesfully and false if index i is over capacity
 **
 **/
i_DECLIMEX_ char rfListV_Set(RF_ListV* l,uint32_t i,void* e,char free);
/**
 ** @memberof RF_ListV
 ** @brief Sets a copy of the given objects to a specific index in the list
 **
 ** A copy of @c object is made using the equivalent copy function
 ** given at list initialization. If no copy function was given the
 ** function returns false. For setting an object without copying
 ** refer to @ref rfListV_Add() .
 ** This function can't set the location of an unitialized object. This is not how this data structure works.
 ** For a data structure with such functionality that achieves this take a look at @ref RF_ArrayV.
 ** @param l The list for which to set the object
 ** @param i The index of the object to set
 ** @param e A pointer to the object to add.
 ** @param free A boolean value. Give @c true if you want to call the destructing function of the
 ** previous object at @c i or @c false if you don't
 ** @return True if the element got set succesfully and false if index i is over capacity or no
 ** copy function has been given to the list.
 **
 **/
i_DECLIMEX_ char rfListV_Set_copy(RF_ListV* l,uint32_t i,void* e,char free);

/**
 ** @memberof RF_ListV
 ** @brief Removes an object from the list.
 **
 ** All objects get moved to make up for the freed up space and if there is too much space left the list gets reallocated downwards.
 ** Also the destructor of the object at index @c i is called if given. For a function
 ** that removes the object without calling the destructor take a look at @ref rfListV_Remove_nofree()
 ** @param l The list from which to remove
 ** @param i The index of the element to remove
 **
 **/
i_DECLIMEX_ void rfListV_Remove(RF_ListV* l, uint32_t i);
/**
 ** @memberof RF_ListV
 ** @brief Removes an object from the list without calling its destroy function
 **
 ** All objects get moved to make up for the freed up space and if there is too much space left the list gets reallocated downwards.
 ** Note that the destructor of the object at index @c i is not called. If you need to call the destructor refer
 ** to the function @ref rfListV_Remove()
 ** @param l The list from which to remove
 ** @param i The index of the element to remove
 **
 **/
i_DECLIMEX_ void rfListV_Remove_nofree(RF_ListV* l, uint32_t i);

//*------------------------------------------------ListP starts--------------------------------------------------------------


/**
 ** @memberof RF_ListP
 ** @brief Allocates and returns a list of pointers to objects
 **
 ** @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
 ** @param numElements The number of elements to allocate initially for the List
 ** @param ptr2Destroy \rfoptional{The object's destruction function} A pointer to the destruction function for the elements stored inside the list.
 ** Used for object deletion. Can also be @c 0 which means no destruction of objects will happen. They will simply be removed
 ** from the list of pointers. This argument can only be omitted for objects of the refu library. If omitted then the appropriate
 ** @c objectType_Destroy() function will be called. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @param ptr2Copy \rfoptional{The object's Copy_OUT function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
 ** List and also making copies of the objects themselves.  Can also be @c 0 which means that at List copying the copied
 ** list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
 ** that stores data on the heap. It is recommended to always provide a copy function.
 ** This function must be in the form of all the functions of the library that return an allocated copy of an object
 ** from a destination of the same object type. For reference look at @ref rfString_Copy_OUT() or rfListV_Copy_OUT()
 ** For storing objects of the Refu library this argument can be omitted and will be given a pointer
 ** to the object's @c objectType_Copy_OUT() function. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @return The newly initialized group or null pointer if the object size was 0
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_ListP*   rfListP_Create(Type objectType,uint32_t numElements,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
#else
i_DECLIMEX_ RF_ListP*   i_rfListP_Create(uint32_t numElements,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfListP_Create(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_LISTP_CREATE,4,__VA_ARGS__)
        #define i_NPSELECT_RF_LISTP_CREATE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Create() accepts from 2 to 4 arguments\"")
        #define i_NPSELECT_RF_LISTP_CREATE0(...) RF_SELECT_FUNC(i_SELECT_RF_LISTP_CREATE,__VA_ARGS__)
        #define i_SELECT_RF_LISTP_CREATE4(i_TYPE_,i_NUM_ELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY_)   \
            i_rfListP_Create(i_NUM_ELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
        #define i_SELECT_RF_LISTP_CREATE3(i_TYPE_,i_NUM_ELEMENTS_,i_PTR2DESTROY_)  \
            i_rfListP_Create(i_NUM_ELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define i_SELECT_RF_LISTP_CREATE2(i_TYPE_,i_NUM_ELEMENTS_)  \
            i_rfListP_Create(i_NUM_ELEMENTS_,(void(*)(void*))i_RF_GET_DESTROY(i_TYPE_) ,(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define  i_SELECT_RF_LISTP_CREATE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Create() accepts from 2 to 4 arguments\"")
        #define  i_SELECT_RF_LISTP_CREATE0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Create() accepts from 2 to 4 arguments\"")
    #else
        #define rfListP_Create(i_NUM_ELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY_)  i_rfListP_Create(i_NUM_ELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
    #endif
#endif

/**
 ** @memberof RF_ListP
 ** @brief Initialises a list of pointers to objects
 **
 ** @param list The list to initialize
 ** @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
 ** @param numElements The number of elements to allocate initially for the List
 ** @param ptr2Destroy \rfoptional{The object's Destroy() function} A pointer to the destruction function for the elements stored inside the list.
 ** Used for object deletion. Can also be @c 0 which means no destruction of objects will happen. They will simply be removed
 ** from the list of pointers. This argument can only be omitted for objects of the refu library. If omitted then the appropriate
 ** @c objectType_Destroy() function will be called. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @param ptr2Copy \rfoptional{The object's Copy_OUT() function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
 ** List and also making copies of the objects themselves.  Can also be @c 0 which means that at List copying the copied
 ** list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
 ** that stores data on the heap. It is recommended to always provide a copy function.
 ** This function must be in the form of all the functions of the library that return an allocated copy of an object
 ** from a destination of the same object type. For reference look at @ref rfString_Copy_OUT() or rfListV_Copy_OUT()
 ** For storing objects of the Refu library this argument can be omitted and will be given a pointer
 ** to the object's @c objectType_Copy_OUT() function. If you are making a list of non Refu object and omit this argument
 ** it will lead to a compilation error.
 ** @return true for success and false for falure
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char   rfListP_Init(RF_ListP* list,Type objectType,uint32_t numElements,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
#else
i_DECLIMEX_ char   i_rfListP_Init(RF_ListP* list,uint32_t numElements,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfListP_Init(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_LISTP_INIT,5,__VA_ARGS__)
        #define i_NPSELECT_RF_LISTP_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Init() accepts from 3 to 5 arguments\"")
        #define i_NPSELECT_RF_LISTP_INIT0(...) RF_SELECT_FUNC(i_SELECT_RF_LISTP_INIT,__VA_ARGS__)
        #define i_SELECT_RF_LISTP_INIT5(i_LIST_,i_TYPE_,i_NUM_ELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY_)   \
            i_rfListP_Init(i_LIST_,i_NUM_ELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
        #define i_SELECT_RF_LISTP_INIT4(i_LIST_,i_TYPE_,i_NUM_ELEMENTS_,i_PTR2DESTROY_)   \
            i_rfListP_Init(i_LIST_,i_NUM_ELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define i_SELECT_RF_LISTP_INIT3(i_LIST_,i_TYPE_,i_NUM_ELEMENTS_)   \
            i_rfListP_Init(i_LIST_,i_NUM_ELEMENTS_,(void(*)(void*))i_RF_GET_DESTROY(i_TYPE_),(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
         #define i_SELECT_RF_LISTP_INIT2(...)   RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Init() accepts from 3 to 5 arguments\"")
         #define i_SELECT_RF_LISTP_INIT1(...)   RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Init() accepts from 3 to 5 arguments\"")
         #define i_SELECT_RF_LISTP_INIT0(...)   RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfListP_Init() accepts from 3 to 5 arguments\"")
    #else
        #define rfListP_Init(i_LIST_,i_TYPE_,i_NUM_ELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY_) i_rfListP_Init(i_LIST_,i_NUM_ELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
    #endif
#endif

/**
 ** @memberof RF_ListP
 ** @brief Copies RF_ListP @c src into RF_ListP @c dst
 **
 ** Note that since this create a copy of a list of pointers, the pointers
 ** themselves are the same. So there is always the danger of destroying/deiniting
 ** one list and the other list becoming unusable. Use with caution
 ** @param dst The new list copy to create
 ** @param src The list to copy from
 **
 **/
i_DECLIMEX_ void rfListP_Copy_IN(RF_ListP* dst,RF_ListP* src);
/**
 ** @memberof RF_ListP
 ** @brief Creates and returns an allocated copy of the given RF_ListP
 **
 ** Note that since this create a copy of a list of pointers, the pointers
 ** themselves are the same. So there is always the danger of destroying/deiniting
 ** one list and the other list becoming unusable. Use with caution
 ** @param src The list to copy from
 ** @return An allocated RF_ListP copy of the given list
 **
 **/
i_DECLIMEX_ RF_ListP* rfListP_Copy_OUT(RF_ListP* src);

/**
 ** @memberof RF_ListP
 ** @brief Destroys a list of pointers calling the destructing functions of the objects they point to
 **
 ** If you don't want to call the destructor of the object itself
 ** call the @ref rfListP_Destroy_nofree version of the function
 ** @param l The list to destroy
 **
 **/
i_DECLIMEX_ void rfListP_Destroy(RF_ListP* l);
/**
 ** @memberof RF_ListP
 ** @brief Destroys a list of pointers without calling the destructing functions of the obejcts they point to
 **
 ** The objects the pointers point to are not disturbed.
 ** If you want to also delete the objects pointed by the
 ** pointers call @ref rfListP_Destroy()
 ** @param l The List to destroy
 **
 **/
i_DECLIMEX_ void rfListP_Destroy_nofree(RF_ListP* l);
/**
 ** @memberof RF_ListP
 ** @brief Deinitializes a list of pointers, calling the destructing functions of the objects they point to
 **
 ** If you don't want to call the destructing function for each object of the list
 ** call the @ref rfListP_Deinit_nofree() version of the function
 ** @param l The list to destroy
 **
 **/
i_DECLIMEX_ void rfListP_Deinit(RF_ListP* l);
/**
 ** @memberof RF_ListP
 ** @brief Deinitializes a list of pointers without calling the destructing functions of the objects they point to
 **
 ** The objects the pointers point to are not disturbed.
 ** If you also need to  delete the objects pointed to by the
 ** pointers call @ref rfListP_Deinit()
 ** @param l The List to destroy
 **
 **/
i_DECLIMEX_ void rfListP_Deinit_nofree(RF_ListP* l);


/**
 ** @memberof RF_ListP
 ** @brief Adds a new pointer to the list
 **
 ** The pointer is added right after the last pointer and if it does not fit in the buffer a reallocation happens
 ** The pointer itself is added to the list. If you need to first copy the object the pointer points to
 ** and then add a pointer of that copy to the list use the function @ref rfListP_Add_copy().
 ** @param l The list in which to add the object pointer
 ** @param object The object to add. Should be a pointer.
 **
 **/
i_DECLIMEX_ void rfListP_Add(RF_ListP* l,void* object);
/**
 ** @memberof RF_ListP
 ** @brief Adds a pointer to the copy of the object pointed to by @c object
 **
 ** The pointer to the copy is added right after the last pointer and if it does not fit in the buffer a reallocation happens
 ** If you require to add the poitner itself and not a copy use @ref rfListP_Add()
 ** @param l The list in which to add the object pointer
 ** @param object The object to add. Should be a pointer.
 ** @return Returns false if no copy function has been provided to the list. Else returns true.
 **
 **/
i_DECLIMEX_ char rfListP_Add_copy(RF_ListP* l,void* object);

/**
 ** @memberof RF_ListP
 ** @brief Retrieves a pointer to an object from the list
 **
 ** @param l The list from which to retrieve
 ** @param i The index of the element to retrieve
 ** @return A pointer to the element or null pointer if the element's index is out of bounds
 **
 **/
i_DECLIMEX_ void* rfListP_Get(RF_ListP* l,uint32_t i);

/**
 ** @memberof RF_ListP
 ** @brief Sets the pointer of an already existing specific index of the list to the given pointer
 **
 ** Set the pointer at @c i to the pointer passed at @c e. This does not make a copy of the object pointed to
 ** by @c e so the pointer itself is entered into the function.
 ** If there was already a pointer in its position then its destructing function is called
 ** before the new pointer overwrites its position. If you don't want this to happen call the @ref rfListP_Set_nofree() version of the function
 ** This function can't set the location of an unitialized pointer. This is not how this data structure works.
 ** For such functionality refer to @ref RF_ArrayP
 ** @param l The list for which to set the element
 ** @param i The index of the pointer to set
 ** @param e The pointer to add
 ** @param free A boolean value. Give @c true if you want to call the destructing function of the
 ** object pointed to by the pointer at index @c i of the list or @c false if you don't
 ** @return True if the element got set succesfully and false if index i is out of bounds
 **
 **/
i_DECLIMEX_ char rfListP_Set(RF_ListP* l,uint32_t i,void* e,char free);
/**
 ** @memberof RF_ListP
 ** @brief Sets the pointer of an already existing specific index of the list to the copy of the given pointer
 **
 ** If there was already an element at the previous position it is removed but the location it is pointing to <b>IS NOT</b> destroyed.
 ** For a function that also calls the destroying function call @ref rfListP_Set()
 ** This function can't set the location of an unitialized pointer. This is not how this data structure works.
 ** For such functionality refer to @ref RF_ArrayP
 ** @param l The list for which to set the element
 ** @param i The index of the pointer to set
 ** @param e The pointer to add
 ** @param free A boolean value. Give @c true if you want to call the destructing function of the
 ** object pointed to by the pointer at index @c i of the list or @c false if you don't
 ** @return True if the element got set succesfully and false if index i is over capacity or if no copy function was given at initialization
 **
 **/
i_DECLIMEX_ char rfListP_Set_copy(RF_ListP* l,uint32_t i,void* e,char free);

/**
 ** @memberof RF_ListP
 ** @brief Removes a pointer to an object from the list, calling the its destructing function.
 **
 ** The object that the pointer points to is also destroyed. If you don't want that to
 **  happen call the @ref rfListP_Remove_nofree version of the function
 ** @param l The list from which to remove
 ** @param i The index of the pointer to remove
 **
 **/
i_DECLIMEX_ void rfListP_Remove(RF_ListP* l, uint32_t i);
/**
 ** @memberof RF_ListP
 ** @brief Removes a pointer to an object  from the list, without calling its destructing function.
 **
 ** The object that the pointer points to is <b>NOT</b> destroyed.
 ** For a function that also destroys it look at @ref rfListP_Remove().
 ** @param l The list from which to remove
 ** @param i The index of the element to remove
 **
 **/
i_DECLIMEX_ void rfListP_Remove_nofree(RF_ListP* l, uint32_t i);


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif //end of include guards
