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
*/


#ifndef RF_THREADC_H
#define RF_THREADC_H

#include <rf_setup.h>
#ifdef RF_MODULE_THREAD //module check

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

#include <rf_setup.h>

/*
//including windows.h if needed
#ifndef ME_WINDOWS_INCLUDED
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h> //! @testchange THIS HERE SEEMS TO BE INCLUDED ALREADY
    #define ME_WIN32_VERSION
#endif
*/

#ifdef REFU_WIN32_VERSION
    #include <windows.h>
#elif defined(REFU_LINUX_VERSION)
    #include <errno.h> //for all the error signals
    #include <pthread.h> //for POSIX threads
    #include <semaphore.h> //for POSIX semaphores
#endif


/**
** @internal
** @author Lefteris
** @date 28/12/2010
** @endinternal
** @brief A cross-platform Thread Handle
**
** The Thread structure allows
** you to implement multithreaded applications
** indepent of the OS you are using. It offers basic thread functionalities.
** For more extended Thread operations that allow the sending of messages to Threads
** utlizing a message queue take a look at @ref RF_ThreadX
**
** For a nice example of using the Thread take a look at the @ref RF_ThreadingPGE_Sec1 "Refu Threading Documentation"
*/
typedef struct RF_Thread
{
    //! The size of the thread's local memory stack
    uint64_t lmsSize;
    //! The flags of the thread
    uint32_t flags;
    //! The data passed to the thread
    void* data;
    //! Function pointer for behaviour during thread's execution
    void* (*ptr2onExecution )(void*);
#ifdef REFU_WIN32_VERSION
    //! Thread handle in windows
    HANDLE tHandle;
#elif defined(REFU_LINUX_VERSION)
    //! The thread handle in linux
    pthread_t tHandle;
#else
    #error RF_Thread Unsupported Operating System
#endif
}RF_Thread;

/// Thread attributes ///
// This creates a detached thread. Detached thread means that it is of the fire and forget type. You can't join it with the owner thread later.
#define RF_THREAD_DETACHED          0x01
// This is an RF_ThreadX thread, which means that it can accept messages since it has a message queue
#define RF_THRATT_THREADX           0x02

// The function that serves as the starting address for a RF_Thread in win32
#ifdef REFU_WIN32_VERSION
DWORD  WINAPI RF_THREAD_FUNCTION(LPVOID  t);
#elif defined (REFU_LINUX_VERSION)
//  The function that serves as the starting address for a RF_Thread in Linux
void* RF_THREAD_FUNCTION(void* param);
#else
    #error RF_Thread Unsupported Operating System
#endif

//! @memberof RF_Thread
//! @brief Allocates and returns a thread
//!
//! @param flags Bitflags parameters for the thread. Default is 0. Which means an RF_Thread that is joinable.
//! Possible combination of values is:
//! + @c RF_THREAD_DETACHED: This creates a detached thread. Detached thread means that it is of the fire and forget type.
//!      You can't join it with the owner thread later. If this flag is off then a joinable thread is created which means that
//!      you can use rfThread_Join on the thread from the main thread and the main thread's execution will be suspended until this thread
//!      has finished
//! @param ptr2onExecution Give the address of the function you want to run as the main thread's execution function here
//! @param data Give a pointer to the data to be passed to the thread
//! @param lmsSize \rfoptional{RF_OPTION_LOCALSTACKMEMORY_SIZE} The size of the thread's local stack memory. Provide a value here
//! that shall determine the value of this thread's local memory stack size,that will be used for temporary objects initialized by macros
//! such as @ref RFS_() and @ref RFXML_(). If inside that thread such objects are not initialized it is safe to give a small value or even 0
//! to save memory. If no value is given then the default is to create a thread with the same value as that given in compiling with refu builder.
//! @return Returns a pointer to the thread if succesfull or NULL in any other case
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_Thread* rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_ RF_Thread* i_rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
    #define rfThread_Create(...)    RF_SELECT_FUNC(i_SELECT_RF_THREAD_CREATE,__VA_ARGS__)
    #define i_SELECT_RF_THREAD_CREATE3(...) i_rfThread_Create(__VA_ARGS__,RF_OPTION_LOCALSTACKMEMORY_SIZE)
    #define i_SELECT_RF_THREAD_CREATE4(...) i_rfThread_Create(__VA_ARGS__)
