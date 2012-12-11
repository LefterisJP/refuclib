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
**
**
** --Utils/error.h
** Defines the error and info logging macros
**
**
---------------------For internal library include make sure to have----------------------------
#include <stdio.h>//for FILE* used inside printf.h
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>
#include <IO/printf.h> //for rfFpintf() used in the error logging macros
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <Utils/error.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_UTILS_ERROR_H
#define RF_UTILS_ERROR_H

/**
** @defgroup RF_ErrorLoggingGRP Error Logging
** @addtogroup RF_ErrorLoggingGRP
** @{
**/

//! This is the last error code that occured
extern int32_t  i_DECLIMEX_ RF_LastError;
//! The standard log used by the library
extern i_DECLIMEX_ FILE* rfStdLog;
//! The standard error used by the library
extern i_DECLIMEX_ FILE* rfStdErr;

#ifdef RF_OPTION_VERBOSE_ERRORS

#ifndef RF_NO_C99
//! @brief Logs an error to the refu standard error stream
//!
//! This macro logs an error to the refu error stream
//! @param i_msg_ A string literal containing the message to be logged for the error in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param i_errorCode_ The error code to log. Must be an int32_t value
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define LOG_ERROR(i_msg_,i_errorCode_,...)  i_PICK_LOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,RF_NARG(__VA_ARGS__),__VA_ARGS__)
//! This macro logs an error to the refu error stream and returns the error code from the function it was called from.
#define RETURN_LOG_ERROR(i_msg_,i_errorCode_,...)  {LOG_ERROR(i_msg_,i_errorCode_,__VA_ARGS__) return i_errorCode_; }
//! This macro logs an error to the refu error stream and jumps to the specified goto label to exit a function
#define RETURNGOTO_LOG_ERROR(i_msg_,i_errorCode_,i_retvar,i_label,...)  {LOG_ERROR(i_msg_,i_errorCode_,__VA_ARGS__) i_retvar=i_errorCode_;goto i_label; }

//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,i_argN,...) i_PICK_LOG_ERROR_FUNC(i_msg_,i_errorCode_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_ERROR_FUNC(i_msg_,i_errorCode_,i_gt0,...) i_RP_PASTE2(i_LOG_ERROR,i_gt0) (i_msg_,i_errorCode_,__VA_ARGS__)

//Logs an error to the refu standard error stream with varargs
#define i_LOG_ERROR1(i_msg_,i_errorCode_, ...)  {rfFPrintf(rfStdErr,"At \"%s\" >>> "i_msg_"\n",__FUNCTION__,__VA_ARGS__);RF_LastError = i_errorCode_;fflush(rfStdErr);}
//Logs an error to the refu standard error stream without varargs
#define i_LOG_ERROR0(i_msg_,i_errorCode_, ...)  {rfFPrintf(rfStdErr,"At \"%s\" >>> "i_msg_"\n",__FUNCTION__);RF_LastError = i_errorCode_;fflush(rfStdErr);}


//! @brief Logs an error to the standard error stream in detail
//!
//! This macro logs an error in detail to the refu error stream
//! In detail means that it also saves the file and line number it occured
//! @param i_msg_ A string literal containing the message to be logged for the error in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param i_errorCode_ The error code to log. Must be an int32_t value
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define DLOG_ERROR(i_msg_,i_errorCode_, ...)  i_PICK_DLOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,RF_NARG(__VA_ARGS__),__VA_ARGS__)
//! This macro logs an error in detail to the refu error stream and returns the error code from the function it was called from.
#define DRETURN_LOG_ERROR(i_msg_,i_errorCode_,...)  {DLOG_ERROR(i_msg_,i_errorCode_,__VA_ARGS__) return i_errorCode_; }
//! This macro logs an error to the refu error stream and jumps to the specified goto label to exit a function
#define DRETURNGOTO_LOG_ERROR(i_msg_,i_errorCode_,i_retvar,i_label,...)  {DLOG_ERROR(i_msg_,i_errorCode_,__VA_ARGS__) i_retvar=i_errorCode_;goto i_label; }

//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_DLOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,i_argN,...) i_PICK_DLOG_ERROR_FUNC(i_msg_,i_errorCode_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_DLOG_ERROR_FUNC(i_msg_,i_errorCode_,i_gt0,...) i_RP_PASTE2(i_DLOG_ERROR,i_gt0) (i_msg_,i_errorCode_,__VA_ARGS__)

// Logs an error to the refu standard error stream in detail with varargs
#define i_DLOG_ERROR1(i_msg_,i_errorCode_, ...)   {rfFPrintf(rfStdErr,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__VA_ARGS__,__FILE__, __LINE__,__FUNCTION__ ); RF_LastError = i_errorCode_;fflush(rfStdErr);}
// Logs an error to the refu standard error stream in detail without varargs
#define i_DLOG_ERROR0(i_msg_,i_errorCode_, ...)   {rfFPrintf(rfStdErr,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__FILE__, __LINE__,__FUNCTION__ );RF_LastError = i_errorCode_; fflush(rfStdErr);}


