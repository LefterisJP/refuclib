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
#include "files.ph" //for some macros

//for rfString_FInit
#include <String/files.h>

//Needed for IO/file.h
#include <Definitions/inline.h> //for inline definitions
#include <IO/common.h> //for stat_rft
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <IO/file.h> //for rfReadLine family  of functions

//for rfStringXGet_Append()
#include "manipulation.ph"

/*------------- Outside Module inclusion -------------*/
//for error logging macros
    #include <Utils/log.h>
//for memory allocation macros
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
    if(!rfStringX_FInit(ret, f, eof, eol, encoding, endianess))
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}

// Initializes RF_String from a file descriptor
bool rfStringX_FInit(RF_StringX* str, FILE* f, char* eof, char eol,
                     int encoding, int endianess)
{
    if(!rfString_FInit(&str->INH_String, f, eof, eol,
                       encoding, endianess, &str->bSize))
    {
        return false;
    }
    //success
    str->bIndex = 0;
    return true;
}

//Assigns to a StringX from file parsing
bool rfStringX_FAssign(RF_StringX* str, FILE* f, char* eof, char eol,
                       int encoding, int endianess)
{
    UTF_FILE_READLINE(f, eol, eof, "assign")
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength)
    {
        str->bSize = (utf8ByteLength)*2;
        RF_REALLOC_JMP(rfString_Data(str), char, str->bSize,
                                     ret = false, cleanup);
    }
    memcpy(rfString_Data(str), utf8, utf8ByteLength);
    rfString_ByteLength(str) = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

//Appends to a String from UTF-8 file parsing
bool rfStringX_FAppend(RF_StringX* str, FILE* f, char* eof, char eol,
                      int encoding, int endianess)
{
    UTF_FILE_READLINE(f, eol, eof, "append")
    //append the utf8 to the given string
    if(!rfStringXGEN_Append(str, utf8, utf8ByteLength))
    {
        RF_ERROR("Failed to append a utf8 buffer to a stringX");
        ret = false;
    }
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}
