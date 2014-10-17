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

/*------------- Corrensponding Header inclusion -------------*/
#include <String/rf_str_filesx.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_common.h> // for RFS_()
#include <String/rf_str_manipulationx.h> //for rf_stringx_append and others
#include <String/rf_str_traversalx.h> //for rf_stringx_reset and others
#include "rf_str_files.ph" //for some macros
#include <String/rf_str_files.h> //for rf_string_from_file_init e.t.c.
#include <IO/rf_file.h> //for rfReadLine family  of functions
#include "rf_str_manipulation.ph" //for rf_stringxget_append()
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h> //for error logging macros
#include <Utils/memory.h> //for memory allocation
/*------------- Libc inclusion -------------*/
#include <stdio.h> //for FILE*
#include <string.h> //for memcpy()
/*------------- End of includes -------------*/




// Allocates and returns an RFstringx from a file descriptor
struct RFstringx* rf_stringx_from_file_create(
    FILE* f, char* eof,
    enum RFeol_mark eol,
    enum RFtext_encoding encoding,
    enum RFendianess endianess
)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_from_file_init(ret, f, eof, eol, encoding, endianess))
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}

bool rf_stringx_from_file_init(struct RFstringx* str,
                               FILE* f, char* eof,
                               enum RFeol_mark eol,
                               enum RFtext_encoding encoding,
                               enum RFendianess endianess)
{
    if(!rf_string_from_file_init(&str->INH_String, f, eof, eol,
                                 encoding, endianess, &str->bSize))
    {
        return false;
    }
    //success
    str->bIndex = 0;
    return true;
}

bool rf_stringx_from_file_assign(struct RFstringx* str,
                                 FILE* f, char* eof,
                                 enum RFeol_mark eol,
                                 enum RFtext_encoding encoding,
                                 enum RFendianess endianess)
{
    RF_ASSERT(str, "got null string in function");
    UTF_FILE_READLINE(f, eol, eof, "assign")
    //success
    //assign it to the string
    RF_STRINGX_REALLOC(str, utf8ByteLength, ret = false; goto cleanup);
    memcpy(rf_string_data(str), utf8, utf8ByteLength);
    rf_string_length_bytes(str) = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

bool rf_stringx_from_file_append(struct RFstringx* str,
                                 FILE* f, char* eof,
                                 enum RFeol_mark eol,
                                 enum RFtext_encoding encoding,
                                 enum RFendianess endianess)
{
    RF_ASSERT(str, "got null string in function");
    UTF_FILE_READLINE(f, eol, eof, "append")
    //append the utf8 to the given string
    if(!rf_stringx_generic_append(str, utf8, utf8ByteLength))
    {
        RF_ERROR("Failed to append a utf8 buffer to a stringX");
        ret = false;
    }
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}
