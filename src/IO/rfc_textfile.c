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
**/

#include <errno.h>
#include <string.h> //for strstr

#include <rf_setup.h>
#include <IO/rfc_textfile.h>
#include <rf_utils.h>
#include <rf_io.h>
#include <rf_system.h>
#include <IO/rf_unicode.h> //for unicode stuff
#include "../String/string_private.h"//for the READ_VNPRINTF_ARGS macro
#include "io_private.h"
#include "textfile_private.h"

//Initializes a new text file
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfTextFile_Init(RF_TextFile* t,const void* nameP,char mode,char encoding)
#else
int32_t i_rfTextFile_Init(RF_TextFile* t,const void* nameP,char mode,char encoding)
#endif
{
    int32_t error;
    RF_String* name = (RF_String*) nameP;
    RF_ENTER_LOCAL_SCOPE()

    //save the name of the file
    rfString_Copy_IN(&t->name,name);
    t->hasBom = false;

    //depending on the mode open the file
    switch(mode)
    {
        case RF_FILE_WRITE:
            t->mode = RF_FILE_READWRITE;
            t->f = fopen(name->bytes,"a"i_PLUSB_WIN32"+");
        break;
        case RF_FILE_READ:
            t->mode = RF_FILE_READ;
            t->f = fopen(name->bytes,"r"i_PLUSB_WIN32);
        break;
        case RF_FILE_NEW:
            t->mode=RF_FILE_WRITE;
            t->f = fopen(name->bytes,"w"i_PLUSB_WIN32);
        break;
        case RF_FILE_READWRITE:
            t->mode = RF_FILE_READWRITE;
            t->f = fopen(name->bytes,"r"i_PLUSB_WIN32"+");
        break;
        case RF_FILE_READWRITE_NEW:
            t->mode = RF_FILE_READWRITE;
            t->f = fopen(name->bytes,"w"i_PLUSB_WIN32"+");
        break;
        default:
            LOG_ERROR("Attempted to initialize textfile \"%s\" with illegal mode",RE_FILE_MODE,name->bytes);
            rfString_Deinit(&t->name);
            error = RE_FILE_MODE;
            goto cleanup1;
        break;
    }//end of opening mode switch
    //if the file failed to open read errno
    if(t->f == 0)
        i_TEXTFILE_FOPEN_CHECK_GOTO(t,"Initialization of",error,cleanup1)



    //for an existing file, get endianess and check for Byte Order Mark (BOM)
    if(mode == RF_FILE_READ || mode == RF_FILE_READ || mode == RF_FILE_READWRITE)
    {
        //check for correct input of the encoding parameter
        switch(encoding)
        {
            case RF_UTF16://if we are reading the file in UTF-16 we need to find endianess
            {
                //search for the BOM
                int16_t c;
                if( fread(&c,2,1,t->f)!=1)
                {
                    i_READ_CHECK_GOTO(t->f,"While opening UTF-16 text file",error,cleanup1)
                    RETURNGOTO_LOG_ERROR("Attempted to open an empty UTF-16 text file for reading",RE_FILE_EOF,error,cleanup1)
                }
                if(c == (int16_t)0xFEFF)
                {
                    if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                        t->encoding = RF_UTF16_LE;
                    else
                        t->encoding = RF_UTF16_BE;
                    t->hasBom = true;
                }
                else if(c == (int16_t)0xFFFE)
                {
                    if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                        t->encoding = RF_UTF16_BE;
                    else
                        t->encoding = RF_UTF16_LE;
                    t->hasBom = true;
                }
                else//no BOM at beginning, scan the file for the space character 0x0020
                {
                    uint32_t bytesN=0;
                    //go back to start
                    fseek(t->f,-2,SEEK_CUR);
                    //scan for a space character
                    while(1)
                    {
                        if(fread(&c,2,1,t->f)!=1)
                        {
                            i_READ_CHECK_GOTO(t->f,"While reading a UTF-16 text file to determine endianess",error,cleanup1);
                            RETURNGOTO_LOG_ERROR("EOF found very close to UTF-16 file's beginning while attempting to determine endianess",RE_FILE_EOF,error,cleanup1)
                        }
                        bytesN+=2;
                        if(c == (int16_t) 0x0020)
                        {
                            if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                                t->encoding = RF_UTF16_LE;
                            else
                                t->encoding = RF_UTF16_BE;
                            break;
                        }
                        else if(c == (int16_t) 0x2000)
                        {
                            if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                                t->encoding = RF_UTF16_BE;
                            else
                                t->encoding = RF_UTF16_LE;
                            break;
                        }
                        //else keep reading
                    }//end of scanning
                    //go back to the file's beginning
                    fseek(t->f,0,SEEK_SET);
                }//end of no BOM at beginning case
            }
            break;
            case RF_UTF32://if we are reading the file in UTF-32 we need to find endianess
            {
                //search for the BOM
                int32_t c;
                if( fread(&c,4,1,t->f)!=1)
                {
                    i_READ_CHECK_GOTO(t->f,"While opening UTF-32 text file",error,cleanup1)
                    RETURNGOTO_LOG_ERROR("Attempted to open an empty UTF-32 text file for reading",RE_FILE_EOF,error,cleanup1)
                }
                if(c == (int32_t)0xFEFF)
                {
                    if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                        t->encoding = RF_UTF32_LE;
                    else
                        t->encoding = RF_UTF32_BE;
                    t->hasBom = true;
                }
                else if(c == (int16_t)0xFFFE0000)
                {
                    if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                        t->encoding = RF_UTF32_BE;
                    else
                        t->encoding = RF_UTF32_LE;
                    t->hasBom = true;
                }
                else//no BOM at beginning, scan the file for the space character 0x0020
                {
                    uint32_t bytesN=0;
                    //go back to start
                    fseek(t->f,-4,SEEK_CUR);
                    //scan for a space character
                    while(1)
                    {
                        if(fread(&c,4,1,t->f)!=1)
                        {
                            i_READ_CHECK_GOTO(t->f,"While reading a UTF-32 text file to determine endianess",error,cleanup1);
                            RETURNGOTO_LOG_ERROR("EOF found very close to UTF-32 file's beginning while attempting to determine endianess",RE_FILE_EOF,error,cleanup1)
                        }
                        bytesN+=4;
                        if(c == (int32_t) 0x20)
                        {
                            if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                                t->encoding = RF_UTF32_LE;
                            else
                                t->encoding = RF_UTF32_BE;
                            break;
                        }
                        else if(c == (int16_t) 0x20000000)
                        {
                            if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                                t->encoding = RF_UTF32_BE;
                            else
                                t->encoding = RF_UTF32_LE;
                            break;
                        }
                        //else keep reading
                    }//end of scanning
                    //go back to the file's beginning
                    fseek(t->f,0,SEEK_SET);
                }//end of no BOM at beginning case
            }
            break;
            case RF_UTF8:
            {
                char c = fgetc(t->f);
                if(RF_HEXEQ_C(c,0xEF))
                {
                    c = fgetc(t->f);
                    if(RF_HEXEQ_C(c,0xBB))
                    {
                        c = fgetc(t->f);
                        //if the last byte of the signature does not match, go back 2 bytes to get to the first one
                        if(!RF_HEXEQ_C(c,0xBF))
                        {
                            fseek (t->f,-3,SEEK_CUR );
                        }
                        //else we must have moved 3 bytes ahead ready to parse the actual stream
                        t->hasBom = true;
                    }
                    else//then this was not BOM signature so go back one byte to get to the first one
                    {
                        fseek (t->f,-2,SEEK_CUR );
                    }
                }
                else//not a BOM signature, go back
                    fseek(t->f,-1,SEEK_CUR );
                t->encoding = RF_UTF8;
            }
            break;
            case RF_UTF16_BE:
            {
                //check for BOM
                uint16_t c;
                if(fread(&c,2,1,t->f)!=1)
                {
                    i_READ_CHECK_GOTO(t->f,"While reading a UTF-16 text file to check for Byte Order Mark Existence",error,cleanup1);
                    RETURNGOTO_LOG_ERROR("EOF found very close to a UTF-16 file's beginning while attempting to check for Byte Order Mark Existence",RE_FILE_EOF,error,cleanup1)
                }
                if(rfUTILS_Endianess()==RF_LITTLE_ENDIAN)
                    rfUTILS_SwapEndianUS(&c);
                if(c==(uint16_t)0xFEFF)
                    t->hasBom = true;
                else
                    fseek(t->f,0,SEEK_SET);
                t->encoding = encoding;
            }
            break;
            case RF_UTF16_LE:
            {//check for BOM
                uint16_t c;
                if(fread(&c,2,1,t->f)!=1)
                {
                    i_READ_CHECK_GOTO(t->f,"While reading a UTF-16 text file to check for Byte Order Mark Existence",error,cleanup1);
                    RETURNGOTO_LOG_ERROR("EOF found very close to a UTF-16 file's beginning while attempting to check for Byte Order Mark Existence",RE_FILE_EOF,error,cleanup1)
                }
                if(rfUTILS_Endianess()==RF_BIG_ENDIAN)
                    rfUTILS_SwapEndianUS(&c);
                if(c==(uint16_t)0xFEFF)
                    t->hasBom = true;
                else
                    fseek(t->f,0,SEEK_SET);
                t->encoding = encoding;
            }
            break;
            case RF_UTF32_BE:
            {//check for BOM
                uint32_t c;
                if(fread(&c,4,1,t->f)!=1)
                {
                    i_READ_CHECK_GOTO(t->f,"While reading a UTF-32 text file to check for Byte Order Mark Existence",error,cleanup1);
                    RETURNGOTO_LOG_ERROR("EOF found very close to a UTF-32 file's beginning while attempting to check for Byte Order Mark Existence",RE_FILE_EOF,error,cleanup1)
                }
                if(rfUTILS_Endianess()==RF_LITTLE_ENDIAN)
                    rfUTILS_SwapEndianUI(&c);
                if(c==(uint32_t)0xFEFF)
                    t->hasBom = true;
                else
                    fseek(t->f,0,SEEK_SET);
                t->encoding = encoding;
            }
            break;
            case RF_UTF32_LE:
            {//check for BOM
                uint32_t c;
                if(fread(&c,4,1,t->f)!=1)
                {
                    i_READ_CHECK_GOTO(t->f,"While reading a UTF-32 text file to check for Byte Order Mark Existence",error,cleanup1);
                    RETURNGOTO_LOG_ERROR("EOF found very close to a UTF-32 file's beginning while attempting to check for Byte Order Mark Existence",RE_FILE_EOF,error,cleanup1)
                }
                if(rfUTILS_Endianess()==RF_BIG_ENDIAN)
                    rfUTILS_SwapEndianUI(&c);
                if(c==(uint32_t)0xFEFF)
                    t->hasBom = true;
                else
                    fseek(t->f,0,SEEK_SET);
                t->encoding = encoding;
            }
            break;
            default:
                LOG_ERROR("Attempted to initialize TextFile \"%s\" with illegal encoding parameter",RE_FILE_ILLEGAL_ENCODING,name->bytes);
                rfString_Deinit(&t->name);
                error = RE_FILE_ILLEGAL_ENCODING;
                goto cleanup1;
            break;
        }
    }//end of existing file case
    else//new file case
    {
        switch(encoding)
        {

            case RF_UTF16:
                if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                {
                    t->encoding = RF_UTF16_LE;
                }
                else
                {
                    t->encoding = RF_UTF16_BE;
                }
            break;

            case RF_UTF32:
                if(rfUTILS_Endianess() == RF_LITTLE_ENDIAN)
                {
                    t->encoding = RF_UTF32_LE;
                }
                else
                {
                    t->encoding = RF_UTF32_BE;
                }
            break;
            case RF_UTF32_LE:
            case RF_UTF32_BE:
            case RF_UTF8:
            case RF_UTF16_LE:
            case RF_UTF16_BE:
                t->encoding = encoding;
            break;
            default:
                LOG_ERROR("Attempted to initialize a new TextFile \"%s\" with illegal encoding parameter",RE_FILE_ILLEGAL_ENCODING,name->bytes);
                rfString_Deinit(&t->name);
                error = RE_FILE_ILLEGAL_ENCODING;
                goto cleanup1;
            break;
        }//end of new file encoding switch
        //add a BOM if required
#ifdef RF_OPTION_FILE_ADDBOM
        t->hasBom = true;
        error=i_rfTextFile_AddBom(t->f,t);//can fail and so this function will also fail
#endif
    }//end of newfile case

    //init some values
    t->eof =false;
    t->line = 1;
    t->previousOp = 0;

cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return error;
}
//Create a new text file
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_TextFile* rfTextFile_Create(const void* name,char mode,char encoding)
#else
RF_TextFile* i_rfTextFile_Create(const void* name,char mode,char encoding)
#endif
{
    RF_TextFile* ret;
    RF_ENTER_LOCAL_SCOPE()
    RF_MALLOC(ret,sizeof(RF_TextFile));
    if(rfTextFile_Init(ret,name,mode,encoding) != RF_SUCCESS)
        ret = 0;

    RF_EXIT_LOCAL_SCOPE()
    return ret;
}


