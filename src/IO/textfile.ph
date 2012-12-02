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
**  Internal header for textfiles
**/
#include <rf_io.h>
#include "io.ph" //for the private io header


/// ================ Functions for usage only inside RF_TextFile functions ================= ///

struct RF_TextFile;

//! Adds a Byte order mark to the text file at the current position. Only to be used at the start of the file
int32_t i_rfTextFile_AddBom(struct RF_TextFile* t);
//! Adds a Byte order mark to the file at the current position. Only to be used at the start of the file
int32_t i_rfFile_AddBom(FILE*f,char encoding);

//! Handles the EOL encoding for this textfile by either setting the desired encoding pattern or auto detecting it
char TextFile_HandleEol(struct RF_TextFile* t,char eol);
//! Takes a textfile's inner FILE* to the beginning of the file depending on the encoding and BOM existence. Can fail and returns an appropriate error
int32_t TextFile_GoToStart(struct RF_TextFile* t);

/// ================ Macros for usage only inside RF_TextFile functions ================= ///

//! A macro to resume the text file pointer back to a saved position starting from the file's start and if there is an error return it
#define TEXTFILE_RESETPTR_FROMSTART(i_TEXTFILE_,i_PRLINE_,i_PREOF_,i_PROFF_)\
if(rfFseek((i_TEXTFILE_)->f,i_PROFF_,SEEK_SET)!=0)\
{\
    i_TEXTFILE_FSEEK_CHECK(i_TEXTFILE_,"Resetting the file pointer to a given position")\
}\
(i_TEXTFILE_)->line = i_PRLINE_;\
(i_TEXTFILE_)->eof = i_PREOF_;

//! A macro to resume the text file pointer back to a saved position starting from the file's start and if there is an error keep it and go to cleanup code
#define TEXTFILE_RESETPTR_FROMSTART_GOTO(i_TEXTFILE_,i_PRLINE_,i_PREOF_,i_PROFF_,i_ERROR_,i_LABEL_)\
if(rfFseek((i_TEXTFILE_)->f,i_PROFF_,SEEK_SET)!=0)\
{\
    i_TEXTFILE_FSEEK_CHECK_GOTO(i_TEXTFILE_,"Resetting the file pointer to a given position",i_ERROR_,i_LABEL_)\
}\
(i_TEXTFILE_)->line = i_PRLINE_;\
(i_TEXTFILE_)->eof = i_PREOF_;

//! A macro to resume the text file pointer back to a saved position without starting from the file's start but from the current file position
#define TEXTFILE_RESETPTR(i_TEXTFILE_,i_PRLINE_,i_PREOF_,i_PROFF_){\
foff_rft i_tomove_ = i_PROFF_ - rfFtell((i_TEXTFILE_)->f);\
if(rfFseek((i_TEXTFILE_)->f,i_tomove_,SEEK_CUR)!=0)\
{\
    i_TEXTFILE_FSEEK_CHECK(i_TEXTFILE_,"Resetting the file pointer to a given position")\
}\
(i_TEXTFILE_)->line = i_PRLINE_;\
(i_TEXTFILE_)->eof = i_PREOF_;\
}

//! A macro to check if a textfile needs to change its mode in order to perform a read operation. If an error occurs it returns it
#define RF_TEXTFILE_CANREAD(i_TEXTFILE_) {\
/*Get current file position*/\
foff_rft i_cPos_;\
if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1)\
{\
    i_FTELL_CHECK("Querying the current file position")\
}\
/*if the file mode is writing then reopen in reading mode*/\
if((i_TEXTFILE_)->mode == RF_FILE_WRITE)\
{\
    if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"r"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0)\
    {\
        i_TEXTFILE_FOPEN_CHECK(i_TEXTFILE_,"Switching from writing to reading mode at")\
        return  RE_FILE_MODE_CHANGE;\
    }\
    (i_TEXTFILE_)->mode = RF_FILE_READ;\
    /*go to the saved file position*/\
    if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)\
    {\
        i_TEXTFILE_FSEEK_CHECK(t,"resetting the file pointer when switching from write to read mode")\
    }\
}/*else if the file is read/write mode and  the previous file operation was a write we gotta rewind*/\
else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)\
{\
    if((i_TEXTFILE_)->previousOp == RF_FILE_WRITE)\
    {\
        if(rfFseek(t->f,0,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK(t,"rewinding to enable reading")\
        }\
        if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK(t,"rewinding to enable reading")\
        }\
    }\
}}

