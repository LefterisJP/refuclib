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
#include <Utils/bits.h> //for RF_BITFLAG_ON

#include <Utils/constcmp.h> //for RF_HEXEQ_C() macro and others
//for the local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>
//for the memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h> //for LOG_ERROR() macros
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <stdarg.h>
//*----------------------------End of Includes------------------------------------------

// Finds the length of the string in characters
uint32_t rfString_Length(const void* str)
{
    const RF_String* s = (const RF_String*)str;
    uint32_t length,i;
    RF_STRING_ITERATE_START(s,length,i)
    RF_STRING_ITERATE_END(length,i);
    return length;
}

//Retrieves the unicode code point of the parameter character.
uint32_t rfString_GetChar(const void* str,uint32_t c)
{
    const RF_String* thisstr = (const RF_String*)str;
    uint32_t length,i;
    uint32_t codePoint = RF_STRING_INDEX_OUT_OF_BOUNDS;
    RF_STRING_ITERATE_START(thisstr,length,i)
        //if we found the character,inspect the 4 different cases
        if(length == c)
        {
            //take the codepoint from the byte position and break from the loop
            codePoint = rfString_BytePosToCodePoint(thisstr,i);
            break;
        }
    RF_STRING_ITERATE_END(length,i)

    //and return the code point. Notice that if the character was not found this will return RF_STRING_INDEX_OUT_OF_BOUNDS
    return codePoint;
}

/*------------------------------------------------------------------------ RF_String retrieval functions-------------------------------------------------------------------------------*/

//Returns a substring of this string
char rfString_Substr(const void* thisstrP,uint32_t startPos,uint32_t charsN,RF_String* ret)
{
    uint32_t charI,byteI,startI,endI;
    char started,ended;
    started = ended = false;
    const RF_String* s = (const RF_String*)thisstrP;
    startI = endI = 0;


    RF_STRING_ITERATE_START(s,charI,byteI)
        if(charI==startPos)
        {
            startI = byteI;
            started = true;
        }
        if(charI==startPos+charsN)
        {
            endI = byteI;
            ended = true;
            break;
        }
    RF_STRING_ITERATE_END(charI,byteI)
    //check if the end was not found
    if(ended == false)
    {
        if(started == true)//if the start was found return from the start until the end of the string
        {
            endI = byteI;
        }
        else//else the substring was not found
            return false;
    }

    //else success
    ret->byteLength = endI-startI;
    ret->bytes = malloc(ret->byteLength+1);
    strncpy(ret->bytes,s->bytes+startI,ret->byteLength);
    ret->bytes[ret->byteLength]='\0';
    return true;
}


