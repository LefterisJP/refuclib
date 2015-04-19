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
#ifndef REFU_IO_FILE_H
#define REFU_IO_FILE_H


/*------------- Module Headers inclusion -------------*/
#include <IO/rf_io_common.h> //for RFeol_mark
/*------------- Outside Module inclusion -------------*/
#include <Definitions/types.h> //for fixed size types needed in various places
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/inline.h> //for inline definitions
#include <Definitions/retcodes.h> //for booleans
#include <Utils/endianess.h> //for RFendianess
#include <String/rf_str_decl.h> //for RFstring
#include <String/rf_str_common.h> //for RFstring
#include <String/rf_str_conversion.h> //for RFstring
/*------------- System specific inclusion --------------*/
#ifdef _MSC_VER
#error TODO
#else
#include <sys/stat.h>
#endif
/*------------- libc inclusion --------------*/
#include <stdio.h> //for FILE*
#include <stdlib.h> //for free
/*------------- End of includes -------------*/


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


#if _FILE_OFFSET_BITS == 64

//Fseek and Ftell definitions
#ifdef _MSC_VER
    #define rfFseek(i_FILE_,i_OFFSET_,i_WHENCE_)    _fseeki64(i_FILE_,i_OFFSET_,i_WHENCE_)
    #define rfFtell(i_FILE_)                        _ftelli64(i_FILE_)
#else
    #define rfFseek(i_FILE_,i_OFFSET_,i_WHENCE_)    fseeko64(i_FILE_,i_OFFSET_,i_WHENCE_)
    #define rfFtell(i_FILE_)                        ftello64(i_FILE_)
#endif

#ifdef _MSC_VER

#error TODO

#else
i_INLINE_DECL int rfStat(struct RFstring* f, stat_rft* buffer)
{
    int ret = -1;
    char *cstr;

    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(f))) {
        goto end_pop;
    }
    ret = stat(cstr, (struct stat*)buffer);

end_pop:
    RFS_POP();
    return ret;
}
#endif

#else /* __ FILE_OFFSET_BITS != 64 */

//Fseek and Ftell definitions
#ifdef _MSC_VER
    #define rfFseek(i_FILE_,i_OFFSET_,i_WHENCE_)    fseek(i_FILE_,i_OFFSET_,i_WHENCE_)
    #define rfFtell(i_FILE_)                        ftell(i_FILE_)
#else
    #define rfFseek(i_FILE_,i_OFFSET_,i_WHENCE_)    fseeko(i_FILE_,i_OFFSET_,i_WHENCE_)
    #define rfFtell(i_FILE_)                        ftello(i_FILE_)
#endif

#endif

/**
** @defgroup RF_IOGRP I/O
** @addtogroup RF_IOGRP
** @{
**/


/**
 ** @brief Reads a UTF-8 file descriptor until end of line or EOF is found and
 **  returns a UTF-8 byte buffer
 **
 ** The file descriptor at @c f must have been opened in <b>binary</b> and not
 ** text mode. That means that if under
 ** Windows make sure to call fopen with "wb", "rb" e.t.c. instead of the 
 ** simple "w", "r" e.t.c. since the initial
 ** default value under Windows is text mode. Alternatively you can set the 
 ** initial value using _get_fmode() and
 ** _set_fmode(). For more information take a look at the msdn pages here:
 ** http://msdn.microsoft.com/en-us/library/ktss1a9b.aspx
 **
 ** Depending on the argument value given at @c eol the extra @c '\r' characters
 **  or any others for more exotic file line endings will be skipped and not 
 ** appear in the returned buffer.
 **
 ** @param[in] f The file descriptor to read
 ** @param[in] eol The End Of Line type that this file uses. Look @ref RFeol_mark
 **                for possible values
 ** @param[out] utf8 Give here a refence to an unitialized char* that will be
 ** allocated inside the function and contain the utf8 byte buffer. Needs to
 ** be freed by the caller explicitly later
 ** @param[out] byte_length Give an @c uint32_t here to receive the length
 **  of the @c utf8 buffer in bytes. This is also the number of bytes read
 ** from the file
 ** @param[out] buffer_size Give an @c uint32_t here to receive the capacity
 ** of the @c utf8 buffer in bytes
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 **  value in case the end of file with reading this line
 **
 ** @return Returns @c true for success and @c false for an error occuring.
 ** Even if there is an error the number of bytes read given by @c byteLength
 ** can be trusted.
 **/
i_DECLIMEX_ bool rf_file_read_line_utf8(FILE* f, 
                                        enum RFeol_mark eol,
                                        char** utf8,
                                        uint32_t* byte_length,
                                        uint32_t* buffer_size,
                                        char* eof);

