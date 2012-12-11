//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/common.h>
//*---------------------Module related inclusion----------------------------------------
#include <String/stringx_decl.h> //for RF_StringX
#include <stdarg.h> //needed for the va_list argument in rfStringX_Formatv() and also in the functions below
#include <String/format.h> //for the String formatting function
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for UTF8 macro
#include <String/unicode.h> //for rfUTF8_VerifySequence()
//for the ioBuffer
    #include <Definitions/threadspecific.h> // for the thread specific keyword used in the ioBuffer
    #include "../IO/buff.ph" //for the ioBuffer StringX
//for local scope macros
    #include <string.h> //for size_t and memset() used in the Local scope macros
    #include <Utils/localmem_decl.h> //for local memory stack
    #include <Definitions/retcodes.h> //for return codes
    #include "../Utils/localmem.ph" //for the private local memory macros
//*---------------------libc Headers inclusion------------------------------------------
#include <stdlib.h> //for exit()
//*----------------------------End of Includes------------------------------------------

//Allocates and returns a string with the given characters a refu string with the given characters. Given characters have to be in UTF-8. A check for valid sequence of bytes is performed.
RF_String* i_rfString_CreateLocal1(const char* s,...)
{
    RF_String* ret;
    va_list args;
    //remember the stack pointer before this macro evaluation
    i_rfLMS_ArgsEval()

    //read the var args
    va_start(args,s);
    if(rfStringX_Formatv(&ioBuffer,s,args)!=RF_SUCCESS)
    {
        LOG_ERROR("Local string creation failure due to failing at reading the formatted string",RF_LastError);
        return 0;
    }
    va_end(args);

    //allocate the string in the local memory stack
    i_rfLMS_Push(ret,sizeof(RF_String));
    if(ret == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    //get length
    ret->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    i_rfLMS_Push(ret->bytes,ret->byteLength+1);
    if(ret->bytes == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    memcpy(ret->bytes,ioBuffer.INH_String.bytes,ret->byteLength+1);

    return ret;
}
RF_String* i_NVrfString_CreateLocal(const char* s)
{
    RF_String* ret;
    uint32_t byteLength;
    //remember the stack pointer before this macro evaluation
    i_rfLMS_ArgsEval()
    //check for validity of the given sequence and get the character length
    if( rfUTF8_VerifySequence(s,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at String Allocation due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }

    //allocate the string in the local memory stack
    i_rfLMS_Push(ret,sizeof(RF_String));
    if(ret == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed during string allocation. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    //get length
    ret->byteLength = byteLength;
    i_rfLMS_Push(ret->bytes,ret->byteLength+1);
    if(ret->bytes == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed during string allocation. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    memcpy(ret->bytes,s,ret->byteLength+1);

    return ret;
}

