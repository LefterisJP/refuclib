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
** --String/manipulationx.h
** This header declares the functions that manipulate RF_StringX
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/manipulationx.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_MANIPULATIONX_H
#define RF_STRING_MANIPULATIONX_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/*-------------------------------------------------------------------------Functions to  append tp am RF_StringX-------------------------------------------------------------------------------*/
//! @name RF_StringX Appending
//! @{

/**
 ** @memberof RF_StringX
 ** @brief Appends the parameter String to this extended string.
 **
 ** @lmsFunction
 ** @param thisstr The extended string to append to
 ** @param other The string to add to this string. @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringsX_Prepend()
 **
 **/
i_DECLIMEX_ char rfStringX_Append(RF_StringX* thisstr, const void* other);

/**
 ** @memberof RF_StringX
 ** @brief Appends a number of characters from the parameter String to this extended string.
 **
 ** If the number of characters given at @c chars is greater than the actual character
 ** length of the @c other String then all of the string is appended.
 ** @lmsFunction
 ** @param thisstr The extended string to append to
 ** @param other The string to add to this string. @inhtype{String,StringX} @tmpSTR
 ** @param chars The number of chars of @c other to append to @c thisstr
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringsX_Prepend()
 **
 **/
i_DECLIMEX_ char rfStringX_Append_i(RF_StringX* thisstr, 
                                    const void* other, int chars);



/**
 ** @memberof RF_StringX
 ** @brief Appends a single unicode character to this string
 **
 ** @param thisstr The extended string to append to
 ** @param unichar The character to append to this string. It should be a unicode codepoint
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringsX_Prepend()
 **
 **/
i_DECLIMEX_ char rfStringX_Append_char(RF_StringX* thisstr,
                                       uint32_t unichar);
/**
 ** @memberof RF_StringX
 ** @brief Appends a single utf8 character to this string
 **
 ** @param thisstr The extended string to append to
 ** @param utf8char The character to append to this string. It should be utf8-encoded
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringsX_Prepend()
 **
 **/
i_DECLIMEX_ char rfStringX_Append_charutf8(RF_StringX* thisstr,
                                           uint32_t utf8char);

/**
 ** @memberof RF_StringX
 ** @brief Appends an integer to the extended string following some specific parameters
 **
 ** This appending attempts to follow the C standard's print formatting as specified
 ** in the <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/printf.html">documentation of printf</a>
 ** The @c width, @c precision and @c flags arguments are inspired from there
 **
 ** @param thisstr The extended string to append to
 ** @param i The integer to add
 ** @param width The minimum width that the number @c i will take up in the string. If the space taken by the number is
 ** less than @c width then it will be padded with either zeroes or spaces on either the left or the right
 ** depending on the @c flags argument
 ** @param precision Specifies the minimum number of digits to appear. If the number @c i can be represented in fewer digits
 **  it shall be expanded with leading zeros.
 ** @param flags A combination of flags to determine how the number will be appeneded to the string. Possible values are:
 ** + @c RF_FORMATFLAG_LJUSTIFY: The value shall be left-justified within the field. The conversion is right-justified if this flag is not specified.
 ** + @c RF_FORMATFLAG_SIGN: The result of a signed conversion shall always begin with a sign ( '+' or '-' ).
 **   The conversion shall begin with a sign only when a negative value is converted if this flag is not specified.
 ** + @c RF_FORMATFLAG_NOSIGN: If no sign is going to be written, a blank space is inserted before the value.
 ** + @c RF_FORMATFLAG_PADZEROES: If this flag is specified and the given width if bigger than the value then it will be
 **   padded with 0s instead of empty spaces.
  ** @return Returns @c true in success and @c false otherwise
 **
 **/
i_DECLIMEX_ char rfStringX_Append_int(RF_StringX* thisstr, int64_t i,
                                      int width, int precision,
                                      char flags);
