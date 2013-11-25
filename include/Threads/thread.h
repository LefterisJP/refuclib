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
** -Threads/thread.h
** This header declares functions and macros that operate on RF_Thread
*/
#ifndef RF_THREAD_H
#define RF_THREAD_H

#include <Definitions/retcodes.h> //for bool
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
    #ifdef REFU_WIN32_VERSION
    #include <windows.h> //for HANDLE
    #else
    #include <pthread.h> //for pthread_t
    #endif
#include <Threads/thread_flags.h> //for common Thread flags
#include <Threads/thread_decl.h> //for RF_Thread
#include <Threads/thread.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif




/**
 ** @memberof RF_Thread
 ** @brief Allocates and returns a thread
 **
 ** Identical to @ref rfThread_Init()
 **/
i_DECLIMEX_ RF_Thread* rfThread_Create(uint32_t flags,
                                       void* (*ptr2onExecution)(void*),
                                       void* data, uint64_t lmsSize );

/**
 ** @memberof RF_Thread
 ** @brief Initializes a thread
 **
 ** @param t                      The thread to initialize
 ** @param flags                  Bitflags parameters for the thread. Default is
 **                               0. Which means an RF_Thread that is joinable.
 **                               Possible combination of values is:
 **                                  + @c RF_THREAD_DETACHED: This creates a
 **                                       detached thread.
 ** @param ptr2onExecution        Give the main execution function of the thread
 ** @param data                   Give a pointer to the data to be passed to
 **                               the thread
 ** @param lmsSize                The size of the thread's local stack memory.
 **                               Provide a value here that shall determine this
 **                               thread's local memory stack size, used for
 **                               temporary objects. If 0 the default size is 
 **                               used
 ** @return                       Returns true for success
 **/
i_DECLIMEX_ bool rfThread_Init(RF_Thread* t, uint32_t flags,
                               void* (*ptr2onExecution)(void*),
                               void* data,
                               uint64_t lmsSize);


/**
 ** @memberof RF_Thread
 ** @brief Destroys a thread
 **
 ** @param t A pointer to the thread in question
 **/
i_DECLIMEX_ void rfThread_Destroy(RF_Thread* t);
/**
 ** @brief Deinitializes a thread
 **
 ** @param t A pointer to the thread in question
 **
 **/
i_DECLIMEX_ void rfThread_Deinit(RF_Thread* t);
/**
 ** @brief Frees a thread but also immediately causes it to exit.
 **
 ** @warning This is a dangerous function because the target thread is forced
 **  to prematurely exit without having any chance to execute any user code that
 **  might be remaining
 ** @param t A pointer to the thread in question
 ** @return Returns true in success and false in thread deinit failure
 **
 **/
i_DECLIMEX_ char rfThread_Kill(RF_Thread* t);

/**
 ** @memberof RF_Thread
 ** @brief Suspends execution of the calling thread until the target thread
 **  has terminated
 **
 ** If the target thread is detached in Linux this fails.
 ** @param t A pointer to the thread in question. Can be either RF_Thread
 **  or RF_ThreadX
 ** @return Returns RF_SUCCESS in success and an error code in case of an error.
 **  Possible errors are:
 ** + @c RE_THREAD_NOTJOINABLE: If The parameter thread is not joinable
 ** + @c RE_THREAD_INVALID: The parameter thread is invalid
 ** + @c RE_THREAD_DEADLOCK: A deadlock has been detected due to the attempted joining.
 ** + @c Negative number if there was a generic error in joining
 **/
i_DECLIMEX_ int32_t rfThread_Join(void* t);

/**
 ** @memberof RF_Thread
 ** @brief Returns the data of the thread
 **
 ** @param thread A pointer to the thread in question. Can be either RF_Thread
 ** or RF_ThreadX
 ** @return Returns a @c void* to the Thread's data
 **
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
void* rfThread_GetData(void* thread);
#else
#define rfThread_GetData(i_THREAD)  (i_THREAD)->data
#endif

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end

