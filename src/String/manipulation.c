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
#include <String/manipulation.h>
/*------------- Module related inclusion -------------*/
#include <String/stringx_decl.h> //for RF_StringX
#include <String/corex.h> //for rfStringX_Deinit() and rfStringX_FromString_IN
#include <Utils/constcmp.h> //for RF_HEXEQ_C() used in the iteration macros
#include <String/core.h> //for string iterations
#include <String/retrieval.h> //for rfString_Length()
#include "common.ph" //for required string private macros and functions
/*------------- Outside Module inclusion -------------*/
#include <String/unicode.h> //for unicode functions
//for bool
    #include <Definitions/retcodes.h>
//for error logging macros
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for local scope macros
    #include <Utils/localscope.h>
/*------------- libc inclusion --------------*/
#include <limits.h> //for UINT_MAX e.t.c.
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/


// Appends the parameter String to this one
char rfString_Append(RF_String* thisstr,const void* otherP)
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
    thisstr->byteLength +=other->byteLength;
    //reallocate this string to fit the new addition
    RF_REALLOC_JMP(thisstr->bytes, char, thisstr->byteLength+1,
                   ret = false, cleanup);
    //add the string to this one
    strncat(thisstr->bytes, other->bytes, other->byteLength);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

// Appends an integer to the string
char rfString_Append_i(RF_String* thisstr,const int32_t i)
{
    //create a new buffer for the string big enough to fit any number plus the original string
    char* buff;
    int rc;
    RF_MALLOC(buff, thisstr->byteLength+15, false);//max uint32_t is 
    //4,294,967,295 in most environment so 12 chars will certainly fit it
    //put the int32_t inside the string
    if((rc=snprintf(buff, thisstr->byteLength+15,
                    "%s%i", thisstr->bytes, i)) < 0)
    {
        RF_ERROR("During appending an int to a string snprintf failed");
        free(buff);
        return false;
    }
    if(rc > thisstr->byteLength + 15)
    {
        RF_ERROR("During appending an int to a string snprintf failed"
                 "because of insufficient buffer size");
        free(buff);
        return false;
    }
    //free the previous c string
    free(thisstr->bytes);
    //point the string pointer to the new string
    thisstr->bytes = buff;
    thisstr->byteLength = strlen(thisstr->bytes);
    return true;
}
// Appends a float to the string. <b>Can't be used with RF_StringX</b>
char rfString_Append_f(RF_String* thisstr,const float f)
{
    //a temporary buffer to hold the float and the string
    char* buff;
    int rc;
    RF_MALLOC(buff, thisstr->byteLength + 64, false);
    //put the float inside the string
    if((rc=snprintf(buff, thisstr->byteLength + 64,
                    "%s%f",thisstr->bytes,f)) < 0)
    {
        RF_ERROR("During appending a float to a string snprintf failed");
        free(buff);
        return false;
    }
    if(rc > thisstr->byteLength + 64)
    {
        RF_ERROR("During appending a float to a string snprint failed "
                 "due to insufficient buffer size");
        free(buff);
        return false;
    }
    //free the previous c string
    free(thisstr->bytes);
    //point the string pointer to the new string
    thisstr->bytes = buff;
    thisstr->byteLength = strlen(thisstr->bytes);
    return true;
}

