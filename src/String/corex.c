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
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/corex.h>
/*------------- Module related inclusion -------------*/
#include <String/traversalx.h> //for rfStringX_Reset()
#include <stdarg.h> //needed for the va_list
#include <String/core.h> //for some needed RF_String functions
#include "common.ph" //for RF_STRINGX_REALLOC()
/*------------- Outside Module inclusion -------------*/
//for endianess detection functionality
    #include <String/unicode.h> //for unicode functions
    #include <Utils/endianess.h> //for endianess detection
// for error logging
    #include <Utils/log.h>
//for error codes
    #include <Definitions/retcodes.h> 
//for RF_HEXLE_UI() macro and others
    #include <Utils/constcmp.h> 
//for the local scope macros
    #include <Utils/localscope.h>//for local scope macros
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for the internal buffer
    #include "../Internal/internal_mod.ph"
/*------------- libc inclusion --------------*/
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/

/* === Init functions === */

/* --- Functions that already exist for RF_String - START --- */
RF_StringX* rfStringX_Createv(const char* lit, ...)
{
    RF_StringX* ret;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();

    va_start(args, lit);
    //read the var args
    if(!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = NULL;
        goto cleanup_lscope;
    }
    va_end(args);

    RF_MALLOC_JMP(ret, sizeof(RF_StringX), ret = NULL, cleanup_buffer);
    if(!rfStringX_Init_unsafe_nnt(ret, buff_ptr, size))
    {
        free(ret);
        ret = NULL;
    }

#ifdef RF_OPTION_DEBUG
cleanup_buffer:
#endif
    rfBuffer_SetIndex(TSBUFFA, buff_index);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

RF_StringX* rfStringX_Create(const char* lit)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init(ret, lit))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfStringX_Initv(RF_StringX* str, const char* lit, ...)
{
    bool ret = true;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();

    va_start(args, lit);
    //read the var args
    if(!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = false;
        goto cleanup_lscope;
    }
    va_end(args);
    //initialize the string
    if(!rfStringX_Init_unsafe_nnt(str,
                                  (const char*)buff_ptr,
                                  size))
    {
        ret = false;
    }


    rfBuffer_SetIndex(TSBUFFA, buff_index);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfStringX_Init(RF_StringX* str, const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if(!rfUTF8_VerifyCstr(lit ,&byteLength))
    {
        RF_ERROR("Error at StringX initialization due to invalid UTF-8 "
                 "byte sequence");
        return false;
    }

    return rfStringX_Init_unsafe_nnt(str, lit, byteLength);
}

RF_StringX* rfStringX_Create_cp(uint32_t codepoint)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_cp(ret, codepoint))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rfStringX_Init_cp(RF_StringX* str, uint32_t codepoint)
{
    if(!rfString_Init_cp(&str->INH_String, codepoint))
    {
        return false;
    }
    str->bSize = MAX_UTF8C_BYTES;
    str->bIndex = 0;    
    return true;
}

RF_StringX* rfStringX_Create_i(int32_t i)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_i(ret, i))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rfStringX_Init_i(RF_StringX* str, int32_t i)
{
    if(!rfString_Init_i(&str->INH_String, i))
    {
        return false;
    }

    str->bSize = rfString_ByteLength(str);
    str->bIndex = 0;
    return true;
}
RF_StringX* rfStringX_Create_f(float f)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_f(ret, f))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rfStringX_Init_f(RF_StringX* str, float f)
{
    if(!rfString_Init_f(&str->INH_String, f))
    {
        return false;
    }
    str->bSize = rfString_ByteLength(str);
    return true;
}

