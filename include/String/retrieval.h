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
** --String/retrieval.h
** This header declares the functions that retrieve things from Strings
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/retrieval.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_RETRIEVAL_H
#define RF_STRING_RETRIEVAL_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/*------------------------------------------------------------------------ RF_String accessor functions-------------------------------------------------------------------------------*/
//! @name String accessors
//! @{

/**
 ** @memberof RF_String
 ** @brief Finds the length of the string in characters.
 **
 ** @isinherited{StringX}
 ** @param s The string whose number of characters to find. @inhtype{String,StringX}
 ** @return Returns the length of the sting in characters, not including the null termintion character
 **
 **/
i_DECLIMEX_ uint32_t rfString_Length(const void * s);


// Denotes that a requested character/byte index in an RF_String is out of bounds
#define RF_STRING_INDEX_OUT_OF_BOUNDS   ((uint32_t)0xFF0FFFF)
/**
 ** @memberof RF_String
 ** @brief Retrieves the unicode code point of the parameter character.
 **
 ** @isinherited{StringX}
 ** If the character position is out of bounds RF_STRING_INDEX_OUT_OF_BOUNDS is returned.
 ** @param thisstr The string whose character code point we need. @inhtype{String,StringX}
 ** @param c The character index whose unicode code point to return. Must be a positive (including zero) integer.
 ** @return Returns the code point as an uint32_t or the value RF_STRING_INDEX_OUT_OF_BOUNDS if the requested character index is out of bounds
 **
 **/
i_DECLIMEX_ uint32_t rfString_GetChar(const void* thisstr,uint32_t c);

//! @}

/*------------------------------------------------------------------------ RF_String retrieval functions-------------------------------------------------------------------------------*/
//! @name String Retrieval
//! @{

/**
 ** @memberof RF_String
 ** @brief Returns a substring of this string
 **
 ** @isinherited{StringX}
 ** Returns the substring of @c thisstr starting from @c startPos and for @c charsN characters
 ** It is returned inside @c ret.
 ** @param thisstr Te string whose substring we want @inhtype{String,StringX}
 ** @param startPos The starting character where the substring will begin from
 ** @param charsN The number of characters the substring will have. If they exceed the end of the string then,
 ** a substring only until the end of the string shall be returned.
 ** @param ret Pass a reference to an RF_String here to receive the resulting substring.
 ** @return Returns @c true if a substring exists and @c false otherwise
 ** @see rfString_Between()
 ** @see rfString_After()
 ** @see rfString_Before()
 **
 **/
i_DECLIMEX_ char rfString_Substr(const void* thisstr,uint32_t startPos,uint32_t charsN,RF_String* ret);