#else
    i_DECLIMEX_ RF_Thread* rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
#endif
#endif
//! @memberof RF_Thread
//! @brief Initializes a thread
//!
//! @param t The thread to initialize
//! @param flags Bitflags parameters for the thread. Default is 0. Which means an RF_Thread that is joinable.
//! Possible combination of values is:
//! + @c RF_THREAD_DETACHED: This creates a detached thread. Detached thread means that it is of the fire and forget type.
//!      You can't join it with the owner thread later. If this flag is off then a joinable thread is created which means that
//!      you can use rfThread_Join on the thread from the main thread and the main thread's execution will be suspended until this thread
//!      has finished
//! @param ptr2onExecution Give the address of the function you want to run as the main thread's execution function here
//! @param data Give a pointer to the data to be passed to the thread
//! @param lmsSize \rfoptional{RF_OPTION_LOCALSTACKMEMORY_SIZE} The size of the thread's local stack memory. Provide a value here
//! that shall determine the value of this thread's local memory stack size,that will be used for temporary objects initialized by macros
//! such as @ref RFS_() and @ref RFXML_(). If inside that thread such objects are not initialized it is safe to give a small value or even 0
//! to save memory. If no value is given then the default is to create a thread with the same value as that given in compiling with refu builder.
//! @return Returns true for success and false for failure of initialization
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize);
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_ char i_rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
    #define rfThread_Init(...)    RF_SELECT_FUNC(i_SELECT_RF_THREAD_INIT,__VA_ARGS__)
    #define i_SELECT_RF_THREAD_INIT4(...) i_rfThread_Init(__VA_ARGS__,RF_OPTION_LOCALSTACKMEMORY_SIZE)
    #define i_SELECT_RF_THREAD_INIT5(...) i_rfThread_Init(__VA_ARGS__)
#else
    i_DECLIMEX_ char rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize );
#endif
#endif
//! @memberof RF_Thread
//! @brief Destroys a thread
//!
//! @param t A pointer to the thread in question
i_DECLIMEX_ void rfThread_Destroy(RF_Thread* t);
//! @brief Deinitializes a thread
//!
//! @param t A pointer to the thread in question
i_DECLIMEX_ void rfThread_Deinit(RF_Thread* t);
//! @brief Frees a thread but also immediately causes it to exit.
//!
//! @warning This is a dangerous function because the target thread is forced to prematurely exit without having any chance to execute any user code that might be remaining
//! @param t A pointer to the thread in question
//! @return Returns true in success and false in thread deinit failure
i_DECLIMEX_ char rfThread_Kill(RF_Thread* t);

//! @memberof RF_Thread
//! @brief Suspends execution of the calling thread until the target thread has terminated
//!
//! If the target thread is detached in Linux this fails.
//! @param t A pointer to the thread in question. Can be either RF_Thread or RF_ThreadX
//! @return Returns RF_SUCCESS in success and an error code in case of an error. Possible errors are:
//! + @c RE_THREAD_NOTJOINABLE: If The parameter thread is not joinable
//! + @c RE_THREAD_JOIN: If there was a generic error in Joining
//! + @c RE_THREAD_INVALID: The parameter thread is invalid
//! + @c RE_THREAD_DEADLOCK: A deadlock has been detected due to the attempted joining.
i_DECLIMEX_ int32_t rfThread_Join(void* t);

//! @memberof RF_Thread
//! @brief Returns the data of the thread
//!
//! @param thread A pointer to the thread in question. Can be either RF_Thread or RF_ThreadX
//! @return Returns a @c void* to the Thread's data
#ifdef RF_IAMHERE_FOR_DOXYGEN
void* rfThread_GetData(void* thread);
#else
#define rfThread_GetData(i_THREAD)  (i_THREAD)->data
#endif