//! A macro to check if a textfile needs to change its mode in order to perform a read operation. If error occurs it keeps it and goes to the cleanup code
#define RF_TEXTFILE_CANREAD_GOTO(i_TEXTFILE_,i_ERROR_,i_LABEL_) {\
/*Get current file position*/\
foff_rft i_cPos_;\
if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1)\
{\
    i_FTELL_CHECK_GOTO("Querying the current file position",i_ERROR_,i_LABEL_)\
}\
/*if the file mode is writing then reopen in reading mode*/\
if((i_TEXTFILE_)->mode == RF_FILE_WRITE)\
{\
    if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"r"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0)\
    {\
        i_TEXTFILE_FOPEN_CHECK_GOTO(i_TEXTFILE_,"Switching from writing to reading mode at",i_ERROR_,i_LABEL_)\
        i_ERROR_ = RE_FILE_MODE_CHANGE;\
        goto i_LABEL_;\
    }\
    (i_TEXTFILE_)->mode = RF_FILE_READ;\
    /*go to the saved file position*/\
    if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)\
    {\
        i_TEXTFILE_FSEEK_CHECK_GOTO(t,"resetting the file pointer when switching from write to read mode",i_ERROR_,i_LABEL_)\
    }\
}/*else if the file is read/write mode and  the previous file operation was a write we gotta rewind*/\
else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)\
{\
    if((i_TEXTFILE_)->previousOp == RF_FILE_WRITE)\
    {\
        if(rfFseek(t->f,0,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK_GOTO(t,"rewinding to enable reading",i_ERROR_,i_LABEL_)\
        }\
        if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK_GOTO(t,"rewinding to enable reading",i_ERROR_,i_LABEL_)\
        }\
    }\
}}

//! A macro to check if a textfile needs to changes its mode in order to perform a write operation. If there is an error it returns it
#define RF_TEXTFILE_CANWRITE(i_TEXTFILE_){ \
/*Get current file position*/\
foff_rft i_cPos_;\
if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1)\
{\
    i_FTELL_CHECK("Querying the current file position")\
}\
/*if the file mode is reading then reopen in writing mode*/\
if((i_TEXTFILE_)->mode == RF_FILE_READ)\
{\
    if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"a"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0)\
    {\
        i_TEXTFILE_FOPEN_CHECK(i_TEXTFILE_,"Switching from reading to writing mode at")\
        return  RE_FILE_MODE_CHANGE;\
    }\
    (i_TEXTFILE_)->mode = RF_FILE_WRITE;\
    /*go to the saved file position*/\
    if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)\
    {\
        i_TEXTFILE_FSEEK_CHECK(t,"resetting the file pointer when switching from read to write mode")\
    }\
}/*else if the file is read/write mode and  the previous file operation was a read we gotta rewind*/\
else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)\
{\
    if((i_TEXTFILE_)->previousOp == RF_FILE_READ)\
    {\
        if(rfFseek(t->f,0,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK(t,"rewinding to enable writing")\
        }\
        if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK(t,"rewinding to enable writing")\
        }\
    }\
}}
//! A macro to check if a textfile needs to changes its mode in order to perform a write operation. If there is an error it keeps it and goes to cleanup code
#define RF_TEXTFILE_CANWRITE_GOTO(i_TEXTFILE_,i_ERROR_,i_LABEL_){ \
/*Get current file position*/\
foff_rft i_cPos_;\
if((i_cPos_=rfFtell((i_TEXTFILE_)->f)) == (foff_rft)-1)\
{\
    i_FTELL_CHECK_GOTO("Querying the current file position",i_ERROR_,i_LABEL_)\
}\
/*if the file mode is reading then reopen in writing mode*/\
if((i_TEXTFILE_)->mode == RF_FILE_READ)\
{\
    if( ((i_TEXTFILE_)->f = freopen((i_TEXTFILE_)->name.bytes,"a"i_PLUSB_WIN32,(i_TEXTFILE_)->f)) == 0)\
    {\
        i_TEXTFILE_FOPEN_CHECK_GOTO(i_TEXTFILE_,"Switching from reading to writing mode at",i_ERROR_,i_LABEL_)\
        i_ERROR_ =  RE_FILE_MODE_CHANGE;\
        goto i_LABEL_;\
    }\
    (i_TEXTFILE_)->mode = RF_FILE_WRITE;\
    /*go to the saved file position*/\
    if( rfFseek((i_TEXTFILE_)->f,i_cPos_,SEEK_SET)!=0)\
    {\
        i_TEXTFILE_FSEEK_CHECK_GOTO(t,"resetting the file pointer when switching from read to write mode",i_ERROR_,i_LABEL_)\
    }\
}/*else if the file is read/write mode and  the previous file operation was a read we gotta rewind*/\
else if((i_TEXTFILE_)->mode == RF_FILE_READWRITE)\
{\
    if((i_TEXTFILE_)->previousOp == RF_FILE_READ)\
    {\
        if(rfFseek(t->f,0,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK_GOTO(t,"rewinding to enable writing",i_ERROR_,i_LABEL_)\
        }\
        if(rfFseek(t->f,i_cPos_,SEEK_SET) != 0)\
        {\
            i_TEXTFILE_FSEEK_CHECK_GOTO(t,"rewinding to enable writing",i_ERROR_,i_LABEL_)\
        }\
    }\
}}


