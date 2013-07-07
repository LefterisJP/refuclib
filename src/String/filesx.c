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


//*---------------------Corrensponding Header inclusion---------------------------------
#include <stdio.h> //for FILE*
#include <IO/common.h> //for RF_EOL macros
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/unicode.h> //for the unicode macros RF_UTF8 and friends
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include <String/filesx.h>
//*---------------------Module related inclusion----------------------------------------
#include <String/common.h> // for RFS_()
#include <String/manipulationx.h> //for rfStringX_Append and others
#include <String/traversalx.h> //for rfStringX_Reset and others
#include <IO/file.h> //for rfReadLine family  of functions
//*---------------------Outside module inclusion----------------------------------------
//for error logging macros
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation macros
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <Utils/memory.h> //for refu memory allocation
//for the endianess macros
#include <Utils/endianess.h> //for the endianess macros
//*-----------------------------Libc inclusion------------------------------------------
#include <string.h> //for memcpy()
//*----------------------------End of Includes------------------------------------------


//Allocates and returns an extended string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
RF_StringX* rfStringX_Create_fUTF8(FILE* f, char* eof,char eol)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(rfStringX_Init_fUTF8(ret, f, eof, eol) < 0)
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}
//Initializes an extended string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
int32_t rfStringX_Init_fUTF8(RF_StringX* str,FILE* f,char* eof,char eol)
{
    int32_t bytesN;
    if((bytesN = rfFReadLine_UTF8(
            f, eol, &str->INH_String.bytes, &str->INH_String.byteLength,
            &str->bSize,eof)) < 0)
    {
        RETURN_LOG_ERROR("Failed to initialize StringX from a UTF-8 file",
                         bytesN)
    }

    //success
    str->bIndex = 0;
    return bytesN;
}
//Assigns to an extended String from UTF-8 file parsing
int32_t rfStringX_Assign_fUTF8(RF_StringX* str,FILE*f,char* eof,char eol)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN = rfFReadLine_UTF8(
            f, eol, &utf8, &utf8ByteLength, &utf8BufferSize, eof)) < 0)
    {
        RETURN_LOG_ERROR("Failed to assign the contents of a UTF-8 file "
                         "to a StringX", bytesN)
    }

    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC_JMP(str->INH_String.bytes, char, str->bSize,
                       bytesN = RE_REALLOC_FAILURE, cleanup);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;

  cleanup:
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
//Appends to an extended String from UTF-8 file parsing
int32_t rfStringX_Append_fUTF8(RF_StringX* str, FILE*f,
                               char* eof, char eol)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN = rfFReadLine_UTF8(f, eol, &utf8, &utf8ByteLength,
                                  &utf8BufferSize,eof)) < 0)
    {
        RETURN_LOG_ERROR("Failed to assign the contents of a UTF-8 file "
                         "to a StringX", bytesN)
    }

   //append the utf8 to the given string
   if(rfStringX_Append(str,RFS_(utf8)) == false)
   {
       //means error
       bytesN = RE_STRING_APPEND;
   }
   //free the file's utf8 buffer
   free(utf8);
   return bytesN;
}

//Allocates and returns an extended string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_StringX* rfStringX_Create_fUTF16(FILE* f, char endianess,
                                    char* eof, char eol)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(rfStringX_Init_fUTF16(ret, f, endianess, eof, eol) < 0)
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}
//Initializes an extended string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfStringX_Init_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof,char eol)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN = rfFReadLine_UTF16LE(
                f, eol, &str->INH_String.bytes,
                &str->INH_String.byteLength,eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to initialize a StringX from "
                             "reading a UTF-16 file", bytesN)
        }

    }//end of little endian
    else//big endian
    {
        if((bytesN = rfFReadLine_UTF16BE(
                f, eol, &str->INH_String.bytes,
                &str->INH_String.byteLength, eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to initialize a StringX from "
                             "reading a UTF-16 file", bytesN)
        }

    }//end of big endian case
    //success
    str->bIndex = 0;
    str->bSize = 0;
    return bytesN;
}