/**
 ** @memberof RF_String
 ** @brief Finds if a substring exists inside another string.
 **
 ** @isinherited{StringX}
 ** Finds the existence of String sstr inside this string with the given options. You have the
 ** option to either match case or perform a case-insensitive search. In addition you can search
 ** for the exact string and not it just being a part of another string.
 ** @lmsFunction
 ** @param thisstr This string we want to search in @inhtype{String,StringX}
 ** @param sstr The substring string we want to search for @inhtype{String,StringX} @tmpSTR
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the search.Can have values:
 ** + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
 ** @return Returns the character position of the found substring or RF_FAILURE for not found
 ** @see rfString_Find_i()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfString_Find(const void* thisstr,const void* sstr,const char options);
#else
i_DECLIMEX_ int32_t i_rfString_Find(const void* thisstr,const void* sstr,const char options);
#define rfString_Find(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_FIND,3,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_FIND1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Find() accepts from 2 to 3 arguments\"")
#define i_NPSELECT_RF_STRING_FIND0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_FIND,__VA_ARGS__)
#define i_SELECT_RF_STRING_FIND3(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) i_rfString_Find(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_)
#define i_SELECT_RF_STRING_FIND2(i_THISSTR_,i_SEARCHSTR_) i_rfString_Find(i_THISSTR_,i_SEARCHSTR_,0)
#define i_SELECT_RF_STRING_FIND1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Find() accepts from 2 to 3 arguments\"")
#define i_SELECT_RF_STRING_FIND0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Find() accepts from 2 to 3 arguments\"")
#endif


/**
 ** @memberof RF_String
 ** @brief Finds if a substring exists inside a specific part of another string.
 **
 ** @isinherited{StringX}
 ** Finds the existence of String @c sstr inside a specific part of this string with the given options. You have the
 ** option to either match case or perform a case-insensitive search. In addition you can search
 ** for the exact string and not it just being a part of another string.
 ** @lmsFunction
 ** @param thisstr This string we want to search in @inhtype{String,StringX}
 ** @param sstr The substring string we want to search for @inhtype{String,StringX} @tmpSTR
 ** @param startPos The starting character position of @c thisstr from which the search will begin for @c sstr
 ** @param length The character length of @c thisstr insde which you need to search for the substr @c sstr
 ** If the combination of @c startPos and @c length exceed the end of the string then the string shall only be
 ** searched until the end.
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the search.Can have values:
 ** + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
 ** @return Returns the character position of the found substring inside the original string or RF_FAILURE for not found
 ** @see rfString_Find()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfString_Find_i(const void* thisstr,const void* sstr,uint32_t startPos,uint32_t length,const char options);
#else
i_DECLIMEX_ int32_t i_rfString_Find_i(const void* thisstr,const void* sstr,uint32_t startPos,uint32_t length,const char options);
#define rfString_Find_i(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_FIND_I,5,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_FIND_I1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Find_i() accepts from 4 to 5 arguments\"")
#define i_NPSELECT_RF_STRING_FIND_I0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_FIND_I,__VA_ARGS__)
#define i_SELECT_RF_STRING_FIND_I5(i_THISSTR_,i_SEARCHSTR_,i_STARTPOS_,i_LENGTH_,i_OPTIONS_) i_rfString_Find_i(i_THISSTR_,i_SEARCHSTR_,i_STARTPOS_,i_LENGTH_,i_OPTIONS_)
#define i_SELECT_RF_STRING_FIND_I4(i_THISSTR_,i_SEARCHSTR_,i_STARTPOS_,i_LENGTH_)            i_rfString_Find_i(i_THISSTR_,i_SEARCHSTR_,i_STARTPOS_,i_LENGTH_,0)
#define i_SELECT_RF_STRING_FIND_I3(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Find_i() accepts from 4 to 5 arguments\"")
#define i_SELECT_RF_STRING_FIND_I2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Find_i() accepts from 4 to 5 arguments\"")
#define i_SELECT_RF_STRING_FIND_I1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Find_i() accepts from 4 to 5 arguments\"")
#define i_SELECT_RF_STRING_FIND_I0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Find_i() accepts from 4 to 5 arguments\"")
#endif


/**
 ** @memberof RF_String
 ** @brief Finds if a String begins with a particular substring
 **
 ** @isinherited{StringX}
 ** Determines if the string begins with the @c sstr substring. You have the
 ** option to either match case or perform a case-insensitive search. In addition you can search
 ** for the exact string and not it just being a part of another string.
 ** @lmsFunction
 ** @param thisstr This string we want to search in @inhtype{String,StringX}
 ** @param sstr The substring to check for in the beginning @inhtype{String,StringX} @tmpSTR
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the search.Can have values:
 ** + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
 ** @return Returns true if the string does begin with @c sstr and false if not
 ** @see rfString_EndsWith()
 **
 **/
