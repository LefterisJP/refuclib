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
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <Utils/constcmp.h> //for RF_HEXEQ_C() used in rfUTF8_IsContinuationByte() which itself is used in the iteration macros
#include <String/core.h>
/*------------- Module related inclusion -------------*/
#include <String/stringx_decl.h> //for RF_StringX
#include <stdarg.h> //needed for the va_list
//for private string iteration
    #include "common.ph"
//for some defines
#include "defines.ph"
/*------------- Outside Module inclusion -------------*/
//for unicode functions
    #include <String/unicode.h>
//for error code
    #include <Definitions/retcodes.h>
//for error logging
    #include <Utils/log.h>
//for the local scope macros
    #include <Utils/localscope.h>//for local scope macros
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for the internal buffer
    #include "../Internal/internal_mod.ph"
/*------------- libc inclusion --------------*/
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/



RF_String* rfString_Createv(const char* s, ...)
{
    RF_String* ret;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();

    va_start(args,s);
    //read the var args
    if(!fill_fmt_buffer(s, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("String creation failure due to failing at reading the "
                 "formatted string");
        ret = NULL;
        goto cleanup_lscope;
    }
    va_end(args);
    RF_MALLOC_JMP(ret, sizeof(RF_String), ret = NULL, cleanup_buffer);

    //get length
    rfString_ByteLength(ret) = size;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC_JMP(rfString_Data(ret), rfString_ByteLength(ret),
                  ret = NULL, cleanup_buffer);
    memcpy(rfString_Data(ret), buff_ptr, rfString_ByteLength(ret));

#ifdef RF_OPTION_DEBUG
cleanup_buffer:
#endif
    rfBuffer_SetIndex(TSBUFFA, buff_index);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

RF_String* rfString_Create(const char* s)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(!rfString_Init(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}




bool rfString_Initv(RF_String* str, const char* s, ...)
{
    va_list args;
    unsigned int size, buff_index;
    bool ret = true;
    char *buff_ptr;
    RF_ENTER_LOCAL_SCOPE();
    va_start(args, s);
    //read the var args
    if(!fill_fmt_buffer(s, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("String creation failure due to failing at reading the "
                 "formatted string");
        ret = false;
        goto cleanup_lscope;
    }
    va_end(args);

    //get length
    rfString_ByteLength(str) = size;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC_JMP(rfString_Data(str), rfString_ByteLength(str),
                  ret = false, cleanup_buffer);
    memcpy(rfString_Data(str), buff_ptr, rfString_ByteLength(str));

#ifdef RF_OPTION_DEBUG
cleanup_buffer:
#endif
    rfBuffer_SetIndex(TSBUFFA, buff_index);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
bool rfString_Init(RF_String* str, const char* s)
{
    //check for validity of the given sequence and get the character length
    uint32_t byteLength;
    if(!rfUTF8_VerifyCstr(s, &byteLength))
    {
        RF_ERROR("Error at String Initialization due to invalid UTF-8 "
                 "byte sequence");
        return false;
    }

    //get length
    rfString_ByteLength(str) = byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(rfString_Data(str), rfString_ByteLength(str), false);
    memcpy(rfString_Data(str), s, rfString_ByteLength(str));

    return true;
}


//Allocates a String by turning a unicode code point in a String (encoded in UTF-8).
RF_String* rfString_Create_cp(uint32_t codepoint)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_cp(ret, codepoint))
    {
        return ret;
    }
    //failure
    free(ret);
    return NULL;
}

//Turns a unicode code point in a String (encoded in UTF-8).
bool rfString_Init_cp(RF_String* str, uint32_t codepoint)
{
    //alloc enough for a utf8 character
    RF_MALLOC(rfString_Data(str), MAX_UTF8C_BYTES, false);
    rfString_ByteLength(str) = rfUTF8_Encode_single(codepoint, rfString_Data(str));
    if(!rfString_ByteLength(str))
    {
        free(rfString_Data(str));
        return false;
    }
    return true;
}

RF_String* rfString_Create_i(int32_t i)
{
    //initialize the string and return it
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(!rfString_Init_i(ret, i))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfString_Init_i(RF_String* str, int32_t i)
{
    //the size of the int32_t buffer
    int len;
    //put the int32_t into a buffer and turn it in a char*
    char buff[MAX_UINT32_STRING_CHAR_SIZE];
    len = snprintf(buff, MAX_UINT32_STRING_CHAR_SIZE, "%d", i);
    if(len < 0 || len >= MAX_UINT32_STRING_CHAR_SIZE)
    {
        RF_ERROR("String initialization from integer failed due to snprintf() "
                 "failing with errno %d", errno);
        return false;
    }
    rfString_ByteLength(str) = len;
    RF_MALLOC(rfString_Data(str), len, false);
    memcpy(rfString_Data(str), buff, len);

    return true;
}

RF_String* rfString_Create_f(float f)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_f(ret, f) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfString_Init_f(RF_String* str,float f)
{
    char buff[MAX_DOUBLE_STRING_CHAR_SIZE];
    int len;
    len = snprintf(buff, MAX_DOUBLE_STRING_CHAR_SIZE, "%f", f);
    if(len < 0 || len >= MAX_DOUBLE_STRING_CHAR_SIZE)
    {
        RF_ERROR("String initialization from float failed due to snprintf() "
                 "failing with errno %d", errno);
        return false;
    }

    rfString_ByteLength(str) = len;
    RF_MALLOC(rfString_Data(str), len, false);
    memcpy(rfString_Data(str), buff, len);

    //success
    return true;
}

RF_String* rfString_Create_UTF16(const uint16_t* s, unsigned int len)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(!rfString_Init_UTF16(ret, s, len))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfString_Init_UTF16(RF_String* str, const uint16_t* s, unsigned int len)
{
    //decode the utf-16 and get the code points
    uint32_t* codepoints;
    uint32_t characterLength, utf8ByteLength;
    char* utf8;


    RF_MALLOC(codepoints, len * 2, false); //allocate the codepoints
    //parse the given byte stream normally since it has to be in the
    //endianess of the system
    if(!rfUTF16_Decode((const char*)s, len, &characterLength, codepoints,
                       len * 2))
    {
        free(codepoints);
        RF_ERROR("String initialization failed due to invalide UTF-16 "
                 "sequence");
        return false;
    }
    //now encode these codepoints into UTF8
    RF_MALLOC(utf8, characterLength * 4, false);
    if(!rfUTF8_Encode(codepoints, characterLength,
                      &utf8ByteLength, utf8, characterLength * 4))
    {
        RF_ERROR("String initialization failed during encoding in UTF8");
        free(codepoints);
        free(utf8);
        return false;
    }
    //success
    free(codepoints);
    rfString_Data(str) = utf8;
    rfString_ByteLength(str) = utf8ByteLength;
    return true;
}

RF_String* rfString_Create_UTF32(const uint32_t* s, unsigned int len)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_UTF32(ret, s, len) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfString_Init_UTF32(RF_String* str, const uint32_t* codeBuffer,
                         unsigned int length)
{
    //find the length of the utf32 buffer in characters
    uint32_t utf8ByteLength;
    char *utf8;

    //turn the codepoints into a utf-8 encoded buffer
    RF_MALLOC(utf8, length * 4, false);
    if(!rfUTF8_Encode(codeBuffer, length, &utf8ByteLength, utf8, length * 4))
    {
        RF_ERROR("Could not properly encode a UTF32 buffer into UTF8");
        free(utf8);
        return false;
    }
    rfString_Data(str) = utf8;
    rfString_ByteLength(str) = utf8ByteLength;
    return true;
}


RF_String* rfString_Create_unsafe(const char* s)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(!rfString_Init_unsafe(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfString_Init_unsafe(RF_String* str, const char* s)
{
    return rfString_Init_unsafe_nnt(str, s, strlen(s));
}


bool rfString_Init_unsafe_nnt(RF_String* str, const char* s, size_t length)
{
    RF_MALLOC(rfString_Data(str), length, false);
    memcpy(rfString_Data(str), s, length);
    rfString_ByteLength(str) = length;
    return true;
}


/* --- Assigning to a String --- */


bool rfString_Assign(RF_String* dst, const void* src)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    //only if new string value won't fit in the buffer reallocate
    if(rfString_ByteLength(src) > rfString_ByteLength(dst))
    {
        RF_REALLOC_JMP(rfString_Data(dst), char, rfString_ByteLength(src),
                       ret = false, cleanup);
    }
    //now copy the value
    memcpy(rfString_Data(dst), rfString_Data(src), rfString_ByteLength(src));
    //and fix the lengths
    rfString_ByteLength(dst) = rfString_ByteLength(src);
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Assigns the value of a unicode character to the string
bool rfString_Assign_char(RF_String* str,uint32_t codepoint)
{
    //realloc if needed
    if(str->length <5)
    {
        RF_REALLOC(rfString_Data(str), char, 5, false);
    }
    rfString_ByteLength(str) = rfUTF8_Encode_single(codepoint,
                                                    rfString_Data(str));
    if(!rfString_ByteLength(str))
    {
        return false;
    }
    return true;
}


/*--- RF_String copying functions ---*/

RF_String* rfString_Copy_OUT(const void* src)
{
    RF_String* ret;
    //create the new string
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(!rfString_Copy_IN(ret, src))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfString_Copy_IN(RF_String* dst, const void* src)
{
    return rfString_Init_unsafe_nnt(dst,
                                    rfString_Data(src),
                                    rfString_ByteLength(src));
}
//Copies a certain number of characters from a string
bool rfString_Copy_chars(RF_String* dst, const void* src, uint32_t charsN)
{
    uint32_t i = 0,bytePos;

    //find the byte position until which we need to copy
    RF_STRING_ITERATE_START(src, i, bytePos)
        if(i == charsN) { break;}
    RF_STRING_ITERATE_END(i, bytePos)

    return rfString_Init_unsafe_nnt(dst, rfString_Data(src), bytePos);
}


/*--- Methods to get rid of an RF_String ---*/

// Deletes a string object and also frees its pointer.
void rfString_Destroy(RF_String* s)
{
    if(s != 0)
    {
        free(rfString_Data(s));
        free(s);
    }
}
// Deletes a string object only, not its memory.
void rfString_Deinit(RF_String* s)
{
    if(s != 0)
    {
        free(rfString_Data(s));
    }
}

/*--- Equality check ---*/

bool rfString_Equal(const void* s1, const void* s2)
{
    bool ret;
    RF_ENTER_LOCAL_SCOPE();
    ret = strcmp_nnt(rfString_Data(s1), rfString_ByteLength(s1),
                     rfString_Data(s2), rfString_ByteLength(s2));
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

/*--- 2 functions used in the iteration macros ---*/


uint32_t rfString_BytePosToCodePoint(const void* str, uint32_t i)
{
    uint32_t codePoint=0;
    /*
      Here I am not checking if byte position 'i' is withing bounds
      and especially if it is a start of a character. This is assumed
      to have been checked or to be known beforehand by the programmer.
      That's one of the reasons why this is an internal function and
      should not be used unless you know what you are doing
    */
    //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
    if( ((rfString_Data(str)[i] & 0x80)>>7) == 0 )
    {
        //and the code point is this whole byte only
        codePoint = rfString_Data(str)[i];
    }
    //if the leading bits are in the form of 0b110xxxxx then range is: U+0080 to U+07FF (2 bytes)
    else if ( RF_HEXEQ_C( ( (~(rfString_Data(str)[i] ^  0xC0))>>5),0x7) )
    {
        codePoint =0;
        //from the second byte take the first 6 bits
        codePoint = (rfString_Data(str)[i+1] & 0x3F) ;
        //from the first byte take the first 5 bits and put them in the start
        codePoint |= ((rfString_Data(str)[i] & 0x1F) << 6);
    }
    //if the leading bits are in the form of 0b1110xxxx then range is U+0800 to U+FFFF  (3 bytes)
    else if( RF_HEXEQ_C( ( (~(rfString_Data(str)[i] ^ 0xE0))>>4),0xF) )
    {
        codePoint = 0;
        //from the third byte take the first 6 bits
        codePoint = (rfString_Data(str)[i+2] & 0x3F) ;
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((rfString_Data(str)[i+1] & 0x3F) << 6);
        //from the first byte take the first 4 bits and put them to the left of the previous 6 bits
        codePoint |= ((rfString_Data(str)[i] & 0xF) << 12);
    }
    //if the leading bits are in the form of 0b11110xxx then range is U+010000 to U+10FFFF (4 bytes)
    else if( RF_HEXEQ_C( ( (~(rfString_Data(str)[i] ^ 0xF0))>>3), 0x1F))
    {
        codePoint = 0;
        //from the fourth byte take the first 6 bits
        codePoint = (rfString_Data(str)[i+3] & 0x3F) ;
        //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((rfString_Data(str)[i+2] & 0x3F) << 6);
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((rfString_Data(str)[i+1] & 0x3F) << 12);
        //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
        codePoint |= ((rfString_Data(str)[i] & 0x7) << 18);
    }

    return codePoint;
}
//Retrieves character position of a byte position
uint32_t rfString_BytePosToCharPos(const void* str, uint32_t bytepos,
                                   bool before)
{
    /*
      here there is no check if this is actually a byte pos inside the string's
      byte buffer. The programmer should have made sure it is before hand.
      This is why it is an internal function and should only be used if
      you know what you are doing
    */
    uint32_t charPos = 0;
    uint32_t byteI = 0;
    //iterate the string's bytes until you get to the required byte
    //if it is not a continuation byte, return the position
    if(!rfUTF8_IsContinuationByte(rfString_Data(str)[bytepos]))
    {
        RF_STRING_ITERATE_START(str, charPos, byteI)
            if(byteI == bytepos) {return charPos;}
        RF_STRING_ITERATE_END(charPos, byteI)
    }
    //else iterate the string's bytes until you get sth bigger than the required byte
    RF_STRING_ITERATE_START(str, charPos, byteI)
        if(byteI > bytepos)
            break;
    RF_STRING_ITERATE_END(charPos, byteI)
    //if we need the previous one return it
    if(before)
    {
        return charPos - 1;
    }
    //else return this
    return charPos;
}

