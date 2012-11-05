/**
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include <String/rfc_stringx.h>
#include <rf_utils.h>
#include "string_private.h"
#include <rf_options.h> //for the capacity multiplier option

//for variable arguments
#include <stdarg.h>
//for variable type limits
#include <limits.h>



//! A macro that calculates an extended string's remaining buffer size (excluding the bytes hidden behind the index pointer)
#define RF_STRINGX_REMAINING_SIZE(s__)   ((s__)->bSize-(s__)->bIndex) - ((s__)->INH_String.byteLength-1)

/********************************************RF_StringX functions start***************************************************/

/*-------------------------------------------------------------------------Methods to create an RF_StringX-------------------------------------------------------------------------------*/

//Allocates and returns an extended String. Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* rfStringX_Create(const char* lit,...)
#else
RF_StringX* i_rfStringX_Create(const char* lit,...)
#endif
{
    READ_VSNPRINTF_ARGS(lit,lit,0)
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(buff,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        if(buffAllocated == true)
            free(buff);
        return 0;
    }
    //initialize the string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength = byteLength;
    //allocate and populate the data
    ret->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,buff);
    ret->bIndex = 0;
    if(buffAllocated==true)
        free(buff);
    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* i_NVrfStringX_Create(const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(lit,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }
    //initialize the string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength = byteLength;
    //allocate and populate the data
    ret->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,lit);
    ret->bIndex = 0;

    return ret;
}
#endif

//Initializes an extended String. Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_Init(RF_StringX* str,const char* lit,...)
#else
char i_rfStringX_Init(RF_StringX* str,const char* lit,...)
#endif
{
    READ_VSNPRINTF_ARGS(lit,lit,false)
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(buff,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        if(buffAllocated == true)
            free(buff);
        return false;
    }
    //get bytelength
    str->INH_String.byteLength = byteLength;
    //allocate and populate the data
    str->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,buff);
    str->bIndex = 0;
    if(buffAllocated == true)
        free(buff);
    //success
    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfStringX_Init(RF_StringX* str,const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(lit,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //get bytelength
    str->INH_String.byteLength = byteLength;
    //allocate and populate the data
    str->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,lit);
    str->bIndex = 0;
    //success
    return true;
}
#endif

//Turns a unicode code point in an extended  String and allocates and returns it (encoded in UTF-8)
RF_StringX* rfStringX_Create_cp(uint32_t codepoint)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //since this is an extended string let's be a bit more generous with the buffer
    ret->bSize = 5*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;

    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        ret->INH_String.bytes[0] = codepoint;
        ret->INH_String.bytes[1] = '\0';
        ret->INH_String.byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[1] = (codepoint & 0x3F)|(0x02<<6);
            //get the 5 following bits and encode them in the second byte
            ret->INH_String.bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        }
        else
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[0] = (codepoint & 0x3F)|(0x02<<6);
            //get the 5 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        }
        ret->INH_String.bytes[2] = '\0';
        ret->INH_String.byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[2] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 4 following bits and encode them in the third byte
            ret->INH_String.bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        }
        else
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[0] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 4 following bits and encode them in the third byte
            ret->INH_String.bytes[2] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        }
        ret->INH_String.bytes[3] = '\0';
        ret->INH_String.byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[3] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 6 following bits and encode them in the third byte
            ret->INH_String.bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
            //get the 3 following bits and encode them in the fourth byte
            ret->INH_String.bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        }
        else
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[0] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 6 following bits and encode them in the third byte
            ret->INH_String.bytes[2] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
            //get the 3 following bits and encode them in the fourth byte
            ret->INH_String.bytes[3] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        }
        ret->INH_String.bytes[4] = '\0';
        ret->INH_String.byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into an extended string",RE_UTF8_INVALID_CODE_POINT);
        rfStringX_Destroy(ret);
        return 0;
    }

    return ret;
}
//Turns a unicode code point in an extended  String and initializes it (encoded in UTF-8)
char rfStringX_Init_cp(RF_StringX* str,uint32_t codepoint)
{

    //since this is an extended string let's be a bit more generous with the buffer
    str->bSize = 5*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;

    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        str->INH_String.bytes[0] = codepoint;
        str->INH_String.bytes[1] = '\0';
        str->INH_String.byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->INH_String.bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->INH_String.bytes[2] = '\0';
        str->INH_String.byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->INH_String.bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->INH_String.bytes[3] = '\0';
        str->INH_String.byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->INH_String.bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->INH_String.bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->INH_String.bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->INH_String.bytes[4] = '\0';
        str->INH_String.byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into an extended string",RE_UTF8_INVALID_CODE_POINT);
        free(str->INH_String.bytes);
        return false;
    }

    return true;
}

//Allocates and returns a string with the given integer
RF_StringX* rfStringX_Create_i(int32_t i)
{
    //the size of the int32_t buffer
    int32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%d",i);
    numLen = strlen(buff)+1;

    //initialize the string and return it
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    ret->bSize = numLen*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(ret->INH_String.bytes,sizeof(char)*ret->bSize);
    ret->INH_String.byteLength = numLen;
    strcpy(ret->INH_String.bytes,buff);
    return ret;
}
//Initializes a string with the given integer
char rfStringX_Init_i(RF_StringX* str,int32_t i)
{
    //the size of the int32_t buffer
    int32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%d",i);
    numLen = strlen(buff)+1;

    str->bSize = numLen*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(str->INH_String.bytes,sizeof(char)*str->bSize);
    str->INH_String.byteLength = numLen;
    strcpy(str->INH_String.bytes,buff);
    return true;
}
// Allocates and returns an extended string with the given float.
RF_StringX* rfStringX_Create_f(float f)
{
    //allocate a buffer for the float in characters
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);
    uint32_t len = strlen(buff);

    //initialize and return the string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    ret->INH_String.byteLength = len;
    ret->bSize = len*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;
    strcpy(ret->INH_String.bytes,buff);

    free(buff);

    return ret;
}
// Initializes an extended string with the given float.
char rfStringX_Init_f(RF_StringX* str,float f)
{
    //allocate a buffer for the float in characters
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);
    uint32_t len = strlen(buff);

    //initialize and return the string
    str->INH_String.byteLength = len;
    str->bSize = len*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;
    strcpy(str->INH_String.bytes,buff);

    free(buff);

    return true;
}


