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
** --Threads/threadx.h
** This header declares the functions and macros to be used with RF_ThreadX
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
    #ifdef REFU_LINUX_VERSION
    #include <Threads/mutex_decl.h> //for RF_Mutex used in RF_ThreadX
    #endif
#include <Threads/threadx_decl.h> //for RF_ThreadX
#include <Threads/threadx.h>
---------------------For internal library include make sure to have----------------------------
*/


#ifndef RF_THREADX_H
#define RF_THREADX_H

#include <rf_options.h>
#ifdef RF_MODULE_THREADX //module check

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @memberof RF_ThreadX
//! @brief Allocates and returns a ThreadX
//!
//! @param flags Bitflags parameters for the thread. Default is 0. Which means an RF_Thread that is joinable.
//! Possible combination of values is:
//! + @c RF_THREAD_DETACHED: This creates a detached thread. Detached thread means that it is of the fire and forget type.
//!      You can't join it with the owner thread later. If this flag is off then a joinable thread is created which means that
//!      you can use rfThread_Join on the thread from the main thread and the main thread's execution will be suspended until this thread
//!      has finished
//! @param ptr2onExecution Give the address of the function you want to run as the main thread's execution function here. Note, the function must be accepting one parameter and that is a pointer to RF_ThreadX
//! @param data Give a pointer to the data to be passed to the thread. They can later be accessed by the thread at the main thread function by calling rfThreadX_GetData()
//! @param lmsSize \rfoptional{RF_OPTION_LOCALSTACKMEMORY_SIZE} The size of the thread's local stack memory. Provide a value here
//! that shall determine the value of this thread's local memory stack size,that will be used for temporary objects initialized by macros
//! such as @ref RFS_() and @ref RFXML_(). If inside that thread such objects are not initialized it is safe to give a small value or even 0
//! to save memory. If no value is given then the default is to create a thread with the same value as that given in compiling with refu builder.
//! @param maxMsgQueue \rfoptional{RF_OPTION_THREADX_MSGQUEUE} The maximum number of signals that can be waiting in the thread's message queue. If no
//! value is given then the value of the macro @c RF_OPTION_THREADX_MSGQUEUE that was defined by refu builder during compiling is used.
//! @return Returns a pointer to the thread if succesfull or NULL in any other case
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_ThreadX* rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxMsgQueue );
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_ RF_ThreadX* i_rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxMsgQueue );
    #define rfThreadX_Create(...) RF_SELECT_FUNC(i_SELECT_RF_THREADX_CREATE,__VA_ARGS__)
    #define i_SELECT_RF_THREADX_CREATE3(...)    i_rfThreadX_Create(__VA_ARGS__,RF_OPTION_LOCALSTACKMEMORY_SIZE,RF_OPTION_THREADX_MSGQUEUE)
    #define i_SELECT_RF_THREADX_CREATE4(...)    i_rfThreadX_Create(__VA_ARGS__,RF_OPTION_THREADX_MSGQUEUE)
    #define i_SELECT_RF_THREADX_CREATE5(...)    i_rfThreadX_Create(__VA_ARGS__)
#else
    i_DECLIMEX_ RF_ThreadX* rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxMsgQueue );
#endif
#endif
//! @memberof RF_ThreadX
//! @brief Initializes a ThreadX
//!
//! @param t The thread to initialize
//! @param flags Bitflags parameters for the thread. Default is 0. Which means an RF_Thread that is joinable.
//! Possible combination of values is:
//! + @c RF_THREAD_DETACHED: This creates a detached thread. Detached thread means that it is of the fire and forget type.
//!      You can't join it with the owner thread later. If this flag is off then a joinable thread is created which means that
//!      you can use rfThread_Join on the thread from the main thread and the main thread's execution will be suspended until this thread
//!      has finished
//! @param ptr2onExecution Give the address of the function you want to run as the main thread's execution function here.  Note, the function must be accepting one parameter and that is a pointer to RF_ThreadX
//! @param data Give a pointer to the data to be passed to the thread. They can later be accessed by the thread at the main thread function by calling rfThreadX_GetData()
//! @param lmsSize \rfoptional{RF_OPTION_LOCALSTACKMEMORY_SIZE} The size of the thread's local stack memory. Provide a value here
//! that shall determine the value of this thread's local memory stack size,that will be used for temporary objects initialized by macros
//! such as @ref RFS_() and @ref RFXML_(). If inside that thread such objects are not initialized it is safe to give a small value or even 0
//! to save memory. If no value is given then the default is to create a thread with the same value as that given in compiling with refu builder.
//! @param maxMsgQueue \rfoptional{RF_OPTION_THREADX_MSGQUEUE} The maximum number of signals that can be waiting in the thread's message queue. If no
//! value is given then the value of the macro @c RF_OPTION_THREADX_MSGQUEUE that was defined by refu builder during compiling is used.
//! @return Returns true for success and false for failure of initialization
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data ,uint64_t lmsSize,uint32_t maxMsgQueue);
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_ char i_rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data ,uint64_t lmsSize,uint32_t maxMsgQueue);
    #define rfThreadX_Init(...) RF_SELECT_FUNC(i_SELECT_RF_THREADX_INIT,__VA_ARGS__)
    #define i_SELECT_RF_THREADX_INIT4(...)  i_rfThreadX_Init(__VA_ARGS__,RF_OPTION_LOCALSTACKMEMORY_SIZE,RF_OPTION_THREADX_MSGQUEUE)
    #define i_SELECT_RF_THREADX_INIT5(...)  i_rfThreadX_Init(__VA_ARGS__,RF_OPTION_THREADX_MSGQUEUE)
    #define i_SELECT_RF_THREADX_INIT6(...)  i_rfThreadX_Init(__VA_ARGS__)
