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
*/
#ifndef RF_STRING_DECL_H
#define RF_STRING_DECL_H


/**
** @internal
** @author Lefteris
** @date 09/12/2010
** @endinternal
** @brief A unicode String with UTF-8 internal representation
**
** The Refu String is a Unicode String that has two versions. One is this and for the other check @ref RF_StringX to see what operations can be performed on extended Strings.
** Functions to convert to and from all UTF encoding exists but the internal representation is always at UTF-8. Once a
** a String has been created it is always assumed that the stream of bytes inside it is valid UTF-8 since every function
** performs a UTF-8 check unless otherwise specified.
**
** All the functions which have @isinherited{StringX} on their description can be used with extended strings safely, since no specific
** version of the function exists, or needs to exist to manipulate Extended Strings. To make the documentation even clearer the functions that should not
** be used with the extended string are marked with @notinherited{StringX}
**
** @see RF_StringX
** @internal
** @cppcode
** //default constructor
** String(){this->i_StringCHandle = rfString_Create("");}
** @endcpp
** @endinternal
*/
typedef struct RF_String
{
    //! The string's data
    char* bytes;
    //! The string's length in bytes (not including the null termination). The string keeps its length in bytes
    //! to avoid multiple calls to strlen()
    uint32_t byteLength;
}RF_String;

#endif//include guards end
