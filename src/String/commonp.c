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
**
**
** This is the source file where private string functions are implemented
*/
/*------------- Corrensponding Header inclusion -------------*/
#include <Definitions/types.h> //for fixed size types needed in various places
#include "common.ph"
/*------------- Module related inclusion -------------*/
#include <String/string_decl.h>
#include <String/flags.h>
/*------------- Outside Module inclusion -------------*/
#include <Utils/bits.h> //for RF_BITFLAG_ON() macro
#include <Definitions/retcodes.h>//for true/false and other retcodes
/*------------- libc inclusion --------------*/
#include <string.h>
/*------------- End of includes -------------*/

char* strstr_nnt(const char* s1, unsigned int s1_len,
                 const char* s2, unsigned int s2_len)
{
    char *p;
    const char *end;

    /* edge case first */
    if(!s2_len)
    {
        return (char*)s1;
    }
    p = (char*)s1;
    end = s1 + s1_len;

    while(p <= end - s2_len)
    {

        if(*p == s2[0])
        {
            if((end - p >= s2_len)            &&
               (memcmp(p, s2, s2_len) == 0)    )
            {
                return p;
            }
        }

        p++;
    }

    return 0;
}

bool strcmp_nnt(char* s1, unsigned int s1_len,
                char* s2, unsigned int s2_len)
{
    unsigned int i;
    if(s1_len != s2_len) {return false;}

    for(i = 0; i < s1_len; i++)
    {
        if(s1[i] != s2[i])
        {
            return false;
        }
    }
    return true;
}

// Internal version of rfString_Find, used for byte position.
int rfString_FindBytePos(const void* tstr, const void* sstr, const char options)
{
#define CHECK_NOT_CHAR(s_, i_)                  \
    do{                                         \
        /* if is is not a character */          \
        switch(rfString_Data(s_)[i_])           \
        {                                       \
        case ' ':case '\t':case '\n':           \
            break;                              \
        default:                                \
            return RF_FAILURE;                  \
            break;                              \
        }                                       \
    }while(0)

    const char* found = 0;
    //search matching characters
    if(!RF_BITFLAG_ON(options, RF_CASE_IGNORE))
    {
        //if it is not found
        if(!(found = strstr_nnt(rfString_Data(tstr), rfString_ByteLength(tstr),
                               rfString_Data(sstr), rfString_ByteLength(sstr))))
            return RF_FAILURE;

        //get the byte position
        uint32_t bytepos = found - rfString_Data(tstr);
        //if we need the exact string as it is given
        if(RF_BITFLAG_ON(options, RF_MATCH_WORD))
        {
            //check before the found string
            if(bytepos != 0)
            {
                CHECK_NOT_CHAR(tstr, bytepos - 1);
            }
            //check after the found string
            if(bytepos + rfString_ByteLength(sstr) != rfString_ByteLength(tstr))
            {
                //if is is not a character
                CHECK_NOT_CHAR(tstr, bytepos + rfString_ByteLength(sstr));
            }
        }//end of the exact string option
        //else return the position in the bytes buffer
        return bytepos;
    }

    //else, case insensitive search
    uint32_t i,j;
    for(i=0; i < rfString_ByteLength(tstr); i ++)
    {
        //if i matches the start of the substring
        for(j = 0; j < rfString_ByteLength(sstr); j++)
        {
            //if the jth char is a big letter
            if(rfString_Data(sstr)[j] >= 0x41 && rfString_Data(sstr)[j] <= 0x5a)
            {
                //no match
                if(rfString_Data(sstr)[j] != rfString_Data(tstr)[i+j] &&
                   rfString_Data(sstr)[j]+32 != rfString_Data(tstr)[i+j])
                    break;
                //there is a match in the jth character
                if(RF_BITFLAG_ON(options, RF_MATCH_WORD))
                {
                    /*
                      if it's the first substring character and if the string
                      we search is not in it's beginning, check for EXACT 
                      string before
                    */
                    if(j == 0 && i != 0)
                    {
                        CHECK_NOT_CHAR(tstr, i - 1);
                    }
                }//exact string check if ends
            }
            //small letter
            else if(rfString_Data(sstr)[j] >= 0x61 && 
                    rfString_Data(sstr)[j] <= 0x7a)
            {
                //no match
                if(rfString_Data(sstr)[j] != rfString_Data(tstr)[i+j] &&
                   rfString_Data(sstr)[j]-32 != rfString_Data(tstr)[i+j])
                    break;
                //there is a match in the jth character
                if(RF_BITFLAG_ON(options, RF_MATCH_WORD))
                {
                    /*
                      if it's the first substring character and if the string
                      we search is not in it's beginning, check for EXACT 
                      string before
                    */
                    if(j == 0 && i != 0)
                    {
                        CHECK_NOT_CHAR(tstr, i - 1);
                    }
                }//exact string check if ends
            }
            //not a letter and no match
            else if(rfString_Data(sstr)[j] != rfString_Data(tstr)[i+j])
            {
                break;//break off the substring search loop
            }

            //we either found it or need to perform one last check for exact string
            if(j == rfString_ByteLength(sstr) - 1)
            {
                //only if the end of the string is not right after the substring
                if(RF_BITFLAG_ON(options, RF_MATCH_WORD) &&
                   i + rfString_ByteLength(sstr) < rfString_ByteLength(tstr))
                {
                    CHECK_NOT_CHAR(tstr, i + rfString_ByteLength(sstr));
                }//end of the exact string check
                //succes
                return i;
            }//end of it's the last char of the substring check
        }//substring iteration ends
    }//this string iteration ends
    return RF_FAILURE; //getting here means nothing was found

#undef CHECK_NOT_CHAR
}



i_INLINE_INS bool fill_fmt_buffer(const char* fmt,
                                  unsigned int* size,
                                  char **buffPtr,
                                  unsigned int *bIndex,
                                  va_list args);
i_INLINE_INS void rfStringGEN_Append(void* thisstr,
                                     const char* other,
                                     unsigned int bytes_to_copy);