//! A macro that checks errno after an fseek() call FOR A TEXTFILE and returns appropriate error
#define i_TEXTFILE_FSEEK_CHECK(i_TEXTFILE_,i_TEXT_) \
/*Depending on the errno*/\
switch(errno)\
{\
    /*if EOVERFLOW errno is defined*/\
    i_TEXTFILE_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXTFILE_,i_TEXT_) \
    case EAGAIN:\
        RETURN_LOG_ERROR("While "i_TEXT_" at Text File \"%S\", the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK,(i_TEXTFILE_)->name)\
    break;\
    case EBADF:\
        RETURN_LOG_ERROR("While "i_TEXT_" at Text File \"%S\", the file descriptor was found to be corrupt",RE_FILE_BAD,(i_TEXTFILE_)->name)\
    break;\
    case EFBIG:\
        RETURN_LOG_ERROR("While "i_TEXT_" backwards at Text File \"%S\", the file's size exceeded the limit",RE_FILE_TOOBIG,(i_TEXTFILE_)->name)\
    break;\
    case EINTR:\
        RETURN_LOG_ERROR(i_TEXT_"at Text File \"%S\" failed due to a system interrupt",RE_INTERRUPT,(i_TEXTFILE_)->name)\
    break;\
    case EIO:\
        RETURN_LOG_ERROR("While "i_TEXT_" at Text File \"%S\", a physical I/O error was encountered",RE_FILE_IO,(i_TEXTFILE_)->name)\
    break;\
    case ENOSPC:case ENOMEM:\
        RETURN_LOG_ERROR(i_TEXT_" at Text File \"%S\" failed due to the device containing the file having no free space",RE_FILE_NOSPACE,(i_TEXTFILE_)->name)\
    break;\
    case ENXIO:\
        RETURN_LOG_ERROR(i_TEXT_" at Text File \"%S\" failed due to the device being non-existant",RE_FILE_NOTFILE,(i_TEXTFILE_)->name)\
    break;\
    case EPIPE:\
    case ESPIPE:\
        RETURN_LOG_ERROR(i_TEXT_" at Text File \"%S\" failed due to the file descriptor not being a file and being a Pipe",RE_FILE_NOTFILE,(i_TEXTFILE_)->name)\
    break;\
    default:\
        RETURN_LOG_ERROR("There was a generic error when "i_TEXT_" at Text File using fseek() \"%S\"",RE_FILE_GETFILEPOS,(i_TEXTFILE_)->name)\
    break;\
}//end of error handling

