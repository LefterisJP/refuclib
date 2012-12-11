//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>
#include <stdio.h> //for FILE*
#include <IO/printf.h>
//*---------------------Module related inclusion----------------------------------------
#include <Definitions/threadspecific.h> //for the thread specific macro
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include "buff.ph"//include the stdio private declaration of ioBuffer
//*---------------------Outside module inclusion----------------------------------------
#include <Definitions/defarg.h> //for the default argument used in the following headers
#include <stdarg.h> //for va_list() macros. Must come before <String/format.h>
#include <String/format.h>
#include <String/corex.h> //for rfStringX_Assign()
#include <String/common.h> //for RFS_()
#include <String/files.h> //for rfString_Fwrite()
#include <String/unicode.h> //for the unicode macros

#include <Definitions/retcodes.h>
//*----------------------------End of Includes------------------------------------------



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
