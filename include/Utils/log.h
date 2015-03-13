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
#include <Definitions/inline.h> //for inline
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size types
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <String/rf_str_decl.h> //for RFstring
#include <String/rf_str_common.h> //for RFS_() macro
#include <Persistent/buffers.h> // for TSBUFFA
/*------------- libc inclusion -------------*/
#include <errno.h>  // for std library function error reporting
/*------------- End of includes -------------*/

/**
** @Defgroup RFerror_logging_GRP Error Logging
** @addtogroup RFerror_logging_GRP
** @{
**/

/* -- RFlog_target -- */

enum RFlog_target_type {
    LOG_TARGET_FILE = 0,
    LOG_TARGET_STDOUT,
    LOG_TARGET_STDERR,
};

struct RFlog_target {
    enum RFlog_target_type type;
    struct RFstring file_name;
};

bool rflog_target_init(struct RFlog_target *t,
                       enum RFlog_target_type type,
                       const char *file_name);

void rflog_target_deinit(struct RFlog_target *t);

/* -- RFlog -- */

/**
 * The logging system levels. They follow
 * the syslog protocol as defined in RFC 5424
 * http://tools.ietf.org/html/rfc5424
 * Must be kept in sync with the severity_level_string array in
 * log.c
 */
enum RFlog_level {
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

struct RFlog;


i_DECLIMEX_ struct RFlog *rf_log_create(enum RFlog_target_type type,
                                        const char *log_file_name,
                                        enum RFlog_level level);

i_DECLIMEX_ void rf_log_destroy(struct RFlog *log);

i_DECLIMEX_ bool rf_log_flush(struct RFlog *log);
//convenience macro (used only in tests)
#define RF_LOG_FLUSH() rf_log_flush(refu_clib_get_log())

i_DECLIMEX_ void rf_log(enum RFlog_level level, const char* file,
                        const char* func, int line,
                        struct RFstring* msg);


/*--- Logging macros --- */


#define RF_ALERT(...) do {                      \
        RFS_buffer_push();                      \
        rf_log(LOG_ALERT, __FILE__, __func__,   \
               __LINE__,                        \
               RFS_(__VA_ARGS__));              \
        RFS_buffer_pop();                       \
    } while (0)

#define RF_CRITICAL(...) do {                       \
        RFS_buffer_push();                          \
        rf_log(LOG_CRITICAL, __FILE__, __func__,    \
               __LINE__,                            \
               RFS_(__VA_ARGS__));                  \
        RFS_buffer_pop();                           \
    } while (0)

#define RF_ERROR(...) do {                      \
        RFS_buffer_push();                      \
        rf_log(LOG_ERROR, __FILE__, __func__,   \
               __LINE__,                        \
               RFS_(__VA_ARGS__));              \
        RFS_buffer_pop();                       \
    } while (0)

#define RF_WARNING(...) do {                    \
        RFS_buffer_push();                      \
        rf_log(LOG_WARNING, __FILE__, __func__, \
               __LINE__,                        \
               RFS_(__VA_ARGS__));              \
        RFS_buffer_pop();                       \
    } while (0)

#define RF_NOTICE(...) do {                     \
        RFS_buffer_push();                      \
        rf_log(LOG_NOTICE, __FILE__, __func__, \
               __LINE__,                        \
               RFS_(__VA_ARGS__));              \
        RFS_buffer_pop();                       \
    } while (0)

#define RF_INFO(...) do {                       \
        RFS_buffer_push();                      \
        rf_log(LOG_INFO, __FILE__, __func__,    \
               __LINE__,                        \
               RFS_(__VA_ARGS__));              \
        RFS_buffer_pop();                       \
    } while (0)



#ifdef RF_OPTION_DEBUG
#define RF_DEBUG(...) do {                      \
        RFS_buffer_push();                      \
        rf_log(LOG_DEBUG, __FILE__, __func__,   \
               __LINE__,                        \
               RFS_(__VA_ARGS__));              \
        RFS_buffer_pop();                       \
    } while (0)
#else
#define RF_DEBUG(...)
#endif

#define RF_ERRNOMEM() RF_CRITICAL("We are out of memory!")

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
