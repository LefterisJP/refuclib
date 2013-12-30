/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/


/*------------- Corrensponding Header inclusion -------------*/
#include <IO/rf_textfile.h>
/*------------- Module related inclusion -------------*/
#include <IO/rf_file.h> //for all the IO functions
#include "rf_textfile.ph"//for the private textfile functionality
/*------------- Outside Module inclusion -------------*/
#include <String/rf_str_common.h>//for RFS_()
#include <String/rf_str_core.h> //for rf_string_destroy(),and copying functions
#include <String/rf_str_corex.h> //for rf_stringx_assign functions
#include <String/rf_str_retrieval.h> //for rf_string_count() and others
#include <String/rf_str_manipulation.h>
#include <String/rf_str_filesx.h> //for rfStringX file functions
#include <String/rf_str_files.h> //for rfStringX file functions
#include <String/rf_str_traversalx.h> //for rf_stringx_reset()
#include <Utils/rf_unicode.h> //for rfReadLine family of functions
#include "../String/rf_str_conversion.ph" //for rf_string_cstr with buffer

#include <Utils/endianess.h> //for the endianess stuff
#include <Utils/log.h> //for error logging
#include <Utils/memory.h> //for refu memory allocation
#include <System/rf_system.h> //for System functions
#include <Utils/constcmp.h> //for RF_HEXEQ_I() macro and others
#include <Utils/localscope.h> //for the local scope macros
#include <Utils/sanity.h> //for the sanity check macros
/*------------- libc inclusion --------------*/
#include <errno.h>
/*------------- End of includes -------------*/

static const char BOM_UTF8[3] = {0xEF, 0xBB, 0xBF};
static const char BOM_UTF16_LE[2] = {0xFF, 0xFE};
static const char BOM_UTF16_BE[2] = {0xFE, 0xFF};
static const char BOM_UTF32_LE[4] = {0xFF, 0xFE, 0, 0};
static const char BOM_UTF32_BE[4] = {0, 0, 0xFE, 0xFF};

/**
 ** @brief Writes string to a file appending the proper end of line
 */
static inline bool write_to_file_eol(FILE* f, struct RFtextfile *t,
                                     struct RFstring* str)
{
    char ret;
    if(t->eol == RF_EOL_CRLF)
    {
        ret = rf_string_fwrite(RFS_(RF_STR_PF_FMT"\xD\n", RF_STR_PF_ARG(str)),
                              f, t->encoding, t->endianess);
    }
    else if(t->eol == RF_EOL_CR)
    {
        ret = rf_string_fwrite(RFS_(RF_STR_PF_FMT"\xD", RF_STR_PF_ARG(str)),
                              f, t->encoding, t->endianess);
    }
    else
    {
        ret = rf_string_fwrite(RFS_(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(str)),
                              f, t->encoding, t->endianess);
    } 
    return ret;
}

/* Takes a textfile's inner FILE* to the beginning of the file depending
   on the encoding and BOM existence. Can fail and returns an appropriate error*/
static inline bool goto_filestart(struct RFtextfile* t)
{
    //depending on the encoding of the file
    int byteOffset = 0;
    switch(t->encoding)
    {
        case RF_UTF8:
         if(t->hasBom == true)
            byteOffset = 3;
        break;
        case RF_UTF16:
            if(t->hasBom == true)
                byteOffset = 2;
        break;
        case RF_UTF32:
            if(t->hasBom == true)
                byteOffset = 4;
        break;
    }
    //First rewind back to the start so that read/write operations can be reset
    if(rfFseek(t->f, 0, SEEK_SET) != 0)
    {
        RF_ERROR("Rewinding to the beginning "
                 "of a TextFile's inner file pointer failed due to "
                 "fseek() fail with errno %d", errno);
        return false;
    }
    if(rfFseek(t->f,byteOffset,SEEK_SET) != 0)
    {
        RF_ERROR("Attempting to go over the BOM of a "
                 "TextFile failed due to fseek() failure with errno %d",
                 errno);
        return false;
    }
    t->previousOp = 0;
    t->line = 1;
    t->eof = false;
    return true;
}

/* Handles the EOL encoding for this textfile by either setting the desired
   encoding pattern or auto detecting it */
static bool handle_EOL(struct RFtextfile* t, enum RFeol_mark eol)
{
    uint32_t c,n;
    char ret=true;
    char eof_reached;
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
            RF_ERROR("An illegal eol value has been given to the "
                     "initialization of TextFile \""RF_STR_PF_FMT"\"",
                     RF_STR_PF_ARG(&t->name));
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
                   ret = rf_file_read_char_utf8(t->f,&c,true, &eof_reached);
               break;
               case RF_UTF16:
                   ret = rf_file_read_char_utf16(t->f,&c,true, t->endianess, &eof_reached);
               break;
               case RF_UTF32:
                   ret = rf_file_read_char_utf32(t->f, &c, t->endianess, &eof_reached);
               break;
            }
            //check for reading problem
            if(ret == -1 || eof_reached)
            {
                break;
            }

            //if you find a carriage return
            if(c == RF_CR)
            {   //check the next character and decide the EOL pattern accordingly
                switch(t->encoding)
                {
                   case RF_UTF8:
                       ret = rf_file_read_char_utf8(t->f,&n,true, &eof_reached);
                   break;
                   case RF_UTF16:
                       ret = rf_file_read_char_utf16(t->f, &n, true, t->endianess,
                                           &eof_reached);
                   break;
                   case RF_UTF32:
                       ret = rf_file_read_char_utf32(t->f, &n, t->endianess,
                                           &eof_reached);
                   break;
                }
                //check for reading problem
                if(ret == -1 || eof_reached)
                {
                    break;
                }

                t->eol = RF_EOL_CR;
                if(n == RF_LF)
                {
                    t->eol = RF_EOL_CRLF;
                }
                //break from the reading loop
                break;
            }
            else if(c == RF_LF)//else if we find a line feed character without
            {                   //having found a CR then it should be the 
                               //default case of Unix style endings
                break;
            }
        }while(ret >= 0 );//end of reading loop
        
        //if we got here without ret being false but with eof make sure to note
        if(eof_reached)
        {
            ret = false;
        }

        //and now go back to the beginning of the text file
        if(!(goto_filestart(t)))
        {
            RF_ERROR("Failed to move the internal file pointer to the "
                     "beginning after EOL pattern auto-detection");
            ret=false;
        }
    }
    return ret;
}




/* Adds a Byte order mark to the file at the current position.
   Only to be used at the start of the file */
static bool add_BOM(FILE* f, enum RFtext_encoding encoding,
                    enum RFendianess endianess)
{
#define FWRITE_FAIL(i_msg_)                                             \
    do{                                                                 \
        if(ferror(f) == 0)                                              \
        {                                                               \
            RF_ERROR("While adding a BOM to a file EOF was encountered"); \
            return false;                                               \
        }                                                               \
        else                                                            \
        {                                                               \
            RF_ERROR(i_msg_" because fwrite() failed with errno: %d", errno); \
            return false;                                               \
        }                                                               \
    }while(0)


                    switch(encoding)
    {
        case RF_UTF8:
            if(fwrite(BOM_UTF8, 1, 3, f) != 3)
            {
                FWRITE_FAIL("Failed to add BOM to a UTF8 file");
            }
        break;
        case RF_UTF16:
            if(endianess == RF_LITTLE_ENDIAN)
            {
                if(fwrite(BOM_UTF16_LE, 1, 2, f) != 2)
                {
                    FWRITE_FAIL("Failed to add BOM to a UTF16 little endian "
                                " file");
                }
            }
            else
            {
                if(fwrite(BOM_UTF16_BE, 1, 2, f) != 2)
                {
                    FWRITE_FAIL("Failed to add BOM to a UTF16 big endian "
                                " file");
                }
            }
        break;
        case RF_UTF32:
            if(endianess == RF_LITTLE_ENDIAN)
            {
                if(fwrite(BOM_UTF32_LE, 1, 4, f) != 4)
                {
                    FWRITE_FAIL("Failed to add BOM to a UTF32 little endian "
                                " file");
                }
            }
            else
            {
                if(fwrite(BOM_UTF32_BE, 1, 4, f) != 4)
                {
                    FWRITE_FAIL("Failed to add BOM to a UTF32 big endian "
                                " file");
                }
            }
        break;
    }
    return true;
#undef FWRITE_FAIL
}

static bool check_BOM_UTF8(struct RFtextfile* t, enum RFendianess endianess)
{
    uint32_t c;
    char eof;
    if(!rf_file_read_char_utf8(t->f, &c, false, &eof))
    {
        return false;
    }
    if(eof)
    {
        RF_ERROR("Encountered EOF at the beginning of a file "
                 "while searching for a Byte Order mark");
        return false;
    }
    if(c == 0xEFBBBF || c == 0xBFBBEF)
    {//some times the BOM in UTF-8 can be reverse .. but why?
        t->hasBom = true;
    }
    //now go back
    if(rfFseek(t->f, 0, SEEK_SET) != 0)
    {
        RF_ERROR("Problem when going to the file's "
                 "beginning for a UTF-8 file due to fseek() failure "
                 "with errno %d", errno);
        return false;
    }
    t->encoding = RF_UTF8;
    t->endianess = rf_system_get_endianess();
    return true;
}

