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
//for bool and retcodes
    #include <Definitions/retcodes.h>
//for rfUTF8_IsContinuationByte()
    #include <String/unicode.h>  
//for RF_HEXEQ_C() macro and others
    #include <Utils/constcmp.h> 
//for the local scope macros
    #include <Utils/localscope.h>//for the local scope macros
//for memory alllocation macros
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <stdarg.h> //for the va_ macros
#include <limits.h> //for INT_MAX e.t.c.
//*----------------------------End of Includes------------------------------------------


static inline bool move_internal_ptr(RF_StringX* s, int32_t move,
                                     void* resultP, uint32_t len, char options)
{
    s->bIndex += move;
    rfString_Data(s) += move;
    //also reduce the byte length of this string
    rfString_ByteLength(s) -= move;

    //also if we want the string returned
    if(resultP != 0)
    {
        if(options & RF_STRINGX_ARGUMENT)
        {
            RF_StringX* result = (RF_StringX*) resultP;
            rfString_ByteLength(result) = move - len;
            result->bSize = (
                rfString_ByteLength(result) * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER
            );
            result->bIndex = 0;
            RF_MALLOC(rfString_Data(result), result->bSize, false);
            memcpy(
                rfString_Data(result),
                rfString_Data(s) - move,
                rfString_ByteLength(result)
            );
        }
        else
        {
            RF_String* result = (RF_String*) resultP;
            rfString_ByteLength(result) = move - len;
            RF_MALLOC(rfString_Data(result), rfString_ByteLength(result), false);
            memcpy(
                rfString_Data(result),
                rfString_Data(s) - move,
                rfString_ByteLength(result)
            );
        }
    }
    return true;
}

int32_t rfStringX_MoveAfter(RF_StringX* thisstr, const void* sub,
                            void* result, const char options)
{
    int32_t move;
    RF_ENTER_LOCAL_SCOPE();

    //check for substring existence and return failure if not found
    if((move = rfString_FindBytePos(thisstr, sub, options)) == RF_FAILURE)
    {
        move = RF_FAILURE;
        goto cleanup;
    }
    //if found, move the internal pointer
    move += rfString_ByteLength(sub);

    if(!move_internal_ptr(thisstr, move, result,
                          rfString_ByteLength(sub), options))
    {
        move = RF_FAILURE;
    }
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return move;
}

void rfStringX_MoveBack(RF_StringX* thisstr, uint32_t n)
{
    uint32_t length;
    length = 0;

    while(thisstr->bIndex >0)
    {
        if(!rfUTF8_IsContinuationByte(rfString_Data(thisstr)[0]))
        {
            if(n == length)
            {
                break;
            }
            length ++;
        }
        //so keep changing the internal index and the bytes pointer
        rfString_Data(thisstr)--;
        thisstr->bIndex--;
        //and also keep adding to the bytelength
        rfString_ByteLength(thisstr) ++;
    }
}

void rfStringX_MoveForward(RF_StringX* thisstr, uint32_t n)
{
    uint32_t length;
    length = 0;

    while(thisstr->bIndex < thisstr->bSize)
    {
        if(!rfUTF8_IsContinuationByte(rfString_Data(thisstr)[0]))
        {
            if(n == length)
            {
                break;
            }
            length ++;
        }
        //so keep changing the internal index and the bytes pointer
        rfString_Data(thisstr)++;
        thisstr->bIndex++;
        //and reducing from the byte length
        rfString_ByteLength(thisstr)--;
    }
}

void rfStringX_Reset(RF_StringX* thisstr)
{
    rfString_Data(thisstr) -= thisstr->bIndex;
    rfString_ByteLength(thisstr) += thisstr->bIndex;
    thisstr->bIndex = 0;
}

bool rfStringX_MoveAfterv(RF_StringX* thisstr, void* result, 
                          const char options,
                          const unsigned char parN, ...)
{
    uint32_t i,paramLen;
    int32_t minPos;
    int32_t thisPos;
    //will keep the argument list
    va_list argList;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();

    // will keep the winning parameter length
    paramLen = 0;
    //get the parameter characters
    va_start(argList,parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        //get the param
        RF_String* s = va_arg(argList, RF_String*);
        //if the parameter got found in the string see if it's the closest
        if((thisPos = rfString_FindBytePos(thisstr, s, options)) != RF_FAILURE)
        {
            if(thisPos < minPos)
            {
                minPos = thisPos;
                //keep the length of the winnning parameter
                paramLen = rfString_ByteLength(s);
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
    ret = move_internal_ptr(thisstr, minPos + paramLen, result, paramLen, options);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfStringX_MoveAfterPair(RF_StringX* thisstr, const void* left,
                             const void* right, void* result,
                             char options, uint32_t occurence)
{
    uint32_t i,move,start = thisstr->bIndex;
    bool found = false;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();

    //check the occurence parameter
    if(occurence == 0)
    {
        occurence = 1;
    }

    //get the in between string and if it is null return false
    for(i = 1; i <= occurence; i ++)
    {
        //attempt to get the in between string
        if(!rfString_Between(thisstr, left, right, result, options))
        {
            ret = false;
            goto cleanup;
        }

        //move after this occurence of the pair
        rfStringX_MoveAfter(thisstr, left, 0, options);
        rfStringX_MoveAfter(thisstr, right, 0, options);

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
        move = thisstr->bIndex - start;
        rfString_Data(thisstr) -= move;
        rfString_ByteLength(thisstr) += move;
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
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
