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
**
**
**
** --Utils/localmem_decl.h
**
** Contains the local memory stack declaration
**
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/threadspecific.h> //for the thread specific attribute
#include <Utils/localmem_decl.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_LOCAL_MEMORY_DECL_H
#define RF_LOCAL_MEMORY_DECL_H


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

#endif //include guards end