RF_StringX* rfStringX_Create_UTF16(const uint16_t* s, unsigned int len)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_UTF16(ret, s, len))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rfStringX_Init_UTF16(RF_StringX* str, const uint16_t* s, unsigned int len)
{
    if(!rfString_Init_UTF16(&str->INH_String, s, len))
    {
        return false;
    }
    str->bIndex = 0;
    str->bSize = rfString_ByteLength(str);
    return true;
}
RF_StringX* rfStringX_Create_UTF32(const uint32_t* s, unsigned int len)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_UTF32(ret, s, len))
    {
        free(ret);
        return 0;
    }
    return ret;
}
bool rfStringX_Init_UTF32(RF_StringX* str, const uint32_t* codeBuffer,
                          unsigned int len)
{
    if(!rfString_Init_UTF32(&str->INH_String, codeBuffer, len))
    {
        return false;
    }
    str->bIndex = 0;
    str->bSize = rfString_ByteLength(str);
    return true;
}
RF_StringX* rfStringX_Create_unsafe(const char* lit)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_unsafe(ret, lit))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rfStringX_Init_unsafe(RF_StringX* str, const char* lit)
{
    if(!rfString_Init_unsafe(&str->INH_String, lit))
    {
        return false;
    }
    str->bIndex = 0;
    str->bSize = rfString_ByteLength(str);
    return true;
}
bool rfStringX_Init_unsafe_nnt(RF_StringX* str, const char* s,
                               size_t length)
{
    str->bSize = length * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER;
    RF_MALLOC(rfString_Data(str), str->bSize, false);
    memcpy(rfString_Data(str), s, length);
    rfString_ByteLength(str) = length;
    str->bIndex = 0;
    return true;
}

bool rfStringX_Init_unsafe_bnnt(RF_StringX* str, const char* s,
                                size_t length, size_t buff_size)
{
    str->bSize = buff_size;
    RF_MALLOC(rfString_Data(str), str->bSize, false);
    memcpy(rfString_Data(str), s, length);
    rfString_ByteLength(str) = length;
    str->bIndex = 0;
    return true;    
}

/* --- Functions that already exist for RF_String - END --- */

RF_StringX* rfStringX_Create_buffv(uint32_t buffSize, const char* lit, ...)
{
    RF_StringX* ret;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();

    va_start(args, lit);
    //read the var args
    if(!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = NULL;
        goto cleanup_lscope;
    }
    va_end(args);

    //Make sure that the buff size fits the string
    if(buffSize < size)
    {
        buffSize = size;
    }
    RF_MALLOC_JMP(ret, sizeof(RF_StringX), ret = NULL, cleanup_buffer);
    if(!rfStringX_Init_unsafe_bnnt(ret, buff_ptr, size, buffSize))
    {
        free(ret);
        ret = NULL;
    }

#ifdef RF_OPTION_DEBUG
cleanup_buffer:
#endif
    rfBuffer_SetIndex(TSBUFFA, buff_index);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
RF_StringX* rfStringX_Create_buff(uint32_t buffSize,const char* lit)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Init_buff(ret, buffSize, lit))
    {
        free(ret);
        return NULL;
    }
    return ret;
}


