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
#include <String/manipulationx.h>
/*------------- Module related inclusion -------------*/
#include <String/flags.h> //for the flags
#include <String/core.h> //for rfString_Deinit
#include <String/retrieval.h> //for rfString_Between and others
#include <String/traversalx.h> //for rfStringX_MoveAfter() and other functions
#include "common.ph" //for private string functions and macros
/*------------- Outside Module inclusion -------------*/
#include <String/unicode.h> //for unicode functions
//for the integer int and uint conversions
    #include <Definitions/inline.h> //for the inline declaration
    #include <Definitions/retcodes.h> //for return codes and true/false definition
    #include "../Numeric/Integer/rf_int.ph" //for the integer int and uint conversions
//for the float conversions
#include "../Numeric/Float/rf_float.ph" //for doubleToStr()
//for error logging macros
    #include <stdio.h>//for FILE* used inside printf.h
    #include <Threads/common.h> //for rfThread_GetID()
    #include <String/string_decl.h> //for RF_String
    #include <String/common.h> //for RFS_() macro
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Utils/memory.h> //for refu memory allocation

#include <Utils/constcmp.h> //for RF_HEXEQ_C() and other macros
//for local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>//for local scope macros
/*------------- End of includes -------------*/


// Appends the parameter String to this extended string.
char rfStringX_Append(RF_StringX* thisstr, const void* otherP)
{
    char ret = true;
    const RF_String* other = (const RF_String*)otherP;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(other == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the other string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //calculate the new byte length
    thisstr->INH_String.byteLength += other->byteLength;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC_JMP(thisstr,
                           thisstr->INH_String.byteLength+thisstr->bIndex+1,
                           ret = false,
                           cleanup);
    //add the string to this one
    strncat(thisstr->INH_String.bytes, other->bytes, other->byteLength);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


//Appends a number of characters from the parameter String to this extended string.
char rfStringX_Append_i(RF_StringX* thisstr, const void* otherP, int chars)
{
    const RF_String* other = (const RF_String*)otherP;
    uint32_t i;
    uint32_t length = 0;
    unsigned end = 0;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(other == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the other string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //find out the bytelength of the requested char position
    RF_STRING_ITERATE_START(other,length,i);
        if(length == chars)
        {
            end = i;
            break;
        }
    RF_STRING_ITERATE_END(length,i)

    //if the end is not found (requested out of bounds character) then just take all of the string
    if(end == 0)
    {
        end = other->byteLength;
    }
    //calculate the new bytelength
    thisstr->INH_String.byteLength += end;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC_JMP(thisstr,
                           thisstr->INH_String.byteLength+thisstr->bIndex+1,
                           ret = false, cleanup);
    //add the part of the other string to this one
    strncat(thisstr->INH_String.bytes, other->bytes, end);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Appends a single unicode character to this string
char rfStringX_Append_char(RF_StringX* thisstr, uint32_t unichar)
{
    char utf8[5];
    int length;
    if((length = rfUTF8_Encode_single(unichar,utf8)) <= 0)
    {
        RF_ERROR("Encoding the given unicode codepoint to UTF8 failed");
        return false;
    }

    //calculate the new bytelength
    thisstr->INH_String.byteLength += length;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(thisstr,
                       thisstr->INH_String.byteLength+thisstr->bIndex+1,
                       false);
    //add it
    strcat(thisstr->INH_String.bytes,utf8);
    return true;
}

//Appends a single utf8 character to this string
char rfStringX_Append_charutf8(RF_StringX* thisstr,uint32_t utf8char)
{
    int i,j;
    uint32_t length = 0;
    char c = utf8char&0xFF,utf8[5];

    //if it may not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(thisstr,
                       thisstr->INH_String.byteLength+thisstr->bIndex+5,
                       false);
    //add the bytes of the utf8
    while(c != 0 && length<4)
    {
        utf8[length] = c;
        length++;
        utf8char = utf8char>>8;
        c = utf8char&0xFF;
    }
    for(i=length-1,j=0; i>=0; i--,j++)
    {
        thisstr->INH_String.bytes[thisstr->INH_String.byteLength+j] = utf8[i];
    }
    //determine the new bytelength
    thisstr->INH_String.byteLength += length;
    //and null terminate it
    thisstr->INH_String.bytes[thisstr->INH_String.byteLength]='\0';
    return true;
}

/* max number of bytes used for an int */
#define INTNUMBUFF 64
//Appends an int to the extended string
char rfStringX_Append_int(RF_StringX* thisstr, int64_t num, int width,
                          int precision, char flags)
{
    char buff[INTNUMBUFF];
    int len=0;
    char hasSign=0;

    if(flags & RF_FORMATFLAG_SIGN && num > 0)//force a '+' for positive
    {
        buff[0] ='+';
        len=1;
        hasSign=1;
    }
    else if(flags & RF_FORMATFLAG_NOSIGN && num > 0)
    {//If the first character of a signed conversion is not a sign or if a signed conversion results in no characters, a <space> shall be prefixed to the result
        buff[0] = ' ';
        len=1;
        hasSign=1;
    }


    //turn the string into a number
    len+=intToStr(num,buff+hasSign);

    //if we need less digits
    if(len-hasSign > precision)
    {
        len=precision + hasSign;
    }
    //if we need more space
    if(width> len)
    {
        char pad = ' ';
        int diff = width-len;
        if(flags & RF_FORMATFLAG_PADZEROES)
        {
            pad = '0';
        }
        if(flags & RF_FORMATFLAG_LJUSTIFY)
        {
            memset(buff+len,pad,diff);
        }
        else//right justify
        {
            //move the buffer to the right to make room for the padding
            memmove(buff+diff,buff,len);
            memset(buff,pad,diff);
        }
        len = width;
    }
    //null terminate the buffer
    buff[len] = '\0';

    //calculate the new bytelength
    thisstr->INH_String.byteLength += len;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(thisstr,
                       thisstr->INH_String.byteLength+thisstr->bIndex+1,
                       false);
    //add the int to the string
    strncat(thisstr->INH_String.bytes,buff,len);
    return true;
}

//Appends an unsigned int to the extended string
char rfStringX_Append_uint(RF_StringX* thisstr, uint64_t num, int width,
                           int precision, char flags)
{
    char buff[INTNUMBUFF];
    char hasSign=0;
    int len =0;
    /* - finding it useless for unsigned ints, will leave it commented and think about it
    if(flags & RF_FORMATFLAG_SIGN)//force a '+'
    {
        buff[0] ='+';
        i=1;
        hasSign=1;
    }*/
    if(flags & RF_FORMATFLAG_NOSIGN)
    {
        buff[0] = ' ';
        hasSign=1;
        len=1;
    }

    //turn the string into a number
    len+=uintToStr(num,buff+hasSign);

    //if we need less digits
    if(len - hasSign > precision)
    {
        len=precision;
        if(hasSign)
        {
            len +=1;
        }
    }
    //if we need more space
    if(width> len)
    {
        int diff=width-len;
        char pad = ' ';
        if(flags & RF_FORMATFLAG_PADZEROES)
        {
            pad = '0';
        }
        if(flags & RF_FORMATFLAG_LJUSTIFY)
        {
            memset(buff+len,pad,diff);
        }
        else//right justify
        {
            //move the buffer to the right to make room for the padding
            memmove(buff+diff,buff,len);
            memset(buff,pad,diff);
        }
        len = width;
    }
    //null terminate the buffer
    buff[len] = '\0';

    //calculate the new bytelength
    thisstr->INH_String.byteLength += len;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(thisstr,
                       thisstr->INH_String.byteLength+thisstr->bIndex+1,
                       false);
    //add the int to the string
    strncat(thisstr->INH_String.bytes,buff,len);
    return true;
}

//Appends an unsigned int as a Hexadecimal value to the extended string following some specific parameters
char rfStringX_Append_hex(RF_StringX* thisstr, uint64_t num, int width,
                          int precision, char flags)
{
    char buff[INTNUMBUFF];
    int len = 0;
    if(flags&RF_FORMATFLAG_SHARP)
    {
        len+=2;
        buff[0] = '0';
        buff[1] = 'x';
    }
    if(flags&RF_FORMATFLAG_UPPER)
    {
        len+=uintToUHexStr(num,buff+len);
    }
    else
    {
        len+=uintToLHexStr(num,buff+len);
    }
    //if we need less digits
    if(len>precision)
    {
        len=precision;
    }
    //if we need more space
    if(width> len)
    {
        int diff=width-len;
        char pad = ' ';
        if(flags & RF_FORMATFLAG_PADZEROES)
        {
            pad = '0';
        }
        if(flags & RF_FORMATFLAG_LJUSTIFY)
        {
            memset(buff+len, pad, diff);
        }
        else//right justify
        {
            //move the buffer to the right to make room for the padding
            memmove(buff+diff,buff,len);
            memset(buff,pad,diff);
        }
        len = width;
    }
    //null terminate the buffer
    buff[len] = '\0';
    //calculate the new bytelength
    thisstr->INH_String.byteLength += len;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(thisstr,
                       thisstr->INH_String.byteLength+thisstr->bIndex+1,
                       false);
    //add the int to the string
    strncat(thisstr->INH_String.bytes,buff,len);
    return true;
}

// Appends a float or double value to the extended string following some specific parameters
char rfStringX_Append_double(RF_StringX* thisstr, double f, int width,
                             int precision, char flags)
{
    char* result,*end,*str,hasSign;
    int decpt,sign,len,start;
    result=doubleToStr(f,3,precision,&decpt,&sign,&end);
    len = end-result+1;
    str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength;
    start = thisstr->INH_String.byteLength;
    //negative check
    hasSign=0;
    if(sign!=0)
    {
        len+=1;
        *str++ = '-';
        hasSign=1;
    }
    if(decpt < 0)//if decpt is less than zero then we gotta play it differently
    {
        int udec = -1*decpt;
        len+= udec+2;//+2 is for "0."
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+3+len+width,
            false);//3 = 2 (for both sign and decpt==0) +1 (null termination character) +width (may be an overkill but maybe it's better than an extra comparison)
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength+hasSign;
        *str++='0';
        *str++='.';

        memset(str,'0',udec);
        str = str+ udec;
        while(*result !='\0')
        {
            *str++=*result++;
        }
        *str='\0';
        thisstr->INH_String.byteLength+=len;

    }
    //if there are no characters after the radix
    else if(len == decpt+1)
    {
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+3+len+width,
            false);//3 = 2 (for both sign and decpt==0) +1 (null termination character) +width (may be an overkill but maybe it's better than an extra comparison)
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength+hasSign;
        do
        {
            *str++ = *result++;
        }while(*result!='\0');
        *str='\0';
        len=decpt;
        thisstr->INH_String.byteLength+=len;
        if(flags & RF_FORMATFLAG_SHARP)
        {
            *str++ = '.';
            *str++ = '0';
            *str++ = '\0';
            len+=2;
            thisstr->INH_String.byteLength+=2;
        }
    }
    //else check all the bad number cases
    else if(decpt == 9999)
    {
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+4+width,
            false);
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength;
        len = 3;
        thisstr->INH_String.byteLength+=3;
        if(strcmp(result,"Infinity")==0)
        {
            if(flags & RF_FORMATFLAG_UPPER)
            {
                *str++ = 'I';
                *str++ = 'N';
                *str++ = 'F';
                *str++ = '\0';
            }
            else
            {
                *str++ = 'i';
                *str++ = 'n';
                *str++ = 'f';
                *str++ = '\0';
            }
        }
        else if(strcmp(result,"NaN")==0)
        {
            if(flags & RF_FORMATFLAG_UPPER)
            {
                *str++ = 'N';
                *str++ = 'A';
                *str++ = 'N';
                *str++ = '\0';
            }
            else
            {
                *str++ = 'n';
                *str++ = 'a';
                *str++ = 'n';
                *str++ = '\0';
            }
        }
    }
    else//normal case
    {
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+3+len+width,
            false);//3 = 2 (for both sign and decpt==0) +1 (null termination character) +width (may be an overkill but maybe it's better than an extra comparison)
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength+hasSign;
        if(!decpt)//if decimal point is at zero we got 1 more digit in the length
        {
            len+=1;
            *str++='0';
        }

        memcpy(str, result, decpt);
        str = str+decpt;
        *str++ = '.';
        result = result+decpt;
        do
        {
             *(str++) = *(result++);
        }while(*result != '\0');
        *str = '\0';

        //take care of byteLength
        thisstr->INH_String.byteLength+=len;
    }//end of all cases

    //check to see if more width should be given
    if(width > len)
    {
        int diff = width-len;
        char pad = ' ';
        if(flags & RF_FORMATFLAG_PADZEROES)
        {
            pad = '0';
        }
        if(flags & RF_FORMATFLAG_LJUSTIFY)//left justify
        {
            memset(thisstr->INH_String.bytes+thisstr->INH_String.byteLength,
                   pad,
                   diff);
        }
        else//right justify
        {
            memmove(thisstr->INH_String.bytes+start+diff,
                    thisstr->INH_String.bytes+start,len);
            memset(thisstr->INH_String.bytes+start, pad, diff);
        }
        thisstr->INH_String.byteLength+=diff;
        thisstr->INH_String.bytes[thisstr->INH_String.byteLength] ='\0';
    }
    //success
    return true;
}

