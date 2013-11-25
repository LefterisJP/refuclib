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


//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/retrieval.h>
//*---------------------Module related inclusion----------------------------------------
#include <String/flags.h> //for String macro flags
#include <String/core.h> //for rfString_Deinit(), rfString_BytePosToCharPos() and others
#include <String/unicode.h> //rfUTF8_IsContinuationByte()
#include <String/stringx_decl.h> //for RF_StringX
#include <String/corex.h> //for rfStringX_Init_unsafe()
#include "common.ph" //for required string private macros and functions
//*---------------------Outside module inclusion----------------------------------------
//for bool
    #include <Definitions/retcodes.h>
//for RF_BITFLAG_ON
    #include <Utils/bits.h> 
//for RF_HEXEQ_C() macro and others
    #include <Utils/constcmp.h> 
//for the local scope macros
    #include <Utils/localscope.h>
//for the memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for sanity macros
    #include <Utils/sanity.h>
//*---------------------libc Headers inclusion------------------------------------------
#include <stdarg.h>
#include <limits.h> //for INT_MAX e.t.c.
#include <stdio.h> // for sscanf
//*----------------------------End of Includes------------------------------------------

// Finds the length of the string in characters
uint32_t rfString_Length(const void* str)
{
    uint32_t length,i;
    RF_STRING_ITERATE_START(str, length, i)
    RF_STRING_ITERATE_END(length, i);
    return length;
}

bool rfString_GetChar(const void* str, uint32_t c, uint32_t* cp)
{
    uint32_t length, i;
    RF_STRING_ITERATE_START(str, length, i)
        if(length == c)
        {
            *cp = rfString_BytePosToCodePoint(str, i);
            return true;
        }
    RF_STRING_ITERATE_END(length, i)

    return false;
}

bool rfString_Substr(const void* s, uint32_t startPos,
                     uint32_t charsN, RF_String* ret)
{
    uint32_t charI,byteI,startI,endI;
    bool started = false, ended = false;
    startI = endI = 0;
    i_NULLPTR_CHECK_2(s, ret, return false);

    RF_STRING_ITERATE_START(s, charI, byteI)
        if(charI == startPos)
        {
            startI = byteI;
            started = true;
        }
        if(charI == startPos+charsN)
        {
            endI = byteI;
            ended = true;
            break;
        }
    RF_STRING_ITERATE_END(charI, byteI)
    if(!ended)
    {
        //if the start was found return from there until the end of the string
        if(started)
        {
            endI = byteI;
        }
        else//else the substring was not found
        {
            return false;
        }
    }
    /* else success and init the return string */
    return rfString_Init_unsafe_nnt(ret,
                                    rfString_Data(s) + startI,
                                    endI - startI
    );
 
}