static bool check_BOM_UTF16(struct RFtextfile* t, enum RFendianess endianess)
{
    //search for the BOM
    uint16_t c;
    if(fread(&c, 2, 1, t->f) != 1)
    {
        if(ferror(t->f) == 0)
        {
            RF_ERROR("Attempted to open an empty UTF-16 text file "
                     " for reading");
            return false;
        }
        RF_ERROR("Error while opening a UTF-16 text file due to fread() "
                 "failing with errno %d", errno);
        return false;
    }
    if(c == (uint16_t)0xFEFF)
    {
        t->encoding = RF_UTF16;
        t->hasBom = true;
        t->endianess = RF_BIG_ENDIAN;


        if(endianess != t->endianess &&
           endianess != RF_ENDIANESS_UNKNOWN)
        {
            RF_WARNING("The given endianess for Textfile \""RF_STR_PF_FMT"\" "
                       "does not match the one detected by the BOM.",
                       RF_STR_PF_ARG(&t->name));
        }
    }
    else if(c == (uint16_t)0xFFFE)
    {
        t->encoding = RF_UTF16;
        t->hasBom = true;
        t->endianess = RF_LITTLE_ENDIAN;
        
        if(endianess != t->endianess &&
           endianess != RF_ENDIANESS_UNKNOWN)
        {
            RF_WARNING("The given endianess for Textfile \""RF_STR_PF_FMT"\" "
                       "does not match the one detected by the BOM",
                       RF_STR_PF_ARG(&t->name));            
        }
    }
    return true;
}

static bool check_BOM_UTF32(struct RFtextfile* t, enum RFendianess endianess)
{
    //search for the BOM
    uint32_t c;
    if(fread(&c, 4, 1, t->f) != 1)
    {
        if(ferror(t->f) == 0)
        {
            RF_ERROR("Attempted to open an empty UTF-32 text file "
                     " for reading");
            return false;
        }
        RF_ERROR("Error while opening a UTF-32 text file due to fread() "
                 "failing with errno %d", errno);
        return false;
    }
    if(c == (uint32_t)0xFEFF)
    {
        t->encoding = RF_UTF32;
        t->hasBom = true;
        t->endianess = RF_BIG_ENDIAN;

        if(endianess != t->endianess &&
           endianess != RF_ENDIANESS_UNKNOWN)
        {
            RF_WARNING("The given endianess for Textfile \""RF_STR_PF_FMT"\" "
                       "does not match the one detected by the BOM",
                       RF_STR_PF_ARG(&t->name));
        }
    }
    else if(c == (uint32_t)0xFFFE0000)
    {
        t->encoding = RF_UTF32;
        t->hasBom = true;
        t->endianess = RF_LITTLE_ENDIAN;
        
        if(endianess != t->endianess &&
           endianess != RF_ENDIANESS_UNKNOWN)
        {
            RF_WARNING("The given endianess for Textfile \""RF_STR_PF_FMT"\" "
                       "does not match the one detected by the BOM",
                       RF_STR_PF_ARG(&t->name));            
        }
    }
    return true;
}


static bool scan_for_space_UTF16(struct RFtextfile* t)
{
    uint32_t c, bytesN=0;
    //go back to start
    if(rfFseek(t->f, -2, SEEK_CUR) != 0)
    {
        RF_ERROR("Problem when going to the file's "
                 "beginning for a UTF-16 file due to fseek() "
                 "failure with errno %d", errno);
        return false;
    }
    //scan for a space character
    while(1)
    {
        if(fread(&c, 2, 1, t->f) != 1)
        {
            if(ferror(t->f) == 0)
            {
                RF_ERROR("EOF found very close to a UTF-16 "
                         "file's beginning while attempting to "
                         "determine endianess");
                return false;
            }
            RF_ERROR("Error at reading a UTF-16 while "
                     "trying to determine endianess due to fread failing "
                     "with errno %d", errno);
            return false;
        }
        bytesN+=2;
        if(c == (uint16_t) 0x0020)
        {
            t->encoding = RF_UTF16;
            t->endianess = RF_BIG_ENDIAN;
            break;
        }
        else if(c == (uint16_t) 0x2000)
        {
            t->encoding = RF_UTF16;
            t->endianess = RF_LITTLE_ENDIAN;
            break;
        }
        //else keep reading
    }//end of scanning
    return true;
}


static bool scan_for_space_UTF32(struct RFtextfile* t)
{
    uint32_t c, bytesN=0;
    //go back to start
    if(rfFseek(t->f, -4, SEEK_CUR) != 0)
    {
        RF_ERROR("Problem when going to the file's "
                 "beginning for a UTF-32 file due to fseek() failing "
                 "with errno %d", errno);
        return false;
    }
    //scan for a space character
    while(1)
    {
        if(fread(&c, 4, 1, t->f) != 1)
        {
            if(ferror(t->f) == 0)
            {
                RF_ERROR("EOF found very close to a UTF-32 "
          
                         "text file's beginning while attempting "
                         "to scan for endianess");
                return false;
            }
            RF_ERROR("Error while opening a UTF-32 text "
                     "file to determine endianess due to fread() failing with "
                     "errno %d", errno);
            return false;
        }
        bytesN+=4;
        if(c == (uint32_t) 0x20)
        {
            t->encoding = RF_UTF32;
            t->endianess = RF_BIG_ENDIAN;
            break;
        }
        else if(c == (uint32_t) 0x20000000)
        {
            t->encoding = RF_UTF32;
            t->endianess = RF_LITTLE_ENDIAN;
            break;
        }
        //else keep reading
    }//end of scanning
    return true;
}

/* Scans a textfile's beginning for a BOM or space character to determine endianess*/
static bool determine_endianess(struct RFtextfile* t,
                                enum RFtext_encoding encoding,
                                enum RFendianess endianess)
{
    //in case no detection works go with what we have in the beginning
    if(endianess != RF_ENDIANESS_UNKNOWN)
    {
        t->endianess = endianess;
    }
    t->encoding = encoding;
    switch(encoding)
    {
        case RF_UTF16://if we are reading the file in UTF-16 we need to find endianess
            //search for the BOM
            if(!check_BOM_UTF16(t, endianess))
            {
                return false;//error
            }
                
           if(!t->hasBom &&
              endianess == RF_ENDIANESS_UNKNOWN)//no BOM at beginning, scan the file for the space character 0x0020
            {
                if(!scan_for_space_UTF16(t))
                {
                    return false;
                }
            }//end of no BOM at beginning case
        break;
        case RF_UTF32://if we are reading the file in UTF-32 we need to find endianess

            if(!check_BOM_UTF32(t, endianess))
            {
                return false;//error
            }
            if(!t->hasBom &&
               endianess == RF_ENDIANESS_UNKNOWN)//no BOM at beginning, scan the file for the space character 0x0020
            {
                if(!scan_for_space_UTF32(t))
                {
                    return false;
                }
            }//end of no BOM at beginning case
        break;
        case RF_UTF8:
            if(!check_BOM_UTF8(t, endianess))
            {
                //no BOM, just take the system's endianess
                t->endianess = rf_system_get_endianess();
            }
        break;
        default:
            RF_ERROR(
                "Attempted to initialize TextFile \""RF_STR_PF_FMT"\" with "
                "illegal encoding parameter", RF_STR_PF_ARG(&t->name));
            rf_string_deinit(&t->name);
            return false;
            break;
    }//end of switch
    //after all operations go to the beginning of the file
    if(!goto_filestart(t))
    {
        return false;
    }
    return true;
}

/* --- Textfile Creation Functions --- */