//Appends a float or double value to the extended string in scientific notation following some specific parameters
char rfStringX_Append_double_scientific(RF_StringX* thisstr, double f,
                                        int width, int precision,
                                        char flags)
{
    char* result,*str,*end,hasSign=0;
    int decpt,sign,len,start;
    result=doubleToStr(f, 3, 999, &decpt, &sign, &end);//for scientific notation calling doubleToStr() with 999 precision because there might be cases where
    len=0;      //in normal notation the given precision would be enough but since the radix moves in scientific we would be left with less data than we need
    start = thisstr->INH_String.byteLength;
    str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength;
    //negative check
    if(sign!=0)
    {
        len+=1;
        hasSign=1;
        *str++ = '-';
    }
    //if there are no characters after the radix and the flag does not specifically request the radix in all cases
    if(decpt == end-result && (!(flags &RF_FORMATFLAG_SHARP)))
    {
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+3+len+width,
            false);//3 = 2 (for both sign and decpt==0) +1 (null termination character) +width (may be an overkill but maybe it's better than an extra comparison)
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength+hasSign;
        do
        {
            len++;
            *str++ = *result++;
        }while(*result!='\0');
        *str='\0';
        thisstr->INH_String.byteLength+=len;
    }
    //else check all the bad number cases
    else if(decpt == 9999)
    {
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+4+width,
            false);
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength;
        len = 3;
        thisstr->INH_String.byteLength+=3;
        if(strcmp(result,"Infinity")==0)
        {
            if(flags & RF_FORMATFLAG_UPPER)
            {
                *str++ = 'I';
                *str++ = 'N';
                *str++ = 'F';
                *str++ = '\0';
            }
            else
            {
                *str++ = 'i';
                *str++ = 'n';
                *str++ = 'f';
                *str++ = '\0';
            }
        }
        else if(strcmp(result,"NaN")==0)
        {
            if(flags & RF_FORMATFLAG_UPPER)
            {
                *str++ = 'N';
                *str++ = 'A';
                *str++ = 'N';
                *str++ = '\0';
            }
            else
            {
                *str++ = 'n';
                *str++ = 'a';
                *str++ = 'n';
                *str++ = '\0';
            }
        }
    }
    else//normal case
    {
        char buff[4];
        int i = 0;
        int pCount = 0;//will hold the counter of precision for the digits after the radix
        RF_STRINGX_REALLOC(
            thisstr,
            thisstr->INH_String.byteLength+thisstr->bIndex+precision+width+6,
            false);//+6 is for the digits needed for scientific notation
        str = thisstr->INH_String.bytes+thisstr->INH_String.byteLength+hasSign;
        if(!decpt)//if decimal point is at zero we got 1 more digit in the length
        {
            len+=1;
            *str++='0';
        }
        else//take the first digit of the result
        {
            *str++=*result++;
        }
        *str++='.';//output the decimal point
        do
        {
            *str++=*result++;
            pCount++;
        }while(*result != '\0' && pCount < precision);
        len+=pCount+4;//+1 is for the 'E' or 'e' below,+1 for the operator '+' or '-', +1 for the first digit at the start of the else and +1 for the radix right after that
        if(flags & RF_FORMATFLAG_UPPER)
        {
            *str++='E';
        }
        else
        {
            *str++='e';
        }
        if(decpt >=0)
        {
            *str++='+';
            uintToStr(decpt-1,buff);
            do
            {
                *str++ = buff[i];
                i++;
                len++;
            }while(buff[i] != '\0');

        }
        else
        {
            *str++='-';
            uintToStr(-1*(-1+decpt),buff);
            do
            {
                *str++ = buff[i];
                i++;
                len++;
            }while(buff[i] != '\0');
        }
        *str='\0';
        //take care of byteLength
        thisstr->INH_String.byteLength+=len;
    }//end of all cases

    //check to see if more width should be given
    if(width > len)
    {
        int diff = width-len;
        char pad = ' ';
        if(flags & RF_FORMATFLAG_PADZEROES)
        {
            pad = '0';
        }
        if(flags & RF_FORMATFLAG_LJUSTIFY)//left justify
        {
            memset(thisstr->INH_String.bytes+thisstr->INH_String.byteLength,pad,diff);
        }
        else//right justify
        {
            memmove(thisstr->INH_String.bytes+start+diff,
                    thisstr->INH_String.bytes+start,len);
            memset(thisstr->INH_String.bytes+start, pad, diff);
        }
        thisstr->INH_String.byteLength+=diff;
        thisstr->INH_String.bytes[thisstr->INH_String.byteLength] ='\0';
    }
    //success
    return true;
}