bool rfStringX_Init_buffv(RF_StringX* str, uint32_t buffSize,
                         const char* lit, ...)
{
    bool ret = true;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();

    va_start(args, lit);
    //read the var args
    if(!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = false;
        goto cleanup_lscope;
    }
    va_end(args);

    //Make sure that the buff size fits the string
    if(buffSize < size)
    {
        buffSize = size;
    }
    ret = rfStringX_Init_unsafe_bnnt(str, buff_ptr, size, buffSize);

    rfBuffer_SetIndex(TSBUFFA, buff_index);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfStringX_Init_buff(RF_StringX* str, uint32_t buffSize,
                             const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if(!rfUTF8_VerifyCstr(lit, &byteLength))
    {
        RF_ERROR("Error at StringX initialization due to invalid UTF-8 "
                  "byte sequence");
        return false;
    }
    //make sure that the buff size fits the string
    if(buffSize < byteLength)
    {
        buffSize = byteLength;
    }
    return rfStringX_Init_unsafe_bnnt(str, lit, byteLength, buffSize);
}


/*---- Methods to copy/assign an RF_StringX ----*/

bool rfStringX_Assign(RF_StringX* dst, const void* sourceP)
{
    const RF_String* source = (const RF_String*)sourceP;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    rfStringX_Reset(dst);//make sure that the destination's byte index is reset
    //only if the new string value won't fit in the buffer reallocate the buffer
    RF_STRINGX_REALLOC_JMP(dst, rfString_ByteLength(source), ret=false, cleanup);
    //now copy the value and the bytelength
    memcpy(rfString_Data(dst),
           rfString_Data(source),
           rfString_ByteLength(source));
    rfString_ByteLength(dst) = rfString_ByteLength(source);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Assigns the value of a unicode character to the string
bool rfStringX_Assign_char(RF_StringX* str, uint32_t codepoint)
{
    int bytes;
    //reset the buffer if needed
    rfStringX_Reset(str);
    //realloc if needed
    if(str->bSize <5)
    {
        RF_REALLOC(rfString_Data(str), char,
                   5 * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER, false);
    }
    if((bytes=rfUTF8_Encode_single(codepoint, rfString_Data(str))) <= 0)
    {
        RF_ERROR("Assigning a character to an RF_StringX failed "
                 "due to utf8 encoding");
        return false;
    }
    rfString_ByteLength(str) = bytes;
    return true;
}


// Returns an RF_StringX version of the parameter RF_String
RF_StringX* rfStringX_FromString_OUT(const RF_String* s)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_FromString_IN(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
//Initializes an RF_StringX from an RF_string
bool rfStringX_FromString_IN(RF_StringX* dst, const RF_String* src)
{
    //allocate the stringX
    dst->bIndex = 0;
    dst->bSize = rfString_ByteLength(src) * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER;
    RF_MALLOC(rfString_Data(dst), dst->bSize, false);
    rfString_ByteLength(dst) = rfString_ByteLength(src);
    //copy the bytes from the String
    memcpy(rfString_Data(dst), rfString_Data(src), rfString_ByteLength(src));
    return true;
}

//Creates a copy of an extended String and returns it
RF_StringX* rfStringX_Copy_OUT(RF_StringX* s)
{
    RF_StringX* ret;
    RF_MALLOC(ret, sizeof(RF_StringX), NULL);
    if(!rfStringX_Copy_IN(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
//Copies the given source string into the destination string.
bool rfStringX_Copy_IN(RF_StringX* dst, RF_StringX* src)
{
    //assert that the source string is copied from the beginning of its buffer
    rfString_Data(src) -= src->bIndex;
    //copy the bytes
    RF_MALLOC(rfString_Data(dst), src->bSize, false);
    memcpy(rfString_Data(dst), rfString_Data(src), src->bSize);
    //push the source bytes back and also the new copied bytes
    //buffer to the same index
    rfString_Data(src) += src->bIndex;
    rfString_Data(dst) += src->bIndex;
    //copy the parameters
    dst->bSize = src->bSize;
    dst->bIndex = src->bIndex;
    rfString_ByteLength(dst) = rfString_ByteLength(src);
    return true;
}
// Copies a certain number of characters from an RF_StringX
bool rfStringX_Copy_chars(RF_StringX* dst, RF_StringX* src,
                          uint32_t charsN)
{
    uint32_t i=0,bytePos;
    rfStringX_Copy_IN(dst, src);
    //find the byte position and end the string there
    RF_STRING_ITERATE_START(src, i, bytePos)
    if(i == charsN)
        break;
    RF_STRING_ITERATE_END(i, bytePos)
    rfString_ByteLength(dst)= bytePos;
    return true;
}

/*--- Methods to get rid of an RF_StringX ---*/

void rfStringX_Destroy(RF_StringX* s)
{
    //an extended string can have moved its internal pointer forward
    //so we have to put it back at the origin to free properly
    rfString_Data(s) -= s->bIndex;
    free(rfString_Data(s));
    free(s);
}
void rfStringX_Deinit(RF_StringX* s)
{
    //an extended string can have moved its internal pointer forward
    //so we have to put it back at the origin to free properly
    rfString_Data(s) -= s->bIndex;
    free(rfString_Data(s));
}