#ifdef REFU_WIN32_VERSION
//! @brief Gets Windows Last System error
//!
//! Gets Windows Last System error and turns it into a char* that later needs to be freed by the user using the @c LocalFree() function
//! @param i_STRBUFF The String buffer
#define RF_WIN32_GETSYSERROR(i_STRBUFF) \
/*The buffer to hold the string*/\
char* i_STRBUFF;\
/*The DWORD to hold the error code*/\
DWORD i_ERROR_CODE = GetLastError();\
FormatMessage(\
FORMAT_MESSAGE_ALLOCATE_BUFFER |\
FORMAT_MESSAGE_FROM_SYSTEM |\
FORMAT_MESSAGE_IGNORE_INSERTS,\
NULL,\
i_ERROR_CODE,\
MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
(LPTSTR) &i_STRBUFF,\
0, NULL );
#endif


//! @brief Logs a message in the standard information log
//! @param i_msg_ A string literal containing the message to be logged in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define LOG_INFO(i_msg_,...)  i_PICK_LOG_INFO_FUNC_GT(i_msg_,RF_NARG(__VA_ARGS__),__VA_ARGS__)

//This macro picks up the correct logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_INFO_FUNC_GT(i_msg_,i_argN,...) i_PICK_LOG_INFO_FUNC(i_msg_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_INFO_FUNC(i_msg_,i_gt0,...) i_RP_PASTE2(i_LOG_INFO,i_gt0) (i_msg_,__VA_ARGS__)

//Logs something to the standard refu log with var_args
#define i_LOG_INFO1(i_msg_, ...)  {rfFPrintf(rfStdLog,"At \"%s\" >>> "i_msg_"\n",__FUNCTION__,__VA_ARGS__);fflush(rfStdLog);}
//Logs something to the standard refu log without var_args
#define i_LOG_INFO0(i_msg_, ...)   {rfFPrintf(rfStdLog,"At \"%s\" >>> "i_msg_"\n",__FUNCTION__);fflush(rfStdLog);}

//! @brief Logs a message in the standard information log with details
//!
//! In detail means that it also saves the file and line number it occured
//! @param i_msg_ A string literal containing the message to be logged in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define DLOG_INFO(i_msg_,...)  i_PICK_DLOG_INFO_FUNC_GT(i_msg_,RF_NARG(__VA_ARGS__),__VA_ARGS__)

//This macro picks up the correct logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_DLOG_INFO_FUNC_GT(i_msg_,i_argN,...) i_PICK_DLOG_INFO_FUNC(i_msg_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_DLOG_INFO_FUNC(i_msg_,i_gt0,...) i_RP_PASTE2(i_DLOG_INFO,i_gt0) (i_msg_,__VA_ARGS__)

//Logs something in detail to the standard refu log with var_args
#define i_DLOG_INFO1(i_msg_, ...)   {rfFPrintf(rfStdLog,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__VA_ARGS__,__FILE__, __LINE__,__FUNCTION__ ); fflush(rfStdLog);}
// Logs something in detail to the standard refu log without var_args
#define i_DLOG_INFO0(i_msg_, ...)   {rfFPrintf(rfStdLog,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__FILE__, __LINE__,__FUNCTION__ ); fflush(rfStdLog);}


#else// if the library is compiled with no C99 features on
///In this case the logs differ quite a lot since the function name and file and line can't be logged. So in MSVC's case this is simply reduced to error message logging
void LOG_ERROR(const char* msg,int32_t errorCode,...);
void DLOG_ERROR(const char* msg,int32_t errorCode,...);
void LOG_INFO(const char* msg,...);
void DLOG_INFO(const char* msg,...);
#endif

#else //non verbose errors
    #define LOG_ERROR(i_msg_,i_errorCode_,...)
    #define RETURN_LOG_ERROR(i_msg_,i_errorCode_,...) return i_errorCode_;
    #define DLOG_ERROR(i_msg_,i_errorCode_,...)
    #define RETURN_DLOG_ERROR(i_msg_,i_errorCode_,...) return i_errorCode_;
    #define LOG_INFO(i_msg_,...)
    #define DLOG_INFO(i_msg_,...)
#endif

//! @brief Returns the error code of the last error that occured
#define RF_GETLASTERROR()  RF_LastError
//! @brief Returns the standard refu log file descriptor
#define RF_GET_LOG()      rfStdLog
//! @brief Returns the standard refu error file descriptor
#define RF_GET_ERR()      rfStdErr


//!@}
//end of the Doxygen RF_ErrorLoggingGRP group

#endif//include guards end
