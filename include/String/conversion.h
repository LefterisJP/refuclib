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
** --String/conversion.h
** This header includes functions for String conversion
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/conversion.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_CONVERSION_H
#define RF_STRING_CONVERSION_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/*------------------------------------------------------------------------ RF_String unicode conversion-------------------------------------------------------------------------------*/
//! @name Unicode Conversion Functions
//! @{

//! @memberof RF_String
//! @brief Returns the strings contents as a UTF-8 buffer
//!
//! @isinherited{StringX}
//! This is just a macro wrapper of @ref rfString_Cstr() and exists here
//! just so that users can guess function names for all unicode encodings.
//!
//!  Note that just like in @ref rfString_Cstr() this is just a pointer to
//! the String's internal UTF8 buffer and as such should be read only. If there
//! is a need to do anything other than that copy the buffer.
//! @param s The string in question
//! @return Returns a pointer to the String's internal UTF-8 uffer
//! @see rfString_ToUTF16()
//! @see rfString_ToUTF32()
#if defined(RF_IAMHERE_FOR_DOXYGEN)
i_DECLIMEX_ const char* rfString_ToUTF8(RF_String* s);
#else
#define rfString_ToUTF8(i_STRING_)  rfString_Cstr(i_STRING_)
#endif

//! @memberof RF_String
//! @brief Returns the strings contents as a UTF-16 buffer
//!
//! @isinherited{StringX}
//! This function allocates a UTF-16 buffer in which the string's
//! UTF-8 contents are encoded as UTF-16. The endianess of the buffer
//! is that of the system. The returned buffer needs to be freed by the user
//! later.
//! @param[in] s The string in question. @inhtype{String,StringX}
//! @param[out] length Give a reference to a uint32_t in this argument to receive the length of
//! the returned UTF-16 buffer in 16-bit words
//! @return Returns an allocated UTF-16 buffer. Needs to be freed by the user later.
//! @see rfString_ToUTF8()
//! @see rfString_ToUTF32()
i_DECLIMEX_ uint16_t* rfString_ToUTF16(const void* s,uint32_t* length);

//! @memberof RF_String
//! @brief Returns the strings contents as a UTF-32 buffer
//!
//! @isinherited{StringX}
//! This function allocates a UTF-32 buffer in which the string's
//! UTF-8 contents are encoded as UTF-32. The endianess of the buffer
//! is that of the system. The returned buffer needs to be freed by the user
//! later.
//! @param[in] s The string in question. @inhtype{String,StringX}
//! @param[out] length Give a reference to a uint32_t in this argument to receive the length
//! of the returned UTF-32 buffer in codepoints. (32-bit) words
//! @return Returns an allocated UTF-16 buffer. Needs to be freed by the user later.
//! @see rfString_ToUTF8()
//! @see rfString_ToUTF16()
i_DECLIMEX_ uint32_t* rfString_ToUTF32(const void* s,uint32_t*length);

//! @}

/*------------------------------------------------------------------------ RF_String Conversion functions-------------------------------------------------------------------------------*/

//! @name String Conversion Functions
//! @{

//! @memberof RF_String
//! @brief Returns the cstring representation of the string
//!
//! @isinherited{StringX}
//! Remember that this is just a pointer to the string data. It can't be modified. memcpy it if you need a copy of it.
//! @param s The string whose cstring repesentation to get. @inhtype{String,StringX}
//! @return Returns the a pointer to the array of bytes held inside the string in the UTF-8 encoding
i_DECLIMEX_ char* rfString_Cstr(const void* s);

//! @memberof RF_String
//! @brief Returns the integer value of a String
//!
//! @isinherited{StringX}
//! The parameter string must contains only numbers. If it contains anything else the function fails.
//! @param thisstr The string whose integer value to return. @inhtype{String,StringX}
//! @param[out] v A refence to an integer that will return the float value
//! @return Returns true in case of succesfull conversion or false if no integer was represented by the string
//! @see rfString_ToDouble()
i_DECLIMEX_ char rfString_ToInt(const void* thisstr,int32_t* v);

//! @memberof RF_String
//! @brief Returns the double value of a String
//!
//! @isinherited{StringX}
//! The parameter string must contain only a number. If it contains anything else the function fails.
//! @param thisstr The string whose floating point value to return. @inhtype{String,StringX}
//! @param[out] f A refence to a double that will return the floating point number value
//! @return Returns RF_SUCCESS in case of succesfull conversion or error if there was failure. Possible errors are:
//! + @c RE_STRING_TOFLOAT: There was a conversion error. The string probably does not represent a float
//! + @c RE_STRING_TOFLOAT_RANGE: The represented floating point number is of a range bigger than what can be
//! represented by the system
//! + @c RE_STRING_TOFLOAT_UNDERFLOW: Representing the string's floating point number in a double would cause underflow
//! @see rfString_ToInt()
i_DECLIMEX_ int rfString_ToDouble(const void* thisstr,double* f);

//! @memberof RF_String
//! @brief Turns any uppercase characters of the string into lower case
//!
//! @isinherited{StringX}
//! This function as of now only works with letter of the English alphabet.
//! All the characters of the string that are uppercase shall be turned into lowercase
//! @param thisstr The string for which to perform the uppercase to lowercase conversion
//! @see rfString_ToUpper()
i_DECLIMEX_ void rfString_ToLower(void* thisstr);

//! @memberof RF_String
//! @brief Turns any lowercase characters of the string into upper case
//!
//! @isinherited{StringX}
//! This function as of now only works with letter of the English alphabet.
//! All the characters of the string that are lowercase shall be turned into uppercase
//! @param thisstr The string for which to perform the lowercase to uppercase conversion
//! @see rfString_ToLower()
i_DECLIMEX_ void rfString_ToUpper(void* thisstr);

//! @memberof RF_String
//! @brief Tokenizes the given string
//!
//! @isinherited{StringX}
//! Separates it into @c tokensN depending on how many substrings can be created from the @c sep separatior and stores them
//! into the Array of RF_String* that should be passed to the function. The array gets initialized inside the function and
//! <b>has to be freed explicitly</b> later by thg user. Also each String inside the array has to be Deinitialized too.
//! Here is an example usage:
//! @snippet Strings/tokenize.cpp Tokenize_C
//! @lmsFunction
//! @param[in] thisstr The string to tokenize. @inhtype{String,StringX}
//! @param[in] sep A substring that will be used as a separator @inhtype{String,StringX}
//! @param[out] tokensN The number of tokens that got created
//! @param[out] tokens Pass a pointer to an array of RF_Strings. @keepptr
//! @return Returns true in success and false in case the the separating character has not been found
//! @internal @cppcode
//! char String::Tokenize(char* sep,uint32_t* tokensN, String*** tokens)
//! {
//!     RF_String* t;
//!     uint32_t i;
//!     if(rfString_Tokenize(this->i_StringCHandle,sep,tokensN,&t)==false)
//!         return false;
//!
//!     *tokens = (String**) malloc(sizeof(String*)* (*tokensN));
//!     for(i=0;i<(*tokensN);i++)
//!     {
//!        (*tokens)[i] = new String((RF_String*)&t[i]);
//!     }
//!     return true;
//! }
//! @endcpp @endinternal
i_DECLIMEX_ char rfString_Tokenize(const void* thisstr,const void* sep,uint32_t* tokensN,RF_String** tokens);

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
