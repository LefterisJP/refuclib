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
**
** --Utils/error.h
** Defines the error and info logging macros
**
**
------For internal library include make sure to have------(mirror: RFerror.h)
#include <rf_options.h> //for the logging level defines
#include <stdio.h>//for FILE* used inside printf.h
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>
#include <IO/printf.h> //for rfFpintf() used in the error logging macros
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <Threads/common.h> //for rfThread_GetID()
#include <errno.h>  //only if using the stdlibrary functions that report errors with it
#include <Utils/error.h>
------For internal library include make sure to have------
*/
#ifndef RF_UTILS_ERROR_H
#define RF_UTILS_ERROR_H

/**
** @Defgroup RF_ErrorLoggingGRP Error Logging
** @addtogroup RF_ErrorLoggingGRP
** @{
**/


/**
 ** The file stream used by the library for logging
 **
 **/
extern i_DECLIMEX_ FILE* RF_Log_Stream;

// helping macro to evaluate both a_ and b_ before passing them
// on to RP_GT
#define i_RP_EVAL_GT(a_, b_) \
    RP_GT(a_, b_)

#define i_RP_EVAL_GT2(a_, b_) \
    RP_GT2(a_, b_)

/*--- Error Logging macros --- */

//logs an error at the given level with the specified message and
//optional varargs
#define RF_ERROR(i_level_, i_msg_, ...)                  \
    i_PICK_LOG_ERROR_FUNC_GT(i_level_, i_msg_, RF_NARG(__VA_ARGS__),\
                             __VA_ARGS__)