#if defined(RF_IAMHERE_FOR_DOXYGEN)
i_DECLIMEX_ char rfString_BeginsWith(const void* thisstr,const void* sstr,const char options);
#else
    #ifndef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfString_BeginsWith(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) (rfString_Find((i_THISSTR_),(i_SEARCHSTR_),(i_OPTIONS_)) == 0)
    #else
        #define rfString_BeginsWith(...) RF_SELECT_FUNC_IF_NARGGT2(i_NPSELECT_RF_STRING_BEGINSWITH,3,__VA_ARGS__)
        #define i_NPSELECT_RF_STRING_BEGINSWITH1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_BeginsWith() accepts from 2 to 3 arguments\"")
        #define i_NPSELECT_RF_STRING_BEGINSWITH0(...) RF_SELECT_FUNC2(i_SELECT_RF_STRING_BEGINSWITH,__VA_ARGS__)
        #define i_SELECT_RF_STRING_BEGINSWITH3(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) (rfString_Find(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) == 0)
        #define i_SELECT_RF_STRING_BEGINSWITH2(i_THISSTR_,i_SEARCHSTR_) (rfString_Find(i_THISSTR_,i_SEARCHSTR_) == 0)
        #define i_SELECT_RF_STRING_BEGINSWITH1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_BeginsWith() accepts from 2 to 3 arguments\"")
        #define i_SELECT_RF_STRING_BEGINSWITH0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_BeginsWith() accepts from 2 to 3 arguments\"")
    #endif
#endif

/**
 ** @memberof RF_String
 ** @brief Finds if a String ends with a particular substring
 **
 ** @isinherited{StringX}
 ** Determines if the string ends with the @c sstr substring. You have the
 ** option to either match case or perform a case-insensitive search. In addition you can search
 ** for the exact string and not it just being a part of another string.
 ** @lmsFunction
 ** @param thisstr This string we want to search in @inhtype{String,StringX}
 ** @param sstr The substring to check for in the end of the string @inhtype{String,StringX} @tmpSTR
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the search.Can have values:
 ** + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
 ** @return Returns true if the string does end with @c sstr and false if not
 ** @see rfString_BeginsWith()
 **
 **/
#if defined(RF_IAMHERE_FOR_DOXYGEN)
i_DECLIMEX_ char rfString_EndsWith(const void* thisstr,const void* sstr,const char options);
#else
    #ifndef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfString_EndsWith(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) \
            (rfString_Find(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) == (rfString_Length(i_THISSTR_)-rfString_Length(i_SEARCHSTR_)) )
    #else
        #define rfString_EndsWith(...) RF_SELECT_FUNC_IF_NARGGT2(i_NPSELECT_RF_STRING_ENDSWITH,3,__VA_ARGS__)
        #define i_NPSELECT_RF_STRING_ENDSWITH1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_EndsWith() accepts from 2 to 3 arguments\"")
        #define i_NPSELECT_RF_STRING_ENDSWITH0(...) RF_SELECT_FUNC2(i_SELECT_RF_STRING_ENDSWITH,__VA_ARGS__)
        #define i_SELECT_RF_STRING_ENDSWITH3(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) \
            (rfString_Find(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) == (rfString_Length(i_THISSTR_)-rfString_Length(i_SEARCHSTR_)) )
        #define i_SELECT_RF_STRING_ENDSWITH2(i_THISSTR_,i_SEARCHSTR_) \
            (rfString_Find(i_THISSTR_,i_SEARCHSTR_) == (rfString_Length(i_THISSTR_)-rfString_Length(i_SEARCHSTR_)) )
        #define i_SELECT_RF_STRING_ENDSWITH1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_EndsWith() accepts from 2 to 3 arguments\"")
        #define i_SELECT_RF_STRING_ENDSWITH0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_EndsWith() accepts from 2 to 3 arguments\"")
    #endif
#endif