#else
    i_DECLIMEX_ char rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data ,uint64_t lmsSize,uint32_t maxMsgQueue);
#endif
#endif

//! @memberof RF_ThreadX
//! @brief Destroys a ThreadX
//!
//! @param t The Thread to destroy
i_DECLIMEX_ void rfThreadX_Destroy(RF_ThreadX* t);
//! @memberof RF_ThreadX
//! @brief Deinitializes a ThreadX
//!
//! @param t The Thread to deinit
i_DECLIMEX_ void rfThreadX_Deinit(RF_ThreadX* t);
//! @memberof RF_ThreadX
//! @brief Frees a thread but also immediately causes it to exit.
//!
//! @warning This is a dangerous function because the target thread is forced to prematurely exit without having any chance to execute any user code that might be remaining
//! @param t A pointer to the thread in question
//! @return Returns true in success and false in thread deinit failure
i_DECLIMEX_ char rfThreadX_Kill(RF_ThreadX* t);


//! @memberof RF_ThreadX
//! @brief Gets a pointer to the data passed to the thread during initialization
//!
//! @param t The thread
//! @return The data passed to the thread during initialization
void* rfThreadX_GetData(RF_ThreadX* t);

//! @memberof RF_ThreadX
//! @brief Attempts to send a signal from the current thread to the target RF_ThreadX
//!
//! If the message queue is currently being used then RF_THREADX_QUEUE_BUSY is returned
//! @param t A pointer to the thread that will receive the signal.The thread must be able to receive signals
//! @param data A pointer to the data to send
//! @param size The size of the data we are sending in bytes
//! @return Returns @c RF_SUCCESS in case of success
//! In case of error it returns:
//!  <ul><li>@c RF_THREADX_QUEUE_BUSY if the queue is being written at the moment of accesss</li>
//!  <li>@c RF_THREADX_QUEUE_FULL if the queue is full</li>
//!  <li>@c RF_THREADX_QUEUE_ERROR if there was a problem</li></ul>
i_DECLIMEX_ int32_t rfThreadX_TrySendSignal(RF_ThreadX* t,void* data,uint32_t size);

//! @memberof RF_ThreadX
//! @brief Sends a signal from the current thread to the target RF_ThreadX
//!
//! If the message queue is currently being used the thrad will hang and wait until it's unlocked and its turn comes
//! @param t A pointer to the thread that will receive the signal.The thread must be able to receive signals
//! @param data A pointer to the data to send
//! @param size The size of the data we are sending in bytes
//! @return Returns @c RF_SUCCESS in case of success. If an error happens it returns
//! <ul><li>@c RF_THREADX_QUEUE_FULL if the queue is full</li>
//! <li>@c RF_THREADX_QUEUE_ERROR if there was a problem</li></ul>
i_DECLIMEX_ int32_t rfThreadX_SendSignal(RF_ThreadX* t,void* data,uint32_t size);

//! @memberof RF_ThreadX
//! @brief Attempts to read the message queue of the thread for signals
//!
//! If the message queue is currently being used the thread will hang and wait until it's unlocked and its turn comes.
//! @param[in]  t A pointer to this thread
//! @param[out] data A pointer to the data that we want to receive. Since the programmer is responsible for the data, they should already have been allocated
//! @param[out] size Pass a reference to an uint32_t to receive here the size of the data in bytes.
//! @return     Returns @c RF_SUCCESS in case of success. If something happens it returns:
//! <ul><li>@c RF_THREADX_QUEUE_EMPTY if the queue is empty</li>
//! <li>@c RF_THREADX_QUEUE_ERROR if there was a problem</li></ul>
i_DECLIMEX_ int32_t rfThreadX_ReadSignal(RF_ThreadX* t,void *data,uint32_t* size);

//! @memberof RF_ThreadX
//! @brief Reads the message queue of the thread for signals
//!
//! If the message queue is currently being used the thread then RF_THREADX_QUEUE_BUSY is returned
//! @param[in]  t A pointer to this thread
//! @param[out] data A pointer to the data that we want to receive. Since the programmer is responsible for the data, they should already have been allocated
//! @param[out] size Pass a reference to an uint32_t to receive here the size of the data in bytes.
//! @return     Returns @c RF_SUCCESS in case of success.If something happens it can return:
//! <ul><li>@c RF_THREADX_QUEUE_BUSY if the queue is being written at the moment of accesss</li>
//! <li>@c RF_THREADX_QUEUE_EMPTY if the queue is empty</li>
//! <li>@c RF_THREADX_QUEUE_ERROR if there was a problem</li></ul>
i_DECLIMEX_ int32_t rfThreadX_TryReadSignal(RF_ThreadX* t,void *data,uint32_t* size);

#ifdef __cplusplus
}//closing brack for C++ calling
#endif

#else //RF_ThreadX module check ends
    #error #error Attempted to include Refu ThreadX without the appropriate flag module on. Rebuild the library with that option on if you need to use them.
#endif

#endif //include guards end
