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


/*------------- Corrensponding Header inclusion -------------*/
#include <stdio.h> //for FILE*
#include <IO/common.h> //for RF_EOL macros
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/unicode.h> //for the unicode macros RF_UTF8 and friends
#include <String/string_decl.h>//for RF_String
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/files.h>
/*------------- Module related inclusion -------------*/
#include <String/conversion.h> //for unicode conversions
#include <String/manipulation.h> //for rfString_Append()
#include <String/common.h> //for RFS_()
#include "files.ph" //for some macros
#include "common.ph" //for some useful string inline functions
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for Endianess swapping functions
    #include <Utils/endianess.h>
//Needed for IO/file.h
    #include <Definitions/inline.h> //for inline definitions
    #include <IO/common.h> //for stat_rft
    #include <IO/file.h> //for rfReadLine family  of functions
//for local scope macros
    #include <Utils/localscope.h>//for local scope macros
/*------------- End of includes -------------*/
 


RF_String* rfString_FCreate(FILE* f, char* eof, char eol,
                            int encoding, int endianess,
                            unsigned int* buff_size)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(!rfString_FInit(ret, f, eof, eol, encoding, endianess, buff_size))
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}

bool rfString_FInit(RF_String* str, FILE* f, char* eof, char eol,
                    int encoding, int endianess, unsigned int* ret_buff_size)
{
    uint32_t buff_size;
    switch(encoding)
    {
        case RF_UTF8:
            if(!rfFReadLine_UTF8(f, eol, &rfString_Data(str),
                                 &rfString_ByteLength(str), &buff_size, eof))
            {
                RF_ERROR("Failed to initialize String from a UTF-8 file");
                return false;
            }
            break;
        case RF_UTF16:
            if(!rfFReadLine_UTF16(f, eol, &rfString_Data(str),
                                  &rfString_ByteLength(str), eof, &buff_size,
                                  endianess))
            {
                RF_ERROR("Failure to initialize a String from reading"
                         " a UTF-16 file");
                return false;
            }
            break;
        case RF_UTF32:
            if(!rfFReadLine_UTF32(f, eol, &rfString_Data(str),
                                  &rfString_ByteLength(str), eof,
                                  &buff_size, endianess))
            {
                RF_ERROR("Failure to initialize a String from reading"
                         " a UTF-32 file");
                return false;
            }
            break;
#if RF_OPTION_DEBUG
        default:
            RF_ERROR("Provided an illegal encoding value to function "
                     "rfString_Init_f()");
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

bool rfString_FAssign(RF_String* str, FILE* f, char* eof, char eol,
                      int encoding, int endianess)
{
    UTF_FILE_READLINE(f, eol, eof, "assign")
    //success
    //assign it to the string
    if(rfString_ByteLength(str) <= utf8ByteLength)
    {
        RF_REALLOC_JMP(rfString_Data(str), char, utf8ByteLength,
                       ret = false, cleanup);
    }
    memcpy(rfString_Data(str), utf8, utf8ByteLength);
    rfString_ByteLength(str) = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

bool rfString_FAppend(RF_String* str, FILE* f, char* eof, char eol,
                      int encoding, int endianess)
{
    UTF_FILE_READLINE(f, eol, eof, "append")
    RF_REALLOC_JMP(rfString_Data(str), char,
                   rfString_ByteLength(str) + utf8ByteLength,
                   ret = false, cleanup
    );
    rfStringGEN_Append(str, utf8, utf8ByteLength);

cleanup:
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}

//Writes a string to a file in the given encoding
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
bool rfString_Fwrite(void* s, FILE* f, int encoding, int endianess)
#else
bool i_rfString_Fwrite(void* s, FILE* f, int encoding, int endianess)
#endif
{
    uint32_t *utf32,length;
    uint16_t* utf16;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();

    //depending on the encoding
    switch(encoding)
    {
        case RF_UTF8:
            if(fwrite(
                   rfString_Data(s), 1,
                   rfString_ByteLength(s), f) != rfString_ByteLength(s))
            {
                break;//and go to error logging
            }
            goto cleanup1;//success
        break;
        case RF_UTF16:
            if((utf16 = rfString_ToUTF16(s, &length)) == NULL)
            {
                ret = false;
                RF_ERROR("Error in converting a string to UTF-16 LE");
                goto cleanup1;
            }
            rfProcessByteOrderUSA(utf16, length, endianess);
            if(fwrite(utf16, 2, length, f) != length)
            {
                free(utf16);
                break;//and go to error logging
            }
            free(utf16);
            goto cleanup1;//success
        break;
        case RF_UTF32:
            if((utf32 = rfString_ToUTF32(s, &length)) == NULL)
            {
                ret = false;
                RF_ERROR("Error in converting a string to LE UTF-32");
                goto cleanup1;
            }
            rfProcessByteOrderUIA(utf32, length, endianess);
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
