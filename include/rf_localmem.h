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

#ifndef RF_LOCAL_MEMORY_H
#define RF_LOCAL_MEMORY_H


#include <rf_setup.h>
#include <Preprocessor/rf_xmacro_argcount.h>
#include <rf_numconst.h>

#define RF_MAX_FUNC_ARGS    32

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

///==========================Conversion from the void* type back to basic types ==================================================///
//! @brief Accepts the void* return result of the wrapper and turns it into a char
//!
//! This is an internal function and is not intended to be used by the user
//! @param ret The void* returned by the function
//! @return The char that the function returned
i_DECLIMEX_ char i_rfRETURN_char(void* ret);

//! @brief Accepts the void* return result of the wrapper and turns it into an int32_t type
//!
//! This is an internal function and is not intended to be used by the user
//! @param ret The void* returned by the function
//! @return The int32_t that the function returned
i_DECLIMEX_ int32_t i_rfRETURN_int32_t(void* ret);
//do nothing for void
#define i_rfRETURN_void(i_PARAM_)

/**These are automatically generated by the python LMS.py script**/
///===================================Local Memory Stack Function Wrappers Begin===================================///

/* The i_rfLMSWrapperN functions can be called to have total control of how to pass the void* arguments but for fast and easy usage
   the i_rfLMS_WrapN macros are provided. They just universally cast everything to void* and the function to the corresponding type */
