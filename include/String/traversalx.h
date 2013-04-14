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
** --String/traversalx.h
** This header declares the functions that traverse an RF_StringX
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/traversalx.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_TRAVERSALX_H
#define RF_STRING_TRAVERSALX_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/*-------------------------------------------------------------------------Functions to traverse an RF_StringX-------------------------------------------------------------------------------*/
//! @name Traversing an RF_StringX
//! @{

//! @memberof RF_StringX
//! @brief Moves the internal pointer right after the the first occurence of substring
//!
//! Searches the StringX for the @c sub substring and moves the internal pointer right after it
//! Optionally the string between the beginning and the end of the moving is returned in @c result parameter
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param sub The substring after which to move inside the current String. @inhtype{String,StringX} @tmpSTR
//! @param result \rfoptional{0} Pass a pointer to a @c String type to be initialized with the substring between the start of @c thisstr and the end of the moving.
//! If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument
//! If 0 nothing is returned @inhtype{String,StringX}
//! @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substrings inside the string. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
//! @return Returns the number of positions (bytes) moved or RF_FAILURE if the substring was not found in the String
//! @see rfStringX_MoveAfterv()
//! @see rfStringX_MoveAfterPair()
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_Reset()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfStringX_MoveAfter(RF_StringX* thisstr,const void* sub,void* result,const char options);
#else
i_DECLIMEX_ int32_t i_rfStringX_MoveAfter(RF_StringX* thisstr,const void* sub,void* result,const char options);
#define rfStringX_MoveAfter(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_MOVEAFTER,4,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_MOVEAFTER1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfter() accepts from 2 to 4 arguments\"")
#define i_NPSELECT_RF_STRINGX_MOVEAFTER0(...) RF_SELECT_FUNC(i_SELECT_RF_STRINGX_MOVEAFTER,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_MOVEAFTER4(i_THISSTR_,i_SUBSTR_,i_RESULT_,i_OPTIONS_) \
     i_rfStringX_MoveAfter(i_THISSTR_,i_SUBSTR_,i_RESULT_,i_OPTIONS_)
#define i_SELECT_RF_STRINGX_MOVEAFTER3(i_THISSTR_,i_SUBSTR_,i_RESULT_) \
     i_rfStringX_MoveAfter(i_THISSTR_,i_SUBSTR_,i_RESULT_,0)
#define i_SELECT_RF_STRINGX_MOVEAFTER2(i_THISSTR_,i_SUBSTR_) \
     i_rfStringX_MoveAfter(i_THISSTR_,i_SUBSTR_,0,0)
#define i_SELECT_RF_STRINGX_MOVEAFTER1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfter() accepts from 2 to 4 arguments\"")
#define i_SELECT_RF_STRINGX_MOVEAFTER0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfter() accepts from 2 to 4 arguments\"")
#endif

//! @memberof RF_StringX
//! @brief Moves the internal pointer n characters back
//!
//! It never goes before the original start of the buffer so there is no worry of memory corruption.
//! @param thisstr The extended string to work on
//! @param n The number of characters to move the internal pointer
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveAfter()
//! @see rfStringX_Reset()
i_DECLIMEX_ void rfStringX_MoveBack(RF_StringX* thisstr,uint32_t n);
//! @memberof RF_StringX
//! @brief Moves the internal pointer n characters forward
//!
//! It never goes beyond the buffer's length so there is no fear of memory corruption.
//! But since many times the buffer can be bigger than the string we can have it go to non-string buffer data so the user has to be careful.
//! @param thisstr The extended string to work on
//! @param n The number of character to move the internal pointer
//! @see rfStringX_MoveBack()
//! @see rfStringX_MoveAfter()
//! @see rfStringX_Reset()
i_DECLIMEX_ void rfStringX_MoveForward(RF_StringX* thisstr,uint32_t n);
//! @memberof RF_StringX
//! @brief Resets the internal pointer of the StringX
//!
//! @param thisstr The stringX whose internal pointer to reset
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_MoveAfter()
i_DECLIMEX_ void rfStringX_Reset(RF_StringX* thisstr);

//! @memberof RF_StringX
//! @brief Moves the internal pointer after the first occurence of any of the given substrings
//!
//! Searches for any of the given Substrings inside @c thisstr. Moves the internal pointer right after
//! the first occuring substring. Optionally and if the value given at @c result is not 0 the string between
//! the start and the end of the moving is returned. The parameters accepted can be of any string type, even temporary
//! strings initialized with the temporary string macro
//! @rfNoDefArgsWarn1
//! @warning if the library has been compiled with @c DEFAULT_ARGUMENTS off then arguments @c options and @c parN are actually pointers
//! to @c char and @c unsigned char respectively
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param result \rfoptional{0} Pass a pointer to a string type to have it initialized with the before part of the string before the found substring.
//! If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument
//! This is an optional parameter. Give 0 for nothing returned. @inhtype{String,StringX}
//! @param options Bitflag options denoting the method with which to search for the substrings inside the string. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
//! @param parN The number of parameters
//! @extraVarArgLim
//! @param ... The strings to search for. @inhtype{String,StringX} @tmpSTR
//! @return true if the substring got initialized and false if none of the parameters are found
//! @see rfStringX_MoveAfter()
//! @see rfStringX_MoveAfterPair()
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_Reset()
i_DECLIMEX_ char rfStringX_MoveAfterv(RF_StringX* thisstr,void* result,const char options,const unsigned char parN, ...);

//! @memberof RF_StringX
//! @brief Moves the internal string pointer after the substring formed by the @c left and @c right substrings
//!
//! Searches @c thisstr for the occurence of both the @c left and @c right substrings and if found moves the internal pointer right after them.
//! If the number given at @c occurence does not exist in the string the function returns @c false. Finally if @c result is not zero then what is between
//! the @c left and @c right substrings is returned inside @c result.
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param left The left substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
//! @param right The right substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
//! @param result \rfoptional{0} If this is not 0 then pass a pointer to a string type here to have it initialized with what's in between the @c left and @c right substring
//! after which the internal string pointer gets moved.
//! If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument
//! @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
//!     @e "HELLOWORLD" would find nothing. Default is with this flag off.
//! + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
//! @param occurence \rfoptional{0} If this is not 0 then the function will search for the number of occurence given in this parameter.
//! If it is 0 it will search for the first occurence. If it is not found then the function shall return false
//! @return Returns true if the substring is found and false if not
//! @see rfStringX_MoveAfter()
//! @see rfStringX_MoveAfterv()
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_Reset()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_MoveAfterPair(RF_StringX* thisstr,const void* left,const void* right,void* result,char options,uint32_t occurence);
#else
i_DECLIMEX_ char i_rfStringX_MoveAfterPair(RF_StringX* thisstr,const void* left,const void* right,void* result,char options,uint32_t occurence);
#define rfStringX_MoveAfterPair(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_MOVEAFTERPAIR,6,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_MOVEAFTERPAIR1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#define i_NPSELECT_RF_STRINGX_MOVEAFTERPAIR0(...) RF_SELECT_FUNC(i_SELECT_RF_STRINGX_MOVEAFTERPAIR,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR6(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_,i_OCCURENCE_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_,i_OCCURENCE_)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR5(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_,0)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR4(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,0,0)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR3(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,0,0,0)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#endif

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