// Prepends the parameter String to this string
char rfString_Prepend(RF_String* thisstr,const void* otherP)
{
    const RF_String* other = (const RF_String*)otherP;
    uint32_t size;
    char ret = true;
    int32_t i;//is not unsigned since it goes to -1 in the loop
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(other == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the other string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //keeep the original byte size of the string
    size = thisstr->byteLength;
    //calculate the new lengths
    thisstr->byteLength += other->byteLength;
    //reallocate this string to fit the new addition
    RF_REALLOC_JMP(thisstr->bytes, char, thisstr->byteLength+1,
                   ret = false, cleanup);
    //move the pre-existing string to the end of the buffer, by dislocating each byte by cstrlen
    for(i =size; i >=0 ; i--)
    {
        thisstr->bytes[i+other->byteLength] = thisstr->bytes[i];
    }
    //and now add the new string to the start
    memcpy(thisstr->bytes,other->bytes,other->byteLength);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all of the specifed string occurences from this String matching case or not, DOES NOT reallocate buffer size.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Remove(void* thisstrP, const void* rstrP, uint32_t number,
                     const char options)
#else
char i_rfString_Remove(void* thisstrP, const void* rstrP, uint32_t number,
                       const char options)
#endif
{
    RF_String* thisstr = (RF_String*)thisstrP;
    const RF_String* rstr = (const RF_String*)rstrP;
    uint32_t i,count,occurences=0;
    int32_t bytePos;
    char found = false, ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(rstr == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the To Remove string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //as long as we keep finding rstr in the string keep removing it
    do
    {   //if the substring is not found
        if((bytePos = rfString_FindBytePos(
                thisstr, rstr, options)) == RF_FAILURE)
        {
            //if we have not even found it once , we fail
            if(found == false)
            {
                ret = false;
                goto cleanup;
            }
            else //else we are done
                break;
        }
        //substring found
        found = true;
        //move all of the string a position back
        count = 0;
        for(i = bytePos; i <=thisstr->byteLength; i ++)
        {
            thisstr->bytes[i] = thisstr->bytes[i+rstr->byteLength];
            count++;
        }
        //now change the byte length
        thisstr->byteLength -= rstr->byteLength;
        //count the number of occurences and if we reached the required amount, stop
        occurences++;
        if(occurences == number)
        {
            break;
        }
    }while(bytePos != RF_FAILURE);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all of the characters of the string except those specified
char rfString_KeepOnly(void* thisstrP,const void* keepstrP)
{
    uint32_t keepLength,i,j,charValue,temp;
    uint32_t *keepChars;
    RF_String* thisstr = (RF_String*)thisstrP;
    const RF_String* keepstr = (const RF_String*)keepstrP;
    char exists,charBLength, ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(keepstr == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the keep string argument");
        ret = false;
        goto cleanup;
    }
#endif

    //first let's get all of the characters of the keep string in an array
    i=0;
    keepLength = rfString_Length(keepstr);
    RF_MALLOC_JMP(keepChars, 4*keepLength, ret = false, cleanup);
    rfString_Iterate_Start(keepstr,i,charValue)
        keepChars[i] = charValue;
    rfString_Iterate_End(i)
    //now iterate every character of this string
    i=0;
    rfString_Iterate_Start(thisstr,i,charValue)
        //for every character check if it exists in the keep str
        exists = false;
        for(j=0;j<keepLength; j++)
        {
            if(keepChars[j] == charValue)
            {
                exists = true;
            }
        }
        //if it does not exist, move the string back to cover it so that it
        // effectively gets deleted
        if(exists == false)
        {
            if((charBLength = rfUTF8_FromCodepoint(charValue,&temp)) < 0)
            {
                RF_ERROR("Could not decode a codepoint into UTF-8");
                ret = false;
                goto cleanup2;
            }
            //this is kind of a dirty way to do it. the rfString_Iterate_Start macro internally uses a byteIndex_ variable
            //we use that here to determine the current byteIndex_ of the string in the iteration and move the string backwards
            memmove(
                thisstr->bytes+byteIndex_,
                thisstr->bytes+byteIndex_+charBLength,
                thisstr->byteLength-byteIndex_);
            thisstr->byteLength-=charBLength;
            continue;//by contiuing here we make sure that the current string position won't be moved to assure that we also check the newly move characters
        }
    rfString_Iterate_End(i)


    cleanup2:
    //before returning free the keep string's character array
    free(keepChars);
#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION //only used for malloc fail
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes the first n characters from the start of the string
char rfString_PruneStart(void* thisstrP,uint32_t n)
{
    RF_String* thisstr = (RF_String*)thisstrP;
    //iterate the characters of the string
    uint32_t i;
    uint32_t length = 0;
    unsigned nBytePos = 0;
    char found = false;
    RF_STRING_ITERATE_START(thisstr,length,i);
        //if we reach the number of characters passed as a parameter, note it
        if(length == n)
        {
            //remember that now i is the byte position we need
            nBytePos = i;
            found = true;
            break;
        }
    RF_STRING_ITERATE_END(length,i)

    //if the string does not have n chars to remove it becomes an empty string
    // and we return failure
    if(found == false)
    {
        thisstr->bytes[0] = '\0';
        thisstr->byteLength = 0;
        return false;
    }

    //move the string back to cover the empty places.reallocation here would be an overkill, everything will be freed together when the string gets freed
    for(i =0; i < thisstr->byteLength-nBytePos+1;i++ )
    {
        thisstr->bytes[i] = thisstr->bytes[i+nBytePos];
    }
    //get the new bytelength
    thisstr->byteLength -= nBytePos;

    return true;
}

//Removes the last n characters from the end of the string
char rfString_PruneEnd(void* thisstrP,uint32_t n)
{
    RF_String* thisstr = (RF_String*)thisstrP;
    //start the iteration of the characters from the end of the string
    int32_t nBytePos = -1;
    uint32_t length,i;
    RF_STRING_ITERATEB_START(thisstr,length,i)
        //if we found the requested number of characters from the end of the string
        if(length == n)
        {
            //remember that now i is the byte position we need
            nBytePos = i;
            break;
        }
    RF_STRING_ITERATEB_END(length,i)

    //if the string does not have n chars to remove it becomes an empty string and we return failure
    if(nBytePos == -1)
    {
        thisstr->bytes[0] = '\0';
        return false;
    }

    //just set the end of string character characters back, reallocation here would be an overkill, everything will be freed together when the string gets freed
    thisstr->bytes[nBytePos] = '\0';
    //and also set the new byte length
    thisstr->byteLength -= (thisstr->byteLength - nBytePos);
    //success
    return true;
}

//Removes n characters from the position p of the string counting backwards. If there is no space to do so, nothing is done and returns false.
char rfString_PruneMiddleB(void* thisstrP, uint32_t p, uint32_t n)
{
    RF_String* thisstr = (RF_String*)thisstrP;
    uint32_t i,length;
    int32_t pBytePos,nBytePos;
    //if we ask to remove more characters from the position that it would be possible do nothign and return false
    if(n>p+1)
    {
        return false;
    }
    //iterate the characters of the string

    pBytePos = nBytePos = -1;
    RF_STRING_ITERATE_START(thisstr,length,i)
        //if we reach the number of characters passed as a parameter, note it
        if(length == p+1)
        {
            //we search for p+1  because we want to include all of the p character
            pBytePos = i;
            //also break since we don't care after position p
            break;
        }
        if(length == p-n+1)//+1 is to make sure that indexing works from 0
            nBytePos = i;
    RF_STRING_ITERATE_END(length,i)

    //if the position was not found in the string do nothing
    if(pBytePos == -1 || nBytePos == -1)
    {
        return false;
    }
    //move the contents of the string to cover the removed characters and change its length
    memmove(
        thisstr->bytes+nBytePos,
        thisstr->bytes+pBytePos,
        thisstr->byteLength-pBytePos+1);
    thisstr->byteLength -= (pBytePos-nBytePos);

    return true;
}

//Removes n characters from the position p of the string counting forwards. If there is no space, nothing is done and returns false.
char rfString_PruneMiddleF(void* thisstrP, uint32_t p, uint32_t n)
{
    RF_String* thisstr = (RF_String*)thisstrP;
    //iterate the characters of the string
    uint32_t j,i,length;
    int32_t pBytePos,nBytePos;
    pBytePos = nBytePos = -1;
    RF_STRING_ITERATE_START(thisstr,length,i)
        //if we reach the number of characters passed as a parameter, note it
        if(length == p)
        {
            pBytePos = i;
        }

        if(length == p+n)
        {
            nBytePos = i;
            break;//since we got all the data we needed
        }

    RF_STRING_ITERATE_END(length,i)

    //if the position was not found in the string do nothing
    if(pBytePos == -1 )
    {
        return false;
    }

    //if we did not find the byte position of p+n then we remove everything from pBytePos until the end of the string
    if(nBytePos == -1)
    {
        thisstr->bytes[pBytePos] = '\0';
        thisstr->byteLength -= (thisstr->byteLength-pBytePos);
        return true;
    }

    //move the bytes in the buffer to remove the requested characters
    for(i=pBytePos,j=0;j<= thisstr->byteLength-nBytePos+1; i ++,j++) // here +2 is for (+1 for pbytePos to go to the start of pth character) (+1 for the byteLength to include the null termination character)
    {
        thisstr->bytes[i] = thisstr->bytes[nBytePos+j];
    }

    //find the new byte length
    thisstr->byteLength -= (nBytePos - pBytePos);
    return true;
}

// Replaces all of the specified sstr substring from the String with rstr and reallocates size, unless the new size is smaller
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Replace(RF_String* thisstr, const void* sstrP,
                      const void* rstrP, const uint32_t num,
                      const char options)
#else
char i_rfString_Replace(RF_String* thisstr, const void* sstrP,
                        const void* rstrP, const uint32_t num,
                        const char options)
#endif
{
    const RF_String* sstr = (const RF_String*)sstrP;
    const RF_String* rstr = (const RF_String*)rstrP;
    RF_StringX temp;//just a temporary string for finding the occurences
    //will keep the number of found instances of the substring
    uint32_t foundN = 0;
    //will keep the number of given instances to find
    uint32_t number = num;
    uint32_t diff,i,j;
    int32_t * bytePositions;
    char ret = true;
    uint32_t bSize = 50;

    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(sstr == NULL || rstr == NULL)
    {
        RF_ERROR("Provided a NULL pointer for either the search or "
                 "for the replace string");
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

    RF_MALLOC_JMP(bytePositions, bSize*sizeof(int32_t),
                  ret = false, cleanup);
    //if the given num is 0 just make sure we replace all
    if(number == 0)
        number = UINT_MAX;//max number of occurences

    //find how many occurences exist
    if(!rfStringX_FromString_IN(&temp,thisstr))
    {
        ret = false;
        goto cleanup;
    }
    while((bytePositions[foundN] = rfString_FindBytePos(
               &temp, sstr, options))  != RF_FAILURE)
    {
        int32_t move = bytePositions[foundN] + sstr->byteLength;
        bytePositions[foundN] = bytePositions[foundN]+temp.bIndex;
        temp.bIndex += move;
        temp.INH_String.bytes += move;
        temp.INH_String.byteLength -= move;
        foundN++;
        //if buffer is in danger of overflow realloc it
        if(foundN > bSize)
        {
            bSize *=2;
            RF_REALLOC_JMP(bytePositions, int32_t, bSize,
                           ret = false, cleanup);
        }
        //if we found the required number of occurences break;
        if(foundN >= number)
            break;
    }
    rfStringX_Deinit(&temp);
    //make sure that the number of occurence to replace do not exceed the actual number of occurences
    if(number > foundN)
        number = foundN;
    //act depending on the size difference of rstr and sstr
    if(rstr->byteLength > sstr->byteLength) //replace string is bigger than the removed one
    {
        int32_t nSize;

        diff = rstr->byteLength - sstr->byteLength;
        //reallocate the string to fit the new bigger size
        nSize= thisstr->byteLength +1 + number*diff;
        RF_REALLOC_JMP(thisstr->bytes, char, nSize, ret = false, cleanup);
        //now replace all the substrings one by one
        for(i = 0; i < number; i ++)
        {
            memmove(thisstr->bytes+bytePositions[i]+sstr->byteLength+diff,
                    thisstr->bytes+bytePositions[i]+sstr->byteLength,
                    thisstr->byteLength+1-(bytePositions[i]+sstr->byteLength));
            //copy in the replacement
            memcpy(thisstr->bytes+bytePositions[i], rstr->bytes,
                   rstr->byteLength);
            //also increase the original size (since now we moved the whole string by one replacement)
            thisstr->byteLength += diff;
            //also increase all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < number; j ++)
                bytePositions[j] = bytePositions[j]+diff;
        }
    }
    else if( rstr->byteLength < sstr->byteLength) //replace string is smaller than the removed one
    {
        //get the differenc in byte length of removed substring and replace string
        diff = sstr->byteLength-rstr->byteLength;

        //now replace all the substrings one by one
        for(i =0; i < number; i ++)
        {
            //copy in the replacement
            memcpy(thisstr->bytes+bytePositions[i], rstr->bytes,
                   rstr->byteLength);
            //move all of the contents of the string to fit the replacement
            memmove(thisstr->bytes+bytePositions[i]+rstr->byteLength,
                    thisstr->bytes+bytePositions[i]+sstr->byteLength,
                    thisstr->byteLength+1-(bytePositions[i]+sstr->byteLength));
            //reduce bytelength
            thisstr->byteLength -= diff;
            //also decrease all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < number; j ++)
                bytePositions[j] = bytePositions[j]-diff;
        }
        //just note that reallocating downwards is not necessary
    }
    else //replace and remove strings are equal
    {
        for(i = 0; i < number; i ++)
            strncpy(thisstr->bytes+bytePositions[i], rstr->bytes,
                    rstr->byteLength);
    }
    free(bytePositions);
    //end
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all characters of a substring only from the start of the String
char rfString_TrimStart(void* thisstrP,const void* subP)
{
    RF_String* thisstr = (RF_String*) thisstrP;
    const RF_String* sub = (const RF_String*) subP;
    char ret = false,noMatch;
    uint32_t charValue,i = 0,*subValues,j,subLength,bytePos;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(sub == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the substring argument");
        ret = false;
        goto cleanup;
    }
#endif
    //firstly get all of the characters of the substring in an array
    subLength = rfString_Length(sub);
    RF_MALLOC_JMP(subValues, 4*subLength, ret=false, cleanup);
    rfString_Iterate_Start(sub,i,charValue)
        subValues[i] = charValue;
    rfString_Iterate_End(i)

    //iterate thisstring from the beginning
    i = 0;
    RF_STRING_ITERATE_START(thisstr,i,bytePos)
        noMatch = true;
        //for every substring character
        for(j = 0;j < subLength; j++)
        {
            //if we got a match
            if(rfString_BytePosToCodePoint(thisstr,bytePos) == subValues[j])
            {
                ret = true;
                noMatch = false;
                break;
            }
        }
        //if we get out of iterating the substring without having found a match, we get out of the iteration in general
        if(noMatch)
        {
            break;
        }
    RF_STRING_ITERATE_END(i,bytePos)

    //if we had any match
    if(ret == true)
    {
        //remove the characters
        for(i =0; i < thisstr->byteLength-bytePos+1;i++ )
            thisstr->bytes[i] = thisstr->bytes[i+bytePos];
        //also change bytelength
        thisstr->byteLength -= bytePos;
    }

    //free stuff and return
    free(subValues);

#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION //only used for malloc fail
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all characters of a substring starting from the end of the String
char rfString_TrimEnd(void* thisstrP, const void* subP)
{
    RF_String* thisstr = (RF_String*) thisstrP;
    const RF_String*sub = (const RF_String*) subP;
    char ret = false,noMatch;
    uint32_t charValue,i = 0,*subValues,j,subLength,bytePos,lastBytePos=0;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(sub == NULL)
    {
        RF_ERROR("Provided a NULL pointer for the substring argument");
        ret = false;
        goto cleanup;
    }
#endif
    //firstly get all of the characters of the substring in an array
    subLength = rfString_Length(sub);
    RF_MALLOC_JMP(subValues, 4*subLength, ;, cleanup);
    rfString_Iterate_Start(sub,i,charValue)
        subValues[i] = charValue;
    rfString_Iterate_End(i)

    //iterate thisstring from the end
    i = 0;
    RF_STRING_ITERATEB_START(thisstr,i,bytePos)
        noMatch = true;
        //for every substring character
        for(j = 0;j < subLength; j++)
        {
            //if we got a match
            if(rfString_BytePosToCodePoint(thisstr,bytePos) == subValues[j])
            {
                ret = true;
                noMatch = false;
                lastBytePos = bytePos;
                break;
            }
        }
        //if we get out of iterating the substring without having found a match, we get out of the iteration in general
        if(noMatch)
        {
            break;
        }
    RF_STRING_ITERATEB_END(i,bytePos)

    //if we had any match
    if(ret == true)
    {
        //just set the end of string there
        thisstr->bytes[lastBytePos] = '\0';
        //and also set the new byte length
        thisstr->byteLength -= (thisstr->byteLength - lastBytePos);
    }

    //free stuff and return
    free(subValues);

#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION //only used for malloc fail
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all characters of a substring from both ends of the given String
char rfString_Trim(void* thisstrP,const void* subP)
{
    RF_ENTER_LOCAL_SCOPE();
    char res1 = rfString_TrimStart(thisstrP,subP);
    char res2 = rfString_TrimEnd(thisstrP,subP);
    RF_EXIT_LOCAL_SCOPE();
    return res1|res2;
}
