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
** -Threads/common.h
** This declares some functions that deal with thread related globals
** or commong functionality such as the id of the curent thread.
**  DOES NOT deal with anything having to do with RF_Thread types.
*/
#ifndef RF_THREAD_COMMON_H
#define RF_THREAD_COMMON_H

#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <System/rf_system.h> //for threadid_t

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif



/**
 ** @brief Returns a unique identifier for the 
 ** current thread.
 **/
i_DECLIMEX_ threadid_t rfThread_GetID();


/**
 ** @brief Initializes the threading system.
 **
 ** The only thing that this currently does is
 ** initialize the main thread's id with the pid of the process
 **/
i_DECLIMEX_ char rfThreads_Init();


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end
