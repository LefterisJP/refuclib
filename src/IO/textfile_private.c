/**
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
**
**
** In this source file the private text file functionality
** is implemented
**/

#include "textfile_private.h"
#include <IO/rfc_textfile.h>
#include <errno.h>

// Adds a Byte order mark to the file at the current position. Only to be used at the start of the file
int32_t i_rfTextFile_AddBom(RF_TextFile* t)
{
    switch(t->encoding)
    {
        case RF_UTF8:
            t->hasBom = true;
            fputc(0xEF,t->f);
            fputc(0xBB,t->f);
            fputc(0xBF,t->f);
            i_WRITE_CHECK(t->f,"Appending the UTF-8 BOM")
        break;
        case RF_UTF16_LE:
            t->hasBom = true;
            fputc(0xFF,t->f);
            fputc(0xFE,t->f);
            i_WRITE_CHECK(t->f,"Appending the UTF-16 Litte Endian BOM")
        break;
        case RF_UTF16_BE:
            t->hasBom = true;
            fputc(0xFE,t->f);
            fputc(0xFF,t->f);
            i_WRITE_CHECK(t->f,"Appending the UTF-16 Big Endian BOM")
        break;
        case RF_UTF32_LE:
            t->hasBom = true;
            fputc(0xFF,t->f);
            fputc(0xFE,t->f);
            fputc(0x00,t->f);
            fputc(0x00,t->f);
            i_WRITE_CHECK(t->f,"Appending the UTF-32 Little Endian BOM")
        break;
        case RF_UTF32_BE:
            t->hasBom = true;
            fputc(0x00,t->f);
            fputc(0x00,t->f);
            fputc(0xFE,t->f);
            fputc(0xFF,t->f);
            i_WRITE_CHECK(t->f,"Appending the UTF-32 Big Endian BOM")
        break;
    }
    return RF_SUCCESS;
}

//Adds a Byte order mark to the file at the current position. Only to be used at the start of the file
int32_t i_rfFile_AddBom(FILE*f,char encoding)
{
    switch(encoding)
    {
        case RF_UTF8:
            fputc(0xEF,f);
            fputc(0xBB,f);
            fputc(0xBF,f);
            i_WRITE_CHECK(f,"Appending the UTF-8 BOM")
        break;
        case RF_UTF16_LE:
            fputc(0xFF,f);
            fputc(0xFE,f);
            i_WRITE_CHECK(f,"Appending the UTF-16 Litte Endian BOM")
        break;
        case RF_UTF16_BE:
            fputc(0xFE,f);
            fputc(0xFF,f);
            i_WRITE_CHECK(f,"Appending the UTF-16 Big Endian BOM")
        break;
        case RF_UTF32_LE:
            fputc(0xFF,f);
            fputc(0xFE,f);
            fputc(0x00,f);
            fputc(0x00,f);
            i_WRITE_CHECK(f,"Appending the UTF-32 Little Endian BOM")
        break;
        case RF_UTF32_BE:
            fputc(0x00,f);
            fputc(0x00,f);
            fputc(0xFE,f);
            fputc(0xFF,f);
            i_WRITE_CHECK(f,"Appending the UTF-32 Big Endian BOM")
        break;
    }
    return RF_SUCCESS;
}