//Copies one textfile into another
void rfTextFile_Copy_IN(RF_TextFile* dst,RF_TextFile* src)
{
    //get the data
    dst->mode = src->mode;
    dst->encoding = src->encoding;
    dst->line = src->line;
    dst->eof = src->eof;
    dst->previousOp = src->previousOp;
    dst->hasBom = src->hasBom;
    //open the same file with the same mode and at the same position
    if(src->mode == RF_FILE_WRITE)
        dst->f = fopen(src->name.bytes,"a");
    else if(src->mode == RF_FILE_READ)
        dst->f = fopen(src->name.bytes,"r");
    else
        dst->f = fopen(src->name.bytes,"r+");
    fpos_t pos;
    fgetpos (src->f,&pos);
    fsetpos(dst->f,&pos);
    //copy the name
    rfString_Copy_IN(&dst->name,&src->name);
}
//Allocates and returns a copy of an RF_TextFile
RF_TextFile* rfTextFile_Copy_OUT(RF_TextFile* src)
{
    RF_TextFile* dst;
    RF_MALLOC(dst,sizeof(RF_TextFile))
    //get the data
    dst->mode = src->mode;
    dst->encoding = src->encoding;
    dst->line = src->line;
    dst->eof = src->eof;
    dst->previousOp = src->previousOp;
    dst->hasBom = src->hasBom;
    //open the same file with the same mode and at the same position
    if(src->mode == RF_FILE_WRITE)
        dst->f = fopen(src->name.bytes,"a");
    else if(src->mode == RF_FILE_READ)
        dst->f = fopen(src->name.bytes,"r");
    else
        dst->f = fopen(src->name.bytes,"r+");
    fpos_t pos;
    fgetpos (src->f,&pos);
    fsetpos(dst->f,&pos);
    //copy the name
    rfString_Copy_IN(&dst->name,&src->name);
    return dst;
}


