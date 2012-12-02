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
#ifndef RF_STRING_MANIPULATION_H
#define RF_STRING_MANIPULATION_H


#include <rf_setup.h> //for the macro flags
#include <String/string_decl.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/*------------------------------------------------------------------------ RF_String appending functions-------------------------------------------------------------------------------*/
//! @name Adding to a String
//! @{

//! @memberof RF_String
//! @opadd
//! @brief Appends a string to this one
//!
//! @notinherited{StringX}
//! @lmsFunction
//! @param thisstr The string to append to
//! @param other The string to add to this string. @inhtype{String,StringX} @tmpSTR
//! @see rfString_Append_i()
//! @see rfString_Append_f()
//! @see rfString_Prepend()
i_DECLIMEX_ void rfString_Append(RF_String* thisstr,const void* other);


//! @memberof RF_String
//! @opadd
//! @brief Appends an integer to the string
//!
//! @notinherited{StringX}
//! @param thisstr The string to append to
//! @param i The integer to add
//! @see rfString_Append()
//! @see rfString_Append_f()
i_DECLIMEX_ void rfString_Append_i(RF_String* thisstr,const int32_t i);

//! @memberof RF_String
//! @opadd
//! @brief Appends a float to the string
//!
//! @notinherited{StringX}
//! @param thisstr The string to append to
//! @param f The float to add
//! @see rfString_Append()
//! @see rfString_Append_i()
i_DECLIMEX_ void rfString_Append_f(RF_String* thisstr,const float f);

//! @memberof RF_String
//! @brief Prepends the parameter String to this string
//!
//! @notinherited{StringX}
//! @lmsFunction
//! @param thisstr The string to prepend to
//! @param other The string to prepend to this string. @inhtype{String,StringX} @tmpSTR
//! @see rfString_Append()
i_DECLIMEX_ void rfString_Prepend(RF_String* thisstr,const void* other);


//! @}

/*------------------------------------------------------------------------ RF_String Removal functions-------------------------------------------------------------------------------*/
//! @name Removing from a String
//! @{

//! @memberof RF_String
//! @brief Removes occurences of a substring
//!
//! @isinherited{StringX}
//! Removes a @c number of occurences of a substring from this string, that agree with the given parameters.
//! <b>Does not</b> reallocate buffer size
//! @lmsFunction
//! @param thisstr This string we want to remove from. @inhtype{String,StringX}
//! @param rstr The string whose occurences we need to locate and remove from the current string. @inhtype{String,StringX} @tmpSTR
//! @param number \rfoptional{0}. The number of occurences to remove. Give @e 0 for all the occurences.
//! If the given number is bigger than the actual number of occurences, still all occurences get replaced.
//! @param options \rfoptional{0}. Bitflag options denoting some options for the search. Give 0 for the defaults.
//! Can have values:
//! + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
//!     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
//! @return Returns true in case of success, and false if the substring was not even found inside the string
//! @see rfString_Append()
//! @see rfString_Trim()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_Remove(void* thisstr,const void* rstr,uint32_t number,const char options);
#else
i_DECLIMEX_ char i_rfString_Remove(void* thisstr,const void* rstr,uint32_t number,const char options);
#define rfString_Remove(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_REMOVE,4,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_REMOVE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Remove() accepts from 2 to 4 arguments\"")
#define i_NPSELECT_RF_STRING_REMOVE0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_REMOVE,__VA_ARGS__)
#define i_SELECT_RF_STRING_REMOVE4(i_THISSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_) \
    i_rfString_Remove(i_THISSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_)
#define i_SELECT_RF_STRING_REMOVE3(i_THISSTR_,i_REPSTR_,i_NUMBER_) i_rfString_Remove(i_THISSTR_,i_REPSTR_,i_NUMBER_,0)
#define i_SELECT_RF_STRING_REMOVE2(i_THISSTR_,i_REPSTR_) i_rfString_Remove(i_THISSTR_,i_REPSTR_,0,0)
#define i_SELECT_RF_STRING_REMOVE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Remove() accepts from 2 to 4 arguments\"")
#define i_SELECT_RF_STRING_REMOVE0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Remove() accepts from 2 to 4 arguments\"")
#endif