/**
 ** @memberof RF_String
 ** @brief Counts how many times a substring occurs inside the string.
 **
 ** @isinherited{StringX}
 ** @lmsFunction
 ** @param thisstr The string inside which to count. @inhtype{String,StringX}
 ** @param sstr The substring for which to search. @inhtype{String,StringX} @tmpSTR
 ** @param options \rfoptional{0}. Bitflag options denoting some options for the search. Give 0 for the defaults.Can have values:
 ** + @c RF_CASE_IGNORE: If you want the found substring to ignore the case and returns success for any occurence of the string in any case.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you want the found substring to be exact. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would return a failure. Default search is to return any found substring.
 ** @return Returns the number of times cstr exists inside the string (0 is returned in case it's not found at all
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfString_Count(const void* thisstr,const void* sstr,const char options);
#else
i_DECLIMEX_ int32_t i_rfString_Count(const void* thisstr,const void* sstr,const char options);
#define rfString_Count(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_COUNT,3,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_COUNT1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Count() accepts from 2 to 3 arguments\"")
#define i_NPSELECT_RF_STRING_COUNT0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_COUNT,__VA_ARGS__)
#define i_SELECT_RF_STRING_COUNT3(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_) i_rfString_Count(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_)
#define i_SELECT_RF_STRING_COUNT2(i_THISSTR_,i_SEARCHSTR_)  i_rfString_Count(i_THISSTR_,i_SEARCHSTR_,0)
#define i_SELECT_RF_STRING_COUNT1(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Count() accepts from 2 to 3 arguments\"")
#define i_SELECT_RF_STRING_COUNT0(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Count() accepts from 2 to 3 arguments\"")
#endif

//! @}

/*------------------------------------------------------------------------ RF_String Positional retrieval functions-------------------------------------------------------------------------------*/
//! @name Positional String Retrieval Functions
//! @{

/**
 ** @memberof RF_String
 ** @brief Applies a limited version of sscanf after the specified substring
 **
 ** @isinherited{StringX}
 ** @lmsFunction
 ** @param thisstr The current string. @inhtype{String,StringX}
 ** @param afterstr The substring after which to apply sscanf. @inhtype{String,StringX} @tmpSTR
 ** @param format The tokens parameter which give the format of scanf
 ** @param var A void* to pass in any variable we need to get a value
 ** @return Returns true if a value was read and false otherwise, substring not being found in the string or sscanf unable to read into the variable
 **
 **/
i_DECLIMEX_ char rfString_ScanfAfter(const void* thisstr,const void* afterstr,const char* format,void* var);

