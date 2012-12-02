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
#ifndef RF_LOCAL_MEMORY_H
#define RF_LOCAL_MEMORY_H


#include <rf_setup.h>
#include <Preprocessor/rf_xmacro_argcount.h>

#include <limits.h> //for ULONG_MAX
#include <string.h> //for size_t

#define RF_MAX_FUNC_ARGS    32

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/** @brief The Local Memory stack
**
** This structure represents the local memory stack of a thread
** It contains a stack array along with its pointer that are initialized
** by the compile time option @c RF_OPTION_LOCALSTACKMEMORY_SIZE.
**
** Also contains some additional data for macro evaluations so that the functions
** can determine where the stack pointer was before each function call. They are
** necessary since at function call there is no guarantee which macro will be executed first
** and so in the case of having two macros that create a temporary object a way was needed to determine
** which came first and hence determine where the stack pointer was before a function call.
**/
typedef struct RF_LocalMemoryStack
{
    //! The local memory stack
    char* stack;
    //! The local memory stack pointer
    uint32_t stackPtr;
    //! The array that remembers the stack memory pointer for each local macro evaluation
    uint32_t macroEvals[RF_MAX_FUNC_ARGS];
    //! The index to the macro evaluations array
    uint32_t macroEvalsI;
}RF_LocalMemoryStack;

//! Declare a pointer to the thread-specific the local memory stack
extern i_THREAD__ i_DECLIMEX_  RF_LocalMemoryStack* RF_LMS;
//! The main thread's local memory stack
extern i_DECLIMEX_ RF_LocalMemoryStack RF_MainLMS;

//! @memberof RF_LocalMemoryStack
//! @brief Initializes the local memory stack
//!
//! This function simply initializes a local memory stack and should not be used by
//! ther user directly. It is used in 2 occasions inside the library. Once inside the library's
//! initialization function @ref rfInit() and at every new @ref RF_Thread initialization since
//! this stack is thread specific.
//! @param lms The Local Memory stack to initialize
//! @param size The size that the local memory stack will have
void rfLMS_Init(RF_LocalMemoryStack* lms,uint64_t size);

//! @memberof RF_LocalMemoryStack
//! @brief Pushes the local memory stack
//!
//! This function simply pushes the local memory stack for @c size bytes so that the stack pointer
//! gets moved up by that amount of bytes. It also returns a pointer to the position of the stack
//! pointer before the push which will be the allocated pointer memory position. In that respect
//! the function works similarly to malloc()
//! @param lms The local memory stack to push
//! @param size The size of the block to allocate
//! @return Returns a pointer to the allocated block or 0 for no more local memory available
i_DECLIMEX_ void* rfLMS_Push(RF_LocalMemoryStack* lms,size_t size);


//! @brief Pops the local stack memory back to a certain value
//!
//! This function puts the local stack pointer back to a specific value
//! and in doing so essentially frees all the objects that were allocated to
//! parts of the stack before that movement.
//! @param lms The local memory stack to pop from
//! @param t The stack value to which to pop the stack back to
i_DECLIMEX_ void rfLMS_Pop(RF_LocalMemoryStack* lms,size_t t);

//! @brief Keeps the stack pointer before the specific macro evaluation
//!
//! This function remembers the stack pointer right before a given
//! local macro evaluation. Because of the sequence points there is not a given
//! method to see which macro gets evaluated first so at the beginning of the function
//! wrapper we will use all the saved pointer positions to find the one that is
//! closer at the beginning and this will be determined as the local memory stack
//! pointer before the function call
//!
//! @param lms The local memory stack with which the macro gets evaluated
i_DECLIMEX_ void rfLMS_MacroEvalPtr(RF_LocalMemoryStack* lms);

//! Gets the stack pointer. A macro for now.
#define rfLMS_GetStackPtr() RF_LMS->stackPtr


//! A small macro to find the smallest of the macro evaluation indices. Just to avoid copying pasting
#define RF_ENTER_LOCAL_SCOPE()  \
    uint32_t i_stackPtrBefore;\
    uint32_t i_index_;\
    /*If we had any macro evaluations in the function call*/\
    if(RF_LMS->macroEvalsI > 0)\
    {\
        /*find the smallest pointer of the macro evaluations. This is where the stack pointer was before the function call*/\
        i_stackPtrBefore = ULONG_MAX;\
        for(i_index_=0;i_index_< RF_LMS->macroEvalsI; i_index_ ++)\
        {\
            if(RF_LMS->macroEvals[i_index_] < i_stackPtrBefore)\
                i_stackPtrBefore = RF_LMS->macroEvals[i_index_];\
        }\
        /*now reset the macro evaluations to be ready for the next function call*/\
        RF_LMS->macroEvalsI = 0;\
        memset(RF_LMS->macroEvals,0,4*RF_MAX_FUNC_ARGS);\
    }\
    else\
        i_stackPtrBefore= RF_LMS->stackPtr;

//! Another small macro to free the temporary objects after a function or scope has been passed
#define RF_EXIT_LOCAL_SCOPE()   \
    RF_LMS->stackPtr = i_stackPtrBefore;

#ifdef __cplusplus
} //closing bracket for C++ calling
#endif

#endif//include guards end
