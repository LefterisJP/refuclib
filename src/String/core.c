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
#include <stdarg.h> //needed for the va_list argument in rfStringX_Formatv() and also in the functions below
#include <String/format.h> //for the String formatting function
#include "common.ph"//for private string iteration
/*------------- Outside Module inclusion -------------*/
#include <String/unicode.h> //for rfUTF8_VerifySequence()

#include <Definitions/retcodes.h> //for error codes
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Threads/common.h> //for rfThread_GetID()
    #include <errno.h> //for error reporting that needs it
    #include <Utils/error.h>
//for the io buffer
    #include <Definitions/threadspecific.h> // for the thread specific keyword used in the ioBuffer
    #include "../IO/buff.ph" //for the ioBuffer StringX
//for the local scope macros
    #include <Utils/localmem_decl.h> //for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>//for local scope macros
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Utils/memory.h> //for refu memory allocation
/*------------- End of includes -------------*/


//Allocates and returns a string with the given characters a refu string with the given characters. Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_String* rfString_Create(const char* s, ...)
#else
RF_String* i_rfString_Create(const char* s, ...)
#endif
{
    RF_String* ret;
    va_list args;//note that no local scope macros are needed here since the

    va_start(args,s);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,s,args) == false)
    {
        RF_ERROR(0, "String creation failure due to failing at reading the "
                 "formatted string");
        return NULL;
    }
    va_end(args);

    RF_MALLOC(ret, sizeof(RF_String), NULL);
    //get length
    ret->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(ret->bytes, ret->byteLength+1, NULL);
    memcpy(ret->bytes, ioBuffer.INH_String.bytes, ret->byteLength+1);

    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_String* i_NVrfString_Create(const char* s)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(i_NVrfString_Init(ret, s) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}
#endif


// Initializes a string with the given characters. Given characters have to be
// in UTF-8. A check for valid sequence of bytes is performed.
// <b>Can't be used with RF_StringX</b>
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Init(RF_String* str, const char* s, ...)
#else
char i_rfString_Init(RF_String* str, const char* s, ...)
#endif
{
    va_list args;//note that no local scope macros are needed here since the 
                 //arguments get evaluated in rfStringX_Formatv()
    va_start(args,s);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,s,args) == false)
    {
        RF_ERROR(0, "String creation failure due to failing at reading the "
                 "formatted string");
        return false;
    }
    va_end(args);

    //get length
    str->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(str->bytes, str->byteLength+1, false);
    memcpy(str->bytes, ioBuffer.INH_String.bytes, str->byteLength+1);

    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfString_Init(RF_String* str, const char* s)
{
    //check for validity of the given sequence and get the character length
    uint32_t byteLength;
    if(!rfUTF8_VerifySequence(s,&byteLength))
    {
        RF_ERROR(0, "Error at String Initialization due to invalid UTF-8 "
                 "byte sequence");
        return false;
    }

    //get length
    str->byteLength = byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(str->bytes, str->byteLength+1, false);
    memcpy(str->bytes, s, str->byteLength+1);

    return true;
}
#endif

//Allocates a String by turning a unicode code point in a String (encoded in UTF-8).
RF_String* rfString_Create_cp(uint32_t codepoint)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_cp(ret,codepoint) == true)
    {
        return ret;
    }
    //failure
    free(ret);
    return NULL;
}

//Initializes a string by turning a unicode code point in a String (encoded in UTF-8).
char rfString_Init_cp(RF_String* str, uint32_t codepoint)
{
    //alloc enough for a character
    RF_MALLOC(str->bytes, 5, false);

    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        str->bytes[0] = codepoint;
        str->bytes[1] = '\0';
        str->byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && 
             RF_HEXLE_UI(codepoint,0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->bytes[2] = '\0';
        str->byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) &&
             RF_HEXLE_UI(codepoint,0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->bytes[3] = '\0';
        str->byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) &&
             RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->bytes[4] = '\0';
        str->byteLength = 4;
    }
    else
    {
        RF_ERROR(0, "Attempted to encode an invalid unicode code point into"
                 " a string");
        free(str->bytes);
        return false;
    }

    return true;
}


