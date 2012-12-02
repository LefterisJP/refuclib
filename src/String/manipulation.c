#include <String/manipulation.h>

#include <String/core.h> //for string iterations
#include <String/retrieval.h> //for rfString_Length()
#include "common.ph" //for required string private macros and functions
#include <String/rf_stringx.h> //for RF_Stringx functions

#include <rf_memory.h>
#include <rf_error.h>

#include <rf_localmem.h> //for local memory stack
#include <IO/rf_unicode.h> //for unicode functions

#include <string.h> //for some c stdlib string functions

/*------------------------------------------------------------------------ RF_String manipulation functions-------------------------------------------------------------------------------*/


// Appends the parameter String to this one
void rfString_Append(RF_String* thisstr,const void* otherP)
{
    const RF_String* other = (const RF_String*)otherP;
    RF_ENTER_LOCAL_SCOPE()
    ///@note Here if a null addition is given lots of actions are done but the result is safe and the same string as the one entered.
    ///A check here would result in an additional check for every appending so I decided against it
    //calculate the new length
    thisstr->byteLength +=other->byteLength;
    //reallocate this string to fit the new addition
    RF_REALLOC(thisstr->bytes,char,thisstr->byteLength+1);
    //add the string to this one
    strncat(thisstr->bytes,other->bytes,other->byteLength);
    RF_EXIT_LOCAL_SCOPE()
}

// Appends an integer to the string
void rfString_Append_i(RF_String* thisstr,const int32_t i)
{
    //create a new buffer for the string big enough to fit any number plus the original string
    char* buff;
    RF_MALLOC(buff,thisstr->byteLength+15);//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    //put the int32_t inside the string
    sprintf(buff,"%s%i",thisstr->bytes,i);
    //free the previous c string
    free(thisstr->bytes);
    //point the string pointer to the new string
    thisstr->bytes = buff;
    thisstr->byteLength = strlen(thisstr->bytes);
}
// Appends a float to the string. <b>Can't be used with RF_StringX</b>
void rfString_Append_f(RF_String* thisstr,const float f)
{
    //a temporary buffer to hold the float and the string
    char* buff;
    RF_MALLOC(buff,thisstr->byteLength+64);
    //put the float inside the string
    sprintf(buff,"%s%f",thisstr->bytes,f);
    //free the previous c string
    free(thisstr->bytes);
    //point the string pointer to the new string
    thisstr->bytes = buff;
    thisstr->byteLength = strlen(thisstr->bytes);
}

// Prepends the parameter String to this string
void rfString_Prepend(RF_String* thisstr,const void* otherP)
{
    const RF_String* other = (const RF_String*)otherP;
    uint32_t size;
    int32_t i;//is not unsigned since it goes to -1 in the loop
    RF_ENTER_LOCAL_SCOPE()
    //keeep the original byte size of the string
    size = thisstr->byteLength;
    //calculate the new lengths
    thisstr->byteLength += other->byteLength;
    //reallocate this string to fit the new addition
    RF_REALLOC(thisstr->bytes,char,thisstr->byteLength+1);
    //move the pre-existing string to the end of the buffer, by dislocating each byte by cstrlen
    for(i =size; i >=0 ; i--)
        thisstr->bytes[i+other->byteLength] = thisstr->bytes[i];
    //and now add the new string to the start
    memcpy(thisstr->bytes,other->bytes,other->byteLength);
    RF_EXIT_LOCAL_SCOPE()
}

