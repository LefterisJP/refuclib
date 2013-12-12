/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/
#ifndef RF_STRING_FILES_H
#define RF_STRING_FILES_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstring/X
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
#include <Utils/rf_unicode.h> //for RFtext_encoding
#include <Utils/endianess.h> //for RFendianess
#include <IO/rf_io_common.h> //for RFeol_mark
/*------------- libc inclusion --------------*/
#include <stdio.h> //for FILE
#include <string.h> //for size_t
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name String File Descriptor I/O functions
//! @{

/**
 ** @brief Initializes a string from file parsing
 **
 ** @notinherited{StringX}
 ** Read the file stream @c f until either a newline character or the EOF is 
 ** reached and saves it as an RFstring
 ** If for some reason (like EOF reached) no string can be read then null is returned
 ** @param str The String to initialize
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 **                for possible values
 ** @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 **                 definition for possible values.
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Look @ref RFendianess for posssible values.
 ** @param buff_size Give a pointer to an unsigned int here to get the size
 ** of the buffer allocated for the string. You can give NULL if you are not
 ** interested in its value
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_string_from_file_init(struct RFstring* str, FILE* f, char* eof,
                                 enum RFeol_mark eol,
                                 enum RFtext_encoding encoding,
                                 enum RFendianess endianess,
                                 unsigned int* buff_size);

/**
 ** @brief Allocates and returns a string from file parsing
 ** @notinherited{StringX}
 ** @see rf_string_from_file_init()
 ** @see rf_string_from_file_assign()
 ** @see rf_string_from_file_append()
 **/
i_DECLIMEX_ struct RFstring* rf_string_from_file_create(FILE* f, char* eof,
                                        enum RFeol_mark eol,
                                        enum RFtext_encoding encoding,
                                        enum RFendianess endianess,
                                        unsigned int* buff_size);
/**
 ** @brief Assigns to a string from file parsing
 **
 ** @notinherited{StringX}
 ** Read the file stream @c f until either a newline character or the EOF is
 ** reached and assigns it to an RFstring
 ** @param str The string to assign to
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 **                for possible values
 ** @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 **                 definition for possible values.
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Look @ref RFendianess for posssible values.
 ** @return Returns @c true for success and @c false otherwise
 ** @see rf_string_from_file_init()
 ** @see rf_string_from_file_create()
 ** @see rf_string_from_file_append()
 **
 **/
i_DECLIMEX_ bool rf_string_from_file_assign(struct RFstring* str,
                                            FILE* f, char* eof,
                                            enum RFeol_mark eol,
                                            enum RFtext_encoding encoding,
                                            enum RFendianess endianess);
/**
 ** @brief Appends to a string from file parsing
 **
 ** @notinherited{StringX}
 ** @param str The string to append to
 ** @param f A valid and open file pointer in read mode from which to read the
 ** string. 
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 ** value in case the end of file was reached with this initialization
 ** @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 **                for possible values
 ** @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 **                 definition for possible values.
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Look @ref RFendianess for posssible values.
 ** @return Returns @c true for success and @c false otherwise
 ** @see rf_string_from_file_init()
 ** @see rf_string_from_file_create()
 ** @see rf_string_from_file_assign()
 **
 **/
i_DECLIMEX_ bool rf_string_from_file_append(struct RFstring* str, FILE* f,
                                            char* eof,
                                            enum RFeol_mark eol,
                                            enum RFtext_encoding encoding,
                                            enum RFendianess endianess);

/**
 ** @brief Writes a string to a file depending on the given encoding
 **
 ** @isinherited{StringX}
 ** This function shall output the string @c s into the file descriptor @c f in
 **  the given @c encoding .
 ** @lmsFunction
 ** @param s           The string to write to the file @inhtype{String,StringX}
 **                    @tmpSTR
 ** @param f           A valid and open file pointer into which to write the
 **                    string.
 ** @param encoding The encoding of the file's text. Look @ref RFtext_encoding
 **                 definition for possible values.
 ** @param endianess A flag that determines in what endianess the file is 
 ** encoded in. Look @ref RFendianess for posssible values.
 ** @return            Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_string_fwrite(void* s, FILE* f,
                                  enum RFtext_encoding encoding,
                                  enum RFendianess endianess);


//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