//Finds if a substring exists inside a specific part of another string.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfString_Find_i(const void* thisstr,const void* sstr,uint32_t startPos,uint32_t length,const char options)
#else
int32_t i_rfString_Find_i(const void* thisstr,const void* sstr,uint32_t startPos,uint32_t length,const char options)
#endif
{
    RF_String sub;
    int32_t ret = RF_FAILURE;//the return value
    RF_ENTER_LOCAL_SCOPE()
    //if the substring does not exist fail
    if(rfString_Substr(thisstr,startPos,length,&sub) == false)
        goto cleanup1;

    //now search for the sstr substring inside the sub substring defined by length
    if((ret=rfString_Find(&sub,sstr,options)) != RF_FAILURE)
        ret += startPos;//if found make sure to return the proper position in the original string
    rfString_Deinit(&sub);//free the sub substring and return

cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}
// Finds the existence of String sstr inside this string, either matching case or not
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfString_Find(const void* thisstr,const void* sstr,const char options)
#else
int32_t i_rfString_Find(const void* str,const void* sstrP,const char options)
#endif
{
    /// note TO SELF: If I make any changes to this function do not forget to change the private version that returns byte position too
    /// located at string_private.c and called rfString_FindByte and rfString_FindByte_s
    const RF_String* thisstr = (const RF_String*)str;
    const RF_String* sstr = (const RF_String*)sstrP;
    int32_t ret = RF_FAILURE;
    RF_ENTER_LOCAL_SCOPE()

    char* found = 0;
    //if we want to match the case of the string then it's a simple search of matching characters
    if( (RF_BITFLAG_ON( options,RF_CASE_IGNORE)) == false)
    {
        //if it is not found
        if( (found = strstr(thisstr->bytes,sstr->bytes)) == 0)
        {
            goto cleanup1;
        }
        //get the byte position
        uint32_t bytepos = found-thisstr->bytes;
        //if we need the exact string as it is given
        if(RF_BITFLAG_ON( options,RF_MATCH_WORD))
        {
            //check before the found string
            if(bytepos != 0)
            {
                //if is is not a character
                switch(thisstr->bytes[bytepos-1])
                {
                    case ' ':case '\t':case '\n':
                    break;
                    default:
                        goto cleanup1;
                    break;
                }
            }
            //check after the found string
            if(bytepos+sstr->byteLength != thisstr->byteLength)
            {
                //if is is not a character
                switch(thisstr->bytes[bytepos+sstr->byteLength])
                {
                    case ' ':case '\t':case '\n':
                    break;
                    default:
                        goto cleanup1;
                    break;
                }
            }
        }//end of the exact string option
        //success
        ret = rfString_BytePosToCharPos(thisstr,bytepos,false);
        goto cleanup1;
    }

    //else ignore case matching
    uint32_t i,j;
    //if(cstr[0] >= 0x41 && cstr[0] <= 0x5a)
    for(i=0;i<thisstr->byteLength; i ++)
    {
        //if i matches the start of the substring
        for(j = 0; j < sstr->byteLength; j++)
        {
            //if the jth char is a big letter
            if(sstr->bytes[j] >= 0x41 && sstr->bytes[j] <= 0x5a)
            {
                //no match
                if(sstr->bytes[j] != thisstr->bytes[i+j] && sstr->bytes[j]+32 != thisstr->bytes[i+j])
                    break;
                //there is a match in the jth character so let's perform additional checks if needed
                if(RF_BITFLAG_ON( options,RF_MATCH_WORD))
                {
                    //if it's the first substring character and if the string we search is not in it's beginning, check for EXACT string before
                    if(j == 0 && i != 0)
                    {
                        switch(thisstr->bytes[i-1])
                        {
                            case ' ':case '\t':case '\n':
                            break;
                            default:
                                goto cleanup1;
                            break;
                        }
                    }
                }//exact string check if ends
            }
            //small letter
            else if(sstr->bytes[j] >= 0x61 && sstr->bytes[j] <= 0x7a)
            {
                //no match
                if(sstr->bytes[j] != thisstr->bytes[i+j] && sstr->bytes[j]-32 != thisstr->bytes[i+j])
                    break;
                //there is a match in the jth character so let's perform additional checks if needed
                if(RF_BITFLAG_ON(options,RF_MATCH_WORD))
                {
                    //if it's the first substring character and if the string we search is not in it's beginning, check for EXACT string before
                    if(j == 0 && i != 0)
                    {
                        switch(thisstr->bytes[i-1])
                        {
                            case ' ':case '\t':case '\n':
                            break;
                            default:
                                goto cleanup1;
                            break;
                        }
                    }
                }//exact string check if ends
            }
            //not a letter and no match
            else if(sstr->bytes[j] != thisstr->bytes[i+j])
                break;//break off the substring search loop

            //if we get here and it's the last char of the substring we either found it or need to perform one last check for exact string
            if(j == sstr->byteLength-1)
            {
                //only if the end of the string is not right after the substring
                if( RF_BITFLAG_ON(options,RF_MATCH_WORD) && i+sstr->byteLength < thisstr->byteLength)
                {
                    switch(thisstr->bytes[i+sstr->byteLength])
                    {
                        case ' ':case '\t':case '\n':
                        break;
                        default:
                            goto cleanup1;
                        break;
                    }
                }//end of the exact string check
                //succes
                ret = rfString_BytePosToCharPos(thisstr,i,false);
                goto cleanup1;
            }//end of it's the last char of the substring check
        }//substring iteration ends
    }//this string iteration ends

cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}


