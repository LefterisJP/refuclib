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
**  --IO/textfile.ph
** Internal private header for textfile functionality that is not required to be exposed to the user
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size types needed in various places
#include <stdio.h> //for FILE*
#include <String/string_decl.h> // for RF_String (used in RF_TextFile
#include <IO/textfile_decl.h> //for RF_TextFile
#include "textfile.ph"
---------------------For internal library include make sure to have----------------------------
*/


/// ================ Macros for usage only inside RF_TextFile functions ================= ///

/**
 ** A macro to resume the text file pointer back to a saved position starting
 ** from the file's start. If there is an error it returns @c i_RET_
 **/
#define TEXTFILE_RESETPTR_FROMSTART(i_TEXTFILE_, i_PRLINE_, i_PREOF_,\
                                    i_PROFF_, i_RET_)             do{   \
    if(rfFseek((i_TEXTFILE_)->f,i_PROFF_,SEEK_SET)!=0)                  \
    {                                                                   \
        RF_ERROR_FSEEK("Resetting the file pointer to a given position failed", \
                       "fseek");                                        \
        return i_RET_;                                                  \
    }                                                                   \
        (i_TEXTFILE_)->line = i_PRLINE_;                                \
        (i_TEXTFILE_)->eof = i_PREOF_; }while(0)

/**
 ** A macro to resume the text file pointer back to a saved position starting
 ** from the file's start. If there is an error it executes @c i_STMT_ and jumps
 ** to @c i_FLAG_
 **/
#define TEXTFILE_RESETPTR_FROMSTART_JMP(i_TEXTFILE_, i_PRLINE_, i_PREOF_,\
                                    i_PROFF_, i_STMT_, i_FLAG_)  do{    \
    if(rfFseek((i_TEXTFILE_)->f,i_PROFF_,SEEK_SET)!=0)                  \
    {                                                                   \
        RF_ERROR_FSEEK("Resetting the file pointer to a given position failed", \
                       "fseek");                                        \
        i_STMT_;                                                        \
        goto i_FLAG_;                                                   \
    }                                                                   \
    (i_TEXTFILE_)->line = i_PRLINE_;                                    \
    (i_TEXTFILE_)->eof = i_PREOF_; }while(0)



/**
 ** A macro to resume the text file pointer back to a saved position
 ** without starting from the file's start but from the current file position
 ** In case of error it returns @c i_RET_
 **/
#define TEXTFILE_RESETPTR(i_TEXTFILE_,i_PRLINE_,i_PREOF_,i_PROFF_, i_RET_) do{ \
        foff_rft i_tomove_ = i_PROFF_ - rfFtell((i_TEXTFILE_)->f);      \
        if(rfFseek((i_TEXTFILE_)->f, i_tomove_, SEEK_CUR) != 0)         \
        {                                                               \
            RF_ERROR_FSEEK("Resetting the file pointer to a given position failed", \
                           "fseek");                                    \
            return i_RET_;                                              \
        }                                                               \
        (i_TEXTFILE_)->line = i_PRLINE_;                                \
        (i_TEXTFILE_)->eof = i_PREOF_;                                  \
    }while(0)

/**
 ** A macro to resume the text file pointer back to a saved position
 ** without starting from the file's start but from the current file position
 ** In case of error it executes @c i_STMT_ and jumps to @c i_FLAG_
 **/
#define TEXTFILE_RESETPTR_JMP(i_TEXTFILE_,i_PRLINE_,i_PREOF_,           \
                              i_PROFF_, i_STMT_, i_FLAG_) do{          \
        foff_rft i_tomove_ = i_PROFF_ - rfFtell((i_TEXTFILE_)->f);      \
        if(rfFseek((i_TEXTFILE_)->f, i_tomove_, SEEK_CUR) != 0)         \
        {                                                               \
            RF_ERROR_FSEEK("Resetting the file pointer to a given position failed", \
                           "fseek");                                    \
            i_STMT_;                                                    \
            goto i_FLAG_;                                               \
        }                                                               \
        (i_TEXTFILE_)->line = i_PRLINE_;                                \
        (i_TEXTFILE_)->eof = i_PREOF_;                                  \
    }while(0)

