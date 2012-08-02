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
**
**
** This is the source file where private string functions are implemented
**/

#include "string_private.h"
#include <String/rfc_string.h>

// Internal version of rfString_Find, used for byte position.
int32_t rfString_FindBytePos(const void* str,const void* sstrP,char options)
{
    RF_String* thisstr = (RF_String*)str;
    RF_String* sstr = (RF_String*)sstrP;

    char* found = 0;
    //if we want to match the case of the string then it's a simple search of matching characters
    if( (RF_BITFLAG_ON(options,RF_CASE_IGNORE)) == false)
    {
        //if it is not found
        if( (found = strstr(thisstr->bytes,sstr->bytes)) == 0)
            return RF_FAILURE;

        //get the byte position
        uint32_t bytepos = found-thisstr->bytes;
        //if we need the exact string as it is given
        if(RF_BITFLAG_ON(options,RF_MATCH_WORD))
        {
            //check before the found string
            if(bytepos != 0)
            {
                //if is is not a character
                switch(thisstr->bytes[bytepos-1])
                {
                    case ' ':case '\t':case '\n':
                    break;
                    default:
                        return RF_FAILURE;
                    break;
                }
            }
            //check after the found string
            if(bytepos+sstr->byteLength != thisstr->byteLength)
            {
                //if is is not a character
                switch(thisstr->bytes[bytepos+sstr->byteLength])
                {
                    case ' ':case '\t':case '\n':
                    break;
                    default:
                        return RF_FAILURE;
                    break;
                }
            }
        }//end of the exact string option
        //else return the position in the bytes buffer
        return bytepos;
    }

    //else, case insensitive search
    uint32_t i,j;
    //if(cstr[0] >= 0x41 && cstr[0] <= 0x5a)
    for(i=0;i<thisstr->byteLength; i ++)
    {
        //if i matches the start of the substring
        for(j = 0; j < sstr->byteLength; j++)
        {
            //if the jth char is a big letter
            if(sstr->bytes[j] >= 0x41 && sstr->bytes[j] <= 0x5a)
            {
                //no match
                if(sstr->bytes[j] != thisstr->bytes[i+j] && sstr->bytes[j]+32 != thisstr->bytes[i+j])
                    break;
                //there is a match in the jth character so let's perform additional checks if needed
                if(RF_BITFLAG_ON(options,RF_MATCH_WORD))
                {
                    //if it's the first substring character and if the string we search is not in it's beginning, check for EXACT string before
                    if(j == 0 && i != 0)
                    {
                        switch(thisstr->bytes[i-1])
                        {
                            case ' ':case '\t':case '\n':
                            break;
                            default:
                                return RF_FAILURE;
                            break;
                        }
                    }
                }//exact string check if ends
            }
            //small letter
            else if(sstr->bytes[j] >= 0x61 && sstr->bytes[j] <= 0x7a)
            {
                //no match
                if(sstr->bytes[j] != thisstr->bytes[i+j] && sstr->bytes[j]-32 != thisstr->bytes[i+j])
                    break;
                //there is a match in the jth character so let's perform additional checks if needed
                if(RF_BITFLAG_ON(options,RF_MATCH_WORD))
                {
                    //if it's the first substring character and if the string we search is not in it's beginning, check for EXACT string before
                    if(j == 0 && i != 0)
                    {
                        switch(thisstr->bytes[i-1])
                        {
                            case ' ':case '\t':case '\n':
                            break;
                            default:
                                return RF_FAILURE;
                            break;
                        }
                    }
                }//exact string check if ends
            }
            //not a letter and no match
            else if(sstr->bytes[j] != thisstr->bytes[i+j])
                break;//break off the substring search loop

            //if we get here and it's the last char of the substring we either found it or need to perform one last check for exact string
            if(j == sstr->byteLength-1)
            {
                //only if the end of the string is not right after the substring
                if( RF_BITFLAG_ON(options,RF_MATCH_WORD) && i+sstr->byteLength < thisstr->byteLength)
                {
                    switch(thisstr->bytes[i+sstr->byteLength])
                    {
                        case ' ':case '\t':case '\n':
                        break;
                        default:
                            return RF_FAILURE;
                        break;
                    }
                }//end of the exact string check
                //succes
                return i;
            }//end of it's the last char of the substring check
        }//substring iteration ends
    }//this string iteration ends
    return RF_FAILURE; //getting here means nothing was found
}

// Finds if a substring literal exists inside another string. Internal version which returns the byte position
int32_t rfString_FindBytePos_s(const void* str,const char* lit,char options)
{
    RF_String litstr;
    if(rfString_Init(&litstr,lit) == false)
    {
        LOG_ERROR("Error at FindBytePos_s due to invalid parameter UTF-8 byte sequence",RE_UTF8_INVALID_SEQUENCE);
        return RF_FAILURE;
    }
    int32_t ret = rfString_FindBytePos(str,&litstr,options);
    rfString_Deinit(&litstr);
    return ret;
}