/**
 ** @memberof RF_StringX
 ** @brief Appends an unsigned integer to the extended string following some specific parameters
 **
 ** This appending attempts to follow the C standard's print formatting as specified
 ** in the <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/printf.html">documentation of printf</a>
 ** The @c width, @c precision and @c flags arguments are inspired from there
 **
 ** @param thisstr The extended string to append to
 ** @param i The integer to add
 ** @param width The minimum width that the number @c i will take up in the string. If the space taken by the number is
 ** less than @c width then it will be padded with either zeroes or spaces on either the left or the right
 ** depending on the @c flags argument
 ** @param precision Specifies the minimum number of digits to appear. If the number @c i can be represented in fewer digits
 **  it shall be expanded with leading zeros.
 ** @param flags A combination of flags to determine how the number will be appeneded to the string. Possible values are:
 ** + @c RF_FORMATFLAG_LJUSTIFY: The value shall be left-justified within the field. The conversion is right-justified if this flag is not specified.
 ** + @c RF_FORMATFLAG_SIGN: The result of a signed conversion shall always begin with a sign ( '+' or '-' ).
 **   The conversion shall begin with a sign only when a negative value is converted if this flag is not specified.
 ** + @c RF_FORMATFLAG_NOSIGN: If no sign is going to be written, a blank space is inserted before the value.
 ** + @c RF_FORMATFLAG_PADZEROES: If this flag is specified and the given width if bigger than the value then it will be
 **   padded with 0s instead of empty spaces.
 ** @return Returns @c true in success and @c false otherwise
 **/
i_DECLIMEX_ char rfStringX_Append_uint(RF_StringX* thisstr,
                                       uint64_t i, int width,
                                       int precision, char flags);

/**
 ** @memberof RF_StringX
 ** @brief Appends an unsigned int as a Hexadecimal value to the extended string following some specific parameters
 **
 ** This appending attempts to follow the C standard's print formatting as specified
 ** in the <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/printf.html">documentation of printf</a>
 ** The @c width, @c precision and @c flags arguments are inspired from there
 **
 ** @param thisstr The extended string to append to
 ** @param i The integer to add as a hexadecimal value
 ** @param width The minimum width that the number @c i will take up in the string. If the space taken by the number is
 ** less than @c width then it will be padded with either zeroes or spaces on either the left or the right
 ** depending on the @c flags argument
 ** @param precision Specifies the minimum number of digits to appear. If the number @c i can be represented in fewer digits
 **  it shall be expanded with leading zeros.
 ** @param flags A combination of flags to determine how the number will be appeneded to the string. Possible values are:
 ** + @c RF_FORMATFLAG_LJUSTIFY: The value shall be left-justified within the field. The conversion is right-justified if this flag is not specified.
 ** + @c RF_FORMATFLAG_SIGN: The result of a signed conversion shall always begin with a sign ( '+' or '-' ).
 **   The conversion shall begin with a sign only when a negative value is converted if this flag is not specified.
 ** + @c RF_FORMATFLAG_NOSIGN: If no sign is going to be written, a blank space is inserted before the value.
 ** + @c RF_FORMATFLAG_PADZEROES: If this flag is specified and the given width if bigger than the value then it will be
 **   padded with 0s instead of empty spaces.
 ** + @c RF_FORMATFLAG_UPPER: If this flag is specified then uppercase letter are used to represent the hexadecimals
  ** @return Returns @c true in success and @c false otherwise
 **/
i_DECLIMEX_ char rfStringX_Append_hex(RF_StringX* thisstr, uint64_t i,
                                      int width, int precision,
                                      char flags);

/**
 ** @memberof RF_StringX
 ** @brief Appends a float or double value to the extended string following some specific parameters
 **
 ** This appending attempts to follow the C standard's print formatting as specified
 ** in the <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/printf.html">documentation of printf</a>
 ** The @c width, @c precision and @c flags arguments are inspired from there
 **
 ** @param thisstr The extended string to append to
 ** @param f The floating point value to add
 ** @param width The minimum width that the number @c f will take up in the string. If the space taken by the number is
 ** less than @c width then it will be padded with either zeroes or spaces on either the left or the right
 ** depending on the @c flags argument
 ** @param precision Specifies the minimum number of digits to appear @b after the decima point.
 ** @param flags A combination of flags to determine how the number will be appeneded to the string. Possible values are:
 ** + @c RF_FORMATFLAG_LJUSTIFY: The value shall be left-justified within the field. The conversion is right-justified if this flag is not specified.
 ** + @c RF_FORMATFLAG_SIGN: The result of a signed conversion shall always begin with a sign ( '+' or '-' ).
 **   The conversion shall begin with a sign only when a negative value is converted if this flag is not specified.
 ** + @c RF_FORMATFLAG_NOSIGN: If no sign is going to be written, a blank space is inserted before the value.
 ** + @c RF_FORMATFLAG_PADZEROES: If this flag is specified and the given width if bigger than the value then it will be
 **   padded with 0s instead of empty spaces.
 ** + @c RF_FORMATFLAG_UPPER: If this flag is specified then uppercase letter are used to represent the hexadecimals
 ** @return Returns @c true in success and @c false otherwise
 **/