bool rf_textfile_init(struct RFtextfile* t, const void* name,
                      enum RFtextfile_mode mode,
                      enum RFendianess endianess,
                      enum RFtext_encoding encoding,
                      enum RFeol_mark eol)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(t);

    if(!name)
    {
        RF_ERROR("Provided a null pointer for the file name");
        ret = false;
        goto cleanup1;
    }

    //save the name of the file
    if(!rf_string_copy_in(&t->name,name))
    {
        ret = false;
        RF_ERROR("Could not copy the file's \""RF_STR_PF_FMT"\" filename",
                 RF_STR_PF_ARG(&name));
        goto cleanup1;
    }
    t->hasBom = false;

    //depending on the mode open the file
    switch(mode)
    {
        case RF_FILE_WRITE:
            t->mode = RF_FILE_READWRITE;
            t->f = rf_fopen(name, "a"i_PLUSB_WIN32"+");
        break;
        case RF_FILE_READ:
            t->mode = RF_FILE_READ;
            t->f = rf_fopen(name, "r"i_PLUSB_WIN32);
        break;
        case RF_FILE_NEW:
            t->mode=RF_FILE_WRITE;
            t->f = rf_fopen(name, "w"i_PLUSB_WIN32);
        break;
        case RF_FILE_READWRITE:
            t->mode = RF_FILE_READWRITE;
            t->f = rf_fopen(name, "r"i_PLUSB_WIN32"+");
        break;
        case RF_FILE_READWRITE_NEW:
            t->mode = RF_FILE_READWRITE;
            t->f = rf_fopen(name, "w"i_PLUSB_WIN32"+");
        break;
        default:
            RF_ERROR("Attempted to initialize textfile \""RF_STR_PF_FMT"\" with "
                     "illegal mode", RF_STR_PF_ARG(name));
            rf_string_deinit(&t->name);
            ret = false;
            goto cleanup1;
        break;
    }//end of opening mode switch
    //if the file failed to open read errno
    if(!t->f)
    {
        RF_ERROR("Failed to open a textfile due to fopen() "
                 "with errno %d", errno);
        ret = false;
        goto cleanup1;
    }


    //for an existing file
    if(mode == RF_FILE_READ || mode == RF_FILE_READWRITE)
    {
        //scan the file for BOM to determine endianess if needed
        if(!determine_endianess(t, encoding, endianess))
        {
            RF_ERROR("Error while trying to determine the endianess of "
                     "text file \""RF_STR_PF_FMT"\"", RF_STR_PF_ARG(&t->name));
            ret = false;
            goto cleanup1;
        }
        //finally handle the line encoding for the existingfile case
        if(!handle_EOL(t, eol))
        {
            RF_ERROR(
                     "During initializing TextFile \""RF_STR_PF_FMT"\" "
                     "auto-detecting the "
                     "End Of Line Pattern failed. Considering default of "
                     "Unix-Style LF Endings", RF_STR_PF_ARG(&t->name));
            ret = false;
            goto cleanup1;
        }

    }//end of existing file case
    else//new file case
    {
        //check the eol argument
        switch(eol)
        {
            case RF_EOL_LF:
            case RF_EOL_CR:
            case RF_EOL_CRLF:
                t->eol = eol;
            break;
            case RF_EOL_AUTO://auto in new file case means default
                t->eol = RF_EOL_DEFAULT;
            break;
            default:
                RF_ERROR(
                         "An illegal eol value has been given to the "
                         "initialization of TextFile \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(&t->name));
                ret = false;
                goto cleanup1;
            break;
        }
        t->encoding = encoding;
        if(endianess == RF_ENDIANESS_UNKNOWN)
        {
            endianess = rf_system_get_endianess();
        }
        t->endianess = endianess;
        //add a BOM if required
#ifdef RF_OPTION_TEXTFILE_ADDBOM
        t->hasBom = true;
        ret = add_BOM(t);
#endif
    }//end of newfile case

    //init some values
    t->eof =false;
    t->line = 1;
    t->previousOp = 0;

cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
struct RFtextfile* rf_textfile_create(const void* name,
                                      enum RFtextfile_mode mode,
                                      enum RFendianess endianess,
                                      enum RFtext_encoding encoding,
                                      enum RFeol_mark eol)
{
    struct RFtextfile* ret;
    RF_ENTER_LOCAL_SCOPE();
    RF_MALLOC_JMP(ret, sizeof(*ret), ret = NULL, cleanup);
    if(!rf_textfile_init(ret, name, mode, endianess, encoding, eol))
    {
        free(ret);
        ret = NULL;
    }

#if defined(RF_OPTION_SAFE_MEMORY_ALLOCATION) || defined(RF_OPTION_DEBUG)
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

/* --- Textfile Copying Functions --- */

bool rf_textfile_copy_in(struct RFtextfile* dst, struct RFtextfile* src)
{
    fpos_t pos;
    RF_ASSERT(dst);

    if (!src) {
        RF_WARNING("Provided NULL pointer for the source textfile");
        return false;
    }
    //get the data
    dst->mode = src->mode;
    dst->encoding = src->encoding;
    dst->line = src->line;
    dst->eof = src->eof;
    dst->previousOp = src->previousOp;
    dst->hasBom = src->hasBom;
    dst->eol = src->eol;
    //open the same file with the same mode and at the same position
    if(src->mode == RF_FILE_WRITE)
    {
        if((dst->f = rf_fopen(&src->name, "a")) == NULL)
        {
            RF_ERROR(
                "During copying from Textfile \""RF_STR_PF_FMT"\""
                " the file could"
                " not be opened for writing due to fopen with errno %d",
                RF_STR_PF_ARG(&src->name), errno);
            return false;
        }
    }
    else if(src->mode == RF_FILE_READ)
    {
        if((dst->f = rf_fopen(&src->name, "r")) == NULL)
        {
            RF_ERROR(
                "During copying from Textfile \""RF_STR_PF_FMT"\""
                " the file could"
                " not be opened for reading due to fopen with errno %d",
                RF_STR_PF_ARG(&src->name), errno);
            return false;
        }
    }
    else
    {
        if((dst->f = rf_fopen(&src->name, "r+")) == NULL)
        {
            RF_ERROR(
                "During copying from Textfile \""RF_STR_PF_FMT"\""
                " the file could"
                " not be opened for appending due to fopen with errno %d",
                RF_STR_PF_ARG(&src->name), errno);
            return false;
        }
    }
    if(fgetpos(src->f, &pos) != 0)
    {
        RF_ERROR("Failed to get file position due to fgetpos() "
                 "with errno %d", errno);
        return false;
    }
    if(fsetpos(dst->f,&pos) != 0)
    {
        RF_ERROR("Failed to set a file position due to fsetpos() "
                 "with errno %d", errno);
        return false;
    }
    //copy the name
    if(!rf_string_copy_in(&dst->name, &src->name))
    {
        RF_ERROR("During copying from Textfile \""RF_STR_PF_FMT"\""
                 " the file's name "
                 "could not be copied",
                 RF_STR_PF_ARG(&src->name));
        return false;
    }
    return true;
}
struct RFtextfile* rf_textfile_copy_out(struct RFtextfile* src)
{
    struct RFtextfile* dst;
    RF_MALLOC(dst, sizeof(*dst), NULL);
    if(!rf_textfile_copy_in(dst, src))
    {
        free(dst);
        dst = NULL;
    }
    return dst;
}

/* --- Textfile Destruction Functions --- */

void rf_textfile_deinit(struct RFtextfile* t)
{
    fclose(t->f);
    rf_string_deinit(&t->name);
}

void rf_textfile_destroy(struct RFtextfile* t)
{
    fclose(t->f);
    rf_string_deinit(&t->name);
    free(t);
}

/* --- Textfile Conversion Functions --- */

bool rf_textfile_set_mode(struct RFtextfile* t, enum RFtextfile_mode mode)
{
    FILE* temp;
    switch(mode)
    {
        case RF_FILE_WRITE:
            if(t->mode == RF_FILE_WRITE)
            {
                return true;
            }
            if((temp = rf_freopen(&t->name, "a", t->f)) == 0)
            {
                RF_ERROR("Changing the file access to write mode failed "
                         "due to freopen() with errno %d", errno);
                return false;
            }
            t->f = temp;
        break;
        case RF_FILE_READ:
            if(t->mode == RF_FILE_READ)
            {
                return true;
            }
            if((temp = rf_freopen(&t->name, "r", t->f)) == 0)
            {
                RF_ERROR("Changing the file access to read mode failed "
                         "due to freopen() with errno %d", errno);
                return false;
            }
            t->f = temp;
        break;
        case RF_FILE_READWRITE:
            if(t->mode == RF_FILE_READWRITE)
            {
                return true;
            }
            if((temp = rf_freopen(&t->name, "r+", t->f)) == 0)
            {
                RF_ERROR("Changing the file access to read mode failed "
                         "due to freopen() with errno %d", errno);
                return false;
            }
            t->f = temp;
        break;
        default:
            RF_ERROR("Provided illegal mode input");
            return false;
        break;
    }
    //success
    return true;
}

/* --- Textfile Traversal Functions --- */

int32_t rf_textfile_move_lines(struct RFtextfile* t,
                               int64_t linesN)
{
    uint64_t prLine;
    uint32_t i;
    RFfile_offset prOff;
    int32_t error;
    uint64_t targetLine;
    char prEof;
    struct RFstringx buffer;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    prEof = t->eof;
    if((prOff = rfFtell(t->f)) == (RFfile_offset)-1)
    {
        RF_ERROR("Querying current file pointer at function start failed "
                 "due to ftell() with errno %d", errno);
        return -1;
    }
    //now depending on the sign of linesN
    ///positive case///
    if(linesN>0)
    {
        char success = false;
        targetLine = prLine+linesN;
        if(!rf_stringx_init_buff(&buffer,RF_OPTION_FGETS_READ_BYTESN,""))
        {
            RF_ERROR("Initialization of the line buffer string failed");
            return -1;
        }
        //in the positive case the operation is easy, just read the lines until we get to the required one
        while((error = rf_textfile_read_line(t,&buffer)) == RF_SUCCESS)
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
            TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, -1);
            //else there was an error at line reading
            if(error != RE_FILE_EOF)
            {
                RF_ERROR(
                         "While reading Text File's lines forward "
                         "\""RF_STR_PF_FMT"\" "
                         "there was an error in file reading. File must be "
                         "corrupt",  RF_STR_PF_ARG(&t->name));
                return -1;
            }
            //if it was EOF just return it
            return RE_FILE_EOF;
        }
        // SUCCESS getting here means we are done
        rf_stringx_deinit(&buffer);
        return RF_SUCCESS;
    }
    ///-----------------------------------
    /// negative or 0 case starts
    ///-----------------------------------
    if( (uint64_t)(linesN*-1) >= prLine) //if we ask to go back before the start
    {
        RF_ERROR("Requested to go back before the beginning of the file");
        return RF_FAILURE;
    }

    targetLine = prLine+linesN;
    //since we will be going backwards anyway we can't be at the end of file, so falsify it in case it was true before
    t->eof = false;
    ///special case , target line is the very first one, or is closer to the beginning of the file than here
    if(targetLine == 1 || targetLine < prLine/2)
    {
        //now get the position to the beginning of the file
        if(!goto_filestart(t))
        {
            RF_ERROR(
                     "Failed to move the internal filepointer of TextFile "
                     "\""RF_STR_PF_FMT"\" to the beginning", RF_STR_PF_ARG(&t->name));
            return -1;
        }

        //read as many lines as needed
        if(rf_stringx_init_buff(&buffer,RF_OPTION_FGETS_READ_BYTESN,""))
        {
            RF_ERROR("Initialization of the line buffer string failed");
            return -1;
        }
        for(i=1; i<targetLine; i ++)
        {
            if((error = rf_textfile_read_line(t,&buffer)) != RF_SUCCESS)
            {
                //there was an error at line reading
                TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, -1);
                RF_ERROR(
                         "While reading Text File's lines forward "
                         "\""RF_STR_PF_FMT"\" "
                         "there was an error in file reading. File must be "
                         "corrupt", RF_STR_PF_ARG(&t->name));
                return -1;
            }

        }
        //deinit line buffer and success
        rf_stringx_deinit(&buffer);
        return RF_SUCCESS;
    }//end of special case
    ///start moving back by the required number of lines
    RF_TEXTFILE_CANREAD(t, -1);
    t->previousOp = RF_FILE_READ;
    while(t->line >= targetLine)
    {//start of moving back loop
        if((error=rf_textfile_move_chars_b(t,1)) != RF_SUCCESS)
        {
            RF_ERROR("Error while moving backwards in Text File"
                     " \""RF_STR_PF_FMT"\"", RF_STR_PF_ARG(&t->name));
            return -1;
        }

    }//end of moving back loop
    //go 1 char forward to be at the line's start
    if(rf_textfile_move_chars_f(t,1) < 0)
    {
        RF_ERROR("Error at moving 1 char forward in the textfile");
        return -1;
    }
    ///success here
    return RF_SUCCESS;
}

