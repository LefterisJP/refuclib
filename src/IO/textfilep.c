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
**
**
** In this source file the private text file functionality
** is implemented
*/
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <stdio.h> //for FILE*
#include <String/string_decl.h> // for RF_String (used in RF_TextFile
#include <IO/textfile_decl.h> //for RF_TextFile
#include "textfile.ph"
//*---------------------Module related inclusion----------------------------------------
#include <IO/common.h> //for common I/O flags and definitions
#include <Definitions/imex.h>
#include <IO/file.h> //for all the IO functions
#include "io.ph"//for private I/O macros
//*---------------------Outside module inclusion----------------------------------------
#include <String/unicode.h> //for the unicode macros

#include <Definitions/retcodes.h>//for the return codes
//for error logging
    #include <Definitions/types.h>
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h>
#include <string.h> //for strstr()
//*----------------------------End of Includes------------------------------------------

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

// Takes a textfile's inner FILE* to the beginning of the file depending on the encoding and BOM existence. Can fail and returns an appropriate error
int32_t TextFile_GoToStart(RF_TextFile* t)
{
    //depending on the encoding of the file
    int byteOffset = 0;
    switch(t->encoding)
    {
        case RF_UTF8:
         if(t->hasBom == true)
            byteOffset = 3;
        break;
        case RF_UTF16_BE:
        case RF_UTF16_LE:
            if(t->hasBom == true)
                byteOffset = 2;
        break;
        case RF_UTF32_BE:
        case RF_UTF32_LE:
            if(t->hasBom == true)
                byteOffset = 4;
        break;
    }
    //First rewind back to the start so that read/write operations can be reset
    if(rfFseek(t->f, 0, SEEK_SET) != 0)
    {
        i_TEXTFILE_FSEEK_CHECK(t,"attempting to rewind to the beginning "
                               "of a TextFile's inner file pointer")
    }
    if(rfFseek(t->f,byteOffset,SEEK_SET) != 0)
    {
        i_TEXTFILE_FSEEK_CHECK(t,"attempting to go over the BOM of a "
                               "TextFile")
    }
    t->previousOp = 0;
    t->line = 1;
    t->eof = false;
    return RF_SUCCESS;
}


// Handles the EOL encoding for this textfile by either setting the desired encoding pattern or auto detecting it
char TextFile_HandleEol(RF_TextFile* t,char eol)
{
    char ret;
    uint32_t c,n;
    int32_t error=RF_SUCCESS;
    ret=true;
    //set the eol and if we need to auto detect it then do it
    switch(eol)
    {
        case RF_EOL_AUTO:
        case RF_EOL_LF:
        case RF_EOL_CR:
        case RF_EOL_CRLF:
            t->eol = eol;
        break;
        default:
            LOG_ERROR("An illegal eol value has been given to the initialization of TextFile \"%S\"",RE_INPUT,&t->name)
            t->eol = RF_EOL_DEFAULT;
            return false;
        break;
    }
    if(eol == RF_EOL_AUTO)
    {
        t->eol = RF_EOL_DEFAULT;//set it as newline by default
        do//begin the reading loop
        {
            switch(t->encoding)
            {
               case RF_UTF8:
                    error = rfFgetc_UTF8(t->f,&c,true);
               break;
               case RF_UTF16_LE:
                    error = rfFgetc_UTF16LE(t->f,&c,true);
               break;
               case RF_UTF16_BE:
                    error = rfFgetc_UTF16BE(t->f,&c,true);
               break;
               case RF_UTF32_LE:
                    error = rfFgetc_UTF32LE(t->f,&c);
               break;
               case RF_UTF32_BE:
                    error = rfFgetc_UTF32BE(t->f,&c);
               break;
            }
            //check for reading problem
            if(error < 0)
                break;

            //if you find a carriage return
            if(c == RF_CR)
            {   //check the next character and decide the EOL pattern accordingly
                switch(t->encoding)
                {
                   case RF_UTF8:
                        error = rfFgetc_UTF8(t->f,&n,true);
                   break;
                   case RF_UTF16_LE:
                        error = rfFgetc_UTF16LE(t->f,&n,true);
                   break;
                   case RF_UTF16_BE:
                        error = rfFgetc_UTF16BE(t->f,&n,true);
                   break;
                   case RF_UTF32_LE:
                        error = rfFgetc_UTF32LE(t->f,&n);//careful, this returns RF_SUCCESS(=0) for success and negative for error
                   break;
                   case RF_UTF32_BE:
                        error = rfFgetc_UTF32BE(t->f,&n);//careful, this returns RF_SUCCESS(=0) for success and negative for error
                   break;
                }
                //check for reading problem
                if(error < 0)
                    break;

                t->eol = RF_EOL_CR;
                if(n == RF_LF)
                    t->eol = RF_EOL_CRLF;
                //break from the reading loop
                break;
            }
            else if(c == RF_LF)//else if we find a line feed character without having found a CR then it should be the default case of Unix style endings
                break;
        }while(error >=0 );//end of reading loop
        if(error < 0 && error != RE_FILE_EOF)
            ret = false;

        //and now go back to the beginning of the text file
        if((error=TextFile_GoToStart(t))!= RF_SUCCESS)
        {
            LOG_ERROR("Failed to move the internal file pointer to the beginning after EOL pattern auto-detection",error)
            ret=false;
        }
    }

    return ret;
}
