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
**/
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/imex.h>
#include <Definitions/types.h>
#include <String/unicode.h>
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <Utils/memory.h> //for refu memory allocation

#include <Utils/endianess.h> //endianess
#include <Utils/constcmp.h> //for RF_HEXGE_UI() macro and others
#include <Definitions/retcodes.h> //for return codes
//*---------------------libc Headers inclusion------------------------------------------
#include <string.h>//memcpy
//*----------------------------End of Includes------------------------------------------

//Takes a buffer of unicode character and turns them into a UTF-8 encoded string
char* rfUTF8_Encode(const uint32_t* codepoints,uint32_t charsN,uint32_t* byteLength)
{
    char* utf8;
    uint32_t charN;
    //allocate the utf8 buffer
    RF_MALLOC(utf8,(charsN+1)*4);
    *byteLength = charN = 0;
    /*Start iterating the codepoints until you find end of string*/
    while(codepoints[charN] != 0)
    {
        /*If the code point requires only 1 byte*/
        if(RF_HEXLE_UI(codepoints[charN],0x007f))
        {
            utf8[*byteLength] = codepoints[charN];
            (*byteLength)+=1;
        }
        /*If the code point requires two bytes*/
        else if( RF_HEXGE_UI(codepoints[charN],0x0080) && RF_HEXLE_UI(codepoints[charN],0x07ff))
        {
            /*get the first 6 bits of the msb byte and encode them to the second byte*/
            utf8[(*byteLength)+1] = (codepoints[charN] & 0x3F)|(0x02<<6);
            /*get the 5 following bits and encode them in the first byte*/
            utf8[*byteLength] = ((codepoints[charN] & 0x7C0) >> 6)  | (0x6<<5);
            (*byteLength)+=2;
        }
        /*if we need 3 bytes to encode it*/
        else if( RF_HEXGE_UI(codepoints[charN],0x0800) && RF_HEXLE_UI(codepoints[charN],0x0ffff))
        {
            /*get the first bits of the msb byte and encode them to the third byte*/
            utf8[(*byteLength)+2] = (codepoints[charN] & 0x3F)|(0x02<<6);
            /*get the 6 following bits and encode them in the second byte*/
            utf8[(*byteLength)+1] = ((codepoints[charN] & 0xFC0) >> 6)  | (0x02<<6);
            /*get the 4 following bits and encode them in the first byte*/
            utf8[*byteLength] = (((codepoints[charN] & 0xF000))>>12) | (0xE<<4);
            (*byteLength)+=3;
        }
        /*if we need 4 bytes to encode it*/
        else if( RF_HEXGE_UI(codepoints[charN],0x10000) && RF_HEXLE_UI(codepoints[charN],0x10ffff))
        {
            /*get the first bits of the msb byte and encode them to the fourth byte*/
            utf8[(*byteLength)+3] = (codepoints[charN] & 0x3F)|(0x02<<6);
            /*get the 6 following bits and encode them in the third byte*/
            utf8[(*byteLength)+2] = ((codepoints[charN] & 0xFC0) >> 6)  | (0x02<<6);
            /*get the 6 following bits and encode them in the second byte*/
            utf8[(*byteLength)+1] = (((codepoints[charN] & 0x3F000))>>12) | (0x02<<6);
            /*get the 3 following bits and encode them in the first byte*/
            utf8[*byteLength] = (((codepoints[charN] & 0x1C0000))>>18) | (0x1E<<3);
            (*byteLength)+=4;
        }
        else
        {
            LOG_ERROR("Attempted to encode an invalid unicode code point into a string",RE_UTF8_INVALID_CODE_POINT);
            free(utf8);
            utf8 = 0;
            return 0;
        }
        charN++;
    }/*end of the iteration loop*/
    ///success
    /*at the end realloc the buffer to its true (smaller) size and add the string termination character*/\
    /**@warning This realloc here is not very safe, since there is no check. If it fails, it destroys everything*/\
    utf8 =  realloc(utf8,(*byteLength)+1);
    utf8[*byteLength] = '\0';
    return utf8;
}
//akes a unicode codepoint and turns them into a UTF-8 byte
char rfUTF8_Encode_single(const uint32_t codepoint,char* utf8)
{
    int i = 0;
    /*If the code point requires only 1 byte*/
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        utf8[0] = codepoint;
        i=1;
    }
    /*If the code point requires two bytes*/
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        /*get the first 6 bits of the msb byte and encode them to the second byte*/
        utf8[1] = (codepoint & 0x3F)|(0x02<<6);
        /*get the 5 following bits and encode them in the first byte*/
        utf8[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        i=2;
    }
    /*if we need 3 bytes to encode it*/
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        /*get the first bits of the msb byte and encode them to the third byte*/
        utf8[2] = (codepoint & 0x3F)|(0x02<<6);
        /*get the 6 following bits and encode them in the second byte*/
        utf8[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        /*get the 4 following bits and encode them in the first byte*/
        utf8[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        i=3;
    }
    /*if we need 4 bytes to encode it*/
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        /*get the first bits of the msb byte and encode them to the fourth byte*/
        utf8[3] = (codepoint & 0x3F)|(0x02<<6);
        /*get the 6 following bits and encode them in the third byte*/
        utf8[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        /*get the 6 following bits and encode them in the second byte*/
        utf8[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        /*get the 3 following bits and encode them in the first byte*/
        utf8[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        i=4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into a string",RE_UTF8_INVALID_CODE_POINT);
        utf8[0] = '\0';
        return 0;
    }

    ///success
    utf8[i] = '\0';
    return i-1;
}

//Takes a utf8 buffer and decodes it into unicode codepoints
uint32_t* rfUTF8_Decode(const char* utf8,uint32_t utf8Length,uint32_t* charsN)
{
    uint32_t* codePoints,i=0;
    RF_MALLOC(codePoints,utf8Length*4+4);//allocate worst max amount of bytes that would be needed
    *charsN = 0;
    while(utf8[i] != 0)
    {
        //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
        if( ((utf8[i] & 0x80)>>7) == 0 )
        {
            //and the code point is this whole byte only
            codePoints[*charsN] = utf8[i];
            i++;
        }
        //if the leading bits are in the form of 0b110xxxxx then range is: U+0080 to U+07FF (2 bytes)
        else if ( RF_HEXEQ_C( ( (~(utf8[i] ^  0xC0))>>5),0x7) )
        {
            codePoints[*charsN] =0;
            //from the second byte take the first 6 bits
            codePoints[*charsN] = (utf8[i+1] & 0x3F) ;
            //from the first byte take the first 5 bits and put them in the start
            codePoints[*charsN] |= ((utf8[i] & 0x1F) << 6);
            i+=2;
        }
        //if the leading bits are in the form of 0b1110xxxx then range is U+0800 to U+FFFF  (3 bytes)
        else if( RF_HEXEQ_C( ( (~(utf8[i] ^ 0xE0))>>4),0xF) )
        {
            codePoints[*charsN] = 0;
            //from the third byte take the first 6 bits
            codePoints[*charsN] = (utf8[i+2] & 0x3F) ;
            //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
            codePoints[*charsN] |= ((utf8[i+1] & 0x3F) << 6);
            //from the first byte take the first 4 bits and put them to the left of the previous 6 bits
            codePoints[*charsN] |= ((utf8[i] & 0xF) << 12);
            i+=3;
        }
        //if the leading bits are in the form of 0b11110xxx then range is U+010000 to U+10FFFF (4 bytes)
        else if( RF_HEXEQ_C( ( (~(utf8[i] ^ 0xF0))>>3), 0x1F))
        {
            codePoints[*charsN] = 0;
            //from the fourth byte take the first 6 bits
            codePoints[*charsN] = (utf8[i+3] & 0x3F) ;
            //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
            codePoints[*charsN] |= ((utf8[i+2] & 0x3F) << 6);
            //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
            codePoints[*charsN] |= ((utf8[i+1] & 0x3F) << 12);
            //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
            codePoints[*charsN] |= ((utf8[i] & 0x7) << 18);
            i+=4;
        }
        (*charsN) +=1;
    }
    //null terminate it
    codePoints[*charsN] = 0;
    return codePoints;
}

// Parses a utf-8 byte sequence trying to verify if it is valid or not
int32_t rfUTF8_VerifySequence(const char* bytes,uint32_t * i)
{
    //i is the byte index
    *i=0;
    while(bytes[*i] != '\0')
    {
        //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
        if( ((bytes[*i] & 0x80)>>7) == 0 )
        {
            *i = *i+1;
        }
        //if the leading bits are in the form of 0b110xxxxx then range is: U+0080 to U+07FF (2 bytes)
        else if ( RF_HEXEQ_C( ( (~(bytes[*i] ^  0xC0))>>5), 0x7) )
        {
            //also remember bytes 0xC0 and 0xC1 are invalid and could possibly be found in a starting byte of this type so check for them here
            if( RF_HEXEQ_C(bytes[*i],0xC0) || RF_HEXEQ_C(bytes[*i],0xC1))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, an invalid byte was encountered",RE_UTF8_INVALID_SEQUENCE_INVALID_BYTE)

            //if the next byte is NOT a continuation byte
            if( !rfUTF8_IsContinuationByte(bytes[*i+1]))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, and expecting a continuation byte, one was not found",RE_UTF8_INVALID_SEQUENCE_CONBYTE)

            *i= *i+2;
        }
        //if the leading bits are in the form of 0b1110xxxx then range is U+0800 to U+FFFF  (3 bytes)
        else if( RF_HEXEQ_C( ( (~(bytes[*i] ^ 0xE0))>>4),0xF))
        {
            //if the string has invalid continuation bytes quit. They are in separate if checks to avoid checking outside of array bounds
            if(!rfUTF8_IsContinuationByte(bytes[*i+1]))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, and expecting a continuation byte, one was not found",RE_UTF8_INVALID_SEQUENCE_CONBYTE)

            if(!rfUTF8_IsContinuationByte(bytes[*i+2]))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, and expecting a continuation byte, one was not found",RE_UTF8_INVALID_SEQUENCE_CONBYTE)

            *i= *i+3;
        }
        //if the leading bits are in the form of 0b11110xxx then range is U+010000 to U+10FFFF (4 bytes)
        else if(RF_HEXEQ_C( ( (~(bytes[*i] ^ 0xF0))>>3), 0x1F))
        {
            //in this type of starting byte a number of invalid bytes can be encountered. We have to check for them.
            if(RF_HEXGE_C(bytes[*i],0xF5)) //invalid byte value are from 0xF5 to 0xFF
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, an invalid byte was encountered",RE_UTF8_INVALID_SEQUENCE_INVALID_BYTE)

            //if the string has invalid continuation bytes quit. They are in separate if checks to avoid checking outside of array bounds
            if(!rfUTF8_IsContinuationByte(bytes[*i+1]))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, and expecting a continuation byte, one was not found",RE_UTF8_INVALID_SEQUENCE_CONBYTE)

            if(!rfUTF8_IsContinuationByte(bytes[*i+2]))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, and expecting a continuation byte, one was not found",RE_UTF8_INVALID_SEQUENCE_CONBYTE)

            if(!rfUTF8_IsContinuationByte(bytes[*i+3]))
                RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, and expecting a continuation byte, one was not found",RE_UTF8_INVALID_SEQUENCE_CONBYTE)

            *i=*i+4;
        }
        else//we were expecting one of the 4 different start bytes and we did not find it, there is an error
            RETURN_LOG_ERROR("While decoding a UTF-8 byte sequence, the first byte of a character was not valid UTF-8",RE_UTF8_INVALID_SEQUENCE)

    }//end of iterating the bytes

    //if we get here it means the byte sequence is valid so return the number of characters found
    return RF_SUCCESS;
}
//Decodes a UTF-16  byte stream into codepoints
int32_t rfUTF16_Decode(const char* buff,uint32_t* charactersN,uint32_t* codepoints)
{
    uint16_t v1,v2;
    uint32_t byteLength,U;
    *charactersN = 0;
    byteLength = 0;
    //get the value of each character
    v1 = *((uint16_t*) (buff+byteLength));
    //iterate the bytes
    while(v1!=0)
    {
        /*If the value is in the surrogate area*/
        if(RF_HEXGE_US(v1,0xD800) && RF_HEXLE_US(v1,0xDFFF))
        {
            //determine if it's a valid first pair(between 0xD800 and 0xDBFF)
            if(RF_HEXG_US(v1,0xDBFF))
                RETURN_LOG_ERROR("Invalid 16-bit integer found in the given sequence. Can't decode.",RE_UTF16_INVALID_SEQUENCE)

            v2 = *((uint16_t*) (buff+byteLength+2));
            //determine if the surrogate pair is valid. Between 0xDC00 and 0xDFFF
            if(RF_HEXL_US(v2,0xDC00) || RF_HEXG_US(v2,0xDFFF))
            {
                if(v2 != 0)
                    RETURN_LOG_ERROR("Invalid surrogate pair found in the given sequence. Can't decode.",RE_UTF16_INVALID_SEQUENCE)
                else
                    RETURN_LOG_ERROR("A surrogate pair was expected and it was not found. Can't decode",RE_UTF16_NO_SURRPAIR)
            }
            /*now decode the surrogate pairs*/
            U = v2&(uint16_t)0x3ff;
            U |= (((uint32_t)(v1&(uint16_t)0x3ff))<<10);
            U += 0x10000;
            codepoints[*charactersN] = U;
            /*increment the index to the utf16 buffer for surrogate pair*/
            byteLength+=4;
        }
        else//no surrogate pair, this is the code point
        {
            codepoints[*charactersN] = (uint16_t)v1;
            byteLength+=2;
        }
        //increase the characters counter
        (*charactersN)+=1;
        //get the next value in the buffer
        v1 = *((uint16_t*) (buff+byteLength));
    }//end of byte iteration
    //null terminate the codepoints
    codepoints[*charactersN]=0;
    return RF_SUCCESS;
}
//Decodes a  UTF-16 byte stream into codepoints also swapping endianess
int32_t rfUTF16_Decode_swap(const char* buff,uint32_t* charactersN,uint32_t* codepoints)
{
    uint16_t v1,v2;
    uint32_t byteLength,U;
    *charactersN = 0;
    byteLength = 0;
    //get the value of each character
    v1 = *((uint16_t*) (buff+byteLength));
    rfSwapEndianUS(&v1);
    //iterate the bytes
    while(v1!=0)
    {
        /*If the value is in the surrogate area*/
        if(RF_HEXGE_US(v1,0xD800) && RF_HEXLE_US(v1,0xDFFF))
        {
            //determine if it's a valid first pair(between 0xD800 and 0xDBFF)
            if(RF_HEXG_US(v1,0xDBFF))
                RETURN_LOG_ERROR("Invalid 16-bit integer found in the given sequence. Can't decode.",RE_UTF16_INVALID_SEQUENCE)

            v2 = *((uint16_t*) (buff+byteLength+2));
            rfSwapEndianUS(&v2);
            //determine if the surrogate pair is valid. Between 0xDC00 and 0xDFFF
            if(RF_HEXL_US(v2,0xDC00) || RF_HEXG_US(v2,0xDFFF))
            {
                if(v2 != 0)
                    RETURN_LOG_ERROR("Invalid surrogate pair found in the given sequence. Can't decode.",RE_UTF16_INVALID_SEQUENCE)
                else
                    RETURN_LOG_ERROR("A surrogate pair was expected and it was not found. Can't decode",RE_UTF16_NO_SURRPAIR)
            }
            /*now decode the surrogate pairs*/
            U = v2&(uint16_t)0x3ff;
            U |= (((uint32_t)(v1&(uint16_t)0x3ff))<<10);
            U += 0x10000;
            codepoints[*charactersN] = U;
            /*increment the index to the utf16 buffer for surrogate pair*/
            byteLength+=4;
        }
        else//no surrogate pair, this is the code point
        {
            codepoints[*charactersN] = (uint16_t)v1;
            byteLength+=2;
        }
        //increase the characters counter
        (*charactersN)+=1;
        //get the next value in the buffer
        v1 = *((uint16_t*) (buff+byteLength));
    }//end of byte iteration
    //null terminate the codepoints
    codepoints[*charactersN]=0;
    return true;
}
//Encodes a buffer of unicode codepoints into UTF-16
uint16_t* rfUTF16_Encode(const uint32_t* codepoints,uint32_t charsN,uint32_t* length)
{
    uint32_t i,U;
    uint16_t* utf16;
    RF_MALLOC(utf16,4*charsN+2)//worst case scenario allocation
    //for all codepoints  (i -> charIndex , length -> byteIndex
    for(i = 0,*length=0; i < charsN; i ++)
    {
        if(RF_HEXG_UI(codepoints[i],0x10FFFF))
        {
            LOG_ERROR("While encoding unicode codepoints into a UTF-16 buffer a codepoint greater than 0x10FFFF was encountered",RE_UTF16_ENCODING);
            free(utf16);
            return 0;
        }
        if(RF_HEXL_UI(codepoints[i],0x10000))//if it is encoded in just one word
        {
            utf16[*length] = codepoints[i];
            (*length)+=1;
            continue;
        }
        U = codepoints[i]-(uint32_t)0x10000;
        utf16[*length] = (uint16_t)0xD800;
        utf16[(*length)+1] = (uint16_t)0xDC00;
        utf16[*length] |= (uint16_t)((U>>10)&(uint32_t)0x3FF);
        utf16[(*length)+1] |= (uint16_t)(U&(uint32_t)0x3FF);
        (*length)+=2;
    }
    //null terminate it
    utf16[(*length)] = 0;
    return utf16;
}

//Turns a unicode code point into utf8
char rfUTF8_FromCodepoint(uint32_t codepoint,uint32_t* utf8Int)
{
    char byteLength = 0;
    char* utf8 = (char*)utf8Int;
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        utf8[0] = codepoint;
        byteLength=1;
    }
    /*If the code point requires two bytes*/
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        /*get the first 6 bits of the msb byte and encode them to the second byte*/
        utf8[1] = (codepoint & 0x3F)|(0x02<<6);
        /*get the 5 following bits and encode them in the first byte*/
        utf8[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        byteLength=2;
    }
    /*if we need 3 bytes to encode it*/
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        /*get the first bits of the msb byte and encode them to the third byte*/
        utf8[2] = (codepoint & 0x3F)|(0x02<<6);
        /*get the 6 following bits and encode them in the second byte*/
        utf8[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        /*get the 4 following bits and encode them in the first byte*/
        utf8[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        byteLength=3;
    }
    /*if we need 4 bytes to encode it*/
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        /*get the first bits of the msb byte and encode them to the fourth byte*/
        utf8[3] = (codepoint & 0x3F)|(0x02<<6);
        /*get the 6 following bits and encode them in the third byte*/
        utf8[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        /*get the 6 following bits and encode them in the second byte*/
        utf8[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        /*get the 3 following bits and encode them in the first byte*/
        utf8[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        byteLength=4;
    }
    else
        LOG_ERROR("Attempted to encode an invalid unicode code point into a string",RE_UTF8_INVALID_CODE_POINT);
    return byteLength;
}
