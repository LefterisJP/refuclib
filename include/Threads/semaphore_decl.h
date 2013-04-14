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
*/
#ifndef RF_SEMAPHORE_DECL_H
#define RF_SEMAPHORE_DECL_H


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


#endif//include guards end