//Removes all of the specifed string occurences from this String matching case or not, DOES NOT reallocate buffer size.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Remove(void* thisstrP,const void* rstrP,uint32_t number,const char options)
#else
char i_rfString_Remove(void* thisstrP,const void* rstrP,uint32_t number,const char options)
#endif
{
    RF_String* thisstr = (RF_String*)thisstrP;
    const RF_String* rstr = (const RF_String*)rstrP;
    uint32_t i,count,occurences=0;
    int32_t bytePos;
    char found = false;
    RF_ENTER_LOCAL_SCOPE()
    //as long as we keep finding rstr in the string keep removing it
    do
    {   //if the substring is not found
        if( (bytePos = rfString_FindBytePos(thisstr,rstr,options)) == RF_FAILURE)
        {
            //if we have not even found it once , we fail
            if(found == false)
            {
                RF_EXIT_LOCAL_SCOPE()
                return false;
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
            break;
    }while(bytePos != RF_FAILURE);
    //succcess
    RF_EXIT_LOCAL_SCOPE()
    return true;
}

//Removes all of the characters of the string except those specified
void rfString_KeepOnly(void* thisstrP,const void* keepstrP)
{
    uint32_t keepLength,i,j,charValue,temp;
    uint32_t *keepChars;
    RF_String* thisstr = (RF_String*)thisstrP;
    const RF_String* keepstr = (const RF_String*)keepstrP;
    char exists,charBLength;
    RF_ENTER_LOCAL_SCOPE()
    //first let's get all of the characters of the keep string in an array
    i=0;
    keepLength = rfString_Length(keepstr);
    RF_MALLOC(keepChars,4*keepLength);
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
                exists = true;
        }
        //if it does not exist, move the string back to cover it so that it effectively gets deleted
        if(exists == false)
        {
            charBLength = rfUTF8_FromCodepoint(charValue,&temp);
            //this is kind of a dirty way to do it. the rfString_Iterate_Start macro internally uses a byteIndex_ variable
            //we use that here to determine the current byteIndex_ of the string in the iteration and move the string backwards
            memmove(thisstr->bytes+byteIndex_,thisstr->bytes+byteIndex_+charBLength,thisstr->byteLength-byteIndex_);
            thisstr->byteLength-=charBLength;
            continue;//by contiuing here we make sure that the current string position won't be moved to assure that we also check the newly move characters
        }
    rfString_Iterate_End(i)
    //before returning free the keep string's character array
    free(keepChars);
    RF_EXIT_LOCAL_SCOPE()
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

    //if the string does not have n chars to remove it becomes an empty string and we return failure
    if(found == false)
    {
        thisstr->bytes[0] = '\0';
        thisstr->byteLength = 0;
        return false;
    }

    //move the string back to cover the empty places.reallocation here would be an overkill, everything will be freed together when the string gets freed
    for(i =0; i < thisstr->byteLength-nBytePos+1;i++ )
        thisstr->bytes[i] = thisstr->bytes[i+nBytePos];

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
char rfString_PruneMiddleB(void* thisstrP,uint32_t p,uint32_t n)
{
    RF_String* thisstr = (RF_String*)thisstrP;
    //if we ask to remove more characters from the position that it would be possible do nothign and return false
    if(n>p+1)
        return false;

    //iterate the characters of the string
    uint32_t i,length;
    int32_t pBytePos,nBytePos;
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
        return false;
    //move the contents of the string to cover the removed characters and change its length
    memmove(thisstr->bytes+nBytePos,thisstr->bytes+pBytePos,thisstr->byteLength-pBytePos+1);
    thisstr->byteLength -= (pBytePos-nBytePos);

    return true;
}

//Removes n characters from the position p of the string counting forwards. If there is no space, nothing is done and returns false.
char rfString_PruneMiddleF(void* thisstrP,uint32_t p,uint32_t n)
{
    RF_String* thisstr = (RF_String*)thisstrP;
    //iterate the characters of the string
    uint32_t j,i,length;
    int32_t pBytePos,nBytePos;
    pBytePos = nBytePos = -1;
    RF_STRING_ITERATE_START(thisstr,length,i)
        //if we reach the number of characters passed as a parameter, note it
        if(length == p)
            pBytePos = i;

        if(length == p+n)
        {
            nBytePos = i;
            break;//since we got all the data we needed
        }

    RF_STRING_ITERATE_END(length,i)

    //if the position was not found in the string do nothing
    if(pBytePos == -1 )
        return false;

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
char rfString_Replace(RF_String* thisstr,const void* sstrP,const void* rstrP,const uint32_t num,const char options)
#else
char i_rfString_Replace(RF_String* thisstr,const void* sstrP,const void* rstrP,const uint32_t num,const char options)
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
    RF_ENTER_LOCAL_SCOPE()

    //if the substring string is not even found return false
    if(rfString_FindBytePos(thisstr,sstr,options) == RF_FAILURE)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //create a buffer that will keep the byte positions
    uint32_t bSize = 50;
    int32_t * bytePositions;
    RF_MALLOC(bytePositions,bSize*sizeof(int32_t));
    //if the given num is 0 just make sure we replace all
    if(number == 0)
        number = ULONG_MAX;//max number of occurences

    //find how many occurences exist
    rfStringX_FromString_IN(&temp,thisstr);
    while( (bytePositions[foundN] = rfString_FindBytePos(&temp,sstr,options))  != RF_FAILURE)
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
            RF_REALLOC(bytePositions,int32_t,bSize);
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
        int32_t orSize,nSize;

        diff = rstr->byteLength - sstr->byteLength;
        //will keep the original size in bytes
        orSize = thisstr->byteLength +1;
        //reallocate the string to fit the new bigger size
        nSize= orSize + number*diff;
        RF_REALLOC(thisstr->bytes,char,nSize)
        //now replace all the substrings one by one
        for(i = 0; i < number; i ++)
        {
            //move all of the contents of the string to fit the replacement
            for(j =orSize+diff-1; j > bytePositions[i]+sstr->byteLength; j -- )
                thisstr->bytes[j] = thisstr->bytes[j-diff];
            //copy in the replacement
            strncpy(thisstr->bytes+bytePositions[i],rstr->bytes,rstr->byteLength);
            //also increase the original size (since now we moved the whole string by one replacement)
            orSize += diff;
            //also increase all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < number; j ++)
                bytePositions[j] = bytePositions[j]+diff;

        }
        //finally let's keep the new byte length
        thisstr->byteLength = nSize-1;
    }
    else if( rstr->byteLength < sstr->byteLength) //replace string is smaller than the removed one
    {
        //get the differenc in byte length of removed substring and replace string
        diff = sstr->byteLength-rstr->byteLength;

        //now replace all the substrings one by one
        for(i =0; i < number; i ++)
        {
            //copy in the replacement
            strncpy(thisstr->bytes+bytePositions[i],rstr->bytes,rstr->byteLength);
            //move all of the contents of the string to fit the replacement
            for(j =bytePositions[i]+rstr->byteLength; j < thisstr->byteLength; j ++ )
                thisstr->bytes[j] = thisstr->bytes[j+diff];
            //also decrease all the subsequent found byte positions since there is a change of string size
            for(j = i+1; j < number; j ++)
                bytePositions[j] = bytePositions[j]-diff;
        }
        //finally let's keep the new byte length
        thisstr->byteLength -= diff*number;
        //just note that reallocating downwards is not necessary
    }
    else //replace and remove strings are equal
    {
        for(i = 0; i < number; i ++)
            strncpy(thisstr->bytes+bytePositions[i],rstr->bytes,rstr->byteLength);
    }
    free(bytePositions);
    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}

//Removes all characters of a substring only from the start of the String
char rfString_TrimStart(void* thisstrP,const void* subP)
{
    RF_String* thisstr = (RF_String*) thisstrP;
    const RF_String*sub = (const RF_String*) subP;
    char ret = false,noMatch;
    uint32_t charValue,i = 0,*subValues,j,subLength,bytePos;
    RF_ENTER_LOCAL_SCOPE()

    //firstly get all of the characters of the substring in an array
    subLength = rfString_Length(sub);
    RF_MALLOC(subValues,4*subLength)
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
            break;
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

    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

//Removes all characters of a substring starting from the end of the String
char rfString_TrimEnd(void* thisstrP,const void* subP)
{
    RF_String* thisstr = (RF_String*) thisstrP;
    const RF_String*sub = (const RF_String*) subP;
    char ret = false,noMatch;
    uint32_t charValue,i = 0,*subValues,j,subLength,bytePos,lastBytePos=0,testity;
    RF_ENTER_LOCAL_SCOPE()

    //firstly get all of the characters of the substring in an array
    subLength = rfString_Length(sub);
    RF_MALLOC(subValues,4*subLength)
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
            if((testity=rfString_BytePosToCodePoint(thisstr,bytePos)) == subValues[j])
            {
                ret = true;
                noMatch = false;
                lastBytePos = bytePos;
                break;
            }
        }
        //if we get out of iterating the substring without having found a match, we get out of the iteration in general
        if(noMatch)
            break;
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
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

//Removes all characters of a substring from both ends of the given String
char rfString_Trim(void* thisstrP,const void* subP)
{
    RF_ENTER_LOCAL_SCOPE()
    char res1 = rfString_TrimStart(thisstrP,subP);
    char res2 = rfString_TrimEnd(thisstrP,subP);
    RF_EXIT_LOCAL_SCOPE()
    return res1|res2;
}