/**
 ** A macro to check if a textfile needs to change its mode in order to
 ** perform a read operation. If an error occurs it returns i_RET_
 **/
#define RF_TEXTFILE_CANREAD(i_TEXTFILE_, i_RET_) do{   \
        /*Get current file position*/       \
        foff_rft i_cPos_;                               \
        if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1) \
        {                                                       \
            RF_ERROR_FTELL("Querying the current file position failed", "ftell"); \
            return i_RET_;                                              \
        }                                                               \
        /*if the file mode is writing then reopen in reading mode*/     \
        if((i_TEXTFILE_)->mode == RF_FILE_WRITE)                        \
        {                                                               \
            if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"r"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0) \
            {                                                           \
                RF_ERROR_FOPEN("Switching from writing to reading mode failed",\
                               "freopen"); \
                return  i_RET_;                \
            }                                   \
            (i_TEXTFILE_)->mode = RF_FILE_READ; \
            /*go to the saved file position*/           \
            if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)  \
            {                                                           \
                RF_ERROR_FSEEK("Resetting the file pointer when switching from "\
                               "write to read mode failed", "fseek");   \
                return i_RET_;                                          \
            }                                                           \
        }                                                               \
/*else if the file is read/write mode and  the previous file operation was \
  a write we gotta rewind*/                                             \
        else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)               \
        {                                                               \
            if((i_TEXTFILE_)->previousOp == RF_FILE_WRITE)              \
            {                                                           \
                if(rfFseek(t->f,0,SEEK_SET) != 0)                       \
                {                                                       \
                    RF_ERROR_FSEEK("Rewinding to enable reading failed", \
                                   "fseek");                            \
                    return i_RET_;                                      \
                }                                                       \
                if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)                 \
                {                                                       \
                    RF_ERROR_FSEEK("Rewinding to enable reading failed", "fseek"); \
                    return i_RET_;                                      \
                }                                                       \
            }                                                           \
        }}while(0)



/**
 ** A macro to check if a textfile needs to change its mode in order to
 ** perform a read operation. If an error occurs it executes a statement
 ** and jumps to a flag
 **/
#define RF_TEXTFILE_CANREAD_JMP(i_TEXTFILE_, i_STMT_, i_FLAG_) do{  \
        /*Get current file position*/       \
        foff_rft i_cPos_;                               \
        if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1) \
        {                                                       \
            RF_ERROR_FTELL("Querying the current file position failed", "ftell"); \
            i_STMT_;                                                    \
            goto i_FLAG_;                                               \
        }                                                               \
        /*if the file mode is writing then reopen in reading mode*/     \
        if((i_TEXTFILE_)->mode == RF_FILE_WRITE)                        \
        {                                                               \
            if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"r"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0) \
            {                                                           \
                RF_ERROR_FOPEN("Switching from writing to reading mode failed",\
                               "freopen"); \
                i_STMT_;                                                \
                goto i_FLAG_;                                           \
            }                                   \
            (i_TEXTFILE_)->mode = RF_FILE_READ; \
            /*go to the saved file position*/           \
            if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)  \
            {                                                           \
                RF_ERROR_FSEEK("Resetting the file pointer when switching from "\
                               "write to read mode failed", "fseek");   \
                i_STMT_;                                                \
                goto i_FLAG_;                                           \
            }                                                           \
        }                                                               \
/*else if the file is read/write mode and  the previous file operation was \
  a write we gotta rewind*/                                             \
        else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)               \
        {                                                               \
            if((i_TEXTFILE_)->previousOp == RF_FILE_WRITE)              \
            {                                                           \
                if(rfFseek(t->f,0,SEEK_SET) != 0)                       \
                {                                                       \
                    RF_ERROR_FSEEK("Rewinding to enable reading failed", \
                                   "fseek");                            \
                    i_STMT_;                                            \
                    goto i_FLAG_;                                       \
                }                                                       \
                if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)                 \
                {                                                       \
                    RF_ERROR_FSEEK("Rewinding to enable reading failed", "fseek"); \
                    i_STMT_;                                            \
                    goto i_FLAG_;                                       \
                }                                                       \
            }                                                           \
        }}while(0)