int32_t rfString_Find(const void* tstr, const void* sstr, const char options)
{
    int32_t ret;
    RF_ENTER_LOCAL_SCOPE();

    if((ret = rfString_FindBytePos(tstr, sstr, options)) == RF_FAILURE)
    {
        ret = RF_FAILURE;
        goto cleanup;
    }

    ret = rfString_BytePosToCharPos(tstr, ret, false);
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


int32_t rfString_Find_i(const void* thisstr, const void* sstr,
                        uint32_t startPos, uint32_t length,
                        const char options)
{
    RF_String sub;
    int32_t ret = RF_FAILURE;//the return value
    RF_ENTER_LOCAL_SCOPE();
    //if the substring does not exist fail
    if(!rfString_Substr(thisstr, startPos, length, &sub))
    {
        goto cleanup1;
    }

    //now search for the sstr substring inside the sub substring defined by length
    if((ret=rfString_Find(&sub, sstr, options)) != RF_FAILURE)
    {
        ret += startPos;//return the proper position in the original string
    }
    rfString_Deinit(&sub);//free the sub substring and return

cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

unsigned int rfString_Count(const void* tstr, const void* sstr,
                            const char options)
{
    int move, index;
    unsigned int n;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_2(tstr, sstr, goto cleanup);

    move = index = n = 0;
    //as long as the substring is found in the string
    while((move = rfString_FindBytePos(
               tstr, sstr, options)) != RF_FAILURE)
    {
        move += rfString_ByteLength(sstr);
        //proceed searching inside the string and also increase the counter
        n++;
        rfString_Data(tstr) += move;
        index += move;
        rfString_ByteLength(tstr) -= move;
    }

    //return string to its original state
    rfString_Data(tstr) -= index;
    rfString_ByteLength(tstr) += index;

#ifdef RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return n;
}


bool rfString_ScanfAfter(const void* str, const void* astr,
                         const char* format, void* var)
{
    char *s;
    const char *found;
    bool ret = false;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_2(str, astr, goto cleanup);

    found = strstr_nnt(rfString_Data(str), rfString_ByteLength(str),
                      rfString_Data(astr), rfString_ByteLength(astr));
    if(!found)
    {
        goto cleanup;
    }
    //get a pointer to the start of the position where sscanf will be used
    s = rfString_Data(str) + (
        found - rfString_Data(str) + rfString_ByteLength(astr)
    );

    //use sscanf
    if(sscanf(s, format, var) <= 0)
    {
        goto cleanup;
    }
    //success
    ret = true;
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


bool rfString_Between(const void* tstr, const void* lstr,
                      const void* rstr, void* result, const char options)
{
    int start, end;
    RF_String temp;
    bool ret = false;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_4(tstr, lstr, rstr, result, goto cleanup);

    //find the left substring
    if((start = rfString_FindBytePos(tstr, lstr, options)) == RF_FAILURE)
    {
        goto cleanup;
    }
    //get what is after it, no check since we already found it
    rfString_After(tstr, lstr, &temp, options);
    //find the right substring in the remaining part
    if((end = rfString_FindBytePos(&temp, rstr, options)) == RF_FAILURE)
    {
        goto cleanup1;
    }

    //initialize the string to return
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rfStringX_Init_unsafe_nnt(
               result,
               rfString_Data(tstr) + start + rfString_ByteLength(lstr),
               end))
        {
            goto cleanup1; 
        }
    }
    else
    {
        if(!rfString_Init_unsafe_nnt(
               result,
               rfString_Data(tstr) + start + rfString_ByteLength(lstr),
               end))
        {
            goto cleanup1; 
        }
    }
    //success
    ret = true;


cleanup1:
    //free temp string
    rfString_Deinit(&temp);
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_Beforev(const void* thisstr, void* result, const char options,
                      const unsigned char parN,  ...)
{
    const RF_String* s;
    int32_t i,minPos,thisPos;
    va_list argList;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_2(thisstr, result, ret = false; goto cleanup);

    //get the parameter characters
    va_start(argList, parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        s = (const RF_String*) va_arg(argList, RF_String*);
        if((thisPos= rfString_FindBytePos(thisstr, s, options))!= RF_FAILURE)
        {
            if(thisPos < minPos)      
            {
               minPos = thisPos;
            }
        }
    }
    va_end(argList);

    //if it is not found
    if(minPos == INT_MAX)
    {
        ret = false;
        goto cleanup;
    }
    //if it is found initialize the substring for return
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rfStringX_Init_unsafe_nnt(result, rfString_Data(thisstr), minPos))
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rfString_Init_unsafe_nnt(result, rfString_Data(thisstr), minPos))
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_Before(const void* thisstr, const void* sstr,
                     void* result, const char options)
{
    int32_t rv;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_3(thisstr, sstr, result, ret = false; goto cleanup);

    //find the substring
    if((rv = rfString_FindBytePos(thisstr, sstr, options)) == RF_FAILURE)
    {
        ret = false;
        goto cleanup;
    }
    //if it is found initialize the substring for return
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rfStringX_Init_unsafe_nnt(result, rfString_Data(thisstr), rv))
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rfString_Init_unsafe_nnt(result, rfString_Data(thisstr), rv))
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_After(const void* thisstr, const void* after,
                    void* result, const char options)
{
    int32_t bytePos;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_3(thisstr, after, result, ret = false;goto cleanup);

    //check for substring existence
    if((bytePos = rfString_FindBytePos(thisstr, after, options)) == RF_FAILURE)
    {
        ret = false;
        goto cleanup;
    }
    bytePos += rfString_ByteLength(after);
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rfStringX_Init_unsafe_nnt(
               result,
               rfString_Data(thisstr) + bytePos,
               rfString_ByteLength(thisstr) - bytePos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rfString_Init_unsafe_nnt(
               result,
               rfString_Data(thisstr) + bytePos,
               rfString_ByteLength(thisstr) - bytePos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


bool rfString_Afterv(const void* thisstr, void* result,
                     const char options, const unsigned char parN, ...)
{
    const RF_String* s;
    int32_t i, minPos, thisPos;
    uint32_t minPosLength=0;
    //will keep the argument list
    va_list argList;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_2(thisstr, result, ret = false; goto cleanup);

    //get the parameter characters
    va_start(argList,parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        s = (const RF_String*) va_arg(argList, RF_String*);
        if((thisPos= rfString_FindBytePos(thisstr, s, options))!= RF_FAILURE)
        {
            if(thisPos < minPos)
            {
                minPos = thisPos;
                minPosLength = rfString_ByteLength(s);
            }
        }
    }
    va_end(argList);
    //if it is not found
    if(minPos == INT_MAX)
    {
        ret = false;
        goto cleanup;
    }
    //if it is found initialize the substring
    minPos += minPosLength;//go after the found substring
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rfStringX_Init_unsafe_nnt(
               result,
               rfString_Data(thisstr) + minPos,
               rfString_ByteLength(thisstr) - minPos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rfString_Init_unsafe_nnt(
               result,
               rfString_Data(thisstr) + minPos,
               rfString_ByteLength(thisstr) - minPos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
