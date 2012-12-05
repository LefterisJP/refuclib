#include <String/traversalx.h>

#include <String/common.h> //for common macro flags needed for strings

#include <rf_localmem.h> //for the local scope macros
#include <IO/rf_unicode.h> //for rfUTF8_IsContinuationByte()
#include <String/retrieval.h> //for rfString_Between
#include <String/core.h> //for rfString_Deinit()
#include <String/corex.h> //for rfStringX_Deinit()
#include "common.ph" //for rfString_FindBytePos() and other private macros and functions

#include <rf_memory.h>

#include <stdarg.h> //for the va_ macros



/*-------------------------------------------------------------------------Functions to traverse an RF_StringX-------------------------------------------------------------------------------*/


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
        RF_EXIT_LOCAL_SCOPE()
        return RF_FAILURE;
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
            result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
            result->bIndex=0;
            RF_MALLOC(result->INH_String.bytes,result->bSize);
            memcpy(result->INH_String.bytes,thisstr->INH_String.bytes-move,result->INH_String.byteLength);
            result->INH_String.bytes[result->INH_String.byteLength] = '\0';
        }
        else
        {
            RF_String* result = (RF_String*) resultP;
            result->byteLength = move-sub->byteLength;
            RF_MALLOC(result->bytes,result->byteLength+1);
            memcpy(result->bytes,thisstr->INH_String.bytes-move,result->byteLength);
            result->bytes[result->byteLength] = '\0';
        }
    }
    //return positions moved
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
        if(rfUTF8_IsContinuationByte(thisstr->INH_String.bytes[0]) == false)
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
        if(rfUTF8_IsContinuationByte(thisstr->INH_String.bytes[0]) == false)
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
char rfStringX_MoveAfterv(RF_StringX* thisstr,void* resultP,const char options,const unsigned char parN, ...)
{
    uint32_t i,paramLen,move;
    int32_t minPos;
    int32_t thisPos;
    //will keep the argument list
    va_list argList;
    RF_ENTER_LOCAL_SCOPE()

    // will keep the winning parameter length
    paramLen = 0;
    //get the parameter characters
    va_start(argList,parN);
    minPos = LONG_MAX;
    for(i = 0; i < parN; i++)
    {
        //get the param
        RF_String* s = va_arg(argList,RF_String*);
        //if the parameter got found in the string see if it's the closest
        if( (thisPos = rfString_FindBytePos(thisstr,s,options))!= RF_FAILURE)
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
    if(minPos == LONG_MAX)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
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
            result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
            result->bIndex=0;
            RF_MALLOC(result->INH_String.bytes,result->bSize);
            memcpy(result->INH_String.bytes,thisstr->INH_String.bytes-move,result->INH_String.byteLength);
            result->INH_String.bytes[result->INH_String.byteLength] = '\0';
        }
        else
        {
            RF_String* result = (RF_String*) resultP;
            result->byteLength = move-paramLen;
            RF_MALLOC(result->bytes,result->byteLength+1);
            memcpy(result->bytes,thisstr->INH_String.bytes-move,result->byteLength);
            result->bytes[result->byteLength] = '\0';
        }
    }
    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}
//Moves the internal string pointer after the substring formed by the @c left and @c right substrings
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_MoveAfterPair(RF_StringX* thisstr,const void* leftP,const void* rightP,void* result,char options,uint32_t occurence)
#else
char i_rfStringX_MoveAfterPair(RF_StringX* thisstr,const void* leftP,const void* rightP,void* result,char options,uint32_t occurence)
#endif
{
    uint32_t i,move,start = thisstr->bIndex;
    char found = false;
    const RF_String* left = (const RF_String*)leftP;
    const RF_String* right = (const RF_String*)rightP;
    RF_ENTER_LOCAL_SCOPE()

    //check the occurence parameter
    if(occurence == 0)
        occurence =1;


    //get the in between string and if it is null return false
    for(i = 1; i <= occurence; i ++)
    {
        //attempt to get the in between string
        if(rfString_Between(thisstr,left,right,result,options) == false)
        {
            RF_EXIT_LOCAL_SCOPE()
            return false;
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
            rfStringX_Deinit(result);
        else
            rfString_Deinit(result);
    }
    //if we get here and the result is not found return failure
    if(found == false)
    {
        if(options & RF_STRINGX_ARGUMENT)
            rfStringX_Deinit(result);
        else
            rfString_Deinit(result);
        //get the pointer back
        move =thisstr->bIndex - start;
        thisstr->INH_String.bytes -= move;
        thisstr->INH_String.byteLength += move;
        thisstr->bIndex = start;
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //if we don't want to keep the result free it
    if(result == 0)
    {
        if(options & RF_STRINGX_ARGUMENT)
            rfStringX_Deinit(result);
        else
            rfString_Deinit(result);
    }

    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}