// Allocates and returns a string with the given integer
RF_String* rfString_Create_i(int32_t i)
{
    //initialize the string and return it
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_i(ret, i) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}
// Initializes a string with the given integer.
char rfString_Init_i(RF_String* str, int32_t i)
{
    //the size of the int32_t buffer
    int32_t len;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    if((len = sprintf(buff, "%d", i)) < 0)
    {
        RF_ERROR_PRINTF("String initialization from integer failed", "sprintf");
        return false;
    }
    str->byteLength = len;
    RF_MALLOC(str->bytes, len+1, false);
    strcpy(str->bytes, buff);

    return true;
}

// Allocates and returns a string with the given float
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
// Initializes a string with the given float
char rfString_Init_f(RF_String* str,float f)
{
    char buff[50];//a buffer to hold the string float, TODO: check if size if ok
    int32_t len;

    if((len = sprintf(buff,"%f",f)) < 0)
    {
        RF_ERROR_PRINTF("String initialization from float failed", "sprintf");
        return false;
    }

    str->byteLength = len;
    RF_MALLOC(str->bytes, len+1, false);
    strcpy(str->bytes, buff);

    //success
    return true;
}

//Allocates and returns a string with the given UTF-16 byte sequence. Given
// characters have to be in UTF-16. A check for valid sequence of bytes is
// performed.<b>Can't be used with RF_StringX</b>
RF_String* rfString_Create_UTF16(const uint16_t* s)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_UTF16(ret, s) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

//Initializes a string with the given UTF-16 byte sequence. Given characters
// have to be in UTF-16. A check for valid sequence of bytes is performed.
// <b>Can't be used with RF_StringX</b>
char rfString_Init_UTF16(RF_String* str, const uint16_t* s)
{
    //decode the utf-16 and get the code points
    uint32_t* codepoints;
    uint32_t byteLength, characterLength, utf8ByteLength;
    char* utf8;

    byteLength = 0;
    while(s[byteLength/2]!= 0)
    {
        byteLength += 2;
    }
    byteLength+=3;//for the last utf-16 null termination character
    RF_MALLOC(codepoints, byteLength*2, false); //allocate the codepoints
    //parse the given byte stream normally since it has to be in the 
    //endianess of the system
    if(!rfUTF16_Decode((const char*)s, &characterLength, codepoints,
                       byteLength*2))
    {
        free(codepoints);
        RF_ERROR(0, "String initialization failed due to invalide UTF-16 "
                 "sequence");
        return false;
    }
    //now encode these codepoints into UTF8
    RF_MALLOC(utf8, characterLength*4, false);
    if(!rfUTF8_Encode(codepoints,characterLength,
                      &utf8ByteLength, utf8, characterLength*4))
    {
        RF_ERROR(0, "String initialization failed during encoding in UTF8");
        free(codepoints);
        free(utf8);
        return false;
    }
    //success
    free(codepoints);
    str->bytes = utf8;
    str->byteLength = utf8ByteLength;
    return true;
}

//Allocates and returns a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32.
RF_String* rfString_Create_UTF32(const uint32_t* s)
{
    RF_String* ret;
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Init_UTF32(ret, s) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}
//Initializes a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32.
char rfString_Init_UTF32(RF_String* str, const uint32_t* codeBuffer)
{
    //find the length of the utf32 buffer in characters
    uint32_t length,utf8ByteLength;
    char* utf8;
    rfUTF32_Length(codeBuffer,length);

    //turn the codepoints into a utf-8 encoded buffer
    RF_MALLOC(utf8, length*4, false);
    if(!rfUTF8_Encode(codeBuffer, length, &utf8ByteLength, utf8, length*4))
    {
        RF_ERROR(0, "Could not properly encode a UTF32 buffer into UTF8");
        free(utf8);
        return false;
    }
    str->bytes = utf8;
    str->byteLength = utf8ByteLength;
    return true;
}

