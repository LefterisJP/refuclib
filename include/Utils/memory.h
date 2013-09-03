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
** --Utils/memory.h
** This file simply contains some macros that allow for easy tweaking of memory allocation and freeing.
** In the future may contain specific memory allocation algorithm
**
---------------------For internal library include make sure to have----------------------------
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <stdio.h>//for FILE* used inside printf.h
#include <Definitions/imex.h> //for import export macro used inside <Utils/error.h>
#include <Definitions/types.h> //for fixed size data types used inside <Utils/error.h>
#include <IO/printf.h> //for rfFpintf() used in the error logging macros
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <Utils/error.h> //for RF_ERROR() macros family
#include <Utils/memory.h> //for refu memory allocation
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_MEMORY_H
#define RF_MEMORY_H

#include <rf_options.h> //check the memory allocation options


//Here are some macro wrappers of malloc,calloc and realloc that depending
//on the flag @c RF_OPTION_SAFE_MEMORY_ALLOCATION check their return
//value or not

//for realloc I check no matter what since it's a bit more complicated case than the other two




/**
 ** Wrapper macro of the realloc() function that does check for memory
 ** allocation failure. The function that calls it must return type of 
 ** @c RETVAL_
 ** @param REALLOC_RETURN_ Give the pointer that you need to realloc here
 ** @param TYPE_ give the type of the pointer here
 ** @param SIZE_ Give the size parameter of realloc here
 ** @param RETVAL_ This is what the calling function should return
 ** if the memory allocarion fails
 **
 **/
#define RF_REALLOC(REALLOC_RETURN_, TYPE_, SIZE_, RETVAL_)              \
    do{ TYPE_* i_TEMPPTR_ = realloc( (REALLOC_RETURN_),(SIZE_));        \
        if(i_TEMPPTR_ == NULL)                                          \
        {                                                               \
            RF_ERROR(0, "realloc() failure");                           \
            return RETVAL_;                                             \
        }                                                               \
        REALLOC_RETURN_ = i_TEMPPTR_;                                   \
    }while(0)

/**
 ** Wrapper macro of the realloc() function that does check for memory
 ** allocation failure. The function that calls it must jump to its return
 ** @param REALLOC_RETURN_ Give the pointer that you need to realloc here
 ** @param TYPE_ give the type of the pointer here
 ** @param SIZE_ Give the size parameter of realloc here
 ** @param STMT_ A statement to execute before jumping. Give ';' for nothing
 ** @param GOTOFLAG_ This is where the calling function should jump to
 ** if the memory allocation fails
 **
 **/
#define RF_REALLOC_JMP(REALLOC_RETURN_, TYPE_, SIZE_, STMT_, GOTOFLAG_)  \
    do{ TYPE_* i_TEMPPTR_ = realloc( (REALLOC_RETURN_),(SIZE_));        \
        if(i_TEMPPTR_ == NULL)                                          \
        {                                                               \
            RF_ERROR(0, "realloc() failure");                            \
            STMT_;                                                      \
            goto GOTOFLAG_;                                             \
        }                                                               \
        REALLOC_RETURN_ = i_TEMPPTR_;                                   \
    }while(0)


/* ---- SAFE MEMORY ALLOCATION macros ---- */
#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION

/**
 ** Wrapper macro of the malloc() function that does check for memory
 ** allocation failure. The function that calls it must return value of
 ** type @c RETVAL_
 ** @param MALLOC_RETURN_ Give the pointer of that you want to point to
 ** malloc's return here
 ** @param MALLOC_SIZE_   Give the size parameter of malloc here
 ** @param RETVAL_ This is what the calling function should return
 ** if the memory allocarion fails
 **/
#define RF_MALLOC(MALLOC_RETURN_, MALLOC_SIZE_, RETVAL_)                \
    do{ MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) );                      \
        if(MALLOC_RETURN_ == NULL)                                      \
        {                                                               \
            RF_ERROR(0, " malloc() failure");          \
            return RETVAL_;                                    \
        } }while(0)

/**
 ** Wrapper macro of the malloc() function that does check for memory
 ** allocation failure. The function that calls it must return be 
 ** jumping to its return value
 ** @param MALLOC_RETURN_ Give the pointer of that you want to point to
 ** malloc's return here
 ** @param MALLOC_SIZE_   Give the size parameter of malloc here
 ** @param STMT_ A statement to execute before jumping. Give ';' for nothing
 ** @param GOTOFLAG_ This is where the calling function should jump to
 ** if the memory allocation fails
 **/
#define RF_MALLOC_JMP(MALLOC_RETURN_, MALLOC_SIZE_, STMT_, GOTOFLAG_)   \
    do{ MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) );                      \
        if(MALLOC_RETURN_ == NULL)                                      \
        {                                                               \
            RF_ERROR(0, "malloc() failure");           \
            STMT_;                                                      \
            goto GOTOFLAG_;                                             \
        } }while(0)

/**
 ** Wrapper macro of the calloc() function that does check for memory
 ** allocation failure.
 ** @param CALLOC_RETURN_ Give the pointer of that you want to point to
 ** calloc's return here
 ** @param CALLOC_NUM_    Give the number parameter of calloc here
 ** @param MALLOC_SIZE_   Give the size parameter of calloc here
 ** @param RETVAL_ This is what the calling function should return
 ** if the memory allocarion fails
 **
 **/
#define RF_CALLOC(CALLOC_RETURN_,CALLOC_NUM_,CALLOC_SIZE_, RETVAL_)     \
    do{ CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) );       \
        if(CALLOC_RETURN_ == NULL)                                      \
        {                                                               \
            RF_ERROR(0, "calloc() failure");                           \
            return RETVAL_;                                             \
        } }while(0)

/**
 ** Wrapper macro of the calloc() function that does check for memory
 ** allocation failure and if it fails jumps to a goto flag
 ** @param CALLOC_RETURN_ Give the pointer of that you want to point to
 ** calloc's return here
 ** @param CALLOC_NUM_    Give the number parameter of calloc here
 ** @param MALLOC_SIZE_   Give the size parameter of calloc here
 ** @param STMT_ A statement to execute before jumping. Give ';' for nothing
 ** @param GOTOFLAG_ This is where the calling function should jump to
 ** if the memory allocation fails
 **
 **/
#define RF_CALLOC_JMP(CALLOC_RETURN_,CALLOC_NUM_,                       \
                      CALLOC_SIZE_, STMT_, GOTOFLAG_)                   \
    do{ CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) );       \
        if(CALLOC_RETURN_ == NULL)                                      \
        {                                                               \
        RF_ERROR(0, "calloc() failure");              \
            STMT_;
            goto GOTOFLAG__;                                             \
        } }while(0)

/* ---- NOT SAFE MEMORY ALLOCATION macros ---- */
#else

#define RF_MALLOC(MALLOC_RETURN_,MALLOC_SIZE_, RETVAL_) \
    MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) )
#define RF_MALLOC_JMP(MALLOC_RETURN_, MALLOC_SIZE_, STMT_, GOTOFLAG_)  \
    MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) )

#define RF_CALLOC(CALLOC_RETURN_, CALLOC_NUM_, CALLOC_SIZE_, RETVAL_) \
    CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) )
#define RF_CALLOC_JMP(CALLOC_RETURN_,CALLOC_NUM_,CALLOC_SIZE_,    \
                      STMT_, GOTOFLAG_)                           \
        CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) )
#endif

#endif//include guards end