//Critical Section exists only in Windows
#ifdef REFU_WIN32_VERSION
/**
** @internal
** @author Lefteris
** @date 29/12/2010
** @endinternal
** @brief A Critical Section Handle for Win32
**
** The Critical section structure denotes
** an area that must be protected because of a shared resource
** that we don't want to be accesses by other parts of code
** Is faster than using a mutex
** @note This structure is useable only in Windows
*/
typedef struct RF_CriticalSection
{
    //! The lock Object for windows
    CRITICAL_SECTION lockObject;
}RF_CriticalSection;

//! @memberof RF_CriticalSection
//! @brief Allocates and returns a critical section
//!
//! @return The initialized Critical Section
i_DECLIMEX_ RF_CriticalSection* rfCriticalSection_Create();
//! @memberof RF_CriticalSection
//! @brief Initializes a critical section
//! @param c The critical section to initialize
//!
//! @return true for success and false otherwise
i_DECLIMEX_ char rfCriticalSection_Init(RF_CriticalSection* c);

//! @memberof RF_CriticalSection
//! @brief Destroys a critical section and frees its memory
//!
//! @param C The critical section in question
i_DECLIMEX_ void rfCriticalSection_Destroy(RF_CriticalSection* C);
//! @memberof RF_CriticalSection
//! @brief Deinitializes a critical section
//!
//! @param C The critical section in question
i_DECLIMEX_ void rfCriticalSection_Deinit(RF_CriticalSection* C);

//! @memberof RF_CriticalSection
//! @brief Enters a Critical Section. The function returns only when the calling thread is granted ownership of the section
//!
//! @param C The critical section in question
i_DECLIMEX_ void rfCriticalSection_Enter(RF_CriticalSection* C);

//! @memberof RF_CriticalSection
//! @brief Attempts to enter a critical section without locking the calling thread.
//!
//! @param C The critical section in question
//! @return Returns true if the critical section has been succesfully entered or the calling thread already owns it. If not it returns false.
i_DECLIMEX_ char rfCriticalSection_Try(RF_CriticalSection* C);

//! @memberof RF_CriticalSection
//! @brief Leaves a critical Section
//!
//! @param C The critical section in question
i_DECLIMEX_ void rfCriticalSection_Leave(RF_CriticalSection* C);
#endif//end of Windows only Critical section



/**
** @internal
** @author Lefteris
** @date 04/02/2012
** @endinternal
** @brief A cross-platform Mutex Handle
**
** The Mutex struct is an object that can be held by any other thread
** and either be locked by it or released. If it is locked by any
** one thread other threads have to wait for it. Beware of racing situations.
**
** For a detailed example of Mutex usage take a look at the @ref RF_ThreadingPGE_Sec2 "Refu Threading Documentation"
*/
typedef struct RF_Mutex
{
#ifdef REFU_WIN32_VERSION
    //! The windows mutex
    HANDLE mutexObj;
#elif defined (REFU_LINUX_VERSION)
    //! The posix mutex
    pthread_mutex_t mutexObj;
#else
    #error Not yet implemented OS Solution
#endif
}RF_Mutex;

// Values for the flags parameters of a mutex creation//
// The mutex object will be locked at the beginning and owned by the calling thread
#define RF_MUTEX_LOCK_AT_INIT       0x01
// This is a hint and will only have an effect under Linux which use pthreads. The effect is as if calling PTHREAD_MUTEX_NORMAL attribute at creation which means that when you attempt to lock
// the mutex when owned by another thread, the calling thread is suspended forever.
#define RF_MUTEX_NORMAL             0x02
// This is a hint and will only have an effect under Linux which use pthreads. The effect is as if calling PTHREAD_MUTEX_RECURSIVE attribute at creation which means that when you attempt to lock
// the mutex when owned by another thread, the calling thread returns immediately with a success return code. The number of times the thread owning the mutex has locked it is recorded in the mutex.
// The owning thread must call rfThread_Unlcosk  the same number of times before the mutex returns to the unlocked state.
#define RF_MUTEX_RECURSIVE          0x04
// This is a hint and will only have an effect under Linux which use pthreads. The effect is as if calling PTHREAD_MUTEX_ERRORCHECK attribute at creation which means that when you attempt to lock
// the mutex when owned by another thread, the calling thread returns immediately with the error code RF_MUTEX_DEADLOCK
#define RF_MUTEX_ERRORCHECK         0x08

