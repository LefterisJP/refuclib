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
** --Utils/localscope.h
**
** Contains the local scope macros that should be used in any function
** the may get arguments that have been initialize in the local memory
** stack
*/
#ifndef RF_LOCALSCOPE_H
#define RF_LOCALSCOPE_H

#include <Definitions/types.h> //fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/threadspecific.h> //for the thread specific attribute
#include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
#include <string.h> //for memset()
#include <stdint.h> //for UINT32_MAX used in RF_ENTER_LOCAL_SCOPE() macro

/**
 ** A small macro to find the smallest of the macro evaluation indices. 
 ** Just to avoid copying pasting
 **
 **/
#define RF_ENTER_LOCAL_SCOPE() \
uint32_t i_stackPtrBefore_;                 \
do{                                                           \
    uint32_t i_index_;                                        \
    /*If we had any macro evaluations in the function call*/  \
    if(RF_LMS->macroEvalsI > 0)                               \
    {\
        /*find the smallest pointer of the macro evaluations. This is where the stack pointer was before the function call*/\
        i_stackPtrBefore_ = UINT32_MAX;\
        for(i_index_=0; i_index_< RF_LMS->macroEvalsI; i_index_ ++)\
        {\
            if(RF_LMS->macroEvals[i_index_] < i_stackPtrBefore_)\
                i_stackPtrBefore_ = RF_LMS->macroEvals[i_index_];\
        }\
        /*now reset the macro evaluations to be ready for the next function call*/\
        RF_LMS->macroEvalsI = 0;\
        memset(RF_LMS->macroEvals,0,4*RF_MAX_FUNC_ARGS);\
    }\
    else\
        i_stackPtrBefore_= RF_LMS->stackPtr;\
    }while(0)

/**
 ** Another small macro to free the temporary objects after a function or
 ** scope has been passed. Effectively doing the same thing as popping the LMS
 **
 **/
#define RF_EXIT_LOCAL_SCOPE()   \
    RF_LMS->stackPtr = i_stackPtrBefore_


#endif //include guards end