i_DECLIMEX_ void* i_rfLMSWrapper1(void*(*fptr)(void*),void* arg1);
i_DECLIMEX_ void* i_rfLMSWrapper2(void*(*fptr)(void*,void*),void* arg1,void* arg2);
i_DECLIMEX_ void* i_rfLMSWrapper3(void*(*fptr)(void*,void*,void*),void* arg1,void* arg2,void* arg3);
i_DECLIMEX_ void* i_rfLMSWrapper4(void*(*fptr)(void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4);
i_DECLIMEX_ void* i_rfLMSWrapper5(void*(*fptr)(void*,void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4,void* arg5);
i_DECLIMEX_ void* i_rfLMSWrapper6(void*(*fptr)(void*,void*,void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6);
i_DECLIMEX_ void* i_rfLMSWrapper7(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7);
i_DECLIMEX_ void* i_rfLMSWrapper8(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8);
i_DECLIMEX_ void* i_rfLMSWrapper9(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9);
i_DECLIMEX_ void* i_rfLMSWrapper10(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10);
i_DECLIMEX_ void* i_rfLMSWrapper11(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11);
i_DECLIMEX_ void* i_rfLMSWrapper12(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12);
i_DECLIMEX_ void* i_rfLMSWrapper13(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12,void*arg13);
i_DECLIMEX_ void* i_rfLMSWrapper14(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12,void*arg13,void*arg14);
i_DECLIMEX_ void* i_rfLMSWrapper15(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12,void*arg13,void*arg14,void*arg15);
i_DECLIMEX_ void* i_rfLMSWrapper16(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12,void*arg13,void*arg14,void*arg15,void*arg16);
i_DECLIMEX_ void* i_rfLMSWrapper17(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12,void*arg13,void*arg14,void*arg15,void*arg16,void*arg17);
i_DECLIMEX_ void* i_rfLMSWrapper18(void*(*fptr)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*),
void* arg1,void* arg2,void* arg3,void* arg4,void* arg5,void* arg6,void* arg7,void* arg8,void* arg9,void* arg10,void* arg11,void* arg12,void*arg13,void*arg14,void*arg15,void*arg16,void*arg17,void* arg18);
//these are wrapper macros that just cast the result. This produces a warning for anything that casts
//the result to a smaller data type. But they are appropriate for functions that return pointers
#define i_rfLMS_WRAP1(i_RETTYPE_,i_FUNCNAME_,i_ARG1_) ((i_RETTYPE_)i_rfLMSWrapper1((void*(*)(void*))(i_FUNCNAME_),(void*)i_ARG1_))
#define i_rfLMS_WRAP2(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_) ((i_RETTYPE_) i_rfLMSWrapper2((void*(*)(void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_)))
#define i_rfLMS_WRAP3(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_) ((i_RETTYPE_)i_rfLMSWrapper3((void*(*)(void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_)))
#define i_rfLMS_WRAP4(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_) ((i_RETTYPE_)i_rfLMSWrapper4((void*(*)(void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_)))
#define i_rfLMS_WRAP5(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_) \
    ((i_RETTYPE_)i_rfLMSWrapper5((void*(*)(void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_)))
#define i_rfLMS_WRAP6(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_)  \
    ((i_RETTYPE_)i_rfLMSWrapper6((void*(*)(void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_)))
#define i_rfLMS_WRAP7(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_)  \
    ((i_RETTYPE_)i_rfLMSWrapper7((void*(*)(void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_)))
#define i_rfLMS_WRAP8(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_)  \
    ((i_RETTYPE_)i_rfLMSWrapper8((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_)))
#define i_rfLMS_WRAP9(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_)  \
    ((i_RETTYPE_)i_rfLMSWrapper9((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_)))
#define i_rfLMS_WRAP10(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_)  \
    ((i_RETTYPE_)i_rfLMSWrapper10((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_)))
#define i_rfLMS_WRAP11(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_)  \
    ((i_RETTYPE_)i_rfLMSWrapper11((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_)))
#define i_rfLMS_WRAP12(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_)  \
    ((i_RETTYPE_)i_rfLMSWrapper12((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_)))
#define i_rfLMS_WRAP13(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_)  \
    ((i_RETTYPE_)i_rfLMSWrapper13((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_)))
#define i_rfLMS_WRAP14(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_)  \
    ((i_RETTYPE_)i_rfLMSWrapper14((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_)))
#define i_rfLMS_WRAP15(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_)  \
    ((i_RETTYPE_)i_rfLMSWrapper15((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_)))
#define i_rfLMS_WRAP16(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_,i_ARG16_)  \
    ((i_RETTYPE_)i_rfLMSWrapper16((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_),(void*)(i_ARG16_)))
#define i_rfLMS_WRAP17(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_,i_ARG16_,i_ARG17_)  \
    ((i_RETTYPE_)i_rfLMSWrapper17((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_),(void*)(i_ARG16_),(void*)(i_ARG17_)))
#define i_rfLMS_WRAP18(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_,i_ARG16_,i_ARG17_,i_ARG18_)  \
    ((i_RETTYPE_)i_rfLMSWrapper18((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_),(void*)(i_ARG16_),(void*)(i_ARG17_),(void*)(i_ARG18_)))
//these are wrapper macros that use an extra function to wrap the resulting void* and return it as a basic data type
#define i_rfLMSX_WRAP1(i_RETTYPE_,i_FUNCNAME_,i_ARG1_)  i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper1((void*(*)(void*))(i_FUNCNAME_),(void*)i_ARG1_))
#define i_rfLMSX_WRAP2(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_) i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper2((void*(*)(void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_)))
#define i_rfLMSX_WRAP3(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_)  i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper3((void*(*)(void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_)))
#define i_rfLMSX_WRAP4(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_) i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper4((void*(*)(void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_)))
#define i_rfLMSX_WRAP5(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_) \
     i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper5((void*(*)(void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_)))
#define i_rfLMSX_WRAP6(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_)  \
     i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(  i_rfLMSWrapper6((void*(*)(void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_)))
#define i_rfLMSX_WRAP7(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper7((void*(*)(void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_)))
#define i_rfLMSX_WRAP8(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper8((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_)))
#define i_rfLMSX_WRAP9(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper9((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_)))
#define i_rfLMSX_WRAP10(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper10((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_)))
#define i_rfLMSX_WRAP11(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper11((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_)))
#define i_rfLMSX_WRAP12(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper12((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_)))
#define i_rfLMSX_WRAP13(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper13((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_)))
#define i_rfLMSX_WRAP14(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper14((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_)))
#define i_rfLMSX_WRAP15(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper15((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_)))
#define i_rfLMSX_WRAP16(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_,i_ARG16_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper16((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_),(void*)(i_ARG16_)))
#define i_rfLMSX_WRAP17(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_,i_ARG16_,i_ARG17_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper17((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_),(void*)(i_ARG16_),(void*)(i_ARG17_)))
#define i_rfLMSX_WRAP18(i_RETTYPE_,i_FUNCNAME_,i_ARG1_,i_ARG2_,i_ARG3_,i_ARG4_,i_ARG5_,i_ARG6_,i_ARG7_,i_ARG8_,i_ARG9_,i_ARG10_,i_ARG11_,i_ARG12_,i_ARG13_,i_ARG14_,i_ARG15_,i_ARG16_,i_ARG17_,i_ARG18_)  \
    i_RP_PASTE2(i_rfRETURN_,i_RETTYPE_)(i_rfLMSWrapper18((void*(*)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*,void*))(i_FUNCNAME_),(void*)(i_ARG1_),(void*)(i_ARG2_),(void*)(i_ARG3_),(void*)(i_ARG4_),(void*)(i_ARG5_),(void*)(i_ARG6_),(void*)(i_ARG7_),(void*)(i_ARG8_),\
    (void*)(i_ARG9_),(void*)(i_ARG10_),(void*)(i_ARG11_),(void*)(i_ARG12_),(void*)(i_ARG13_),(void*)(i_ARG14_),(void*)(i_ARG15_),(void*)(i_ARG16_),(void*)(i_ARG17_),(void*)(i_ARG18_)))


#endif//include guards end