//Applies a limited version of sscanf after the specified substring
char rfString_ScanfAfter(const void* str,const void* afterstrP,const char* format,void* var)
{
    const RF_String* thisstr = (const RF_String*)str;
    const RF_String* afterstr = (const RF_String*)afterstrP;
    char* found,*s,ret=false;
    RF_ENTER_LOCAL_SCOPE()

    //return false if the substring is not found
    if( (found = strstr(thisstr->bytes,afterstr->bytes)) ==0 )
    {
        goto cleanup1;
    }
    //get a pointer to the start of the position where sscanf will be used
    s = thisstr->bytes + (found-thisstr->bytes+afterstr->byteLength);

    //use sscanf
    if(sscanf(s,format,var) <=0)
    {
        goto cleanup1;
    }
    //success
    ret = true;
cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

//Counts how many times a substring s occurs inside the string
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfString_Count(const void* str,const void* sstr2,const char options)
#else
int32_t i_rfString_Count(const void* str,const void* sstr2,const char options)
#endif
{
    RF_String* thisstr = (RF_String*)str;
    const RF_String* sstr = (const RF_String*)sstr2;
    int32_t index = 0;
    int32_t move;
    int32_t n = 0;

    RF_ENTER_LOCAL_SCOPE()

    //as long as the substring is found in the string
    while ((move = rfString_FindBytePos(thisstr,sstr,options)) != RF_FAILURE)
    {
        move+= sstr->byteLength;
        //proceed searching inside the string and also increase the counter
        n++;
        thisstr->bytes+=move;
        index +=move;
        thisstr->byteLength -=move;
    }

    //return string to its original state and return the number of occurences, also returns 0 if not found
    thisstr->bytes-=index;
    thisstr->byteLength += index;
    //success
    RF_EXIT_LOCAL_SCOPE()
    return n;
}


//Initializes the given string as the first substring existing between the left and right parameter substrings.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Between(const void* thisstrP,const void* lstrP,const void* rstrP,void* resultP,const char options)
#else
char i_rfString_Between(const void* thisstrP,const void* lstrP,const void* rstrP,void* resultP,const char options)
#endif
{
    int32_t start,end;
    const RF_String* thisstr = (const RF_String*)thisstrP;
    const RF_String* lstr = (const RF_String*)lstrP;
    const RF_String* rstr = (const RF_String*)rstrP;
    RF_String temp;
    char ret= false;
    RF_ENTER_LOCAL_SCOPE()

    //find the left substring
    if( (start = rfString_FindBytePos(thisstr,lstr,options))== RF_FAILURE)
    {
        goto cleanup1;
    }
    //get what is after it
    rfString_After(thisstr,lstr,&temp,options);
    //find the right substring in the remaining part
    if( (end = rfString_FindBytePos(&temp,rstr,options))== RF_FAILURE)
    {
        goto cleanup1;
    }
    //free temp string
    rfString_Deinit(&temp);
    //initialize the string to return
    if(options & RF_STRINGX_ARGUMENT)
    {
        RF_StringX* result = (RF_StringX*) resultP;
        result->INH_String.byteLength = end;
        result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        result->bIndex = 0;
        RF_MALLOC(result->INH_String.bytes,result->bSize)
        memcpy(result->INH_String.bytes,thisstr->bytes+start+lstr->byteLength,result->INH_String.byteLength+1);
        result->INH_String.bytes[end]= '\0';
    }
    else
    {
        RF_String* result = (RF_String*) resultP;
        result->byteLength = end;
        RF_MALLOC(result->bytes,result->byteLength+1);
        memcpy(result->bytes,thisstr->bytes+start+lstr->byteLength,result->byteLength+1);
        result->bytes[end]= '\0';
    }
    //success
    ret = true;
cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

//Initializes the given string as the substring from the start until any of the given Strings are found.
char rfString_Beforev(const void* thisstrP,void* resultP,const char options,const unsigned char parN, ...)
{
    const RF_String* thisstr = (const RF_String*)thisstrP;
    const RF_String* s;
    int32_t i,minPos,thisPos;

    //will keep the argument list
    va_list argList;
    //get the parameter characters
    va_start(argList,parN);
    RF_ENTER_LOCAL_SCOPE()

    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        s = (const RF_String*) va_arg(argList,RF_String*);
        if( (thisPos= rfString_FindBytePos(thisstr,s,options))!= RF_FAILURE)
        {
            if(thisPos < minPos)      
               minPos = thisPos;
        }
    }
    va_end(argList);

    //if it is not found
    if(minPos == INT_MAX)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //if it is found initialize the substring for return
    if(options & RF_STRINGX_ARGUMENT)
    {
        RF_StringX* result = (RF_StringX*) resultP;
        result->INH_String.byteLength = minPos;
        result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        result->bIndex = 0;
        RF_MALLOC(result->INH_String.bytes,result->bSize)
        memcpy(result->INH_String.bytes,thisstr->bytes,minPos);
        result->INH_String.bytes[minPos]= '\0';
    }
    else
    {
        RF_String* result = (RF_String*) resultP;
        result->byteLength = minPos;
        RF_MALLOC(result->bytes,minPos+1);
        memcpy(result->bytes,thisstr->bytes,minPos);
        result->bytes[minPos] = '\0';
    }
    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}

//Initializes the given string as the substring from the start until the given string is found
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Before(const void* thisstrP,const void* sstrP,void* resultP,const char options)
#else
char i_rfString_Before(const void* thisstrP,const void* sstrP,void* resultP,const char options)
#endif
{
    const RF_String* thisstr = (const RF_String*)thisstrP;
    const RF_String* sstr = (const RF_String*) sstrP;
    int32_t ret;
    RF_ENTER_LOCAL_SCOPE()

    //find the substring
    if( (ret = rfString_FindBytePos(thisstr,sstr,options)) == RF_FAILURE)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //if it is found initialize the substring for return
    if(options & RF_STRINGX_ARGUMENT)
    {
        RF_StringX* result = (RF_StringX*) resultP;
        result->INH_String.byteLength = ret;
        result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        result->bIndex = 0;
        RF_MALLOC(result->INH_String.bytes,result->bSize)
        memcpy(result->INH_String.bytes,thisstr->bytes,result->INH_String.byteLength);
        result->INH_String.bytes[result->INH_String.byteLength]= '\0';
    }
    else
    {
        RF_String* result = (RF_String*) resultP;
        result->byteLength = ret;
        RF_MALLOC(result->bytes,result->byteLength+1);
        memcpy(result->bytes,thisstr->bytes,result->byteLength);
        result->bytes[result->byteLength] = '\0';
    }
    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}


// Initializes the given String with the substring located after (and not including) the after substring inside the parameter string. If the substring is not located the function returns false.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_After(const void* thisstrP,const void* afterP,void* resultP,const char options)
#else
char i_rfString_After(const void* thisstrP,const void* afterP,void* resultP,const char options)
#endif
{
    const RF_String* thisstr = (const RF_String*)thisstrP;
    const RF_String* after = (const RF_String*)afterP;
    int32_t bytePos;
    RF_ENTER_LOCAL_SCOPE()
    //check for substring existence
    if( (bytePos = rfString_FindBytePos(thisstr,after,options)) == RF_FAILURE)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //done so let's get it. Notice the use of unsafe initialization
    if(options & RF_STRINGX_ARGUMENT)
    {
        RF_StringX* result = (RF_StringX*) resultP;
        rfStringX_Init_unsafe(result,thisstr->bytes+bytePos+after->byteLength);
    }
    else
    {
        RF_String* result = (RF_String*) resultP;
        rfString_Init_unsafe(result,thisstr->bytes+bytePos+after->byteLength);
    }
    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}


//Initialize a string after the first of the given substrings found
char rfString_Afterv(const void* thisstrP,void* resultP,const char options,const unsigned char parN,...)
{
    const RF_String* thisstr = (const RF_String*)thisstrP;
    const RF_String* s;
    int32_t i,minPos,thisPos;
    uint32_t minPosLength=0;
    //will keep the argument list
    va_list argList;
    RF_ENTER_LOCAL_SCOPE()
    //get the parameter characters
    va_start(argList,parN);

    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        s = (const RF_String*) va_arg(argList,RF_String*);
        if( (thisPos= rfString_FindBytePos(thisstr,s,options))!= RF_FAILURE)
        {
            if(thisPos < minPos)
            {
                minPos = thisPos;
                minPosLength = s->byteLength;
            }
        }
    }
    va_end(argList);
    //if it is not found
    if(minPos == INT_MAX)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //if it is found initialize the substring
    minPos += minPosLength;//go after the found substring
    if(options & RF_STRINGX_ARGUMENT)
    {
        RF_StringX* result = (RF_StringX*) resultP;
        result->INH_String.byteLength = thisstr->byteLength-minPos;
        result->bSize = result->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
        result->bIndex = 0;
        RF_MALLOC(result->INH_String.bytes,result->bSize)
        memcpy(result->INH_String.bytes,thisstr->bytes+minPos,result->INH_String.byteLength);
        result->INH_String.bytes[result->INH_String.byteLength]= '\0';
    }
    else
    {
        RF_String* result = (RF_String*) resultP;
        result->byteLength = thisstr->byteLength-minPos;
        RF_MALLOC(result->bytes,result->byteLength);
        memcpy(result->bytes,thisstr->bytes+minPos,result->byteLength);
        result->bytes[result->byteLength] = '\0';
    }
    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}
