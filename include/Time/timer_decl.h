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
#ifndef RF_TIMER_DECL_H
#define RF_TIMER_DECL_H


/**
** @internal
** @author Lefteris
** @date 17/05/2012
** @endinternal
** @brief A cross-platform high resolution Timer structure
**
** This is a cross-platform Timer structure that allows the user to implement a timer with different resolutions
** on any system that he might need to. This timer is supposed to be used for high resolution counting such as milliseconds
** microseconds, or nanoseconds.
**/
typedef struct RF_Timer
{
    //! The resolution of the timer. Will be the value given in the initialization
    char resolution;
#ifdef REFU_WIN32_VERSION
    //! The variable holding the start of the timer or the time of the last query
    int64_t lastQuery;
    //! The amount that the difference should be multiplied by to provide the desired result. (Seconds, milliseconds, microseconds e.t.c.)
    double div;
#elif defined(REFU_LINUX_VERSION)
    //! The struct holding the start ofthe timer of the time of the last query
    struct timespec lastQuery;
    //! Holds the type of clockid_t to be given to the posix timer clock functions
    clockid_t type;
#else
    #error Unsuported timer system
    //!@todo
#endif
}RF_Timer;


#endif//include guards end
