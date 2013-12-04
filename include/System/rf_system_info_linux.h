/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
 */
#ifndef RF_SYSTEM_INFO_LINUX_H
#define RF_SYSTEM_INFO_LINUX_H

#ifdef REFU_LINUX_VERSION
#include <System/rf_system_info_linux.h>
#else
#include <System/rf_system_info_win32.h>
#endif


#include <time.h> /* for clockid_t */


/**
 **Globals for Systeminformation
 **
 **/
typedef struct RFsystem_info
{
    //! A byte holding the system's endianess. Is @c 0 (@c RF_LITTLE_ENDIAN) for little endian and @c 1 (@c RF_BIG_ENDIAN) for big endian
    char endianess;
    //! A boolean flag indicating whether the system supports high resolution performance counter in windows and clock_gettime in Linux
    char hasHighResTimer;
#ifdef REFU_WIN32_VERSION
    //! In Windows we should also keep the PCFrequency in case we want to use a performance counter
    int64_t pcFrequency;
#elif defined(REFU_LINUX_VERSION)
    //! In Linux we will keep the type of the high res counter to be used by the timers
    clockid_t   timerType;
#else
    //! @todo
#endif
}RFsystem_info;


//system info global declaration
extern RFsystem_info rfSysInfo;


/**
 ** A function which detects the system's endianess
 **
 **/
void i_DetectEndianess();


#endif//include guards end
