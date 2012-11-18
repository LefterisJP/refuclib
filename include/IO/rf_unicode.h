/*
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RF_UNICODE_H
#define RF_UNICODE_H

#include <rf_setup.h>

//! UTF-8 Encoding
#define RF_UTF8     1
//! UTF-16 Encoding
#define RF_UTF16    2
//! UTF-16 Big Endian encoding
#define RF_UTF16_BE 3
//! UTF-16 Little Endian encoding
#define RF_UTF16_LE 4
//! UTF-32 Encoding
#define RF_UTF32    5
//! UTF-32 Big Endian encoding
#define RF_UTF32_BE 6
//! UTF-32 Little Endian encoding
#define RF_UTF32_LE 7

// Checks if a given  byte (must be char and not unsigned char) is a continuation byte
#define rfUTF8_IsContinuationByte(b__)  ( RF_HEXEQ_C( (  (~( (b__) ^ 0x80))>>6),0x3 ))

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @defgroup RF_UnicodeGRP Unicode
** @addtogroup RF_UnicodeGRP
** @{
**/

//! @brief Takes a buffer of unicode characters and turns them into a UTF-8 encoded string
//!
//! This function accepts a buffer of unicode codepoints in @c codepoints and also requires to know
//! the size of that buffer in characters as the @c charsN argument.
//!
//! If successfull it will return the number of bytes in the encoded utf8 buffer
//! inside the variable given at argument @c utf8Length and return the allocated UTF-8 encoded
//! byte buffer.
//!
//! @warning This function allocates the UTF-8 buffer internally and the user of this function either
//! has to store it or free it after use.
//! @param[in] codepoints The buffer of code points. Must be given as a buffer of @c int32_t.
//! @param[in] charsN The number of codepoints inside the buffer, that is how many characters are in there
//! @param[out] utf8Length Pass a pointer to @c uint32_t to receive inside it the bytelength of the allcoated utf8 buffer without the null termination counted
//! @return Returns the allocated UTF-8 encoded byte buffer for success or @c 0 if there was an error
i_DECLIMEX_ char* rfUTF8_Encode(const uint32_t* codepoints,uint32_t charsN,uint32_t* utf8Length);

//! @brief Takes a utf8 buffer and decodes it into unicode codepoints
//!
//! Note that the returned decoded codepoints buffer needs to be freed by
//! the user explicitly after done with its use. The returned buffer is null terminated
//! @param[in] utf8 The utf8 buffer
//! @param[in] utf8BLength The bytes length of the UTF8 buffer
//! @param[out] charsN Pass a reference to an @c uint32_t here to receive the number of
//! unicode characters contained in the @c utf8 and the returned codepoints buffer
//! @return Returns the allocated codepoints buffer. The returned buffer is null terminated
i_DECLIMEX_ uint32_t* rfUTF8_Decode(const char* utf8,uint32_t utf8BLength,uint32_t* charsN);


//! @brief Decodes a  UTF-16 byte stream into codepoints
//!
//! Takes in a UTF-16 endoded byte stream  inside @c buff.
//! Pass an int32_t in @c characterLength to receive the character length and an int32_t buffer at
//! @c codepoints with enough allocated size to fit any amount of characters that the UTF-16
//! stream could produce.
//! @param[in] buff The UTF-16 byte stream
//! @param[out] characterLength Give a uint32_t here to receive the character length of the decoded codepoints
//! @param[in] codepoints Provide an already allocated int32_t buffer to hold the decoded unicode codepoints.
//! The buffer must be allocated with the max amount of characters the UTF-16 byte stream can have, or the exact amount
//! if somehow you already know that at the function's call. If all you know is the bytes number ofthe UTF-16 buffer
//! then allocate this with @c UTF16BLENGTH*2.
//! @return Returns @c RF_SUCCESS for succesfull UTF-16 decoding and error otherwise. Possible errors are:
//! + @c RE_UTF16_INVALID_SEQUENCE: An invalid UTF-16 byte was found in @c buff
//! + @c RE_UTF16_NO_SURRPAIR: A surrogate pair was expected somewhere in @c buff but it was not found
i_DECLIMEX_ int32_t rfUTF16_Decode(const char* buff,uint32_t* length,uint32_t* codepoints);
//! @brief Decodes a UTF-16 byte stream into codepoints also swapping endianess
//!
//! Takes in a UTF-16 endoded byte stream inside @c buff.
//! Pass an int32_t in @c characterLength to receive the character length and an int32_t buffer at
//! @c codepoints with enough allocated size to fit any amount of characters that the UTF-16
//! stream could produce.
//!
//! This function swaps endianess internally
//! @param[in] buff The UTF-16 byte stream
//! @param[out] characterLength Give a uint32_t here to receive the character length of the decoded codepoints
//! @param[in] codepoints Provide an already allocated int32_t buffer to hold the decoded unicode codepoints.
//! The buffer must be allocated with the max amount of characters the UTF-16 byte stream can have, or the exact amount
//! if somehow you already know that at the function's call. If all you know is the bytes number ofthe UTF-16 buffer
//! then allocate this with @c UTF16BLENGTH*2.
//! @return Returns @c RF_SUCCESS for succesfull UTF-16 decoding and error otherwise. Possible errors are:
//! + @c RE_UTF16_INVALID_SEQUENCE: An invalid UTF-16 byte was found in @c buff
//! + @c RE_UTF16_NO_SURRPAIR: A surrogate pair was expected somewhere in @c buff but it was not found
i_DECLIMEX_ int32_t rfUTF16_Decode_swap(const char* buff,uint32_t* length,uint32_t* codepoints);

