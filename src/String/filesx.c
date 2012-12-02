#include <String/filesx.h>

#include <String/common.h> // for RFS_()
#include <String/manipulationx.h> //for rfStringX_Append and others
#include <String/traversalx.h> //for rfStringX_Reset and others

#include <rf_error.h>
#include <rf_memory.h>

#include <IO/rf_unicode.h> //for the rfReadLine family of functions
#include <rf_localmem.h> //for the local memory scope macros
#include <rf_io.h> //for the rfReadLine family of functions

#include <string.h> //for some stdlib c string functions and memcpy()
/*------------------------------------------------------------------------- File I/O RF_StringX Functions -------------------------------------------------------------------------------*/

//Allocates and returns an extended string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
RF_StringX* rfStringX_Create_fUTF8(FILE* f, char* eof,char eol)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_fUTF8(ret,f,eof,eol) < 0)
    {
        free(ret);
        ret=0;
    }
    return ret;
}
//Initializes an extended string from file parsing. The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
int32_t rfStringX_Init_fUTF8(RF_StringX* str,FILE* f,char* eof,char eol)
{
    int32_t bytesN;
    if((bytesN=rfFReadLine_UTF8(f,eol,&str->INH_String.bytes,&str->INH_String.byteLength,&str->bSize,eof)) < 0)
        RETURN_LOG_ERROR("Failed to initialize StringX from a UTF-8 file",bytesN)

    //success
    str->bIndex = 0;
    return bytesN;
}
//Assigns to an extended String from UTF-8 file parsing
int32_t rfStringX_Assign_fUTF8(RF_StringX* str,FILE*f,char* eof,char eol)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN=rfFReadLine_UTF8(f,eol,&utf8,&utf8ByteLength,&utf8BufferSize,eof)) < 0)
        RETURN_LOG_ERROR("Failed to assign the contents of a UTF-8 file to a StringX",bytesN)

    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC(str->INH_String.bytes,char,str->bSize);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
//Appends to an extended String from UTF-8 file parsing
int32_t rfStringX_Append_fUTF8(RF_StringX* str,FILE*f,char* eof,char eol)
{
    int32_t bytesN;
    uint32_t utf8ByteLength,utf8BufferSize;//bufferSize unused in this function
    char* utf8 = 0;
    if((bytesN=rfFReadLine_UTF8(f,eol,&utf8,&utf8ByteLength,&utf8BufferSize,eof)) < 0)
        RETURN_LOG_ERROR("Failed to assign the contents of a UTF-8 file to a StringX",bytesN)

    //append the utf8 to the given string
    rfStringX_Append(str,RFS_(utf8));
    //free the file's utf8 buffer
    free(utf8);
    return bytesN;
}

//Allocates and returns an extended string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_StringX* rfStringX_Create_fUTF16(FILE* f,char endianess,char* eof,char eol)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_fUTF16(ret,f,endianess,eof,eol) < 0)
    {
        free(ret);
        ret=0;
    }
    return ret;
}
//Initializes an extended string from file parsing. The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfStringX_Init_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof,char eol)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,eol,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to initialize a StringX from reading a UTF-16 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,eol,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to initialize a StringX from reading a UTF-16 file",bytesN)

    }//end of big endian case
    //success
    str->bIndex = 0;
    str->bSize = 0;
    return bytesN;
}

//Assigns to an already initialized String from File parsing
int32_t rfStringX_Assign_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof,char eol)
{
    uint32_t utf8ByteLength;
    int32_t bytesN;
    char* utf8 = 0;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign the contents of a Little Endian UTF-16 file to a StringX",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            LOG_ERROR("Failure to assign the contents of a Big Endian UTF-16 file to a StringX",bytesN)

    }//end of big endian case
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC(str->INH_String.bytes,char,str->bSize);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}

//Appends to an already initialized String from File parsing
int32_t rfStringX_Append_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof,char eol)
{
    char*utf8;
    uint32_t utf8ByteLength;
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF16LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append the contents of a Little Endian UTF-16 file to a StringX",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF16BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            LOG_ERROR("Failure to append the contents of a Big Endian UTF-16 file to a StringX",bytesN)

    }//end of big endian case
    //success
    rfStringX_Append(str,RFS_(utf8));
    free(utf8);
    return bytesN;
}

// Allocates and returns an extended string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
RF_StringX* rfStringX_Create_fUTF32(FILE* f,char endianess,char* eof,char eol)
{
    RF_StringX* ret;
    RF_MALLOC(ret,sizeof(RF_StringX));
    if(rfStringX_Init_fUTF32(ret,f,endianess,eof,eol) < 0)
    {
        free(ret);
        ret=0;
    }
    return ret;
}
// Initializes an extended string from file parsing. The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
int32_t rfStringX_Init_fUTF32(RF_StringX* str,FILE* f,char endianess,char* eof,char eol)
{
    int32_t bytesN;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,eol,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) <0)
            RETURN_LOG_ERROR("Failure to initialize a StringX from reading a Little Endian UTF-32 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF32BE(f,eol,&str->INH_String.bytes,&str->INH_String.byteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to initialize a StringX from reading a Big Endian UTF-32 file",bytesN)

    }//end of big endian case
    //success
    str->bIndex = 0;
    str->bSize = 0;
    return bytesN;
}
//Assigns the contents of a UTF-32 file to an extended string
int32_t rfStringX_Assign_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof,char eol)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign to a StringX from reading a Little Endian UTF-32 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF32BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to assign to a StringX from reading a Big Endian UTF-32 file",bytesN)

    }//end of big endian case
    //success
    //assign it to the string
    rfStringX_Reset(str);
    if(str->bSize <= utf8ByteLength+1)
    {
        str->bSize = (utf8ByteLength+1)*2;
        RF_REALLOC(str->INH_String.bytes,char,str->bSize);
    }
    memcpy(str->INH_String.bytes,utf8,utf8ByteLength+1);
    str->INH_String.byteLength = utf8ByteLength;
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
//Appends the contents of a UTF-32 file to an extended string
int32_t rfStringX_Append_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof,char eol)
{
    int32_t bytesN;
    char*utf8;
    uint32_t utf8ByteLength;
    //depending on the file's endianess
    if(endianess == RF_LITTLE_ENDIAN)
    {
        if((bytesN=rfFReadLine_UTF32LE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append to a StringX from reading a Little Endian UTF-32 file",bytesN)

    }//end of little endian
    else//big endian
    {
        if((bytesN=rfFReadLine_UTF32BE(f,eol,&utf8,&utf8ByteLength,eof)) < 0)
            RETURN_LOG_ERROR("Failure to append to a StringX from reading a Big Endian UTF-32 file",bytesN)

    }//end of big endian case
    //success
    //append it
    rfStringX_Append(str,RFS_(utf8));
    //free the file'sutf8 buffer
    free(utf8);
    return bytesN;
}