//! @memberof RF_String
//! @brief Removes all of the characters of the string except those specified
//!
//! This string is scanned for the existence of each characters inside the given
//! @c keepstr. Any character found there is kept in the original string. All other
//! characters are removed.
//! @isinherited{StringX}
//! @lmsFunction
//! @param thisstr The string to remove from @inhtype{String,StringX}
//! @param keepstr A string all of whose characters will be kept in @c thisstr @inhtype{String,StringX} @tmpSTR
i_DECLIMEX_ void rfString_KeepOnly(void* thisstr,const void* keepstr);


//! @memberof RF_String
//! @brief Removes the first n characters from the start of the string.
//!
//! @isinherited{StringX}
//! @param thisstr The string to prune from. @inhtype{String,StringX}
//! @param n The number of characters to remove. Must be a positive integer.
//! @return True if n characters got removed and false if there are not enough characters to remove. (in which case the string becomes empty)
//! @see rfString_PruneEnd()
//! @see rfString_PruneMiddleB()
//! @see rfString_PruneMiddleF()
i_DECLIMEX_ char rfString_PruneStart(void* thisstr,uint32_t n);

//! @memberof RF_String
//! @brief Removes the last n characters from the end of the string
//!
//! @isinherited{StringX}
//! @param thisstr The string to prune from. @inhtype{String,StringX}
//! @param n The number of characters to remove. Must be a positive integer.
//! @return True if n characters got removed and false if there are not enough characters to remove. (in which case the string becomes empty)
//! @see rfString_PruneStart()
//! @see rfString_PruneMiddleB()
//! @see rfString_PruneMiddleF()
i_DECLIMEX_ char rfString_PruneEnd(void* thisstr,uint32_t n);

//! @memberof RF_String
//! @brief Removes characters from one point of the string to another going backwards
//!
//! @isinherited{StringX}
//! Removes n characters from the position p (including the character at p) of the string counting backwards. If there is no space to do so, nothing is done and returns false.
//! @param thisstr The string to prune from. @inhtype{String,StringX}
//! @param p The position to remove the characters from. Must be a positive integer. Indexing starts from zero.
//! @param n The number of characters to remove from the position and back.Must be a positive integer.
//! @return Returns true in case of succesfull removal and false in any other case.
//! @see rfString_PruneMiddleF()
//! @see rfString_PruneStart()
//! @see rfString_PruneEnd()
i_DECLIMEX_ char rfString_PruneMiddleB(void* thisstr,uint32_t p,uint32_t n);
//! @memberof RF_String
//! @brief Removes characters from one point of the string to another going forward
//!
//! @isinherited{StringX}
//! Removes n characters from the position p (including the character at p) of the string counting forwards. If there is no space, nothing is done and returns false.
//! @param thisstr The string to prune from. @inhtype{String,StringX}
//! @param p The position to remove the characters from. Must be a positive integer. Indexing starts from zero.
//! @param n The number of characters to remove from the position and on. Must be a positive integer.
//! @return Returns true in case of succesfull removal and false in any other case.
//! @see rfString_PruneMiddleB()
//! @see rfString_PruneStart()
//! @see rfString_PruneEnd()
i_DECLIMEX_ char rfString_PruneMiddleF(void* thisstr,uint32_t p,uint32_t n);

