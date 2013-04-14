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
#include <Definitions/threadspecific.h> //for the thread specific macro
#include <Definitions/types.h> //for fixed size data types appearing in RF_String and RF_StringX
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include "buff.ph"
//*---------------------Outside module inclusion----------------------------------------
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //to enable default arguments for the corex functions
#include <String/corex.h> //for rfStringX_Init_buff()

#include <Definitions/retcodes.h>
//error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>



//the String to act as the ioBuffer for all the printf family of functions
#define STDIO_BUFF_SIZE 4096
i_THREAD__ RF_StringX ioBuffer;

//Initializes the stdio buffer that is needed by all Refu stdio functions
char rfInitStdio()
{
    if(!rfStringX_Init_buff(&ioBuffer,STDIO_BUFF_SIZE,""))
    {
        LOG_ERROR("Failed to initialize the refu stdio buffer",RE_STDIO_INIT)
        return false;
    }
    return true;
}
