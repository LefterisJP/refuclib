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
#ifndef RF_STRING_MANIPULATIONX_H
#define RF_STRING_MANIPULATIONX_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstringX
#include <String/rf_str_flags.h> //for RFstring_matching_options
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


//! @name RFstringx Appending
//! @{

/**
 ** @brief Appends the parameter String to this extended string.
 **
 ** @lmsFunction
 ** @param thisstr     The extended string to append to
 ** @param other       The string to add to this string.
 **                    @inhtype{String,StringX} @tmpSTR
 ** @return            Returns @c true in success and @c false otherwise
 ** @see rf_stringx_prepend()
 **/
i_DECLIMEX_ bool rf_stringx_append(struct RFstringx* thisstr, const void* other);

/**
 ** @brief Appends a number of characters from the parameter String
 ** to this extended string.
 **
 ** If the number of characters given at @c chars is greater than the actual
 ** character length of the @c other String then all of the string is appended.
 ** @lmsFunction
 ** @param thisstr      The extended string to append to
 ** @param other        The string to add to this string.
 **                     @inhtype{String,StringX} @tmpSTR
 ** @param chars        The number of chars of @c other to append to @c thisstr
 ** @return             Returns @c true in success and @c false otherwise
 ** @see rf_stringx_prepend()
 **/
i_DECLIMEX_ bool rf_stringx_append_chars(struct RFstringx* thisstr,
                                        const void* other,
                                        int chars);

/**
 ** @brief Appends a single unicode character to this string
 **
 ** @param thisstr      The extended string to append to
 ** @param unichar      The character to append to this string.
 **                     It should be a unicode codepoint (UTF-32)
 ** @return             Returns @c true in success and @c false otherwise
 ** @see rf_stringx_prepend()
 **/
i_DECLIMEX_ bool rf_stringx_append_char(struct RFstringx* thisstr, uint32_t unichar);

/**
 ** @brief Prepends the parameter String to this extended string
 **
 ** @lmsFunction
 ** @param thisstr        The extended string to prepend to
 ** @param other          The string to prepend to this string.
 **                       @inhtype{String,StringX} @tmpSTR
 ** @return               Returns @c true in success and @c false otherwise
 ** @see rf_stringx_append()
 **/
i_DECLIMEX_ bool rf_stringx_prepend(struct RFstringx* thisstr, const void* other);



/**
 ** @brief Inserts a string to this extended string at the
 ** parameter character position
 **
 ** This function takes the given string @c thisstr and finds its @c pos
 ** character. Then it inserts the @c other string in the position of that
 ** character pushing the rest of the string further ahead.
 ** @lmsFunction
 ** @param thisstr     The extended string to insert to
 ** @param pos         The character position in the string to add it.
 **                    Should be a positive (or zero) integer. If the position
 **                    is over the string's size nothing happens.
 ** @param other       The string to add. @inhtype{String,StringX} @tmpSTR
 ** @return            Returns @c true in success and @c false otherwise
 ** @see rf_stringx_append()
 ** @see rf_stringx_prepend()
 **/
i_DECLIMEX_ bool rf_stringx_insert(struct RFstringx* thisstr, uint32_t pos,
                                  const void* other);

//! @}

//! @name Unsafe appending functions
//! @{

/**
 ** @brief Appends some bytes of the parameter String to this extended string.
 **
 ** @warning This is a dangerous function. No check is performed in the
 ** function to see if a byte is a continuation byte or not. It is mostly
 ** used internally and should not
 ** be utilized by the user unless you know what you are doing.
 ** @lmsFunction
 ** @param thisstr      The extended string to append to
 ** @param other        The string to add to this string.
 **                     @inhtype{String,StringX} @tmpSTR
 ** @param bytes        The number of bytes of @c other to append to @c thisstr
 ** @return             Returns @c true in success and @c false otherwise
 ** @see rf_stringx_append()
 **/
i_DECLIMEX_ bool rf_stringx_append_bytes(struct RFstringx* thisstr,
                                        const void* other,
                                        const int32_t bytes);
/**
 ** @brief Appends a cstring to this string
 **
 ** @warning This is a dangerous function. No check is performed in the
 ** function to see if the given @c cstr is a valid UTF8 buffer.
 ** For a safe way to add a UTF8 string use
 ** @ref rf_stringx_append() and enclose the @c cstr with @ref RFS_()
 ** @param thisstr      The extended string to append to
 ** @param cstr         The cstring to add to this functions
 ** @return             Returns @c true in success and @c false otherwise
 ** @see rf_stringx_append()
 **/
i_DECLIMEX_ bool rf_stringx_append_cstr(struct RFstringx* thisstr,
                                        const char* cstr);

//! @}

//! @name RFstringx Replacing
//! @{

/**
 ** @brief Replaces all occurences of a String
 ** @see rf_string_replace()
 **/
i_DECLIMEX_ bool rf_stringx_replace(struct RFstringx* thisstr, const void* sstr,
                                    const void* rstr, uint32_t number,
                                    enum RFstring_matching_options options);


/**
 ** @brief Replaces Strings between substrings for a given number of occurences
 **
 ** Replaces what exists between the ith left and right substrings of this
 ** extended String. Utilizes the internal string pointer.
 ** @lmsFunction
 ** @param thisstr     The extended string to work on
 ** @param left        The left substring that will define the new substring.
 **                    @inhtype{String,StringX} @tmpSTR
 ** @param rstr        The string to act as replacement.
 **                    @inhtype{String,StringX} @tmpSTR
 ** @param options     @see rf_string_replace()
 ** @param i           \rfoptional{0}. The specific between occurence to 
 **                    replace. Should range between 1 and infinity. If 0 all
 **                    occurences will be replaced
 ** @return            Returns true if the replacing happened and false if
 **                    either the left or the right strings were not found or
 **                    for error
 ** @see rf_stringx_replace()
 **/
i_DECLIMEX_ bool rf_stringx_replace_between(
    struct RFstringx* thisstr,
    const void* left,
    const void* right,
    const void* rstr,
    enum RFstring_matching_options options,
    uint32_t i
);


//! @}


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