/**
 ** A macro to check if a textfile needs to changes its mode in order
 ** to perform a write operation. If there is an error it returns @c i_RET_
 **/
#define RF_TEXTFILE_CANWRITE(i_TEXTFILE_, i_RET_){ \
    /*Get current file position*/                       \
    foff_rft i_cPos_;                                           \
    if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1)     \
    {                                                                   \
        RF_ERROR_FTELL("Querying the current file position failed", "ftell"); \
        return i_RET_;                                                  \
    }                                                                   \
    /*if the file mode is reading then reopen in writing mode*/         \
    if((i_TEXTFILE_)->mode == RF_FILE_READ)                             \
    {                                                                   \
        if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"a"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0) \
        {                                                               \
            RF_ERROR_FOPEN("Switching from reading to writting mode failed", \
                           "freopen");                                  \
            return i_RET_;                                              \
        }                                                               \
        (i_TEXTFILE_)->mode = RF_FILE_WRITE;                            \
        /*go to the saved file position*/                               \
        if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)              \
        {                                                               \
            RF_ERROR_FSEEK("Resetting the file pointer when switching from " \
                           "read to write mode failed", "fseek");       \
            return i_RET_;                                              \
        }                                                               \
    }                                                                   \
/*else if the file is read/write mode and  the previous file operation was\ 
  a read we gotta rewind*/                                              \
   else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)             \
   {                                                             \
       if((i_TEXTFILE_)->previousOp == RF_FILE_READ)             \
       {                                                         \
           if(rfFseek(t->f,0,SEEK_SET) != 0)                     \
           {                                                        \
               RF_ERROR_FSEEK("Rewinding to enable writing failed", "fseek"); \
               return i_RET_;                                           \
           }                                    \
           if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)  \
           {                                                        \
               RF_ERROR_FSEEK("Rewinding to enable writing failed", "fseek"); \
               return i_RET_;                                           \
           }                                                            \
       }                                                                \
   }}while(0)

/**
 ** A macro to check if a textfile needs to changes its mode in order
 ** to perform a write operation. If there is an error it executes
 ** @c i_STMT_ and jumps to @c i_FLAG_
 **/
#define RF_TEXTFILE_CANWRITE_JMP(i_TEXTFILE_, i_STMT_, i_FLAG_){    \
         /*Get current file position*/                              \
         foff_rft i_cPos_;                                          \
         if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1)        \
         {                                                              \
             RF_ERROR_FTELL("Querying the current file position failed", "ftell"); \
             i_STMT_;                                                   \
             goto i_FLAG_;                                              \
         }                                                              \
         /*if the file mode is reading then reopen in writing mode*/    \
         if((i_TEXTFILE_)->mode == RF_FILE_READ)                        \
         {                                                              \
             if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"a"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0) \
             {                                                          \
                 RF_ERROR_FOPEN("Switching from reading to writting mode failed", \
                                "freopen");                             \
                 i_STMT_;                                               \
                 goto i_FLAG_;                                          \
             }                                                          \
             (i_TEXTFILE_)->mode = RF_FILE_WRITE;                       \
             /*go to the saved file position*/                          \
             if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)         \
             {                                                          \
                 RF_ERROR_FSEEK("Resetting the file pointer when switching from " \
                                "read to write mode failed", "fseek");  \
                 i_STMT_;                                               \
                 goto i_FLAG_;                                          \
             }                                                          \
         }                                                              \
/*else if the file is read/write mode and  the previous file operation was\ 
  a read we gotta rewind*/                                              \
         else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)              \
         {                                                              \
             if((i_TEXTFILE_)->previousOp == RF_FILE_READ)              \
             {                                                          \
                 if(rfFseek(t->f,0,SEEK_SET) != 0)                      \
                 {                                                      \
                     RF_ERROR_FSEEK("Rewinding to enable writing failed", "fseek"); \
                     i_STMT_;                                           \
                     goto i_FLAG_;                                      \
                 }                                                      \
                 if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)                \
                 {                                                      \
                     RF_ERROR_FSEEK("Rewinding to enable writing failed", "fseek"); \
                     i_STMT_;                                           \
                     goto i_FLAG_;                                      \
                 }                                                      \
             }                                                          \
         }}while(0)