/**
 ** @memberof RF_String
 ** @brief Initializes the first substring, between two given strings
 **
 ** @isinherited{StringX}
 ** Initializes the given string as the first substring existing between the left and right parameter substrings
 ** @lmsFunction
 ** @note The Returned Substring needs to be deinitialized by the user.
 ** @param thisstr This current string. @inhtype{String,StringX}
 ** @param[in] lstr The left substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
 ** @param[in] rstr The right substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
 ** @param[out] result Pass a pointer to a String type to receive the string between @c lstr and @c rstr.
 ** If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument. This should NOT ever be null. @inhtype{String,StringX}
 ** @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults.
 ** Can have values:
 ** + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would find nothing. Default is with this flag off.
 ** + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
 ** @return Returns true if the substring is found and initialized and false otherwise
 ** @see rfString_Before()
 ** @see rfString_After()
 ** @see rfStringX_MoveAfterPair()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_Between(const void* thisstr,const void* lstr,const void* rstr,void* result,const char options);
#else
i_DECLIMEX_ char i_rfString_Between(const void* thisstr,const void* lstr,const void* rstr,void* result,const char options);
        #define rfString_Between(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_BETWEEN,5,__VA_ARGS__)
        #define i_NPSELECT_RF_STRING_BETWEEN1(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Between() accepts from 4 to 5 arguments\"")
        #define i_NPSELECT_RF_STRING_BETWEEN0(...)  RF_SELECT_FUNC(i_SELECT_RF_STRING_BETWEEN,__VA_ARGS__)
        #define i_SELECT_RF_STRING_BETWEEN5(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_) \
            i_rfString_Between(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_)
        #define i_SELECT_RF_STRING_BETWEEN4(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_) \
            i_rfString_Between(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,0)
        #define i_SELECT_RF_STRING_BETWEEN3(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Between() accepts from 4 to 5 arguments\"")
        #define i_SELECT_RF_STRING_BETWEEN2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Between() accepts from 4 to 5 arguments\"")
        #define i_SELECT_RF_STRING_BETWEEN1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Between() accepts from 4 to 5 arguments\"")
        #define i_SELECT_RF_STRING_BETWEEN0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Between() accepts from 4 to 5 arguments\"")
#endif


/**
 ** @memberof RF_String
 ** @brief Initializes the given string as the substring from the start until any of the given Strings are found
 **
 ** @isinherited{StringX}
 ** The parameters that have to be given as variable argument <b>must</b> be of type RF_String* or RF_StringX* or even
 ** string initialized with the temporary string macro
 ** @lmsFunction
 ** @param thisstr The string to operate in. @inhtype{String,StringX}
 ** @param[out] result Pass a pointer to a String type to receive the string from the start of @c thisstr until any of the given substrings are found.
 ** If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument. This should NOT ever be null. @inhtype{String,StringX}
 ** @param options Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults.
 **  Can have values:
 ** + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would find nothing. Default is with this flag off.
 ** + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
 ** @param parN The number of strings to search for
 ** @param ... The strings to search for. @inhtype{String,StringX} @tmpSTR
 ** @return Returns true if the substring was initialized and false if none of the parameters were found or an invalid UTF-8 sequence was given. In the latter case an error is also logged.
 ** @see rfString_Before()
 ** @see rfString_Afterv()
 **
 **/
i_DECLIMEX_ char rfString_Beforev(const void* thisstr,void* result,const char options,const unsigned char parN, ...);