int32_t rf_textfile_move_chars(struct RFtextfile* t, int64_t charsN)
{
    //do nothing if charsN is 0
    if(charsN == 0)
    {
        RF_DEBUG("Provided a 0 value to the characters to move argument");
        return 0;
    }
    if(charsN > 0)
    {
        return rf_textfile_move_chars_f(t,(uint64_t)charsN);
    }
    return rf_textfile_move_chars_b(t,(uint64_t)(charsN*-1));
}

int32_t rf_textfile_move_chars_f(struct RFtextfile* t, uint64_t charsN)
{
    uint32_t c;
    uint64_t i;
    char eof_reached;
    //move charsN chars forward
    for(i=0; i<charsN; i ++)
    {
        //depending on the encoding
        switch(t->encoding)
        {
            case RF_UTF8:
                if(rf_file_read_char_utf8(t->f, &c,true, &eof_reached) < 0)
                {

                    RF_ERROR(
                        0, "There was an error while moving forward in "
                        "textfile \""RF_STR_PF_FMT"\"",
                        RF_STR_PF_ARG(&t->name));
                    return -1;
                }
            break;
            case RF_UTF16:
                if(rf_file_read_char_utf16(t->f, &c, true, t->endianess,
                                   &eof_reached) < 0)
                {
                    RF_ERROR(
                        0, "There was an error while moving forward in "
                        "textfile \""RF_STR_PF_FMT"\"",
                        RF_STR_PF_ARG(&t->name));
                    return -1;
                }
            break;
            case RF_UTF32:
                if(rf_file_read_char_utf32(t->f, &c, t->endianess, &eof_reached) < 0)
                {
                    RF_ERROR(
                        0, "There was an error while moving forward in "
                        "textfile \""RF_STR_PF_FMT"\"",
                        RF_STR_PF_ARG(&t->name));
                    return -1;
                }
        }//end of encoding switch
        //if it's a new line increase the line
        if(RF_HEXEQ_I(c,RF_LF))
        {
            t->line++;
        }
        if(eof_reached)
        {
            break;
        }
    }//end of for
    return i;
}

int32_t rf_textfile_move_chars_b(struct RFtextfile* t, uint64_t charsN)
{
    uint32_t c;
    uint64_t i;
    //move charsN chars back
    for(i=0; i < charsN; i ++)
    {
        //depending on the encoding
        switch(t->encoding)
        {
            case RF_UTF8:
                if(rf_file_move_back_char_utf8(t->f,&c) < 0)
                {
                    
                    RF_ERROR(
                             "There was an error while moving backwards "
                             "in textfile \""RF_STR_PF_FMT"\"",
                             RF_STR_PF_ARG(&t->name));
                    return -1;
                }
            break;
            case RF_UTF16:
                if(rf_file_move_back_char_utf16(t->f, &c, t->endianess) < 0)
                {

                    RF_ERROR(
                             "There was an error while moving backwards "
                             "in textfile \""RF_STR_PF_FMT"\"",
                             RF_STR_PF_ARG(&t->name));
                    return -1;
                }
            break;
            case RF_UTF32:
                if(rf_file_move_back_char_utf32(t->f,&c, t->endianess) < 0)
                {

                    RF_ERROR(
                             "There was an error while moving backwards "
                             "in textfile \""RF_STR_PF_FMT"\"",
                             RF_STR_PF_ARG(&t->name));
                        return -1;
                }
        }//end of encoding switch
        //if it's a new line decrease the line
        if(RF_HEXEQ_I(c,RF_LF))
        {
            t->line--;
        }
    }//end of for
    //also make sure eof is false, and finish
    t->eof = false;
    return i;
}

int32_t rf_textfile_go_to_line(struct RFtextfile* t, uint64_t lineN)
{
    if(lineN == 0)
    {
        RF_ERROR(
                 "Illegal linesN value was given. 0 is not a legal value");
        return -1;
    }

    if(lineN == 1)
    {
        //go to the file's beginning
        if(!goto_filestart(t))
        {
            RF_ERROR(
                     "Failed to move the internal filepointer of TextFile "
                     "\""RF_STR_PF_FMT"\" to the beginning",
                     RF_STR_PF_ARG(&t->name));
            return -1;
        }
        return RF_SUCCESS;
    }
    else if(lineN == t->line)
    {
        return RF_SUCCESS;
    }
    return rf_textfile_move_lines(t, lineN - t->line);
}

