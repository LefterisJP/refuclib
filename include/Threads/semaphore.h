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
** -Threads/semaphore.h
** This header declares the functions that operate on RF_Semaphore
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
    #ifdef REFU_WIN32_VERSION
    #include <windows.h> //for HANDLE
    #else
    #include <semaphore.h> //for pthread_semaphores
    #endif
#include <Threads/semaphore_decl.h> //for RF_Semaphore
#include <Threads/semaphore.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_SEMAPHORE_H
#define RF_SEMAPHORE_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
 ** @memberof RF_Semaphore
 ** @brief Allocates and returns a semaphore object
 **
 ** @param initialCount The initial count for the semaphore. Must be greater than or equal to zero and less than or equal to maxCount.
 ** @param maxCount The maximum count for the semaphore. Is a Windows only option. Has no meaning in Linux using POSIX threads, just kept for compatibility with Windows.
 ** @return RF_SUCCESS for succesful initialization and an error otherwise. Possible errors are:
 ** + @c RE_SEMAPHORE_INIT: Generic Semaphore initialization error
 ** + @c RE_SEMAPHORE_MAXCOUNT: Error because the given initial count exceeds the maximum semaphore count of the system (POSIX only)
 ** + @c RE_INSUFFICIENT_RESOURCES: Error because the system did not have enough resources to allocate another semaphore (POSIX only)
 ** + @c RE_PERMISSION: Failed because the process lacks the appropriate privileges to initialise the semaphore.
 **
 **/
i_DECLIMEX_ RF_Semaphore* rfSemaphore_Create(uint32_t initialCount,uint32_t maxCount);
/**
 ** @memberof RF_Semaphore
 ** @brief Initializes a semaphore object
 **
 ** @param s The semaphore to initialize
 ** @param initialCount The initial count for the semaphore. Must be greater than or equal to zero and less than or equal to maxCount.
 ** @param maxCount The maximum count for the semaphore. Is a Windows only option. Has no meaning in Linux using POSIX threads, just kept for compatibility with Windows.
 ** @return RF_SUCCESS for succesful initialization and an error otherwise. Possible errors are:
 ** + @c RE_SEMAPHORE_INIT: Generic Semaphore initialization error
 ** + @c RE_SEMAPHORE_MAXCOUNT: Error because the given initial count exceeds the maximum semaphore count of the system (POSIX only)
 ** + @c RE_INSUFFICIENT_RESOURCES: Error because the system did not have enough resources to allocate another semaphore (POSIX only)
 ** + @c RE_PERMISSION: Failed because the process lacks the appropriate privileges to initialise the semaphore. (POSIX only)
 ** + @c RE_UNSUPPORTED: Semaphores are not supported by the system (POSIX only)
 **
 **/
i_DECLIMEX_ int32_t rfSemaphore_Init(RF_Semaphore* s,uint32_t initialCount,uint32_t maxCount);

/**
 ** @memberof RF_Semaphore
 ** @brief Destroys a semaphore object and frees its memory
 **
 ** Be careful not to use this if the semaphore is still being waited for by any other active thread
 ** @param s The semaphore object
 ** @return Returns true in succesfull destruction and false with error logging otherwise
 **
 **/
i_DECLIMEX_ char rfSemaphore_Destroy(RF_Semaphore* s);
/**
 ** @memberof RF_Semaphore
 ** @brief Deinitializes a semaphore
 **
 ** Be careful not to use this if the semaphore is still being waited for by any other active thread
 ** @param s The semaphore object
 ** @return Returns true in succesfull destruction and false with error logging otherwise
 **
 **/
i_DECLIMEX_ char rfSemaphore_Deinit(RF_Semaphore* s);