/**
 ** @memberof RF_String
 ** @brief Initializes the given string as the substring from the start until the given string is found
 **
 ** @isinherited{StringX}
 ** @lmsFunction
 ** @param thisstr The string to operate in. @inhtype{String,StringX}
 ** @param sstr The substring that we want to find inside the string @inhtype{String,StringX} @tmpSTR
 ** @param[out] result Pass a pointer to a String type to receive the string from the start of @c thisstr until the given substring is found.
 ** If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument. This should NOT ever be null. @inhtype{String,StringX}
 ** @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults.
 ** Can have values:
 ** + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would find nothing. Default is with this flag off.
 ** + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
 ** @return Returns true if the substring was initialized and false if none of the parameters were found or an invalid UTF-8 sequence was given.
 ** In the latter case an error is also logged.
 ** @see rfString_Beforev()
 ** @see rfString_After()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_Before(const void* thisstr,const void* sstr,void* result,const char options);
#else
i_DECLIMEX_ char i_rfString_Before(const void* thisstr,const void* sstr,void* result,const char options);
#define rfString_Before(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_BEFORE,4,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_BEFORE1(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Before() accepts from 3 to 4 arguments\"")
#define i_NPSELECT_RF_STRING_BEFORE0(...)  RF_SELECT_FUNC(i_SELECT_RF_STRING_BEFORE,__VA_ARGS__)
#define i_SELECT_RF_STRING_BEFORE4(i_THISSTR_,i_SEARCHSTR_,i_RESULT_,i_OPTIONS_) i_rfString_Before(i_THISSTR_,i_SEARCHSTR_,i_RESULT_,i_OPTIONS_)
#define i_SELECT_RF_STRING_BEFORE3(i_THISSTR_,i_SEARCHSTR_,i_RESULT_) i_rfString_Before(i_THISSTR_,i_SEARCHSTR_,i_RESULT_,0)
#define i_SELECT_RF_STRING_BEFORE2(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Before() accepts from 3 to 4 arguments\"")
#define i_SELECT_RF_STRING_BEFORE1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Before() accepts from 3 to 4 arguments\"")
#define i_SELECT_RF_STRING_BEFORE0(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_Before() accepts from 3 to 4 arguments\"")
#endif

/**
 ** @memberof RF_String
 ** @brief Initialize a string after a given substring
 **
 ** @isinherited{StringX}
 ** Initializes the given String with the substring located after (and not including) the after substring inside the parameter string. If the substring is not located the function returns false.
 ** @note The given String needs to be deinitialized by the user
 ** @lmsFunction
 ** @param[in] thisstr The parameter string from which the substring will be formed. @inhtype{String,StringX}
 ** @param[in] after The substring to search for inside the parameter string. @inhtype{String,StringX} @tmpSTR
 ** @param[out] result Pass a pointer to a String type to receive the substring of @c thisstr after the @c after string has been found.
 ** If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument. This should NOT ever be null. @inhtype{String,StringX}
 ** @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults.
 ** Can have values:
 ** + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would find nothing. Default is with this flag off.
 ** + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
 ** @return Returns true for success and false if the substring is not found in the parameter string.
 ** @see rfString_Afterv()
 ** @see rfString_Before()
 ** @see rfStringX_MoveAfter()
 **
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_After(const void* thisstr,const void* after,void* result,const char options);
#else
i_DECLIMEX_ char i_rfString_After(const void* thisstr,const void* after,void* result,const char options);
#define rfString_After(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_AFTER,4,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_AFTER1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_After() accepts from 3 to 4 arguments\"")
#define i_NPSELECT_RF_STRING_AFTER0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_AFTER,__VA_ARGS__)
#define i_SELECT_RF_STRING_AFTER4(i_THISSTR_,i_AFTERSTR_,i_OUTSTR_,i_OPTIONS_) i_rfString_After(i_THISSTR_,i_AFTERSTR_,i_OUTSTR_,i_OPTIONS_)
#define i_SELECT_RF_STRING_AFTER3(i_THISSTR_,i_AFTERSTR_,i_OUTSTR_) i_rfString_After(i_THISSTR_,i_AFTERSTR_,i_OUTSTR_,0)
#define i_SELECT_RF_STRING_AFTER2(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_After() accepts from 3 to 4 arguments\"")
#define i_SELECT_RF_STRING_AFTER1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_After() accepts from 3 to 4 arguments\"")
#define i_SELECT_RF_STRING_AFTER0(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfString_After() accepts from 3 to 4 arguments\"")
#endif

/**
 ** @memberof RF_String
 ** @brief Initialize a string after the first of the given substrings found
 **
 ** @isinherited{StringX}
 ** Initializes the given String with the substring located after (and not including) the after substring inside the parameter string. If the substring is not located the function returns false.
 ** The parameters that have to be given as variable argument <b>must</b> be of type RF_String* or RF_StringX* or even
 ** string initializes with the temporary string macro
 ** @lmsFunction
 ** @param[in] thisstr The parameter string from which the substring will be formed. @inhtype{String,StringX}
 ** @param[out] result Pass a pointer to a String type to receive the substring of @c thisstr after the the first of the given substrings has been found.
 ** If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument. This should NOT ever be null. @inhtype{String,StringX}
 ** @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults.
 ** Can have values:
 ** + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
 **     Default search option is to @b match the case. For now this works only for characters of the english language.
 ** + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
 **     @e "HELLOWORLD" would find nothing. Default is with this flag off.
 ** + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
 ** @param parN The number of substrings to search for.
 ** @param ... The substrings to search for. @inhtype{String,StringX} @tmpSTR
 ** @return Returns true for success and false if the substring is not found in the parameter string or in error
 ** @see rfString_After()
 ** @see rfString_Beforev()
 **
 **/
i_DECLIMEX_ char rfString_Afterv(const void* thisstr,void* result,const char options,const unsigned char parN,...);


//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif
