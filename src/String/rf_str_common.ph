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
#ifndef RF_STRING_COMMON_PH
#define RF_STRING_COMMON_PH


/*------------- Module related inclusion -------------*/
#include <String/rf_str_retrieval.h> //for String accessors
/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for bool
#include <Definitions/inline.h> //for inline
#include "../Internal/internal_mod.ph" //for the internal buffer
/*------------- libc inclusion --------------*/
#include <stdio.h> //for vsnprintf
#include <string.h> //for vsnprintf
/*------------- End of includes -------------*/


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
 ** Two macros to accomplish iteration of an RFstring starting from
 ** the beginning of the String. This macro should be used with its end pair.
 ** We take advantage of the fact that an RFstring is always guaranteed
 ** to contain a valid UTF-8 sequence and thus no checks are performed.
 ** @param i_string_ The string to iterate
 ** @param i_char_ A variable to hold the current character position of the iteration
 ** @param i_index_ A variable to hold the current byte position of the iteration
 **
 **/
#define RF_STRING_ITERATE_START(i_string_, i_char_, i_index_)         \
    i_index_ = 0;i_char_ = 0;                                         \
    while( i_index_ < rf_string_length_bytes(i_string_)){                \
    if(!rf_utf8_is_continuation_byte(                                    \
       rf_string_data(i_string_)[(i_index_)])){

#define RF_STRING_ITERATE_END(i_char_, i_index_)  (i_char_)++;}(i_index_)++;}

/**
 ** Two macros to accomplish iteration of an RFstring starting from the end
 ** of the String. This macro should be used with its end pair.
 ** We take advantage of the fact that an RFstring is always guaranteed
 ** to contain a valid UTF-8 sequence and thus no checks are performed
 ** @param i_string_ The string to iterate
 ** @param i_char_ An unsigned variable to hold the current character
 **        position of the iteration.
 ** @param i_index_ An uint32_t variable to hold the current byte
 **        position of the iteration
 **
 **/
#define RF_STRING_ITERATEB_START(i_string_, i_char_, i_index_)     \
    i_index_ = rf_string_length_bytes(i_string_) - 1;(i_char_)=1;     \
    do{                                                            \
    if(!rf_utf8_is_continuation_byte(                                 \
           rf_string_data(i_string_)[(i_index_)])){

#define RF_STRING_ITERATEB_END(i_char_, i_index_)       \
    (i_char_)++;}(i_index_)--;}while((i_index_) != 0);


/**
 ** A macro to easily reallocate a StringX's buffer using RF_REALOC
 ** @param STR_         The StringX whose buffer to reallocate
 ** @param REQSIZE_     The new required size the buffer should have
 ** @param RETVALUE_    What to return in realloc failure
 **
 ** @note No need to take the bIndex into account when giving the REQSIZE_
 ** the macro takes care of it.
 **/
#define RF_STRINGX_REALLOC(STR_, REQSIZE_, RETVALUE_)                   \
    do{                                                                 \
        /*If the buffer size is not enough for the required size*/      \
        if(REQSIZE_ + (STR_)->bIndex >= (STR_)->bSize)                  \
        {                                                               \
            /*create the new size*/                                     \
            (STR_)->bSize = (REQSIZE_ + (STR_)->bIndex) * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER; \
            /*Reallocate the buffer depending on whether its internal pointer has a value or not*/ \
            if(STR_->bIndex == 0)                                       \
            {                                                           \
                RF_REALLOC(rf_string_data(STR_), char,                   \
                           (STR_)->bSize, RETVALUE_);                     \
            }                                                           \
            else                                                        \
            {                                                           \
                rf_string_data(STR_) -= (STR_)->bIndex;                  \
                RF_REALLOC(rf_string_data(STR_), char,                   \
                           (STR_)->bSize, RETVALUE_);                     \
                rf_string_data(STR_) += (STR_)->bIndex;                    \
            }                                                           \
        }}while(0)

/**
 ** A macro to easily reallocate a StringX's buffer using RF_REALLOC_JMP
 ** @param STR_         The StringX whose buffer to reallocate
 ** @param REQSIZE_     The new required size the buffer should have
 ** @param STMT_    What to return in realloc failure
 ** @param GOTOFLAG_ Where to jump to in realloc failure
 **
 ** @note No need to take the bIndex into account when giving the REQSIZE_
 ** the macro takes care of it.
 **/
#define RF_STRINGX_REALLOC_JMP(STR_, REQSIZE_, STMT_, GOTOFLAG_)        \
    do{                                                                 \
        /*If the buffer size is not enough for the required size*/      \
        if(REQSIZE_ + (STR_)->bIndex >= (STR_)->bSize)                  \
        {                                                               \
            /*create the new size*/                                     \
            (STR_)->bSize = (REQSIZE_+ (STR_)->bIndex) * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER; \
            /*Reallocate the buffer depending on whether its internal pointer has a value or not*/ \
            if((STR_)->bIndex == 0)                                       \
                RF_REALLOC_JMP(rf_string_data(STR_), char,               \
                               (STR_)->bSize, STMT_, GOTOFLAG_);          \
                else                                                    \
                {                                                       \
                    rf_string_data(STR_) -= (STR_)->bIndex;              \
                    RF_REALLOC_JMP(rf_string_data(STR_), char,           \
                                   (STR_)->bSize, STMT_, GOTOFLAG_);      \
                    rf_string_data(STR_) += (STR_)->bIndex;                \
                }                                                       \
        }}while(0)


/**
 ** @internal
 ** Searches a string for a substring
 ** when both are not null terminated
 ** @endinternal
 **/
char* strstr_nnt(const char* s1, unsigned int s1_len,
                 const char* s2, unsigned int s2_len);

/**
 ** @internal
 ** Compares two non null terminated strings
 ** @endinternal
**/
bool strcmp_nnt(char* s1, unsigned int s1_len,
                char* s2, unsigned int s2_len);

/**
 ** @internal
 ** @memberof RFstring
 ** @brief Internal version of rf_string_find, used for byte position.
 **
 ** @isinherited{StringX}
 ** Finds the existence of String sstr inside this string with the given
 ** options. You have the
 ** option to either match case or perform a case-insensitive search.
 ** In addition you can search for the exact string and not it just being a
 ** part of another string. This is an internal version
 ** used by other functions to obtain the byte position instead of the
 ** character position.
 ** @param thisstr This string we want to search in @inhtype{String,StringX}
 ** @param sstr The substring string we want to search for @inhtype{String,StringX}
 ** @param options Bitflag options denoting some options for the search.
 ** Give 0 for the defaults. Can have values:
 ** + @c RF_CASE_IGNORE: If you want the found substring to
 **      ignore the case and returns success for any occurence of the 
 **      string in any case.
 **      Default search option is to @b match the case.
 **      For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you want the found substring to be exact.
 **      For example an exact search for @e "HELLO" in the string
 **      @e "HELLOWORLD" would return a failure. Default search is
 **      to return any found substring.
 ** @return Returns the byte position of the found substring or 
 ** RF_FAILURE for not found
 ** @endinternal
 **
 **/
int rf_string_find_byte_pos(const void* thisstr, const void* sstr,
                            const char options);


/**
 * Reads the formatted string and the va_args and fills in
 * the a buffer returning the string's size in bytes
 **
 ** @param fmt[in]         The formatted string
 ** @param size[out]       The string's byte size
 ** @param buffPtr[out]    The pointer to the beginning of the String
 **                        in the internal buffer
 ** @param bIndex[out]     The index to return the buffer to after we
 **                        are done with it
 ** @return                Returns @c true in success and @c false in failure
 */
i_INLINE_DECL bool fill_fmt_buffer(const char *fmt,
                                   unsigned int *size,
                                   char **buffPtr,
                                   unsigned int *bIndex,
                                   va_list args)
{
    int ret;
    size_t n = rf_buffer_size(TSBUFFA);
    *bIndex = rf_buffer_index(TSBUFFA);
    *buffPtr = rf_buffer_ptr(TSBUFFA);
    ret = vsnprintf(rf_buffer_ptr(TSBUFFA), n, fmt, args);
    if(ret < 0)
    {
        return false;
    }
    if(ret >= n)
    {
        ret = vsnprintf(rf_buffer_ptr(TSBUFFA), n + ret, fmt, args);
        if(ret < 0 || ret >= n)
        {
            return false;
        }
    }
    TSBUFFA->index += ret;
    *size = ret;
    return true;
}


i_INLINE_DECL void rf_string_generic_append(void *thisstr, const char* other,
                                            unsigned int bytes_to_copy)
{
    //add the string to this one
    memcpy(
        rf_string_data(thisstr) + rf_string_length_bytes(thisstr),
        other,
        bytes_to_copy
   );
   rf_string_length_bytes(thisstr) += bytes_to_copy;
}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//end of include guards
