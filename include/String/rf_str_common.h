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
#ifndef RF_STRING_COMMON_H
#define RF_STRING_COMMON_H


/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h>
#include <String/rf_str_retrieval.h> //for string accessors
/*------------- Outside Module inclusion -------------*/
#include <Definitions/defarg.h> //for default argument trick
#include <Definitions/imex.h> //for the import export macro
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C" {
#endif

#define RF_STR_PF_FMT "%.*s"
#define RF_STR_PF_ARG(i_str_) \
    rf_string_length_bytes(i_str_), rf_string_data(i_str_)
#if 0 /* TODO: Using this I get address always evaluates to true in some cases,
         think how we can get rid of that warning */

#if RF_OPTION_DEBUG
#define RF_STR_PF_ARG(i_str_)                      \
    (i_str_) ? rf_string_length_bytes(i_str_) : 0, \
        (i_str_) ? rf_string_data(i_str_) : ""
#else
#define RF_STR_PF_ARG(i_str_) \
    rf_string_length_bytes(i_str_), rf_string_data(i_str_)
#endif
#endif
/**
 ** @brief Create a termporary String from a String literal
 **
 ** A macro to be used only inside a function call that accepts an
 ** @ref RFstring or any inherited data type
 ** such as @ref RF_StringX. It creates a Temporary RFstring*
 ** that will be used by the function. This macro accepts from 1 to N arguments.
 **
 ** The first argument shall either be a String literal or a printf styled
 ** string literal given in UTF-8 encoding. Other encodings are easy to support
 ** but are not currently supported since the current
 ** limitation is for UTF-8 encoded source code.
 **
 ** Optionally the first argument can be followed by a sequence of additional
 ** arguments that would specify how to format
 ** the string @c s.
 ** @warning
 ** + If no extra arguments are given then the given string @c s is
 ** not interpreted as a formatted
 ** string and is instead printed as is.
 ** @warning
 ** + This macro can not be called recursively with extra arguments and doing
 ** so is undefined behaviour.
 ** @param s                The formatted string that will constitute the
 **                         RF_String. Must be in the same encoding as that of
 **                         the source file. Default is UTF-8.
 ** @param ...              \rfoptional{nothing}  Depending on the string
 **                         literal, the function may expect a sequence of
 **                         additional arguments, each containing one value to
 **                         be inserted instead of each %-tag
 ** specified in the @c slit parameter, if any. There should be
 ** the same number of these arguments as the number of %-tags that expect a value.
 ** @return Returns true in case of correct initialization and false
 ** due to invalid byte sequence for the given encoding
 ** @isinherited{StringX}
 **/
#ifdef RF_IAMHERE_FOR_DOXYGEN
struct RFstring* RFS_(const char *s, ...);
#else
#define RFS_(...) i_rf_string_create_local(__VA_ARGS__)
#endif

///Internal functions that create a temporary RFstring*
i_DECLIMEX_ struct RFstring* i_rf_string_create_local1(const char* s,...);
i_DECLIMEX_ struct RFstring* i_NVrf_string_create_local(const char* s);
#define i_rf_string_create_local(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL,1,__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL1(...) i_NVrf_string_create_local(__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL0(...) i_rf_string_create_local1(__VA_ARGS__)


/// Since RFS_() macro uses the internal buffer we need to push and pop the
/// internal buffer for strings wherever RFS_() is used.
/// Note: All users need to include Persistent/buffers.h
/// TODO: fix this nasty inclusion requirement ...
#define RFS_buffer_push() uint32_t i_buffer_index_ ## __FILE__ = rf_buffer_index(TSBUFFA)
#define RFS_buffer_pop() rf_buffer_set_index_(TSBUFFA, i_buffer_index_ ## __FILE__)

#ifdef __cplusplus
}//closing bracket for calling from C++
#endif


#endif//include guards end
