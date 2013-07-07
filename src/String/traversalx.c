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
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/traversalx.h>
//*---------------------Module related inclusion----------------------------------------
#include <String/flags.h> //for string flags
#include <String/retrieval.h> //for rfString_Between
#include <String/core.h> //for rfString_Deinit()
#include <String/corex.h> //for rfStringX_Deinit()
#include "common.ph" //for rfString_FindBytePos() and other private macros and functions
//*---------------------Outside module inclusion----------------------------------------
#include <String/unicode.h> //for rfUTF8_IsContinuationByte()

#include <Utils/constcmp.h> //for RF_HEXEQ_C() macro and others
//for the local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>//for the local scope macros
//for memory alllocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h> //for LOG_ERROR() macros
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <stdarg.h> //for the va_ macros
//*----------------------------End of Includes------------------------------------------


//Moves the internal pointer right after the substring
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfStringX_MoveAfter(RF_StringX* thisstr,const void* subP,void* resultP,const char options)
#else
int32_t i_rfStringX_MoveAfter(RF_StringX* thisstr,const void* subP,void* resultP,const char options)
#endif
{
    const RF_String* sub = (const RF_String*)subP;
    int32_t move;
    RF_ENTER_LOCAL_SCOPE()

    //check for substring existence and return failure if not found
    if( (move = rfString_FindBytePos(thisstr,sub,options)) == RF_FAILURE)
    {
        move = RF_FAILURE;
        goto cleanup;
    }
    //if found, move the internal pointer
    move += sub->byteLength;
    thisstr->bIndex += move;
    thisstr->INH_String.bytes+=move;
    //also reduce the byte length of this string
    thisstr->INH_String.byteLength -= move;

    //also if we want the string returned
    if(resultP != 0)
    {
        if(options & RF_STRINGX_ARGUMENT)
        {
            RF_StringX* result = (RF_StringX*) resultP;
            result->INH_String.byteLength = move-sub->byteLength;
            result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_MULTIPLIER+1;
            result->bIndex=0;
            RF_MALLOC_JMP(result->INH_String.bytes, result->bSize,
                          move = RF_FAILURE, cleanup);
            memcpy(
                result->INH_String.bytes,
                thisstr->INH_String.bytes-move,
                result->INH_String.byteLength);
            result->INH_String.bytes[result->INH_String.byteLength] = '\0';
        }
        else
        {
            RF_String* result = (RF_String*) resultP;
            result->byteLength = move-sub->byteLength;
            RF_MALLOC_JMP(result->bytes, result->byteLength+1,
                          move = RF_FAILURE, cleanup);
            memcpy(result->bytes,
                   thisstr->INH_String.bytes-move,
                   result->byteLength);
            result->bytes[result->byteLength] = '\0';
        }
    }
    //return positions moved
  cleanup:
    RF_EXIT_LOCAL_SCOPE()
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
        if(rfUTF8_IsContinuationByte(
               thisstr->INH_String.bytes[0]) == false)
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
        if(rfUTF8_IsContinuationByte(
               thisstr->INH_String.bytes[0]) == false)
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
char rfStringX_MoveAfterv(RF_StringX* thisstr, void* resultP, 
                          const char options,
                          const unsigned char parN, ...)
{
    uint32_t i,paramLen,move;
    int32_t minPos;
    int32_t thisPos;
    //will keep the argument list
    va_list argList;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE()

    // will keep the winning parameter length
    paramLen = 0;
    //get the parameter characters
    va_start(argList,parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        //get the param
        RF_String* s = va_arg(argList,RF_String*);
        //if the parameter got found in the string see if it's the closest
        if( (thisPos = rfString_FindBytePos(
                 thisstr, s, options))!= RF_FAILURE)
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
    if(minPos == INT_MAX)
    {
        ret = false;
        goto cleanup;
    }

    //move the internal pointer after the substring
    move = minPos + paramLen;
    thisstr->bIndex += move;
    thisstr->INH_String.bytes += move;
    //and reduce its bytelength
    thisstr->INH_String.byteLength -= move;

    //initialize the given string if needed
    if(resultP != 0)
    {
        if(options & RF_STRINGX_ARGUMENT)
        {
            RF_StringX* result = (RF_StringX*) resultP;
            result->INH_String.byteLength = move-paramLen;
            result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_MULTIPLIER+1;
            result->bIndex=0;
            RF_MALLOC_JMP(result->INH_String.bytes, result->bSize,
                          ret = false, cleanup);
            memcpy(
                result->INH_String.bytes,
                thisstr->INH_String.bytes-move,
                result->INH_String.byteLength);
            result->INH_String.bytes[result->INH_String.byteLength] = '\0';
        }
        else
        {
            RF_String* result = (RF_String*) resultP;
            result->byteLength = move-paramLen;
            RF_MALLOC_JMP(result->bytes, result->byteLength+1,
                          ret = false, cleanup);
            memcpy(
                result->bytes,
                thisstr->INH_String.bytes-move,
                result->byteLength);
            result->bytes[result->byteLength] = '\0';
        }
    }

  cleanup:
    //success
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}
//Moves the internal string pointer after the substring formed by the @c left and @c right substrings
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_MoveAfterPair(RF_StringX* thisstr, const void* leftP,
                             const void* rightP, void* result,
                             char options, uint32_t occurence)
#else
char i_rfStringX_MoveAfterPair(RF_StringX* thisstr, const void* leftP,
                               const void* rightP, void* result,
                               char options, uint32_t occurence)
#endif
{
    uint32_t i,move,start = thisstr->bIndex;
    char found = false;
    const RF_String* left = (const RF_String*)leftP;
    const RF_String* right = (const RF_String*)rightP;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE()

    //check the occurence parameter
    if(occurence == 0)
    {
        occurence =1;
    }

    //get the in between string and if it is null return false
    for(i = 1; i <= occurence; i ++)
    {
        //attempt to get the in between string
        if(rfString_Between(thisstr,left,right,result,options) == false)
        {
            ret = false;
            goto cleanup;
        }

        //move after this occurence of the pair
        rfStringX_MoveAfter(thisstr,left,0,options);
        rfStringX_MoveAfter(thisstr,right,0,options);

        //if we found it
        if(i == occurence)
        {
            found = true;
            break;
        }
        //else depending on the passed parameter type get rid of this result
        if(options & RF_STRINGX_ARGUMENT)
        {
            rfStringX_Deinit(result);
        }
        else
        {
            rfString_Deinit(result);
        }
    }
    //if we get here and the result is not found return failure
    if(found == false)
    {
        if(options & RF_STRINGX_ARGUMENT)
        {
            rfStringX_Deinit(result);
        }
        else
        {
            rfString_Deinit(result);
        }
        //get the pointer back
        move =thisstr->bIndex - start;
        thisstr->INH_String.bytes -= move;
        thisstr->INH_String.byteLength += move;
        thisstr->bIndex = start;
        ret = false;
        goto cleanup;
    }
    //if we don't want to keep the result free it
    if(result == 0)
    {
        if(options & RF_STRINGX_ARGUMENT)
        {
            rfStringX_Deinit(result);
        }
        else
        {
            rfString_Deinit(result);
        }
    }

  cleanup:
    //success
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}