//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_ERROR_FUNC_GT(i_level_, i_msg_, i_argN, ...)         \
    i_PICK_LOG_ERROR_FUNC(i_level_, i_msg_, RP_GT(i_argN,0), __VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_ERROR_FUNC(i_level_, i_msg_, i_gt0,...)              \
    i_RP_PASTE2(i_LOG_ERROR_VARGS, i_gt0) (i_level_, i_msg_, __VA_ARGS__)

//Logs an error to the refu standard error stream with varargs
#define i_LOG_ERROR_VARGS1(i_level_, i_msg_, ...)                 \
    i_RP_PICK_ERROR_GT_VARARGS(i_level_, i_msg_, __VA_ARGS__)

//Logs an error to the refu standard error stream without varargs
#define i_LOG_ERROR_VARGS0(i_level_, i_msg_, ...) \
    i_RP_PICK_ERROR_GT(i_level_, i_msg_)

//these macros check the error level for the varargs case
#define i_RP_PICK_ERROR_GT_VARARGS(i_level_, i_msg_, ...)            \
    i_RP_PICK_ERROR_VARARGS(i_msg_,                                  \
                            i_RP_EVAL_GT(RF_OPTION_ERROR_LOGGING_LEVEL, i_level_), \
                            __VA_ARGS__)

#define i_RP_PICK_ERROR_VARARGS(i_msg_, i_result_, ...)                 \
         i_RP_PASTE2(i_RP_LOG_ERROR_VARARGS, i_result_)(i_msg_, __VA_ARGS__)

// >>> --- LOGGING ERROR with varargs
#define i_RP_LOG_ERROR_VARARGS1(i_msg_, ...)                            \
    do{                                                                 \
        rfFPrintf(RF_Log_Stream,                                        \
                  "[ERROR] (thread: 0x%x) [%s():"__FILE__":%d] "i_msg_"\n", \
                  rfThread_GetID(), __func__, __LINE__,                 \
                  __VA_ARGS__);                                         \
        fflush(RF_Log_Stream);                                          \
    }while(0)

#define i_RP_LOG_ERROR_VARARGS0(i_msg_, ...)


//these macros check the error level for the no varargs case
#define i_RP_PICK_ERROR_GT(i_level_, i_msg_)                        \
    i_RP_PICK_ERROR(i_msg_,                                         \
                    i_RP_EVAL_GT(RF_OPTION_ERROR_LOGGING_LEVEL, i_level_))

#define i_RP_PICK_ERROR(i_msg_, i_result_)            \
    i_RP_PASTE2(i_RP_LOG_ERROR, i_result_)(i_msg_)

// >>> --- LOGGING ERROR without varargs
#define i_RP_LOG_ERROR1(i_msg_)                                         \
    do{                                                                 \
             rfFPrintf(RF_Log_Stream,                                   \
                       "[ERROR] (thread: 0x%x) [%s():"__FILE__":%d] "i_msg_"\n", \
                       rfThread_GetID(), __func__, __LINE__);                    \
             fflush(RF_Log_Stream);                                     \
         }while(0)

#define RF_LOG_ERROR0(i_msg_)



/*--- Info Logging macros --- */

//logs an info message at the given level with the specified message and
//optional varargs
#define RF_INFO(i_level_, i_msg_, ...)                              \
    i_PICK_LOG_INFO_FUNC_GT(i_level_, i_msg_, RF_NARG(__VA_ARGS__), \
                            __VA_ARGS__)


//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_INFO_FUNC_GT(i_level_, i_msg_, i_argN, ...)          \
    i_PICK_LOG_INFO_FUNC(i_level_, i_msg_, RP_GT(i_argN,0), __VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_INFO_FUNC(i_level_, i_msg_, i_gt0,...)               \
    i_RP_PASTE2(i_LOG_INFO_VARGS, i_gt0) (i_level_, i_msg_, __VA_ARGS__)

//Logs an error to the refu standard error stream with varargs
#define i_LOG_INFO_VARGS1(i_level_, i_msg_, ...)            \
    i_RP_PICK_INFO_GT_VARARGS(i_level_, i_msg_, __VA_ARGS__)

//Logs an error to the refu standard error stream without varargs
#define i_LOG_INFO_VARGS0(i_level_, i_msg_, ...)    \
    i_RP_PICK_INFO_GT(i_level_, i_msg_)

//these macros check the error level for the varargs case
#define i_RP_PICK_INFO_GT_VARARGS(i_level_, i_msg_, ...)            \
    i_RP_PICK_INFO_VARARGS(i_msg_,                                    \
                           i_RP_EVAL_GT(RF_OPTION_INFO_LOGGING_LEVEL, i_level_),  \
                           __VA_ARGS__)

#define i_RP_PICK_INFO_VARARGS(i_msg_, i_result_, ...)                  \
        i_RP_PASTE2(i_RP_LOG_INFO_VARARGS, i_result_)(i_msg_, __VA_ARGS__)

// >>> --- LOGGING INFO with varargs
#define i_RP_LOG_INFO_VARARGS1(i_msg_, ...)                             \
        do{                                                             \
            rfFPrintf(RF_Log_Stream,                                     \
                      "[INFO] (thread: 0x%x) [%s():"__FILE__":%d] "i_msg_"\n", \
                      rfThread_GetID(), __func__, __LINE__,             \
                      __VA_ARGS__);                                     \
            fflush(RF_Log_Stream);                                      \
        }while(0)

#define i_RP_LOG_INFO_VARARGS0(i_msg_, ...)


//these macros check the error level for the no varargs case
#define i_RP_PICK_INFO_GT(i_level_, i_msg_)                         \
    i_RP_PICK_INFO(i_msg_,                                          \
                   i_RP_EVAL_GT(RF_OPTION_INFO_LOGGING_LEVEL, i_level_))

#define i_RP_PICK_INFO(i_msg_, i_result_)             \
        i_RP_PASTE2(i_RP_LOG_INFO, i_result_)(i_msg_)

// >>> --- LOGGING INFO without varargs
#define i_RP_LOG_INFO1(i_msg_)                                          \
        do{                                                             \
            rfFPrintf(RF_Log_Stream,                                    \
                      "[INFO] (thread: 0x%x) [%s():"__FILE__":%d] "i_msg_"\n", \
                      rfThread_GetID(), __func__, __LINE__);                     \
            fflush(RF_Log_Stream);                                      \
        }while(0)

#define i_RP_LOG_INFO0(i_msg_)


/*--- Warning Logging macros --- */

//logs an info message at the given level with the specified message and
//optional varargs
#define RF_WARNING(i_level_, i_msg_, ...)                              \
    i_PICK_LOG_WARNING_FUNC_GT(i_level_, i_msg_, RF_NARG(__VA_ARGS__), \
                            __VA_ARGS__)


//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_WARNING_FUNC_GT(i_level_, i_msg_, i_argN, ...)          \
    i_PICK_LOG_WARNING_FUNC(i_level_, i_msg_, RP_GT(i_argN,0), __VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_WARNING_FUNC(i_level_, i_msg_, i_gt0,...)               \
    i_RP_PASTE2(i_LOG_WARNING_VARGS, i_gt0) (i_level_, i_msg_, __VA_ARGS__)

//Logs an error to the refu standard error stream with varargs
#define i_LOG_WARNING_VARGS1(i_level_, i_msg_, ...)            \
    i_RP_PICK_WARNING_GT_VARARGS(i_level_, i_msg_, __VA_ARGS__)

//Logs an error to the refu standard error stream without varargs
#define i_LOG_WARNING_VARGS0(i_level_, i_msg_, ...)    \
    i_RP_PICK_WARNING_GT(i_level_, i_msg_)

//these macros check the error level for the varargs case
#define i_RP_PICK_WARNING_GT_VARARGS(i_level_, i_msg_, ...)            \
    i_RP_PICK_WARNING_VARARGS(i_msg_,                                    \
                           i_RP_EVAL_GT(RF_OPTION_WARNING_LOGGING_LEVEL, i_level_),  \
                           __VA_ARGS__)

#define i_RP_PICK_WARNING_VARARGS(i_msg_, i_result_, ...)                  \
        i_RP_PASTE2(i_RP_LOG_WARNING_VARARGS, i_result_)(i_msg_, __VA_ARGS__)

// >>> --- LOGGING WARNING with varargs
#define i_RP_LOG_WARNING_VARARGS1(i_msg_, ...)                             \
        do{                                                             \
            rfFPrintf(RF_Log_Stream,                                     \
                      "[WARNING] (thread: 0x%x) [%s():"__FILE__":%d] "i_msg_"\n", \
                      rfThread_GetID(), __func__, __LINE__,             \
                      __VA_ARGS__);                                     \
            fflush(RF_Log_Stream);                                      \
        }while(0)

#define i_RP_LOG_WARNING_VARARGS0(i_msg_, ...)


//these macros check the error level for the no varargs case
#define i_RP_PICK_WARNING_GT(i_level_, i_msg_)                         \
    i_RP_PICK_WARNING(i_msg_,                                          \
                   i_RP_EVAL_GT(RF_OPTION_WARNING_LOGGING_LEVEL, i_level_))

#define i_RP_PICK_WARNING(i_msg_, i_result_)             \
        i_RP_PASTE2(i_RP_LOG_WARNING, i_result_)(i_msg_)

// >>> --- LOGGING WARNING without varargs
#define i_RP_LOG_WARNING1(i_msg_)                                          \
        do{                                                             \
            rfFPrintf(RF_Log_Stream,                                    \
                      "[WARNING] (thread: 0x%x) [%s():"__FILE__":%d] "i_msg_"\n", \
                      rfThread_GetID(), __func__, __LINE__);                     \
            fflush(RF_Log_Stream);                                      \
        }while(0)

#define i_RP_LOG_WARNING0(i_msg_)






/*--- General default case for all functions when errno is unknown ---*/
#define i_RP_DEFAULT_SWITCH_CASE_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    RP_END_OF_ARGLIST2(i_RP_VARGS_DEFAULT_SWITCH, i_msg_, i_func_name_, i_error_, __VA_ARGS__) 

#define i_RP_VARGS_DEFAULT_SWITCH(i_msg_, i_func_name_, ...) \
    default:                                                            \
        RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed."\
                 " Error code [%d] was unexpected and we can't determine what it means." \
                 , __VA_ARGS__);                                        \
        break;


#define i_RP_DEFAULT_SWITCH_CASE_VARGS0(i_msg_, i_func_name_, i_error_, ...) \
    default:                                                            \
        RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                 " Error code [%d] was unexpected and we can't determine what " \
                 "it means.", i_error_);                                \
        break;



#define i_RP_PICK_DEFAULT_SWITCH_FUNC_GT(i_msg_, i_func_name_, i_error_, i_argN, ...) \
        i_PICK_DEFAULT_SWITCH_FUNC(i_msg_, i_func_name_, i_error_, RP_GT(i_argN,0), __VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_DEFAULT_SWITCH_FUNC(i_msg_, i_func_name, i_error_, i_gt0, ...) \
            i_RP_PASTE2(i_RP_DEFAULT_SWITCH_CASE_VARGS, i_gt0) (i_msg_, i_func_name, i_error_, __VA_ARGS__)

#define i_RP_DEFAULT_SWITCH_CASE(i_msg_ , i_func_, i_err_, ...)                                \
            i_RP_PICK_DEFAULT_SWITCH_FUNC_GT(i_msg_ , i_func_, i_err_, RF_NARG(__VA_ARGS__), __VA_ARGS__)






/*--- Logging macros for fputc --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "fprintf" */
#define RF_ERROR_FPUTC(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FPUTC_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FPUTC_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FPUTC_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FPUTC_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FPUTC_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FPUTC_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EAGAIN:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the O_NONBLOCK flag being set for the file " \
                     "descriptor underlying stream and the thread would be " \
                     "delayed in the write operation", __VA_ARGS__);    \
            break;                                                      \
            case EBADF:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the file descriptor underlying stream not being a"\
                     " valid file descriptor open for writing.", __VA_ARGS__); \
            break;                                                      \
        case EFBIG: \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to an attempt that was made to write to a file that " \
                     "exceeds the maximum file size or to an offset that" \
                     "exceeds offset maximum. ", __VA_ARGS__);           \
            break;                                                      \
        case EINTR:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the write operation being"                \
                     "terminated because of the receipt of a signal, and no data "\
                     "being transferred", __VA_ARGS__);                   \
            break;                                                      \
        case EIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a physical I/O error occuring or some other " \
                     "implementation defined I/O conditions", __VA_ARGS__); \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to no free space remaining in the device"     \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EPIPE:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to an attempt being made to write to a pipe or FIFO " \
                     "that is not open for reading by any process. A SIGPIPE " \
                     "signal shall also be sent to the thread"          \
                     "delayed in the write operation", __VA_ARGS__);    \
            break;                                                      \
        case ENOMEM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to insufficient storage space available", __VA_ARGS__); \
            break;                                                      \
        case ENXIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a request being made of a nonexistent device, or "\
                     "the request being outside the capabilities of the device" \
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FPUTC_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FPUTC_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FPUTC_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FPUTC_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FPUTC_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FPUTC_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FPUTC_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FPUTC_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FPUTC_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif


/*--- Logging macros for s/n/printf --- */
#define RF_ERROR_PRINTF(...)                                          \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_PRINTF_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_PRINTF_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
     i_RP_PRINTF_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_PRINTF_ERROR_VARGS0(i_msg_, i_func_name_)        \
     i_RP_PRINTF_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_PRINTF_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EILSEQ:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a wide-character code that does not correspond to "\
                     "a valid character being detected", __VA_ARGS__);    \
            break;                                                      \
        case EINVAL:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to having insufficient arguments", __VA_ARGS__);    \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the value of n being greater than {INT_MAX} or "\
                     "the number of bytes needed to hold the output excluding "\
                     "the terminating null is greater than {INT_MAX}",\
                     __VA_ARGS__);                                      \
            break;

#define i_RP_PRINTF_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_PRINTF_CASES_VARGS, 2, __VA_ARGS__)

#define i_RP_PRINTF_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_PRINTF_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_PRINTF_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_PRINTF_CASES_ACTUAL(i_msg_, i_func_name_)


#define i_RP_PRINTF_ERROR_DETAILS(i_msg_, i_func_name_, ...)  \
    do{                                                                 \
        switch(errno)                                                   \
        {                                                               \
            i_RP_FPUTC_CASES(i_msg_, i_func_name_, __VA_ARGS__)         \
            i_RP_PRINTF_CASES(i_msg_, i_func_name_, __VA_ARGS__)        \
            i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
        }                                                           \
}while(0)




#else
// >> --- no details just report error
#define i_RP_PRINTF_ERROR_DETAILS(i_msg_, i_func_name_, ...)  \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);


#endif




/*--- Logging macros for fgetc --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "fread" */
#define RF_ERROR_FGETC(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FGETC_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FGETC_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FGETC_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FGETC_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FGETC_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FGETC_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EAGAIN:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the the file being blocked by another thread and "\
                     "the read operation would be delayed", __VA_ARGS__); \
            break;                                                      \
            case EBADF:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the file descriptor underlying stream not being a"\
                     " valid file descriptor open for reading.", __VA_ARGS__); \
            break;                                                      \
        case EINTR:                                                         \
            RF_ERROR(0, i_msg_". This happened because the read operation was "\
                     "terminated due to the receipt of a signal, and no data "\
                     "was transferred", __VA_ARGS__);                   \
            break;                                                      \
        case EIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a physical I/O error occuring or some other " \
                     "implementation defined I/O conditions", __VA_ARGS__); \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to The file being a regular file and an attempt being"\
                     " made to read at or beyond the offset maximum associated"\
                     " with the corresponding stream", __VA_ARGS__);    \
            break;                                                      \
        case ENOMEM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to insufficient storage space available", __VA_ARGS__); \
            break;                                                      \
        case ENXIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a request being made of a nonexistent device, or "\
                     "the request being outside the capabilities of the device" \
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FGETC_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FGETC_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FGETC_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FGETC_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FGETC_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FGETC_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FGETC_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FGETC_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FGETC_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif




/*--- Logging macros for fseek --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "fseek" */
#define RF_ERROR_FSEEK(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FSEEK_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FSEEK_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FSEEK_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FSEEK_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FSEEK_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FSEEK_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EAGAIN:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the O_NONBLOCK flag being set for the file " \
                     "descriptor underlying stream and the thread would be " \
                     "delayed in the write operation", __VA_ARGS__);    \
            break;                                                      \
            case EBADF:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the file descriptor underlying stream not being a"\
                     " valid file descriptor open for writing or the stream's "\
                     "buffer needed flushing and file is not open", __VA_ARGS__); \
            break;                                                      \
        case EFBIG: \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to an attempt that was made to write to a file that " \
                     "exceeds the maximum file size or to an offset that" \
                     "exceeds offset maximum. ", __VA_ARGS__);           \
            break;                                                      \
        case EINTR:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the write operation being"                \
                     "terminated because of the receipt of a signal, and no data "\
                     "being transferred", __VA_ARGS__);                   \
            break;                                                      \
        case EINVAL:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to The whence argument being invalid. The resulting "\
                     "file-position indicator would be set to a negative value" \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a physical I/O error occuring or some other " \
                     "implementation defined I/O conditions", __VA_ARGS__); \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to no free space remaining in the device"     \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EPIPE:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to an attempt being made to write to a pipe or FIFO " \
                     "that is not open for reading by any process. A SIGPIPE " \
                     "signal shall also be sent to the thread"          \
                     "delayed in the write operation", __VA_ARGS__);    \
            break;                                                      \
        case ESPIPE:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file descriptor underlying stream being "\
                     "associated with a pipe or FIFO.", __VA_ARGS__); \
            break;                                                      \
        case ENOMEM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to insufficient storage space available", __VA_ARGS__); \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the resulting file offset being of a value which " \
                     "cannot be represented correctly in an object of "\
                     "type long or off_t depending on the fseek()", __VA_ARGS__); \
            break;                                                      \
        case ENXIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a request being made of a nonexistent device, or "\
                     "the request being outside the capabilities of the device" \
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FSEEK_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FSEEK_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FSEEK_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FSEEK_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FSEEK_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FSEEK_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FSEEK_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FSEEK_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FSEEK_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif



/*--- Logging macros for fopen --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "fopen" */
#define RF_ERROR_FOPEN(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FOPEN_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FOPEN_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FOPEN_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FOPEN_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FOPEN_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FOPEN_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EACCES:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to search permission being denied on a component of "\
                     "the path prefix, or the file exists and the permissions "\
                     "specified by mode are denied, or the file does not exist "\
                     "and write permission is denied for the parent directory "\
                     "of the file to be created", __VA_ARGS__);    \
            break;                                                      \
            case EISDIR:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the named file being a directory and mode "\
                     "requires write access", __VA_ARGS__); \
            break;                                                      \
            case ELOOP:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to a loop existing in symbolic links encountered "\
                     "during resolution of the path argument or rore than "\
                     "{SYMLOOP_MAX} symbolic links were encountered during "\
                     "resolution of the path argument", __VA_ARGS__);   \
            break;                                                      \
        case EMFILE: \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to either {OPEN_MAX} file descriptors being currently "\
                     "open in the calling process or {FOPEN_MAX} or {STREAM_MAX} "\
                     "streams are open in the calling process.", __VA_ARGS__); \
            break;                                                      \
    case ENAMETOOLONG:                                                  \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to either the length of the filename argument exceeding "\
                     " {PATH_MAX} or a pathname component is longer than "\
                     "{NAME_MAX} or pathname resolution of a symbolic link "\
                     "produced an intermediate result whose length "\
                     "exceeds {PATH_MAX}", __VA_ARGS__);           \
            break;                                                      \
        case EINTR:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a signal being caught during fopen", __VA_ARGS__); \
            break;                                                      \
        case ENFILE:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the maximum allowable number of files being "\
                     "currently open in the system.", __VA_ARGS__);     \
            break;                                                      \
        case ENOENT:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a component of filename  not naming an existing "\
                     "file or filename being an empty string.", __VA_ARGS__); \
            break;                                                      \
        case EINVAL:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the value of the mode argument being invalid."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOTDIR:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a component of the path prefix not being a "\
                     "directory", __VA_ARGS__); \
            break;                                                   \
        case ENXIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the named file being a character special or block "\
                     "special file, and the device associated with this special "\
                     "file not existing ", __VA_ARGS__); \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the directory or file system that would contain "\
                     "the new file not being able to be expanded, the file not "\
                     "existing, and the file was to be created."        \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the named file being a regular file and the size "\
                     "of the file cannot be represented correctly in an object "\
                     "of type off_t", __VA_ARGS__);                     \
            break;                                                      \
        case EROFS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the named file residing on a read-only file system "\
                     "and mode requiring write access", __VA_ARGS__);   \
            break;                                                      \
        case ENOMEM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to insufficient storage space being available."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ETXTBSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file being a pure procedure (shared text) file "\
                     "that is being executed and mode requires write access."\
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FOPEN_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FOPEN_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FOPEN_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FOPEN_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FOPEN_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FOPEN_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FOPEN_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FOPEN_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FOPEN_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif



/*--- Logging macros for fgetpos --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "fgetpos" */
#define RF_ERROR_FGETPOS(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FGETPOS_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FGETPOS_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FGETPOS_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FGETPOS_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FGETPOS_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FGETPOS_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case ESPIPE:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the file descriptor underlying stream being "\
                     "associated with a pipe, FIFO, or socket", __VA_ARGS__);    \
            break;                                                      \
        case EBADF:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file descriptor underlying stream not being "\
                     "valid", __VA_ARGS__);                                    \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the current value of the file position not being "\
                     " represented correctly in an object of type fpos_t"\
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FGETPOS_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FGETPOS_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FGETPOS_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FGETPOS_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FGETPOS_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FGETPOS_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FGETPOS_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FGETPOS_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FGETPOS_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif






/*--- Logging macros for fsetpos --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "fsetpos" */
#define RF_ERROR_FSETPOS(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FSETPOS_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FSETPOS_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FSETPOS_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FSETPOS_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FSETPOS_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FSETPOS_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EAGAIN:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the O_NONBLOCK flag being set for the file " \
                     "descriptor underlying stream and the thread would be " \
                     "delayed in the write operation", __VA_ARGS__);    \
            break;                                                      \
            case EBADF:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the file descriptor underlying stream not being a"\
                     " valid file descriptor open for writing or the stream's "\
                     "buffer needed flushing and file is not open", __VA_ARGS__); \
            break;                                                      \
        case EFBIG: \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to an attempt that was made to write to a file that " \
                     "exceeds the maximum file size or to an offset that" \
                     "exceeds offset maximum. ", __VA_ARGS__);           \
            break;                                                      \
        case EINTR:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the write operation being"                \
                     "terminated because of the receipt of a signal, and no data "\
                     "being transferred", __VA_ARGS__);                   \
            break;                                                      \
        case EINVAL:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to The whence argument being invalid. The resulting "\
                     "file-position indicator would be set to a negative value" \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a physical I/O error occuring or some other " \
                     "implementation defined I/O conditions", __VA_ARGS__); \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to no free space remaining in the device containing " \
                     "the file", __VA_ARGS__);                                    \
            break;                                                      \
        case EPIPE:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to an attempt being made to write to a pipe or FIFO " \
                     "that is not open for reading by any process or the file "\
                     "descriptor undelrying stream is associated with a pipe "\
                     "or FIFO", __VA_ARGS__);                           \
            break;                                                      \
        case ENXIO:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a request being made of a nonexistent device, or "\
                     "the request being outside the capabilities of the device" \
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FSETPOS_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FSETPOS_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FSETPOS_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FSETPOS_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FSETPOS_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FSETPOS_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FSETPOS_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FSETPOS_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FSETPOS_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif



/*--- Logging macros for ftell --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "ftello" */
#define RF_ERROR_FTELL(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_FTELL_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_FTELL_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_FTELL_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_FTELL_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_FTELL_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_FTELL_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case ESPIPE:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the file descriptor underlying stream being "\
                     "associated with a pipe, FIFO, or socket", __VA_ARGS__);    \
            break;                                                      \
        case EBADF:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file descriptor underlying stream not being "\
                     "an open file descriptor", __VA_ARGS__);           \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the current value of the file position not being "\
                     " represented correctly in an object of type long or"\
                     "of type off_t depending on the function type"     \
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_FTELL_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_FTELL_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_FTELL_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_FTELL_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_FTELL_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_FTELL_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_FTELL_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_FTELL_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_FTELL_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif





/*--- Logging macros for mkdir --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "mkdir" */
#define RF_ERROR_MKDIR(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_MKDIR_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_MKDIR_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_MKDIR_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_MKDIR_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_MKDIR_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_MKDIR_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EACCES:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to search permission being denied on a component of "\
                     "the path prefix, or write permission is denied for the "\
                     "parent directory of the directory to be created", \
                     __VA_ARGS__);                                      \
            break;                                                      \
            case EEXIST:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the named file existing", __VA_ARGS__); \
            break;                                                      \
            case ELOOP:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to a loop existing in symbolic links encountered "\
                     "during resolution of the path argument", __VA_ARGS__);   \
            break;                                                      \
        case EMLINK: \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the link count of the parent directory exceeding " \
                     " {LINK_MAX} should the directory have been created"\
                     , __VA_ARGS__);                                    \
            break;                                                      \
    case ENAMETOOLONG:                                                  \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to either the length of the path argument exceeding "\
                     " {PATH_MAX} or a pathname component is longer than "\
                     "{NAME_MAX} or as a result of encountering a symbolic "\
                     "link in resolution of the path argument, the length of "\
                     "the substituted pathname string exceeded {PATH_MAX}."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOENT:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a component of the path prefix specified by path"\
                     "  not naming an existing "                        \
                     "file or path being an empty string.", __VA_ARGS__); \
            break;                                                      \
        case ENOTDIR:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a component of the path prefix not being a directory" \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file system  not containing enough space to "\
                     "hold the contents of the new directory or to extend "\
                     "the parent directory of the new directory."       \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EROFS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the parent directory  residing on a read-only" \
                     " file system ", __VA_ARGS__);                     \
            break;

#define i_RP_MKDIR_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_MKDIR_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_MKDIR_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_MKDIR_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_MKDIR_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_MKDIR_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_MKDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_MKDIR_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_MKDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif





/*--- Logging macros for opendir --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "opendir" */
#define RF_ERROR_OPENDIR(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_OPENDIR_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_OPENDIR_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_OPENDIR_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_OPENDIR_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_OPENDIR_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_OPENDIR_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EACCES:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to search permission being denied on a component of "\
                     "the path prefix, or read permission is denied for dirname", \
                     __VA_ARGS__);                                      \
            break;                                                      \
            case ELOOP:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to a loop existing in symbolic links encountered "\
                     "during resolution of the dirname argument or more than "\
                     "{SYMLOOP_MAX} symbolic links were encountered during "\
                     "resolution of the dirname argument", __VA_ARGS__); \
            break;                                                      \
    case ENAMETOOLONG:                                                  \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to either the length of the dirname argument exceeding "\
                     " {PATH_MAX} or a pathname component is longer than "\
                     "{NAME_MAX} or as a result of encountering a symbolic link "\
                     "in resolution of the dirname argument, the length of the "\
                     "substituted pathname string exceeded {PATH_MAX}."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOENT:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a component of dirname not naming an existing "\
                     "directory or dirname being an empty string.", __VA_ARGS__); \
            break;         \
        case ENOTDIR:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a component of the dirname not being a directory" \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EMFILE:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to {OPEN_MAX} file descriptors being currently "\
                     "open in the calling process."                     \
                     , __VA_ARGS__);                    \
            break;                                      \
        case ENFILE:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to too many files currently being open in the system" \
                     , __VA_ARGS__);                                    \
            break;

#define i_RP_OPENDIR_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_OPENDIR_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_OPENDIR_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_OPENDIR_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_OPENDIR_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_OPENDIR_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_OPENDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_OPENDIR_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_OPENDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif




/*--- Logging macros for mkdir --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "mkdir" */
#define RF_ERROR_CLOSEDIR(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_CLOSEDIR_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_CLOSEDIR_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_CLOSEDIR_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_CLOSEDIR_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_CLOSEDIR_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_CLOSEDIR_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
            case EBADF:                                                \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the dirp argument not refering to an open "\
                     "directory stream.", __VA_ARGS__);                 \
            break;                                                      \
            case EINTR:                                                         \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed" \
                     "due to the closedir() function being interrupted "\
                     "by a signal.", __VA_ARGS__); \
            break;

#define i_RP_CLOSEDIR_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_CLOSEDIR_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_CLOSEDIR_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_CLOSEDIR_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_CLOSEDIR_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_CLOSEDIR_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_CLOSEDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_CLOSEDIR_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_CLOSEDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif






/*--- Logging macros for readdir --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "readdir" */
#define RF_ERROR_READDIR(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_READDIR_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_READDIR_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_READDIR_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_READDIR_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_READDIR_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_READDIR_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case ENOENT:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the current position of the directory stream "\
                     "being invalid.", __VA_ARGS__); \
            break;                                                      \
        case EBADF:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the dirp argument not refering to an open "\
                     "directory stream." , __VA_ARGS__);                \
            break;                                                      \
        case EOVERFLOW:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to tne of the values in the structure to be returned "\
                     "not being represented correctly.", __VA_ARGS__);  \
            break;

#define i_RP_READDIR_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_READDIR_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_READDIR_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_READDIR_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_READDIR_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_READDIR_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_READDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_READDIR_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_READDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif



/*--- Logging macros for unlink --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "readdir" */
#define RF_ERROR_UNLINK(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_UNLINK_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_UNLINK_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_UNLINK_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_UNLINK_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_UNLINK_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_UNLINK_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EACCES:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to search permission being denied for a component of "\
                     "the path prefix, or write permission is denied on the "\
                     "directory containing the directory entry to be removed."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EBUSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file named by the path argument not being able "\
                     "to be unlinked because it is being used by the system or "\
                     "another process and the implementation considers this an "\
                     "error or the file named by path is a named STREAM",\
                     __VA_ARGS__);                                      \
            break;                                                      \
        case ENAMETOOLONG:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the length of the path argument exceeding "\
                     "{PATH_MAX} or a pathname component being longer than "\
                     "{NAME_MAX} or as a result of encountering a symbolic "\
                     "link in resolution of the path argument, the length of "\
                     "the substituted pathname string exceeded {PATH_MAX}."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOENT:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a component of path not naming an existing file "\
                     "or path being an empty string.",                  \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case ENOTDIR:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a a component of the path prefix not being a "\
                     "directory.",                                      \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case EPERM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file named by path being a directory, and "\
                     "either the calling process does not have appropriate "\
                     "privileges, or the implementation prohibits using " \
                     "unlink() on directories.",                        \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case EROFS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the directory entry to be unlinked is part of a "\
                     "read-only file system.",\
                     __VA_ARGS__);                                      \
            break;                                                      \
        case ETXTBSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the entry to be unlinked being the last directory "\
                     "entry to a pure procedure (shared text) file that is "\
                     "being executed.",                                 \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case ELOOP:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a loop existing in symbolic links encountered "\
                     "during resolution of the path argument or more than "\
                     "{SYMLOOP_MAX} symbolic links were encountered during "\
                     "resolution of the path argument.", __VA_ARGS__);  \
            break;

