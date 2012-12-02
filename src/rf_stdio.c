#include <rf_stdio.h>
#include "stdio.ph"//include the stdio private declaration of ioBuffer

#include <String/rf_stringx.h>
#include <String/format.h>
#include <String/files.h> //for rfString_Fwrite()
#include <String/common.h> //for RFS_()

#include <IO/rf_unicode.h> //for the unicode macros

#include <rf_error.h>

//the String to act as the ioBuffer for all the printf family of functions
#define STDIO_BUFF_SIZE 4096
i_THREAD__ RF_StringX ioBuffer;

char rfInitStdio()
{
    if(!rfStringX_Init_buff(&ioBuffer,STDIO_BUFF_SIZE,""))
    {
        LOG_ERROR("Failed to initialize the refu stdio buffer",RE_STDIO_INIT)
        return false;
    }
    return true;
}


int32_t rfPrintf(const char * format, ...)
{
    int32_t ret;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()
    //clear out the stdio buffer
    rfStringX_Assign(&ioBuffer,RFS_(""));
    //now read the formatted string
    va_list args;
    va_start(args,format);
    ret=rfStringX_Formatv(&ioBuffer,format,args);
    va_end(args);

    //check for error
    if(ret != RF_SUCCESS)
        return ret;
    //else return the number of bytes written
    ret = ioBuffer.INH_String.byteLength;
    rfString_Fwrite(&ioBuffer,stdout,RF_UTF8);
    return ret;
}

int32_t rfFPrintf(FILE* f,const char * format, ...)
{
    int32_t ret;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()
    //clear out the stdio buffer
    rfStringX_Assign(&ioBuffer,RFS_(""));
    //now read the formatted string
    va_list args;
    va_start(args,format);
    ret=rfStringX_Formatv(&ioBuffer,format,args);
    va_end(args);

    //check for error
    if(ret != RF_SUCCESS)
        return ret;
    //else return the number of bytes written
    ret = ioBuffer.INH_String.byteLength;
    rfString_Fwrite(&ioBuffer,f,RF_UTF8);
    return ret;
}
