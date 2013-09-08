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
#include <String/stringx_decl.h>//for RF_StringX
#include <String/filesx.h>
/*------------- Module related inclusion -------------*/
#include <String/common.h> // for RFS_()
#include <String/manipulationx.h> //for rfStringX_Append and others
#include <String/traversalx.h> //for rfStringX_Reset and others

//Needed for IO/file.h
#include <Definitions/inline.h> //for inline definitions
#include <IO/common.h> //for stat_rft
#include <String/conversion.h> //for rfString_Cstr()
#include <IO/file.h> //for rfReadLine family  of functions

/*------------- Outside Module inclusion -------------*/
//for error logging macros
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since ERROR macros use argument counting
    #include <Threads/common.h> //for rfThread_GetID()
    #include <errno.h> //for error reporting that needs it
    #include <Utils/error.h>
//for memory allocation macros
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <Utils/memory.h> //for refu memory allocation
//for the endianess macros
#include <Utils/endianess.h> //for the endianess macros
/*------------- Libc inclusion -------------*/
#include <string.h> //for memcpy()
/*------------- End of includes -------------*/




// Allocates and returns an RF_StringX from a file descriptor
RF_StringX* rfStringX_FCreate(FILE* f, char* eof, char eol,
                              int encoding, int endianess)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(rfStringX_FInit(ret, f, eof, eol, encoding, endianess) == false)
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}

// Initializes RF_String from a file descriptor
char rfStringX_FInit(RF_StringX* str, FILE* f, char* eof, char eol,
                     int encoding, int endianess)
{
    switch(encoding)
    {
        case RF_UTF8:
            if(!rfFReadLine_UTF8(f, eol, &str->INH_String.bytes,
                                 &str->INH_String.byteLength, &str->bSize, eof))
            {
                RF_ERROR(0, "Failed to initialize a StringX from a UTF-8 file");
                return false;
            }
            break;
        case RF_UTF16:
            if(!rfFReadLine_UTF16(f, eol, &str->INH_String.bytes,
                                  &str->INH_String.byteLength,
                                  eof, &str->bSize, endianess))
            {
                RF_ERROR(0, "Failure to initialize a StringX from reading"
                         " a UTF-16 file");
                return false;
            }
            break;
        case RF_UTF32:
            if(!rfFReadLine_UTF32(f, eol, &str->INH_String.bytes,
                                  &str->INH_String.byteLength, eof,
                                  &str->bSize, endianess))
            {
                RF_ERROR(0, "Failure to initialize a StringX from reading"
                         " a UTF-32 file");
                return false;
            }
            break;
#if RF_OPTION_DEBUG
        default:
            RF_WARNING(0, "Provided an illegal encoding value to function "
                       "rfStringX_Init_f()");
            return false;
            break;
#endif
    }

    //success
    str->bIndex = 0;
    return true;
}

//Assigns to a StringX from file parsing
char rfStringX_FAssign(RF_StringX* str, FILE* f, char* eof, char eol,
                       int encoding, int endianess)
{
    uint32_t utf8ByteLength, bytes_read;
    char* utf8 = 0, ret = true;
    switch(encoding)
    {
        case RF_UTF8:
            if(!rfFReadLine_UTF8(f, eol, &utf8, &utf8ByteLength, &bytes_read, eof))
            {
                RF_ERROR(0, "Failed to assign the contents of a UTF-8 file "
                         "to a StringX");
                return false;
            }
            break;
        case RF_UTF16:
            if(!rfFReadLine_UTF16(f, eol, &utf8, &utf8ByteLength, eof,
                                  &bytes_read, endianess))
            {
                RF_ERROR(0,"Failure to assign the contents of a "
                         "UTF-16 file to a StringX");
                return false;
            }
            break;
        case RF_UTF32:
            if(!rfFReadLine_UTF32(f, eol, &utf8, &utf8ByteLength, eof,
                                  &bytes_read, endianess))
            {
                RF_ERROR(0, "Failure to assign to a StringX from reading "
                         "a UTF-32 file");
                return false;
            }
            break;
#if RF_OPTION_DEBUG
        default:
            RF_WARNING(0, "Provided an illegal encoding value to function "
                       "rfStringX_Assign_f()");
            return false;
            break;
#endif
    }
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC_JMP(str->INH_String.bytes, char, str->bSize,
                       ret = false, cleanup);
    }
    memcpy(str->INH_String.bytes, utf8, utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

//Appends to a String from UTF-8 file parsing
char rfStringX_FAppend(RF_StringX* str, FILE* f, char* eof, char eol,
                      int encoding, int endianess)
{
    uint32_t utf8ByteLength, bytes_read;
    char* utf8 = 0, ret = true;

    switch(encoding)
    {
        case RF_UTF8:
            if(!rfFReadLine_UTF8(f, eol, &utf8,
                                 &utf8ByteLength, &bytes_read, eof))
            {
                RF_ERROR(0, "Failed to append to a StringX from reading "
                         "a UTF-8 file");
                return false;
            }
            break;
        case RF_UTF16:
            if(!rfFReadLine_UTF16(f, eol, &utf8, &utf8ByteLength, eof,
                                  &bytes_read, endianess))
            {
                RF_ERROR(0,"Failure to append the contents of a "
                         "UTF-16 file to a StringX");
                return false;
            }
            break;
        case RF_UTF32:
            if(!rfFReadLine_UTF32(f, eol, &utf8, &utf8ByteLength, eof,
                                  &bytes_read, endianess))
            {
                RF_ERROR(0, "Failure to append to a StringX from reading "
                         "a UTF-32 file");
                return false;
            }
            break;
#if RF_OPTION_DEBUG
        default:
            RF_WARNING(0, "Provided an illegal encoding value to function "
                       "rfString_Append_f()");
            return false;
            break;
#endif
    }
    //append the utf8 to the given string
    if(rfStringX_Append(str, RFS_(utf8)) == false)
    {
        RF_ERROR(0, "Failed to append a utf8 buffer to a stringX");
        ret = false;
    }
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}