/**
 ** @brief Reads a UTF-16 file descriptor until end of line or
 ** EOF is found and returns a UTF-8 byte buffer
 **
 ** The function works just like @ref rf_readline_utf8() but for UTF-16
 ** and as such has an extra endianess parameter.
 ** @param endianess The endianess of the file. Look @ref RFendianess
 ** for a listing of possible values
 ** @param bytes_read: The number of bytes succesfully read from the file
 ** descriptor. Can be NULL in which case nothing is returned.
 **/
i_DECLIMEX_ bool rf_file_read_line_utf16(FILE* f, enum RFeol_mark eol,
                                         char** utf8,
                                         uint32_t* byte_length, char* eof,
                                         uint32_t* bytes_read,
                                         enum RFendianess endianess);

/**
 ** @brief Reads a  UTF-32 file descriptor until end of line
 **  or EOF is found and returns a UTF-8 byte buffer
 ** 
 ** The function works just like @ref rf_readline_utf8() but for UTF-32
 ** @param endianess The endianess of the file. Look @ref RFendianess
 ** for a listing of possible values
 ** @param bytes_read: The number of bytes succesfully read from the file
 ** descriptor. Can be NULL in which case nothing is returned.
 **/
i_DECLIMEX_ bool rf_file_read_line_utf32(FILE* f, enum RFeol_mark eol,
                                         char** utf8,
                                         uint32_t* byte_length, char* eof,
                                         uint32_t* bytes_read,
                                         enum RFendianess endianess);


/**
 ** @brief Gets a number of bytes from a UTF8 file descriptor
 **
 ** This is a function that's similar to c library fgets but it also returns 
 ** the number of bytes read. Reads in from the file until @c num bytes
 ** have been read or new line or EOF character has been encountered.
 **
 ** The character pattern that shall be considered as newline signal depends
 **  on the value the user passes to the @c eol argument.
 **
 ** The function will read until @c num characters are read and if @c num
 ** would take us to the middle of a UTF16 character then the next character
 **  shall also be read
 ** and the function will return the number of bytes read.
 ** Since the function null terminates the buffer the given @c buff needs to be
 ** of at least @c num+5 size to cater for the worst case.
 **
 ** The final bytestream stored inside @c buff is in the endianess of the system.
 **
 ** If right after the last character read comes the EOF, the function
 ** shall detect so and assign @c true to @c eof.
 **
 ** The file descriptor at @c f must have been opened in <b>binary</b> and 
 ** not text mode. That means that if under
 ** Windows make sure to call fopen with "wb", "rb" e.t.c. instead of the
 ** simple "w", "r" e.t.c. since the initial
 ** default value under Windows is text mode. Alternatively you can set the
 **  initial value using _get_fmode() and
 ** _set_fmode(). For more information take a look at the msdn pages here:
 ** http://msdn.microsoft.com/en-us/library/ktss1a9b.aspx
 **
 ** @param[in] buff A buffer to be filled with the contents of the file.
 **  Should be of size at least @c num+5
 ** @param[in] num The maximum number of bytes to read from within the file
 **  NOT including the null terminating character(which in itelf is 2 bytes).
 **  Should be a multiple of 2
 ** @param[in] f A valid FILE descriptor from which to read the bytes
 ** @param[out] eof Pass a reference to a char to receive a true/false value
 **  for whether EOF has been reached.
 ** @param[in] eol The End Of Line type that this file uses.
 **                Look at @ref RFeol_mark for possible value
 ** @param[out] bytes_read Pass a pointer to a uint32_t to get the number of
 ** bytes read from this function. Can also be NULL in which
 ** case nothing is returned.
 **
 ** @return Returns @c true for success and @c false for an error occuring.
 ** Even if there is an error the number of bytes read can be trusted
 **/
i_DECLIMEX_ bool rf_file_read_bytes_utf8(char* buff, uint32_t num, FILE* f,
                                         char* eof,
                                         enum RFeol_mark eol,
                                         uint32_t* bytes_read);


/**
 ** @brief Gets a number of bytes from a UTF-16 file descriptor
 **
 ** This function is similar to @ref rf_file_read_bytes_utf8() only for UTF16 and as such
 ** has an extra endianess parameter
 ** @param endianess The endianess of the input file. For possible values look 
 **                  at @ref RFendianess
 **/
i_DECLIMEX_ bool rf_file_read_bytes_utf16(char* buff, uint32_t num, FILE* f,
                                          char* eof, enum RFeol_mark eol,
                                          uint32_t* bytes_read,
                                          enum RFendianess endianess);

/**
 ** @brief Gets a number of bytes from a UTF-32 file descriptor
 **
 ** This function is similar to @ref rf_file_read_bytes_utf8() only for UTF32
 ** @param endianess The endianess of the input file. For possible values look 
 **                  at @ref RFendianess
 **/
i_DECLIMEX_ bool rf_file_read_bytes_utf32(char* buff, uint32_t num, FILE* f,
                                          char* eof, enum RFeol_mark eol,
                                          uint32_t* bytes_read,
                                          enum RFendianess endianess);