// Allocates and returns a string with the given characters. NO VALID-UTF8 check is performed
RF_String* rfString_Create_unsafe(const char* s)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String), NULL);
    //get length
    ret->byteLength = strlen(s);
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(ret->bytes, ret->byteLength+1, NULL);
    memcpy(ret->bytes, s, ret->byteLength+1);
    return ret;
}


// Initializes a string with the given characters. NO VALID-UTF8 check is performed
char rfString_Init_unsafe(RF_String* str, const char* s)
{
    //get its length
    str->byteLength = strlen(s);
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(str->bytes, str->byteLength+1, false);
    memcpy(str->bytes, s, str->byteLength+1);
    return true;
}


// Assigns the value of the source string to the destination.Both strings should already be initialized and hold a value. It is an error to give null parameters.
char rfString_Assign(RF_String* dest,const void* sourceP)
{
    const RF_String* source = (const RF_String*)sourceP;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();
    //only if the new string value won't fit in the buffer reallocate the buffer (let's avoid unecessary reallocs)
    if(source->byteLength > dest->byteLength)
    {
        RF_REALLOC_JMP(dest->bytes, char, source->byteLength+1,
                       ret=false, cleanup);
    }
    //now copy the value
    memcpy(dest->bytes,source->bytes, source->byteLength+1);
    //and fix the lengths
    dest->byteLength = source->byteLength;
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Assigns the value of a unicode character to the string
char rfString_Assign_char(RF_String* str,uint32_t codepoint)
{
    //realloc if needed
    if(str->byteLength <5)
    {
        RF_REALLOC(str->bytes, char, 5, false);
    }
    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint, 0x007f))
    {
        str->bytes[0] = codepoint;
        str->bytes[1] = '\0';
        str->byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint, 0x0080) &&
             RF_HEXLE_UI(codepoint, 0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->bytes[2] = '\0';
        str->byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint, 0x0800) &&
             RF_HEXLE_UI(codepoint, 0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->bytes[3] = '\0';
        str->byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) &&
             RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->bytes[4] = '\0';
        str->byteLength = 4;
    }
    else
    {
        RF_ERROR(0, "Attempted to encode an invalid unicode code point into"
                 " a string");
        return false;
    }

    return true;
}

/*--- RF_String copying functions ---*/




