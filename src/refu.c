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

/*------------- Corrensponding Header inclusion -------------*/
#include <refu.h>
/*------------- Outside Module inclusion -------------*/
#include "System/info.ph" /* detecting endianess */
#include <Utils/localmem.h> /* local memory stack */
#include <Utils/log.h> /* logging system */
#include "Internal/internal_mod.ph" /* internal data initialization */
/*------------- Modules init/deinit -------------*/
#include "String/mod.ph"
/*------------- System Specific includes -------------*/
#ifdef REFU_WIN32_VERSION
#include <windows.h> //for QueryPerformanceFrequency() and other related flags
#endif
/*------------- libc includes -------------*/
#include <string.h> //for strcmp
/*------------- End of includes -------------*/

//Initializes the Refu library
bool rfInit(char *logstr, uint64_t lmsSize, RFlog_level level)
{
    rf_LogModule_Init(level, logstr);
    module_string_init();
    module_internal_init();


    /* //initialize regu log stream */
    /* if(strcmp(logstr,"stderr") == 0) */
    /*     RFlog_stream = stderr; */
    /* else if(strcmp(logstr,"stdout")== 0) */
    /*     RFlog_stream = stdout; */
    /* else//just open the given file */
    /* { */
    /*     RFlog_stream = fopen(logstr,"w"); */
    /*     //Send the standard error also to the same file stream as the chosen rfStdErr */
    /*     if(freopen(logstr,"w",stderr) ==0) */
    /*         printf("Failed to reopen stderr stream to the given file name \"%s\"",logstr); */
    /* } */

    //detect system endianess and save it in rfSysInfo
    i_DetectEndianess();

#ifdef REFU_WIN32_VERSION
    rfSysInfo.hasHighResTimer = QueryPerformanceFrequency(
        (LARGE_INTEGER *)&rfSysInfo.pcFrequency,
    );
#elif defined(REFU_LINUX_VERSION)
    rfSysInfo.hasHighResTimer = true;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, 0) == -1) {
        if (clock_getres(CLOCK_MONOTONIC, 0) == -1) {
            if (clock_getres(CLOCK_REALTIME, 0) == -1) {
                RF_ERROR("No high resolution timer is supported. Even "
                         "CLOCK_REALTIME initialization failed.");
                rfSysInfo.hasHighResTimer = false;
            } else {
                rfSysInfo.timerType = CLOCK_REALTIME;
            }
        } else {
            rfSysInfo.timerType = CLOCK_MONOTONIC;
        }
    } else {
        rfSysInfo.timerType = CLOCK_PROCESS_CPUTIME_ID;
    }
#else
    //! @todo
#endif
    //initialize the main thread's local stack memory
    if (lmsSize == 0) {
        lmsSize = RF_OPTION_LOCALSTACK_MEMORY_SIZE;
    }
    if (!rf_module_lms_init(lmsSize)) {
        RF_ERROR("Could not initialize main thread's local memory stack");
        return false;
    }

    return true;
}