//! @memberof RF_Mutex
//! @brief Allocates and returns a mutex object
//!
//! @param flags Hints for the creation of the mutex. Possible bitflag values/combinations:
//! <ul><li><b>For all OS</b>: @c RF_MUTEX_LOCK_AT_INIT, means that the mutex object will be locked at the beginning and owned by the calling thread</li>
//! <li><b>For Linux</b>:
//! <ul><li>@c RF_MUTEX_NORMAL, The effect is as if calling @c PTHREAD_MUTEX_NORMAL attribute at creation which means that when you attempt to lock
//! the mutex when owned by another thread, the calling thread is suspended forever</li>
//! <li>@c RF_MUTEX_RECURSIVE, The effect is as if calling @c PTHREAD_MUTEX_RECURSIVE attribute at creation which means that when you attempt to lock
//! the mutex when owned by another thread, the calling thread returns immediately with a success return code. The number of times the thread owning the mutex has locked it is recorded in the mutex.
//! The owning thread must call @ref rfMutex_Unlock the same number of times before the mutex returns to the unlocked state</li>
//! <li>@c RF_MUTEX_ERRORCHECK, The effect is as if calling @c PTHREAD_MUTEX_ERRORCHECK attribute at creation which means that when you attempt to lock
//! the mutex when owned by another thread, the calling thread returns immediately with the error code RF_MUTEX_DEADLOCK</li></ul>
//! </li></ul>
//! @return Returns the newly allocated mutex for success or false for failure
i_DECLIMEX_ RF_Mutex* rfMutex_Create(uint32_t flags);
//! @memberof RF_Mutex
//! @brief Initializes a mutex object
//!
//! @param m The mutex to initialize
//! @param flags Hints for the creation of the mutex. Possible bitflag values/combinations:
//! <ul><li><b>For all OS</b>: @c RF_MUTEX_LOCK_AT_INIT, means that the mutex object will be locked at the beginning and owned by the calling thread</li>
//! <li><b>For Linux</b>:
//! <ul><li>@c RF_MUTEX_NORMAL, The effect is as if calling @c PTHREAD_MUTEX_NORMAL attribute at creation which means that when you attempt to lock
//! the mutex when owned by another thread, the calling thread is suspended forever</li>
//! <li>@c RF_MUTEX_RECURSIVE, The effect is as if calling @c PTHREAD_MUTEX_RECURSIVE attribute at creation which means that when you attempt to lock
//! the mutex when owned by another thread, the calling thread returns immediately with a success return code. The number of times the thread owning the mutex has locked it is recorded in the mutex.
//! The owning thread must call @ref rfMutex_Unlock the same number of times before the mutex returns to the unlocked state</li>
//! <li>@c RF_MUTEX_ERRORCHECK, The effect is as if calling @c PTHREAD_MUTEX_ERRORCHECK attribute at creation which means that when you attempt to lock
//! the mutex when owned by another thread, the calling thread returns immediately with the error code RF_MUTEX_DEADLOCK</li></ul>
//! </li></ul>
//! @return RF_SUCCESS for success and an error otherwise. Possible error values are:
//! + @c RE_MUTEX_INIT: Generix mutex init error
//! + @c RE_INSUFFICIENT_RESOURCES: Insufficient system resources to allocate a new mutex (POSIX only)
//! + @c RE_INSUFFICIENT_MEMORY: Insufficient system memory to allocate a new mutex (POSIX only)
//! + @c RE_PERMISSION: Insufficient permission to initialize a new mutex (POSIX only)
//! + @c RE_MUTEX_INVALID: The given mutex value is invalid
i_DECLIMEX_ int32_t rfMutex_Init(RF_Mutex* m,uint32_t flags);

