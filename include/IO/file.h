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
** --IO/file.h
** This header defines functions that operate on FILE descriptors and
** also those that can open other processes
**
---------------------For internal library include make sure to have----------------------------
#include <stdio.h> //for FILE*
#include <Definitions/types.h> //for fixed size types needed in various places
#include <Definitions/imex.h> //for the import export macro
#include <IO/file.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef REFU_IO_FILE_H
#define REFU_IO_FILE_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


///Fseek and Ftelll definitions
#ifdef _MSC_VER
    #define rfFseek(i_FILE_,i_OFFSET_,i_WHENCE_)    _fseeki64(i_FILE_,i_OFFSET_,i_WHENCE_)
    #define rfFtell(i_FILE_)                        _ftelli64(i_FILE_)
#else
    #define rfFseek(i_FILE_,i_OFFSET_,i_WHENCE_)    fseeko64(i_FILE_,i_OFFSET_,i_WHENCE_)
    #define rfFtell(i_FILE_)                        ftello64(i_FILE_)
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
 ** @param[in] eol The End Of Line type that this file uses. Legal values are:
 ** + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as
 **      the end of line signal
 ** + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as
 **      the end of line signal
 ** + @c RF_EOL_CR: For the old Mac OS style of line endings taking "\r" as
 **      the end of line signal
 **
 ** @param[out] utf8 Give here a refence to an unitialized char* that will be
 ** allocated inside the function and contain the utf8 byte buffer. Needs to
 ** be freed by the caller explicitly later
 ** @param[out] byteLength Give an @c uint32_t here to receive the length
 **  of the @c utf8 buffer in bytes. This is also the number of bytes read
 ** from the file
 ** @param[out] bufferSize Give an @c uint32_t here to receive the capacity
 ** of the @c utf8 buffer in bytes
 ** @param[out] eof Pass a pointer to a char to receive a true or false
 **  value in case the end of file with reading this line
 **
 ** @return Returns @c true for success and @c false for an error occuring.
 ** Even if there is an error the number of bytes read given by @c byteLength
 ** can be trusted.
 **/
i_DECLIMEX_ char rfFReadLine_UTF8(FILE* f, char eol, char** utf8,
                                  uint32_t* byteLength,
                                  uint32_t* bufferSize, char* eof);

/**
 ** @brief Reads a UTF-16 file descriptor until end of line or
 ** EOF is found and returns a UTF-8 byte buffer
 **
 ** The function works just like @ref rfReadLine_UTF8() but for UTF-16
 ** and as such has an extra endianess parameter.
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 ** @param bytes_read: The number of bytes succesfully read from the file
 ** descriptor
 **/
i_DECLIMEX_ char rfFReadLine_UTF16(FILE* f, char eol, char** utf8,
                                   uint32_t* byteLength, char* eof,
                                   uint32_t* bytes_read, int endianess);

/**
 ** @brief Reads a  UTF-32 file descriptor until end of line
 **  or EOF is found and returns a UTF-8 byte buffer
 ** 
 ** The function works just like @ref rfReadLine_UTF8() but for UTF-32
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 ** @param bytes_read: The number of bytes succesfully read from the file
 ** descriptor
 **/
i_DECLIMEX_ char rfFReadLine_UTF32(FILE* f, char eol, char** utf8,
                                   uint32_t* byteLength, char* eof,
                                   uint32_t* bytes_read, int endianess);


/**
 ** @brief Gets a number of bytes from a UTF-32 file descriptor
 **
 ** This function is similar to @ref rfFgets_UTF8() only for UTF32
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 **/
i_DECLIMEX_ char rfFgets_UTF32(char* buff, uint32_t num, FILE* f,
                               char* eof, char eol, uint32_t* bytes_read,
                               int endianess);

