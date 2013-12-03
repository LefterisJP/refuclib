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

#ifndef RF_UTILS_LOG_H
#define RF_UTILS_LOG_H

/*------------- Outside Module inclusion -------------*/
#include <rf_options.h> //for the logging level defines
#include <Definitions/retcodes.h> //for bool
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size types
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <String/rf_str_decl.h> //for RFstring
#include <String/rf_str_common.h> //for RFS_() macro
/*------------- libc inclusion -------------*/
#include <errno.h>  // for std library function error reporting
/*------------- End of includes -------------*/

/**
** @Defgroup RFerror_logging_g_r_p Error Logging
** @addtogroup RFerror_logging_g_r_p
** @{
**/

/**
 * The logging system levels. They follow
 * the syslog protocol as defined in RFC 5424
 * http://tools.ietf.org/html/rfc5424
 */
enum RFlog_level{
    LOG_EMERGENCY = 0,
    LOG_ALERT,
    LOG_CRITICAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_NOTICE,
    LOG_INFO,
    LOG_DEBUG,

    LOG_LEVELS
};




i_DECLIMEX_ bool rf_LogModule_Init(enum RFlog_level level, char *log_file_name);
i_DECLIMEX_ void rf_LogModule_Deinit();
i_DECLIMEX_ void rf_Log(enum RFlog_level level, const char* file,
                        const char* func,
                        int line, struct RFstring* msg);

i_DECLIMEX_ bool rf_LogFlush();


/*--- Logging macros --- */


#define RF_ERROR(...) rf_Log(LOG_ERROR, __FILE__, __func__, \
                             __LINE__,                      \
                             RFS_(__VA_ARGS__))
#define RF_INFO(...) rf_Log(LOG_INFO, __FILE__, __func__, \
                            __LINE__,                     \
                            RFS_(__VA_ARGS__))
#define RF_WARNING(...)  rf_Log(LOG_WARNING, __FILE__, __func__,  \
                                __LINE__,                         \
                                RFS_(__VA_ARGS__))

#ifdef RF_OPTION_DEBUG
#define RF_DEBUG(...)  rf_Log(LOG_DEBUG, __FILE__, __func__,  \
                              __LINE__, RFS_(__VA_ARGS__))
#else
#define RF_DEBUG(...)
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
//end of the Doxygen RFerror_logging_g_r_p group

#endif//include guards end
