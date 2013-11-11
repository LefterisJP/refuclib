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
/*------------- Outside Module inclusion -------------*/
#include <String/unicode.h>
#include <Definitions/retcodes.h> //for error codes
#include <Utils/constcmp.h> //for RF_HEXEQ_C() macro and others
//for local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/memory.h> //for refu memory allocation
//for error logging
    #include <Threads/common.h> //for rfThread_GetID()
    #include <String/string_decl.h> //for RF_String
    #include <String/common.h> //for RFS_() macro
    #include <Utils/error.h> //for LOG_ERROR() macros
/*------------- libc inclusion --------------*/
#include <errno.h> //for errno
#include <math.h> //for HUGE_VAL
/*------------- End of includes -------------*/

//Returns the strings contents as a UTF-16 buffer
uint16_t* rfString_ToUTF16(const void* sP, uint32_t* length)
{
    uint32_t* codepoints,charsN;
    uint16_t* utf16;
    const RF_String* s = (const RF_String*) sP;
    RF_MALLOC(codepoints, s->byteLength*4, NULL);
    //get the unicode codepoints
    if(!rfUTF8_Decode(s->bytes, s->byteLength, &charsN,
                      codepoints, s->byteLength*4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        free(codepoints);
        return NULL;
    }
    //encode them in UTF-16, no check here since it comes from an RF_String
    // which is always guaranteed to have valid UTF-8 and as such valid codepoints
    RF_MALLOC(utf16, s->byteLength*4, NULL);  
    if(!rfUTF16_Encode(codepoints, charsN, length, utf16, s->byteLength*4))
    {
        RF_ERROR("Error at encoding a buffer in UTF-16");
        free(utf16);
        free(codepoints);
        utf16 = NULL;
    }
    free(codepoints);
    return utf16;
}

//Returns the strings contents as a UTF-32 buffer
uint32_t* rfString_ToUTF32(const void* sP,uint32_t* length)
{
    const RF_String* s = (const RF_String*) sP;
    uint32_t* cp;
    RF_MALLOC(cp, s->byteLength*4, NULL);
    //get the unicode codepoints
    if(!rfUTF8_Decode(s->bytes, s->byteLength, length, cp, s->byteLength*4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        cp = NULL;
    }
    return cp;
}

// Returns the cstring representation of the string
char* rfString_Cstr(const void* str)
{
    const RF_String* s = (const RF_String*)str;
    return s->bytes;
}


//Returns the integer value of the string if and only if it contains only numbers. If it contains anything else the function fails.
char rfString_ToInt(const void* str,int32_t* v)
{
    const RF_String* thisstr = (const RF_String*)str;
    char* end;
    //get the integer
    *v = strtol ( thisstr->bytes, &end,10);

///This is the non-strict case. Takes the number out of the string no matter what else it has inside
/*    //if we did get something
    if(strlen(end) < this->length())
        return true;
*/
///This is the strict case, and the one we will go with. The non-strict case might be moved to its own function, if ever in the future
    if(end[0] == '\0')
        return true;

    //else false
    return false;
}

//Returns the float value of a String
int rfString_ToDouble(const void* thisstrP,double* f)
{
    const RF_String* str = (const RF_String*)thisstrP;
    *f = strtod(str->bytes,NULL);
    //check the result
    if(*f == 0.0)
    {
        //if it's zero and the string equals to zero then we are okay
        if(rfString_Equal(str,RFS_("0")) || rfString_Equal(str,RFS_("0.0")))
            return RF_SUCCESS;
        //underflow error
        if(errno == ERANGE)
            return RE_STRING_TOFLOAT_UNDERFLOW;
        //in any other case it's a conversion error
        return RE_STRING_TOFLOAT;
    }
    //if the result is a HUGE_VAL and errno is set,the number is not representable by a double
    if(*f == HUGE_VAL && errno == ERANGE)
        return RE_STRING_TOFLOAT_RANGE;

    //any other case success
    return RF_SUCCESS;
}

//Turns any uppercase characters of the string into lower case
void rfString_ToLower(void* thisstr)
{
    uint32_t charI,byteI;
    RF_String* s = (RF_String*)thisstr;
    RF_STRING_ITERATE_START(s,charI,byteI)
        //if the character is lowercase
        if(s->bytes[byteI] >= 65 && s->bytes[byteI]<=90)
        {
            //turn it into uppercase
            s->bytes[byteI] += 32;
        }
    RF_STRING_ITERATE_END(charI,byteI)
}

//Turns any lowercase characters of the string into upper case
void rfString_ToUpper(void* thisstr)
{
    uint32_t charI,byteI;
    RF_String* s = (RF_String*)thisstr;
    RF_STRING_ITERATE_START(s,charI,byteI)
        //if the character is lowercase
        if(s->bytes[byteI] >= 97 && s->bytes[byteI]<=122)
        {
            //turn it into uppercase
            s->bytes[byteI] -= 32;
        }
    RF_STRING_ITERATE_END(charI,byteI)
}

// Tokenizes the given string. Separates it into @c tokensN depending on how many substrings can be created from the @c sep separatior and stores them
// into the Array of RF_String* that should be passed to the function
char rfString_Tokenize(const void* str,const void* sepP,uint32_t* tokensN,RF_String** tokens)
{
    const RF_String* thisstr = (const RF_String*)str;
    const RF_String* sep = (const RF_String*)sepP;
    uint32_t i;
    RF_ENTER_LOCAL_SCOPE();
    //first find the occurences of the separator, and then the number of tokens
    *tokensN = rfString_Count(thisstr,sep,0)+1;
    //error checking
    if(*tokensN == 1)
    {
        RF_EXIT_LOCAL_SCOPE();
        return false;
    }
    //allocate the tokens
    RF_MALLOC(*tokens,sizeof(RF_String) *(*tokensN), false);
    //find the length of the separator
    uint32_t sepLen = sep->byteLength;
    char* s,*e;
    s = thisstr->bytes;
    for(i = 0; i < (*tokensN)-1; i ++)
    {
        //find each substring
        e = strstr(s,sep->bytes);
        (*tokens)[i].byteLength = e-s;
        RF_MALLOC((*tokens)[i].bytes,(*tokens)[i].byteLength+1, false);
        //put in the data
        strncpy((*tokens)[i].bytes,s,(*tokens)[i].byteLength);
        //null terminate
        (*tokens)[i].bytes[(*tokens)[i].byteLength] = '\0';

        //prepare for next sub-string
        s = e+sepLen;

    }
    ///make sure that if it's the last substring we change strategy
    (*tokens)[i].byteLength = strlen(s);
    RF_MALLOC((*tokens)[i].bytes,(*tokens)[i].byteLength+1, false);
    //put in the data
    strncpy((*tokens)[i].bytes,s,(*tokens)[i].byteLength);
    //null terminate
    (*tokens)[i].bytes[(*tokens)[i].byteLength] = '\0';

    //success
    RF_EXIT_LOCAL_SCOPE();
    return true;
}
