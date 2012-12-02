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
#ifndef RF_STRING_FORMAT_H
#define RF_STRING_FORMAT_H

#include <stdarg.h>
#include <String/stringx_decl.h>//StringX declaration

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name Formatted String functions
//! @{

//! @memberof RF_StringX
//! @brief Formats an @ref RF_StringX according to the given format string and variable argument list
//!
//! This implementation loosely interprets the C standard on @c printf() and friends. Specifically it has some additional rules
//! and may not follow all that the standard says about printf so please check below for a full documentation of what this function does.
//!
//! @lmsFunction
//! @param s Pass an @ref RF_StringX that is already initialized with a big enough buffer (if it is not it will be reallocated inside the function)
//! inside which the string will be returned in the end. Note that all its previous contents shall be erased.
//! @param format The formatted string which specifies what kind of arguments to expect. Everything will be copied inside @c s except
//! for specific format specifiers which begin with the special character @c '%'.
//!
//! A format specifier follows this prototype: %[@c flags][@c width][.@c precision][@c length]@c specifier
//! Everything but the @c specifier are optional.
//!
//! | @b Specifier  | Output | Example|
//! | :----: | :----:| :----: |
//! | d or i | Signed integer                           | -1337             |
//! | u      | Unsigned integer                         | 2034                          |
//! | x or X | Hexadecimal integer <br /> <p align="justify"> If the '#' flag is given then a "0x" shall be prepended to the hexadecimal. If '%f' is given the the hexadecimals will be printed in lowercase and if '%F' is given they will be in uppercase </p> | F 0xFA 0xc23 |
//! | f or F | Floating point number <br/> <p align="justify">The argument shall be interpreted as a double and it will be printed as a floating point number with @c precision digits after the radix. Default precision is 6. The low-order digit shall be rounded in an implementation-defined manner. For infinity and nan "INF" and "NAN shall be printed for %F and "inf" and "nan" for %f</p>  | 3.141592                      |
//! | c      | UTF-8 Encoded Unicode Character          | 'a','誠'                       |
//! | s      | C String                                 | "Hello","Θεσσαλονικη"                 |
//! | S      | Refu String                              | RFS_("こんにちは"), RFS_("Refu String")  |
//! <br />
//! | @b Flags  | Explanation|
//! | :----: | :----:|
//! | - | <p align="justify">Left-justifies the value of the variable. What this means is that if a minimum @b width is given and <br />it's bigger than the actual characters required for the value then the extra padding character shall be put at the right of the value. The default is right-justification.</p>  |
//! | + | <p align="justify">Forces the sign in positive numbers </p>|
//! | (space) |<p align="justify">If the first character of a signed conversion is not a sign or if a signed conversion results in no characters, a <space> shall be prefixed to the result </p>|
//! | # | <p align="justify">Specifies that the value is to be converted to an alternative form. <ul><li>For hexadecimal arguments it requests the prepending of @c '0x'</li> <li>For all floating point argments(@c f,@cF) a radix shall always be printed even if no digits actually follow the radix</li></ul></p> |
//! | 0 | <p align="justify">The padding used in the case of a given width is zeroes instead of empty spaces</p> |
//! <br />
//! | @b Width value  | Explanation|
//! | :----: | :----:|
//! | (number) | <p align="justify">If a a number is given as width then it is interpeted as the minimum number of characters to print for the value. If the value contains less characters than this minimum then padding is added to make up for it. The padding is added to the left of the value by default unless the right justification flag is active. Also if the @c '0' flag has been passed then zeroes are used for padding</p>|
//! | * | <p align="justify">If a @c * is given then the width is not read from the formatted string but instead its value is passed as an additional integer argument preceding the argument for formatting </p> |
//! <br />
//! | @b Precision value  | Explanation|
//! | :----: | :----:|
//! | .(number) | <p align="justify">If a a number is given for precision then it is interpeted as the minimum number of digits to print for the value if it is decimal, hex or int.For floating point values it is the number of digits to print after the radix</p>|
//! | .* | <p align="justify">If a @c * is given then the precision is not read from the formatted string but instead its value is passed as an additional integer argument preceding the argument for formatting </p> |
//!
//! @param argList A @c va_list which holds the extra arguments with which to format the string
//! @return @c RF_SUCCESS for succesful formatting and error otherwise. Possible errors are:
//! + @c RE_FORMAT_UNEXPECTEDCHAR: If during parsing the formatted string an unexpected character
//! is encountered after the @c '%'
//! + @c RE_FORMAT_ILLEGALPRECISION: If an illegal precision flag has been detected in the formatted string
i_DECLIMEX_ int32_t rfStringX_Formatv(RF_StringX *s,const char* format,va_list argList);

//! @memberof RF_StringX
//! @brief Formats an @ref RF_StringX according to the given format string and variable arguments
//!
//! For a full documentation of this function look at the equivalent function
//! accepting a variable argument list @ref rfStringX_Formatv()
//! @lmsFunction
i_DECLIMEX_ int32_t rfStringX_Format(RF_StringX *s,const char* format,...);

//! @}
#ifdef __cplusplus
}//closing bracket for calling from C++
#endif

#endif//include guards end
