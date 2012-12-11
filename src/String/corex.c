//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/corex.h>
//*---------------------Module related inclusion----------------------------------------
#include <String/traversalx.h> //for rfStringX_Reset()
#include <stdarg.h> //needed for the va_list argument in rfStringX_Formatv() and also in the functions below
#include <String/format.h> //for the String formatting function
#include "common.ph" //for RF_STRINGX_REALLOC()
//*---------------------Outside module inclusion----------------------------------------
//for endianess detection functionality
    #include <String/unicode.h> //for unicode functions such as rfUTF8_VerifySequence()
    #include <Utils/endianess.h> //for endianess detection
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>

#include <Definitions/retcodes.h> //for error codes
//for the ioBuffer
    #include <Definitions/threadspecific.h> // for the thread specific keyword used in the ioBuffer
    #include "../IO/buff.ph" //for the ioBuffer StringX

#include <Utils/constcmp.h> //for RF_HEXLE_UI() macro and others
//for the local scope macros
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>//for local scope macros
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Utils/memory.h> //for refu memory allocation
//*----------------------------End of Includes------------------------------------------

//Allocates and returns an extended String. Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* rfStringX_Create(const char* lit,...)
#else
RF_StringX* i_rfStringX_Create(const char* lit,...)
#endif
{
    RF_StringX* ret;
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()

    va_start(args,lit);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,lit,args)!=RF_SUCCESS)
    {
        LOG_ERROR("StringX creation failure due to failing at reading the formatted string",RF_LastError);
        return 0;
    }
    va_end(args);
    //initialize the string
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength =ioBuffer.INH_String.byteLength;
    //allocate and populate the data
    ret->bSize = ret->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,ioBuffer.INH_String.bytes);
    ret->bIndex = 0;

    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* i_NVrfStringX_Create(const char* lit)
{
    RF_StringX* ret;
    uint32_t byteLength;
    //check the string literal for valid utf-8 byte sequence
    if( rfUTF8_VerifySequence(lit,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }
    //initialize the string
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength = byteLength;
    //allocate and populate the data
    ret->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,lit);
    ret->bIndex = 0;

    return ret;
}
#endif

//Initializes an extended String. Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_Init(RF_StringX* str,const char* lit,...)
#else
char i_rfStringX_Init(RF_StringX* str,const char* lit,...)
#endif
{
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()

    va_start(args,lit);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,lit,args)!=RF_SUCCESS)
    {
        LOG_ERROR("StringX initialization failure due to failing at reading the formatted string",RF_LastError);
        return false;
    }
    va_end(args);
    //get bytelength
    str->INH_String.byteLength = ioBuffer.INH_String.byteLength;
    //allocate and populate the data
    str->bSize = str->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,ioBuffer.INH_String.bytes);
    str->bIndex = 0;

    //success
    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfStringX_Init(RF_StringX* str,const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(lit,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //get bytelength
    str->INH_String.byteLength = byteLength;
    //allocate and populate the data
    str->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,lit);
    str->bIndex = 0;
    //success
    return true;
}
#endif

