/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <Definitions/types.h> //for fixed size types needed in various places
#include "rf_str_common.ph"
/*------------- Module related inclusion -------------*/
#include <String/rf_str_decl.h>
#include <String/rf_str_flags.h>
/*------------- Outside Module inclusion -------------*/
#include <Utils/bits.h> //for RF_BITFLAG_ON() macro
#include <Definitions/retcodes.h>//for true/false and other retcodes
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

// Internal version of rf_string_find, used for byte position.
int rf_string_find_byte_pos(const void* tstr, const void* sstr, const char options)
{
#define CHECK_NOT_CHAR(s_, i_)                  \
    do{                                         \
        /* if is is not a character */          \
        switch(rf_string_data(s_)[i_])           \
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
        if(!(found = strstr_nnt(rf_string_data(tstr), rf_string_length_bytes(tstr),
                               rf_string_data(sstr), rf_string_length_bytes(sstr))))
            return RF_FAILURE;

        //get the byte position
        uint32_t bytepos = found - rf_string_data(tstr);
        //if we need the exact string as it is given
        if(RF_BITFLAG_ON(options, RF_MATCH_WORD))
        {
            //check before the found string
            if(bytepos != 0)
            {
                CHECK_NOT_CHAR(tstr, bytepos - 1);
            }
            //check after the found string
            if(bytepos + rf_string_length_bytes(sstr) != rf_string_length_bytes(tstr))
            {
                //if is is not a character
                CHECK_NOT_CHAR(tstr, bytepos + rf_string_length_bytes(sstr));
            }
        }//end of the exact string option
        //else return the position in the bytes buffer
        return bytepos;
    }

    //else, case insensitive search
    uint32_t i,j;
    for(i=0; i < rf_string_length_bytes(tstr); i ++)
    {
        //if i matches the start of the substring
        for(j = 0; j < rf_string_length_bytes(sstr); j++)
        {
            //if the jth char is a big letter
            if(rf_string_data(sstr)[j] >= 0x41 && rf_string_data(sstr)[j] <= 0x5a)
            {
                //no match
                if(rf_string_data(sstr)[j] != rf_string_data(tstr)[i+j] &&
                   rf_string_data(sstr)[j]+32 != rf_string_data(tstr)[i+j])
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
            else if(rf_string_data(sstr)[j] >= 0x61 && 
                    rf_string_data(sstr)[j] <= 0x7a)
            {
                //no match
                if(rf_string_data(sstr)[j] != rf_string_data(tstr)[i+j] &&
                   rf_string_data(sstr)[j]-32 != rf_string_data(tstr)[i+j])
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
            else if(rf_string_data(sstr)[j] != rf_string_data(tstr)[i+j])
            {
                break;//break off the substring search loop
            }

            //we either found it or need to perform one last check for exact string
            if(j == rf_string_length_bytes(sstr) - 1)
            {
                //only if the end of the string is not right after the substring
                if(RF_BITFLAG_ON(options, RF_MATCH_WORD) &&
                   i + rf_string_length_bytes(sstr) < rf_string_length_bytes(tstr))
                {
                    CHECK_NOT_CHAR(tstr, i + rf_string_length_bytes(sstr));
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
i_INLINE_INS void rf_string_generic_append(void* thisstr,
                                     const char* other,
                                     unsigned int bytes_to_copy);
