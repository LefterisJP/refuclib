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
    #include <Threads/common.h> //for rfThread_GetID()
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
    i_rfLMS_ArgsEval(return NULL);

    //read the var args
    va_start(args,s);
    if(rfStringX_Formatv(&ioBuffer,s,args) == false)
    {
        RF_ERROR(0,"Local string creation failure due to failing at reading the"
                 " formatted string");
        return NULL;
    }
    va_end(args);

    //allocate the string in the local memory stack
    i_rfLMS_Push(ret,sizeof(RF_String), return NULL);
    //get length
    ret->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    i_rfLMS_Push(ret->bytes,ret->byteLength+1, return NULL);

    memcpy(ret->bytes,ioBuffer.INH_String.bytes,ret->byteLength+1);

    return ret;
}
RF_String* i_NVrfString_CreateLocal(const char* s)
{
    RF_String* ret;
    uint32_t byteLength;
    //remember the stack pointer before this macro evaluation
    i_rfLMS_ArgsEval(return NULL);
    //check for validity of the given sequence and get the character length
    if(!rfUTF8_VerifySequence(s,&byteLength))
    {
        RF_ERROR(0,
                 "Error at String Allocation due to invalid UTF-8 byte sequence");
        return NULL;
    }

    //allocate the string in the local memory stack
    i_rfLMS_Push(ret,sizeof(RF_String), return NULL);
    //get length
    ret->byteLength = byteLength;
    i_rfLMS_Push(ret->bytes, ret->byteLength+1, return NULL);
    memcpy(ret->bytes,s,ret->byteLength+1);

    return ret;
}

