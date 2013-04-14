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
#ifndef RF_STRING_MODULE_HEADERS
#define RF_STRING_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_STRINGS//check if the strings are included as a module

#include <Definitions/types.h> //fixed size data types
#include <Definitions/imex.h> //including the import export macro
#include <Definitions/defarg.h>//enabling defargs
#include <String/string_decl.h>
#include <String/common.h>
#include <String/unicode.h>
#include <String/flags.h> //for some needed flags

#include <Utils/constcmp.h> //for RF_HEXEQ_C() used in rfUTF8_IsContinuationByte() which itself is used in the iteration macros
#include <String/core.h>

#include <String/retrieval.h>
#include <String/conversion.h>
#include <String/manipulation.h>

#include <stdio.h> //for FILE* used in String/files.h and String/filesx.h
#include <IO/common.h> //for RF_EOL macros used in String/files.h and String/filesx.h
#include <String/files.h>

//*---------------------------------------RF_StringX headers---------------------------------------
#include <String/stringx_decl.h>
#include <String/corex.h>
#include <String/manipulationx.h>
#include <String/traversalx.h>
#include <String/filesx.h>

#include <stdarg.h> //needed for the va_list argument in rfStringX_Formatv()
#include <String/format.h>

#else //end of the strings module include
    #error Attempted to include Refu String manipulation with the String module flag off. Rebuild the library with that option added if you need to include them
#endif

#endif //end OF ifndef guards
