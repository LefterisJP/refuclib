/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <IO/printf.h>
/*------------- Module related inclusion -------------*/
#include <Definitions/threadspecific.h> //for the thread specific macro
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include "buff.ph"//include the stdio private declaration of ioBuffer
/*------------- Outside Module inclusion -------------*/
#include <Definitions/defarg.h> //for the default argument used in the following headers
#include <stdarg.h> //for va_list() macros. Must come before <String/format.h>
#include <String/format.h>
#include <String/corex.h> //for rfStringX_Assign()
#include <String/common.h> //for RFS_()
#include <String/files.h> //for rfString_Fwrite()
#include <String/unicode.h> //for the unicode macros

// for bool
    #include <Definitions/retcodes.h>
// for the endianess check
    #include <Utils/endianess.h>
// for error logging
    #include <Utils/error.h>
// for the local scope macros
    #include <Utils/localmem_decl.h>
    #include <Utils/localscope.h>
/*------------- End of includes -------------*/



int rfPrintf(const char * format, ...)
{
    va_list args;
    int32_t ret;
    RF_ENTER_LOCAL_SCOPE();
    //clear out the stdio buffer
    if(!rfStringX_Assign(&ioBuffer, RFS_("")))
    {
        RF_ERROR("Failure to initialize the stdio buffer during printf");
        RF_EXIT_LOCAL_SCOPE();
        return -1;
    }
    //now read the formatted string
    va_start(args,format);
    if(rfStringX_Formatv(&ioBuffer,format,args) == false)
    {
        RF_ERROR("Failure to format a string during printf");
        va_end(args);
        RF_EXIT_LOCAL_SCOPE();
        return -1;
    }
    va_end(args);

    //else return the number of bytes written
    ret = ioBuffer.INH_String.byteLength;
    if(!rfString_Fwrite(&ioBuffer, stdout, RF_UTF8))
    {
        RF_ERROR("Failure to write the data to stdout during print");
        RF_EXIT_LOCAL_SCOPE();
        return -1;
    }
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

int rfFPrintf(FILE* f,const char * format, ...)
{
    va_list args;
    int32_t ret;
    RF_ENTER_LOCAL_SCOPE();
    //clear out the stdio buffer
    if(!rfStringX_Assign(&ioBuffer, RFS_("")))
    {
        RF_ERROR("Failure to initialize the stdio buffer during fprintf");
        RF_EXIT_LOCAL_SCOPE();
        return -1;
    }
    //now read the formatted string
    va_start(args,format);
    if(rfStringX_Formatv(&ioBuffer,format,args) == false)
    {
        RF_ERROR("Failure to format a string during fprintf");
        va_end(args);
        RF_EXIT_LOCAL_SCOPE();
        return -1;
    }
    va_end(args);

    //else return the number of bytes written
    ret = ioBuffer.INH_String.byteLength;
    if(!rfString_Fwrite(&ioBuffer, f, RF_UTF8, rfEndianess()))
    {
        RF_ERROR("Failure to write the data to the the file during fprint");
        RF_EXIT_LOCAL_SCOPE();
        return -1;
    }
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