int32_t rf_textfile_go_to_offset(struct RFtextfile* t, RFfile_offset offset,
                                 int origin)
{
    uint64_t prLine;
    RFfile_offset prOff;
    char prEof;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    if((prOff = rfFtell(t->f)) == (RFfile_offset)-1)
    {
        RF_ERROR("Querying current file pointer at function start failed "
                 "due to ftell() with errno %d", errno);
        return -1;
    }
    prEof = t->eof;
    ///From the beginning Case
    if(origin == SEEK_SET)
    {
        RFfile_offset cOff;
        if(offset < 0)//illegal values combination
        {
            RF_ERROR("Illegal input to the function. Can't have negative "
                     "offset when starting from the file's beginning");
            return -1;
        }
        //go to the start of the file
        if(!goto_filestart(t))
        {
            RF_ERROR("Failed to move the internal filepointer of"
                     " TextFile \""RF_STR_PF_FMT"\" to the beginning",
                     RF_STR_PF_ARG(&t->name));
            return -1;
        }
        do
        {
            //keep going forward
            if(!rf_textfile_move_chars_f(t,1))
            {

                RF_ERROR(
                         "Moving forward in TextFile \""RF_STR_PF_FMT"\""
                         " failed ", RF_STR_PF_ARG(&t->name));
                return -1;
            }
            if(t->eof)
            {
                    RF_ERROR("Moving forward in Textfile "
                             "\""RF_STR_PF_FMT"\" failed "
                             "because EOF was encountered",
                             RF_STR_PF_ARG(&t->name));
                    return RE_FILE_EOF;
            }

            if((cOff = rfFtell(t->f)) == (RFfile_offset)-1)//get the current file position
            {
                RF_ERROR("Moving forward and querying file pointer failed "
                         "due to ftell() with errno %d", errno);
                return -1;
            }
        }while(cOff <offset);//as long as we are behind the required offset keep going
    }//end of SEEK_SET (from beginning case)
    else if(origin == SEEK_CUR)///from the current position case
    {
        RFfile_offset cOff,tOffset = prOff+offset;//the target offset
        if(offset == 0)
        {
            return RF_SUCCESS;
        }
        else if(offset > 0)
        {///positive case
            do
            {//keep going forward
                if(!rf_textfile_move_chars_f(t,1))
                {
                    TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, -1);

                    RF_ERROR(
                             "Moving forward in TextFile \""RF_STR_PF_FMT"\""
                             " failed ",
                             RF_STR_PF_ARG(&t->name));
                    return -1;
                }
                if(t->eof)
                {
                    RF_ERROR("Moving forward in Textfile \""RF_STR_PF_FMT"\""
                             " failed "
                             "because EOF was encountered",
                             RF_STR_PF_ARG(&t->name));
                    return RE_FILE_EOF;
                }
                if((cOff = rfFtell(t->f)) == (RFfile_offset)-1)//get the current file position
                {
                    RF_ERROR(
                        "Moving forward and querying file pointer failed due "
                        "to ftell() with errno %d", errno);
                    return -1;
                }
            }while(cOff <tOffset);//as long as we are behind the required offset keep going
        }
        else///negative case
        {
            if(tOffset < 0)//if we requeste to go beyond the file's start
            {
                RF_ERROR("Requested to go beyond the file's beginning");
                return -1;
            }
            do
            {//keep going backwards
                if(!rf_textfile_move_chars_b(t,1))
                {
                    TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, -1);
                    RF_ERROR(
                             "Moving backwards in TextFile \""RF_STR_PF_FMT"\""
                             " failed ",
                             RF_STR_PF_ARG(&t->name));
                    return -1;
                }
                if((cOff = rfFtell(t->f)) == (RFfile_offset)-1)//get the current file position
                {
                    RF_ERROR(
                        "Moving backwards and querying file pointer failed "
                        "due to ftell() with errno %d", errno);
                    return -1;
                }
            }while(cOff <offset);//as long as we are behind the required offset keep going
        }//end of the negative case
    }//end of the SEEK_CUR case
    else
    {
        RF_ERROR("Illegal value given for the origin argument. The legal "
                 "values are SEEK_SET and SEEK_CUR");
        return -1;
    }
    //also undo the eof flag if it was on since we moved
    if(t->eof == true)
    {
        t->eof = false;
    }
    //success
    return RF_SUCCESS;
}

int rf_textfile_read_line(struct RFtextfile* t, struct RFstringx* line)
{
    char eof = false;
    RFfile_offset startOff;

    //check if we can read from this textfile
    RF_TEXTFILE_CANREAD(t, -1);
    //set the file operation
    t->previousOp = RF_FILE_READ;
    //get the file position at start
    if((startOff = rfFtell(t->f))== (RFfile_offset)-1)
    {
        RF_ERROR("Reading the file position at function start failed "
                 "due to ftell() with errno %d", errno);
        return -1;
    }
    //check for eof
    if(t->eof == true)
    {
        return RE_FILE_EOF;
    }
    //make sure that the line StringX internal pointer is reset
    rf_stringx_reset(line);

    //Read the file depending on the encoding
    if(!rf_stringx_from_file_assign(line, t->f, &eof, t->eol, t->encoding, t->endianess))
    {
        RF_ERROR(
                 "Reading line [%llu] of a text file failed", t->line);
        if(rfFseek(t->f,startOff,SEEK_SET) != 0)
        {
            RF_ERROR("After a failed readline operation rewinding "
                     "the file pointer to its value before the function"
                     "'s execution failed due to fseek() errno %d",
                     errno);
        }
        return -1;        
    }
    //success
    t->line++;
    t->eof = eof;
    //also if the end of line was found make sure it's not included in the returned string
    if(rf_string_length_bytes(line) > 0 && 
       rf_string_data(line)[rf_string_length_bytes(line) - 1] == '\n')
    {
        rf_string_length_bytes(line)--;
    }
    return RF_SUCCESS;
}

/* --- Textfile Retrieval Functions --- */

int rf_textfile_read_line_chars(struct RFtextfile* t,
                                struct RFstringx* line,
                                uint32_t characters)
{
    RFfile_offset startOff;
    char eof = false;
    //check if we can read from this textfile
    RF_TEXTFILE_CANREAD(t, -1);
    //get the file offset at the function's start
    if((startOff = rfFtell(t->f))== (RFfile_offset)-1)
    {
        RF_ERROR("Reading the file position at function start failed "
                 "due to ftell() with errno %d", errno);
                
        return -1;
    }
    //set the operation
    t->previousOp = RF_FILE_READ;
    //check for eof
    if(t->eof == true)
    {
        return RE_FILE_EOF;
    }
    //make sure that the line StringX internal pointer is reset
    rf_stringx_reset(line);

    //Read the file depending on the encoding
    if(!rf_stringx_from_file_assign(line, t->f, &eof, t->eol, t->encoding, t->endianess))
    {
        RF_ERROR(
                 "Reading line [%llu] of a text file failed", t->line);
        if(rfFseek(t->f,startOff,SEEK_SET) != 0)
        {
            RF_ERROR("After a failed readline operation rewindind "
                     "the file pointer to its value before the function"
                     "'s execution failed due to fseek() errno %d", errno);
        }
        return -1;        
    }

    if(characters != 0)//if we need specific characters
    {
        uint32_t charsN = rf_string_length(line);
        rf_string_prune_end(line, charsN-characters, NULL);
    }
    //else if the end of line was found make sure it's not included in the returned string
    else if(rf_string_data(line)[rf_string_length_bytes(line) - 1] == '\n')
    {
        rf_string_length_bytes(line)--;
    }
    //success
    t->line++;
    t->eof = eof;
    return RF_SUCCESS;
}

bool rf_textfile_get_offset(struct RFtextfile* t, RFfile_offset* offset)
{
    if(((*offset) = rfFtell(t->f)) == (RFfile_offset)-1)
    {
        RF_ERROR("Retrieving the current file offset failed "
                 "due to ftell() with errno %d", errno);
        return false;
    }
    return true;
}

//Gets a specific line from a Text File starting from the beginning
int32_t rf_textfile_get_line_begin(struct RFtextfile* t, uint64_t lineN,
                                   struct RFstringx* line)
{
    uint64_t prLine,i=1;
    int32_t error;
    RFfile_offset prOff;
    char prEof;
    struct RFstringx buffer;
    //in the very beginning keep the previous file position
    prLine = t->line;

    if(lineN == 0)
    {
        RF_ERROR(
                 "Invalid input given for the lineN argument. It can never be"
                 " zero");
        return -1;
    }

    if((prOff = rfFtell(t->f)) == (RFfile_offset)-1)
    {
        RF_ERROR("Querying current file pointer at function start failed "
                 "due to ftell() with errno %d", errno);
        return -1;
    }
    prEof = t->eof;
    //now get the position to the beginning of the file
    if(!goto_filestart(t))
    {
        RF_ERROR(
                 "Failed to move the internal filepointer"
                 " of TextFile \""RF_STR_PF_FMT"\" "
                 "to the beginning", RF_STR_PF_ARG(&t->name));
        return -1;
        
    }

    ///since we got here start reading the file again, line by line until we 
    //get to the requested line. Also initialize the buffer string for readline
    if(!rf_stringx_init_buff(&buffer,RF_OPTION_FGETS_READ_BYTESN,""))
    {
        RF_ERROR("Failed to initialize the line buffer string");
        return -1;
    }
    //read all the lines until you get to the one we need
    while((error = rf_textfile_read_line(t,&buffer)) == RF_SUCCESS)
    {
        //if this is the line we need, assign it to the given string
        if(i==lineN)
        {
            if(rf_stringx_assign(line,&buffer) == false)
            {
                RF_ERROR("Failure at string assignment when reading lines "
                         "from Textfile \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(&t->name));
                return -1;
            }
            break;
        }
        i++;
    }
    //if we get here and we haven't reached the line
    if(i != lineN)
    {
        if(error == RE_FILE_EOF)
        {
            TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, -1);
            RF_ERROR(
                     "While searching for line [%llu] in Text File"
                     " \""RF_STR_PF_FMT"\" the"
                     " end of file was found before being able to reach that "
                     "line", lineN, RF_STR_PF_ARG(&t->name));
            return RE_FILE_EOF;
        }
        //else there was an error at line reading
        TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, -1);
        RF_ERROR(
                 "While reading Text File's lines "
                 "\""RF_STR_PF_FMT"\" there was an error "
                 "in file reading. File must be corrupt",
                 RF_STR_PF_ARG(&t->name));
        return -1;
    }
    //success
    //now we are done so we can return the line number and the file pointer to
    //their original positions, also free stuff
    rf_stringx_deinit(&buffer);
    TEXTFILE_RESETPTR(t, prLine, prEof, prOff, -1);
    return RF_SUCCESS;
}