// Deinitalizes a text file
void rfTextFile_Deinit(RF_TextFile* t)
{
    fclose(t->f);
    rfString_Deinit(&t->name);
}

// Destroys a text file
void rfTextFile_Destroy(RF_TextFile* t)
{
    fclose(t->f);
    rfString_Deinit(&t->name);
    free(t);
}

//Changes the file access mode of the TextFile
int32_t rfTextFile_SetMode(RF_TextFile* t,char mode)
{
    FILE* temp;
    switch(mode)
    {
        case RF_FILE_WRITE:
            if(t->mode == RF_FILE_WRITE)
                return RF_SUCCESS;
            if((temp = freopen(t->name.bytes,"a",t->f)) == 0)
            {
                i_TEXTFILE_FOPEN_CHECK(t,"Changing the file access mode to write mode ")
            }
            t->f = temp;
        break;
        case RF_FILE_READ:
            if(t->mode == RF_FILE_READ)
                return RF_SUCCESS;
            if((temp = freopen(t->name.bytes,"r",t->f)) == 0)
            {
                i_TEXTFILE_FOPEN_CHECK(t,"Changing the file access mode to read mode ")
            }
            t->f = temp;
        break;
        case RF_FILE_READWRITE:
            if(t->mode == RF_FILE_READWRITE)
                return RF_SUCCESS;
            if((temp = freopen(t->name.bytes,"r+",t->f)) == 0)
            {
                i_TEXTFILE_FOPEN_CHECK(t,"Changing the file access mode to read/write mode ")
            }
            t->f = temp;
        break;
        default:
            RETURN_LOG_ERROR("Provided illegal mode input",RE_INPUT)
        break;
    }
    //success
    return RF_SUCCESS;
}