// Allocates and returns an extended String allocating buffSize bytes buffer and puts str inside the buffer
// If the contents of the string are bigger than the buffer Size then an error is logged but the String is initialized correctly
// Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* rfStringX_Create_buff(uint32_t buffSize,const char* lit,...)
#else
RF_StringX* i_rfStringX_Create_buff(uint32_t buffSize,const char* lit,...)
#endif
{
    RF_StringX* ret;
    READ_VSNPRINTF_ARGS(lit,lit,0)
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(buff,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        if(buffAllocated==true)
            free(buff);
        return 0;
    }
    //initialize the string
    RF_MALLOC(ret,sizeof(RF_StringX));
    ret->bSize = buffSize;

    //make sure that the buff size fits the string
    if(buffSize < byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);

        //set the buffer size to double the given string's length
        ret->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }

    //allocate and populate the data
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;
    ret->INH_String.byteLength = byteLength;
    strcpy(ret->INH_String.bytes,buff);
    if(buffAllocated == true)
        free(buff);
    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* i_NVrfStringX_Create_buff(uint32_t buffSize,const char* lit)
{
    RF_StringX* ret;
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(lit,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }
    //initialize the string
    RF_MALLOC(ret,sizeof(RF_StringX));
    ret->bSize = buffSize;
    //make sure that the buff size fits the string
    if(buffSize < byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);
        //set the buffer size to double the given string's length
        ret->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }
    //allocate and populate the data
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;
    ret->INH_String.byteLength = byteLength;
    strcpy(ret->INH_String.bytes,lit);
    return ret;
}
#endif
// Initializes an extended String allocating buffSize bytes buffer and puts str inside the buffer
// If the contents of the string are bigger than the buffer Size then an error is logged but the String is initialized correctly
// Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit,...)
#else
char i_rfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit,...)
#endif
{
    READ_VSNPRINTF_ARGS(lit,lit,false)
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(buff,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        if(buffAllocated == true)
            free(buff);
        return false;
    }
    //get the buffer size
    str->bSize = buffSize;
    //make sure that the buff size fits the string
    if(buffSize < byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);
        //set the buffer size to double the given string's length
        str->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }
    //allocate and populate the data
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;
    str->INH_String.byteLength = byteLength;
    strcpy(str->INH_String.bytes,buff);
    if(buffAllocated == true)
        free(buff);
    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(lit,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //get the buffer size
    str->bSize = buffSize;

    //make sure that the buff size fits the string
    if(buffSize < byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);

        //set the buffer size to double the given string's length
        str->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }

    //allocate and populate the data
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;
    str->INH_String.byteLength = byteLength;
    strcpy(str->INH_String.bytes,lit);
    return true;
}
#endif


//Allocates and returns a string with the given UTF-16 byte sequence. Given characters have to be in UTF-16. A check for valid sequence of bytes is performed
RF_StringX* rfStringX_Create_UTF16(const uint16_t* s)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_UTF16(ret,s)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes a string with the given UTF-16 byte sequence. Given characters have to be in UTF-16. A check for valid sequence of bytes is performed
char rfStringX_Init_UTF16(RF_StringX* str,const uint16_t* s)
{
    //decode the utf-16 and get the code points
    uint32_t* codepoints;
    uint32_t byteLength,characterLength,utf8ByteLength;
    char* utf8;
    byteLength = 0;
    while(s[byteLength]!= 0 || s[byteLength+1]!=0)
    {
        byteLength++;
    }
    byteLength+=3;//for the last utf-16 null termination character
    RF_MALLOC(codepoints,byteLength*2) //allocate the codepoints
    //decode the byte stream into codepoints
    if(rfUTF16_Decode((char*)s,&characterLength,codepoints)==false)
    {
        free(codepoints);
        LOG_ERROR("StringX initialization failed due to invalide UTF-16 sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //now encode these codepoints into UTF8
    if( (utf8 = rfUTF8_Encode(codepoints,characterLength,&utf8ByteLength))==0)
    {
        free(codepoints);
        return false;
    }
    //success
    free(codepoints);
    str->INH_String.bytes = utf8;
    str->bIndex = 0;
    str->bSize = utf8ByteLength+1;
    str->INH_String.byteLength = utf8ByteLength;
    return true;


}

//Allocates and returns a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32
RF_StringX* rfStringX_Create_UTF32(const uint32_t* s)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_UTF32(ret,s)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32
char rfStringX_Init_UTF32(RF_StringX* str,const uint32_t* codeBuffer)
{
    //find the length of the utf32 buffer in characters
    uint32_t length;
    rfUTF32_Length(codeBuffer,length);
    //turn the codepoints into a utf-8 encoded buffer
    char* utf8;uint32_t utf8ByteLength;
    if((utf8=rfUTF8_Encode(codeBuffer,length,&utf8ByteLength)) == 0)
    {
        LOG_ERROR("Could not encode UTF32 codepoints into a UTF7 bytestream",RE_UTF8_ENCODING)
        return false;//error
    }

    str->INH_String.bytes = (char*)utf8;
    str->bIndex = 0;
    str->bSize = utf8ByteLength+1;
    str->INH_String.byteLength = utf8ByteLength;
    return true;
}

// Allocates and returns an extended String. NO VALID-UTF8 check is performed
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
struct RF_StringX* rfStringX_Create_nc(const char* lit,...)
#else
struct RF_StringX* i_rfStringX_Create_nc(const char* lit,...)
#endif
{
    //initialize the string
    RF_StringX* ret;
    READ_VSNPRINTF_ARGS(lit,lit,0)
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength = strlen(buff);
    //allocate and populate the data
    ret->bSize = ret->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,lit);
    ret->bIndex = 0;
    if(buffAllocated == true)
        free(buff);
    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
// Allocates and returns an extended String. NO VALID-UTF8 check is performed
struct RF_StringX* i_NVrfStringX_Create_nc(const char* lit)
{
    //initialize the string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength = strlen(lit);
    //allocate and populate the data
    ret->bSize = ret->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,lit);
    ret->bIndex = 0;

    return ret;
}
#endif
// Initializes an extended String. NO VALID-UTF8 check is performed
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_Init_nc(struct RF_StringX* str,const char* lit,...)
#else
char i_rfStringX_Init_nc(struct RF_StringX* str,const char* lit,...)
#endif
{
    READ_VSNPRINTF_ARGS(lit,lit,false)
    //get bytelength
    str->INH_String.byteLength = strlen(buff);
    //allocate and populate the data
    str->bSize = str->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,buff);
    str->bIndex = 0;
    if(buffAllocated == true)
        free(buff);
    //success
    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfStringX_Init_nc(struct RF_StringX* str,const char* lit)
{
    //get bytelength
    str->INH_String.byteLength = strlen(lit);
    //allocate and populate the data
    str->bSize = str->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,lit);
    str->bIndex = 0;
    //success
    return true;
}
#endif

