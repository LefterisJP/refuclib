/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
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
#include <Data_Structures/mbuffer.h>
#include <Persistent/buffers.h>
/*------------- libc inclusion --------------*/
#include <stdarg.h> //for va_list and friends
#include <stdlib.h>
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C" {
#endif

//! Printf format specifier for RFString
#define RFS_PF "%.*s"
//! Printf argument macro for RFstring, to be used in conjuction with the 'PF'
#define RFS_PA(i_str_)                                      \
    rf_string_length_bytes(i_str_), rf_string_data(i_str_)
//! For printfs, evaluate val_ as bool and output string representation
#define FMT_BOOL(val_) (val_) ? "true" : "false"

#if 0 /* TODO: Using this I get address always evaluates to true in some cases,
         think how we can get rid of that warning */

#if RF_OPTION_DEBUG
#define RFS_PA(i_str_)                             \
    (i_str_) ? rf_string_length_bytes(i_str_) : 0, \
        (i_str_) ? rf_string_data(i_str_) : ""
#else
#define RFS_PA(i_str_)                                      \
    rf_string_length_bytes(i_str_), rf_string_data(i_str_)
#endif
#endif

/**
 * Create a temporary string from a literal with optional printf-like arguments.
 *
 * Wrap around @ref RFS_PUSH() and @ref RFS_POP() to make sure the temporary
 * string is freed
 *
 * @param ret       Pass a string pointer by reference to have it point to the
 *                  temporary string position in the buffer
 * @param s         A string literal
 * @param ...       Optional prinflike arguments
 * @return          A pointer to the temporary string or NULL in failure
 */
#define RFS(...) RF_SELECT_STRING_CREATE_LOCAL(__VA_ARGS__)

/**
 * Just like @ref RFS() but kills the program in case of failure
 */
#define RFS_OR_DIE(...) RF_SELECT_STRING_CREATE_LOCAL_OR_DIE(__VA_ARGS__)

/**
 * Just like @ref RFS() but the resulting string is null terminated. It is useful
 * to create temporary strings to provide to APIs that accept nullterminated cstrings
 */
#define RFS_NT(...) RF_SELECT_STRING_CREATE_LOCAL_NT(__VA_ARGS__)

/**
 * Just like @ref RFS() but the resulting string is null terminated and the
 * program is killed in case of failure
 */
#define RFS_NT_OR_DIE(...) RF_SELECT_STRING_CREATE_LOCAL_OR_DIE_NT(__VA_ARGS__)

/**
 * Remember the current point in the string buffer.
 *
 * Should be used to save a state of the string buffer at any given point before
 * using @ref RFS(). Should always have an equivalent @ref RFS_POP()
 */
#define RFS_PUSH() rf_mbuffer_push(RF_TSBUFFM)

/**
 * Pop a bookmarked string buffer point
 *
 * Used to pop back a buffer position saved by a push.
 * Should always have an equivalent @ref RFS_POP()
 */
#define RFS_POP() rf_mbuffer_pop(RF_TSBUFFM)

/**
 * Reads the formatted string and the va_args and fills in
 * the string buffer returning the string's size in bytes (not including null termination)
 * Can realloc the buffer if not enough space remains
 *
 * @param fmt[in]         The formatted string
 * @param size[out]       The string's byte size not including the null
 *                        terminating character
 * @param buff_ptr[out]   The pointer to the beginning of the String
 *                        in the internal buffer
 * @return                Returns @c true in success and @c false in failure
 */
bool rf_strings_buffer_fillfmt(
    const char *fmt,
    unsigned int *size,
    char **buff_ptr,
    va_list args
);

/* -- internal functions used in the above API -- */
i_DECLIMEX_ struct RFstring *i_rf_string_create_local(
    bool null_terminate,
    const char *s
);
i_DECLIMEX_ struct RFstring *i_rf_string_create_local_or_die(
    bool null_terminate,
    const char *s
);
i_DECLIMEX_ struct RFstring *i_rf_string_create_localv(const char *s, ...);
i_DECLIMEX_ struct RFstring *i_rf_string_create_localv_or_die(
    const char *s,
    ...
);

#define RF_SELECT_STRING_CREATE_LOCAL(...)                              \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL, 1, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL1(slit_) \
    i_rf_string_create_local(false, slit_)
#define i_SELECT_RF_STRING_CREATELOCAL0(...)    \
    i_rf_string_create_localv(__VA_ARGS__)

#define RF_SELECT_STRING_CREATE_LOCAL_NT(...)                              \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL_NT, 1, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL_NT1(slit_) \
    i_rf_string_create_local(true, slit_)
#define i_SELECT_RF_STRING_CREATELOCAL_NT0(...)    \
    i_rf_string_create_localv(__VA_ARGS__)

#define RF_SELECT_STRING_CREATE_LOCAL_OR_DIE(...)                       \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL_OR_DIE, 1, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL_OR_DIE1(slit_)   \
    i_rf_string_create_local_or_die(false, slit_)
#define i_SELECT_RF_STRING_CREATELOCAL_OR_DIE0(...) \
    i_rf_string_create_localv_or_die(__VA_ARGS__)

#define RF_SELECT_STRING_CREATE_LOCAL_OR_DIE_NT(...)                       \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL_OR_DIE_NT, 1, __VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL_OR_DIE_NT1(slit_)   \
    i_rf_string_create_local_or_die(true, slit_)
#define i_SELECT_RF_STRING_CREATELOCAL_OR_DIE_NT0(...) \
    i_rf_string_create_localv_or_die(__VA_ARGS__)

#ifdef __cplusplus
}//closing bracket for calling from C++
#endif


#endif//include guards end
