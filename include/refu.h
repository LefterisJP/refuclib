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
#ifndef REFU_SETUP_H
#define REFU_SETUP_H


#include <rf_options.h>
#include <Utils/log.h>
#include <Definitions/retcodes.h> /* for bool */

#ifdef __cplusplus
extern "C" {
#endif

struct refu_clibctx;
struct RFlog;

#define RF_DEFAULT_STRING_BUFFERS_SIZE 1024

/**
** @defgroup RFgeneral_g_r_p Initialization function
** @addtogroup RFgeneral_g_r_p
** @{
**/

/**
 ** @brief Initializes the ReFu library
 **
 ** This function needs to be called in the very beginning of any program
 ** using the library.
 **
 ** @param log_type             The type of target for all logging functionality.
 **                             Choose between a file, stdout or stderr.
 **                             For more details: @ref RFlog_target_type
 ** @param log_file_name        A c string literal with the filename of the log
 **                             to create.
 ** @param level                The level of the logging system. Possible values
 **                             are enumerated by @ref RFlog_level
 ** @param string_buffer_size   The initial buffer size in bytes that will be
 **                             given to thetemporary string buffer
 ** @return                     Returns @c true in success
 **/
i_DECLIMEX_ bool rf_init(enum RFlog_target_type log_type,
                         const char *log_file_name,
                         enum RFlog_level level,
                         size_t string_buffer_size);

/**
 ** Deinitializes the library. Frees the constructs of all the modules
 **/
i_DECLIMEX_ void rf_deinit();



/* Methods to get specific handlers of the library */
i_DECLIMEX_ struct refu_clibctx *refu_clib_get_ctx();

i_DECLIMEX_ struct RFlog *refu_clib_get_log();

//! @}
//closing RFgeneral_g_r_p doxygen group


#ifdef __cplusplus
}
#endif


#endif//include guards end
