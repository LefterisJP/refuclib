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
/*------------- libc inclusion --------------*/
#include <stdarg.h> //for va_list and friends
#include <stdlib.h>
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
#define RFS_(...) RF_SELECT_STRING_CREATE_LOCAL(__VA_ARGS__)
#define RFS_assign(...) RF_SELECT_STRING_CREATE_LOCAL_ASSIGN(__VA_ARGS__)

///Internal functions that create a temporary RFstring*
i_DECLIMEX_ struct RFstring *i_rf_string_create_local(const char* s);
i_DECLIMEX_ struct RFstring *i_rf_string_create_localv(const char* s, ...);
i_DECLIMEX_ bool i_rf_string_create_local_assign(struct RFstring **ret, const char* s);
i_DECLIMEX_ bool i_rf_string_create_local_assignv(struct RFstring **ret, const char* s, ...);

#define RF_SELECT_STRING_CREATE_LOCAL(...)  \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL, 1, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL1(...)  \
    i_rf_string_create_local(__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL0(...)  \
    i_rf_string_create_localv(__VA_ARGS__)

#define RF_SELECT_STRING_CREATE_LOCAL_ASSIGN(...)  \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL_ASSIGN, 2, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL_ASSIGN1(ptr_, ...)  \
    i_rf_string_create_local_assign(ptr_, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL_ASSIGN0(ptr_, ...)  \
    i_rf_string_create_local_assignv(ptr_, __VA_ARGS__)


/// Since RFS_() macro uses the internal buffer we need to push and pop the
/// internal buffer for strings wherever RFS_() is used.
/// Note: All users need to include Persistent/buffers.h
/// TODO: fix this nasty inclusion requirement ...
#define RFS_buffer_push() uint32_t i_buffer_index_ ## __FILE__ = rf_buffer_index(TSBUFFA)
#define RFS_buffer_pop() rf_buffer_set_index_(TSBUFFA, i_buffer_index_ ## __FILE__)




bool rf_strings_buffer_ctx_init(size_t string_buffer_size);
void rf_strings_buffer_ctx_deinit();

/**
 * Reads the formatted string and the va_args and fills in
 * the buffer returning the string's size in bytes
 **
 ** @param fmt[in]         The formatted string
 ** @param size[out]       The string's byte size
 ** @param buffPtr[out]    The pointer to the beginning of the String
 **                        in the internal buffer
 ** @param bIndex[out]     The index to return the buffer to after we
 **                        are done with it
 ** @return                Returns @c true in success and @c false in failure
 */
bool rf_strings_buffer_fillfmt(const char *fmt,
                               unsigned int *size,
                               char **buffPtr,
                               unsigned int *bIndex,
                               va_list args);

void rf_strings_buffer_ctx_push();
void rf_strings_buffer_ctx_pop();
#define RFS_push() rf_strings_buffer_ctx_push()
#define RFS_pop() rf_strings_buffer_ctx_pop()

#ifdef __cplusplus
}//closing bracket for calling from C++
#endif


#endif//include guards end
