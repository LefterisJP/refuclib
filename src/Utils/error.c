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


//*---------------------Corrensponding Header inclusion---------------------------------
#include <stdio.h>//for FILE* used inside printf.h
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>
#include <IO/printf.h> //for rfFpintf() used in the error logging macros
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <Utils/error.h>
//*----------------------------End of Includes------------------------------------------


//definitions
int32_t  RF_LastError;
FILE* rfStdLog;
FILE* rfStdErr;


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
