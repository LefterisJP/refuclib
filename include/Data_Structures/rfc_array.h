/**
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
**/


#ifndef RF_ARRAY_H
#define RF_ARRAY_H

#include <rf_options.h>
#ifdef RF_MODULE_DS_ARRAY // Array data structure module check

#include <rf_setup.h>
#include "rfc_binaryarray.h"

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//for the defined library tokens
#include <Preprocessor/rf_tokens.h>

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


//! @memberof RF_ArrayV
//! @brief Allocates and returns an array of objects
//!
//! @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
//! @param size The number of object that the array will initially store
//! @param ptr2Destroy \rfoptional{The object's Deinit() functions} A pointer to the destruction function for the objects stored inside the group. Used for object deletion.
//! Can be @c 0 which means no destructor is given, so no freeing will happen when members of the list are removed
//! @warning Be careful this must be a destruction function that DOES NOT free the pointer itself since there are
//! only values saved in here. So it must only be destroying the value of the object. Else undefined behaviour is caused.
//! If omitted then the appropriate  @c objectType_Deinit()
//! function will be called. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @param ptr2Copy \rfoptional{The object's Copy_IN function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
//! List and also for making copies of the objects themselves. Can be @c 0 which means that at List copying the copied
//! list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
//! that stores data on the heap. It is recommended to always provide a copy function.
//! This function must be in the form of all the functions of the library that Copy
//! an object from a destination to a src type. For reference look at @ref rfString_Copy_IN() or rfListV_Copy_IN()
//! For storing objects of the Refu library this argument can be omitted and will be given a pointer
//! to the object's @c objectType_Copy_IN() functio. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @return The newly initialized array or null pointer if there was an error
#ifdef  RF_IAMHERE_FOR_DOXYGEN
RF_ArrayV* rfArrayV_Create(Type objectType,uint32_t size,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
#else
i_DECLIMEX_ RF_ArrayV*  i_rfArrayV_Create(uint32_t elSize,uint32_t size,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfArrayV_Create(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_ARRAYV_CREATE,4,__VA_ARGS__)
        #define i_NPSELECT_RF_ARRAYV_CREATE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Create() accepts from 2 to 4 arguments\"")
        #define i_NPSELECT_RF_ARRAYV_CREATE0(...) RF_SELECT_FUNC(i_SELECT_RF_ARRAYV_CREATE,__VA_ARGS__)
        #define i_SELECT_RF_ARRAYV_CREATE4(i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY) \
            i_rfArrayV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY)
        #define i_SELECT_RF_ARRAYV_CREATE3(i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_) \
            i_rfArrayV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
        #define i_SELECT_RF_ARRAYV_CREATE2(i_TYPE_ ,i_NUMELEMENTS_) \
            i_rfArrayV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_RF_GET_DEINIT(i_TYPE_),(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
        #define i_SELECT_RF_ARRAYV_CREATE1(...)  RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Create() accepts from 2 to 4 arguments\"")
        #define i_SELECT_RF_ARRAYV_CREATE0(...)  RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Create() accepts from 2 to 4 arguments\"")
    #else
        #define rfArrayV_Create(i_TYPE_,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY_)  \
            i_rfArrayV_Create(sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY_)
    #endif
#endif


//! @memberof RF_ArrayV
//! @brief Initializes an array of objects
//!
//! @param arr The array to initialize
//! @param ptr2Destroy \rfoptional{The object's Deinit() functions} A pointer to the destruction function for the objects stored inside the group. Used for object deletion.
//! Can be @c 0 which means no destructor is given, so no freeing will happen when members of the list are removed
//! @warning Be careful this must be a destruction function that DOES NOT free the pointer itself since there are
//! only values saved in here. So it must only be destroying the value of the object. Else undefined behaviour is caused.
//! If omitted then the appropriate  @c objectType_Deinit()
//! function will be called. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @param ptr2Copy \rfoptional{The object's Copy_IN function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
//! List and also for making copies of the objects themselves. Can be @c 0 which means that at List copying the copied
//! list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
//! that stores data on the heap. It is recommended to always provide a copy function.
//! This function must be in the form of all the functions of the library that Copy
//! an object from a destination to a src type. For reference look at @ref rfString_Copy_IN() or rfListV_Copy_IN()
//! For storing objects of the Refu library this argument can be omitted and will be given a pointer
//! to the object's @c objectType_Copy_IN() functio. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @return Returns true in succesfull initialization and false otherwise
#ifdef RF_IAMHERE_FOR_DOXYGEN
char  rfArrayV_Init(RF_ArrayV* arr,Type objectType,uint32_t size,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
#else
i_DECLIMEX_ char  i_rfArrayV_Init(RF_ArrayV* arr,uint32_t elSize,uint32_t size,void (*ptr2Destroy)(void*),void (*ptr2Copy)(void*,void*));
     #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfArrayV_Init(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_ARRAYV_INIT,5,__VA_ARGS__)
        #define i_NPSELECT_RF_ARRAYV_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Init() accepts from 3 to 5 arguments\"")
        #define i_NPSELECT_RF_ARRAYV_INIT0(...) RF_SELECT_FUNC(i_SELECT_RF_ARRAYV_INIT,__VA_ARGS__)
        #define i_SELECT_RF_ARRAYV_INIT5(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY) \
            i_rfArrayV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY)
        #define i_SELECT_RF_ARRAYV_INIT4(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_,i_PTR2DESTROY_) \
            i_rfArrayV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
        #define i_SELECT_RF_ARRAYV_INIT3(i_LIST_,i_TYPE_ ,i_NUMELEMENTS_) \
            i_rfArrayV_Init(i_LIST_,sizeof(i_TYPE_) , i_NUMELEMENTS_ ,(void(*)(void*))i_RF_GET_DEINIT(i_TYPE_),(void(*)(void*,void*))i_RF_GET_COPYIN(i_TYPE_))
        #define i_SELECT_RF_ARRAYV_INIT2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Init() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_ARRAYV_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Init() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_ARRAYV_INIT0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayV_Init() accepts from 3 to 5 arguments\"")
    #else
        #define rfArrayV_Init(i_ARRAY_,i_TYPE_,i_NUMELEMENTS_,i_PTR2DESTROY_,i_PTR2COPY_) \
            i_rfArrayV_Init(i_ARRAY_,sizeof(object_p),sizeof(i_TYPE_), i_NUMELEMENTS_,(void(*)(void*))i_PTR2DESTROY_,(void(*)(void*,void*))i_PTR2COPY_)
    #endif
#endif

//! @memberof RF_ArrayV
//! @brief Copies RF_ArrayV @c src into RF_ArrayV @c dst
//!
//! @param dst The new array copy to create
//! @param src The array to copy from
i_DECLIMEX_ void rfArrayV_Copy_IN(RF_ArrayV* dst,RF_ArrayV* src);
//! @memberof RF_ArrayV
//! @brief Creates and returns an allocated copy of the given RF_ArrayV
//!
//! @param src The array to copy from
//! @return An allocated RF_ArrayV copy of the given array
i_DECLIMEX_ RF_ArrayV* rfArrayV_Copy_OUT(RF_ArrayV* src);

//! @memberof RF_ArrayV
//! @brief Destroys an array of objects, calling the destructing function if it exists
//!
//! If for some reason you do not want to call the destruction
//! function of the given objects at array destruction use
//! @ref rfArrayV_Destroy_nofree()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayV_Destroy(RF_ArrayV* a);
//! @memberof RF_ArrayV
//! @brief Destroys an array of objects, without calling the destructing function if it exists
//!
//! For a function that deletes the array and calls the appropriate
//! destruction function for each individual object refer to
//! @ref rfArrayV_Destroy()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayV_Destroy_nofree(RF_ArrayV* a);
//! @memberof RF_ArrayV
//! @brief Deinitializes an array of objects, calling the destructing function if it exists
//!
//! If for some reason you do not want to call the destruction
//! function of the given objects at array deinitialization use
//! @ref rfArrayV_Deinit_nofree()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayV_Deinit(RF_ArrayV* a);
//! @memberof RF_ArrayV
//! @brief Deinitializes an array of objects, without calling the destructing function if it exists
//!
//! For a function that deletes the array and calls the appropriate
//! deinitalization function for each individual object refer to
//! @ref rfArrayV_Deinit()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayV_Deinit_nofree(RF_ArrayV* a);


//! @memberof RF_ArrayV
//! @brief Sets object @c e at the @c i index of the Array
//!
//! The object is just copied as it is inside the already allocated space of @c sizeof(objectType) of the data structure
//! So basically the object itself is added into the List. No copy is made.
//! For a function that does not add the object itself but instead make a copy first
//! take a look at @ref rfArrayV_Set_copy()
//! @param a The array for which to set
//! @param i The index of the object to set
//! @param e A pointer to the object to add.
//! @param free A boolean value. Give @c true if you want to call the destructing function of the
//! previous object at @c i or @c false if you don't
//! @return True if the element got set succesfully and false if index i is over capacity
i_DECLIMEX_ char rfArrayV_Set(RF_ArrayV* a,uint32_t i,void* e,char free);
//! @memberof RF_ArrayV
//! @brief Sets a copy of object @c e at the @c i index of the Array
//!
//! Before adding object @c e to the array, a copy is made using the array's
//! object type copy function. If such a function has not been given at array initialization
//! then @c false is returned.
//! For a function that adds the object itself without copying take
//! a look at @ref rfArrayV_Set()
//! @param a The array for which to set
//! @param i The index of the object to set
//! @param e A pointer to the object to add.
//! @param free A boolean value. Give @c true if you want to call the destructing function of the
//! previous object at @c i or @c false if you don't
//! @return True if the element got set succesfully and false if index i is over capacity or no copy function is given at list initialization.
i_DECLIMEX_ char rfArrayV_Set_copy(RF_ArrayV* a,uint32_t i,void* e,char free);

//! @memberof RF_ArrayV
//! @brief Retrieves an object from the array and saves it in the given pointer
//!
//! @param a The array from which to retrieve
//! @param i The index of the object to retrieve
//! @param ret A pointer to the object you need to populate with the retrieved data
//! @return RF_SUCCESS if the object was retrieved succesfully , RE_INDEX_OUT_OF_BOUNDS if the index is out of bounds and RE_ARRAY_UNINITIALIZED_OBJECT if the requested object does not exist.
i_DECLIMEX_ int32_t rfArrayV_Get(RF_ArrayV* a,uint32_t i,void* ret);

//! @memberof RF_ArrayV
//! @brief Removes an object from the array
//!
//! If a destructing function was provided in array initialization
//! then it is called for the removed object. If there is a need to not
//! destroy the object call @ref rfArrayV_Remove_nofree()
//! @param a The array from which to remove
//! @param i The index of the element to remove. If it is out of bounds an error is logged and nothing happens
i_DECLIMEX_ void rfArrayV_Remove(RF_ArrayV* a, uint32_t i);
//! @memberof RF_ArrayV
//! @brief Removes an object from the array without calling its destructing function
//!
//! For also destroying the object residing at @c i take a look
//! at @ref rfArrayV_Remove()
//! @param a The array from which to remove
//! @param i The index of the element to remove. If it is out of bounds an error is logged and nothing happens
i_DECLIMEX_ void rfArrayV_Remove_nofree(RF_ArrayV* a, uint32_t i);

//! @memberof RF_ArrayV
//! @brief Reallocates the array changing its size
//!
//! @param a The array to reallocate
//! @param newSize The new size of the array in number of objects
//! @return Returns true for succesful reallocation and false otherwise
i_DECLIMEX_ char rfArrayV_Reallocate(RF_ArrayV* a,uint32_t newSize);


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


//! @memberof RF_ArrayP
//! @brief Allocates and returns an array of pointers to objects
//!
//! @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
//! @param size The number of pointers to allocate for the Array
//! @param ptr2Destroy \rfoptional{The object's destruction function} A pointer to the destruction function for the elements stored inside the list.
//! Used for object deletion. Can also be @c 0 which means no destruction of objects will happen. They will simply be removed
//! from the list of pointers. This argument can only be omitted for objects of the refu library. If omitted then the appropriate
//! @c objectType_Destroy() function will be called. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @param ptr2Copy \rfoptional{The object's Copy_OUT function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
//! List and also making copies of the objects themselves.  Can also be @c 0 which means that at List copying the copied
//! list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
//! that stores data on the heap. It is recommended to always provide a copy function.
//! This function must be in the form of all the functions of the library that return an allocated copy of an object
//! from a destination of the same object type. For reference look at @ref rfString_Copy_OUT() or rfListV_Copy_OUT()
//! For storing objects of the Refu library this argument can be omitted and will be given a pointer
//! to the object's @c objectType_Copy_OUT() function. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @return The newly initialized array or null pointer if the object size was 0
#ifdef  RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_ArrayP* rfArrayP_Create(Type objectType,uint32_t size,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
#else
i_DECLIMEX_ RF_ArrayP* i_rfArrayP_Create(uint32_t size,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfArrayP_Create(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_ARRAYP_CREATE,4,__VA_ARGS__)
        #define i_NPSELECT_RF_ARRAYP_CREATE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Create() accepts from 2 to 4 arguments\"")
        #define i_NPSELECT_RF_ARRAYP_CREATE0(...) RF_SELECT_FUNC(i_SELECT_RF_ARRAYP_CREATE,__VA_ARGS__)
        #define i_SELECT_RF_ARRAYP_CREATE4(i_TYPE_,i_SIZE_,i_PTR2DESTROY_,i_PTR2COPY_) \
            i_rfArrayP_Create(i_SIZE_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
        #define i_SELECT_RF_ARRAYP_CREATE3(i_TYPE_,i_SIZE_,i_PTR2DESTROY_) \
            i_rfListP_Create(i_SIZE_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define i_SELECT_RF_ARRAYP_CREATE2(i_TYPE_,i_SIZE_) \
            i_rfListP_Create(i_SIZE_,(void(*)(void*))i_RF_GET_DESTROY(i_TYPE_),(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define i_SELECT_RF_ARRAYP_CREATE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Create() accepts from 2 to 4 arguments\"")
        #define i_SELECT_RF_ARRAYP_CREATE0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Create() accepts from 2 to 4 arguments\"")
    #else
        #define rfArrayP_Create(i_TYPE_,i_SIZE_,i_PTR2DESTROY_,i_PTR2COPY_) i_rfArrayP_Create(i_SIZE_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
    #endif
#endif

//! @memberof RF_ArrayP
//! @brief Initialises an array of pointers to objects
//!
//! @param arr The array to initialize
//! @param objectType Give the type of the object you want stored in the array. For example if you want strings give @c RF_String
//! @param size The number of pointers to allocate for the Array
//! @param ptr2Destroy \rfoptional{The object's destruction function} A pointer to the destruction function for the elements stored inside the list.
//! Used for object deletion. Can also be @c 0 which means no destruction of objects will happen. They will simply be removed
//! from the list of pointers. This argument can only be omitted for objects of the refu library. If omitted then the appropriate
//! @c objectType_Destroy() function will be called. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @param ptr2Copy \rfoptional{The object's Copy_OUT function} A pointer to the copying function for the objects stored inside the list. It is used for copying the
//! List and also making copies of the objects themselves.  Can also be @c 0 which means that at List copying the copied
//! list contains a blind data buffer copy of the to copy list. Can be dangerous for any type of stored object
//! that stores data on the heap. It is recommended to always provide a copy function.
//! This function must be in the form of all the functions of the library that return an allocated copy of an object
//! from a destination of the same object type. For reference look at @ref rfString_Copy_OUT() or rfListV_Copy_OUT()
//! For storing objects of the Refu library this argument can be omitted and will be given a pointer
//! to the object's @c objectType_Copy_OUT() function. If you are making a list of non Refu object and omit this argument
//! it will lead to a compilation error.
//! @return True for success or false if the object size was 0
#ifdef  RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char rfArrayP_Init(RF_ArrayP* arr,Type objectType,uint32_t size,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
#else
i_DECLIMEX_ char i_rfArrayP_Init(RF_ArrayP* arr,uint32_t size,void (*ptr2Destroy)(void*),void* (*ptr2Copy)(void*));
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfArrayP_Init(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_ARRAYP_INIT,5,__VA_ARGS__)
        #define i_NPSELECT_RF_ARRAYP_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Init() accepts from 3 to 5 arguments\"")
        #define i_NPSELECT_RF_ARRAYP_INIT0(...) RF_SELECT_FUNC(i_SELECT_RF_ARRAYP_INIT,__VA_ARGS__)
        #define i_SELECT_RF_ARRAYP_INIT5(i_ARR_,i_TYPE_,i_SIZE_,i_PTR2DESTROY_,i_PTR2COPY_) \
            i_rfArrayP_Init(i_ARR_,i_SIZE_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
        #define i_SELECT_RF_ARRAYP_INIT4(i_ARR_,i_TYPE_,i_SIZE_,i_PTR2DESTROY_) \
            i_rfArrayP_Init(i_ARR_,i_SIZE_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define i_SELECT_RF_ARRAYP_INIT3(i_ARR_,i_TYPE_,i_SIZE_) \
            i_rfArrayP_Init(i_ARR_,i_SIZE_,(void(*)(void*))i_RF_GET_DESTROY(i_TYPE_),(void*(*)(void*))i_RF_GET_COPYOUT(i_TYPE_))
        #define i_SELECT_RF_ARRAYP_INIT2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Init() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_ARRAYP_INIT1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Init() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_ARRAYP_INIT0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfArrayP_Init() accepts from 3 to 5 arguments\"")
    #else
        #define rfArrayP_Init(i_ARR_,i_TYPE_,i_SIZE_,i_PTR2DESTROY_,i_PTR2COPY_) i_rfArrayP_Init(i_ARR_,i_SIZE_,(void(*)(void*))i_PTR2DESTROY_,(void*(*)(void*))i_PTR2COPY_)
    #endif
#endif

//! @memberof RF_ArrayP
//! @brief Copies RF_ArrayP @c src into RF_ArrayP @c dst
//!
//! @param dst The new array copy to create
//! @param src The array to copy from
i_DECLIMEX_ void rfArrayP_Copy_IN(RF_ArrayP* dst,RF_ArrayP* src);
//! @memberof RF_ArrayP
//! @brief Creates and returns an allocated copy of the given RF_ArrayP
//!
//! @param src The array to copy from
//! @return An allocated RF_ArrayP copy of the given array
i_DECLIMEX_ RF_ArrayP* rfArrayP_Copy_OUT(RF_ArrayP* src);

//! @memberof RF_ArrayP
//! @brief Destroys an array of pointers to objects releasing their memory and calling the destructing function if it exists
//!
//! If for some reason you require to not call the destruction function of the
//! objects pointed to by the individual pointers of the array
//! take a look at @ref rfArrayP_Destroy_nofree()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayP_Destroy(RF_ArrayP* a);
//! @memberof RF_ArrayP
//! @brief Destroys an array of pointers to objects without calling the destructing function
//!
//! For a function that also calls the destructing function of the
//! individual objects pointed to by pointers use @ref rfArrayP_Destroy()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayP_Destroy_nofree(RF_ArrayP* a);
//! @memberof RF_ArrayP
//! @brief Deinitializes an array of pointers to objects releasing their memory and calling the destructing function if it exists
//!
//! If for some reason you require to not call the destruction function of the
//! objects pointed to by the individual pointers of the array
//! take a look at @ref rfArrayP_Deinit_nofree()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayP_Deinit(RF_ArrayP* a);
//! @memberof RF_ArrayP
//! @brief Deinitializes an array of pointers to objects without calling the destructing function
//!
//! For a function that also calls the destructing function of the
//! individual objects pointed to by pointers use @ref rfArrayP_Deinit()
//! @param a The array to destroy
i_DECLIMEX_ void rfArrayP_Deinit_nofree(RF_ArrayP* a);

//! @memberof RF_ArrayP
//! @brief Sets the pointer at index @c i of the array to point to the object pointed to by @c e
//!
//! The pointer at @c e is just inserted into the array. If you require it to be copied
//! first then you should refer to @ref rfArrayP_Set_copy
//! @param a The array for which to set
//! @param i The index of the object to set
//! @param e A pointer to the object to add.
//! @param free A boolean value. Give @c true if you want to call the destructing function of the
//! object pointed to by the pointer at index @c i of the array or @c false if you don't
//! @return True if the pointer got set succesfully and false if index i is over capacity
i_DECLIMEX_ char rfArrayP_Set(RF_ArrayP* a,uint32_t i,void* e,char free);
//! @memberof RF_ArrayP
//! @brief Sets the pointer at index @c i of the array to point to a copy of the object pointed to by @c
//!
//! A copy of the pointer at @c e is made and assigned to the pointer at the array's index @c i.
//! If you require the pointer to be assigned itself then use @ref rfArrayP_Set()
//! @param a The array for which to set
//! @param i The index of the object to set
//! @param e A pointer to the object to add
//! @param free A boolean value. Give @c true if you want to call the destructing function of the
//! object pointed to by the pointer at index @c i of the array or @c false if you don't
//! @return True if the pointer got set succesfully and false if index i is over capacity or no copy function was given at initialization
i_DECLIMEX_ char rfArrayP_Set_copy(RF_ArrayP* a,uint32_t i,void* e,char free);

//! @memberof RF_ArrayP
//! @brief Retrieves a pointer to an object from the array
//! @param a The array from which to retrieve
//! @param i The index of the pointer to retrieve
//! @return A pointer to the element or null pointer if the element's index is out of bounds. The user will have to cast it to the pointer type he requires.
i_DECLIMEX_ void* rfArrayP_Get(RF_ArrayP* a,uint32_t i);

//! @memberof RF_ArrayP
//! @brief Removes a pointer from the array
//!
//! Removes the pointer at index @c i of the array while at the same time also calling the destructing function
//! on the object it was pointing at. For a function that will not be calling the destructing function use
//! @ref rfArrayP_Remove_nofree()
//! @param a The array from which to remove
//! @param i The index of the element to remove. If it is out of bounds an error is logged and nothing happens
i_DECLIMEX_ void rfArrayP_Remove(RF_ArrayP* a, uint32_t i);
//! @memberof RF_ArrayP
//! @brief Removes a pointer from the array without calling the destructing functiong on the object it points to
//!
//! Removes the pointer at index @c i of the array witout calling the destructing function on the object
//! it was pointing at. If you need to destroy it then use @ref rfArrayP_Remove()
//! @param a The array from which to remove
//! @param i The index of the element to remove. If it is out of bounds an error is logged and nothing happens
i_DECLIMEX_ void rfArrayP_Remove_nofree(RF_ArrayP* a, uint32_t i);

//! @memberof RF_ArrayP
//! @brief Reallocates the array of pointers changing its size
//!
//! @param a The array to reallocate
//! @param newSize The new size of the array in pointers
//! @return Returns true for succesful reallocation and false otherwise
i_DECLIMEX_ char rfArrayP_Reallocate(RF_ArrayP* a,uint32_t newSize);

// The generic function pointer typedef
typedef void (*genericfp)(void*);

/**
** @internal
** @author Lefteris
** @date 02/08/2011
** @endinternal
** @brief A function pointer array
**
** A Group of any generic function pointers that can be created
** dynamically. Functions to add, remove and retrieve
** object from the function group exist
**/
typedef struct RF_FuncGroup
{
    //! The function poitner data
    genericfp* data;
    //! An element's size
    uint32_t elementSize;
    //! The current buffer capacity in elements
    uint32_t bufferCapacity;
    //! The current size in functions
    uint32_t size;
}RF_FuncGroup;

//! @memberof RF_FuncGroup
//! @brief Allocates and returns a group of function pointers.
//!
//! This macro is the preferred way to Create a group of function pointer. It hides a call to #i_rfFuncGroup_Create
//! @param functionptr_p The function pointer, whose function types you want to store in this array
//! @param size_p The number of function pointers that the array will have.
//! @return The initializes function pointer array or 0 if there was an error
#define rfFuncGroup_Create(functionptr_p,size_p)   i_rfFuncGroup_Create(sizeof(functionptr_p),size_p);
//! @memberof RF_FuncGroup
//! @brief  Initialises a group of function pointers
//!
//! This macro is the preferred way to Initialize a group of function pointer. It hides a call to #i_rfFuncGroup_Init
//! @param functionarr_p The array of function pointers you want to initialize
//! @param functionptr_p The function pointer, whose function types you want to store in this array
//! @param size_p The number of function pointers that the array will have.
#define rfFuncGroup_Init(functionarr_p,functionptr_p,size_p)   i_rfFuncGroup_Init(functionarr_p,sizeof(functionptr_p),size_p);

//! @memberof RF_FuncGroup
//! @brief Do not use. Use #rfFuncGroup_Create instead.Allocates and returns a group of function pointers
//! @param elSize The size of a single element
//! @param numElements The number of elements to allocate initially for the Group
//! @return The newly initialized group or null pointer if the element size was 0
i_DECLIMEX_ RF_FuncGroup*   i_rfFuncGroup_Create(uint32_t elSize,uint32_t numElements);
//! @memberof RF_FuncGroup
//! @brief Do not use. Use #rfFuncGroup_Init instead. Initialises a group of function pointers
//! @param g The group to initialize
//! @param elSize The size of a single element
//! @param numElements The number of elements to allocate initially for the Group
//! @return true for success and false otherwise
i_DECLIMEX_ char   i_rfFuncGroup_Init(RF_FuncGroup* g,uint32_t elSize,uint32_t numElements);

//! @memberof RF_FuncGroup
//! @brief Destroys a group of function pointer elements
//!
//! Use for groups made by _Create
//! @param g The group to destroy
i_DECLIMEX_ void rfFuncGroup_Destroy(RF_FuncGroup* g);
//! @memberof RF_FuncGroup
//! @brief Deinitializes a group of function pointer elements
//!
//! Use for groups made by _Init
//! @param g The group to destroy
i_DECLIMEX_ void rfFuncGroup_Deinit(RF_FuncGroup* g);

//! @memberof RF_FuncGroup
//! @brief Adds a new function to the group
//!
//! Element must be a function pointer
//! @param g The group in which to add the function
//! @param fp The function pointer
i_DECLIMEX_ void rfFuncGroup_Add(RF_FuncGroup* g,genericfp fp);

//! @memberof RF_FuncGroup
//! @brief Retrieves a generic function pointer from the group
//!
//! The user must cast it into the appropriate function  type
//! @param g The group from which to retrieve
//! @param i The index of the function to retrieve
//! @return A generic function pointer. Needs to be properly cast
i_DECLIMEX_ genericfp rfFuncGroup_Get(RF_FuncGroup* g,uint32_t i);

//! @memberof RF_FuncGroup
//! @brief Removes a function from the group
//! @param g The group from which to remove
//! @param i The index of the function to remove
i_DECLIMEX_ void rfFuncGroup_Remove(RF_FuncGroup* g, uint32_t i);

//! @memberof RF_FuncGroup
//! @brief Empties a function group
//! @param g The group to empty
i_DECLIMEX_ void rfFuncGroup_Empty(RF_FuncGroup* g);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#else //end of the array module include
    #error Attempted to include Refu Array Data Structure module with the module flag off. Rebuild the library with that option added if you need to include them.
#endif

#endif //end of include guards
