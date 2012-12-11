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
#ifndef RF_THREAD_DECL_H
#define RF_THREAD_DECL_H

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

#endif//include guards end
