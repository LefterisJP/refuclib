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
** --String/common.h
** This header includes common String functionality common for all strings
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/common.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_COMMON_H
#define RF_STRING_COMMON_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif



//! @memberof RF_String
//! @brief Create a termporary String from a String literal
//!
//! A macro to be used only inside a function call that accepts an @ref RF_String or any
//! inherited data type such as @ref RF_StringX. It creates a Temporary RF_String*
//! that will be used by the function. This macro accepts from 1 to N arguments.
//!
//! The first argument shall either be a String literal or a printf styled string literal
//! given in UTF-8 encoding. Other encodings are easy to support but are not currently
//! supported since the current limitation is for UTF-8 encoded source code.
//!
//! Optionally the first argument can be followed by a sequence of additional arguments that would
//! specify how to format the string @c s. If additional arguments follow then the @c s shall be
//! formatted using the Refu String formatting rules, more details about which can be found at
//! @ref rfStringX_Formatv()
//! @warning
//! + If no extra arguments are given then the given string @c s is not interpreted as a formatted
//! string and is instead printed as is.
//! @warning
//! + This macro can not be called recursively with extra arguments and doing so is undefined behaviour.
//! Moreover there is no reason to do so as the example below tries to so.
//! <br /><b>Illegal and undefined behaviour</b>
//! @code rfString_Append(&s,RFS_("Never call the %S",RFS_("%s macro recursively with extra arguments such as this %d","RFS macro",1024))); @endcode
//! The equivalent legal (and more simple way to do the above is:
//! @code rfString_Append(&s,RFS_("Never call the %s macro recursively with extra arguments such as this %d","RFS macro",1024)); @endcode
//!
//! @param s The formatted string that will constitute the RF_String. Must be in the same encoding as that of the source file.
//! Default is UTF-8.
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c slit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return Returns true in case of correct initialization and false , due to invalid byte sequence for the given encoding
//! @isinherited{StringX}
#ifdef RF_IAMHERE_FOR_DOXYGEN
RF_String* RFS_(const char* s,...);
#else
#define RFS_(...) i_rfString_CreateLocal(__VA_ARGS__)
#endif

///Internal functions that create a temporary RF_String*
i_DECLIMEX_ RF_String* i_rfString_CreateLocal1(const char* s,...);
i_DECLIMEX_ RF_String* i_NVrfString_CreateLocal(const char* s);
#define i_rfString_CreateLocal(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATELOCAL,1,__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL1(...) i_NVrfString_CreateLocal(__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATELOCAL0(...) i_rfString_CreateLocal1(__VA_ARGS__)


#ifdef __cplusplus
}//closing bracket for calling from C++
#endif


#endif//include guards end
