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
#ifndef RF_MUTEX_DECL_H
#define RF_MUTEX_DECL_H

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

#endif
