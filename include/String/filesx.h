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
** --String/filesx.h
** This header includes FILE descriptor functionality for RF_StringX
**
---------------------For internal library include make sure to have----------------------------
#include <stdio.h> //for FILE*
#include <IO/common.h> //for RF_EOL macros
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/unicode.h> //for the unicode macros RF_UTF8 and friends
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include <String/filesx.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_FILESX_H
#define RF_STRING_FILESX_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/*--- File I/O RF_StringX Functions ---*/
//! @name RF_StringX File I/O
//! @{

/**
 ** @memberof RF_StringX
 ** @brief Allocates and returns a stringX from file parsing
 **
 ** Read the file stream @c f until either a newline character or the EOF is 
 ** reached and saves it as an RF_StringX
 ** If for some reason (like EOF reached) no string can be read then null is returned
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Can be one of:
 ** + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as 
 ** the end of line signal
 ** + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the 
 ** end of line signal
 ** + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as
 ** the end of line signal
 ** @param encoding The encoding of the file.
 ** + @c RF_UTF8: For Unicode UTF-8 encoding
 ** + @c RF_UTF16: For Unicode UTF-16 encoding
 ** + @c RF_UTF32: For Unicode UTF-32 encoding
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Possible values here are @c RF_LITTLE_ENDIAN and
 ** @c RF_BIG_ENDIAN and apply only if the file is either UTF-16 or UTF-32
 ** @return The initialized string or null pointer in case of failure
 **  to read the file, or unexpected data (non-UTF8 encoded string)
 ** @see rfStringX_FInit()
 ** @see rfStringX_FAssign()
 ** @see rfStringX_FAppend()
 **
 **/
i_DECLIMEX_ RF_StringX* rfStringX_FCreate(FILE* f, char* eof, char eol,
                                          int encoding, int endianess);
/**
 ** @memberof RF_StringX
 ** @brief Initializes a stringX from file parsing
 **
 ** Read the file stream @c f until either a newline character or the EOF is 
 ** reached and saves it as an RF_StringX
 ** If for some reason (like EOF reached) no string can be read then null is returned
 ** @param str The StringX to initialize
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Can be one of:
 ** + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as 
 ** the end of line signal
 ** + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the 
 ** end of line signal
 ** + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as
 ** the end of line signal
 ** @param encoding The encoding of the file.
 ** + @c RF_UTF8: For Unicode UTF-8 encoding
 ** + @c RF_UTF16: For Unicode UTF-16 encoding
 ** + @c RF_UTF32: For Unicode UTF-32 encoding
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Possible values here are @c RF_LITTLE_ENDIAN and
 ** @c RF_BIG_ENDIAN and apply only if the file is either UTF-16 or UTF-32
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ char rfStringX_FInit(RF_StringX* str, FILE* f, char* eof, char eol,
                                 int encoding, int endianess);

/**
 ** @memberof RF_StringX
 ** @brief Assigns to a stringX from file parsing
 **
 ** Read the file stream @c f until either a newline character or the EOF is
 ** reached and assigns it to an RF_StringX
 ** @param str The stringX to assign to
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Can be one of:
 ** + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as 
 ** the end of line signal
 ** + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the 
 ** end of line signal
 ** + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as
 ** the end of line signal
 ** @param encoding The encoding of the file.
 ** + @c RF_UTF8: For Unicode UTF-8 encoding
 ** + @c RF_UTF16: For Unicode UTF-16 encoding
 ** + @c RF_UTF32: For Unicode UTF-32 encoding
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Possible values here are @c RF_LITTLE_ENDIAN and
 ** @c RF_BIG_ENDIAN and apply only if the file is either UTF-16 or UTF-32
 ** @return Returns @c true for success and @c false otherwise
 ** @see rfStringX_FInit()
 ** @see rfStringX_FCreate()
 ** @see rfStringX_FAppend()
 **
 **/
i_DECLIMEX_ char rfStringX_FAssign(RF_StringX* str, FILE* f, char* eof,
                                   char eol, int encoding, int endianess);
/**
 ** @memberof RF_StringX
 ** @brief Appends to a stringX from file parsing
 **
 ** @param str The stringX to append to
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Can be one of:
 ** + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as 
 ** the end of line signal
 ** + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the 
 ** end of line signal
 ** + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as
 ** the end of line signal
 ** @param encoding The encoding of the file.
 ** + @c RF_UTF8: For Unicode UTF-8 encoding
 ** + @c RF_UTF16: For Unicode UTF-16 encoding
 ** + @c RF_UTF32: For Unicode UTF-32 encoding
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Possible values here are @c RF_LITTLE_ENDIAN and
 ** @c RF_BIG_ENDIAN and apply only if the file is either UTF-16 or UTF-32
 ** @return Returns @c true for success and @c false otherwise
 ** @see rfStringX_FInit()
 ** @see rfStringX_FCreate()
 ** @see rfStringX_FAssign()
 **
 **/
i_DECLIMEX_ char rfStringX_FAppend(RF_StringX* str, FILE* f, char* eof, char eol,
                                   int encoding, int endianess);

//! @}



#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