i_DECLIMEX_ char rfStringX_Append_double(RF_StringX* thisstr, double f,
                                         int width, int precision,
                                         char flags);
/**
 ** @memberof RF_StringX
 ** @brief Appends a float or double value to the extended string in scientific notation following some specific parameters
 **
 ** This appending attempts to follow the C standard's print formatting as specified
 ** in the <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/printf.html">documentation of printf</a>
 ** The @c width, @c precision and @c flags arguments are inspired from there
 **
 ** @param thisstr The extended string to append to
 ** @param f The floating point value to add
 ** @param width The minimum width that the number @c f will take up in the string. If the space taken by the number is
 ** less than @c width then it will be padded with either zeroes or spaces on either the left or the right
 ** depending on the @c flags argument
 ** @param precision Specifies the minimum number of digits to appear @b after the decima point.
 ** @param flags A combination of flags to determine how the number will be appeneded to the string. Possible values are:
 ** + @c RF_FORMATFLAG_LJUSTIFY: The value shall be left-justified within the field. The conversion is right-justified if this flag is not specified.
 ** + @c RF_FORMATFLAG_SIGN: The result of a signed conversion shall always begin with a sign ( '+' or '-' ).
 **   The conversion shall begin with a sign only when a negative value is converted if this flag is not specified.
 ** + @c RF_FORMATFLAG_NOSIGN: If no sign is going to be written, a blank space is inserted before the value.
 ** + @c RF_FORMATFLAG_PADZEROES: If this flag is specified and the given width if bigger than the value then it will be
 **   padded with 0s instead of empty spaces.
 ** + @c RF_FORMATFLAG_UPPER: If this flag is specified then uppercase letter are used to represent the hexadecimals
 ** @return Returns @c true in success and @c false otherwise
 **/
i_DECLIMEX_ char rfStringX_Append_double_scientific(RF_StringX* thisstr,
                                                    double f, int width,
                                                    int precision,char flags);

/**
 ** @memberof RF_StringX
 ** @brief Prepends the parameter String to this extended string
 **
 ** @lmsFunction
 ** @param thisstr The extended string to prepend to
 ** @param other The string to prepend to this string. @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringX_Append()
 **
 **/
i_DECLIMEX_ char rfStringX_Prepend(RF_StringX* thisstr, const void* other);



/**
 ** @memberof RF_StringX
 ** @brief Inserts a string to this extended string at the parameter character position
 **
 ** This function takes the given string @c thisstr and finds its @c pos character.
 ** Then it inserts the @c other string in the position of that character pushing all
 ** the rest of the string further ahead.
 ** @lmsFunction
 ** @param thisstr The extended string to insert to
 ** @param pos     The character position in the string to add it. Should be a positive (or zero) integer. If the position is over the string's size nothing happens.
 ** @param other   The string to add. @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringX_Append()
 ** @see rfStringX_Prepend()
 **
 **/
i_DECLIMEX_ char rfStringX_Insert(RF_StringX* thisstr, uint32_t pos,
                                  const void* other);

//! @}
/*------------------------------------------------------------------------ RF_StringX unsafe appending  functions-------------------------------------------------------------------------------*/
//! @name Unsafe appending functions
//! @{

/**
 ** @memberof RF_StringX
 ** @brief Appends some bytes of the parameter String to this extended string.
 **
 ** @warning This is a dangerous function. No check is performed in the function to see if
 ** byte is a continuation byte or not. It is mostly used internally and should not
 ** be utilized by the user unless you know what you are doing.
 ** @lmsFunction
 ** @param thisstr The extended string to append to
 ** @param other The string to add to this string. @inhtype{String,StringX} @tmpSTR
 ** @param bytes The number of bytes of @c other to append to @c thisstr
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringX_Append()
 **
 **/
i_DECLIMEX_ char rfStringX_Append_bytes(RF_StringX* thisstr,
                                        const void* other,
                                        const int32_t bytes);
