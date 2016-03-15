/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_FILESX_H
#define RF_STRING_FILESX_H

#include <rflib/string/rf_str_xdecl.h>

#include <rflib/defs/imex.h>
#include <rflib/defs/types.h>
#include <rflib/defs/retcodes.h>
#include <rflib/utils/endianess.h>
#include <rflib/utils/rf_unicode.h>
#include <rflib/io/rf_io_common.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name RFstringx File I/O
//! @{

/**
 * @brief Initializes a stringX from file parsing
 *
 * Read the file stream @c f until either a newline character or the EOF is
 * reached and saves it as an RFstringx
 * If for some reason (like EOF reached) no string can be read then null is returned
 * @param str The StringX to initialize
 * @param f A valid and open file pointer in read mode from which to read the
 * string.
 * @param[out] eof Pass a pointer to a char to receive a true or false
 * value in case the end of file was reached with this initialization
 * @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 *                for possible values
 * @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 *                 definition for possible values.
 * @param endianess A flag that determines in what endianess the file is
 * encoded in. Look @ref RFendianess for posssible values.
 * @return Returns @c true for success and @c false otherwise
 */
i_DECLIMEX_ bool rf_stringx_from_file_init(
    struct RFstringx *str,
    FILE *f,
    char *eof,
    enum RFeol_mark eol,
    enum RFtext_encoding encoding,
    enum RFendianess endianess
);

/**
 * @brief Allocates and returns a stringX from file parsing
 * @return The initialized string or null pointer in case of failure
 *  to read the file, or unexpected data (non-UTF8 encoded string)
 * @see rf_stringx_from_file_init()
 * @see rf_stringx_from_file_assign()
 * @see rf_stringx_from_file_append()
 */
i_DECLIMEX_ struct RFstringx *rf_stringx_from_file_create(
    FILE *f,
    char *eof,
    enum RFeol_mark eol,
    enum RFtext_encoding encoding,
    enum RFendianess endianess
);

/**
 * @brief Assigns to a stringX from file parsing
 *
 * Read the file stream @c f until either a newline character or the EOF is
 * reached and assigns it to an RFstringx
 * @param str The stringX to assign to
 * @param f A valid and open file pointer in read mode from which to read the
 * string.
 * @param[out] eof Pass a pointer to a char to receive a true or false
 * value in case the end of file was reached with this initialization
 * @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 *                for possible values
 * @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 *                 definition for possible values.
 * @param endianess A flag that determines in what endianess the file is
 * encoded in. Look @ref RFendianess for posssible values.
 * @return Returns @c true for success and @c false otherwise
 * @see rf_stringx_from_file_init()
 * @see rf_stringx_from_file_create()
 * @see rf_stringx_from_file_append()
 */
i_DECLIMEX_ bool rf_stringx_from_file_assign(
    struct RFstringx *str,
    FILE *f,
    char *eof,
    enum RFeol_mark eol,
    enum RFtext_encoding encoding,
    enum RFendianess endianess
);

/**
 * @brief Appends to a stringX from file parsing
 *
 * @param str The stringX to append to
 * @param f A valid and open file pointer in read mode from which to read the
 * string.
 * @param[out] eof Pass a pointer to a char to receive a true or false
 * value in case the end of file was reached with this initialization
 * @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 *                for possible values
 * @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 *                 definition for possible values.
 * @param endianess A flag that determines in what endianess the file is
 * encoded in. Look @ref RFendianess for posssible values.
 * @return Returns @c true for success and @c false otherwise
 * @see rf_stringx_from_file_init()
 * @see rf_stringx_from_file_create()
 * @see rf_stringx_from_file_assign()
 */
i_DECLIMEX_ bool rf_stringx_from_file_append(
    struct RFstringx *str,
    FILE *f,
    char *eof,
    enum RFeol_mark eol,
    enum RFtext_encoding encoding,
    enum RFendianess endianess
);

//! @}



#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