/*-------------------------------------------------------------------------Methods to copy/assign an RF_StringX-------------------------------------------------------------------------------*/

// Assigns the value of the source string to the destination extended string.Both strings should already be initialized and hold a value. It is an error to give null parameters.
void i_rfStringX_Assign(RF_StringX* dest,void* sourceP)
{
    RF_String* source = (RF_String*)sourceP;
    rfStringX_Reset(dest);//make sure that the destination's byte index is reset
    //only if the new string value won't fit in the buffer reallocate the buffer
    if(source->byteLength+1 >= dest->bSize)
    {
        dest->bSize = source->byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_REALLOC(dest->INH_String.bytes,char,dest->bSize);
    }

    //now copy the value and the bytelength
    memcpy(dest->INH_String.bytes,source->bytes,source->byteLength+1);
    dest->INH_String.byteLength = source->byteLength;
}

//Assigns the value of a unicode character to the string
char rfStringX_Assign_char(RF_StringX* str,uint32_t codepoint)
{
    //reset the buffer if needed
    rfStringX_Reset(str);
    //realloc if needed
    if(str->bSize <5)
    {
        RF_REALLOC(str->INH_String.bytes,char,5*RF_OPTION_STRINGX_CAPACITY_M+1);
    }
    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        str->INH_String.bytes[0] = codepoint;
        str->INH_String.bytes[1] = '\0';
        str->INH_String.byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->INH_String.bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->INH_String.bytes[2] = '\0';
        str->INH_String.byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->INH_String.bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->INH_String.bytes[3] = '\0';
        str->INH_String.byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->INH_String.bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->INH_String.bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->INH_String.bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->INH_String.bytes[4] = '\0';
        str->INH_String.byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into an extended string",RE_UTF8_INVALID_CODE_POINT);
        return false;
    }

    return true;
}


// Returns an RF_StringX version of the parameter RF_String
RF_StringX* rfStringX_FromString_OUT(RF_String* s)
{
    //create the new string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //copy the bytes
    ret->bSize = s->byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    memcpy(ret->INH_String.bytes,s->bytes,s->byteLength+1);
    ret->bIndex = 0;
    ret->INH_String.byteLength = s->byteLength;
    return ret;
}
//Initializes an RF_StringX from an RF_string
void rfStringX_FromString_IN(RF_StringX* dst,RF_String* src)
{
    //allocate the stringX
    dst->bIndex = 0;
    dst->bSize = src->byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(dst->INH_String.bytes,dst->bSize)
    dst->INH_String.byteLength = src->byteLength;
    //copy the bytes from the String
    memcpy(dst->INH_String.bytes,src->bytes,src->byteLength+1);
}