//! @memberof RF_Mutex
//! @brief Destroys the mutex and releases its memory
//!
//! Make sure that it is not owned by any thread at the time you use this function
//! @param m The mutex in question
//! @return Returns true in succesfull destruction and false with error logging otherwise
i_DECLIMEX_ char rfMutex_Destroy(RF_Mutex* m);
//! @memberof RF_Mutex
//! @brief Deinitializes the mutex
//!
//! Make sure that it is not owned by any thread at the time you use this function
//! @param m The mutex in question
//! @return Returns true in succesfull destruction and false with error logging otherwise
i_DECLIMEX_ char rfMutex_Deinit(RF_Mutex* m);


//! @memberof RF_Mutex
//! @brief Locks the mutex
//!
//! If the mutex is already locked by another thread then this suspends the calling thread until the mutex is unlocked. Behaviour will only vary if
//! The system is in Linux and using POSIX threads where the behaviour will depend on the flags given at mutex initialization.
//! @param m The mutex in question
//! @return Returns @c RF_SUCCESS in case of success and error otherwise. Possible error values are:
//! + @c RE_MUTEX_LOCK: For generic mutex lock error
//! + @c RE_MUTEX_LOCK_INVALID: For the mutex value being invalid or being unable to get locked due to priorities
//! + @c RE_MUTEX_LOCK_NUM: Error because the maximum number of recursive locks for the mutex has been reached. (POSIX only)
//! + @c RE_MUTEX_DEADLOCK: Error because the current thread already owns the mutex (POSIX only)
i_DECLIMEX_ int32_t rfMutex_Lock(RF_Mutex* m);

//! @memberof RF_Mutex
//! @brief Locks the mutex but with a time limit
//!
//! This function works in the same fashion as the @ref rfMutex_Lock function except that if the mutex is already locked then the calling thread is suspended only for the
//! amount of time given as the argument to this function
//! @param m The mutex in question
//! @param ms The millisecond to wait for the mutex to unlock if it's already locked
//! @return Returns @c RF_SUCCESS in case of success or error. The possible errors are:
//! + @c RE_MUTEX_LOCK: For generic mutex lock error
//! + @c RE_MUTEX_LOCK_INVALID: For the mutex value being invalid or being unable to get locked due to priorities
//! + @c RE_MUTEX_TIMEOUT: The @c ms timeout value has elapsed
//! + @c RE_MUTEX_LOCK_NUM: Error because the maximum number of recursive locks for the mutex has been reached. (POSIX only)
//! + @c RE_MUTEX_DEADLOCK: Error because the current thread already owns the mutex (POSIX only)
i_DECLIMEX_ int32_t rfMutex_TimedLock(RF_Mutex* m,uint32_t ms);

//! @memberof RF_Mutex
//! @brief Tries to Lock the mutex
//!
//! If the mutex is already locked by another thread then an error code is returned.
//! @param m The mutex in question
//! @return Returns RF_SUCCESS in case of success and error otherwise. Possible errors are:
//! + @c RE_MUTEX_LOCK: For generic mutex lock error
//! + @c RE_MUTEX_LOCK_INVALID: For the mutex value being invalid or being unable to get locked due to priorities
//! + @c RE_MUTEX_BUSY: If the mutex is already locked by another thread.
//! + @c RE_MUTEX_LOCK_NUM: Error because the maximum number of recursive locks for the mutex has been reached. (POSIX only)
//! + @c RE_MUTEX_DEADLOCK: Error because the current thread already owns the mutex (POSIX only)
i_DECLIMEX_ int32_t rfMutex_TryLock(RF_Mutex* m);

//! @memberof RF_Mutex
//! @brief Unlocks and releases ownership of a mutex object
//! @param m The mutex in question
//! @return Returns @c RF_SUCCESS in case of success and @c RE_MUTEX_UNLOCK otherwise
i_DECLIMEX_  int32_t rfMutex_Unlock(RF_Mutex* m);