int32_t rf_textfile_get_line(struct RFtextfile* t, uint64_t lineN,
                             struct RFstringx* line)
{
    uint64_t prLine;
    char prEof;
    RFfile_offset prOff;
    int32_t error;
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    if((prOff = rfFtell(t->f)) == (RFfile_offset)-1)
    {
        RF_ERROR("Querying current file pointer at function start failed "
                 "due to ftell() with errno %d", errno);
        return -1;
    }
    prEof = t->eof;
    //move to the beginning of the line and get it. If there was an error reset everything back
    if((error=rf_textfile_move_lines(t,lineN-t->line)) != RF_SUCCESS)
    {
        TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff,-1);
        if(error == RE_FILE_EOF)
        {
            RF_ERROR("Tried to move beyond the end of file for Textfile "
                     "\""RF_STR_PF_FMT"\" while requesting a line",
                     RF_STR_PF_ARG(&t->name));
            return error;
        }
        RF_ERROR("Error at getting a line for Textfile "
                 "\""RF_STR_PF_FMT"\"", RF_STR_PF_ARG(&t->name));
        return -1;
    }
    //and now get it
    if(rf_textfile_read_line(t,line) != RF_SUCCESS)
    {
        RF_ERROR("Error at getting the requested line for Textfile "
                 "\""RF_STR_PF_FMT"\"", RF_STR_PF_ARG(&t->name));
        return -1;
    }
    //success, so we can return the line number and the file pointer to their original positions
    TEXTFILE_RESETPTR(t, prLine, prEof, prOff, -1);
    return RF_SUCCESS;
}

/* --- Textfile Writting Functions --- */