/**
 ** @memberof RF_StringX
 ** @brief Appends a cstring to this string
 **
 ** @warning This is a dangerous function. No check is performed in the function to see
 ** if the given @c cstr is a valid UTF8 buffer. For a safe way to add a UTF8 string use
 ** @ref rfStringX_Append() and enclose the @c cstr with @ref RFS_()
 ** @param thisstr The extended string to append to
 ** @param cstr The cstring to add to this functions
 ** @return Returns @c true in success and @c false otherwise
 ** @see rfStringX_Append()
 **
 **/
i_DECLIMEX_ char rfStringX_Append_cstr(RF_StringX* thisstr,
                                       const char* cstr);

//! @}
/*------------------------------------------------------------------------ RF_StringX replacing functions-------------------------------------------------------------------------------*/
//! @name RF_StringX Replacing
//! @{

/**
 ** @memberof RF_StringX
 ** @brief Replaces all occurences of a String
 **
 ** Replaces all of the specified @c sstr substring from the extended String with the string @c rstr
 ** @lmsFunction
 ** @param thisstr The extended string in which to do the replacing
 ** @param sstr The string to locate and replace from the current string. @inhtype{String,StringX} @tmpSTR
 ** @param rstr The string with which to replace it. @inhtype{String,StringX} @tmpSTR
 ** @param number \rfoptional{0}. The number of occurences to replace. Give @e 0 for all the occurences.
 ** If the given number is bigger than the actual number of occurences, still all occurences get replaced.
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the string to replace. Give 0 for the defaults. Can have values:
 ** + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
 **     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
 ** @return Returns true in case of success, and false if the substring was not even found inside the string or for other error
 ** @see rfStringX_ReplaceBetween()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_Replace(RF_StringX* thisstr,const void* sstr,const void* rstr,uint32_t number,char options);
#else
i_DECLIMEX_ char i_rfStringX_Replace(RF_StringX* thisstr,const void* sstr,const void* rstr,uint32_t number,const char options);
#define rfStringX_Replace(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_REPLACE,5,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_REPLACE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#define i_NPSELECT_RF_STRINGX_REPLACE0(...) RF_SELECT_FUNC(i_SELECT_RF_STRINGX_REPLACE,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_REPLACE5(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_) \
   i_rfStringX_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_)
#define i_SELECT_RF_STRINGX_REPLACE4(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_) \
    i_rfStringX_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,0)
#define i_SELECT_RF_STRINGX_REPLACE3(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_) i_rfStringX_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,0,0)
#define i_SELECT_RF_STRINGX_REPLACE2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACE0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#endif

/**
 ** @memberof RF_StringX
 ** @brief Replaces Strings between substrings for a given number of occurences
 **
 ** Replaces what exists between the ith left and right substrings of this extended String. Utilizes the internal string pointer.
 ** @lmsFunction
 ** @param thisstr The extended string to work on
 ** @param left The left substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
 ** @param rstr The string to act as replacement. @inhtype{String,StringX} @tmpSTR
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the string to replace. Give 0 for the defaults. Can have values:
 ** + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
 **     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
 ** @param i \rfoptional{0}. The specific between occurence to replace. Should range between 1 and infinity. If 0 all occurences will be replaced
 ** @return Returns true if the replacing happened and false if either the left or the right strings were not found or for error
 ** @see rfStringX_Replace()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_ReplaceBetween(RF_StringX* thisstr,const void* left,const void* right,const void* rstr,char options,uint32_t i);
#else
i_DECLIMEX_ char i_rfStringX_ReplaceBetween(RF_StringX* thisstr,const void* left,const void* right,const void* rstr,char options,uint32_t i);
#define rfStringX_ReplaceBetween(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_REPLACEBETWEEN,6,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_REPLACEBETWEEN1(...)  RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_NPSELECT_RF_STRINGX_REPLACEBETWEEN0(...)  RF_SELECT_FUNC(i_SELECT_RF_STRINGX_REPLACEBETWEEN,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN6(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_,i_OCCURENCE_)  \
    i_rfStringX_ReplaceBetween(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_,i_OCCURENCE_)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN5(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_)  \
    i_rfStringX_Between(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_,0)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN4(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_)  \
    i_rfStringX_ReplaceBetween(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,0,0)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN3(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#endif

//! @}


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
