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
#ifndef RF_STRING_CONVERSION_H
#define RF_STRING_CONVERSION_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstring/X
#include <String/rf_str_retrieval.h>//for rf_string_data()
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
#include <Definitions/inline.h> //for inline
#include <Utils/rf_unicode.h> //for rf_utf8_is_continutation_byte
/*------------- libc inclusion --------------*/
 #include <string.h> //for size_t
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name Unicode Conversion Functions
//! @{

/**
 ** @brief Returns the strings contents as a UTF-16 buffer
 **
 ** @isinherited{StringX}
 ** This function allocates a UTF-16 buffer in which the string's
 ** UTF-8 contents are encoded as UTF-16. The endianess of the buffer
 ** is that of the system. The returned buffer needs to be freed by the user
 ** later.
 ** @param[in]  s      The string in question. @inhtype{String,StringX}
 ** @param[out] length Give a reference to a uint32_t in
 **             this argument to receive the length of
 **             the returned UTF-16 buffer in 16-bit words
 ** @return Returns an allocated UTF-16 buffer. Needs to be freed by the user later.
 ** @see rf_string_tout_f8()
 ** @see rf_string_to_utf32()
 **/
i_DECLIMEX_ uint16_t* rf_string_to_utf16(const void* s, uint32_t* length);

/**
 ** @brief Returns the strings contents as a UTF-32 buffer
 **
 ** @isinherited{StringX}
 ** This function allocates a UTF-32 buffer in which the string's
 ** UTF-8 contents are encoded as UTF-32. The endianess of the buffer
 ** is that of the system. The returned buffer needs to be freed by the user
 ** later.
 ** @param[in]  s      The string in question. @inhtype{String,StringX}
 ** @param[out] length Give a reference to a uint32_t in this
 **                    argument to receive the length
 **                   of the returned UTF-32 buffer in codepoints. (32-bit) words
 ** @return Returns an allocated UTF-16 buffer. Needs to be freed by the user later.
 ** @see rf_string_tout_f8()
 ** @see rf_string_to_utf16()
 **/
i_DECLIMEX_ uint32_t* rf_string_to_utf32(const void* s,uint32_t*length);

//! @}

//! @name String Conversion Functions
//! @{

/**
 ** @brief Creates a Cstring representation of the string
 **
 ** @isinherited{StringX}
 ** Since RFstring is not null terminated this function shall allocate
 ** a c string where it will be stored.
 **
 ** @warning Make sure to free the returned string after using
 **
 ** @param s       The string whose cstring repesentation to get.
 **                @inhtype{String,StringX}
 ** @return        Returns the pointer to the allocated c string
 **/
i_DECLIMEX_ char *rf_string_cstr(const void* s);

/**
 ** Turns the given string into a null terminated c string.
 ** Make sure to call @ref RFS_push() before use
 **  and @ref RFS_pop() after use
 **/
i_DECLIMEX_ char *rf_string_cstr_from_buff(const void* s);
i_DECLIMEX_ char *rf_string_cstr_from_buff_or_die(const void* s);

/**
 ** @brief Returns the integer value of a String
 **
 ** The parameter string must start with a number in any form.
 ** @isinherited{StringX}
 ** @param thisstr     The string whose integer value to return.
 **                    @inhtype{String,StringX}
 ** @param start_off  The offset of the string at which to start
 **                    the conversion attempt. This is needed since some
 **                    bases, like octal and binary may start with 0b or 0
 **                    and are not understoof by strtol().
 ** @param[out] v      Will hold the return value
 ** @param[out] off    Will hold the offset from the beginning of
 **                    the string up to which a conversion happened.
 **                    Is absolutely needed for hex and octal versions.
 ** @return            True in case of succesfull conversion or false if no
 **                    integer was represented by the string
 ** @see rf_string_to_double()
 **/
i_DECLIMEX_ bool rf_string_to_int(const void* thisstr, int64_t* v, size_t *off);
i_DECLIMEX_ bool rf_string_to_uint(const void* thisstr,
                                   size_t start_off,
                                   uint64_t* v,
                                   size_t *off,
                                   int base);