//Assigns to an already initialized String from File parsing
int32_t rfStringX_Assign_fUTF16(RF_StringX* str, FILE* f, char endianess,
                                char* eof, char eol)
{
    uint32_t utf8ByteLength;
    int32_t bytesN;
    char* utf8 = 0;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN = rfFReadLine_UTF16LE(f, eol, &utf8, &utf8ByteLength,
                                         eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to assign the contents of a Little "
                             "Endian UTF-16 file to a StringX", bytesN)
        }

    }//end of little endian
    else//big endian
    {
        if((bytesN = rfFReadLine_UTF16BE(
                f, eol, &utf8, &utf8ByteLength, eof)) < 0)
        {
            LOG_ERROR("Failure to assign the contents of a Big Endian "
                      "UTF-16 file to a StringX", bytesN)
        }

    }//end of big endian case
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC_JMP(str->INH_String.bytes, char, str->bSize,
                       bytesN = RE_REALLOC_FAILURE, cleanup);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;

  cleanup:
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}

//Appends to an already initialized String from File parsing
int32_t rfStringX_Append_fUTF16(RF_StringX* str, FILE* f, char endianess,
                                char* eof, char eol)
{
    char*utf8;
    uint32_t utf8ByteLength;
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN = rfFReadLine_UTF16LE(f, eol, &utf8, &utf8ByteLength,
                                         eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to append the contents of a Little "
                             "Endian UTF-16 file to a StringX", bytesN)
        }

    }//end of little endian
    else//big endian
    {
        if((bytesN = rfFReadLine_UTF16BE(f, eol, &utf8, &utf8ByteLength,
                                         eof)) < 0)
            LOG_ERROR("Failure to append the contents of a Big Endian "
                      "UTF-16 file to a StringX", bytesN)

    }//end of big endian case

    if(rfStringX_Append(str,RFS_(utf8)) == false)
    {
        //error
        bytesN = RE_STRING_APPEND;
    }
    free(utf8);
    return bytesN;
}

// Allocates and returns an extended string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_StringX* rfStringX_Create_fUTF32(FILE* f, char endianess,
                                    char* eof,char eol)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(rfStringX_Init_fUTF32(ret,f,endianess,eof,eol) < 0)
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}
// Initializes an extended string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfStringX_Init_fUTF32(RF_StringX* str, FILE* f, char endianess,
                              char* eof,char eol)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN = rfFReadLine_UTF32LE(
                f, eol, &str->INH_String.bytes,
                &str->INH_String.byteLength,eof)) <0)
        {
            RETURN_LOG_ERROR("Failure to initialize a StringX from "
                             "reading a Little Endian UTF-32 file", bytesN)
        }

    }//end of little endian
    else//big endian
    {
        if((bytesN = rfFReadLine_UTF32BE(
                f, eol, &str->INH_String.bytes,
                &str->INH_String.byteLength,eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to initialize a StringX from "
                             "reading a Big Endian UTF-32 file", bytesN)
        }

    }//end of big endian case
    //success
    str->bIndex = 0;
    str->bSize = 0;
    return bytesN;
}
//Assigns the contents of a UTF-32 file to an extended string
int32_t rfStringX_Assign_fUTF32(RF_StringX* str, FILE* f, char endianess,
                                char* eof, char eol)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN = rfFReadLine_UTF32LE(
                f, eol, &utf8,&utf8ByteLength, eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to assign to a StringX from reading"
                             " a Little Endian UTF-32 file",bytesN)
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN = rfFReadLine_UTF32BE(
                f, eol, &utf8, &utf8ByteLength, eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to assign to a StringX from "
                             "reading a Big Endian UTF-32 file", bytesN)
        }

    }//end of big endian case
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC_JMP(str->INH_String.bytes, char, str->bSize,
                       bytesN = RE_REALLOC_FAILURE, cleanup);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;

  cleanup:
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
//Appends the contents of a UTF-32 file to an extended string
int32_t rfStringX_Append_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof,char eol)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN = rfFReadLine_UTF32LE(f, eol, &utf8, &utf8ByteLength,
                                         eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to append to a StringX from reading"
                             " a Little Endian UTF-32 file", bytesN)
        }

    }//end of little endian
    else//big endian
    {
        if((bytesN = rfFReadLine_UTF32BE(f, eol, &utf8, &utf8ByteLength,
                                         eof)) < 0)
        {
            RETURN_LOG_ERROR("Failure to append to a StringX from reading"
                             " a Big Endian UTF-32 file", bytesN)
        }

    }//end of big endian case

    //append it
    if(rfStringX_Append(str, RFS_(utf8)) == false)
    {
        //error
        bytesN = RE_STRING_APPEND;
    }
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}