//Turns a unicode code point in an extended  String and allocates and returns it (encoded in UTF-8)
RF_StringX* rfStringX_Create_cp(uint32_t codepoint)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //since this is an extended string let's be a bit more generous with the buffer
    ret->bSize = 5*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;

    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        ret->INH_String.bytes[0] = codepoint;
        ret->INH_String.bytes[1] = '\0';
        ret->INH_String.byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        if(rfEndianess() == RF_LITTLE_ENDIAN)
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[1] = (codepoint & 0x3F)|(0x02<<6);
            //get the 5 following bits and encode them in the second byte
            ret->INH_String.bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        }
        else
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[0] = (codepoint & 0x3F)|(0x02<<6);
            //get the 5 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        }
        ret->INH_String.bytes[2] = '\0';
        ret->INH_String.byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        if(rfEndianess() == RF_LITTLE_ENDIAN)
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[2] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 4 following bits and encode them in the third byte
            ret->INH_String.bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        }
        else
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[0] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 4 following bits and encode them in the third byte
            ret->INH_String.bytes[2] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        }
        ret->INH_String.bytes[3] = '\0';
        ret->INH_String.byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        if(rfEndianess() == RF_LITTLE_ENDIAN)
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[3] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 6 following bits and encode them in the third byte
            ret->INH_String.bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
            //get the 3 following bits and encode them in the fourth byte
            ret->INH_String.bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        }
        else
        {
            //get the first bits of the first byte and encode them to the first byte
            ret->INH_String.bytes[0] = (codepoint & 0x3F)|(0x02<<6);
            //get the 6 following bits and encode them in the second byte
            ret->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
            //get the 6 following bits and encode them in the third byte
            ret->INH_String.bytes[2] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
            //get the 3 following bits and encode them in the fourth byte
            ret->INH_String.bytes[3] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        }
        ret->INH_String.bytes[4] = '\0';
        ret->INH_String.byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into an extended string",RE_UTF8_INVALID_CODE_POINT);
        rfStringX_Destroy(ret);
        return 0;
    }

    return ret;
}
//Turns a unicode code point in an extended  String and initializes it (encoded in UTF-8)
char rfStringX_Init_cp(RF_StringX* str,uint32_t codepoint)
{

    //since this is an extended string let's be a bit more generous with the buffer
    str->bSize = 5*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;

    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        str->INH_String.bytes[0] = codepoint;
        str->INH_String.bytes[1] = '\0';
        str->INH_String.byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->INH_String.bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->INH_String.bytes[2] = '\0';
        str->INH_String.byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->INH_String.bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->INH_String.bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->INH_String.bytes[3] = '\0';
        str->INH_String.byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->INH_String.bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->INH_String.bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->INH_String.bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->INH_String.bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->INH_String.bytes[4] = '\0';
        str->INH_String.byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into an extended string",RE_UTF8_INVALID_CODE_POINT);
        free(str->INH_String.bytes);
        return false;
    }

    return true;
}

//Allocates and returns a string with the given integer
RF_StringX* rfStringX_Create_i(int32_t i)
{
    //the size of the int32_t buffer
    int32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%d",i);
    numLen = strlen(buff)+1;

    //initialize the string and return it
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    ret->bSize = numLen*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(ret->INH_String.bytes,sizeof(char)*ret->bSize);
    ret->INH_String.byteLength = numLen;
    strcpy(ret->INH_String.bytes,buff);
    return ret;
}
//Initializes a string with the given integer
char rfStringX_Init_i(RF_StringX* str,int32_t i)
{
    //the size of the int32_t buffer
    int32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%d",i);
    numLen = strlen(buff)+1;

    str->bSize = numLen*RF_OPTION_STRINGX_CAPACITY_M;
    RF_MALLOC(str->INH_String.bytes,sizeof(char)*str->bSize);
    str->INH_String.byteLength = numLen;
    strcpy(str->INH_String.bytes,buff);
    return true;
}
// Allocates and returns an extended string with the given float.
RF_StringX* rfStringX_Create_f(float f)
{
    //allocate a buffer for the float in characters
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);
    uint32_t len = strlen(buff);

    //initialize and return the string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    ret->INH_String.byteLength = len;
    ret->bSize = len*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;
    strcpy(ret->INH_String.bytes,buff);

    free(buff);

    return ret;
}
// Initializes an extended string with the given float.
char rfStringX_Init_f(RF_StringX* str,float f)
{
    //allocate a buffer for the float in characters
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);
    uint32_t len = strlen(buff);

    //initialize and return the string
    str->INH_String.byteLength = len;
    str->bSize = len*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;
    strcpy(str->INH_String.bytes,buff);

    free(buff);

    return true;
}