//Reads the next line of the text file
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfTextFile_ReadLine(RF_TextFile* t,RF_StringX* line,uint32_t characters)
#else
int32_t i_rfTextFile_ReadLine3(RF_TextFile* t,RF_StringX* line,uint32_t characters)
#endif
{
    //check if we can read from this textfile
    RF_TEXTFILE_CANREAD(t)
    //get the file offset at the function's start
    foff_rft startOff;
    if((startOff = rfFtell(t->f))== (foff_rft)-1)
    {
        i_FTELL_CHECK("Reading the file position at function start")
    }
    t->previousOp = RF_FILE_READ;
    //check for eof
    if(t->eof == true)
        return RE_FILE_EOF;
    //make sure that the line StringX internal pointer is reset
    line->INH_String.bytes -= line->bIndex;
    line->INH_String.byteLength += line->bIndex;
    line->bIndex = 0;
    int32_t bytesN;
    char eof = false;
    //depending on the file's encoding
    switch(t->encoding)
    {
        case RF_UTF8:
            if( (bytesN = rfStringX_Assign_fUTF8(line,t->f,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a UTF-8 text file failed",bytesN,t->line)
            }
        break;//end of utf-8
        case RF_UTF16_LE:
            if( (bytesN = rfStringX_Assign_fUTF16(line,t->f,RF_LITTLE_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Little Endian UTF-16 text file failed",bytesN,t->line)
            }
        break;
        case RF_UTF16_BE:
            if( (bytesN = rfStringX_Assign_fUTF16(line,t->f,RF_BIG_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Big Endian UTF-16 text file failed",bytesN,t->line)
            }
        break;//end of UTF-16 big endian
        case RF_UTF32_LE:
            if( (bytesN = rfStringX_Assign_fUTF32(line,t->f,RF_LITTLE_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Little Endian UTF-32 text file failed",bytesN,t->line);
            }
        break;
        case RF_UTF32_BE:
            if( (bytesN = rfStringX_Assign_fUTF32(line,t->f,RF_BIG_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Big Endian UTF-32 text file failed",bytesN,t->line)
            }
        break;
    }//switch closes
    if(characters != 0)//if we need specific characters
    {
        uint32_t charsN = rfString_Length(line);
        rfString_PruneEnd(line,charsN-characters);
    }
    //else if the end of line was found make sure it's not included in the returned string
    else if(line->INH_String.bytes[line->INH_String.byteLength-1] == '\n')
    {
        line->INH_String.bytes[line->INH_String.byteLength-1] = '\0';
        line->INH_String.byteLength--;
    }
    //success
    t->line++;
    t->eof = eof;
    return RF_SUCCESS;
}
//in the default arguments case, have a different function for the 2 argument readline
int32_t rfTextFile_ReadLine2(RF_TextFile* t,RF_StringX* line)
{
    int32_t bytesN;
    //check if we can read from this textfile
    RF_TEXTFILE_CANREAD(t)
    t->previousOp = RF_FILE_READ;
    //get the file position at start
    foff_rft startOff;
    if((startOff = rfFtell(t->f))== (foff_rft)-1)
    {
        i_FTELL_CHECK("Reading the file position at function start")
    }
    //check for eof
    if(t->eof == true)
        return RE_FILE_EOF;
    //make sure that the line StringX internal pointer is reset
    line->INH_String.bytes -= line->bIndex;
    line->INH_String.byteLength += line->bIndex;
    line->bIndex = 0;
    char eof = false;
    //depending on the encoding
    switch(t->encoding)
    {
        case RF_UTF8:
            if( (bytesN = rfStringX_Assign_fUTF8(line,t->f,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a UTF-8 text file failed",bytesN,t->line)
            }
        break;//end of utf-8
        case RF_UTF16_LE:
            if( (bytesN = rfStringX_Assign_fUTF16(line,t->f,RF_LITTLE_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Little Endian UTF-16 text file failed",bytesN,t->line)
            }
        break;
        case RF_UTF16_BE:
            if( (bytesN = rfStringX_Assign_fUTF16(line,t->f,RF_BIG_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Big Endian UTF-16 text file failed",bytesN,t->line)
            }
        break;//end of UTF-16 big endian
        case RF_UTF32_LE:
            if( (bytesN = rfStringX_Assign_fUTF32(line,t->f,RF_LITTLE_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Little Endian UTF-32 text file failed",bytesN,t->line)
            }
        break;
        case RF_UTF32_BE:
            if( (bytesN = rfStringX_Assign_fUTF32(line,t->f,RF_BIG_ENDIAN,&eof)) < 0)
            {
                rfFseek(t->f,startOff,SEEK_SET);
                RETURN_LOG_ERROR("Reading line [%"PRIu64"] of a Big Endian UTF-32 text file failed",bytesN,t->line)
            }
        break;
    }//switch closes
    //success
    t->line++;
    t->eof = eof;
    //also if the end of line was found make sure it's not included in the returned string
    if(line->INH_String.bytes[line->INH_String.byteLength-1] == '\n')
    {
        line->INH_String.bytes[line->INH_String.byteLength-1] = '\0';
        line->INH_String.byteLength--;
    }
    return RF_SUCCESS;
}

//Gets a specific line from a Text File starting from the beginning
int32_t rfTextFile_GetLine_begin(RF_TextFile* t,uint64_t lineN,RF_StringX* line)
{
    uint64_t prLine,i=1;
    int32_t error;
    foff_rft prOff;
    char prEof;
    RF_StringX buffer;
    //in the very beginning keep the previous file position
    prLine = t->line;
    if(lineN == 0)
        return RE_INPUT;
    if((prOff = rfFtell(t->f)) == (foff_rft)-1)
    {
        i_FTELL_CHECK("Querying current file pointer at function start")
    }
    prEof = t->eof;
    //now get the position to the beginning of the file
    TEXTFILE_GOTOSTART(t,"Resetting the file pointer")
    ///since we got here start reading the file again, line by line until we get to the requested line
    //initialize the buffer string for readline
    rfStringX_Init_buff(&buffer,RF_OPTION_FGETS_READBYTESN,"");
    //read all the lines until you get to the one we need
    while( (error = rfTextFile_ReadLine2(t,&buffer)) == RF_SUCCESS)
    {
        //if this is the line we need, assign it to the given string
        if(i==lineN)
        {
            rfStringX_Assign(line,&buffer);
            break;
        }
        i++;
    }
    //if we get here and we haven't reached the line
    if(i!= lineN)
    {
        if(error == RE_FILE_EOF)
        {
            TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
            RETURN_LOG_ERROR("While searching for line [%"PRIu64"] in Text File \"%s\" the end of file was found before being able to reach that line",RE_FILE_EOF,lineN,t->name.bytes);
        }
        //else there was an error at line reading
        TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
        RETURN_LOG_ERROR("While reading Text File's lines \"%s\" there was an error in file reading. File must be corrupt",RE_FILE_BAD,t->name.bytes)
    }
    //success
    //now we are done so we can return the line number and the file pointer to their original positions, also free stuff
    rfStringX_Deinit(&buffer);
    TEXTFILE_RESETPTR(t,prLine,prEof,prOff)
    return RF_SUCCESS;
}
//Gets a specific line from a Text File
int32_t rfTextFile_GetLine(RF_TextFile* t,uint64_t lineN,RF_StringX* line)
{
    uint64_t prLine;
    char prEof;
    foff_rft prOff;
    int32_t error;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    if((prOff = rfFtell(t->f)) == (foff_rft)-1)
    {
        i_FTELL_CHECK("Querying current file pointer at function start")
    }
    prEof = t->eof;
    //move to the beginning of the line and get it. If there was an error reset everything back
    if((error=rfTextFile_MoveLines(t,lineN-t->line)) != RF_SUCCESS)
    {
        TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
        return error;
    }
    //and now get it
    rfTextFile_ReadLine2(t,line);
    //success, so we can return the line number and the file pointer to their original positions
    TEXTFILE_RESETPTR(t,prLine,prEof,prOff)
    return RF_SUCCESS;
}

//Moves the file pointer by a given number of lines relative to the current line
int32_t rfTextFile_MoveLines(RF_TextFile* t,int64_t linesN)
{
    uint64_t prLine;
    uint32_t i;
    foff_rft prOff;
    int32_t error;
    uint64_t targetLine;
    char prEof;
    RF_StringX buffer;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    prEof = t->eof;
    if((prOff = rfFtell(t->f)) == (foff_rft)-1)
    {
        i_FTELL_CHECK("Querying current file pointer at function start")
    }
    //now depending on the sign of linesN
    ///positive case///
    if(linesN>0)
    {
        char success = false;
        targetLine = prLine+linesN;
        rfStringX_Init_buff(&buffer,RF_OPTION_FGETS_READBYTESN,"");
        //in the positive case the operation is easy, just read the lines until we get to the required one
        while( (error = rfTextFile_ReadLine2(t,&buffer)) == RF_SUCCESS)
        {
            if(t->line == targetLine)
            {
                success = true;
                break;
            }
        }
        //if we got here with no success
        if(success == false)
        {
            //get back to the file position before function call
            TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
            //else there was an error at line reading
            if(error != RE_FILE_EOF)
                RETURN_LOG_ERROR("While reading Text File's lines forward \"%s\" there was an error in file reading. File must be corrupt",RE_FILE_BAD,t->name.bytes)
            //if it was EOF just return it
            return RE_FILE_EOF;
        }
        // SUCCESS getting here means we are done
        rfStringX_Deinit(&buffer);
        return RF_SUCCESS;
    }
    ///-----------------------------------
    /// negative or 0 case starts
    ///-----------------------------------
    if( (uint64_t)(linesN*-1) >= prLine) //if we ask to go back before the start
        return RF_FAILURE;

    targetLine = prLine+linesN;
    //since we will be going backwards anyway we can't be at the end of file, so falsify it in case it was true before
    t->eof = false;
    ///special case , target line is the very first one, or is closer to the beginning of the file than here
    if(targetLine == 1 || targetLine < prLine/2)
    {
        TEXTFILE_GOTOSTART(t,"Resetting the file pointer")
        //read as many lines as needed
        rfStringX_Init_buff(&buffer,RF_OPTION_FGETS_READBYTESN,"");
        for(i=1;i<targetLine; i ++)
        {
            if((error = rfTextFile_ReadLine2(t,&buffer)) != RF_SUCCESS)
            {
                //there was an error at line reading
                TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
                RETURN_LOG_ERROR("While reading Text File's lines forward \"%s\" there was an error in file reading. File must be corrupt",RE_FILE_BAD,t->name.bytes)
            }

        }
        //deinit line buffer and success
        rfStringX_Deinit(&buffer);
        return RF_SUCCESS;
    }//end of special case
    ///start moving back by the required number of lines
    RF_TEXTFILE_CANREAD(t)
    t->previousOp = RF_FILE_READ;
    while(t->line >= targetLine)
    {//start of moving back loop
        if((error=rfTextFile_MoveChars_b(t,1))!=RF_SUCCESS)
            RETURN_LOG_ERROR("Error while moving backwards in Text File \"%s\"",error,t->name.bytes)

    }//end of moving back loop
    //go 1 char forward to be at the line's start
    rfTextFile_MoveChars_f(t,1);
    ///success here
    return RF_SUCCESS;
}

//Moves the internal file pointer by a number of characters
int32_t rfTextFile_MoveChars(RF_TextFile* t,int64_t charsN)
{
    //do nothing if charsN is 0
    if(charsN == 0)
        return RF_SUCCESS;
    if(charsN > 0)
    {
        return rfTextFile_MoveChars_f(t,(uint64_t)charsN);
    }
    return rfTextFile_MoveChars_b(t,(uint64_t)(charsN*-1));
}

//Moves the internal file pointer by an amount of characters forward
int32_t rfTextFile_MoveChars_f(RF_TextFile* t,uint64_t charsN)
{
    int32_t error;
    uint32_t c;
    uint64_t i;
    //move charsN chars forward
    for(i=0;i<charsN; i ++)
    {
        //depending on the encoding
        switch(t->encoding)
        {
            case RF_UTF8:
                if((error=rfFgetc_UTF8(t->f,&c,true)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving forward in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
            case RF_UTF16_LE:
                if((error=rfFgetc_UTF16LE(t->f,&c,true)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving forward in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
            case RF_UTF16_BE:
                if((error=rfFgetc_UTF16BE(t->f,&c,true)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving forward in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
            case RF_UTF32_LE:
                if((error=rfFgetc_UTF32LE(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving forward in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            case RF_UTF32_BE:
                if((error=rfFgetc_UTF32BE(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving forward in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
        }//end of encoding switch
        //if it's a new line increase the line
        if(RF_HEXEQ_I(c,RF_LF))
            t->line++;
    }//end of for
    return RF_SUCCESS;
}
//Moves the internal file pointer by an amount of characters forward
int32_t rfTextFile_MoveChars_b(RF_TextFile* t,uint64_t charsN)
{
    int32_t error;
    uint32_t c;
    uint64_t i;
    //move charsN chars back
    for(i=0;i<charsN; i ++)
    {
        //depending on the encoding
        switch(t->encoding)
        {
            case RF_UTF8:
                if((error=rfFback_UTF8(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving backwards in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
            case RF_UTF16_LE:
                if((error=rfFback_UTF16LE(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving backwards in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
            case RF_UTF16_BE:
                if((error=rfFback_UTF16BE(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving backwards in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
            case RF_UTF32_LE:
                if((error=rfFback_UTF32LE(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving backwards in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            case RF_UTF32_BE:
                if((error=rfFback_UTF32BE(t->f,&c)) < 0)
                {
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("There was an error while moving backwards in textfile \"%s\"",error,t->name.bytes);
                    return error;
                }
            break;
        }//end of encoding switch
        //if it's a new line decrease the line
        if(RF_HEXEQ_I(c,RF_LF))
            t->line--;
    }//end of for
    //also make sure eof is false, and finish
    t->eof = false;
    return RF_SUCCESS;
}

// Moves the file pointer to the start of the given line
int32_t rfTextFile_GoToLine(RF_TextFile* t,uint64_t lineN)
{
    if(lineN == 0)
        RETURN_LOG_ERROR("Illegal linesN value was given. 0 is not a legal value",RE_INPUT)


    if(lineN == 1)
    {
        //go to the file's beginning
        TEXTFILE_GOTOSTART(t,"Going to the file's beginning")
        return RF_SUCCESS;
    }
    else if(lineN == t->line)
        return RF_SUCCESS;
    return rfTextFile_MoveLines(t,lineN-t->line);
}
//Moves the file pointer at the given offset
int32_t rfTextFile_GoToOffset(RF_TextFile* t,foff_rft offset,int origin)
{
    uint64_t prLine;
    int32_t error;
    foff_rft prOff;
    char prEof;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    if((prOff = rfFtell(t->f)) == (foff_rft)-1)
    {
        i_FTELL_CHECK("Querying current file pointer at function start")
    }
    prEof = t->eof;
    ///From the beginning Case
    if(origin == SEEK_SET)
    {
        foff_rft cOff;
        if(offset < 0)//illegal values combination
            return RE_INPUT;
        //go to the start of the file
        TEXTFILE_GOTOSTART(t,"Resetting the file pointer");
        do
        {
            //keep going forward
            if((error=rfTextFile_MoveChars_f(t,1)) != RF_SUCCESS)
            {
                if(error != RE_FILE_EOF)
                    LOG_ERROR("Moving forward in TextFile \"%s\" failed ",error,t->name.bytes)

                return error;
            }
            if((cOff = rfFtell(t->f)) == (foff_rft)-1)//get the current file position
            {
                i_FTELL_CHECK("Moving forward and querying file pointer")
            }
        }while(cOff <offset);//as long as we are behind the required offset keep going
    }//end of SEEK_SET (from beginning case)
    else if(origin == SEEK_CUR)///from the current position case
    {
        foff_rft cOff,tOffset = prOff+offset;//the target offset
        if(offset == 0)
            return RF_SUCCESS;
        else if(offset > 0)
        {///positive case
            do
            {//keep going forward
                if((error=rfTextFile_MoveChars_f(t,1)) != RF_SUCCESS)
                {
                    TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
                    if(error != RE_FILE_EOF)
                        LOG_ERROR("Moving forward in TextFile \"%s\" failed ",error,t->name.bytes)
                    return error;
                }
                if((cOff = rfFtell(t->f)) == (foff_rft)-1)//get the current file position
                {
                    i_FTELL_CHECK("Moving forward and querying file pointer")
                }
            }while(cOff <tOffset);//as long as we are behind the required offset keep going
        }
        else///negative case
        {
            if(tOffset<0)//if we requeste to go beyond the file's start
                return RF_FAILURE;
            do
            {//keep going backwards
                if((error=rfTextFile_MoveChars_b(t,1)) != RF_SUCCESS)
                {
                    TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
                    RETURN_LOG_ERROR("Moving backwards in TextFile \"%s\" failed ",error,t->name.bytes)
                }
                if((cOff = rfFtell(t->f)) == (foff_rft)-1)//get the current file position
                {
                    i_FTELL_CHECK("Moving backwards and querying file pointer")
                }
            }while(cOff <offset);//as long as we are behind the required offset keep going
        }//end of the negative case
    }//end of the SEEK_CUR case
    else
        return RE_INPUT;
    //also undo the eof flag if it was on since we moved
    if(t->eof == true)
        t->eof = false;
    //success
    return RF_SUCCESS;
}

// Writes an RF_String to the end of the text file
int32_t rfTextFile_Write(RF_TextFile* t,void* stringP)
{
    uint32_t linesN;
    int32_t error = RF_SUCCESS;
    char allocatedS = false;
    RF_String* s = (RF_String*)stringP;
    RF_ENTER_LOCAL_SCOPE()

    //if the file mode is not write then turn it to writing
    RF_TEXTFILE_CANWRITE_GOTO(t,error,cleanup1)
    t->previousOp = RF_FILE_WRITE;
    //let's see how many lines it will be adding to the text file
    linesN = rfString_Count(s,RFS_("\n"),0);
#ifdef RF_NEWLINE_CRLF
    //if we consider CRLF the file line ending then change all newlines to CRLF
    if(linesN != 0)
    {
        allocatedS = true;
        //making a new one since stringP can be on the local stack and we can't use replace since that would act on the local stack
        s = rfString_Copy_OUT((RF_String*)stringP);
        rfString_Replace(s,RFS_("\n"),RFS_("\xD\n"),0,0);
    }
#endif
    //depending on the encoding of the file
    if((error=rfString_Fwrite(s,t->f,t->encoding))!=RF_SUCCESS)
    {
        LOG_ERROR("There was a file write error while writting string \"%s\" to Text File \"%s\"",error,s->bytes,t->name.bytes);
        if(allocatedS == true)
            rfString_Destroy(s);
        goto cleanup1;
    }
    t->line += linesN;//also add as many lines as were inside the string

cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return error;
}


//Inserts a line into a specific part of the Text File
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfTextFile_Insert(RF_TextFile* t,uint64_t lineN,void* stringP,char after)
#else
int32_t i_rfTextFile_Insert(RF_TextFile* t,uint64_t lineN,void* stringP,char after)
#endif
{
    char tempFName[L_tmpnam+1], *tmpNamePtr;
    char lineFound,allocatedS;
    foff_rft tOff;
    FILE* newFile;
    uint32_t linesCount;
    RF_StringX buffer;
    int32_t error = RF_FAILURE;
    //get the function's arguments
    RF_String* string = (RF_String*)stringP;
    RF_ENTER_LOCAL_SCOPE()

    lineFound = allocatedS = false;
    //determine the target line
    if(lineN == 0)
        goto cleanup0;//failure
    if(after==false)
        lineN-=1;


    //determine how many lines the given string has
    linesCount = rfString_Count(string,RFS_("\n"),0)+1;
#ifdef RF_NEWLINE_CRLF /// cleanup 1 - For the string
    //if we consider CRLF the file line ending then change all newlines to CRLF
    //making a new one since stringP can be on the local stack and we can't use replace since that would act on the local stack
    if(linesCount>1)
    {
        allocatedS = true;
        string = rfString_Copy_OUT((RF_String*)stringP);
        rfString_Replace(string,RFS_("\n"),RFS_("\xD\n"),0,0);
    }
#endif
    //go to the beginning of this file
    TEXTFILE_GOTOSTART_GOTO(t,"Resetting the file pointer",error,cleanup1);
    //check if this file can read
    RF_TEXTFILE_CANREAD_GOTO(t,error,cleanup1)
    t->previousOp = RF_FILE_READ;
    //now open another temporary file
    tmpNamePtr = tmpnam(tempFName);///cleanup2 - is for temporary file Deletion
    newFile = fopen(tmpNamePtr,"w"i_PLUSB_WIN32);
    if(t->hasBom)
    {
        if((error=i_rfTextFile_AddBom(newFile,t))!= RF_SUCCESS)
        {
            LOG_ERROR("Failed to add BOM to the newly created temporary file",error);
            goto cleanup2;
        }
    }
    //special case
    if(lineN==0)
    {
        lineFound = true;
        if((tOff = rfFtell(newFile))== (foff_rft)-1)//get the file offset that we are going to come back to in success
        {
            i_FTELL_CHECK_GOTO("Reading the file position",error,cleanup2);
        }
#ifdef RF_NEWLINE_CRLF
        if((error=rfString_Fwrite(RFS_("%s\xD\n",string->bytes),newFile,t->encoding))!= RF_SUCCESS)
#else
        if((error=rfString_Fwrite(RFS_("%s\n",string->bytes),newFile,t->encoding))!= RF_SUCCESS)
#endif
        {
            LOG_ERROR("There was a file write error while inserting string \"%s\" at the beginning of Text File \"%s\"",error,string->bytes,t->name.bytes);
            goto cleanup2;
        }
    }
    //initialize a string reading buffer
    rfStringX_Init_buff(&buffer,RF_OPTION_FGETS_READBYTESN+1,"");///cleanup3 - for the string buffer deinitialization
    //read every line of this file from the beginning;
    while((error =rfTextFile_ReadLine2(t,&buffer))==RF_SUCCESS)
    {
        //write the read line to the other file
#ifdef RF_NEWLINE_CRLF
        if((error=rfString_Fwrite(RFS_("%s\xD\n",rfString_Cstr(&buffer)),newFile,t->encoding))!= RF_SUCCESS)
#else
        if((error=rfString_Fwrite(RFS_("%s\n",rfString_Cstr(&buffer)),newFile,t->encoding))!= RF_SUCCESS)
#endif
        {
            LOG_ERROR("There was a file write error while inserting string \"%s\" at line [%"PRIu64"] inside Text File \"%s\"",error,string->bytes,lineN,t->name.bytes);
            goto cleanup3;
        }
        //also if this is the place to put the line, do it
        if(t->line-1 == lineN)//-1 is since right after the ReadLine2 function the current line changes
        {
            lineFound = true;
            if((tOff = rfFtell(newFile))== (foff_rft)-1)//get the file offset that we are going to come back to in success
            {
                i_FTELL_CHECK_GOTO("Reading the file position",error,cleanup3);
            }
            //write the given line to the other file
#ifdef RF_NEWLINE_CRLF
            if((error=rfString_Fwrite(RFS_("%s\xD\n",string->bytes),newFile,t->encoding))!= RF_SUCCESS)
#else
            if((error=rfString_Fwrite(RFS_("%s\n",string->bytes),newFile,t->encoding))!= RF_SUCCESS)
#endif
            {
                LOG_ERROR("There was a file write error while inserting string \"%s\" at line [%"PRIu64"] inside Text File \"%s\"",error,string->bytes,lineN,t->name.bytes);
                goto cleanup3;
            }
        }
    }
    //get rid of the no longer needed buffer
    rfStringX_Deinit(&buffer); /// going back to cleanup2
    // if we reached the end of file and the line was not found
    if(lineFound==false)
    {
        if(error == RE_FILE_EOF)
        {
            LOG_ERROR("While attempting to find line [%"PRIu64"] of TextFile \"%s\" premature End Of File was encountered",RE_FILE_EOF,lineN,t->name.bytes);
            goto cleanup2;
        }//or else if there was an error
        else
        {
            LOG_ERROR("While attempting to find line [%"PRIu64"] of TextFile \"%s\" a file reading error was encountered",error,lineN,t->name.bytes);
            goto cleanup2;
        }
    }
    //delete the old file
    fclose(t->f);
    if(rfDeleteFile(&t->name) != RF_SUCCESS)
    {
        error = RE_FILE_DELETE;
        LOG_ERROR("After the insertion operation the temporary file used, could not be deleted",error);
        goto cleanup1;
    }
    //rename the temp file to be the new file
    fclose(newFile);
    if(rfRenameFile(RFS_(tempFName),&t->name) != RF_SUCCESS)
    {
        error = RE_FILE_RENAME;
        LOG_ERROR("After the insertion operation the temporary file used, could not be renamed to the original file",error);
        goto cleanup1;
    }
    t->f = fopen(t->name.bytes,"r"i_PLUSB_WIN32"+");
    //if the line was inserted before this line then this line should be + the number of lines the string has
    if(lineN<t->line)
    {
        t->line+=linesCount;
    }
    if(allocatedS == true)
        rfString_Destroy(string);
    TEXTFILE_RESETPTR(t,lineN,false,tOff)//go to the beginning of the inserted lines

    ///success
    RF_EXIT_LOCAL_SCOPE()
    return RF_SUCCESS;

    ///cleanup code
cleanup3:
    rfStringX_Deinit(&buffer);
cleanup2:
    fclose(newFile);
    rfDeleteFile(RFS_(tmpNamePtr));
cleanup1:
    if(allocatedS == true)
        rfString_Destroy(string);
cleanup0:
    RF_EXIT_LOCAL_SCOPE()
    return error;
}


//Removes a specific line from the text file
int32_t rfTextFile_Remove(RF_TextFile* t,uint64_t lineN)
{
    char tempFName[L_tmpnam+1], *tmpNamePtr;
    char prEof,lineFound;
    int32_t error;
    foff_rft prOff;
    uint64_t prLine;
    FILE* newFile;
    RF_StringX buffer;
    lineFound = false;
    //determine the target line
    if(lineN == 0)
        return RF_FAILURE;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    prEof = t->eof;
    if((prOff = rfFtell(t->f)) == (foff_rft)-1)
    {
       i_FTELL_CHECK("Querying current file pointer at function start")
    }
    //go to the beginning of this file
    TEXTFILE_GOTOSTART(t,"Resetting the file pointer") ///Go back to the staring file position -- cleanup1
    //check if this file can read
    RF_TEXTFILE_CANREAD_GOTO(t,error,cleanup1)
    t->previousOp = RF_FILE_READ;
    //now open another temporary file
    tmpNamePtr = tmpnam(tempFName);
    newFile = fopen(tmpNamePtr,"w"i_PLUSB_WIN32); ///need to cleanup the temporary file -- cleanup2
    if(t->hasBom)
    {
        if((error=i_rfTextFile_AddBom(newFile,t))!= RF_SUCCESS)
        {
            LOG_ERROR("Failed to add BOM to the newly created temporary file",error);
            goto cleanup2;
        }
    }
    //initialize a string reading buffer
    rfStringX_Init_buff(&buffer,RF_OPTION_FGETS_READBYTESN+1,""); ///Need to clean the buff string -- cleanup3
    //read every line of this file from the beginning
    while((error =rfTextFile_ReadLine2(t,&buffer))==RF_SUCCESS)
    {
        //only if this line is not the one to remove write the line to the temporary file
        if(t->line-1 != lineN)//-1 is since right after the ReadLine2 function the current line changes
        {
#ifdef RF_NEWLINE_CRLF
            if((error = rfString_Fwrite(RFS_("%s\xD\n",rfString_Cstr(&buffer)),newFile,t->encoding))!= RF_SUCCESS)
#else
            if((error = rfString_Fwrite(RFS_("%s\n",rfString_Cstr(&buffer)),newFile,t->encoding))!= RF_SUCCESS)
#endif
            {
                LOG_ERROR("While attempting to remove line [%"PRIu64"] of TextFile \"%s\" a write error occured",error,lineN,t->name.bytes);
                goto cleanup3;
            }
        }
        else//found the line for removal, don't copy in the new file. Keep its size in bytes
            lineFound = true;

    }
    //get rid of the no longer needed buffer
    rfStringX_Deinit(&buffer); ///no need to clean this up anymore later, back to cleanup2
    // if we reached the end of file and the line was not found
    if(lineFound==false)
    {
        if(error == RE_FILE_EOF)
        {
            LOG_ERROR("While attempting to find line [%"PRIu64"] of TextFile \"%s\" for removal premature End Of File was encountered",RE_FILE_EOF,lineN,t->name.bytes);
            goto cleanup2;
        }//or else if there was an error
        else
        {
            LOG_ERROR("While attempting to find line [%"PRIu64"] of TextFile \"%s\" for removal a file reading error was encountered",error,lineN,t->name.bytes);
            goto cleanup2;
        }
    }
    //delete the old file
    fclose(t->f);
    if(rfDeleteFile(&t->name) != RF_SUCCESS)
    {
        error = RE_FILE_DELETE;
        LOG_ERROR("After the removal operation the temporary file used, could not be deleted",error);
        goto cleanup2;//since the file closes already
    }
    //rename the temp file to be the new file
    fclose(newFile); /// no need to clean this up anymore, back to cleanup1
    if(rfRenameFile(RFS_(tempFName),&t->name) != RF_SUCCESS)
    {
        error = RE_FILE_RENAME;
        LOG_ERROR("After the removal operation the temporary file used, could not be renamed to the original file",error);
        goto cleanup1;
    }
    t->f = fopen(t->name.bytes,"r"i_PLUSB_WIN32"+");
    //if the line was removed before this line then we should reduce the current line count
    t->line = prLine;
    t->eof = prEof;
    if(lineN<t->line)
    {
        t->line--;
    }
    //get the file position to the proper place
    TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
    ///success
    return RF_SUCCESS;

    ///cleanup
cleanup3:
    rfStringX_Deinit(&buffer);
cleanup2:
    fclose(newFile);
    rfDeleteFile(RFS_(tmpNamePtr));
cleanup1:
    TEXTFILE_RESETPTR_FROMSTART(t,prLine,prEof,prOff)
    return error;
}
//Replaces a line of the textfile with the given string
int32_t rfTextFile_Replace(RF_TextFile* t,uint64_t lineN,void* stringP)
{
    char tempFName[L_tmpnam+1], *tmpNamePtr;
    char lineFound,allocatedS;
    int32_t error = RF_FAILURE;
    FILE* newFile;
#ifdef RF_NEWLINE_CRLF
    uint32_t linesCount;
#endif
    RF_StringX buffer;
    foff_rft tOff;
    RF_String* string = (RF_String*)stringP;
    RF_ENTER_LOCAL_SCOPE()

    lineFound = allocatedS = false;
    //determine the target line
    if(lineN == 0)
        goto cleanup0;//failure

#ifdef RF_NEWLINE_CRLF
    //determine how many lines the given string has
    linesCount = rfString_Count(string,RFS_("\n"),0);
    /// cleanup 1 - For this string
    //if we consider CRLF the file line ending then change all newlines to CRLF
    //making a new one since stringP can be on the local stack and we can't use replace since that would act on the local stack
    if(linesCount>0)
    {
        allocatedS = true;
        string = rfString_Copy_OUT((RF_String*)stringP);
        rfString_Replace(string,RFS_("\n"),RFS_("\xD\n"),0,0);
    }
#endif
    //go to the beginning of this file
    TEXTFILE_GOTOSTART_GOTO(t,"Resetting the file pointer",error,cleanup1); ///cleanup2 - For the moving back of the file pointer to where it was in the function's beginning
    //check if this file can read
    RF_TEXTFILE_CANREAD_GOTO(t,error,cleanup1)
    t->previousOp = RF_FILE_READ;
    //now open another temporary file
    tmpNamePtr = tmpnam(tempFName);///cleanup2 - For the removal and deallocation of the temporary file
    newFile = fopen(tmpNamePtr,"w"i_PLUSB_WIN32);
    if(t->hasBom)
    {
        if((error=i_rfTextFile_AddBom(newFile,t))!= RF_SUCCESS)
        {
            LOG_ERROR("Failed to add BOM to the newly created temporary file",error);
            goto cleanup2;
        }
    }
    //initialize a string reading buffer
    rfStringX_Init_buff(&buffer,RF_OPTION_FGETS_READBYTESN+1,"");///cleanup3- For the deinit of the string buffer
    //read every line of this file from the beginning;
    while((error =rfTextFile_ReadLine2(t,&buffer))==RF_SUCCESS)
    {
        //write the read line to the other file if this is not the line to replace
        if(t->line-1 != lineN)//-1 is since right after the ReadLine2 function the current line changes
        {
#ifdef RF_NEWLINE_CRLF
            if((error = rfString_Fwrite(RFS_("%s\xD\n",rfString_Cstr(&buffer)),newFile,t->encoding))!= RF_SUCCESS)
#else
            if((error = rfString_Fwrite(RFS_("%s\n",rfString_Cstr(&buffer)),newFile,t->encoding))!= RF_SUCCESS)
#endif
            {
                LOG_ERROR("While attempting to replace line [%"PRIu64"] of TextFile \"%s\" a write error occured",error,lineN,t->name.bytes);
                goto cleanup3;
            }
        }
        else//else this is the line to replace so..
        {
            lineFound = true;
            if((tOff = rfFtell(newFile))== (foff_rft)-1)//get the file offset that we are going to come back to in success
            {
                i_FTELL_CHECK_GOTO("Reading the file position",error,cleanup3);
            }
#ifdef RF_NEWLINE_CRLF
            if((error = rfString_Fwrite(RFS_("%s\xD\n",string->bytes),newFile,t->encoding))!= RF_SUCCESS)
#else
            if((error = rfString_Fwrite(RFS_("%s\n",string->bytes),newFile,t->encoding))!= RF_SUCCESS)
#endif
            {
                LOG_ERROR("While attempting to replace line [%"PRIu64"] of TextFile \"%s\" a write error occured",error,lineN,t->name.bytes);
                goto cleanup3;
            }
        }
    }
    //get rid of the no longer needed buffer
    rfStringX_Deinit(&buffer);///back to cleanup 2
    // if we reached the end of file and the line was not found
    if(lineFound == false)
    {
        if(error == RE_FILE_EOF)
        {
            LOG_ERROR("While attempting to find line [%"PRIu64"] of TextFile \"%s\" for replacement, premature End Of File was encountered",RE_FILE_EOF,lineN,t->name.bytes);
            goto cleanup2;
        }//or else if there was an error
        else
        {
            LOG_ERROR("While attempting to find line [%"PRIu64"] of TextFile \"%s\" for replacement, a file reading error was encountered",error,lineN,t->name.bytes);
            goto cleanup2;
        }
    }
    //delete the old file
    fclose(t->f);
    if(rfDeleteFile(&t->name) != RF_SUCCESS) ///back to cleanup2
    {
        error = RE_FILE_DELETE;
        LOG_ERROR("After the replacement operation the temporary file used, could not be deleted",error);
        goto cleanup1;//since the file closed already
    }
    //rename the temp file to be the new file
    fclose(newFile);
    if(rfRenameFile(RFS_(tempFName),&t->name) != RF_SUCCESS)
    {
        error = RE_FILE_RENAME;
        LOG_ERROR("After the replacement operation the temporary file used, could not be renamed to the original file",error);
        goto cleanup1;
    }
    t->f = fopen(t->name.bytes,"r"i_PLUSB_WIN32"+");
    if(allocatedS == true)
        rfString_Destroy(string);
    //get the file pointer to the beginning of the newly replaced line
    TEXTFILE_RESETPTR_FROMSTART(t,lineN,false,tOff);

    ///success
    RF_EXIT_LOCAL_SCOPE()
    return RF_SUCCESS;
    ///cleanup code
cleanup3:
    rfStringX_Deinit(&buffer);
cleanup2:
    fclose(newFile);
    rfDeleteFile(RFS_(tmpNamePtr));
cleanup1:
    if(allocatedS == true)
        rfString_Destroy(string);
cleanup0:
    RF_EXIT_LOCAL_SCOPE()
    return error;
}


//Gets the current byte offset of the file
int32_t rfTextFile_GetOffset(RF_TextFile* t,foff_rft* offset)
{
    if(((*offset) = rfFtell(t->f)) == (foff_rft)-1)
    {
        i_FTELL_CHECK("Retrieving the current file offset")
    }
    return RF_SUCCESS;
}