//Creates a copy of an extended String and returns it
RF_StringX* rfStringX_Copy_OUT(RF_StringX* s)
{
    //create the new string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //allocate the new string
    RF_MALLOC(ret->INH_String.bytes,s->bSize);
    //before copying make sure that the source string is copied from the beginning of its buffer
    s->INH_String.bytes -= s->bIndex;
    memcpy(ret->INH_String.bytes,s->INH_String.bytes,s->bSize);
    //put the s->bytes back and put the copied bytes buffer to the same inex
    s->INH_String.bytes += s->bIndex;
    ret->INH_String.bytes += s->bIndex;
    //copy the parameters
    ret->bSize = s->bSize;
    ret->bIndex = s->bIndex;
    ret->INH_String.byteLength = s->INH_String.byteLength;
    return ret;
}
//Copies the given source string into the destination string.
void rfStringX_Copy_IN(RF_StringX* dst,RF_StringX* src)
{
    //before copying make sure that the source string is copied from the beginning of its buffer
    src->INH_String.bytes -= src->bIndex;
    //copy the bytes
    RF_MALLOC(dst->INH_String.bytes,src->bSize);
    memcpy(dst->INH_String.bytes,src->INH_String.bytes,src->bSize);
    //pur the source bytes back and also the new copied bytes buffer to the same index
    src->INH_String.bytes += src->bIndex;
    dst->INH_String.bytes += src->bIndex;
    //copy the parameters
    dst->bSize = src->bSize;
    dst->bIndex = src->bIndex;
    dst->INH_String.byteLength = src->INH_String.byteLength;

}
// Copies a certain number of characters from an RF_StringX
void rfStringX_Copy_chars(RF_StringX* dst,RF_StringX* src,uint32_t charsN)
{
    uint32_t i=0,bytePos;
    //before copying make sure that the source string is copied from the beginning of its buffer
    src->INH_String.bytes -= src->bIndex;
    //copy the bytes
    RF_MALLOC(dst->INH_String.bytes,src->bSize);
    memcpy(dst->INH_String.bytes,src->INH_String.bytes,src->bSize);
    //pur the source bytes back and also the new copied bytes buffer to the same index
    src->INH_String.bytes += src->bIndex;
    dst->INH_String.bytes += src->bIndex;
    //copy the parameters
    dst->bSize = src->bSize;
    dst->bIndex = src->bIndex;
    dst->INH_String.byteLength = src->INH_String.byteLength;
    //find the byte position and end the string there
    RF_STRING_ITERATE_START(src,i,bytePos)
    if(i == charsN)
        break;
    RF_STRING_ITERATE_END(i,bytePos)
    dst->INH_String.byteLength = bytePos;
    dst->INH_String.bytes[bytePos] = '\0';
}


/*-------------------------------------------------------------------------Methods to get rid of an RF_StringX-------------------------------------------------------------------------------*/

//Destroys an extended String and releases its memory.It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
void rfStringX_Destroy(RF_StringX* s)
{
    //an extended string can have moved its internal pointer forward, so we have to put it back at the origin to destroy the whole string freeing all the memory properly
    s->INH_String.bytes -= s->bIndex;
    free(s->INH_String.bytes);
    free(s);
}
//Destroys only the contents of the Extended string without freeing the pointer itself.It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
void rfStringX_Deinit(RF_StringX* s)
{
    //an extended string can have moved its internal pointer forward, so we have to put it back at the origin to destroy the whole string freeing all the memory properly
    s->INH_String.bytes -= s->bIndex;
    free(s->INH_String.bytes);
}

/*-------------------------------------------------------------------------Functions to manipulate an RF_StringX-------------------------------------------------------------------------------*/

// Appends the parameter String to this extended string.
void i_rfStringX_Append(RF_StringX* thisstr,void* otherP)
{
    RF_String* other = (RF_String*)otherP;
    ///@note Here if a null addition is given lots of actions are done but the result is safe and the same string as the one entered.
    ///A check here would result in an additional check for every appending so I decided against it
    //get this string's  buffer's remaining size
    uint32_t remSize = RF_STRINGX_REMAINING_SIZE(thisstr);

    //calculate the new byte length
    thisstr->INH_String.byteLength += other->byteLength;

    //if it does not fit inside the remaining size
    if(remSize <= other->byteLength)
    {
        //reallocate this string to fit the new addition
        thisstr->bSize = thisstr->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_REALLOC(thisstr->INH_String.bytes,char,thisstr->bSize);
    }

    //add the string to this one
    strncat(thisstr->INH_String.bytes,other->bytes,other->byteLength);
}

// Appends an integer to the extended string
void rfStringX_Append_i(RF_StringX* thisstr,const int32_t i)
{
    //the size of the int32_t buffer
    uint32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char* buff;
    RF_MALLOC(buff,14);//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%i",i);

    //get the size of the int32_t
    numLen= strlen(buff);
    //get this string's  buffer's remaining size and the string's byte size
    uint32_t remSize = RF_STRINGX_REMAINING_SIZE(thisstr);

    //get the new bytelength
    thisstr->INH_String.byteLength += numLen;

    //if it does not fit inside the remaining size
    if(remSize <= numLen)
    {
        //reallocate this string to fit the new addition
        thisstr->bSize= thisstr->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_REALLOC(thisstr->INH_String.bytes,char,thisstr->bSize);
    }

    //copy the value of the olds string and the number string into the new string
    strncat(thisstr->INH_String.bytes,buff,numLen);

    //free the buffer
    free(buff);
}
// Appends a float to the extended string
void rfStringX_Append_f(RF_StringX* thisstr,const float f)
{
    uint32_t len;
    //a temporary buffer to hold the float
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);

    //get the size
    len = strlen(buff);
    //calculate the new length
    thisstr->INH_String.byteLength += len;
    //get this string's  buffer's remaining size and the string's byte size
    uint32_t remSize = RF_STRINGX_REMAINING_SIZE(thisstr);

    //if it does not fit inside the remaining size
    if(remSize <= len)
    {
        //reallocate this string to fit the new addition
        thisstr->bSize= thisstr->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        //Reallocate this string to fit the new addition
        RF_REALLOC(thisstr->INH_String.bytes,char,thisstr->bSize);
    }


    //copy the value of the olds string and the number string into the new string
    strncat(thisstr->INH_String.bytes,buff,len);
    //free the buffer
    free(buff);
}


