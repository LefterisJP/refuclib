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
#include <Utils/sanity.h>
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
 * Create a temporary tring from a literal with optional printf-like argumens.
 * Unsafe version of @ref RFS(). Check the return value to see why this is unsafe.
 * 
 * @param s         A string literal 
 * @param ...       Optional prinflike arguments
 * @return          A pointer to the created string. The reason this is an unsafe
 *                  function is that if the buffer reallocates at some point after
 *                  we want to use this the pointer is invalidated.
 */
#define RFS_UNSAFE(...) RF_SELECT_STRING_CREATE_LOCAL(__VA_ARGS__)
/**
 * Create a temporary tring from a literal with optional printf-like argumens.
 * @param ret       Pass a string pointer by reference to have it point to the 
 *                  temporary string position in the buffer
 * @param s         A string literal 
 * @param ...       Optional prinflike arguments
 * @return          true if all went fine and false in error. Error most probably
 *                  means problems with the string buffer itself due to realloc
 */
#define RFS(...) RF_SELECT_STRING_CREATE_LOCAL_ASSIGN(__VA_ARGS__)
/**
 * A version of @ref RFS() which also performs an action in case of error
 * Keep in mind most errors are gonna be really hard to recover from.
 */
#define RFS_CHECK(stmt_, ...)                   \
    do {                                        \
        if (!RFS(__VA_ARGS__)) {                \
            RF_ERROR("RFS() failure");          \
            stmt_;                              \
        }                                       \
    } while (0)
/**
 * A version of @ref RFS() which kills the program in case of error
 */
#define RFS_OR_DIE(...)                                         \
    do {                                                    \
        if (!RFS(__VA_ARGS__)) {                            \
            RF_ASSERT_OR_EXIT(false, "RFS() failure");      \
        }                                                   \
    } while (0)
/**
 * Remember the current point in the string buffer.
 *
 * Should be used to save a state of the string buffer at any given point before
 * using @ref RFS(). Should always have an equivalent @ref RFS_pop()
 */
#define RFS_push() rf_strings_buffer_ctx_push()
/**
 * Pop a bookmarked string buffer point
 *
 * Used to pop back a buffer position saved by a push.
 * Should always have an equivalent @ref RFS_pop()
 */
#define RFS_pop() rf_strings_buffer_ctx_pop()

/* -- rf_strings_buffer_ctx_code -- */

/**
 * Initialize the strings buffer context
 *
 * @param string_buffer_size         The initial size of the string buffer
 * @return                           true in succes and false in failure
 */
bool rf_strings_buffer_ctx_init(size_t string_buffer_size);
void rf_strings_buffer_ctx_deinit();

/**
 * Reads the formatted string and the va_args and fills in
 * the string buffer returning the string's size in bytes
 * Can realloc the buffer if not enough space remains
 *
 * @param fmt[in]         The formatted string
 * @param size[out]       The string's byte size
 * @param buffPtr[out]    The pointer to the beginning of the String
 *                        in the internal buffer
 * @return                Returns @c true in success and @c false in failure
 */
bool rf_strings_buffer_fillfmt(const char *fmt,
                               unsigned int *size,
                               char **buffPtr,
                               va_list args);
void rf_strings_buffer_ctx_push();
void rf_strings_buffer_ctx_pop();

/* -- internal functions used in the above API -- */

i_DECLIMEX_ struct RFstring *i_rf_string_create_local(const char* s);
i_DECLIMEX_ struct RFstring *i_rf_string_create_localv(const char* s, ...);
i_DECLIMEX_ bool i_rf_string_create_local_assign(struct RFstring **ret, const char* s);
i_DECLIMEX_ bool i_rf_string_create_local_assignv(struct RFstring **ret, const char* s, ...);

#define RF_SELECT_STRING_CREATE_LOCAL(...)  \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL, 1, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL1(arg_)  \
    i_rf_string_create_local(arg_)
#define i_SELECT_RF_STRING_CREATELOCAL0(...)  \
    i_rf_string_create_localv(__VA_ARGS__)

#define RF_SELECT_STRING_CREATE_LOCAL_ASSIGN(...)  \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL_ASSIGN, 2, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL_ASSIGN1(ptr_, arg_)  \
    i_rf_string_create_local_assign(ptr_, arg_)
#define i_SELECT_RF_STRING_CREATELOCAL_ASSIGN0(ptr_, ...)  \
    i_rf_string_create_local_assignv(ptr_, __VA_ARGS__)

#ifdef __cplusplus
}//closing bracket for calling from C++
#endif


#endif//include guards end