/**
** @internal
** @author Lefteris
** @date 04/02/2012
** @endinternal
** @brief A cross-platform Semaphore Handle
**
** The Semaphore struct is an object that can provide a count of
** how many units of a particular resource are available and provides
** operations to safely adjust the number of units as they become free
** or are required.
**
** A semaphore's state is signaled when its count is greater than or equal
** to zero and non signaled when it's zero. The count is decreased by 1 whenever
** a thread releases the object by waiting
**
** For an example of Semaphore usage visit the @ref RF_ThreadingPGE_Sec3 "Refu Threading Documentation"
*/
typedef struct RF_Semaphore
{
#ifdef REFU_WIN32_VERSION
    //! The semaphore object for windows
    HANDLE semaphoreObj;
#elif defined(REFU_LINUX_VERSION)
    //! Posix semaphore object
    sem_t semaphoreObj;
#endif
}RF_Semaphore;

//! @memberof RF_Semaphore
//! @brief Allocates and returns a semaphore object
//!
//! @param initialCount The initial count for the semaphore. Must be greater than or equal to zero and less than or equal to maxCount.
//! @param maxCount The maximum count for the semaphore. Is a Windows only option. Has no meaning in Linux using POSIX threads, just kept for compatibility with Windows.
//! @return RF_SUCCESS for succesful initialization and an error otherwise. Possible errors are:
//! + @c RE_SEMAPHORE_INIT: Generic Semaphore initialization error
//! + @c RE_SEMAPHORE_MAXCOUNT: Error because the given initial count exceeds the maximum semaphore count of the system (POSIX only)
//! + @c RE_INSUFFICIENT_RESOURCES: Error because the system did not have enough resources to allocate another semaphore (POSIX only)
//! + @c RE_PERMISSION: Failed because the process lacks the appropriate privileges to initialise the semaphore.
i_DECLIMEX_ RF_Semaphore* rfSemaphore_Create(uint32_t initialCount,uint32_t maxCount);
//! @memberof RF_Semaphore
//! @brief Initializes a semaphore object
//!
//! @param s The semaphore to initialize
//! @param initialCount The initial count for the semaphore. Must be greater than or equal to zero and less than or equal to maxCount.
//! @param maxCount The maximum count for the semaphore. Is a Windows only option. Has no meaning in Linux using POSIX threads, just kept for compatibility with Windows.
//! @return RF_SUCCESS for succesful initialization and an error otherwise. Possible errors are:
//! + @c RE_SEMAPHORE_INIT: Generic Semaphore initialization error
//! + @c RE_SEMAPHORE_MAXCOUNT: Error because the given initial count exceeds the maximum semaphore count of the system (POSIX only)
//! + @c RE_INSUFFICIENT_RESOURCES: Error because the system did not have enough resources to allocate another semaphore (POSIX only)
//! + @c RE_PERMISSION: Failed because the process lacks the appropriate privileges to initialise the semaphore. (POSIX only)
//! + @c RE_UNSUPPORTED: Semaphores are not supported by the system (POSIX only)
i_DECLIMEX_ int32_t rfSemaphore_Init(RF_Semaphore* s,uint32_t initialCount,uint32_t maxCount);

//! @memberof RF_Semaphore
//! @brief Destroys a semaphore object and frees its memory
//!
//! Be careful not to use this if the semaphore is still being waited for by any other active thread
//! @param s The semaphore object
//! @return Returns true in succesfull destruction and false with error logging otherwise
i_DECLIMEX_ char rfSemaphore_Destroy(RF_Semaphore* s);
//! @memberof RF_Semaphore
//! @brief Deinitializes a semaphore
//!
//! Be careful not to use this if the semaphore is still being waited for by any other active thread
//! @param s The semaphore object
//! @return Returns true in succesfull destruction and false with error logging otherwise
i_DECLIMEX_ char rfSemaphore_Deinit(RF_Semaphore* s);