// Allocates and returns an extended String allocating buffSize bytes buffer and puts str inside the buffer
// If the contents of the string are bigger than the buffer Size then an error is logged but the String is initialized correctly
// Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* rfStringX_Create_buff(uint32_t buffSize,const char* lit,...)
#else
RF_StringX* i_rfStringX_Create_buff(uint32_t buffSize,const char* lit,...)
#endif
{
    RF_StringX* ret;
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()

    va_start(args,lit);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,lit,args)!=RF_SUCCESS)
    {
        LOG_ERROR("StringX creation failure due to failing at reading the formatted string",RF_LastError);
        return 0;
    }
    va_end(args);
    //initialize the string
    RF_MALLOC(ret,sizeof(RF_StringX));
    ret->bSize = buffSize;

    //make sure that the buff size fits the string
    if(buffSize < ioBuffer.INH_String.byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);

        //set the buffer size to double the given string's length
        ret->bSize = ioBuffer.INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }

    //allocate and populate the data
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;
    ret->INH_String.byteLength = ioBuffer.INH_String.byteLength;
    strcpy(ret->INH_String.bytes,ioBuffer.INH_String.bytes);

    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_StringX* i_NVrfStringX_Create_buff(uint32_t buffSize,const char* lit)
{
    RF_StringX* ret;
    uint32_t byteLength;

    //check the string literal for valid utf-8 byte sequence
    if( rfUTF8_VerifySequence(lit,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }
    //initialize the string
    RF_MALLOC(ret,sizeof(RF_StringX));
    ret->bSize = buffSize;
    //make sure that the buff size fits the string
    if(buffSize < byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);
        //set the buffer size to double the given string's length
        ret->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }
    //allocate and populate the data
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    ret->bIndex = 0;
    ret->INH_String.byteLength = byteLength;
    strcpy(ret->INH_String.bytes,lit);
    return ret;
}
#endif
// Initializes an extended String allocating buffSize bytes buffer and puts str inside the buffer
// If the contents of the string are bigger than the buffer Size then an error is logged but the String is initialized correctly
// Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit,...)
#else
char i_rfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit,...)
#endif
{
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()

    va_start(args,lit);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,lit,args)!=RF_SUCCESS)
    {
        LOG_ERROR("StringX initialization failure due to failing at reading the formatted string",RF_LastError);
        return false;
    }
    va_end(args);
    //get the buffer size
    str->bSize = buffSize;
    //make sure that the buff size fits the string
    if(buffSize < ioBuffer.INH_String.byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);
        //set the buffer size to double the given string's length
        str->bSize = ioBuffer.INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }
    //allocate and populate the data
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;
    str->INH_String.byteLength = ioBuffer.INH_String.byteLength;
    strcpy(str->INH_String.bytes,ioBuffer.INH_String.bytes);

    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(lit,&byteLength) == RF_FAILURE)
    {
        LOG_ERROR("Error at StringX initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //get the buffer size
    str->bSize = buffSize;
    //make sure that the buff size fits the string
    if(buffSize < byteLength+1)
    {
        LOG_ERROR("At initialization of %s as Extended String the given buffer size does not fit the original String. The buffer size is automatically corrected to fit the string",
                   RE_STRING_INIT_FAILURE,lit);

        //set the buffer size to double the given string's length
        str->bSize = byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    }
    //allocate and populate the data
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    str->bIndex = 0;
    str->INH_String.byteLength = byteLength;
    strcpy(str->INH_String.bytes,lit);

    return true;
}
#endif


//Allocates and returns a string with the given UTF-16 byte sequence. Given characters have to be in UTF-16. A check for valid sequence of bytes is performed
RF_StringX* rfStringX_Create_UTF16(const uint16_t* s)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_UTF16(ret,s)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes a string with the given UTF-16 byte sequence. Given characters have to be in UTF-16. A check for valid sequence of bytes is performed
char rfStringX_Init_UTF16(RF_StringX* str,const uint16_t* s)
{
    //decode the utf-16 and get the code points
    uint32_t* codepoints;
    uint32_t byteLength,characterLength,utf8ByteLength;
    char* utf8;
    byteLength = 0;
    while(s[byteLength]!= 0 || s[byteLength+1]!=0)
    {
        byteLength++;
    }
    byteLength+=3;//for the last utf-16 null termination character
    RF_MALLOC(codepoints,byteLength*2) //allocate the codepoints
    //decode the byte stream into codepoints
    if(rfUTF16_Decode((char*)s,&characterLength,codepoints) != RF_SUCCESS)
    {
        free(codepoints);
        LOG_ERROR("StringX initialization failed due to invalide UTF-16 sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //now encode these codepoints into UTF8
    if( (utf8 = rfUTF8_Encode(codepoints,characterLength,&utf8ByteLength))==0)
    {
        free(codepoints);
        return false;
    }
    //success
    free(codepoints);
    str->INH_String.bytes = utf8;
    str->bIndex = 0;
    str->bSize = utf8ByteLength+1;
    str->INH_String.byteLength = utf8ByteLength;
    return true;


}

//Allocates and returns a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32
RF_StringX* rfStringX_Create_UTF32(const uint32_t* s)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_UTF32(ret,s)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32
char rfStringX_Init_UTF32(RF_StringX* str,const uint32_t* codeBuffer)
{
    //find the length of the utf32 buffer in characters
    uint32_t length;
    rfUTF32_Length(codeBuffer,length);
    //turn the codepoints into a utf-8 encoded buffer
    char* utf8;uint32_t utf8ByteLength;
    if((utf8=rfUTF8_Encode(codeBuffer,length,&utf8ByteLength)) == 0)
    {
        LOG_ERROR("Could not encode UTF32 codepoints into a UTF7 bytestream",RE_UTF8_ENCODING)
        return false;//error
    }

    str->INH_String.bytes = (char*)utf8;
    str->bIndex = 0;
    str->bSize = utf8ByteLength+1;
    str->INH_String.byteLength = utf8ByteLength;
    return true;
}

// Allocates and returns an extended String. NO VALID-UTF8 check is performed
RF_StringX* rfStringX_Create_unsafe(const char* lit)
{
    //initialize the string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //get bytelength
    ret->INH_String.byteLength = strlen(lit);
    //allocate and populate the data
    ret->bSize = ret->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    strcpy(ret->INH_String.bytes,lit);
    ret->bIndex = 0;

    return ret;
}

// Initializes an extended String. NO VALID-UTF8 check is performed
void rfStringX_Init_unsafe(RF_StringX* str,const char* lit)
{
    //get bytelength
    str->INH_String.byteLength = strlen(lit);
    //allocate and populate the data
    str->bSize = str->INH_String.byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(str->INH_String.bytes,str->bSize);
    strcpy(str->INH_String.bytes,lit);
    str->bIndex = 0;
}


/*-------------------------------------------------------------------------Methods to copy/assign an RF_StringX-------------------------------------------------------------------------------*/

// Assigns the value of the source string to the destination extended string.Both strings should already be initialized and hold a value. It is an error to give null parameters.
void rfStringX_Assign(RF_StringX* dst,const void* sourceP)
{
    const RF_String* source = (const RF_String*)sourceP;
    RF_ENTER_LOCAL_SCOPE()
    rfStringX_Reset(dst);//make sure that the destination's byte index is reset
    //only if the new string value won't fit in the buffer reallocate the buffer
    RF_STRINGX_REALLOC(dst,source->byteLength+1)
    //now copy the value and the bytelength
    memcpy(dst->INH_String.bytes,source->bytes,source->byteLength+1);
    dst->INH_String.byteLength = source->byteLength;
    RF_EXIT_LOCAL_SCOPE()
}

//Assigns the value of a unicode character to the string
char rfStringX_Assign_char(RF_StringX* str,uint32_t codepoint)
{
    char utf8[5];
    int bytes;
    //reset the buffer if needed
    rfStringX_Reset(str);
    //realloc if needed
    if(str->bSize <5)
    {
        RF_REALLOC(str->INH_String.bytes,char,5*RF_OPTION_STRINGX_CAPACITY_M+1);
    }
    if((bytes=rfUTF8_Encode_single(codepoint,utf8))==0)
    {
        LOG_ERROR("Assigning a character to an RF_StringX failed",RF_LastError);
        return false;
    }
    str->INH_String.byteLength = bytes;
    strcpy(str->INH_String.bytes,utf8);
    return true;
}


// Returns an RF_StringX version of the parameter RF_String
RF_StringX* rfStringX_FromString_OUT(const RF_String* s)
{
    //create the new string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //copy the bytes
    ret->bSize = s->byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(ret->INH_String.bytes,ret->bSize);
    memcpy(ret->INH_String.bytes,s->bytes,s->byteLength+1);
    ret->bIndex = 0;
    ret->INH_String.byteLength = s->byteLength;
    return ret;
}
//Initializes an RF_StringX from an RF_string
void rfStringX_FromString_IN(RF_StringX* dst,const RF_String* src)
{
    //allocate the stringX
    dst->bIndex = 0;
    dst->bSize = src->byteLength*RF_OPTION_STRINGX_CAPACITY_M+1;
    RF_MALLOC(dst->INH_String.bytes,dst->bSize)
    dst->INH_String.byteLength = src->byteLength;
    //copy the bytes from the String
    memcpy(dst->INH_String.bytes,src->bytes,src->byteLength+1);
}


//Creates a copy of an extended String and returns it
RF_StringX* rfStringX_Copy_OUT(RF_StringX* s)
{
    //create the new string
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    //allocate the new string
    RF_MALLOC(ret->INH_String.bytes,s->bSize);
    //before copying make sure that the source string is copied from the beginning of its buffer
    s->INH_String.bytes -= s->bIndex;
    memcpy(ret->INH_String.bytes,s->INH_String.bytes,s->bSize);
    //put the s->bytes back and put the copied bytes buffer to the same inex
    s->INH_String.bytes += s->bIndex;
    ret->INH_String.bytes += s->bIndex;
    //copy the parameters
    ret->bSize = s->bSize;
    ret->bIndex = s->bIndex;
    ret->INH_String.byteLength = s->INH_String.byteLength;
    return ret;
}
//Copies the given source string into the destination string.
void rfStringX_Copy_IN(RF_StringX* dst,RF_StringX* src)
{
    //before copying make sure that the source string is copied from the beginning of its buffer
    src->INH_String.bytes -= src->bIndex;
    //copy the bytes
    RF_MALLOC(dst->INH_String.bytes,src->bSize);
    memcpy(dst->INH_String.bytes,src->INH_String.bytes,src->bSize);
    //pur the source bytes back and also the new copied bytes buffer to the same index
    src->INH_String.bytes += src->bIndex;
    dst->INH_String.bytes += src->bIndex;
    //copy the parameters
    dst->bSize = src->bSize;
    dst->bIndex = src->bIndex;
    dst->INH_String.byteLength = src->INH_String.byteLength;

}
// Copies a certain number of characters from an RF_StringX
void rfStringX_Copy_chars(RF_StringX* dst,RF_StringX* src,uint32_t charsN)
{
    uint32_t i=0,bytePos;
    //before copying make sure that the source string is copied from the beginning of its buffer
    src->INH_String.bytes -= src->bIndex;
    //copy the bytes
    RF_MALLOC(dst->INH_String.bytes,src->bSize);
    memcpy(dst->INH_String.bytes,src->INH_String.bytes,src->bSize);
    //pur the source bytes back and also the new copied bytes buffer to the same index
    src->INH_String.bytes += src->bIndex;
    dst->INH_String.bytes += src->bIndex;
    //copy the parameters
    dst->bSize = src->bSize;
    dst->bIndex = src->bIndex;
    dst->INH_String.byteLength = src->INH_String.byteLength;
    //find the byte position and end the string there
    RF_STRING_ITERATE_START(src,i,bytePos)
    if(i == charsN)
        break;
    RF_STRING_ITERATE_END(i,bytePos)
    dst->INH_String.byteLength = bytePos;
    dst->INH_String.bytes[bytePos] = '\0';
}

/*-------------------------------------------------------------------------Methods to get rid of an RF_StringX-------------------------------------------------------------------------------*/

//Destroys an extended String and releases its memory.It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
void rfStringX_Destroy(RF_StringX* s)
{
    //an extended string can have moved its internal pointer forward, so we have to put it back at the origin to destroy the whole string freeing all the memory properly
    s->INH_String.bytes -= s->bIndex;
    free(s->INH_String.bytes);
    free(s);
}
//Destroys only the contents of the Extended string without freeing the pointer itself.It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
void rfStringX_Deinit(RF_StringX* s)
{
    //an extended string can have moved its internal pointer forward, so we have to put it back at the origin to destroy the whole string freeing all the memory properly
    s->INH_String.bytes -= s->bIndex;
    free(s->INH_String.bytes);
}
