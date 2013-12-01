/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
**
** --String/unicode.h
** This header declares functions and macros that deal with unicode
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h>
#include <Definitions/types.h>
#include <String/unicode.h>
---------------------For internal library include make sure to have----------------------------
*/

#ifndef RF_UNICODE_H
#define RF_UNICODE_H

#include <Utils/constcmp.h> //for RF_HEXEQ_C
#include <Definitions/retcodes.h> //for bool
enum i_RF_ENCODING_TYPES 
    {
        RF_UTF8=1,
        RF_UTF16,
        RF_UTF32
    };

/* maximum bytes of a UTF8 char */
#define MAX_UTF8C_BYTES  5


/**
 **  Checks if a given  byte (must be char and not unsigned char) 
 **/
#define rf_utf8_is_continuation_byte(b__)  \
    ( RF_HEXEQ_C( (  (~( (b__) ^ 0x80))>>6),0x3 ))

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @defgroup RFunicode_g_r_p Unicode
** @addtogroup RFunicode_g_r_p
** @{
**/

/**
 ** @brief Takes a buffer of unicode characters and turns them into a
 **  UTF-8 encoded string
 **
 ** This function accepts a buffer of unicode codepoints in @c codepoints and
 **  also requires to know the size of that buffer in characters
 ** as the @c charsN argument.
 **
 ** If successfull it will return the number of bytes in the encoded utf8 buffer
 ** inside the variable given at argument @c utf8Length and return the
 ** encoded utf8 inside @c utf8.
 **
 ** @warning This function allocates the UTF-8 buffer internally and the caller
 **  of this function either has to store it or free it after use.
 ** @param[in] codepoints The buffer of code points. Must be given as a
 ** buffer of @c uint32_t.
 ** @param[in] charsN The number of codepoints inside the buffer,
 **  that is how many characters are in there
 ** @param[out] utf8Length Pass a pointer to @c uint32_t to receive inside
 **  it the bytelength of the allocated utf8 buffer
 ** @param[in/out] utf8 Here the caller has to provide a byte buffer of size 
 ** great enough to hold the encoded utf8. To be safe provide a buffer of 
 ** size charsN*4. Does not get null terminated.
 ** @param buffSize The size of the buffer given at @c utf8 in bytes
 ** @return Returns @c true for success and @c false otherwise
 **
 **/
i_DECLIMEX_ bool rf_utf8_encode(const uint32_t* codepoints, 
                               uint32_t charsN, uint32_t* utf8Length,
                               char* utf8, uint32_t buffSize);

/**
 ** @brief Takes a unicode codepoint and turns them into a UTF-8 byte
 **
 ** If successfull it will return the number of bytes in the encoded utf8 buffer
 ** and return the UTF-8 encoded byte buffer in @c utf8.
 **
 ** @param[in] codepoint The codepoint to encode. Must be given as a @c uint32_t
 ** @param[out] utf8 Pass a buffer of at least 5 bytes here to receive
 **             the utf8 bytes. Does not get null terminated.
 ** @return Returns the number of bytes used for the utf8 conversion for
 ** success or @c -1 for error at encoding with additional error logging
 **
 **/
i_DECLIMEX_ int rf_utf8_encode_single(const uint32_t codepoint, char* utf8);

/**
 ** @brief Takes a utf8 buffer and decodes it into unicode codepoints
 **
 ** Note that the returned decoded codepoints buffer needs to be freed by
 ** the user explicitly after done with its use.
 ** @param[in] utf8            The utf8 buffer
 ** @param[in] utf8BLength     The bytes length of the UTF8 buffer
 ** @param[out] charsN         Pass a reference to an @c uint32_t here to receive
 **                            the number of unicode characters contained in
 **                            the @c utf8 and the returned codepoints buffer
 ** @param[in/out] code_points Pass a buffer of @c uint32_t to receive the code
 **                            points of the decoded utf8 buffer here.
 **                            The buffer has to be allocated and
 **                            be of at least 4 times the length of the utf8
 **                            buffer to be safe that it fits
 **                            the decoded codepoints
 ** @param buff_size The size of the buffer given at @c code_points.
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_utf8_decode(const char* utf8, uint32_t utf8BLength,
                               uint32_t* charsN, uint32_t* code_points,
                               uint32_t buff_size);


/**
 ** @brief Decodes a  UTF-16 byte stream into codepoints
 **
 ** Takes in a UTF-16 endoded byte stream  inside @c buff.
 ** Pass an int32_t in @c characterLength to receive the character length and 
 ** an @c uint32_t buffer at @c codepoints with enough allocated size to fit 
 ** any amount of characters that the UTF-16 stream could produce.
 ** @param[in] buff             The UTF-16 byte stream
 ** @param[in] in_buff_length   The length of the input UTF-16 stream
 **                             in bytes
 ** @param[out] characterLength Give a @c uint32_t here to receive the
 **                             character length of the decoded codepoints
 ** @param[in/out] codepoints   Provide an already allocated uint32_t buffer to
 **                             hold the decoded unicode codepoints. The buffer
 **                             must have enough length
 **                             to hold any decoded codepoints that will come
 **                             out of the UTF-16. The maximum size you can
 **                             provide to be safe is 2 times the size of
 **                             the UTF-16 buffer length in bytes
 ** @param[in] buff_size        The size of the buffer at @c codepoints 
 **                             given in bytes
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_utf16_decode(const char* buff, uint32_t in_buff_length,
                                uint32_t* length, uint32_t* codepoints,
                                uint32_t buff_size);

/**
 ** @brief Encodes a buffer of unicode codepoints into UTF-16.
 **
 ** @param[in] codepoints     Provides a buffer of unicode
 **                           codepoints for encoding
 ** @param[in] charsN         Provide the number of characters in the codepoints
 **                           buffer (excluding the null termination character)
 ** @param[out] utf16Length   Give a reference to a uint32_t to receive the
 **                           length in 16-bit words of the utf-16 buffer
 **                           that the function will return
 ** @param[out] utf16         Provide a buffer of big enough size here inside
 **                           which to receive the encoded UTF16 byte stream
 ** @param buff_size          Provide the size of the buffer given at @c utf16.
 **                           The size of the buffer must be big enough to fit
 **                           the encoded UTF-16. To be safe give a buffer the 
 **                           same size in bytes as that of the codepoints
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_utf16_encode(const uint32_t* codepoints,
                                uint32_t charsN, uint32_t* utf16Length,
                                uint16_t* utf16, uint32_t buff_size);

/**
 ** Parses a utf-8 byte sequence represented as a null terminated c-string
 ** and returns the byte length verifying its validity
 ** @param[in] bytes        A sequence of bytes encoded in the UTF-8 encoding
 ** @param[out] byteLength  Pass a reference to an uint32_t to obtain the number
 **                         of bytes that make up the sequence
 ** @return                 Returns @c true for proper utf8 byte sequence
 **                         and @c false otherwise
 **/
i_DECLIMEX_ bool rf_utf8_verify_cstr(const char* bytes,
                                   uint32_t* byteLength);


//! @}
//end of unicode doxygen group

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//end of include guards
