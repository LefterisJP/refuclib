/*
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RF_XML_MODULE_HEADERS
#define RF_XML_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_DF_XML //XML module check

#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/string_decl.h> //for RF_String used in RF_XMLTag
#include <String/stringx_decl.h> //for RF_StringX used in RF_XMLTag
#include <Data_Structures/list_decl.h> //for RF_ListP used in RF_XMLTag
#include <Data_Formats/xmltag_decl.h>//for RF_XMLTag
#include <Data_Formats/xmltag.h> //for RF_XMLTag functions

#include <IO/common.h> //for foff_rft used in RF_XML
#include <stdio.h> //for FILE* used in RF_TextFile
#include <IO/textfile_decl.h> //for RF_TextFile used in RF_XML
#include <Data_Formats/xml_decl.h>//for RF_XML
#include <Data_Formats/xml.h>//for RF_XML functions
#else
    #error Attempted to include RF_XML while not having compiled the library with the appropriate module activated
#endif

#endif //include guards end
