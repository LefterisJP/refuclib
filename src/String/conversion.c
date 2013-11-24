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
#include <String/conversion.h>
/*------------- Module related inclusion -------------*/
#include <String/core.h>//for rfString_Equal
#include <String/retrieval.h> //for rfString_Count
#include <String/common.h> //for RFS_()
#include "common.ph" //for required string private macros and functions
#include "defines.ph" //for some defines
/*------------- Outside Module inclusion -------------*/
// for unicode functions
    #include <String/unicode.h>
//for error codes
    #include <Definitions/retcodes.h> 
//for RF_HEXEQ_C() macro and others
    #include <Utils/constcmp.h> 
//for error logging
    #include <Utils/log.h> //for LOG_ERROR() macros
//for local scope macros
    #include <Utils/localscope.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for rfMax
    #include <Utils/math.h>
//for the internal thread specific buffer
    #include "../Internal/internal_mod.ph"
/*------------- libc inclusion --------------*/
#include <errno.h> //for errno
#include <math.h> //for HUGE_VAL
/*------------- End of includes -------------*/


uint16_t* rfString_ToUTF16(const void* s, uint32_t* length)
{
    uint32_t* codepoints,charsN;
    uint16_t* utf16;
    RF_MALLOC(codepoints, rfString_ByteLength(s) * 4, NULL);
    //get the unicode codepoints
    if(!rfUTF8_Decode(rfString_Data(s), rfString_ByteLength(s), &charsN,
                      codepoints, rfString_ByteLength(s) * 4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        free(codepoints);
        return NULL;
    }
    //encode them in UTF-16, no check here since it comes from an RF_String
    // which is always guaranteed to have valid UTF-8 and as such valid codepoints
    RF_MALLOC(utf16, rfString_ByteLength(s) * 4, NULL);  
    if(!rfUTF16_Encode(codepoints, charsN, length,
                       utf16, rfString_ByteLength(s) * 4))
    {
        RF_ERROR("Error at encoding a buffer in UTF-16");
        free(utf16);
        free(codepoints);
        utf16 = NULL;
    }
    free(codepoints);
    return utf16;
}

uint32_t* rfString_ToUTF32(const void* s, uint32_t* length)
{
    uint32_t* cp;
    RF_MALLOC(cp, rfString_ByteLength(s) * 4, NULL);
    //get the unicode codepoints
    if(!rfUTF8_Decode(rfString_Data(s), rfString_ByteLength(s), length,
                      cp, rfString_ByteLength(s) * 4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        cp = NULL;
    }
    return cp;
}

char* rfString_Cstr(const void* str)
{
    char* ret;
    RF_MALLOC(ret, rfString_ByteLength(str) + 1, NULL);
    memcpy(ret, rfString_Data(str), rfString_ByteLength(str));
    ret[rfString_ByteLength(str)] = '\0';
    return ret;
}

bool rfString_ToInt(const void* str, int32_t* v)
{
    char buff[MAX_UINT32_STRING_CHAR_SIZE + 1];
    int index;

    index = rfMax(MAX_UINT32_STRING_CHAR_SIZE, rfString_ByteLength(str));
    memcpy(buff, rfString_Data(str), index);
    buff[index] = '\0';

    errno = 0;
    *v = strtol (buff, NULL, 10);
    
    if(!*v || errno)
    {
        RF_ERROR("Failed to convert %s to int with strtol()"
                 " errno: %d", buff, errno);
        return false;
    }
    return true;
}

bool rfString_ToDouble(const void* str, double* f)
{
    char buff[MAX_DOUBLE_STRING_CHAR_SIZE + 1];
    int index;

    index = rfMax(MAX_DOUBLE_STRING_CHAR_SIZE, rfString_ByteLength(str));
    memcpy(buff, rfString_Data(str), index);
    buff[index] = '\0';

    errno = 0;
    *f = strtod(buff, NULL);
    //check the result
    if(DBLCMP_EQ(*f, 0.0f) || errno)
    {
        RF_ERROR("Failed to convert %s to double with strtod()"
                 " errno: %d", buff, errno);
        return false;        
    }
    return true;
}

void rfString_ToLower(void* s)
{
    uint32_t charI,byteI;
    RF_STRING_ITERATE_START(s, charI, byteI)
        //if the character is lowercase
        if(rfString_Data(s)[byteI] >= 65 && rfString_Data(s)[byteI] <= 90)
        {
            //turn it into uppercase
            rfString_Data(s)[byteI] += 32;
        }
    RF_STRING_ITERATE_END(charI, byteI)
}

void rfString_ToUpper(void* s)
{
    uint32_t charI, byteI;
    RF_STRING_ITERATE_START(s, charI, byteI)
        //if the character is lowercase
        if(rfString_Data(s)[byteI] >= 97 && rfString_Data(s)[byteI] <= 122)
        {
            //turn it into uppercase
            rfString_Data(s)[byteI] -= 32;
        }
    RF_STRING_ITERATE_END(charI, byteI)
}

bool rfString_Tokenize(const void* str, const void* sep,
                       uint32_t* tokensN, RF_String** tokens)
{
    uint32_t i, sepLen;
    char *s;
    char *e;
    RF_ENTER_LOCAL_SCOPE();
    //first find the occurences of the separator, and then the number of tokens
    *tokensN = rfString_Count(str, sep, 0) + 1;
    //error checking
    if(*tokensN == 1)
    {
        RF_EXIT_LOCAL_SCOPE();
        return false;
    }
    //allocate the tokens
    RF_MALLOC(*tokens, sizeof(RF_String) * (*tokensN), false);
    //find the length of the separator
    sepLen = rfString_ByteLength(sep);

    s = rfString_Data(str);
    e = rfString_Data(str) + rfString_ByteLength(str);
    for(i = 0; i < (*tokensN) - 1; i ++)
    {
        //find each substring
        e = strstr_nnt(s, e - s,
                       rfString_Data(sep), rfString_ByteLength(sep));
        rfString_ByteLength(&(*tokens)[i]) = e - s;
        RF_MALLOC(rfString_Data(&(*tokens)[i]),
                  rfString_ByteLength(&(*tokens)[i]),
                  false
        );
        //put in the data
        memcpy(rfString_Data(&(*tokens)[i]),
               s,
               rfString_ByteLength(&(*tokens)[i])
        );
        //prepare for next sub-string
        s = e + sepLen;

    }
    ///make sure that if it's the last substring we change strategy
    rfString_ByteLength(&(*tokens)[i]) = (
        rfString_ByteLength(str) - (s - rfString_Data(str))
    );
    RF_MALLOC(rfString_Data(&(*tokens)[i]),
              rfString_ByteLength(&(*tokens)[i]),
              false
    );
    //put in the data
    memcpy(rfString_Data(&(*tokens)[i]),
           s,
           rfString_ByteLength(&(*tokens)[i])
    );

    //success
    RF_EXIT_LOCAL_SCOPE();
    return true;
}