//! @memberof RF_String
//! @brief Removes all characters of a substring only from the start of the String
//!
//! @isinherited{StringX}
//! Searches for and removes each individual character inside the @c sub substring from the
//! given String @c thisstr starting from the beginning of the String and until it finds any other character
//! @lmsFunction
//! @param thisstr The string to search in. @inhtype{String,StringX}
//! @param sub The substring to search for. @inhtype{String,StringX} @tmpSTR
//! @return Returns true for success and false if none of @c sub characters were found inside the given String
//! @see rfString_Trim()
//! @see rfString_TrimEnd()
i_DECLIMEX_ char rfString_TrimStart(void* thisstr,const void* sub);

//! @memberof RF_String
//! @brief Removes all characters of a substring starting from the end of the String
//!
//! @isinherited{StringX}
//! Searches for and removes each individual character inside the @c sub substring from the
//! given String @c thisstr starting from the end of the String and until it finds any other character
//! @lmsFunction
//! @param thisstr The string to search in. @inhtype{String,StringX}
//! @param sub The substring to search for. @inhtype{String,StringX} @tmpSTR
//! @return Returns true for success and false if none of @c sub characters were found inside the given String
//! @see rfString_Trim()
//! @see rfString_TrimStart()
i_DECLIMEX_ char rfString_TrimEnd(void* thisstr,const void* sub);

//! @memberof RF_String
//! @brief Removes all characters of a substring from both ends of the given String
//!
//! @isinherited{StringX}
//! Searches for and removes each individual character inside the @c sub substring from the
//! given String @c thisstr starting from both the beginning and the end of the String and until it finds any other character
//! @lmsFunction
//! @param thisstr The string to search in. @inhtype{String,StringX}
//! @param sub The substring to search for. @inhtype{String,StringX} @tmpSTR
//! @return Returns true for success and false if none of @c sub characters were found inside the given String
//! @see rfString_TrimStart()
//! @see rfString_TrimEnd()
i_DECLIMEX_ char rfString_Trim(void* thisstr,const void* sub);

//! @}

/*------------------------------------------------------------------------ RF_String Replacement functions-------------------------------------------------------------------------------*/
//! @name Replacing parts of a String
//! @{

//! @memberof RF_String
//! @brief Replace all occurences of a String
//!
//! @notinherited{StringX}
//! Replaces all of the specified sstr substring from the String with rstr and reallocates size, unless the new size is smaller
//! @lmsFunction
//! @param thisstr The string in which to do the replacing
//! @param sstr The string to locate and replace from the current string. @inhtype{String,StringX} @tmpSTR
//! @param rstr The string with which to replace it. @inhtype{String,StringX} @tmpSTR
//! @param number \rfoptional{0}. The number of occurences to replace. Give @e 0 for all the occurences.
//! If the given number is bigger than the actual number of occurences, still all occurences get replaced.
//! @param options \rfoptional{0}. Bitflag options denoting some options for the string to replace. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! @return Returns true in case of success, and false if the substring was not even found inside the string
//! @see rfStringX_Replace()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_Replace(RF_String* thisstr,const void* sstr,const void* rstr,const uint32_t number,const char options);
#else
i_DECLIMEX_ char i_rfString_Replace(RF_String* thisstr,const void* sstr,const void* rstr,const uint32_t number,const char options);
#define rfString_Replace(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_REPLACE,5,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_REPLACE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Replace() accepts from 3 to 5 arguments\"")
#define i_NPSELECT_RF_STRING_REPLACE0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_REPLACE,__VA_ARGS__)
#define i_SELECT_RF_STRING_REPLACE5(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_) \
    i_rfString_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_)
#define i_SELECT_RF_STRING_REPLACE4(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_) \
    i_rfString_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,0)
#define i_SELECT_RF_STRING_REPLACE3(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_) i_rfString_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,0,0)
#define i_SELECT_RF_STRING_REPLACE2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Replace() accepts from 3 to 5 arguments\"")
#define i_SELECT_RF_STRING_REPLACE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Replace() accepts from 3 to 5 arguments\"")
#define i_SELECT_RF_STRING_REPLACE0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Replace() accepts from 3 to 5 arguments\"")
#endif

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