i_INLINE_DECL bool rf_string_to_uint_dec(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off)
{
    return rf_string_to_uint(thisstr, 0, v, off, 10);
}

i_INLINE_DECL bool rf_string_to_uint_hex(const void* thisstr,
                                         uint64_t* v,
                                         size_t *off)
{
    const char *c = rf_string_data(thisstr);
    if (!rf_string_to_uint(thisstr, 0, v, off, 16)) {
        return false;
    }

    if (c[0] == '0' && (c[1] == 'x' || c[1] == 'X') && *off <= 1) {
        return false;
    }

    return true;
}

i_INLINE_DECL bool rf_string_to_uint_bin(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off)
{
    char *s = rf_string_data(thisstr);

    if (rf_string_length_bytes(thisstr) > 2 && *s == '0' && *(s + 1) == 'b') {
        return rf_string_to_uint(thisstr, 2, v, off, 2);
    }
    return rf_string_to_uint(thisstr, 0, v, off, 2);
}

i_INLINE_DECL bool rf_string_to_uint_oct(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off)
{
    const char *c = rf_string_data(thisstr);
    if (!rf_string_to_uint(thisstr, 0, v, off, 0)) {
        return false;
    }

    if (c[0] == '0' && *off <= 1) {
        return false;
    }

    return true;
}

/**
 ** @brief Returns the double value of a String
 **
 ** @isinherited{StringX}
 ** The parameter string must start with a floating point number
 ** @param thisstr     The string whose floating point value to return.
 **                    @inhtype{String,StringX}
 ** @param[out] f      Will hold the return float value
 ** @param[out] off    Optional. Will hold the offset from the beginning of
 **                    the string up to which a conversion happened.
 ** @return            True in case of succesfull conversion or false if no
 **                    double was represented by the string
 ** @see rf_string_to_int()
 **/
i_DECLIMEX_ bool rf_string_to_double(const void* thisstr,
                                     double* f,
                                     size_t *off);

/**
 ** @brief Turns any uppercase characters of the string into lower case
 **
 ** @isinherited{StringX}
 ** This function as of now only works with letter of the English alphabet.
 ** All the characters of the string that are uppercase shall
 ** be turned into lowercase
 ** @param thisstr The string for which to perform the uppercase
 **                to lowercase conversion
 ** @see rf_string_to_upper()
 **/
i_DECLIMEX_ void rf_string_to_lower(void* thisstr);

/**
 ** @brief Turns any lowercase characters of the string into upper case
 **
 ** @isinherited{StringX}
 ** This function as of now only works with letter of the English alphabet.
 ** All the characters of the string that are lowercase shall be turned
 ** into uppercase
 ** @param thisstr The string for which to perform the lowercase to
 **                uppercase conversion
 ** @see rf_string_to_lower()
 **/
i_DECLIMEX_ void rf_string_to_upper(void* thisstr);

/**
 ** @brief Tokenizes the given string
 **
 ** @isinherited{StringX}
 ** Separates it into @c tokensN depending on how many substrings
 ** can be created from the @c sep separator and stores them
 ** into the Array of RFstring* that should be passed to the
 ** function. The array gets initialized inside the function and
 ** <b>has to be freed explicitly</b> later by thg user.
 ** Also each String inside the array has to be Deinitialized too.
 ** Here is an example usage:
 ** @snippet Strings/tokenize.cpp Tokenize_C
 ** @lmsFunction
 ** @param[in] thisstr    The string to tokenize.
 **                       @inhtype{String,StringX}
 ** @param[in] sep        A substring that will be used as a separator
 **                       @inhtype{String,StringX}
 ** @param[out] tokensN   The number of tokens that got created
 ** @param[out] tokens    Pass a pointer to an array of RF_Strings. @keepptr
 ** @return Returns @c true in success and @c false in case the the
 ** separating character has not been found
 **/
i_DECLIMEX_ bool rf_string_tokenize(const void* thisstr, const void* sep,
                                    uint32_t* tokensN, struct RFstring** tokens);

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