//! A macro that checks errno after an fseek() call FOR A TEXTFILE sets the error and goes to cleanup and error return
#define i_TEXTFILE_FSEEK_CHECK_GOTO(i_TEXTFILE_,i_TEXT_,i_ERROR_,i_LABEL_) \
/*Depending on the errno*/\
switch(errno)\
{\
    /*if EOVERFLOW errno is defined*/\
    i_TEXTFILE_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXTFILE_,i_TEXT_) \
    case EAGAIN:\
        LOG_ERROR("While "i_TEXT_" at Text File \"%S\", the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_WRITE_BLOCK;\
    break;\
    case EBADF:\
        LOG_ERROR("While "i_TEXT_" at Text File \"%S\", the file descriptor was found to be corrupt",RE_FILE_BAD,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_BAD;\
    break;\
    case EFBIG:\
        LOG_ERROR("While "i_TEXT_" backwards at Text File \"%S\", the file's size exceeded the limit",RE_FILE_TOOBIG,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_TOOBIG;\
    break;\
    case EINTR:\
        LOG_ERROR(i_TEXT_"at Text File \"%S\" failed due to a system interrupt",RE_INTERRUPT,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_INTERRUPT;\
    break;\
    case EIO:\
        LOG_ERROR("While "i_TEXT_" at Text File \"%S\", a physical I/O error was encountered",RE_FILE_IO,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_IO;\
    break;\
    case ENOSPC:case ENOMEM:\
        LOG_ERROR(i_TEXT_" at Text File \"%S\" failed due to the device containing the file having no free space",RE_FILE_NOSPACE,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NOSPACE;\
    break;\
    case ENXIO:\
        LOG_ERROR(i_TEXT_" at Text File \"%S\" failed due to the device being non-existant",RE_FILE_NOTFILE,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    case EPIPE:\
    case ESPIPE:\
        LOG_ERROR(i_TEXT_" at Text File \"%S\" failed due to the file descriptor not being a file and being a Pipe",RE_FILE_NOTFILE,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    default:\
        LOG_ERROR("There was a generic error when "i_TEXT_" at Text File using fseek() \"%S\"",RE_FILE_GETFILEPOS,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_GETFILEPOS;\
    break;\
}\
goto i_LABEL_;

//! A macro that checks errno after an fopen() call for textfiles, and returns appropriate error
#define i_TEXTFILE_FOPEN_CHECK(i_TEXTFILE_,i_TEXT_) \
/*depending on errno*/\
switch(errno)\
{\
    case EACCES:\
    case EROFS:\
        RETURN_LOG_ERROR(i_TEXT_" textfile \"%S\" failed due to either the file or some part of its path having permissions that do not allow the application to process it in\
                  the requested mode",RE_FILE_PERMISSION,(i_TEXTFILE_)->name)\
    break;\
    case EINTR:\
        RETURN_LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to an interrupt signal",RE_INTERRUPT,(i_TEXTFILE_)->name)\
    break;\
    case EISDIR:\
    case ENOENT:\
    case ENOTDIR:\
    case ENXIO:\
        RETURN_LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to the given name not belonging to a file.",RE_FILE_NOTFILE,(i_TEXTFILE_)->name)\
    break;\
    case EMFILE:\
    case ENFILE:\
        RETURN_LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to the process having reached the maximum number of open files/file descriptors allowed by the system",RE_FILE_MAXFILES,(i_TEXTFILE_)->name)\
    break;\
    case ENAMETOOLONG:\
        RETURN_LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to the the name of or the directory's path exceeding the system limits",RE_FILE_NAMELENGTH,(i_TEXTFILE_)->name)\
    break;\
    case ENOSPC:\
    case ENOMEM:\
        RETURN_LOG_ERROR(i_TEXT_" \"%S\" failed due to insufficient space for creating the file in the directory",RE_FILE_NOSPACE,(i_TEXTFILE_)->name)\
    break;\
    case RE_FILE_TOOBIG:\
        RETURN_LOG_ERROR(i_TEXT_" \"%S\" failed due its size exceeding the maximu size of a file that can be handled",RE_FILE_TOOBIG,(i_TEXTFILE_)->name)\
    break;\
    default:\
        LOG_ERROR(i_TEXT_" of texfile \"%S\" failed",RE_FILE_INIT,(i_TEXTFILE_)->name);\
        return RF_FAILURE;\
    break;\
}

//! A macro that checks errno after an fopen() call for textfiles, keeps the error and goes to cleanup code
#define i_TEXTFILE_FOPEN_CHECK_GOTO(i_TEXTFILE_,i_TEXT_,i_ERROR_,i_LABEL_) {\
/*depending on errno*/\
switch(errno)\
{\
    case EACCES:\
    case EROFS:\
        LOG_ERROR(i_TEXT_" textfile \"%S\" failed due to either the file or some part of its path having permissions that do not allow the application to process it in\
                  the requested mode",RE_FILE_PERMISSION,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_PERMISSION;\
    break;\
    case EINTR:\
        LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to an interrupt signal",RE_INTERRUPT,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    case EISDIR:\
    case ENOENT:\
    case ENOTDIR:\
    case ENXIO:\
        LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to the given name not belonging to a file.",RE_FILE_NOTFILE,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    case EMFILE:\
    case ENFILE:\
        LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to the process having reached the maximum number of open files/file descriptors allowed by the system",RE_FILE_MAXFILES,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_MAXFILES;\
    break;\
    case ENAMETOOLONG:\
        LOG_ERROR(i_TEXT_" texfile \"%S\" failed due to the the name of or the directory's path exceeding the system limits",RE_FILE_NAMELENGTH,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NAMELENGTH;\
    break;\
    case ENOSPC:\
    case ENOMEM:\
        LOG_ERROR(i_TEXT_" \"%S\" failed due to insufficient space for creating the file in the directory",RE_FILE_NOSPACE,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_NOSPACE;\
    break;\
    case RE_FILE_TOOBIG:\
        LOG_ERROR(i_TEXT_" \"%S\" failed due its size exceeding the maximu size of a file that can be handled",RE_FILE_TOOBIG,(i_TEXTFILE_)->name);\
        i_ERROR_ = RE_FILE_TOOBIG;\
    break;\
    default:\
        LOG_ERROR(i_TEXT_" of texfile \"%S\" failed",RE_FILE_INIT,(i_TEXTFILE_)->name);\
        i_ERROR_ = RF_FAILURE;\
    break;\
}\
goto i_LABEL_;}


//! A macro to check the result of file writting functions for TextFiles and if there is an error return it
#define i_TEXTFILE_WRITE_CHECK(i_TEXTFILE_,i_TEXT_)\
/*depending or errno*/\
if(ferror((i_TEXTFILE_)->f) != 0){\
    switch(errno)\
    {\
        case EAGAIN:\
            RETURN_LOG_ERROR("While "i_TEXT_" to Text File \"%S\", the write failed because the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK,(i_TEXTFILE_)->name)\
        break;\
        case EBADF:\
            RETURN_LOG_ERROR("While "i_TEXT_" to Text File \"%S\", the file descriptor was found to be corrupt",RE_FILE_BAD,(i_TEXTFILE_)->name)\
        break;\
        case EFBIG:\
            RETURN_LOG_ERROR("While "i_TEXT_" to Text File \"%S\", the file's size exceeded the limit",RE_FILE_TOOBIG,(i_TEXTFILE_)->name)\
        break;\
        case EINTR:\
            RETURN_LOG_ERROR(i_TEXT_" to Text File \"%S\" failed due to a system interrupt",RE_INTERRUPT,(i_TEXTFILE_)->name)\
        break;\
        case EIO:\
            RETURN_LOG_ERROR("While "i_TEXT_" to Text File \"%S\", a physical I/O error was encountered",RE_FILE_IO,(i_TEXTFILE_)->name)\
        break;\
        case ENOSPC:\
        case ENOMEM:\
            RETURN_LOG_ERROR(i_TEXT_"to Text File \"%S\" failed due to the device containing the file having no free space",RE_FILE_NOSPACE,(i_TEXTFILE_)->name)\
        break;\
        case ENXIO:\
            RETURN_LOG_ERROR(i_TEXT_" to Text File \"%S\" failed due to the device being non-existant",RE_FILE_NOTFILE,(i_TEXTFILE_)->name)\
        break;\
        default:\
            RETURN_LOG_ERROR("There was a generic write error when "i_TEXT_" to Text File \"%S\"",RE_FILE_WRITE,(i_TEXTFILE_)->name)\
        break;\
    }}\

//! A macro to check the result of file writting functions for TextFiles and if there is an error jump to a label
#define i_TEXTFILE_WRITE_CHECK_GOTO(i_TEXTFILE_,i_TEXT_,i_ERROR_,i_LABEL_)\
/*depending or errno*/\
if(ferror((i_TEXTFILE_)->f) != 0){\
    switch(errno)\
    {\
        case EAGAIN:\
            LOG_ERROR("While "i_TEXT_" to Text File \"%S\", the write failed because the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_WRITE_BLOCK;\
        break;\
        case EBADF:\
            LOG_ERROR("While "i_TEXT_" to Text File \"%S\", the file descriptor was found to be corrupt",RE_FILE_BAD,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_BAD;\
        break;\
        case EFBIG:\
            LOG_ERROR("While "i_TEXT_" to Text File \"%S\", the file's size exceeded the limit",RE_FILE_TOOBIG,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_TOOBIG;\
        break;\
        case EINTR:\
            LOG_ERROR(i_TEXT_" to Text File \"%S\" failed due to a system interrupt",RE_INTERRUPT,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_INTERRUPT;\
        break;\
        case EIO:\
            LOG_ERROR("While "i_TEXT_" to Text File \"%S\", a physical I/O error was encountered",RE_FILE_IO,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_IO;\
        break;\
        case ENOSPC:\
        case ENOMEM:\
            LOG_ERROR(i_TEXT_"to Text File \"%S\" failed due to the device containing the file having no free space",RE_FILE_NOSPACE,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_NOSPACE;\
        break;\
        case ENXIO:\
            LOG_ERROR(i_TEXT_" to Text File \"%S\" failed due to the device being non-existant",RE_FILE_NOTFILE,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_NOTFILE;\
        break;\
        default:\
            LOG_ERROR("There was a generic write error when "i_TEXT_" to Text File \"%S\"",RE_FILE_WRITE,(i_TEXTFILE_)->name)\
            i_ERROR_ = RE_FILE_WRITE;\
        break;\
    }\
    goto i_LABEL_;\
    }\

