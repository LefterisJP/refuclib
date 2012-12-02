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
**
**
**
** -- String/common.ph
** This is the private header file of the C RF_String
** containing functions and macros that don't need to be exposed
** to the user
*/
#ifndef RF_STRING_COMMON_PH
#define RF_STRING_COMMON_PH

#include <rf_setup.h> //for int32_t declaration

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

    /********************* ==Internal Use Only== String Iteration (For API use iteration refer to rfString_Iterate() group of macros *************************************/

//! Two macros to accomplish iteration of an RF_String starting from the beginning of the String. This macro should be used with its end pair.
//! We take advantage of the fact that an RF_String is always guaranteed to contain a valid UTF-8 sequence and thus no checks are performed.
//! @param i_string_ The string to iterate
//! @param i_char_ A variable to hold the current character position of the iteration
//! @param i_index_ A variable to hold the current byte position of the iteration
#define RF_STRING_ITERATE_START(i_string_,i_char_,i_index_)     \
            i_index_ = 0;i_char_=0;\
            while( ((RF_String*)(i_string_))->bytes[(i_index_)] != '\0'){\
                if( rfUTF8_IsContinuationByte( ((RF_String*)(i_string_))->bytes[(i_index_)]) ==false){

#define RF_STRING_ITERATE_END(i_char_,i_index_)  (i_char_)++;}(i_index_)++;}

//! Two macros to accomplish iteration of an RF_String starting from the end of the String. This macro should be used with its end pair.
//! We take advantage of the fact that an RF_String is always guaranteed to contain a valid UTF-8 sequence and thus no checks are performed
//! @param i_string_ The string to iterate
//! @param i_char_ An unsigned variable to hold the current character position of the iteration.
//! @param i_index_ An uint32_t variable to hold the current byte position of the iteration
#define RF_STRING_ITERATEB_START(i_string_,i_char_,i_index_)     \
            i_index_ = ((RF_String*)(i_string_))->byteLength-1;(i_char_)=1;\
            do{\
               if( rfUTF8_IsContinuationByte(((RF_String*)(i_string_))->bytes[(i_index_)]) ==false){

#define RF_STRING_ITERATEB_END(i_char_,i_index_)  (i_char_)++;}(i_index_)--;}while((i_index_) != 0);


//! A macro to easily reallocate a StringX's buffer. Should make the code clearer, BUT is not to be used in every occasion
#define RF_STRINGX_REALLOC(STR_,REQSIZE_)    {\
/*If the buffer size is not enough for the requires size*/\
if(REQSIZE_ >= STR_->bSize)\
{\
    /*create the new size*/\
    STR_->bSize = REQSIZE_*RF_OPTION_STRINGX_CAPACITY_M;\
    /*Reallocate the buffer depending on whether its internal pointer has a value or not*/\
    if(STR_->bIndex==0)\
        RF_REALLOC(STR_->INH_String.bytes,char,STR_->bSize)\
    else\
    {\
        STR_->INH_String.bytes-=STR_->bIndex;\
        RF_REALLOC(STR_->INH_String.bytes,char,STR_->bSize)\
        STR_->INH_String.bytes+=STR_->bIndex;\
    }\
}}

//! @internal
//! @memberof RF_String
//! @brief Internal version of rfString_Find, used for byte position.
//!
//! @isinherited{StringX}
//! Finds the existence of String sstr inside this string with the given options. You have the
//! option to either match case or perform a case-insensitive search. In addition you can search
//! for the exact string and not it just being a part of another string. This is an internal version
//! used by other functions to obtain the byte position instead of the character position.
//! @param thisstr This string we want to search in @inhtype{String,StringX}
//! @param sstr The substring string we want to search for @inhtype{String,StringX}
//! @param options Bitflag options denoting some options for the search. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
//!     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
//! @return Returns the byte position of the found substring or RF_FAILURE for not found
//! @endinternal
int32_t rfString_FindBytePos(const void* thisstr,const void* sstr,char options);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//end of include guards