/**
 ** @memberof RF_Semaphore
 ** @brief Waits for the semaphore
 **
 ** If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function hangs and waits
 ** until some other thread releases the semaphore so that its count increases again.
 ** @param s The semaphore in question
 ** @return Returns @c RF_SUCCESS for succesfull waiting or an error otherwise. Possible error values are:
 ** + @c RE_SEMAPHORE_WAIT: For a generic error during semaphore waiting
 ** + @c RE_SEMAPHORE_WAIT_INVALID: For the semaphore value being invalid or being unable to be operated on due to priorities
 ** + @c RE_UNSUPPORTED: Semaphore waiting is not supported by the system (POSIX only)
 ** + @c RE_SEMAPHORE_DEADLOCK: Could not wait for the semaphore because a deadlock condition was detected. (POSIX only)
 ** + @c RE_INTERRUPT: Failure during waiting for a semaphore because of a signal interruption (POSIX only)
 **
 **/
i_DECLIMEX_ int32_t rfSemaphore_Wait(RF_Semaphore* s);

/**
 ** @memberof RF_Semaphore
 ** @brief Waits for the semaphore with a time limit
 **
 ** This functionc acts like rfSemaphore_Wait except that there is a timeout to waiting given as an argument
 ** @param s The semaphore in question
 ** @param ms The number of milliseconds to wait
 ** @return Returns @c RF_SUCCESS for succesfull waiting and error otherwise. Possible error values are:
 ** + @c RE_SEMAPHORE_WAIT: For a generic error during semaphore waiting
 ** + @c RE_SEMAPHORE_WAIT_INVALID: For the semaphore value being invalid or being unable to be operated on due to priorities
 ** + @c RE_SEMAPHORE_TIMEOUT: The amount of milliseconds given in @c ms has expired
 ** + @c RE_UNSUPPORTED: Semaphore waiting is not supported by the system (POSIX only)
 ** + @c RE_SEMAPHORE_DEADLOCK: Could not wait for the semaphore because a deadlock condition was detected. (POSIX only)
 ** + @c RE_INTERRUPT: Failure during waiting for a semaphore because of a signal interruption (POSIX only)
 **
 **/
i_DECLIMEX_ int32_t rfSemaphore_TimedWait(RF_Semaphore* s,uint32_t ms);

/**
 ** @memberof RF_Semaphore
 ** @brief Tries to wait for the semaphore
 **
 ** If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function returns RF_SEMAPHORE_BUSY
 ** @param s The semaphore in question
 ** @return Returns @c RF_SUCCESS for succesfull waiting and error otherwise. Possible error values are:
 ** + @c RE_SEMAPHORE_WAIT: For a generic error during semaphore waiting
 ** + @c RE_SEMAPHORE_WAIT_INVALID: For the semaphore value being invalid or being unable to be operated on due to priorities
 ** + @c RE_SEMAPHORE_BUSY: The count of the semaphore is at zero
 ** + @c RE_UNSUPPORTED: Semaphore waiting is not supported by the system (POSIX only)
 ** + @c RE_SEMAPHORE_DEADLOCK: Could not wait for the semaphore because a deadlock condition was detected. (POSIX only)
 ** + @c RE_INTERRUPT: Failure during waiting for a semaphore because of a signal interruption (POSIX only)
 **
 **/
i_DECLIMEX_ int32_t rfSemaphore_TryWait(RF_Semaphore* s);

/**
 ** @memberof RF_Semaphore
 ** @brief Releases ownerhship of a semaphore object, increasing its count by one
 **
 ** @param s The semaphore object
 ** @return Returns @c RF_SUCCESS for success and an error otherwise. Possible errors are:
 ** + @c RE_SEMAPHORE_POST: Generic error during semaphore posting
 ** + @c RE_SEMAPHORE_POST_INVALID: Invalid semaphore value provided to the post function (POSIX only)
 ** + @c RE_UNSUPPORTED: Semaphore posting is not supported by the system (POSIX only)
 **
 **/
i_DECLIMEX_  int32_t rfSemaphore_Post(RF_Semaphore* s);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