// Prepends the parameter String to this extended string
void i_rfStringX_Prepend(RF_StringX* thisstr,void* otherP)
{
    RF_String* other = (RF_String*)otherP;
    int32_t i;//is not unsigned since it goes to -1 in the loop
    //get this string's  buffer's remaining size and the string's byte size
    uint32_t remSize = RF_STRINGX_REMAINING_SIZE(thisstr);

    //keep the previous byte length
    uint32_t size = thisstr->INH_String.byteLength;
    //get the new byte length
    thisstr->INH_String.byteLength += other->byteLength;

    //if it does not fit inside the remaining size
    if(remSize <= other->byteLength)
    {
        //reallocate this string to fit the new addition
        thisstr->bSize= thisstr->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_REALLOC(thisstr->INH_String.bytes,char,thisstr->bSize);
    }

    //move the pre-existing string to the end of the buffer, by dislocating each byte by cstrlen
    for(i = size; i >= 0; i --)
        thisstr->INH_String.bytes[i+other->byteLength] = thisstr->INH_String.bytes[i];

    //and now add the new string to the start
    memcpy(thisstr->INH_String.bytes,other->bytes,other->byteLength);
}

// Inserts a string to this extended string at the parameter position.
void i_rfStringX_Insert(RF_StringX* thisstr,uint32_t* posP,void* otherP)
{
    RF_String* other = (RF_String*)otherP;
    uint32_t length,bytePos,i,size,pos;
    pos = *posP;
    char found = false;
    //get this string's  buffer's remaining size and the string's byte size
    uint32_t remSize = RF_STRINGX_REMAINING_SIZE(thisstr);

    //keep the original byte length here
    size = thisstr->INH_String.byteLength;
    //get the new byte length
    thisstr->INH_String.byteLength += other->byteLength;

    //if the new String does not fit
    if(remSize <= other->byteLength)
    {
        //reallocate this string to fit the additiona
        thisstr->bSize = thisstr->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_REALLOC(thisstr->INH_String.bytes,char,thisstr->bSize);
    }

    //iterate this string to find the byte position of the character position
    RF_STRING_ITERATE_START(thisstr,length,i)
        if(length == pos)
        {
            //found the position. Is saved in bytePos
            bytePos = i;
            found = true;
            break;
        }
    RF_STRING_ITERATE_END(length,i);

    //if the position is not in the string do nothing
    if(found == false)
        return;

    //copy the part of the string after the insertion in a temporary buffer
    char* temp;
    RF_MALLOC(temp,size-bytePos+1);
    strcpy(temp,thisstr->INH_String.bytes+bytePos);

    //now insert the new string
    strncpy(thisstr->INH_String.bytes+bytePos,other->bytes,other->byteLength);
    //and to the remaining part insert the rest of the string stored in the temporary buffer
    strcpy(thisstr->INH_String.bytes+(bytePos+other->byteLength),temp);

    //finally do not forget to free the temporary buffer
    free(temp);
}

// Replaces all of the specified sstr substring from the extended String with string rstr
char i_rfStringX_Replace(RF_StringX* thisstr,void* sstrP,void* rstrP,const uint32_t* numP,const char* optionsP)
{
    RF_String* sstr = (RF_String*)sstrP;
    RF_String* rstr = (RF_String*)rstrP;
    uint32_t num = *numP;
    char options = *optionsP;
    //will keep the number of found instances of the substring
    uint32_t foundN = 0;
    uint32_t diff,i,j,start;
    //if the substring string is not even found return false
    if(rfString_FindBytePos(thisstr,sstr,options) == RF_FAILURE)
        return false;

    //create a buffer that will keep the byte positions
    uint32_t bSize = 50;
    int32_t * bytePositions;
    RF_MALLOC(bytePositions,bSize*sizeof(int32_t));
    //if the given num is 0 just make sure we replace all
    if(num == 0)
        num = ULONG_MAX;//max number of occurences

    //find how many occurences exist but also remember the previous position of the internal pointer
    start = thisstr->bIndex;
    while( (bytePositions[foundN] = rfString_FindBytePos(thisstr,sstr,options))  != RF_FAILURE)
    {
        int32_t move = bytePositions[foundN] + sstr->byteLength;
        bytePositions[foundN] = bytePositions[foundN]+thisstr->bIndex-start;
        thisstr->bIndex += move;
        thisstr->INH_String.bytes += move;
        thisstr->INH_String.byteLength -= move;
        foundN++;
        //if buffer is in danger of overflow realloc it
        if(foundN > bSize)
        {
            bSize *=2;
            RF_REALLOC(bytePositions,int32_t,bSize);
        }
        //if we found the required number of occurences break;
        if(foundN >= num)
            break;
    }
    //move the internal pointer back since we are done searching
    thisstr->INH_String.bytes -= thisstr->bIndex-start;
    thisstr->INH_String.byteLength += thisstr->bIndex-start;
    thisstr->bIndex = start;
    //make sure that the number of occurences to replace do not exceed the actual number of occurences
    if(num > foundN)
        num = foundN;

    //act depending on the size difference of rstr and sstr
    if(rstr->byteLength > sstr->byteLength) //replace string is bigger than the removed one
    {
        int32_t orSize;
        diff = rstr->byteLength - sstr->byteLength;
        //will keep the original size in bytes
        orSize = thisstr->INH_String.byteLength +1;
        //reallocate the string to fit the new bigger size if needed
        if( orSize + num*diff > thisstr->bSize)
        {
            thisstr->bSize = (orSize + num*diff)*RF_OPTION_STRINGX_CAPACITY_M;
            RF_REALLOC(thisstr->INH_String.bytes,char,thisstr->bSize)
        }
        //now replace all the substrings one by one
        for(i = 0; i < num; i ++)
        {
            //move all of the contents of the string to fit the replacement
            for(j =orSize+diff-1; j > bytePositions[i]+sstr->byteLength; j -- )
                thisstr->INH_String.bytes[j] = thisstr->INH_String.bytes[j-diff];

            //copy in the replacement
            strncpy(thisstr->INH_String.bytes+bytePositions[i],rstr->bytes,rstr->byteLength);
            //also increase the original size (since now we moved the whole string by one replacement)
            orSize += diff;
            //also increase all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < num; j ++)
                bytePositions[j] = bytePositions[j]+diff;
        }
        //finally let's keep the new byte length
        thisstr->INH_String.byteLength = orSize-1;
    }
    else if( rstr->byteLength < sstr->byteLength) //replace string is smaller than the removed one
    {
        //get the differenc in byte length of removed substring and replace string
        diff = sstr->byteLength-rstr->byteLength;

        //now replace all the substrings one by one
        for(i =0; i < num; i ++)
        {
            //copy in the replacement
            strncpy(thisstr->INH_String.bytes+bytePositions[i],rstr->bytes,rstr->byteLength);
            //move all of the contents of the string to fit the replacement
            for(j =bytePositions[i]+rstr->byteLength; j < thisstr->INH_String.byteLength; j ++ )
                thisstr->INH_String.bytes[j] = thisstr->INH_String.bytes[j+diff];
            //also decrease all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < num; j ++)
                bytePositions[j] = bytePositions[j]-diff;
        }
        //finally let's keep the new byte length
        thisstr->INH_String.byteLength -= diff*num;
        //just note that reallocating downwards is not necessary
    }
    else //replace and remove strings are equal
    {
        for(i = 0; i < num; i ++)
            strncpy(thisstr->INH_String.bytes+bytePositions[i],rstr->bytes,rstr->byteLength);
    }

    free(bytePositions);
    return true;
}