bool rf_textfile_write(struct RFtextfile* t, void* s)
{
    int32_t linesN;
    char ret = true;
    char allocatedS = false;
    RF_ENTER_LOCAL_SCOPE();

    if(!s) {
        RF_WARNING("Provided a null pointer for the to-write string");
        ret = false;
        goto cleanup1;
    }

    //if the file mode is not write then turn it to writing
    RF_TEXTFILE_CANWRITE_JMP(t, ret = false, cleanup1);
    t->previousOp = RF_FILE_WRITE;
    //let's see how many lines it will be adding to the text file
    linesN = rf_string_count(s,RFS_("\n"),0);
    //if we don't have the default RFstring Unix style line ending
    if(t->eol != RF_EOL_LF && linesN != 0)
    {
        allocatedS = true;
        //making a new one since stringP can be on the local stack and we
        //can't use replace since that would act on the local stack
        s = rf_string_copy_out((struct RFstring*)s);
        if(t->eol==RF_EOL_CRLF)
        {
            if(!rf_string_replace(s, RFS_("\n"), RFS_("\xD\n"),0,0))
            {
                RF_ERROR("Failure at editing the newline character"
                         "while writing string \""RF_STR_PF_FMT"\""
                         " to Textfile \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(s), RF_STR_PF_ARG(&t->name));
                ret = false;
                goto cleanup1;
            }
        }
        else
        {
            if(!rf_string_replace(s, RFS_("\n"), RFS_("\xD"),0,0))
            {
                RF_ERROR("Failure at editing the newline character"
                         "while writing string \""RF_STR_PF_FMT"\""
                         " to Textfile \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(s), RF_STR_PF_ARG(&t->name));
                ret = false;
                goto cleanup1;
            }
        }
    }
    //depending on the encoding of the file
    if(!rf_string_fwrite(s, t->f, t->encoding, t->endianess))
    {
        RF_ERROR(
                 "There was a file write error while writting string"
                 " \""RF_STR_PF_FMT"\" "
                 "to Text File \""RF_STR_PF_FMT"\"",
                 RF_STR_PF_ARG(s), RF_STR_PF_ARG(&t->name));
        ret = false;
        if(allocatedS == true)
        {
            rf_string_destroy(s);
        }
        goto cleanup1;
    }
    t->line += linesN;//also add as many lines as were inside the string

cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


//Inserts a line into a specific part of the Text File
bool rf_textfile_insert(struct RFtextfile* t, uint64_t lineN, 
                        const void* stringIN, bool after)
{
    struct RFstring tempFileName;
    bool lineFound,allocatedS, ret = true;
    RFfile_offset tOff=0;
    FILE* newFile;
    int32_t linesCount;
    struct RFstringx buffer;
    struct RFstring *string;
    //get the function's arguments
    int32_t error;
    RF_ENTER_LOCAL_SCOPE();

    if (!stringIN) {
        RF_WARNING("Provided null pointer for insertion");
        ret = false;
        goto cleanup0;
    }

    lineFound = allocatedS = false;
    //determine the target line
    if(lineN == 0)
    {
        RF_ERROR("Illegal value of 0 was given for the line argument. Line "
                 "indexing starts from 1");
        ret = false;
        goto cleanup0;//failure
    }
    if(!after)
    {
        lineN-=1;
    }

    //determine how many lines the given string has

    linesCount = rf_string_count(stringIN, RFS_("\n"), 0) + 1;
    /// cleanup 1 - For the string
    //if we don't have the RFstring default Unix style line ending
    //making a new one since stringP can be on the local stack and we can't use replace since that would act on the local stack
    if(t->eol != RF_EOL_LF && linesCount > 1)
    {
        allocatedS = true;
        if(!(string = rf_string_copy_out(stringIN)))
        {
            RF_ERROR("Failure at making a copy of a string");
            ret = false;
            goto cleanup0;
        }
        if(t->eol == RF_EOL_CRLF)
        {
            if(!rf_string_replace(string, RFS_("\n"), RFS_("\xD\n"), 0, 0))
            {
                RF_ERROR("Failure at editing the newline character of string "
                         "\""RF_STR_PF_FMT"\" while inserting it into "
                         "Textfile \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(string), RF_STR_PF_ARG(&t->name));
                ret = false;
                goto cleanup1;
            }
        }
        else
        {
            if(!rf_string_replace(string, RFS_("\n"), RFS_("\xD"), 0, 0))
            {
                RF_ERROR("Failure at editing the newline character of string "
                         "\""RF_STR_PF_FMT"\" while inserting it into "
                         "Textfile \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(string), RF_STR_PF_ARG(&t->name));
                ret = false;
                goto cleanup1;
            }
        }
    } else {
        string = (struct RFstring *) stringIN;
    }

    //go to the beginning of this file
    if(!goto_filestart(t))
    {
        RF_ERROR(
                 "Failed to move the internal filepointer"
                 " of TextFile \""RF_STR_PF_FMT"\" to the beginning",
                 RF_STR_PF_ARG(&t->name));
        ret = false;
        goto cleanup1;
    }
    //check if this file can read
    RF_TEXTFILE_CANREAD_JMP(t, ret = false, cleanup1);
    t->previousOp = RF_FILE_READ;
    //now open another temporary file
    ///cleanup2 - is for temporary file Deletion
    if(!rf_string_initv(&tempFileName, RF_STR_PF_FMT"_temp",
                      RF_STR_PF_ARG(&t->name)))
    {
        RF_ERROR("Failure at string initialization for the temporary file");
        ret = false;
        goto cleanup1;
    }
    if(!(newFile = rf_fopen(&tempFileName, "w"i_PLUSB_WIN32)))
    {
        RF_ERROR("Opening a temporary file failed due to fopen() with "
                 "errno %d", errno);
        ret = false;
        goto cleanup1;
    }
    if(t->hasBom)
    {
        if(!add_BOM(newFile, t->encoding, t->endianess))
        {
            RF_ERROR(
                     "Failed to add BOM to the newly created temporary file");
            ret = false;
            goto cleanup2;
        }
    }
    //special case
    if(lineN==0)
    {
        lineFound = true;
        //get the file offset that we are going to come back to in success
        if((tOff = rfFtell(newFile))== (RFfile_offset)-1)
        {
            RF_ERROR("Reading the file position failed due to ftell() "
                     "with errno %d", errno);
            ret = false;
            goto cleanup2;
        }

        //write the given line to the other file according to eol
        ret = write_to_file_eol(newFile, t, string);
        //and check for errors
        if(!ret)
        {
            RF_ERROR("There was a file write error while inserting "
                     "string \""RF_STR_PF_FMT"\" at the beginning"
                     " of Text File "
                     "\""RF_STR_PF_FMT"\"",
                     RF_STR_PF_ARG(string), RF_STR_PF_ARG(&t->name));
            goto cleanup2;
        }
    }

    //initialize a string reading buffer
    if(!rf_stringx_init_buff(&buffer, RF_OPTION_FGETS_READ_BYTESN+1, ""))
    {
        RF_ERROR("Failure at string buffer initialization");
        ret = false;
        goto cleanup2;
    }
    //cleanup3 - for the string buffer deinitialization
    
    //read every line of this file from the beginning
    error = rf_textfile_read_line(t, &buffer);
    do
    {
        //write the given line to the other file according to eol
        ret = write_to_file_eol(newFile, t, &buffer.INH_String);
        //and check for errors
        if(!ret)
        {
            RF_ERROR("There was a file write error while inserting "
                     "string \""RF_STR_PF_FMT"\" at line [%llu] "
                     "inside Text File \""RF_STR_PF_FMT"\"",
                     RF_STR_PF_ARG(string), lineN, RF_STR_PF_ARG(&t->name));
            goto cleanup3;
        }
        //also if this is the place to put the line, do it
        if(t->line - 1 == lineN)
        //-1 is since right after the ReadLine function
        // the current line changes
        {
            lineFound = true;
            if((tOff = rfFtell(newFile)) == (RFfile_offset)-1)
            {
                //get the file offset that we are going to
                //come back to in success
                RF_ERROR("Reading the file position failed due to ftell() "
                         "with errno %d", errno);
                ret = false;
                goto cleanup3;
            }
            //write the given line to the other file according to eol
            ret = write_to_file_eol(newFile, t, string);
            //and check for errors
            if(!ret)
            {
                RF_ERROR("There was a file write error while inserting "
                         "string \""RF_STR_PF_FMT"\" at line [%llu] "
                         "inside Text File \""RF_STR_PF_FMT"\"",
                         RF_STR_PF_ARG(string), lineN, RF_STR_PF_ARG(&t->name));
                goto cleanup3;
            }
        }
        error = rf_textfile_read_line(t, &buffer);
    }while(error == RF_SUCCESS);
    //get rid of the no longer needed buffer
    rf_stringx_deinit(&buffer); /// going back to cleanup2
    // if we reached the end of file and the line was not found
    if(!lineFound)
    {
        if(error == RE_FILE_EOF)
        {
            RF_ERROR("While attempting to find line [%llu] of TextFile "
                     "\""RF_STR_PF_FMT"\" premature End Of File was encountered",
                     lineN, RF_STR_PF_ARG(&t->name));
            ret = false;
            goto cleanup2;
        }//or else if there was an error
        else
        {
            RF_ERROR("While attempting to find line [%llu] of TextFile "
                     "\""RF_STR_PF_FMT"\" a file reading error was encountered",
                     lineN, RF_STR_PF_ARG(&t->name));
            ret = false;
            goto cleanup2;
        }
    }
    //delete the old file
    fclose(t->f);
    if(!rf_system_delete_file(&t->name))
    {
        ret = false;
        RF_ERROR("After the insertion operation the temporary file used"
                 " could not be deleted");
        goto cleanup1;
    }
    //rename the temp file to be the new file
    fclose(newFile);
    if(!rf_system_rename_file(&tempFileName, &t->name))
    {
        ret = false;
        RF_ERROR("After the insertion operation the temporary file used"
                 ", could not be renamed to the original file");
        goto cleanup1;
    }
    //after renaming we no longer need the string
    rf_string_deinit(&tempFileName);
    if(!(t->f = rf_fopen(&t->name, "r"i_PLUSB_WIN32"+")))
    {
        RF_ERROR("Failed to open the edited file after insertion "
                 "due to fopen() with errno %d", errno);
        ret = false;
        goto cleanup1;
    }
    //if the line was inserted before this line then this line should be
    // + the number of lines the string has
    if(lineN<t->line)
    {
        t->line += linesCount;
    }
    if(allocatedS)
    {
        rf_string_destroy(string);
    }
    //go to the beginning of the inserted lines
    TEXTFILE_RESETPTR_JMP(t, lineN, false, tOff, ret = false, cleanup0);
    ///success, ret should be true
    goto cleanup0;

    ///cleanup code
cleanup3:
    rf_stringx_deinit(&buffer);
cleanup2:
    fclose(newFile);
    if(!rf_system_delete_file(&tempFileName))
    {
        RF_ERROR("Failed to delete file "RF_STR_PF_FMT" during cleanup",
                 RF_STR_PF_ARG(&tempFileName));
        ret = false;
    }
    rf_string_deinit(&tempFileName);
cleanup1:
    if(allocatedS)
    {
        rf_string_destroy(string);
    }
cleanup0:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


//Removes a specific line from the text file
bool rf_textfile_remove(struct RFtextfile* t, uint64_t lineN)
{
    struct RFstring tempFileName;
    char prEof, lineFound, ret = true;
    RFfile_offset prOff;
    uint64_t prLine;
    FILE* newFile;
    struct RFstringx buffer;
    int32_t error;
    lineFound = false;
    //determine the target line
    if(lineN == 0)
    {
        RF_ERROR("Illegal line number given. Line indexing starts from "
                 "zero.");
        return false;
    }
    //in the very beginning keep the previous file position and line number
    prLine = t->line;
    prEof = t->eof;
    if((prOff = rfFtell(t->f)) == (RFfile_offset)-1)
    {
        RF_ERROR("Querying current file pointer at function start failed "
                 "due to ftell() with errno %d", errno);
        return false;
    }
    //go to the beginning of this file
    if(!goto_filestart(t))///Go back to the staring file position -- cleanup1
    {
        RF_ERROR(
                 "Failed to move the internal filepointer of "
                 "TextFile \""RF_STR_PF_FMT"\" "
                 "to the beginning", RF_STR_PF_ARG(&t->name));
        return false;
    }
    //check if this file can read
    RF_TEXTFILE_CANREAD_JMP(t, ret = false, cleanup1);
    t->previousOp = RF_FILE_READ;
    //now open another temporary file
    if(!rf_string_initv(&tempFileName, RF_STR_PF_FMT"_temp",
                      RF_STR_PF_ARG(&t->name)))
    {
        RF_ERROR("Failed to initialize the string for the name of the "
                 "temporary file");
        ret = false;
        goto cleanup1;
    }
    if((newFile = rf_fopen(&tempFileName, "w"i_PLUSB_WIN32)) == NULL)
    {    ///need to cleanup the temporary file -- cleanup2
        RF_ERROR("Failed to open a temporary file during text file line"
                 " removal due to fopen() with errno %d", errno);
        ret = false;
        goto cleanup1;
    }

    if(t->hasBom)
    {
        if(!add_BOM(newFile,t->encoding, t->endianess))
        {
            RF_ERROR(
                     "Failed to add a BOM to the newly created temporary file");
            ret = false;
            goto cleanup2;
        }
    }
    //initialize a string reading buffer
    if(!rf_stringx_init_buff(&buffer, RF_OPTION_FGETS_READ_BYTESN+1, ""))
    { ///Need to clean the buff string -- cleanup3
        RF_ERROR("Failed to initialize a string buffer");
        ret = false;
        goto cleanup2;
    }
    //read every line of this file from the beginning
    while((error =rf_textfile_read_line(t,&buffer)) == RF_SUCCESS)
    {
        //only if this line is not the one to remove write the line to the temporary file
        if(t->line-1 != lineN)//-1 is since right after the ReadLine function the current line changes
        {
            //write the given line to the other file according to eol
            ret = write_to_file_eol(newFile, t, &buffer.INH_String);
            //and check for errors
            if(!ret)
            {
                RF_ERROR(
                    "While attempting to remove line [%llu] of TextFile"
                    " \""RF_STR_PF_FMT"\" a write error occured",
                    lineN, RF_STR_PF_ARG(&t->name));
                goto cleanup3;
            }
        }
        else//found the line for removal, don't copy in the new file. Keep its size in bytes
        {
            lineFound = true;
        }

    }
    //get rid of the no longer needed buffer
    rf_stringx_deinit(&buffer); ///no need to clean this up anymore later, back to cleanup2
    // if we reached the end of file and the line was not found
    if(lineFound==false)
    {
        if(error == RE_FILE_EOF)
        {
            RF_ERROR(
                     "While attempting to find line [%llu]"
                     " of TextFile \""RF_STR_PF_FMT"\""
                     " for removal premature End Of File was encountered",
                     lineN, RF_STR_PF_ARG(&t->name));
        }//or else if there was an error
        else
        {
            RF_ERROR(
                     "While attempting to find line [%llu]"
                     " of TextFile \""RF_STR_PF_FMT"\""
                     " for removal a file reading error was encountered",
                     lineN, RF_STR_PF_ARG(&t->name));
        }
        ret = false;
        goto cleanup2;
    }
    //delete the old file
    fclose(t->f);
    if(!rf_system_delete_file(&t->name))
    {
        RF_ERROR(
                 "After the removal operation the temporary file used, could "
                 "not be deleted");
        ret = false;
        goto cleanup2;//since the file closes already
    }
    //rename the temp file to be the new file
    fclose(newFile); /// no need to clean this up anymore, back to cleanup1
    if(!rf_system_rename_file(&tempFileName,&t->name))
    {
        RF_ERROR(
                 "After the removal operation the temporary file used, could "
                 "not be renamed to the original file");
        ret = false;
        goto cleanup1;
    }
    //after renaming we no longer need the string
    rf_string_deinit(&tempFileName);
    if((t->f = rf_fopen(&t->name, "r"i_PLUSB_WIN32"+")) == NULL)
    {
        RF_ERROR("Error at opening the renamed file after the textfile "
                 "removal operation");
        ret = false;
        goto cleanup1;
    }
    //if the line was removed before this line then we should reduce the current line count
    t->line = prLine;
    t->eof = prEof;
    if(lineN<t->line)
    {
        t->line--;
    }
    //get the file position to the proper place
    TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, false);
    ///success
    return true;

    ///cleanup
cleanup3:
    rf_stringx_deinit(&buffer);
cleanup2:
    fclose(newFile);
    if(!rf_system_delete_file(&tempFileName))
    {
        RF_ERROR("Failed to delete file "RF_STR_PF_FMT" during cleanup",
                 RF_STR_PF_ARG(&tempFileName));
        ret = false;
    }
    rf_string_deinit(&tempFileName);
cleanup1:
    TEXTFILE_RESETPTR_FROMSTART(t, prLine, prEof, prOff, false);
    return ret;
}
//Replaces a line of the textfile with the given string
bool rf_textfile_replace(struct RFtextfile* t, uint64_t lineN, void* string)
{
    struct RFstring tempFileName;
    char lineFound,allocatedS;
    char ret = true;
    FILE* newFile;
    int32_t linesCount;
    struct RFstringx buffer;
    RFfile_offset tOff=0;
    int32_t error;
    RF_ENTER_LOCAL_SCOPE();

#ifdef RF_OPTION_DEBUG
    if(string == NULL)
    {
        RF_ERROR("The replace string argument given is NULL");
        ret = false;
        goto cleanup0;
    }
#endif

    lineFound = allocatedS = false;
    //determine the target line
    if(lineN == 0)
    {
        RF_ERROR("The line number argument given is invalid. Indexing starts "
                 "from one");
        ret = false;
        goto cleanup0;//failure
    }

    //determine how many lines the given string has
    linesCount = rf_string_count(string, RFS_("\n"), 0);
    /// cleanup 1 - For this string
    //if we don't have the RFstring default Unix style line ending
    //making a new one since stringP can be on the local stack and we can't use replace since that would act on the local stack
    if(t->eol != RF_EOL_LF && linesCount>0)
    {
        allocatedS = true;
        if((string = rf_string_copy_out((struct RFstring*)string)) == NULL)
        {
            RF_ERROR("String copying failed");
            ret = false;
            goto cleanup1;
        }
        if(t->eol == RF_EOL_CRLF)
        {
            if(!rf_string_replace(string,RFS_("\n"),RFS_("\xD\n"),0,0))
            {
                RF_ERROR("Editing the newline character for the to-replace "
                         "string failed");
                ret = false;
                goto cleanup1;
            }
        }
        else
        {
            if(!rf_string_replace(string,RFS_("\n"),RFS_("\xD"),0,0))
            {
                RF_ERROR("Editing the newline character for the to-replace "
                         "string failed");
                ret = false;
                goto cleanup1;                
            }
        }
    }

    //go to the beginning of this file
    if(goto_filestart(t))
    {
        RF_ERROR(
                 "Failed to move the internal filepointer"
                 " of TextFile \""RF_STR_PF_FMT"\" "
                 "to the beginning", RF_STR_PF_ARG(&t->name));
        ret = false;
        goto cleanup1;
    }
    //check if this file can read
    RF_TEXTFILE_CANREAD_JMP(t, ret = false, cleanup1);
    t->previousOp = RF_FILE_READ;
    //now open another temporary file
    if(!rf_string_initv(&tempFileName, RF_STR_PF_FMT"_temp",
                      RF_STR_PF_ARG(&t->name)))
    {///cleanup2 - For the removal and deallocation of the temporary file
        RF_ERROR("Failed to initialize a temporary file name");
        ret = false;
        goto cleanup1;
    }
    if((newFile = rf_fopen(&tempFileName, "w"i_PLUSB_WIN32)) == NULL)
    {
        RF_ERROR("Failed to open a temporary file during line replacing");
        ret = false;
        goto cleanup2;
    }
    if(t->hasBom)
    {
        if(!add_BOM(newFile, t->endianess, t->encoding))
        {
            RF_ERROR(
                     "Failed to add BOM to the newly created temporary file");
            ret = false;
            goto cleanup2;
        }
    }
    //initialize a string reading buffer
    if(!rf_stringx_init_buff(&buffer,RF_OPTION_FGETS_READ_BYTESN+1,""))
    {///cleanup3- For the deinit of the string buffer
        RF_ERROR("Failure to initialize a StringX buffer during textfile"
                 "line replacing");
        ret = false;
        goto cleanup2;
    }
    //read every line of this file from the beginning;
    while((error =rf_textfile_read_line(t,&buffer)) == RF_SUCCESS)
    {
        //write the read line to the other file if this is not the line to replace
        if(t->line-1 != lineN)//-1 is since right after the ReadLine function the current line changes
        {
            //write the given line to the other file according to eol
            ret = write_to_file_eol(newFile, t, &buffer.INH_String);
            //and check for errors
            if(!ret)
            {
                RF_ERROR(
                         "While attempting to replace line [%llu] of TextFile"
                         " \""RF_STR_PF_FMT"\" a write error occured",
                         lineN, RF_STR_PF_ARG(&t->name));
                goto cleanup3;
            }
        }
        else//else this is the line to replace so..
        {
            lineFound = true;
            if((tOff = rfFtell(newFile))== (RFfile_offset)-1)//get the file offset that we are going to come back to in success
            {
                RF_ERROR("Reading the file position failed due to ftell() "
                         "with errno %d", errno);
                ret = false;
                goto cleanup3;
            }
            //write the given line to the other file according to eol
            ret = write_to_file_eol(newFile, t, string);
            //and check for errors
            if(!ret)
            {
                RF_ERROR(
                         "While attempting to replace line [%llu] of TextFile"
                         " \""RF_STR_PF_FMT"\" a write error occured",
                         lineN, RF_STR_PF_ARG(&t->name));
                goto cleanup3;
            }
        }
    }
    //get rid of the no longer needed buffer
    rf_stringx_deinit(&buffer);///back to cleanup 2
    // if we reached the end of file and the line was not found
    if(lineFound == false)
    {
        if(error == RE_FILE_EOF)
        {
            RF_ERROR("While attempting to find line [%llu] "
                     "of TextFile \""RF_STR_PF_FMT"\" for replacement,"
                     "premature End Of File was "
                     "encountered", lineN, RF_STR_PF_ARG(&t->name));
        }//or else if there was an error
        else
        {
            RF_ERROR("While attempting to find line [%llu] "
                     "of TextFile \""RF_STR_PF_FMT"\" for replacement,"
                     " a file reading error was "
                     "encountered", lineN, RF_STR_PF_ARG(&t->name));
        }
        ret = false;
        goto cleanup2;
    }
    //delete the old file
    fclose(t->f);
    if(!rf_system_delete_file(&t->name)) ///back to cleanup2
    {
        RF_ERROR("After the replacement operation the temporary file "
                 "used, could not be deleted");
        ret = false;
        goto cleanup1;//since the file closed already
    }
    //rename the temp file to be the new file
    fclose(newFile);
    if(!rf_system_rename_file(&tempFileName,&t->name))
    {
        RF_ERROR("After the replacement operation the temporary file "
                 "used, could not be renamed to the original file");
        ret = false;
        goto cleanup1;
    }
    //after renaming the file we no longer need the string
    rf_string_deinit(&tempFileName);
    if((t->f = rf_fopen(&t->name, "r"i_PLUSB_WIN32"+")) == NULL)
    {
        RF_ERROR("Failed to open the edited file after textfile line "
                 "replacing due to fopen() with errno %d", errno);
        ret = false;
        goto cleanup1;
    }
    if(allocatedS == true)
    {
        rf_string_destroy(string);
    }
    //get the file pointer to the beginning of the newly replaced line
    TEXTFILE_RESETPTR_FROMSTART_JMP(t, lineN, false, tOff, ret = false, cleanup0);

    ///success
    RF_EXIT_LOCAL_SCOPE();
    return true;
    ///cleanup code
cleanup3:
    rf_stringx_deinit(&buffer);
cleanup2:
    fclose(newFile);
    if(!rf_system_delete_file(&tempFileName))
    {
        RF_ERROR("Failed to delete file "RF_STR_PF_FMT" during cleanup",
                 RF_STR_PF_ARG(&tempFileName));
        ret = false;
    }
    rf_string_deinit(&tempFileName);
cleanup1:
    if(allocatedS == true)
    {
        rf_string_destroy(string);
    }
cleanup0:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

