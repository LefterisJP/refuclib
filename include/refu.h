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
** @defgroup RF_GeneralGRP Initialization function
** @addtogroup RF_GeneralGRP
** @{
**/

/**
 ** @brief Initializes the ReFu library
 **
 ** This function needs to be called in the very beginning of any program
 ** using the library.
 **
 ** @param errstr           \rfoptional{"refuclib.log"} The filename of the
 **                          error log to create and append the library's log to
 ** @param lmsSize          \rfoptional{RF_OPTION_LOCALSTACK_MEMORY_SIZE}
 **                         The size of the main thread's local memory stack.
 **                         This will be the size by which the main thread's
 **                         local memory stack will be initialized. It allows
 **                         the usage of macros for temporary object creation.
 ** @param level            The level of the logging system. Possible values
 **                         are enumerated by @ref log_level_t
 ** @return                 Returns @c true in success
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_  bool rfInit(char *logstr, uint64_t size, log_level_t level);
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_  bool i_rfInit(char *logstr, uint64_t size, log_level_t level);
#define rfInit(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_INIT, 3 ,__VA_ARGS__)

#define i_NPSELECT_RF_INIT1(...)                                        \
    RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function "    \
                     "rfStringInit() accepts from 0 to 3 arguments\"")
#define i_NPSELECT_RF_INIT0(...) \
    RF_SELECT_FUNC(i_SELECT_RF_INIT,__VA_ARGS__)
#define i_SELECT_RF_INIT3(...)  i_rfInit(__VA_ARGS__)
#define i_SELECT_RF_INIT2(...)  i_rfInit(__VA_ARGS__, \
                                         RF_OPTION_LOG_LEVEL_DEFAULT)
#define i_SELECT_RF_INIT1(...)  i_rfInit(__VA_ARGS__,  \
                                         RF_OPTION_LOCALSTACK_MEMORY_SIZE, \
                                         RF_OPTION_LOG_LEVEL_DEFAULT)
#define i_SELECT_RF_INIT0(...)  i_rfInit("refuclib.log",    \
                                         RF_OPTION_LOCALSTACK_MEMORY_SIZE, \
                                         RF_OPTION_LOG_LEVEL_DEFAULT)
#else
i_DECLIMEX_  bool rfInit(char *logstr, uint64_t size, log_level_t level);
#endif
#endif
//! @}
//closing RF_GeneralGRP doxygen group


#ifdef __cplusplus
}
#endif


#endif//include guards end