//Replaces what exists between the ith left and right substrings of this extended String.Utilizes the internal string pointer.
char i_rfStringX_ReplaceBetween(RF_StringX* thisstr,void* leftP,void* rightP,void* rstrP,char* optionsP,uint32_t* iP)
{
    RF_String* left = (RF_String*)leftP;
    RF_String* right = (RF_String*)rightP;
    RF_String* rstr = (RF_String*)rstrP;
    char options = *optionsP;
    uint32_t i = *iP;
    uint32_t j,move,start = thisstr->bIndex;
    char found = false;
    RF_String ss;

    if(i==0)//if we want all occurences replaced
    {
        //while we have occurences of the pair
        while(rfString_Between(thisstr,left,right,&ss,options) != false)
        {
            found = true;
            //move the internal pointer right after the left part of the pair and replace what was found there
            //no of the functions below fail since the while condition is true
            rfStringX_MoveAfter(thisstr,left,0,options);
            rfStringX_Replace(thisstr,&ss,rstr,1,options);
            //also move after the right
            move = rstr->byteLength+right->byteLength;
            thisstr->bIndex += move;
            thisstr->INH_String.bytes += move;
            thisstr->INH_String.byteLength -= move;
            //free temp string before next iteration
            rfString_Deinit(&ss);
        }

        if(found)//if we replaced at least 1 occurence success and get the internal pointer back
        {
            move = thisstr->bIndex-start;
            thisstr->INH_String.bytes -= move;
            thisstr->bIndex = start;
            thisstr->INH_String.byteLength += move;
            return true;
        }
        return false;
    }

    ///if we check for a specific instance
    for(j = 1; j < i;j++)
    {
        //move after the pair of the 'j' inbetween substrings
        if(rfStringX_MoveAfterPair(thisstr,left,right,0,options) ==false)
        {
            //and if the occurence does not existget the string back to where it should be
            move = thisstr->bIndex-start;
            thisstr->INH_String.bytes -= move;
            thisstr->bIndex = start;
            //and increase its bytelength
            thisstr->INH_String.byteLength += move;
            return false; //and return failure
        }
    }
    //move after the pair of the inbetween substrings we actually want
    if(rfString_Between(thisstr,left,right,&ss,options) == false)
    {
        //and if the occurence does not existget the string back to where it should be
        move = thisstr->bIndex-start;
        thisstr->INH_String.bytes -= move;
        thisstr->bIndex = start;
        //and increase its bytelength
        thisstr->INH_String.byteLength += move;
        return false; //and return failure
    }
    ///success
    //move after the left part of the pair
    rfStringX_MoveAfter(thisstr,left,0,options);
    //and then replace the occurence
    rfStringX_Replace(thisstr,&ss,rstr,1,options);
    //now we are done and should go back
    rfString_Deinit(&ss);
    move = thisstr->bIndex-start;
    thisstr->INH_String.bytes -= move;
    thisstr->bIndex = start;
    thisstr->INH_String.byteLength += move;
    return true;

}
/*-------------------------------------------------------------------------Functions to traverse an RF_StringX-------------------------------------------------------------------------------*/