//! @brief Encodes a buffer of unicode codepoints into UTF-16
//!
//! Note that the returned allocated UTF-16 buffer needs to be freed
//! by the user explicitly after done with its use. Also the returned buffer is null terminated.
//! @param[in] codepoints Provides a buffer of unicode codepoints for encoding
//! @param[in] charsN Provide the number of characters in the codepoints buffer (excluding the null termination character)
//! @param[out] utf16Length Give a reference to a uint32_t to receive the length in 16bit unsigned integers of the utf-16
//! buffer that the function will return
//! @return Returns the a utf-16 encoded buffer of the given @c codepoints for success.
//! If an illegal codepoint is found then an error is logged and 0 is returned. The buffer needs to be freed by the user later.
//! The returned buffer is null terminated.
i_DECLIMEX_ uint16_t* rfUTF16_Encode(const uint32_t* codepoints,uint32_t charsN,uint32_t* utf16Length);

//! Parses a utf-8 byte sequence returning the byte length and verifying its validity
//! @param[in] bytes A sequence of bytes encoded in the UTF-8 encoding
//! @param[out] byteLength Pass a reference to an uint32_t to obtain the number of bytes that make up the sequence not including the '\0' byte
//! @return Returns  @c RF_SUCCESS if @c bytes is a valid UTF8 sequence and an error if there is a problem. Possible errors are:
//! + @c RE_UTF8_INVALID_SEQUENCE_INVALID_BYTE: An invalid UTF-8 byte was encountered inside @c bytes
//! + @c RE_UTF8_INVALID_SEQUENCE_CONBYTE: Somewhere in @c bytes a continuation byte was expected but was not found
//! + @c RE_UTF8_INVALID_SEQUENCE: General Invalid UTF-8 sequence error
i_DECLIMEX_ int32_t rfUTF8_VerifySequence(const char* bytes,uint32_t* byteLength);

//! @brief Finds the length of a UTF-32 buffer
//!
//! Finds the length (in code points) of a UTF-32 buffer which is already in correct endianess and  is an @c uint32_t buffer
//! @param[in] buffer The utf32 buffer. Must be null terminated and contain @c uint32_t
//! @param[ou] bLength Pass a uint32_t here to get the length of the buffer. It returns the length of the string in characters NOT INLUDING the null terminating character
#ifdef RF_IAMHERE_FOR_DOXYGEN
void rfUTF32_Length(uint32_t* buffer,uint32_t bLength);
#else
#define rfUTF32_Length(i_BUFFER_,i_BLENGTH_)  {\
    i_BLENGTH_=0;\
    while(i_BUFFER_[i_BLENGTH_] != 0){ i_BLENGTH_++;}}
#endif
//! @brief Turns a unicode code point into utf8
//!
//! A function that takes in a single unicode codepoint, turns it into UTF-8
//! and returns the utf8 bytelength of the codepoint
//! @param[in] codepoint The unicode codepoint
//! @param[out] utf8 The returned utf8 byte sequence inside a @c uint32_t
//! @return Returns the bytelength of the utf8 or @c 0 if there was failure
i_DECLIMEX_ char rfUTF8_FromCodepoint(uint32_t codepoint,uint32_t* utf8);

//! @}
//end of unicode doxygen group

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//end of include guards
