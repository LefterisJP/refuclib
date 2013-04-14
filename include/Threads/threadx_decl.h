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
#ifndef RF_THREADX_DECL_H
#define RF_THREADX_DECL_H

//forward declaration of RF_SignalThread
struct RF_SignalThread;
/**
** @internal
** @author Lefteris
** @date 11/04/2012
** @endinternal
** @extends RF_Thread
** @brief An extension to the simple Thread which adds a message queue
**
** The ThreadX structure operates just like an @ref RF_Thread
** does but has the added advantage of having a message queue
** which the user of the thread can access from inside the thread's code
**/
typedef struct RF_ThreadX
{
    //! The inherited RF_Thread data
    RF_Thread INH_Thread;
    //! The message queue
    struct RF_SignalThread* msgQueue;
    //! The index of the last input message
    uint32_t msgIndex;
    //! The maximum number of signals in the msg queuue
    uint32_t maxSignals;
#ifdef REFU_WIN32_VERSION
    //! The lock that makes sure that the message queue is thread-safe. Using a Critical Section in Windows.
    CRITICAL_SECTION lock;
#elif defined (REFU_LINUX_VERSION)
    //! The lock in Linux is a mutex
    RF_Mutex lock;
#else
    #error RF_Thread Unsupported Operating System
#endif
}RF_ThreadX;

#endif//include guards end