//! @memberof RF_Semaphore
//! @brief Waits for the semaphore
//!
//! If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function hangs and waits
//! until some other thread releases the semaphore so that its count increases again.
//! @param s The semaphore in question
//! @return Returns @c RF_SUCCESS for succesfull waiting or an error otherwise. Possible error values are:
//! + @c RE_SEMAPHORE_WAIT: For a generic error during semaphore waiting
//! + @c RE_SEMAPHORE_WAIT_INVALID: For the semaphore value being invalid or being unable to be operated on due to priorities
//! + @c RE_UNSUPPORTED: Semaphore waiting is not supported by the system (POSIX only)
//! + @c RE_SEMAPHORE_DEADLOCK: Could not wait for the semaphore because a deadlock condition was detected. (POSIX only)
//! + @c RE_INTERRUPT: Failure during waiting for a semaphore because of a signal interruption (POSIX only)
i_DECLIMEX_ int32_t rfSemaphore_Wait(RF_Semaphore* s);

//! @memberof RF_Semaphore
//! @brief Waits for the semaphore with a time limit
//!
//! This functionc acts like rfSemaphore_Wait except that there is a timeout to waiting given as an argument
//! @param s The semaphore in question
//! @param ms The number of milliseconds to wait
//! @return Returns @c RF_SUCCESS for succesfull waiting and error otherwise. Possible error values are:
//! + @c RE_SEMAPHORE_WAIT: For a generic error during semaphore waiting
//! + @c RE_SEMAPHORE_WAIT_INVALID: For the semaphore value being invalid or being unable to be operated on due to priorities
//! + @c RE_SEMAPHORE_TIMEOUT: The amount of milliseconds given in @c ms has expired
//! + @c RE_UNSUPPORTED: Semaphore waiting is not supported by the system (POSIX only)
//! + @c RE_SEMAPHORE_DEADLOCK: Could not wait for the semaphore because a deadlock condition was detected. (POSIX only)
//! + @c RE_INTERRUPT: Failure during waiting for a semaphore because of a signal interruption (POSIX only)
i_DECLIMEX_ int32_t rfSemaphore_TimedWait(RF_Semaphore* s,uint32_t ms);

//! @memberof RF_Semaphore
//! @brief Tries to wait for the semaphore
//!
//! If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function returns RF_SEMAPHORE_BUSY
//! @param s The semaphore in question
//! @return Returns @c RF_SUCCESS for succesfull waiting and error otherwise. Possible error values are:
//! + @c RE_SEMAPHORE_WAIT: For a generic error during semaphore waiting
//! + @c RE_SEMAPHORE_WAIT_INVALID: For the semaphore value being invalid or being unable to be operated on due to priorities
//! + @c RE_SEMAPHORE_BUSY: The count of the semaphore is at zero
//! + @c RE_UNSUPPORTED: Semaphore waiting is not supported by the system (POSIX only)
//! + @c RE_SEMAPHORE_DEADLOCK: Could not wait for the semaphore because a deadlock condition was detected. (POSIX only)
//! + @c RE_INTERRUPT: Failure during waiting for a semaphore because of a signal interruption (POSIX only)
i_DECLIMEX_ int32_t rfSemaphore_TryWait(RF_Semaphore* s);

//! @memberof RF_Semaphore
//! @brief Releases ownerhship of a semaphore object, increasing its count by one
//!
//! @param s The semaphore object
//! @return Returns @c RF_SUCCESS for success and an error otherwise. Possible errors are:
//! + @c RE_SEMAPHORE_POST: Generic error during semaphore posting
//! + @c RE_SEMAPHORE_POST_INVALID: Invalid semaphore value provided to the post function (POSIX only)
//! + @c RE_UNSUPPORTED: Semaphore posting is not supported by the system (POSIX only)
i_DECLIMEX_  int32_t rfSemaphore_Post(RF_Semaphore* s);


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#else //module check ends
    #error Attempted to include Refu Threading without the appropriate flag module on. Rebuild the library with that option on if you need to use them.
#endif

#endif//include guards end