//Moves the internal pointer right after the substring
int32_t i_rfStringX_MoveAfter(RF_StringX* thisstr,void* subP,RF_StringX* result,const char* optionsP)
{
    RF_String* sub = (RF_String*)subP;
    char options = *optionsP;
    int32_t move;
    //check for substring existence and return failure if not found
    if( (move = rfString_FindBytePos(thisstr,sub,options)) == RF_FAILURE)
        return RF_FAILURE;
    //if found, move the internal pointer
    move += sub->byteLength;
    thisstr->bIndex += move;
    thisstr->INH_String.bytes+=move;
    //also reduce the byte length of this string
    thisstr->INH_String.byteLength -= move;

    //also if we want the string returned
    if(result != 0)
    {
        result->INH_String.byteLength = move-sub->byteLength;
        result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_MALLOC(result->INH_String.bytes,result->bSize);
        result->bIndex = 0;
        memcpy(result->INH_String.bytes,thisstr->INH_String.bytes-move,result->INH_String.byteLength);
        result->INH_String.bytes[result->INH_String.byteLength] = '\0';
    }
    //return positions moved
    return move;
}

// Moves the internal pointer n characters back. It never goes before the original start of the string so there is no worry of memory corruption.
void rfStringX_MoveBack(RF_StringX* thisstr,uint32_t n)
{
    uint32_t length;
    length = 0;

    //as long as the bIndex is not zero keep reducing the internal pointer and the bytes pointer
    while(thisstr->bIndex >0)
    {
        if(rfUTF8_IsContinuationByte(thisstr->INH_String.bytes[0]) == false)
        {
            if(n == length)
            {
                break;
            }
            length ++;
        }
        //so keep changing the internal index and the bytes pointer
        thisstr->INH_String.bytes--;
        thisstr->bIndex--;
        //and also keep adding to the bytelength
        thisstr->INH_String.byteLength++;
    }

}


//Moves the internal pointer n characters forward. It never goes beyond the string's length
void rfStringX_MoveForward(RF_StringX* thisstr,uint32_t n)
{
    uint32_t length;
    length = 0;

    //as long as the bIndex is not zero keep reducing the internal pointer and the bytes pointer
    while(thisstr->bIndex < thisstr->bSize)
    {
        if(rfUTF8_IsContinuationByte(thisstr->INH_String.bytes[0]) == false)
        {
            if(n == length)
            {
                break;
            }
            length ++;
        }
        //so keep changing the internal index and the bytes pointer
        thisstr->INH_String.bytes++;
        thisstr->bIndex++;
        //and reducing from the byte length
        thisstr->INH_String.byteLength--;
    }
}

//Resets the internal pointer of the StringX
void rfStringX_Reset(RF_StringX* thisstr)
{
    thisstr->INH_String.bytes -= thisstr->bIndex;
    thisstr->INH_String.byteLength += thisstr->bIndex;
    thisstr->bIndex = 0;
}

//Moves the internal pointer after the first occurence of any of the given substrings
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_rfStringX_MoveAfterv(RF_StringX* thisstr,RF_StringX* result,const char* optionsP,const unsigned char* parNP, ...)
#else
char rfStringX_MoveAfterv(RF_StringX* thisstr,RF_StringX* result,const char* optionsP,const unsigned char* parNP, ...)
#endif
{
    uint32_t i,paramLen,move;
    int32_t minPos;
    int32_t thisPos;
    char options = *optionsP;
    unsigned char parN = *parNP;
    //will keep the argument list
    va_list argList;

    // will keep the winning parameter length
    paramLen = 0;
    //get the parameter characters
    va_start(argList,parNP);
    minPos = LONG_MAX;
    for(i = 0; i < parN; i++)
    {
        //get the param
        RF_String* s = va_arg(argList,RF_String*);
        //if the parameter got found in the string see if it's the closest
        if( (thisPos = rfString_FindBytePos(thisstr,s,options))!= RF_FAILURE)
        {
            if( thisPos< minPos)
            {
                minPos = thisPos;
                //keep the length of the winnning parameter
                paramLen = s->byteLength;
            }
        }
    }
    va_end(argList);

    //if it is not found
    if(minPos == LONG_MAX)
        return false;

    //move the internal pointer after the substring
    move = minPos + paramLen;
    thisstr->bIndex += move;
    thisstr->INH_String.bytes += move;
    //and reduce its bytelength
    thisstr->INH_String.byteLength -= move;

    //initialize the given string if needed
    if(result != 0)
    {
        result->INH_String.byteLength = move-paramLen;
        result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        RF_MALLOC(result->INH_String.bytes,result->bSize);
        result->bIndex = 0;
        memcpy(result->INH_String.bytes,thisstr->INH_String.bytes-move,result->INH_String.byteLength);
        result->INH_String.bytes[result->INH_String.byteLength] = '\0';
    }
    return true;
}
//Moves the internal string pointer after the substring formed by the @c left and @c right substrings
char i_rfStringX_MoveAfterPair(RF_StringX* thisstr,void* leftP,void* rightP,RF_StringX* result,char* optionsP,uint32_t* occurenceP)
{
    RF_StringX r;
    char options = *optionsP;
    uint32_t occurence = *occurenceP;
    uint32_t i,move,start = thisstr->bIndex;
    char found = false;
    RF_String* left = (RF_String*)leftP;
    RF_String* right = (RF_String*)rightP;
    //check the occurence parameter
    if(occurence == 0)
        occurence =1;


    //get the in between string and if it is null return false
    for(i = 1; i <= occurence; i ++)
    {
        //attempt to get the in between string
        if(rfString_Between(thisstr,left,right,&r,options) == false)
            return false;

        //move after this occurence of the pair
        rfStringX_MoveAfter(thisstr,left,0,options);
        rfStringX_MoveAfter(thisstr,right,0,options);

        //if we found it
        if(i == occurence)
        {
            found = true;
            break;
        }
        //else deinit the temporary string
        rfStringX_Deinit(&r);
    }
    //if we get here and the result is not found return failure
    if(found == false)
    {
        rfStringX_Deinit(&r);
        //get the pointer back
        move =thisstr->bIndex - start;
        thisstr->INH_String.bytes -= move;
        thisstr->INH_String.byteLength += move;
        thisstr->bIndex = start;
        return false;
    }
    //if we want to keep the result
    if(result != 0)
    {
        result->INH_String.byteLength = r.INH_String.byteLength;
        result->INH_String.bytes = r.INH_String.bytes;
        result->bIndex = 0;
        result->bSize = r.INH_String.byteLength+1;
    }
    else//deinit the temp string
        rfStringX_Deinit(&r);
    //success
    return true;
}
/*------------------------------------------------------------------------- File I/O RF_StringX Functions -------------------------------------------------------------------------------*/