#define i_RP_UNLINK_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_UNLINK_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_UNLINK_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_UNLINK_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_UNLINK_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_UNLINK_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_UNLINK_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_UNLINK_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_UNLINK_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif







/*--- Logging macros for rmdir --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "rmdir" */
#define RF_ERROR_RMDIR(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_RMDIR_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_RMDIR_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_RMDIR_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_RMDIR_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_RMDIR_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_RMDIR_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EACCES:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to search permission being denied for a component of "\
                     "the path prefix, or write permission is denied on the "\
                     "directory containing the directory entry to be removed."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EBUSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the directory to be removed not being able "\
                     "to be remove because it is being used by the system or "\
                     "another process and the implementation considers this an "\
                     "error",                                           \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case EEXIST:                                                    \
        case ENOTEMPTY:                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the path argument naming a directory that is not "\
                     "an empty directory, or there are hard links to the "\
                     "directory other than dot or a single entry in dot-dot." \
                     ,__VA_ARGS__);                                     \
            break;                    \
        case EINVAL:                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the path argument containing a last compontent "\
                     "that is a dot", __VA_ARGS__);                     \
            break;                    \
        case EIO:                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a physical I/O error occuring", __VA_ARGS__); \
            break;                    \
        case ENAMETOOLONG:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the length of the path argument exceeding "\
                     "{PATH_MAX} or a pathname component being longer than "\
                     "{NAME_MAX} or as a result of encountering a symbolic "\
                     "link in resolution of the path argument, the length of "\
                     "the substituted pathname string exceeded {PATH_MAX}."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOENT:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a component of path not naming an exisint file or "\
                     "the path argument naming a  non existing " \
                     "directory or pointing to an empty string."         \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case ENOTDIR:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a a component of the path not being a "    \
                     "directory.",                                      \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case EPERM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the S_ISVTX flag being set on the parent directory "\
                     "of the directory to be removed and the caller not being "\
                     "the owner of the directory to be removed, nor is the "\
                     "caller the owner of the parent directory, nor does the "\
                     "caller have the appropriate privileges.",         \
                     __VA_ARGS__);                                      \
            break;                                                      \
        case EROFS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the directory entry to be removed is part of a "\
                     "read-only file system."                           \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case ELOOP:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a loop existing in symbolic links encountered "\
                     "during resolution of the path argument or more than "\
                     "{SYMLOOP_MAX} symbolic links were encountered during "\
                     "resolution of the path argument.", __VA_ARGS__);  \
            break;

#define i_RP_RMDIR_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_RMDIR_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_RMDIR_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_RMDIR_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_RMDIR_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_RMDIR_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_RMDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_RMDIR_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_RMDIR_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif






/*--- Logging macros for rename --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "rename" */
#define RF_ERROR_RENAME(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_RENAME_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_RENAME_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_RENAME_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_RENAME_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_RENAME_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_RENAME_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EACCES:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to a component of either path prefix denying search "\
                     "permission; or one of the directories containing old or "\
                     "new denies write permissions; or, write permission is "\
                     "required and is denied for a directory pointed to by the " \
                     "old or new arguments", __VA_ARGS__);              \
            break;                                                      \
        case EBUSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the directory named by old or new is currently "\
                     "in use by the system or another process, and the "\
                     "implementation considers this an error or the file named "\
                     "by the old or new arguments is a named STREAM.",__VA_ARGS__); \
            break;                                                      \
        case ENAMETOOLONG:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the length of the new or old argument exceeding "\
                     "{PATH_MAX} or a pathname component being longer than "\
                     "{NAME_MAX} or as a result of encountering a symbolic "\
                     "link in resolution of the path argument, the length of "\
                     "the substituted pathname string exceeded {PATH_MAX}."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EEXIST:\
        case ENOTEMPTY:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the link named by new being a directory that "\
                     "is not an empty directory.",__VA_ARGS__);         \
            break;                                                      \
        case EINVAL:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the new directory pathname containing a path "\
                     "prefix that names the old directory",__VA_ARGS__);         \
            break;                                                      \
        case EIO:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a physical I/O error occuring.",__VA_ARGS__); \
            break;                                                      \
        case EISDIR:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the new argument pointing to a directory and the "\
                     "old argument pointing to a file that is not a directory."\
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EMLINK:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file named by old being a directory, and the "\
                     "link count of the parent directory of new would exceed "\
                     "{LINK_MAX}.",__VA_ARGS__);                        \
            break;                                                      \
        case ENOENT:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the link named by old not naming an existing file," \
                     "  or either old or new pointing to an empty string." \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case ENOTDIR:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a component of either path prefix not being a "\
                     "directory; or the old argument nameing a directory and "\
                     "new argument naming a non-directory file.",__VA_ARGS__); \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the directory that would contain new not being "\
                     "able to be extended.", __VA_ARGS__);              \
            break;                                                      \
        case EPERM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the S_ISVTX flag being set on the directory " \
                     "containing the file referred to by old and the caller is " \
                     "not the file owner, nor is the caller the directory owner,"\
                     " nor does the caller have appropriate privileges; or new " \
                     "refers to an existing file, the S_ISVTX flag is set on " \
                     "the directory containing this file, and the caller is " \
                     "not the file owner, nor is the caller the directory " \
                     "owner, nor does the caller have appropriate privileges." \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EROFS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the requested operation requiring to write in a "\
                     "directory of a read only filesystem.",__VA_ARGS__); \
            break;                                                      \
        case EXDEV:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the links named by new and old being on different "\
                     "file systems and the implementation does not support "\
                     "links between file systems.",__VA_ARGS__); \
            break;                                                      \
        case ETXTBSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the file to be renamed being a pure procedure "\
                     "(shared text) file that is being executed.",__VA_ARGS__); \
            break;                                                      \
        case ELOOP:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a loop existing in symbolic links encountered "\
                     "during resolution of the path argument or more than "\
                     "{SYMLOOP_MAX} symbolic links were encountered during "\
                     "resolution of the path argument.", __VA_ARGS__);  \
            break;

#define i_RP_RENAME_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_RENAME_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_RENAME_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_RENAME_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_RENAME_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_RENAME_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_RENAME_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_RENAME_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_RENAME_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif







/*--- Logging macros for pthread_mutex_init --- */
/* The macro should accept 2 string literals and 1 error variable plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "pthread_mutex_init" */
#define RF_ERROR_PMUTEX_INIT(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_PMUTEX_INIT_ERROR_VARGS, 3, __VA_ARGS__)

