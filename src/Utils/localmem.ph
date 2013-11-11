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
**
**
** --Utils/localmem.ph
**
** Contains the local memory stack private functionality.
** Which is just the functions of the normal header turned macros for
** faster local objects initialization in the local memory stack
**
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/threadspecific.h> //for the thread specific attribute
#include <string.h> //for size_t and memset() used in the Local scope macros
#include <Utils/localmem_decl.h> //for local memory stack

#include <stdio.h>//for FILE* used inside printf.h
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <String/string_decl.h> //for RF_String
#include <String/common.h> //for RFS_() macro
#include <Utils/error.h>

#include <Definitions/retcodes.h> //for the log error return codes

#include <stdlib.h> //for exit()

#include "pathto/Utils/localmem.ph"
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_LOCAL_MEMORY_PH
#define RF_LOCAL_MEMORY_PH


/**
 ** Is the internal version of the @ref rfLMS_Push() function. Is used internally in the library
 ** to allow faster local memory stack objects initialization internally
 ** @param i_ptr_ The pointer to point to the newly allocated place in the stack. Can be 0 if there was not enough size
 ** @param i_size_ The size to allocate
 **
 **/

#define i_rfLMS_Push(i_ptr_, i_size_, i_FAIL_STMT_)    do{              \
    /* If the size of the stack plus the requested allocation goes beyond the stack size*/\
    if(RF_LMS->stackPtr+(i_size_) > RF_OPTION_LOCALSTACK_MEMORY_SIZE)\
    {\
        RF_ERROR("Local Stack Memory pushing error. Attempted to "   \
                 "allocate more memory than currently available in the " \
                 "local memory stack");                                                  \
        i_ptr_ = 0;                                                     \
        i_FAIL_STMT_;                                                   \
    }                                                                   \
    /* else, just increase the stack pointer and return the rightful amount of memory */\
    i_ptr_ = (void*)(RF_LMS->stack+RF_LMS->stackPtr);                   \
    RF_LMS->stackPtr+= (i_size_);                                       \
    }while(0)


/**
 ** Is the internal library version of @ref rfLMS_ArgsEval() function.
 ** It keeps the stack pointer before a specific function's argument 
 ** evaluation. So that we can know what stack pointer value to return 
 ** to after a function with local objects finishes
 ** @param i_FAIL_STMT_ is the statement to execute in case
 ** of failure
 **/
#define i_rfLMS_ArgsEval(i_FAIL_STMT_) do{\
        /*Check if the function had more local object macros evaluation than the max number of arguments*/ \
        if(RF_LMS->macroEvalsI+1 >= RF_MAX_FUNC_ARGS)                   \
        {                                                               \
            RF_ERROR("Local Stack Memory macro evaluation error. More" \
                     " macros than the specified maximum number of "    \
                     "function arguments \"%d\" have been evaluated",   \
                     RF_MAX_FUNC_ARGS);                                 \
            i_FAIL_STMT_;                                               \
        }                                                       \
        RF_LMS->macroEvals[RF_LMS->macroEvalsI] = RF_LMS->stackPtr; \
        RF_LMS->macroEvalsI++;                                      \
    }while(0)


#endif//include guards end