//Allocates and returns an extended string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
RF_StringX* rfStringX_Create_fUTF8(FILE* f, char* eof)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_fUTF8(ret,f,eof) < 0)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes an extended string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
int32_t rfStringX_Init_fUTF8(RF_StringX* str,FILE* f,char* eof)
{
    int32_t bytesN;
    if((bytesN=rfFReadLine_UTF8(f,&str->INH_String.bytes,&str->INH_String.byteLength,&str->bSize,eof)) < 0)
    {
        LOG_ERROR("Failed to initialize StringX from a UTF-8 file",bytesN);
        return bytesN;
    }
    //success
    str->bIndex = 0;
    return bytesN;
}
//Assigns to an extended String from UTF-8 file parsing
int32_t rfStringX_Assign_fUTF8(RF_StringX* str,FILE*f,char* eof)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN=rfFReadLine_UTF8(f,&utf8,&utf8ByteLength,&utf8BufferSize,eof)) < 0)
    {
        LOG_ERROR("Failed to assign the contents of a UTF-8 file to a StringX",bytesN);
        return bytesN;
    }
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC(str->INH_String.bytes,char,str->bSize);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
//Appends to an extended String from UTF-8 file parsing
int32_t rfStringX_Append_fUTF8(RF_StringX* str,FILE*f,char* eof)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN=rfFReadLine_UTF8(f,&utf8,&utf8ByteLength,&utf8BufferSize,eof)) < 0)
    {
        LOG_ERROR("Failed to assign the contents of a UTF-8 file to a StringX",bytesN);
        return bytesN;
    }
    //append the utf8 to the given string
    rfStringX_Append(str,RFS_(utf8));
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}

//Allocates and returns an extended string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_StringX* rfStringX_Create_fUTF16(FILE* f,char endianess,char* eof)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_fUTF16(ret,f,endianess,eof) < 0)
        return 0;
    return ret;
}
//Initializes an extended string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfStringX_Init_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to initialize a StringX from reading a UTF-16 file",bytesN);
            return bytesN;
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to initialize a StringX from reading a UTF-16 file",bytesN);
            return bytesN;
        }
    }//end of big endian case
    //success
    str->bIndex = 0;
    str->bSize = 0;
    return bytesN;
}

//Assigns to an already initialized String from File parsing
int32_t rfStringX_Assign_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof)
{
    uint32_t utf8ByteLength;
    int32_t bytesN;
    char* utf8 = 0;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to assign the contents of a Little Endian UTF-16 file to a StringX",bytesN);
            return bytesN;
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to assign the contents of a Big Endian UTF-16 file to a StringX",bytesN);
            return bytesN;
        }
    }//end of big endian case
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC(str->INH_String.bytes,char,str->bSize);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}

//Appends to an already initialized String from File parsing
int32_t rfStringX_Append_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof)
{
    char*utf8;
    uint32_t utf8ByteLength;
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to append the contents of a Little Endian UTF-16 file to a StringX",bytesN);
            return bytesN;
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to append the contents of a Big Endian UTF-16 file to a StringX",bytesN);
            return bytesN;
        }
    }//end of big endian case
    //success
    rfStringX_Append(str,RFS_(utf8));
    free(utf8);
    return bytesN;
}

// Allocates and returns an extended string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_StringX* rfStringX_Create_fUTF32(FILE* f,char endianess,char* eof)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_fUTF32(ret,f,endianess,eof) < 0)
    {
        free(ret);
        return 0;
    }
    return ret;
}
// Initializes an extended string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfStringX_Init_fUTF32(RF_StringX* str,FILE* f,char endianess,char* eof)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) <0)
        {
            LOG_ERROR("Failure to initialize a StringX from reading a Little Endian UTF-32 file",bytesN);
            return bytesN;
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to initialize a StringX from reading a Big Endian UTF-32 file",bytesN);
            return bytesN;
        }
    }//end of big endian case
    //success
    str->bIndex = 0;
    str->bSize = 0;
    return bytesN;
}
//Assigns the contents of a UTF-32 file to an extended string
int32_t rfStringX_Assign_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to assign to a StringX from reading a Little Endian UTF-32 file",bytesN);
            return bytesN;
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to assign to a StringX from reading a Big Endian UTF-32 file",bytesN);
            return bytesN;
        }
    }//end of big endian case
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC(str->INH_String.bytes,char,str->bSize);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
//Appends the contents of a UTF-32 file to an extended string
int32_t rfStringX_Append_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to append to a StringX from reading a Little Endian UTF-32 file",bytesN);
            return bytesN;
        }
    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,&utf8,&utf8ByteLength,eof)) < 0)
        {
            LOG_ERROR("Failure to append to a StringX from reading a Big Endian UTF-32 file",bytesN);
            return bytesN;
        }
    }//end of big endian case
    //success
    //append it
    rfStringX_Append(str,RFS_(utf8));
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
