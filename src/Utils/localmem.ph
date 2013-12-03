/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/
#ifndef RF_LOCAL_MEMORY_PH
#define RF_LOCAL_MEMORY_PH

/*------------- Module related inclusion -------------*/
#include <Utils/localmem.h> //for RF_LMS
/*------------- Outside module inclusion -------------*/
#include <Definitions/types.h> //fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/threadspecific.h> //for the thread specific attribute
#include <Utils/log.h>
/*------------- End of includes -------------*/

/**
 ** Is the internal version of the @ref rf_lms_push() function.
 ** Is used internally in the library
 ** to allow faster local memory stack objects initialization internally
 ** @param i_ptr_ The pointer to point to the newly allocated place in the stack.
 ** Can be 0 if there was not enough size
 ** @param i_size_ The size to allocate
 **
 **/
#define i_rf_lms_push(i_ptr_, i_size_, i_FAIL_STMT_)    do{              \
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
 ** Is the internal library version of @ref rf_lms_args_eval() function.
 ** It keeps the stack pointer before a specific function's argument 
 ** evaluation. So that we can know what stack pointer value to return 
 ** to after a function with local objects finishes
 ** @param i_FAIL_STMT_ is the statement to execute in case
 ** of failure
 **/
#define i_rf_lms_args_eval(i_FAIL_STMT_) do{\
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
