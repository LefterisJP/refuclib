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
** --String/files.h
** This header includes FILE descriptor functionality for RF_String
**
---------------------For internal library include make sure to have----------------------------
#include <stdio.h> //for FILE*
#include <IO/common.h> //for RF_EOL macros
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/unicode.h> //for the unicode macros RF_UTF8 and friends
#include <String/string_decl.h>//for RF_String
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/files.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_STRING_FILES_H
#define RF_STRING_FILES_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/*--- RF_String File Descriptor I/O functions ---*/
//! @name String File Descriptor I/O functions
//! @{

/**
 ** @memberof RF_String
 ** @brief Allocates and returns a string from file parsing
 **
 ** @notinherited{StringX}
 ** Read the file stream @c f until either a newline character or the EOF is 
 ** reached and saves it as an RF_String
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
 ** @see rfString_FInit()
 ** @see rfString_FAssign()
 ** @see rfString_FAppend()
 **
 **/
i_DECLIMEX_ RF_String* rfString_FCreate(FILE* f, char* eof, char eol,
                                        int encoding, int endianess);
/**
 ** @memberof RF_String
 ** @brief Initializes a string from file parsing
 **
 ** @notinherited{StringX}
 ** Read the file stream @c f until either a newline character or the EOF is 
 ** reached and saves it as an RF_String
 ** If for some reason (like EOF reached) no string can be read then null is returned
 ** @param str The String to initialize
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
i_DECLIMEX_ char rfString_FInit(RF_String* str, FILE* f, char* eof, char eol,
                                int encoding, int endianess);

/**
 ** @memberof RF_String
 ** @brief Assigns to a string from file parsing
 **
 ** @notinherited{StringX}
 ** Read the file stream @c f until either a newline character or the EOF is
 ** reached and assigns it to an RF_String
 ** @param str The string to assign to
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
 ** @see rfString_FInit()
 ** @see rfString_FCreate()
 ** @see rfString_FAppend()
 **
 **/
i_DECLIMEX_ char rfString_FAssign(RF_String* str, FILE* f, char* eof,
                                  char eol, int encoding, int endianess);
/**
 ** @memberof RF_String
 ** @brief Appends to a string from file parsing
 **
 ** @notinherited{StringX}
 ** @param str The string to append to
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
 ** @see rfString_FInit()
 ** @see rfString_FCreate()
 ** @see rfString_FAssign()
 **
 **/
i_DECLIMEX_ char rfString_FAppend(RF_String* str, FILE* f, char* eof, char eol,
                                   int encoding, int endianess);

/**
 ** @memberof RF_String
 ** @brief Writes a string to a file depending on the given encoding
 **
 ** @isinherited{StringX}
 ** This function shall output the string @c s into the file descriptor @c f in
 **  the given @c encoding .
 ** @lmsFunction
 ** @param s The string to write to the file @inhtype{String,StringX} @tmpSTR
 ** @param f A valid and open file pointer into which to write the string.
 ** @param encoding \rfoptional{@c RF_UTF8} The encoding of the file.
 **  Default is @c RF_UTF8. Can be one of:
 ** + @c RF_UTF8: For Unicode UTF-8 encoding
 ** + @c RF_UTF16: For Unicode UTF-16 encoding
 ** + @c RF_UTF32: For Unicode UTF-32 encoding
 ** @param endianess \rfoptional{@c rfEndianess()} The endianess that we want
 ** the written file to have.
 ** Can be one of @c RF_LITTLE_ENDIAN or @c RF_BIG_ENDIAN
 ** @return Returns @c true for success and @c false otherwise
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_Fwrite(void* s, FILE* f, int encoding, int endianess);
#else
i_DECLIMEX_ char i_rfString_Fwrite(void* s, FILE* f, int encoding, int endianess);
#define rfString_Fwrite(...) \
    RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_FWRITE, 4, __VA_ARGS__)
#define i_NPSELECT_RF_STRING_FWRITE1(...) \
    RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function " \
                     "rfString_Fwrite() accepts from 2 to 4 arguments\"")
#define i_NPSELECT_RF_STRING_FWRITE0(...) \
    RF_SELECT_FUNC(i_SELECT_RF_STRING_FWRITE,__VA_ARGS__)
#define i_SELECT_RF_STRING_FWRITE4(i_STR_, i_FILE_, i_ENCODING_, i_ENDIANESS_) \
    i_rfString_Fwrite(i_STR_, i_FILE_, i_ENCODING_, i_ENDIANESS_)
#define i_SELECT_RF_STRING_FWRITE3(i_STR_, i_FILE_, i_ENCODING_)  \
    i_rfString_Fwrite(i_STR_, i_FILE_, i_ENCODING_, rfEndianess())
#define i_SELECT_RF_STRING_FWRITE2(i_STR_, i_FILE_)  \
    i_rfString_Fwrite(i_STR_, i_FILE_, RF_UTF8, rfEndianess())
#define i_SELECT_RF_STRING_FWRITE1(...) \
    RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function "\
                     "rfString_Fwrite() accepts from 2 to 4 arguments\"")
#define i_SELECT_RF_STRING_FWRITE0(...) \
    RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function "\
                     "rfString_Fwrite() accepts from 2 to 4 arguments\"")
#endif

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
