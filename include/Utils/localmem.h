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
** --Utils/localmem.h
**
** Contains the local memory stack functions
*/
#ifndef RF_LOCAL_MEMORY_H
#define RF_LOCAL_MEMORY_H


#include <Definitions/retcodes.h> /* for bool */
#include <Definitions/types.h> /* fixed size data types */
#include <Definitions/imex.h> 
#include <Definitions/threadspecific.h> 
#include <Utils/localmem_decl.h> // for RFlocal_memory_stack

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
 ** @memberof RFlocal_memory_stack
 ** @brief Initializes the local memory stack
 **
 ** This function simply initializes a local memory stack and should not 
 ** be used by the user directly. It is used in 2 occasions inside the
 ** library. Once inside the library's
 ** initialization function @ref rf_init() and at every new @ref RFthread
 **  initialization since this stack is thread specific.
 ** @param lms The Local Memory stack to initialize
 ** @param size The size that the local memory stack will have
 ** @return Returns @c true if initialized correctly and @c false otherwise
 **
 **/
bool rf_lms_init(RFlocal_memory_stack* lms, uint64_t size);

/**
 ** @memberof RFlocal_memory_stack
 ** @brief Pushes the local memory stack
 **
 ** This function simply pushes the local memory stack for @c size bytes so
 ** that the stack pointer gets moved up by that amount of bytes. It also
 ** returns a pointer to the position of the stack pointer before the push
 ** which will be the allocated pointer memory position. In that respect
 ** the function works similarly to malloc()
 ** @param size The size of the block to allocate
 ** @return Returns a pointer to the allocated block or NULL
 **  for no more local memory available
 **
 **/
i_DECLIMEX_ void* rf_lms_push(uint64_t size);

/**
 ** @memberof RFlocal_memory_stack
 ** @brief Pops the local stack memory back to a certain value
 **
 ** This function puts the local stack pointer back to a specific value
 ** and in doing so essentially frees all the objects that were allocated
 ** to parts of the stack before that movement.
 ** @param t The stack value to which to pop the stack back to
 ** @return Returns @c true for success and @c false if popping was 
 ** attemped up to a point that has not yet been allocated
 **
 **/
i_DECLIMEX_ bool rf_lms_pop(uint64_t t);

/**
 ** @memberof RFlocal_memory_stack
 ** @brief Keeps the stack pointer before the specific macro evaluation
 **
 ** This function remembers the stack pointer right before a given
 ** local macro evaluation. Because of the sequence points there is not a
 ** given method to see which macro gets evaluated first so at the 
 ** beginning of the function wrapper we will use all the saved pointer
 ** positions to find the one that is closer at the beginning and this 
 ** will be determined as the local memory stack pointer before the 
 ** function call
 ** @return Returns @c true for success and @c false if the maximum
 ** specified number of function arguments has been used
 **/
i_DECLIMEX_ bool rf_lms_args_eval();



i_DECLIMEX_ bool rf_module_lms_init(uint64_t size);

//functions to declare create old value data types on the local stack
//and pass them as parameters to functions which can accept them
i_DECLIMEX_ int* RFI_(int d);
i_DECLIMEX_ unsigned int* RFUI_(unsigned int d);
i_DECLIMEX_ int8_t* RFI8_(int8_t d);
i_DECLIMEX_ uint8_t* RFUI8_(uint8_t d);
i_DECLIMEX_ int16_t* RFI16_(int16_t d);
i_DECLIMEX_ uint16_t* RFUI16_(uint16_t d);
i_DECLIMEX_ int32_t* RFI32_(int32_t d);
i_DECLIMEX_ uint32_t* RFUI32_(uint32_t d);
i_DECLIMEX_ int64_t* RFI64_(int64_t d);
i_DECLIMEX_ uint64_t* RFUI64_(uint64_t d);
i_DECLIMEX_ float* RFFLOAT_(float d);
i_DECLIMEX_ double* RFDOUBLE_(double d);

#ifdef __cplusplus
} //closing bracket for C++ calling
#endif

#endif //include guards end
