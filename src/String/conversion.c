#include <String/conversion.h>

#include <IO/rf_unicode.h>
#include <rf_localmem.h> //for the local scope macros

#include <String/core.h>//for rfString_Equal
#include <String/retrieval.h> //for rfString_Count
#include <String/common.h> //for RFS_()
#include "common.ph" //for required string private macros and functions

#include <rf_memory.h>

#include <string.h> //for some c stdlib string functions
#include <stdlib.h> //for some conversion functions like strtod()
#include <errno.h> //for errno
#include <math.h> //for HUGE_VAL

//Returns the strings contents as a UTF-16 buffer
uint16_t* rfString_ToUTF16(const void* sP,uint32_t* length)
{
    uint32_t* codepoints,charsN;
    const RF_String* s = (const RF_String*) sP;
    //get the unicode codepoints, no check here since RF_String is always guaranteed to have valid UTF-8 and as such valid codepoints
    codepoints = rfUTF8_Decode(s->bytes,s->byteLength,&charsN);
    //encode them in UTF-16, no check here since it comes from an RF_String which is always guaranteed to have valid UTF-8 and as such valid codepoints
    return rfUTF16_Encode(codepoints,charsN,length);
}

//Returns the strings contents as a UTF-32 buffer
uint32_t* rfString_ToUTF32(const void* sP,uint32_t* length)
{
    const RF_String* s = (const RF_String*) sP;
    //get the unicode codepoints, no check here since RF_String is always guaranteed to have valid UTF-8 and as such valid codepoints
    return rfUTF8_Decode(s->bytes,s->byteLength,length);
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
    RF_ENTER_LOCAL_SCOPE()
    //first find the occurences of the separator, and then the number of tokens
    *tokensN = rfString_Count(thisstr,sep,0)+1;
    //error checking
    if(*tokensN == 1)
    {
        RF_EXIT_LOCAL_SCOPE()
        return false;
    }
    //allocate the tokens
    RF_MALLOC(*tokens,sizeof(RF_String) *(*tokensN));
    //find the length of the separator
    uint32_t sepLen = sep->byteLength;
    char* s,*e;
    s = thisstr->bytes;
    for(i = 0; i < (*tokensN)-1; i ++)
    {
        //find each substring
        e = strstr(s,sep->bytes);
        (*tokens)[i].byteLength = e-s;
        RF_MALLOC((*tokens)[i].bytes,(*tokens)[i].byteLength+1);
        //put in the data
        strncpy((*tokens)[i].bytes,s,(*tokens)[i].byteLength);
        //null terminate
        (*tokens)[i].bytes[(*tokens)[i].byteLength] = '\0';

        //prepare for next sub-string
        s = e+sepLen;

    }
    ///make sure that if it's the last substring we change strategy
    (*tokens)[i].byteLength = strlen(s);
    RF_MALLOC((*tokens)[i].bytes,(*tokens)[i].byteLength+1);
    //put in the data
    strncpy((*tokens)[i].bytes,s,(*tokens)[i].byteLength);
    //null terminate
    (*tokens)[i].bytes[(*tokens)[i].byteLength] = '\0';

    //success
    RF_EXIT_LOCAL_SCOPE()
    return true;
}
