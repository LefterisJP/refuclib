//*---------------------Corrensponding Header inclusion---------------------------------
#include <stdio.h> //for FILE*
#include <IO/common.h> //for RF_EOL macros
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/unicode.h> //for the unicode macros RF_UTF8 and friends
#include <String/string_decl.h>//for RF_String
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/files.h>
//*---------------------Module related inclusion----------------------------------------
#include <String/conversion.h> //for unicode conversions
#include <String/manipulation.h> //for rfString_Append()
#include <String/common.h> //for RFS_()
//*---------------------Outside module inclusion----------------------------------------
//for error logging macros
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation

#include <Utils/endianess.h> //for Endianess swapping functions
#include <IO/file.h> //for rfReadLine family  of functions
#include "../IO/io.ph" //for IO_WRITE_CHECK() macro
//for local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>//for local scope macros
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h> //needed for the same macro as above
//*----------------------------End of Includes------------------------------------------


//Allocates and returns a string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
RF_String* rfString_Create_fUTF8(FILE* f, char* eof,char eol)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    if(rfString_Init_fUTF8(ret,f,eof,eol) < 0)
    {
        free(ret);
        ret=0;
    }
    return ret;
}
//Initializes a string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
int32_t rfString_Init_fUTF8(RF_String* str,FILE* f,char* eof,char eol)
{
    int32_t bytesN;
    uint32_t bufferSize;//unused
    if((bytesN=rfFReadLine_UTF8(f,eol,&str->bytes,&str->byteLength,&bufferSize,eof)) < 0)
        RETURN_LOG_ERROR("Failed to initialize String from a UTF-8 file",bytesN)

    //success
    return bytesN;
}
//Assigns to a String from UTF-8 file parsing
int32_t rfString_Assign_fUTF8(RF_String* str,FILE* f,char* eof,char eol)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN=rfFReadLine_UTF8(f,eol,&utf8,&utf8ByteLength,&utf8BufferSize,eof)) < 0)
        RETURN_LOG_ERROR("Failed to assign the contents of a UTF-8 file to a String",bytesN)

    //success
    //assign it to the string
    if(str->byteLength <= utf8ByteLength)
    {
        RF_REALLOC(str->bytes,char,utf8ByteLength+1);
    }
    memcpy(str->bytes,utf8,utf8ByteLength+1);
    str->byteLength = utf8ByteLength;
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}
//Appends to a String from UTF-8 file parsing
int32_t rfString_Append_fUTF8(RF_String* str,FILE*f,char* eof,char eol)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN=rfFReadLine_UTF8(f,eol,&utf8,&utf8ByteLength,&utf8BufferSize,eof)) < 0)
        RETURN_LOG_ERROR("Failed to assign the contents of a UTF-8 file to a String",bytesN)

    //append the utf8 to the given string
    rfString_Append(str,RFS_(utf8));
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}

//Allocates and returns a string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_String* rfString_Create_fUTF16(FILE* f,char endianess,char* eof,char eol)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    if(rfString_Init_fUTF16(ret,f,endianess,eof,eol) < 0)
    {
        free(ret);
        ret=0;
    }
    return ret;
}
//Initializes a string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfString_Init_fUTF16(RF_String* str,FILE* f, char endianess,char* eof,char eol)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,eol,&str->bytes,&str->byteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to initialize a String from reading a UTF-16 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,eol,&str->bytes,&str->byteLength,eof)) < 0)
            LOG_ERROR("Failure to initialize a String from reading a UTF-16 file",bytesN)

    }//end of big endian case
    //success
    return bytesN;
}

//Assigns to an already initialized String from File parsing
int32_t rfString_Assign_fUTF16(RF_String* str,FILE* f, char endianess,char* eof,char eol)
{

    uint32_t utf8ByteLength;
    int32_t bytesN;
    char* utf8 = 0;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign the contents of a Little Endian UTF-16 file to a String",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign the contents of a Big Endian UTF-16 file to a String",bytesN)

    }//end of big endian case
    //success
    //assign it to the string
    if(str->byteLength <= utf8ByteLength)
    {
        RF_REALLOC(str->bytes,char,utf8ByteLength+1);
    }
    memcpy(str->bytes,utf8,utf8ByteLength+1);
    str->byteLength = utf8ByteLength;
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}

//Appends to an already initialized String from File parsing
int32_t rfString_Append_fUTF16(RF_String* str,FILE* f, char endianess,char* eof,char eol)
{
    char*utf8;
    uint32_t utf8ByteLength;
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append the contents of a Little Endian UTF-16 file to a String",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append the contents of a Big Endian UTF-16 file to a String",bytesN)

    }//end of big endian case
    //success
    rfString_Append(str,RFS_(utf8));
    free(utf8);
    return bytesN;
}

