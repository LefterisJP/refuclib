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
#ifndef RF_COMMON_FLAGS_H
#define RF_COMMON_FLAGS_H

#include <sys/types.h>

/**
 ** This is the type that represents the file offset
 **/
#if _FILE_OFFSET_BITS == 64

#ifdef _MSC_VER
typedef __int64 foff_rft;
#else
typedef off64_t foff_rft;
#endif

/* for now, don't have a stat_t equivalent in windows */
typedef struct stat64 stat_rft;

#else /* __ FILE_OFFSET_BITS != 64 */

#ifdef _MSC_VER
typedef __int32 foff_rft;
#else

typedef off_t foff_rft;
#endif

/* for now, don't have a stat_t equivalent in windows */
typedef struct stat stat_rft;

#endif

/**
 ** New line feed
 **
 **/
#define RF_LF   0xA
/**
 ** Carriage Return
 **
 **/
#define RF_CR   0xD

#ifdef REFU_WIN32_VERSION
    #define i_PLUSB_WIN32   "b"
#else
    #define i_PLUSB_WIN32   ""
#endif

/**
 ** The different end of line mark types for
 **/
enum RFeol_mark {
    RF_EOL_AUTO  = 1,/*!< Used by functions that can autodetect the eol mark */
    RF_EOL_LF, /*!< Unix style line ending with '\n' */
    RF_EOL_CRLF, /*!< Windows style line ending with '\r\n' */
    RF_EOL_CR /*!< Macintosh style line ending with '\r' */
};
#define RF_EOL_DEFAULT  RF_EOL_LF//the default value is LF only (Unix-style)

#endif//include guards end
