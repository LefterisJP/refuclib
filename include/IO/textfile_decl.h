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
#ifndef RF_TEXTFILE_DECL_H
#define RF_TEXTFILE_DECL_H


/*------------- Outside Module inclusion -------------*/
#include <Definitions/types.h> //for fixed size types
#include <Definitions/retcodes.h> //for bool
#include <String/rf_str_decl.h> //for RFstring
/*------------- libc inclusion --------------*/
#include <stdio.h> //for FILE*
/*------------- End of includes -------------*/


/**
** @internal
** @date 23/05/12
** @author Lefteris
** @endinternal
** @brief TextFile handler
**
** This is a TextFile handler that allows for the manipulation of files that contain
** text in any of the UTF encodings.
**
** It allows for manipulation of a text file without placing the whole file in memory
** and contains many different functions for moving inside the file in regards to specific line positions
** or even retrieving said lines themselves inside an @ref RF_String.
** For individual line parsing it is recommended to use the @ref RFstring and @ref RFstringx functions.
**/
typedef struct RFtextfile
{
    //! The file descriptor
    FILE* f;
    //! The name of the file
    struct RFstring name;
    //! The current line number. Basically represents the line that was last
    //! read by any of the file's functions
    uint64_t line;
    //! The encoding. Can be one of @c RF_UTF8, @c RF_UTF16 and @c RF_UTF32
    char encoding;
    //! The endianess of the file. Can be one of @c RC_BIG_ENDIAN or
    //! @c RF_LITTLE_ENDIAN
    char endianess;
    //! A boolean flag denoting if the end of file was reached
    char eof;
    //! the mode of textfile opening. Can be one of @c RF_FILE_READ, @c 
    //!RF_FILE_WRITE and @c RF_FILE_READWRITE
    char mode;
    //! the previous i/o operation on the file. Can be one of @c RF_FILE_READ,
    //! @c RF_FILE_WRITE
    char previousOp;
    //! A boolean flag denoting if the TextFile has a Byte Order Mark in the beginning or not
    char hasBom;
    //! A flag denoting what kind of EOL pattern this particular text file observes
    char eol;
}RFtextfile;


#endif//include guards end