// Allocates and returns a string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_String* rfString_Create_fUTF32(FILE* f,char endianess,char* eof,char eol)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    if(rfString_Init_fUTF32(ret,f,endianess,eof,eol) < 0)
    {
        free(ret);
        ret=0;
    }
    return ret;
}
// Initializes a string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfString_Init_fUTF32(RF_String* str,FILE* f,char endianess,char* eof,char eol)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,eol,&str->bytes,&str->byteLength,eof)) <0)
            RETURN_LOG_ERROR("Failure to initialize a String from reading a Little Endian UTF-32 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF32BE(f,eol,&str->bytes,&str->byteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to initialize a String from reading a Big Endian UTF-32 file",bytesN)

    }//end of big endian case
    //success
    return bytesN;
}
//Assigns the contents of a UTF-32 file to a string
int32_t rfString_Assign_fUTF32(RF_String* str,FILE* f,char endianess, char* eof,char eol)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign to a String from reading a Little Endian UTF-32 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF32BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign to a String from reading a Big Endian UTF-32 file",bytesN)

    }//end of big endian case
    //success
    //assign it to the string
    if(str->byteLength <= utf8ByteLength)
    {
        RF_REALLOC(str->bytes,char,utf8ByteLength+1);
    }
    memcpy(str->bytes,utf8,utf8ByteLength+1);
    str->byteLength = utf8ByteLength;
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}
//Appends the contents of a UTF-32 file to a string
int32_t rfString_Append_fUTF32(RF_String* str,FILE* f,char endianess, char* eof,char eol)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append to a String from reading a Little Endian UTF-32 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF32BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append to a String from reading a Big Endian UTF-32 file",bytesN)

    }//end of big endian case
    //success
    //append it
    rfString_Append(str,RFS_(utf8));
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}

//Writes a string to a file in the given encoding
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfString_Fwrite(void* sP,FILE* f,char encoding)
#else
int32_t i_rfString_Fwrite(void* sP,FILE* f,char encoding)
#endif
{
    uint32_t *utf32,length,i;
    uint16_t* utf16;
    int32_t ret = RF_SUCCESS;
    RF_String* s = (RF_String*)sP;
    RF_ENTER_LOCAL_SCOPE()
    //depending on the encoding
    switch(encoding)
    {
        case RF_UTF8:
            if(fwrite(s->bytes,1,s->byteLength,f) != s->byteLength)
                break;//and go to error logging
            goto cleanup1;//success
        break;
        case RF_UTF16_LE:
            utf16 = rfString_ToUTF16(s,&length);
            if(rfEndianess() != RF_LITTLE_ENDIAN)
            {
                for(i=0;i<length;i++)
                {
                    rfSwapEndianUS(&utf16[i]);
                }
            }
            if(fwrite(utf16,2,length,f) != length)
            {
                free(utf16);
                break;//and go to error logging
            }
            free(utf16);
            goto cleanup1;//success
        break;
        case RF_UTF16_BE:
            utf16 = rfString_ToUTF16(s,&length);
            if(rfEndianess() != RF_BIG_ENDIAN)
            {
                for(i=0;i<length;i++)
                {
                    rfSwapEndianUS(&utf16[i]);
                }
            }
            if(fwrite(utf16,2,length,f) != length)
            {
                free(utf16);
                break;//and go to error logging
            }
            free(utf16);
            goto cleanup1;//success
        break;
        case RF_UTF32_LE:
            utf32 = rfString_ToUTF32(s,&length);
            if(rfEndianess() != RF_LITTLE_ENDIAN)
            {
                for(i=0;i<length;i++)
                {
                    rfSwapEndianUI(&utf32[i]);
                }
            }
            if(fwrite(utf32,4,length,f) != length)
            {
                free(utf32);
                break;//and go to error logging
            }
            free(utf32);
            goto cleanup1;//success
        break;
        case RF_UTF32_BE:
            utf32 = rfString_ToUTF32(s,&length);
            if(rfEndianess() != RF_BIG_ENDIAN)
            {
                for(i=0;i<length;i++)
                {
                    rfSwapEndianUI(&utf32[i]);
                }
            }
            if(fwrite(utf32,4,length,f) != length)
            {
                free(utf32);
                break;//and go to error logging
            }
            free(utf32);
            goto cleanup1;//success
        break;
    }
    //if we get here it means an error, and we log it with the macro
    i_WRITE_CHECK(f,"Writting a string to a file")
    ret = RE_FILE_WRITE;

cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
