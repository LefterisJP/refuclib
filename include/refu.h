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
 ** @param errstr           A c string literal with the filename of the log
 **                         to create. If NULL then the value is "refuclib.log"
 ** @param lmsSize          The size of the main thread's local memory stack.
 **                         This will be the size by which the main thread's
 **                         local memory stack will be initialized. It allows
 **                         the usage of macros for temporary object creation.
 **                         If 0 then the compile time constant
 **                         RF_OPTION_LOCALSTACK_MEMORY_SIZE is used.
 ** @param level            The level of the logging system. Possible values
 **                         are enumerated by @ref RFlog_level
 ** @return                 Returns @c true in success
 **/
i_DECLIMEX_ bool rf_init(char *logstr, uint64_t size, enum RFlog_level level);

/**
 ** Deinitializes the library. Frees the constructs of all the modules
 **/
i_DECLIMEX_ void rf_deinit();

//! @}
//closing RFgeneral_g_r_p doxygen group


#ifdef __cplusplus
}
#endif


#endif//include guards end
