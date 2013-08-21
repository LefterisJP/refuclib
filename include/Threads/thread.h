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
**
---------------------For internal library include make sure to have----------------------------
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
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_THREAD_H
#define RF_THREAD_H

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
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_Thread* rfThread_Create(uint32_t flags,
                                       void* (*ptr2onExecution)(void*),
                                       void* data, uint64_t lmsSize );
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_ RF_Thread* i_rfThread_Create(
        uint32_t flags,
        void* (*ptr2onExecution)(void*),
        void* data, uint64_t lmsSize );
    #define rfThread_Create(...)    RF_SELECT_FUNC(i_SELECT_RF_THREAD_CREATE,__VA_ARGS__)
    #define i_SELECT_RF_THREAD_CREATE3(...) i_rfThread_Create(__VA_ARGS__,RF_OPTION_LOCALSTACK_MEMORY_SIZE)
    #define i_SELECT_RF_THREAD_CREATE4(...) i_rfThread_Create(__VA_ARGS__)
#else
    i_DECLIMEX_ RF_Thread* rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
#endif
#endif
/**
 ** @memberof RF_Thread
 ** @brief Initializes a thread
 **
 ** @param t The thread to initialize
 ** @param flags Bitflags parameters for the thread. Default is 0. Which means
 ** an RF_Thread that is joinable.
 ** Possible combination of values is:
 ** + @c RF_THREAD_DETACHED: This creates a detached thread. Detached thread
 **      means that it is of the fire and forget type. You can't join it with
 **      the owner thread later. If this flag is off then a joinable thread
 **      is created which means that you can use rfThread_Join on the thread
 **      from the main thread and the main thread's execution will be suspended
 **      until this thread has finished
 ** @param ptr2onExecution Give the address of the function you want to run as
 ** the main thread's execution function here
 ** @param data Give a pointer to the data to be passed to the thread
 ** @param lmsSize \rfoptional{RF_OPTION_LOCALSTACK_MEMORY_SIZE} The size of the
 ** thread's local stack memory. Provide a value here
 ** that shall determine the value of this thread's local memory stack size, 
 ** that will be used for temporary objects initialized by macros
 ** such as @ref RFS_() and @ref RFXML_(). If inside that thread such objects 
 ** are not initialized it is safe to give a small value or even 0
 ** to save memory. If no value is given then the default is to create a thread
 **  with the same value as that given in compiling with refu builder.
 ** @return Returns true for success and false for failure of initialization
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize);
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_ char i_rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
    #define rfThread_Init(...)    RF_SELECT_FUNC(i_SELECT_RF_THREAD_INIT,__VA_ARGS__)
    #define i_SELECT_RF_THREAD_INIT4(...) i_rfThread_Init(__VA_ARGS__,RF_OPTION_LOCALSTACK_MEMORY_SIZE)
    #define i_SELECT_RF_THREAD_INIT5(...) i_rfThread_Init(__VA_ARGS__)
#else
    i_DECLIMEX_ char rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
#endif
#endif

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