#define i_RP_PMUTEX_INIT_ERROR_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    i_RP_PMUTEX_INIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, __VA_ARGS__)

#define i_RP_PMUTEX_INIT_ERROR_VARGS0(i_msg_, i_func_name_, i_error_)  \
    i_RP_PMUTEX_INIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_PMUTEX_INIT_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EAGAIN:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the system lacking the necessary resources "\
                     "(other than memory) to initialise another mutex. "\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOMEM:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to insufficient memory existing to initialise the mutex." \
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EPERM:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed"\
                     " due to the caller not having the privilege to perform "\
                     "the operation." , __VA_ARGS__);                   \
            break;                                                      \
        case EBUSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the implementation having detected an attempt to " \
                     "re-initialise the object referenced by mutex, a previously "\
                     "initialised, but not yet destroyed, mutex. "      \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EINVAL:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the value specified by attr being invalid. " \
                     ,__VA_ARGS__);                                     \
            break;


#define i_RP_PMUTEX_INIT_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_PMUTEX_INIT_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_PMUTEX_INIT_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_PMUTEX_INIT_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_PMUTEX_INIT_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_PMUTEX_INIT_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_PMUTEX_INIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
    do{                                                                 \
    switch(i_error_)                                                       \
    {                                                                   \
        i_RP_PMUTEX_INIT_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, i_error_, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_PMUTEX_INIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif





/*--- Logging macros for pthread_mutex_destroy --- */
/* The macro should accept 2 string literals and 1 error variable plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "pthread_mutex_destroy" */
#define RF_ERROR_PMUTEX_DESTROY(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_PMUTEX_DESTROY_ERROR_VARGS, 3, __VA_ARGS__)

#define i_RP_PMUTEX_DESTROY_ERROR_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    i_RP_PMUTEX_DESTROY_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, __VA_ARGS__)

#define i_RP_PMUTEX_DESTROY_ERROR_VARGS0(i_msg_, i_func_name_, i_error_)  \
    i_RP_PMUTEX_DESTROY_ERROR_DETAILS(i_msg_, i_func_name_, i_error_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_PMUTEX_DESTROY_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EBUSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the implementation having detected an attempt to "\
                     "destroy the object referenced by mutex while it is locked "\
                     "or referenced (for example, while being used in a "\
                     "pthread_cond_wait() or pthread_cond_timedwait()) by "\
                     "another thread.",__VA_ARGS__);                    \
            break;                                                      \
        case EINVAL:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the value specified by mutex being invalid. " \
                     ,__VA_ARGS__);                                     \
            break;


#define i_RP_PMUTEX_DESTROY_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_PMUTEX_DESTROY_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_PMUTEX_DESTROY_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_PMUTEX_DESTROY_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_PMUTEX_DESTROY_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_PMUTEX_DESTROY_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_PMUTEX_DESTROY_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
    do{                                                                 \
    switch(i_error_)                                                       \
    {                                                                   \
        i_RP_PMUTEX_DESTROY_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, i_error_, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_PMUTEX_DESTROY_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif


/*--- Logging macros for pthread_mutex_lock --- */
/* The macro should accept 2 string literals and 1 error variable plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "pthread_mutex_lock" */
#define RF_ERROR_PMUTEX_LOCK(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_PMUTEX_LOCK_ERROR_VARGS, 3, __VA_ARGS__)

#define i_RP_PMUTEX_LOCK_ERROR_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    i_RP_PMUTEX_LOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, __VA_ARGS__)

#define i_RP_PMUTEX_LOCK_ERROR_VARGS0(i_msg_, i_func_name_, i_error_)  \
    i_RP_PMUTEX_LOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_PMUTEX_LOCK_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EAGAIN:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the maximum number of recursive locks for mutex " \
                     "having been exceeded. ",__VA_ARGS__);             \
            break;                                                      \
        case EDEADLK:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the current thread already owning the mutex. " \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EINVAL:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the value specified by mutex not refering to an "\
                     "initialised mutex object or the mutex was created with "\
                     "the protocol attribute having the value "\
                     "PTHREAD_PRIO_PROTECT and the calling thread's priority is "\
                     "higher than the mutex's current priority ceiling."\
                     ,__VA_ARGS__);                                     \
            break;


#define i_RP_PMUTEX_LOCK_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_PMUTEX_LOCK_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_PMUTEX_LOCK_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_PMUTEX_LOCK_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_PMUTEX_LOCK_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_PMUTEX_LOCK_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_PMUTEX_LOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
    do{                                                                 \
    switch(i_error_)                                                       \
    {                                                                   \
        i_RP_PMUTEX_LOCK_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, i_error_, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_PMUTEX_LOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif





/*--- Logging macros for pthread_mutex_unlock --- */
/* The macro should accept 2 string literals and 1 error variable plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "pthread_mutex_unlock" */
#define RF_ERROR_PMUTEX_UNLOCK(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_PMUTEX_UNLOCK_ERROR_VARGS, 3, __VA_ARGS__)

#define i_RP_PMUTEX_UNLOCK_ERROR_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    i_RP_PMUTEX_UNLOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, __VA_ARGS__)

#define i_RP_PMUTEX_UNLOCK_ERROR_VARGS0(i_msg_, i_func_name_, i_error_)  \
    i_RP_PMUTEX_UNLOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_PMUTEX_UNLOCK_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EAGAIN:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the maximum number of recursive locks for mutex " \
                     "having been exceeded. ",__VA_ARGS__);             \
            break;                                                      \
        case EPERM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the current thread not owning the mutex."\
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EINVAL:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the value specified by mutex not refering to an " \
                     "initialised mutex object"                         \
                     ,__VA_ARGS__);                                     \
            break;


#define i_RP_PMUTEX_UNLOCK_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_PMUTEX_UNLOCK_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_PMUTEX_UNLOCK_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_PMUTEX_UNLOCK_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_PMUTEX_UNLOCK_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_PMUTEX_UNLOCK_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_PMUTEX_UNLOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
    do{                                                                 \
    switch(i_error_)                                                       \
    {                                                                   \
        i_RP_PMUTEX_UNLOCK_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, i_error_, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_PMUTEX_UNLOCK_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif



/*--- Logging macros for sem_init --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "sem_ini" */
#define RF_ERROR_SEMINIT(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_SEMINIT_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_SEMINIT_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_SEMINIT_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_SEMINIT_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_SEMINIT_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_SEMINIT_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
    case EINVAL:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the value argument exceeding SEM_VALUE_MAX. "\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOSPC:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a resource required to initialise the semaphore "\
                     "having been exhausted, or the limit on semaphores "\
                     "(SEM_NSEMS_MAX) having been reached. "\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case ENOSYS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the function not being supported by this " \
                     "implementation." , __VA_ARGS__);                 \
            break;\
        case EPERM:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the process lacking the appropriate privileges "\
                     "to initialise the semaphore." , __VA_ARGS__);     \
            break;

#define i_RP_SEMINIT_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_SEMINIT_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_SEMINIT_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_SEMINIT_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_SEMINIT_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_SEMINIT_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_SEMINIT_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_SEMINIT_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_SEMINIT_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif






/*--- Logging macros for sem_destroy --- */
/* The macro should accept 2 string literals plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "sem_destroy" */
#define RF_ERROR_SEMDESTROY(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_SEMDESTROY_ERROR_VARGS, 2, __VA_ARGS__)

#define i_RP_SEMDESTROY_ERROR_VARGS1(i_msg_, i_func_name_, ...) \
    i_RP_SEMDESTROY_ERROR_DETAILS(i_msg_, i_func_name_, __VA_ARGS__)

#define i_RP_SEMDESTROY_ERROR_VARGS0(i_msg_, i_func_name_)        \
    i_RP_SEMDESTROY_ERROR_DETAILS(i_msg_, i_func_name_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_SEMDESTROY_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
    case EINVAL:                                                        \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed " \
                     "due to the sem argument not being a valid semaphore."\
                     , __VA_ARGS__);                                    \
            break;                                                      \
        case EBUSY:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "because there are currently processes blocked on "\
                     "the semaphore. ", __VA_ARGS__);                   \
            break;                                                      \
        case ENOSYS:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the function not being supported by this " \
                     "implementation." , __VA_ARGS__);                 \
            break;


#define i_RP_SEMDESTROY_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_SEMDESTROY_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_SEMDESTROY_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_SEMDESTROY_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_SEMDESTROY_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_SEMDESTROY_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_SEMDESTROY_ERROR_DETAILS(i_msg_, i_func_name_, ...)             \
    do{                                                                 \
    switch(errno)                                                       \
    {                                                                   \
        i_RP_SEMDESTROY_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, errno, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_SEMDESTROY_ERROR_DETAILS(i_msg_, i_func_name_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif






/*--- Logging macros for sem_wait --- */
/* The macro should accept 2 string literals and 1 error variable plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "sem_wait" */
#define RF_ERROR_SEMWAIT(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_SEMWAIT_ERROR_VARGS, 3, __VA_ARGS__)

#define i_RP_SEMWAIT_ERROR_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    i_RP_SEMWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, __VA_ARGS__)

#define i_RP_SEMWAIT_ERROR_VARGS0(i_msg_, i_func_name_, i_error_)  \
    i_RP_SEMWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_SEMWAIT_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EINVAL:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the sem argument not refering to a valid semaphore."\
                     ,__VA_ARGS__);                              \
            break;                                                      \
        case EDEADLK:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a deadlock condition being detected. " \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EINTR:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a signal interrupting the function."       \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case ENOSYS:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the function not being supported by this "\
                     "implementation",__VA_ARGS__);                    \
            break;


#define i_RP_SEMWAIT_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_SEMWAIT_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_SEMWAIT_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_SEMWAIT_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_SEMWAIT_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_SEMWAIT_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_SEMWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
    do{                                                                 \
    switch(i_error_)                                                       \
    {                                                                   \
        i_RP_SEMWAIT_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, i_error_, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_SEMWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif



/*--- Logging macros for sem_timedwait --- */
/* The macro should accept 2 string literals and 1 error variable plus any extra arguments */
/* The literals would be the error message and the name of the function */
/* for example "bad error"  "sem_timewait" */
#define RF_ERROR_SEMTIMEDWAIT(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_RP_SEMTIMEDWAIT_ERROR_VARGS, 3, __VA_ARGS__)

#define i_RP_SEMTIMEDWAIT_ERROR_VARGS1(i_msg_, i_func_name_, i_error_, ...) \
    i_RP_SEMTIMEDWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, __VA_ARGS__)

#define i_RP_SEMTIMEDWAIT_ERROR_VARGS0(i_msg_, i_func_name_, i_error_)  \
    i_RP_SEMTIMEDWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_)

#if RF_OPTION_ERROR_LOGGING_LEVEL > 1
// >> --- details with errno

#define i_RP_SEMTIMEDWAIT_CASES_ACTUAL(i_msg_, i_func_name_, ...) \
        case EINVAL:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "because the process or thread would have blocked, and "\
                     "the abs_timeout parameter specified a nanoseconds field "\
                     "value less than zero or greater than or equal to 1000 million."\
                     ,__VA_ARGS__);                              \
            break;                                                      \
        case EDEADLK:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a deadlock condition being detected. " \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case EINTR:                                                    \
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to a signal interrupting the function."       \
                     ,__VA_ARGS__);                                     \
            break;                                                      \
        case ENOSYS:\
            RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed "\
                     "due to the function not being supported by this "\
                     "implementation",__VA_ARGS__);                    \
            break;


#define i_RP_SEMTIMEDWAIT_CASES_VARGS1(i_msg_, i_func_name_, ...) \
        i_RP_SEMTIMEDWAIT_CASES_ACTUAL(i_msg_, i_func_name_, __VA_ARGS__)
#define i_RP_SEMTIMEDWAIT_CASES_VARGS0(i_msg_, i_func_name_) \
        i_RP_SEMTIMEDWAIT_CASES_ACTUAL(i_msg_, i_func_name_)

#define i_RP_SEMTIMEDWAIT_CASES(...) \
            RF_SELECT_FUNC_IF_NARGGT2(i_RP_SEMTIMEDWAIT_CASES_VARGS, 2, __VA_ARGS__)


#define i_RP_SEMTIMEDWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
    do{                                                                 \
    switch(i_error_)                                                       \
    {                                                                   \
        i_RP_SEMTIMEDWAIT_CASES(i_msg_, i_func_name_, __VA_ARGS__)              \
        i_RP_DEFAULT_SWITCH_CASE(i_msg_, i_func_name_, i_error_, __VA_ARGS__) \
    }                                                               \
}while(0)


#else
// >> --- no details just report error

#define i_RP_SEMTIMEDWAIT_ERROR_DETAILS(i_msg_, i_func_name_, i_error_, ...) \
         RF_ERROR(0, i_msg_". This happened because "i_func_name_"() failed." \
                  ,__VA_ARGS__);

#endif








#ifdef REFU_WIN32_VERSION
/**
 ** @brief Gets Windows Last System error
 **
 ** Gets Windows Last System error and turns it into a char* that later needs to be freed by the user using the @c LocalFree() function
 ** @param i_STRBUFF The String buffer
 **
 **/
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


//!@}
//end of the Doxygen RF_ErrorLoggingGRP group

#endif//include guards end