// Prepends the parameter String to this extended string
char rfStringX_Prepend(RF_StringX* thisstr, const void* otherP)
{
    char ret = true;
    const RF_String* other = (const RF_String*)otherP;
    int32_t i;//is not unsigned since it goes to -1 in the loop
    //keep the previous byte length
    uint32_t size = thisstr->INH_String.byteLength;

    RF_ENTER_LOCAL_SCOPE();
#if RF_OPTION_DEBUG
    if(other == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the other string argument");
        ret = false;
        goto cleanup;
    }
#endif 


    //get the new byte length
    thisstr->INH_String.byteLength += other->byteLength;
    //if the new string does not fit inside the buffer reallocate it
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        thisstr->INH_String.byteLength+thisstr->bIndex+1,
        ret = false,
        cleanup
        );
    //move the pre-existing string to the end of the buffer, by dislocating each byte by cstrlen
    for(i = size; i >= 0; i --)
    {
        thisstr->INH_String.bytes[i+other->byteLength] = thisstr->INH_String.bytes[i];
    }
    //and now add the new string to the start
    memcpy(thisstr->INH_String.bytes,other->bytes,other->byteLength);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

// Inserts a string to this extended string at the parameter position.
char rfStringX_Insert(RF_StringX* thisstr, uint32_t pos,
                      const void* otherP)
{
    const RF_String* other = (const RF_String*)otherP;
    uint32_t length,bytePos,size,i;
    char found = false, ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(other == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the other string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //keep the original byte length here
    size = thisstr->INH_String.byteLength;
    //get the new byte length
    thisstr->INH_String.byteLength += other->byteLength;
    //check if the new string fits in the buffer and if not reallocate it
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        thisstr->INH_String.byteLength+thisstr->bIndex+1,
        ret = false,
        cleanup);
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
    //if the position is found in the string then insert
    if(found != false)
    {
        //move the string's contents to make room for the extra string insertion
        memmove(thisstr->INH_String.bytes+other->byteLength+bytePos,
                thisstr->INH_String.bytes+bytePos,size-bytePos+1);
        //+1 is for the null termination character
        //now insert the new string
        memcpy(thisstr->INH_String.bytes+bytePos,
               other->bytes,other->byteLength);
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

/*-- RF_StringX unsafe appending  functions--*/
// Appends some bytes of the parameter String to this extended string.
char rfStringX_Append_bytes(RF_StringX* thisstr, const void* otherP,
                            const int32_t bytes)
{
    const RF_String* other = (const RF_String*)otherP;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(other == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the other string argument");
        ret = false;
        goto cleanup;
    }
#endif
    //calculate the new bytelength
    thisstr->INH_String.byteLength += bytes;
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        thisstr->INH_String.byteLength+thisstr->bIndex+1,
        ret = false,
        cleanup);
    //add the part of the other string to this one
    strncat(thisstr->INH_String.bytes, other->bytes, bytes);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
//Appends a cstring to this string
char rfStringX_Append_cstr(RF_StringX* thisstr, const char* cstr)
{
#if RF_OPTION_DEBUG
    if(cstr == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the c string argument");
        return false;
    }
#endif
    //calculate the new bytelength
    thisstr->INH_String.byteLength += strlen(cstr);
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(
        thisstr,
        thisstr->INH_String.byteLength+thisstr->bIndex+1,
        false);
    //add the part of the other string to this one
    strcat(thisstr->INH_String.bytes,cstr);
    return true;
}

/*--- RF_StringX replacing functions---*/
// Replaces all of the specified sstr substring from the extended String with string rstr
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_Replace(RF_StringX* thisstr, const void* sstrP,
                       const void* rstrP, uint32_t num,
                       const char options)
#else
char i_rfStringX_Replace(RF_StringX* thisstr, const void* sstrP,
                         const void* rstrP, uint32_t num,
                         const char options)
#endif
{
    const RF_String* sstr = (const RF_String*)sstrP;
    const RF_String* rstr = (const RF_String*)rstrP;
    //will keep the number of found instances of the substring
    uint32_t foundN = 0;
    uint32_t diff,i,j,start, bSize;
    int32_t* bytePositions;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(sstr == NULL || rstr == NULL )
    {
        RF_ERROR("Provided a NULL pointer for either the search or "
                 "the replace string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //if the substring string is not even found return false
    if(rfString_FindBytePos(thisstr,sstr,options) == RF_FAILURE)
    {
        ret = false;
        goto cleanup;
    }
    //create a buffer that will keep the byte positions
    bSize = 50;
    RF_MALLOC_JMP(bytePositions, bSize*sizeof(int32_t),
                  ret = false, cleanup1);
    //if the given num is 0 just make sure we replace all
    if(num == 0)
    {
        num = UINT_MAX;//max number of occurences
    }
    //find how many occurences exist but also remember the previous position of the internal pointer
    start = thisstr->bIndex;
    while((bytePositions[foundN] = rfString_FindBytePos(
               thisstr, sstr, options))  != RF_FAILURE)
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
            RF_REALLOC_JMP(bytePositions, int32_t, bSize,
                           ret = false, cleanup);
        }
        //if we found the required number of occurences break;
        if(foundN >= num)
        {
            break;
        }
    }
    //move the internal pointer back since we are done searching
    thisstr->INH_String.bytes -= thisstr->bIndex-start;
    thisstr->INH_String.byteLength += thisstr->bIndex-start;
    thisstr->bIndex = start;
    //make sure that the number of occurences to replace do not exceed the actual number of occurences
    if(num > foundN)
    {
        num = foundN;
    }
    //act depending on the size difference of rstr and sstr
    if(rstr->byteLength > sstr->byteLength) //replace string is bigger than the removed one
    {
        int32_t reqSize;
        diff = rstr->byteLength - sstr->byteLength;
        //calculate the required bytelength
        reqSize = thisstr->INH_String.byteLength + num*diff;
        //reallocate the string to fit the new bigger size if needed
        RF_STRINGX_REALLOC_JMP(thisstr,
                               reqSize+thisstr->bIndex+1,
                               ret = false,
                               cleanup);
        //now replace all the substrings one by one
        for(i = 0; i < num; i ++)
        {
            //make space in the string for the replacement
            memmove(
                thisstr->INH_String.bytes+bytePositions[i]+sstr->byteLength+diff,
                thisstr->INH_String.bytes+bytePositions[i]+sstr->byteLength,
                thisstr->INH_String.byteLength+1-(bytePositions[i]+sstr->byteLength));
            //copy in the replacement
            memcpy(
                thisstr->INH_String.bytes+bytePositions[i],
                rstr->bytes,
                rstr->byteLength);
            //also increase the bytelength (since now we moved the whole string by one replacement)
            thisstr->INH_String.byteLength += diff;
            //also increase all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < num; j ++)
            {
                bytePositions[j] = bytePositions[j]+diff;
            }
        }
    }
    else if(rstr->byteLength < sstr->byteLength) //replace string is smaller than the removed one
    {
        //get the differenc in byte length of removed substring and replace string
        diff = sstr->byteLength-rstr->byteLength;

        //now replace all the substrings one by one
        for(i =0; i < num; i ++)
        {
            //copy in the replacement
            memcpy(
                thisstr->INH_String.bytes+bytePositions[i],
                rstr->bytes,
                rstr->byteLength);
            //move all of the contents of the string to fit the replacement
            memmove(
                thisstr->INH_String.bytes+bytePositions[i]+rstr->byteLength,
                thisstr->INH_String.bytes+bytePositions[i]+sstr->byteLength,
                thisstr->INH_String.byteLength+1-(bytePositions[i]+sstr->byteLength));
            //reduce bytelength
            thisstr->INH_String.byteLength-=diff;
            //also decrease all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < num; j ++)
            {
                bytePositions[j] = bytePositions[j]-diff;
            }
        }
        //just note that reallocating downwards is not necessary
    }
    else //replace and remove strings are equal
    {
        for(i = 0; i < num; i ++)
        {
            strncpy(
                thisstr->INH_String.bytes+bytePositions[i],
                rstr->bytes,
                rstr->byteLength);
        }
    }

#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION //only used for malloc fail
  cleanup1:
#endif
    free(bytePositions);
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Replaces what exists between the ith left and right substrings of this extended String.Utilizes the internal string pointer.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_ReplaceBetween(RF_StringX* thisstr, const void* leftP,
                              const void* rightP, const void* rstrP,
                              char options, uint32_t i)
#else
char i_rfStringX_ReplaceBetween(RF_StringX* thisstr, const void* leftP,
                                const void* rightP, const void* rstrP,
                                char options, uint32_t i)
#endif
{
    const RF_String* left = (const RF_String*)leftP;
    const RF_String* right = (const RF_String*)rightP;
    const RF_String* rstr = (const RF_String*)rstrP;
    uint32_t j,move,start = thisstr->bIndex;
    char found = false,ret=false;
    RF_String ss;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(left == NULL || right == NULL || rstr = NULL)
    {
        RF_ERROR("Provided a NULL pointer for either the left or the right "
                 "or the replace string argument");
        ret = false;
        goto cleanup1;
    }
#endif


    if(i==0)//if we want all occurences replaced
    {
        //while we have occurences of the pair
        while(rfString_Between(thisstr,left,right,&ss,options) != false)
        {
            found = true;
            //move the internal pointer right after the left part of the pair and replace what was found there
            //no of the functions below fail since the while condition is true
            rfStringX_MoveAfter(thisstr,left,0,options);
            if(rfStringX_Replace(thisstr,&ss,rstr,1,options))
            {
                goto cleanup1;
            }
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
            //success
            ret=true;
        }
        goto cleanup1;
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
            goto cleanup1;//falure
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
        goto cleanup1;//falure
    }
    ///success
    //move after the left part of the pair
    rfStringX_MoveAfter(thisstr,left,0,options);
    //and then replace the occurence
    if(!rfStringX_Replace(thisstr,&ss,rstr,1,options))
    {
        //failure
        goto cleanup1;
    }
    //now we are done and should go back
    rfString_Deinit(&ss);
    move = thisstr->bIndex-start;
    thisstr->INH_String.bytes -= move;
    thisstr->bIndex = start;
    thisstr->INH_String.byteLength += move;

    //if we get here success
    ret=true;
cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;

}
