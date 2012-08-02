/**
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
**/


#include <stdio.h>
#include <time.h> //for time() for random seed

#include "../include/rf_setup.h"
#include "../include/rf_utils.h" //for detecting endianess

#ifdef REFU_WIN32_VERSION
    #include <windows.h>
#endif

//for the local memory stack
#include <rf_localmem.h>
#include <string.h> //for strcmp


int32_t  RF_LastError;
FILE* rfStdLog;
FILE* rfStdErr;

//Initializes the ReFu library
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
void rfInit(char* errstr,char* logstr,uint64_t lmsSize)
#else
void i_rfInit(char* errstr,char* logstr,uint64_t lmsSize)
#endif
{
    //initialize last error and open the log files or redirect to the standard stream
    RF_LastError = 0;
    if(strcmp(errstr,"stderr") == 0)
        rfStdErr = stderr;
    else if(strcmp(errstr,"stdout")== 0)
        rfStdErr = stdout;

    else//just open the given file
    {
        rfStdErr = fopen(errstr,"w");
        //Send the standard error also to the same file stream as the chosen rfStdErr
        if(freopen(errstr,"w",stderr) ==0)
            printf("Failed to reopen stderr stream to the given file name \"%s\"",errstr);
    }

    if(strcmp(logstr,"stderr") == 0)
        rfStdLog = stderr;
    else if(strcmp(logstr,"stdout")== 0)
        rfStdLog = stdout;
    else
        rfStdLog = fopen(logstr,"w");

    //detect system endianess and save it in rfUtils
    rfUTILS_DetectEndianess();

#ifdef REFU_WIN32_VERSION
    //if this is windows detect whether we support High Resolution performance counter and if we do get the frequency
    rfUTILS.hasHighResTimer = QueryPerformanceFrequency((LARGE_INTEGER*)&rfUTILS.pcFrequency);
#elif defined(REFU_LINUX_VERSION)
    rfUTILS.hasHighResTimer = true;
    if(clock_getres(CLOCK_PROCESS_CPUTIME_ID, 0) == -1)
    {
        if(clock_getres(CLOCK_MONOTONIC,0) == -1)
        {
            if(clock_getres(CLOCK_REALTIME,0) == -1)
            {
                LOG_ERROR("No high resolution timer is supported. Even CLOCK_REALTIME initialization failed.",RE_TIMER_HIGHRES_UNSUPPORTED);
                rfUTILS.hasHighResTimer = false;
            }
            else
            {
                rfUTILS.timerType = CLOCK_REALTIME;
            }
        }
        else
        {
            rfUTILS.timerType = CLOCK_MONOTONIC;
        }
    }
    else
    {
        rfUTILS.timerType = CLOCK_PROCESS_CPUTIME_ID;
    }
#else
    //! @todo
#endif
    //initialize the random seed
    srand(time(NULL));
    //initialize the main thread's local stack memory
    rfLMS_Init(&RF_MainLMS,lmsSize);
}

#ifdef RF_NO_C99 // if the library is compiled with no C99 features on
///In this case the logs differ quite a lot since the function name and file and line can't be logged. So in MSVC's case this is simply reduced to error message logging
void LOG_ERROR(const char* msg,int32_t errorCode,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
    //remember the set error
    RF_LastError = errorCode;
}
void DLOG_ERROR(const char* msg,int32_t errorCode,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
    //remember the set error
    RF_LastError = errorCode;
}
void LOG_INFO(const char* msg,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
}
void DLOG_INFO(const char* msg,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
}
#endif