/**
 ** @brief  Gets a unicode character from a UTF-8 file descriptor
 **
 ** This function attempts to assume a more modern fgetc() role for UTF-8 
 ** encoded files. Reads bytes from the File descriptor @c f until a full 
 ** UTF-8 unicode character has been read
 **
 ** After this function the file pointer will have moved either by @c 1, @c 2,
 **  @c 3 or @c 4 bytes if the return value is positive.
 **  You can see how much by checking the return value.
 **
 ** You shall need to provide an integer at @c c to contain either the decoded 
 ** unicode codepoint or the UTF-8 endoced byte depending on the value
 **  of the @c cp argument.
 **
 ** @param[in] f A valid FILE descriptor from which to read the bytes
 ** @param[in] c Pass an int that will receive either the unicode code point
 ** value or the UTF8 bytes depending on the value of the @c cp flag
 ** @param[in] cp A boolean flag. If @c true then the int passed at @c c will
 ** contain the unicode code point of the read character, so the UTF-8 
 ** will be decoded.
 ** If @c false the int passed at @c c will contain the value of the read 
 ** bytes in UTF-8 without any decoding
 ** @param[out] eof The function will use this as a flag to denote if EOF
 **  is reached.
 ** @return Returns the number of bytes read (either @c 1, @c 2, @c 3 or @c 4) 
 ** or a negative number if the function fails. If the EOF is reached then 
 ** @c eof will be set to @c true but the function will still return a negative.
 **/
i_DECLIMEX_ int rf_file_read_char_utf8(FILE* f, uint32_t *c, bool cp, char* eof);


/**
 ** @brief  Gets a unicode character from a UTF-16 file descriptor
 **
 ** This function acts just like @ref rf_file_read_char_utf8() but for UTF16
 ** The main difference is that it also takes endianess into account
 ** and as such has en extra argument to that effect.
 ** @param endianess The endianess of the input file. For possible values look 
 **                  at @ref RFendianess
 **/
i_DECLIMEX_ int rf_file_read_char_utf16(FILE* f, uint32_t *c, bool cp,
                                        enum RFendianess endianess, char* eof);

/**
 ** @brief  Gets a unicode character from a UTF-32  file descriptor
 **
 ** This function acts just like @ref rf_file_read_char_utf8() but for UTF32 with the
 ** difference that it has no option to return the decoded character since it 
 ** makes no sense for UTF32 and that it accepts an endianess argument.
 ** @param endianess The endianess of the input file. For possible values look 
 **                  at @ref RFendianess
 **/
i_DECLIMEX_ int rf_file_read_char_utf32(FILE* f, uint32_t *c,
                                        enum RFendianess endianess,
                                        char* eof);

/**
 ** @brief Moves a unicode character backwards in a UTF-8 file stream
 **
 ** The file descriptor at @c f must have been opened in <b>binary</b> and not
 ** text mode. That means that if under Windows make sure to call fopen with 
 ** "wb", "rb" e.t.c. instead of the simple "w", "r" e.t.c. since the initial
 ** default value under Windows is text mode. Alternatively you can set the
 **  initial value using _get_fmode() and _set_fmode(). For more information
 **  take a look at the msdn pages here:
 **  http://msdn.microsoft.com/en-us/library/ktss1a9b.aspx
 **
 ** @param f The file stream
 ** @param c Returns the character we moved back to as a unicode codepoint
 ** @return Returns either the number of bytes moved backwards for success
 ** (either @c 4, @c 3, @c 2 or @c 1) or negative number for error
 **/
i_DECLIMEX_ int rf_file_move_back_char_utf8(FILE* f, uint32_t *c);

/**
 ** @brief Moves a unicode character backwards in a UTF-16 file stream
 **
 ** This function acts just like @ref rf_file_move_back_char_utf8() except that it is 
 ** implemented for UTF-16 encoded byte streams and that it also accepts
 ** an endianess arument.
 ** @param endianess The endianess of the input file. For possible values look 
 **                  at @ref RFendianess
 ** @return Returns either @c 2 or @c 4 for success and negative for failure
 **/
i_DECLIMEX_ int rf_file_move_back_char_utf16(FILE* f, uint32_t *c,
                                             enum RFendianess endianess);

/**
 ** @brief Moves a unicode character backwards in a little endian UTF-32 file stream
 **
 ** This function acts just like @ref rf_file_move_back_char_utf8() except that it is 
 ** implemented for UTF-32 encoded byte streams and that it also accepts
 ** an endianess arument.
 ** @param endianess The endianess of the input file. For possible values look 
 **                  at @ref RFendianess
 ** @return Returns either @c 4 for success or negative for failure
 **/
i_DECLIMEX_ int rf_file_move_back_char_utf32(FILE* f, uint32_t *c,
                                             enum RFendianess endianess);



//! @} End of I/O group

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end
