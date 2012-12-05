#include <String/common.h>

#include <rf_error.h>

#include <IO/rf_unicode.h> //for rfUTF8_VerifySequence()
#include <rf_localmem.h> //for the local memory macros
#include <String/format.h> //for the String formatting function
#include "../stdio.ph" //for the ioBuffer RF_StringX

#include <stdlib.h> //for exit()

//Allocates and returns a string with the given characters a refu string with the given characters. Given characters have to be in UTF-8. A check for valid sequence of bytes is performed.
RF_String* i_rfString_CreateLocal1(const char* s,...)
{
    RF_String* ret;
    va_list args;
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);

    //read the var args
    va_start(args,s);
    if(rfStringX_Formatv(&ioBuffer,s,args)!=RF_SUCCESS)
    {
        LOG_ERROR("Local string creation failure due to failing at reading the formatted string",RF_LastError);
        return 0;
    }
    va_end(args);

    //allocate the string in the local memory stack
    ret = rfLMS_Push(RF_LMS,sizeof(RF_String));
    if(ret == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    //get length
    ret->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    ret->bytes = rfLMS_Push(RF_LMS,ret->byteLength+1);
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
    rfLMS_MacroEvalPtr(RF_LMS);
    //check for validity of the given sequence and get the character length
    if( rfUTF8_VerifySequence(s,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at String Allocation due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }

    //allocate the string in the local memory stack
    ret = rfLMS_Push(RF_LMS,sizeof(RF_String));
    if(ret == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed during string allocation. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    //get length
    ret->byteLength = byteLength;
    ret->bytes = rfLMS_Push(RF_LMS,ret->byteLength+1);
    if(ret->bytes == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed during string allocation. Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    memcpy(ret->bytes,s,ret->byteLength+1);

    return ret;
}