//Creates and returns an allocated copy of the given string
RF_String* rfString_Copy_OUT(const void* srcP)
{
    const RF_String* src = (const RF_String*)srcP;
    RF_String* ret;
    //create the new string
    RF_MALLOC(ret, sizeof(RF_String), NULL);
    if(rfString_Copy_IN(ret, src) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}
// Copies all the contents of a string to another
char rfString_Copy_IN(RF_String* dst, const void* srcP)
{
    const RF_String* src = (const RF_String*)srcP;
    //get the length
    dst->byteLength = src->byteLength;
    //copy the bytes
    RF_MALLOC(dst->bytes, src->byteLength+1, false);
    memcpy(dst->bytes, src->bytes, dst->byteLength+1);
    return true;

}
//Copies a certain number of characters from a string
char rfString_Copy_chars(RF_String* dst, const void* srcP, uint32_t charsN)
{
    uint32_t i = 0,bytePos;
    const RF_String* src = (const RF_String*)srcP;

    //find the byte position until which we need to copy
    RF_STRING_ITERATE_START(src,i,bytePos)
        if(i == charsN)
            break;
    RF_STRING_ITERATE_END(i,bytePos)
    dst->byteLength = bytePos;
    RF_MALLOC(dst->bytes, dst->byteLength+1 ,false);
    memcpy(dst->bytes,src->bytes,dst->byteLength+1);
    dst->bytes[dst->byteLength] = '\0';//null terminate it
    return true;
}


/*--- Methods to get rid of an RF_String ---*/

// Deletes a string object and also frees its pointer.
void rfString_Destroy(RF_String* s)
{
    free(s->bytes);
    free(s);
}
// Deletes a string object only, not its memory.
void rfString_Deinit(RF_String* s)
{
    free(s->bytes);
}

/*--- Equality check ---*/

// Compares two Strings and returns true if they are equal and false otherwise
char rfString_Equal(const void* s1P,const void* s2P)
{
    const RF_String* s1 = (const RF_String*)s1P;
    const RF_String* s2 = (const RF_String*)s2P;
    char ret = false;
    RF_ENTER_LOCAL_SCOPE();
    if( strcmp(s1->bytes,s2->bytes)==0)
    {
        ret = true;
    }
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

/*--- 2 functions used in the iteration macros ---*/

//Retrieves the unicode code point of the parameter bytepos of the string. 
//If the byte position is not the start of a character 0 is returned. This is 
//an internal function, there is no need to use it. <i>Can be used with StringX</i>
uint32_t rfString_BytePosToCodePoint(const void* str, uint32_t i)
{
    uint32_t codePoint=0;
    const RF_String* thisstr = (const RF_String*)str;
    ///Here I am not checking if byte position 'i' is withing bounds and especially if it is a start of a character
    /// This is assumed to have been checked or to be known beforehand by the programmer. That's one of the reasons
    /// why this is an internal function and should not be used unless you know what you are doing
    //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
    if( ((thisstr->bytes[i] & 0x80)>>7) == 0 )
    {
        //and the code point is this whole byte only
        codePoint = thisstr->bytes[i];
    }
    //if the leading bits are in the form of 0b110xxxxx then range is: U+0080 to U+07FF (2 bytes)
    else if ( RF_HEXEQ_C( ( (~(thisstr->bytes[i] ^  0xC0))>>5),0x7) )
    {
        codePoint =0;
        //from the second byte take the first 6 bits
        codePoint = (thisstr->bytes[i+1] & 0x3F) ;
        //from the first byte take the first 5 bits and put them in the start
        codePoint |= ((thisstr->bytes[i] & 0x1F) << 6);
    }
    //if the leading bits are in the form of 0b1110xxxx then range is U+0800 to U+FFFF  (3 bytes)
    else if( RF_HEXEQ_C( ( (~(thisstr->bytes[i] ^ 0xE0))>>4),0xF) )
    {
        codePoint = 0;
        //from the third byte take the first 6 bits
        codePoint = (thisstr->bytes[i+2] & 0x3F) ;
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i+1] & 0x3F) << 6);
        //from the first byte take the first 4 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i] & 0xF) << 12);
    }
    //if the leading bits are in the form of 0b11110xxx then range is U+010000 to U+10FFFF (4 bytes)
    else if( RF_HEXEQ_C( ( (~(thisstr->bytes[i] ^ 0xF0))>>3), 0x1F))
    {
        codePoint = 0;
        //from the fourth byte take the first 6 bits
        codePoint = (thisstr->bytes[i+3] & 0x3F) ;
        //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i+2] & 0x3F) << 6);
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i+1] & 0x3F) << 12);
        //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i] & 0x7) << 18);
    }

    return codePoint;
}
//Retrieves character position of a byte position
uint32_t rfString_BytePosToCharPos(const void* thisstrP,uint32_t bytepos,
                                   char before)
{
    ///here there is no check if this is actually a byte pos inside the string's
    ///byte buffer. The programmer should have made sure it is before hand. This is why it is
    /// an internal function and should only be used if you know what you are doing
    const RF_String* thisstr = (const RF_String*)thisstrP;
    uint32_t charPos = 0;
    uint32_t byteI = 0;
    //iterate the string's bytes until you get to the required byte
    //if it is not a continuation byte, return the position
    if(rfUTF8_IsContinuationByte(thisstr->bytes[bytepos])==false)
    {
        RF_STRING_ITERATE_START(thisstr,charPos,byteI)
            if(byteI == bytepos)
                return charPos;
        RF_STRING_ITERATE_END(charPos,byteI)
    }
    //else  iterate the string's bytes until you get anything bigger than the required byte
    RF_STRING_ITERATE_START(thisstr,charPos,byteI)
        if(byteI > bytepos)
            break;
    RF_STRING_ITERATE_END(charPos,byteI)
    //if we need the previous one return it
    if(before == true)
        return charPos-1;
    //else return this
    return charPos;
}

