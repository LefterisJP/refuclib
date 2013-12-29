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
#include <String/rf_str_files.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_conversion.h> //for unicode conversions
#include <String/rf_str_manipulation.h> //for rf_string_append()
#include <String/rf_str_common.h> //for RFS_()
#include "rf_str_files.ph" //for some macros
#include "rf_str_common.ph" //for some useful string inline functions
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h> //for error logging
#include <Utils/memory.h> //for memory allocation
#include <IO/rf_file.h> //for rfReadLine family  of functions
#include <Utils/localscope.h> //for local scope macros
#include <Utils/sanity.h> //for sanity macros
/*------------- libc inclusion --------------*/
#include <stdio.h> //for FILE*
/*------------- End of includes -------------*/
 


struct RFstring* rf_string_from_file_create(FILE* f, char* eof,
                                            enum RFeol_mark eol,
                                            enum RFtext_encoding encoding,
                                            enum RFendianess endianess,
                                            unsigned int* buff_size)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    if(!rf_string_from_file_init(ret, f, eof, eol, encoding, endianess, buff_size))
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}

bool rf_string_from_file_init(struct RFstring* str, FILE* f, char* eof,
                              enum RFeol_mark eol,
                              enum RFtext_encoding encoding,
                              enum RFendianess endianess,
                              unsigned int* ret_buff_size)
{
    uint32_t buff_size;
    RF_ASSERT(str);
    /* sanity checks for the rest of the arguments are performed further down */
    switch(encoding)
    {
        case RF_UTF8:
            if(!rf_file_read_line_utf8(f, eol, &rf_string_data(str),
                                 &rf_string_length_bytes(str), &buff_size, eof))
            {
                RF_ERROR("Failed to initialize String from a UTF-8 file");
                return false;
            }
            break;
        case RF_UTF16:
            if(!rf_file_read_line_utf16(f, eol, &rf_string_data(str),
                                  &rf_string_length_bytes(str), eof, &buff_size,
                                  endianess))
            {
                RF_ERROR("Failure to initialize a String from reading"
                         " a UTF-16 file");
                return false;
            }
            break;
        case RF_UTF32:
            if(!rf_file_read_line_utf32(f, eol, &rf_string_data(str),
                                  &rf_string_length_bytes(str), eof,
                                  &buff_size, endianess))
            {
                RF_ERROR("Failure to initialize a String from reading"
                         " a UTF-32 file");
                return false;
            }
            break;
#if RF_OPTION_DEBUG
        default:
            RF_WARNING("Provided an illegal encoding value to function "
                     "rf_string_init_f()");
            return false;
            break;
#endif
    }
    if(ret_buff_size)
    {
        *ret_buff_size = buff_size;
    }
    return true;
}

bool rf_string_from_file_assign(struct RFstring* str,
                                FILE* f, char* eof,
                                enum RFeol_mark eol,
                                enum RFtext_encoding encoding,
                                enum RFendianess endianess)
{
    RF_ASSERT(str);
    UTF_FILE_READLINE(f, eol, eof, "assign")
    //success
    //assign it to the string
    if(rf_string_length_bytes(str) <= utf8ByteLength)
    {
        RF_REALLOC_JMP(rf_string_data(str), char, utf8ByteLength,
                       ret = false, cleanup);
    }
    memcpy(rf_string_data(str), utf8, utf8ByteLength);
    rf_string_length_bytes(str) = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

bool rf_string_from_file_append(struct RFstring* str, FILE* f,
                                char* eof,
                                enum RFeol_mark eol,
                                enum RFtext_encoding encoding,
                                enum RFendianess endianess)
{
    UTF_FILE_READLINE(f, eol, eof, "append")
    RF_REALLOC_JMP(rf_string_data(str), char,
                   rf_string_length_bytes(str) + utf8ByteLength,
                   ret = false, cleanup
    );
    rf_string_generic_append(str, utf8, utf8ByteLength);

cleanup:
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}

//Writes a string to a file in the given encoding
bool rf_string_fwrite(void* s, FILE* f,
                      enum RFtext_encoding encoding,
                      enum RFendianess endianess)
{
    uint32_t *utf32,length;
    uint16_t* utf16;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(s);

    //depending on the encoding
    switch(encoding)
    {
        case RF_UTF8:
            if(fwrite(
                   rf_string_data(s), 1,
                   rf_string_length_bytes(s), f) != rf_string_length_bytes(s))
            {
                break;//and go to error logging
            }
            goto cleanup1;//success
        break;
        case RF_UTF16:
            if((utf16 = rf_string_to_utf16(s, &length)) == NULL)
            {
                ret = false;
                RF_ERROR("Error in converting a string to UTF-16 LE");
                goto cleanup1;
            }
            rf_process_byte_order_u16A(utf16, length, endianess);
            if(fwrite(utf16, 2, length, f) != length)
            {
                free(utf16);
                break;//and go to error logging
            }
            free(utf16);
            goto cleanup1;//success
        break;
        case RF_UTF32:
            if((utf32 = rf_string_to_utf32(s, &length)) == NULL)
            {
                ret = false;
                RF_ERROR("Error in converting a string to LE UTF-32");
                goto cleanup1;
            }
            rf_process_byte_order_u32A(utf32, length, endianess);
            if(fwrite(utf32, 4, length, f) != length)
            {
                free(utf32);
                break;//and go to error logging
            }
            free(utf32);
            goto cleanup1;//success
        break;
        default:
            RF_ERROR("Illegal encoding value given to the function");
            ret = false;
            goto cleanup1;
            break;
    }
    //if we get here it means an error, and we log it
    RF_ERROR("Writting a string to a file failed due to "
             "fwrite errno %d", errno);
    ret = false;

cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