/**
 ** @brief Gets a number of bytes from a UTF-16 file descriptor
 **
 ** This function is similar to @ref rfFgets_UTF8() only for UTF16 and as such
 ** has an extra endianess parameter
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 **/
i_DECLIMEX_ char rfFgets_UTF16(char* buff, uint32_t num, FILE* f,
                                 char* eof, char eol, uint32_t* bytes_read,
                                 int endianess);


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
 ** @param[in] eol The End Of Line type that this file uses. Legal values are:
 ** + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as
 **      the end of line signal
 ** + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as
 **      the end of line signal
 ** + @c RF_EOL_CR: For the old Mac OS style of line endings taking "\r" as
 **      the end of line signal

 ** @param[out] bytes_read Pass a pointer to a uint32_t to get the number of
 ** bytes read from this function. Even if an error occured this number should
 ** contain how many bytes were read until that point.
 **
 ** @return Returns @c true for success and @c false for an error occuring.
 ** Even if there is an error the number of bytes read can be trusted
 **/
i_DECLIMEX_ char rfFgets_UTF8(char* buff, uint32_t num, FILE* f,
                              char* eof, char eol, uint32_t* bytes_read);

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
i_DECLIMEX_ int rfFgetc_UTF8(FILE* f, uint32_t *c, char cp, char* eof);


/**
 ** @brief  Gets a unicode character from a UTF-16 file descriptor
 **
 ** This function acts just like @ref rfFgetc_UTF8() but for UTF16
 ** The main difference is that it also takes endianess into account
 ** and as such has en extra argument to that effect.
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 **/
i_DECLIMEX_ int rfFgetc_UTF16(FILE* f, uint32_t *c, char cp,
                                int endianess, char* eof);

/**
 ** @brief  Gets a unicode character from a UTF-32  file descriptor
 **
 ** This function acts just like @ref rfFgetc_UTF8() but for UTF32 with the
 ** difference that it has no option to return the decoded character since it 
 ** makes no sense for UTF32 and that it accepts an endianess argument.
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 **/
i_DECLIMEX_ int rfFgetc_UTF32(FILE* f, uint32_t *c, int endianess, char* eof);


/**
 ** @brief Moves a unicode character backwards in a little endian UTF-32 file stream
 **
 ** This function acts just like @ref rfFback_UTF8() except that it is 
 ** implemented for UTF-32 encoded byte streams and that it also accepts
 ** an endianess arument.
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 ** @return Returns either @c 4 for success or negative for failure
 **/
i_DECLIMEX_ int rfFback_UTF32(FILE* f, uint32_t *c, int endianess);

/**
 ** @brief Moves a unicode character backwards in a UTF-16 file stream
 **
 ** This function acts just like @ref rfFback_UTF8() except that it is 
 ** implemented for UTF-16 encoded byte streams and that it also accepts
 ** an endianess arument.
 ** @param endianess Can be either:
 ** + @c RF_BIG_ENDIAN: If the file stream you want to read is encoded in 
 **   big endian 
 ** + @c RF_LITTLE_ENDIAN: If the file stream you want to read is encoded in
 **  little endian
 ** @return Returns either @c 2 or @c 4 for success and negative for failure
 **/
i_DECLIMEX_ int rfFback_UTF16(FILE* f, uint32_t *c, int endianess);

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
i_DECLIMEX_ int rfFback_UTF8(FILE* f, uint32_t *c);

/**
 ** @brief Opens another process as a pipe
 **
 ** This function is a cross-platform popen wrapper. In linux it uses popen and in Windows it uses
 ** _popen.
 ** @lmsFunction
 ** @param command The string with the command to execute. Is basically the name of the program/process you want to spawn
 ** with its full path and its parameters. @inhtype{String,StringX} @tmpSTR
 ** @param mode The mode you want the pipe to work in. There are two possible values:
 ** + @c "r" The calling process can read the spawned command's standard output via the returned stream.
 ** + @c "w" The calling process can write to the spawned command's standard input via the returned stream.
 **
 ** Anything else will result in an error
 ** @return For success popen will return a FILE descriptor that can be used to either read or write from the pipe.
 ** If there was an error @c 0 is returned and an error is logged.
 **
 **/
i_DECLIMEX_ FILE* rfPopen(void* command, const char* mode);


/**
 ** @brief Closes a pipe
 **
 ** This function is a cross-platform wrapper for pclose. It closes a file descriptor opened with @ref rfPopen() and
 ** returns the exit code of the process that was running
 ** @param stream The file descriptor of the pipe returned by @ref rfPopen() that we want to close
 ** @return Returns the exit code of the process or -1 if there was an error
 **
 **/
i_DECLIMEX_ int rfPclose(FILE* stream);

//! @} End of I/O group

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end
