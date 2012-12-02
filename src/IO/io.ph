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
** Just an internal header to hold the various macros
** used only by the RF_TextFile functions
*/




///defining macros if EOVERFLOW errno is defined
#ifdef EOVERFLOW
    #define i_TEXTFILE_FSEEK_CHECK_CASE_EOVERFLOW_RESET(i_TEXTFILE_,i_TEXT_,i_PRLINE_,i_PREOF_,i_PROFF_) \
        case EOVERFLOW:\
            TEXTFILE_RESETPTR(i_TEXTFILE_,i_PRLINE_,i_PREOF_,i_PROFF_)\
            RETURN_LOG_ERROR("While "i_TEXT_" at Text File \"%S\" the file position is not representable by off_t",RE_FILE_POS_OVERFLOW,&(i_TEXTFILE_)->name)\
        break;
    #define i_TEXTFILE_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXTFILE_,i_TEXT_) \
        case EOVERFLOW:\
            RETURN_LOG_ERROR("While "i_TEXT_" at Text File \"%S\" the file position is not representable by off_t",RE_FILE_POS_OVERFLOW,&(i_TEXTFILE_)->name)\
        break;
    #define i_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXT_) \
        case EOVERFLOW:\
            RETURN_LOG_ERROR("While "i_TEXT_" the file position is not representable by off_t",RE_FILE_POS_OVERFLOW)\
        break;
    #define i_FSEEK_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_) \
        case EOVERFLOW:\
            LOG_ERROR("While "i_TEXT_" the file position is not representable by off_t",RE_FILE_POS_OVERFLOW)\
            i_ERROR_ = RE_FILE_POS_OVERFLOW;\
        break;
    #define i_FTELL_CHECK_CASE_EOVERFLOW(i_TEXT_) \
        case EOVERFLOW:\
            RETURN_LOG_ERROR("While "i_TEXT_" the file position is not representable by the the system's type by off_t",RE_FILE_POS_OVERFLOW)\
        break;
    #define i_FTELL_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_) \
        case EOVERFLOW:\
            LOG_ERROR("While "i_TEXT_" the file position is not representable by the the system's type by off_t",RE_FILE_POS_OVERFLOW)\
            i_ERROR_ = RE_FILE_POS_OVERFLOW;\
        break;
    #define i_READ_CHECK_CASE_EOVERFLOW(i_TEXT_) \
        case EOVERFLOW:\
        RETURN_LOG_ERROR(i_TEXT_" reading failed due to attempted read beyond the maximum offset associated with the corresponding file stream",RE_FILE_POS_OVERFLOW)\
        break;
    #define i_READ_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_) \
        case EOVERFLOW:\
        LOG_ERROR(i_TEXT_" reading failed due to attempted read beyond the maximum offset associated with the corresponding file stream",RE_FILE_POS_OVERFLOW)\
        i_ERROR_ = RE_FILE_POS_OVERFLOW;\
        break;
    #define i_READ_CHECK_CASE_EOVERFLOW_RESET(i_TEXT_) \
        case EOVERFLOW:\
        TEXTFILE_RESETPTR(i_TEXTFILE_,i_PRLINE_,i_PREOF_,i_PROFF_)\
        RETURN_LOG_ERROR(i_TEXT_" reading failed due to attempted read beyond the maximum offset associated with the corresponding file stream",RE_FILE_POS_OVERFLOW)\
        break;
#else
    #define i_TEXTFILE_FSEEK_CHECK_CASE_EOVERFLOW_RESET(i_TEXTFILE_,i_TEXT_,i_PRLINE_,i_PREOF_,i_PROFF_)
    #define i_TEXTFILE_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXTFILE_,i_TEXT_)
    #define i_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXT_)
    #define i_FSEEK_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_)
    #define i_FTELL_CHECK_CASE_EOVERFLOW(i_TEXT_)
    #define i_FTELL_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_GOTO_)
    #define i_READ_CHECK_CASE_EOVERFLOW(i_TEXT_)
    #define i_READ_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_)
    #define i_READ_CHECK_CASE_EOVERFLOW_RESET(i_TEXT_)
#endif

//! A macro that checks errno after an ftell() call and returns appropriate error
#define i_FTELL_CHECK(i_TEXT_) \
/*Depending on errno value*/\
switch(errno)\
{\
    i_FTELL_CHECK_CASE_EOVERFLOW(i_TEXT_)\
    case EBADF:\
        RETURN_LOG_ERROR("While "i_TEXT_" the file descriptor was corrupt or not open",RE_FILE_BAD)\
    break;\
    case ESPIPE:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to the file descriptor being a pipe or a socker and not a file",RE_FILE_NOTFILE)\
    break;\
    default:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to an unrecognized ftell() failure",RE_FILE_GETFILEPOS)\
    break;\
}

//! A macro that checks errno after an ftell() call, sets the appropriate error and uses go to to jump to the appropriate error handling case
#define i_FTELL_CHECK_GOTO(i_TEXT_,i_ERROR_,i_LABEL_) \
/*Depending on errno value*/\
switch(errno)\
{\
    i_FTELL_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_)\
    case EBADF:\
        LOG_ERROR("While "i_TEXT_" the file descriptor was corrupt or not open",RE_FILE_BAD);\
        i_ERROR_ = RE_FILE_BAD;\
    break;\
    case ESPIPE:\
        LOG_ERROR(i_TEXT_" failed due to the file descriptor being a pipe or a socker and not a file",RE_FILE_NOTFILE);\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    default:\
        LOG_ERROR(i_TEXT_" failed due to an unrecognized ftell() failure",RE_FILE_GETFILEPOS);\
        i_ERROR_ = RE_FILE_GETFILEPOS;\
    break;\
}\
goto i_LABEL_;

//! A macro that checks errno after an fseek() call, and returns appropriate error to be used for general refu I/O functions as opposed to i_FSEEK_CHECK() which is for TextFiles only
#define i_FSEEK_CHECK(i_TEXT_)\
switch(errno)\
{\
    /*if EOVERFLOW errno is defined*/\
    i_FSEEK_CHECK_CASE_EOVERFLOW(i_TEXT_) \
    case EAGAIN:\
        RETURN_LOG_ERROR("While "i_TEXT_", the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK)\
    break;\
    case EBADF:\
        RETURN_LOG_ERROR("While "i_TEXT_", the file descriptor was found to be corrupt",RE_FILE_BAD)\
    break;\
    case EFBIG:\
        RETURN_LOG_ERROR("While "i_TEXT_", the file's size exceeded the limit",RE_FILE_TOOBIG)\
    break;\
    case EINTR:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to a system interrupt",RE_INTERRUPT)\
    break;\
    case EIO:\
        RETURN_LOG_ERROR("While "i_TEXT_", a physical I/O error was encountered",RE_FILE_IO)\
    break;\
    case ENOSPC:case ENOMEM:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to the device containing the file having no free space",RE_FILE_NOSPACE)\
    break;\
    case ENXIO:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to the device being non-existant",RE_FILE_NOTFILE)\
    break;\
    case EPIPE:\
    case ESPIPE:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to the file descriptor not being a file and being a Pipe",RE_FILE_NOTFILE)\
    break;\
    default:\
        RETURN_LOG_ERROR(i_TEXT_" failed due to generic fseek() error",RE_FILE_GETFILEPOS)\
    break;\
}//end of error handling

//! A macro that checks errno after an fseek() call, and jumpst to a label logging an appropriate error to be used for general refu I/O functions as opposed to i_FSEEK_CHECK() which is for TextFiles only
#define i_FSEEK_CHECK_GOTO(i_TEXT_,i_ERROR_,i_LABEL_){\
switch(errno)\
{\
    /*if EOVERFLOW errno is defined*/\
    i_FSEEK_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_) \
    case EAGAIN:\
        LOG_ERROR("While "i_TEXT_", the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK)\
        i_ERROR_ = RE_FILE_WRITE_BLOCK;\
    break;\
    case EBADF:\
        LOG_ERROR("While "i_TEXT_", the file descriptor was found to be corrupt",RE_FILE_BAD)\
        i_ERROR_ = RE_FILE_BAD;\
    break;\
    case EFBIG:\
        LOG_ERROR("While "i_TEXT_", the file's size exceeded the limit",RE_FILE_TOOBIG)\
        i_ERROR_ = RE_FILE_TOOBIG;\
    break;\
    case EINTR:\
        LOG_ERROR(i_TEXT_" failed due to a system interrupt",RE_INTERRUPT)\
        i_ERROR_ = RE_INTERRUPT;\
    break;\
    case EIO:\
        LOG_ERROR("While "i_TEXT_", a physical I/O error was encountered",RE_FILE_IO)\
        i_ERROR_ = RE_FILE_IO;\
    break;\
    case ENOSPC:case ENOMEM:\
        LOG_ERROR(i_TEXT_" failed due to the device containing the file having no free space",RE_FILE_NOSPACE)\
        i_ERROR_ = RE_FILE_NOSPACE;\
    break;\
    case ENXIO:\
        LOG_ERROR(i_TEXT_" failed due to the device being non-existant",RE_FILE_NOTFILE)\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    case EPIPE:\
    case ESPIPE:\
        LOG_ERROR(i_TEXT_" failed due to the file descriptor not being a file and being a Pipe",RE_FILE_NOTFILE)\
        i_ERROR_ = RE_FILE_NOTFILE;\
    break;\
    default:\
        LOG_ERROR(i_TEXT_" failed due to generic fseek() error",RE_FILE_GETFILEPOS)\
        i_ERROR_ = RE_FILE_GETFILEPOS;\
    break;\
}\
goto i_LABEL_;\
}

//! A macro to check the result of fopen and fread for files. By opening an else block under the macro you can process the actual EOF case
#define i_READ_CHECK(i_FILE_,i_TEXT_) \
/*if the error indicator of the stream is set*/\
if(ferror(i_FILE_)!=0){\
/*Depending on errno*/\
    switch(errno)\
    {\
        i_READ_CHECK_CASE_EOVERFLOW(i_TEXT_)\
        case EAGAIN:\
            RETURN_LOG_ERROR(i_TEXT_" the file is blocked by another thread and the read operation would be delayed",RE_FILE_READ_BLOCK)\
        break;\
        case EBADF:\
            RETURN_LOG_ERROR(i_TEXT_" the file descriptor is not valid for reading",RE_FILE_MODE)\
        break;\
        case EINTR:\
            RETURN_LOG_ERROR(i_TEXT_" reading failed due to a system interrupt",RE_INTERRUPT)\
        break;\
        case EIO:case ENXIO:\
            RETURN_LOG_ERROR(i_TEXT_" reading failed due a physical or implementation defined I/O error",RE_FILE_IO)\
        break;\
        case ENOMEM:\
            RETURN_LOG_ERROR(i_TEXT_" reading failed due to insufficient storage space",RE_FILE_NOSPACE)\
        break;\
        default:\
            RETURN_LOG_ERROR(i_TEXT_" reading failed due to an unknown reading error",RE_FILE_READ)\
        break;\
    }}

//! A macro to check the result of fopen and fread for files and jumpt to a label with goto. By opening an else block under the macro you can process the actual EOF case
#define i_READ_CHECK_GOTO(i_FILE_,i_TEXT_,i_ERROR_,i_LABEL_) \
/*if the error indicator of the stream is set*/\
if(ferror(i_FILE_)!=0){\
/*Depending on errno*/\
    switch(errno)\
    {\
        i_READ_CHECK_GOTO_CASE_EOVERFLOW(i_TEXT_,i_ERROR_)\
        case EAGAIN:\
            LOG_ERROR(i_TEXT_" the file is blocked by another thread and the read operation would be delayed",RE_FILE_READ_BLOCK)\
            i_ERROR_ = RE_FILE_READ_BLOCK;\
        break;\
        case EBADF:\
            LOG_ERROR(i_TEXT_" the file descriptor is not valid for reading",RE_FILE_MODE)\
            i_ERROR_ = RE_FILE_MODE;\
        break;\
        case EINTR:\
            LOG_ERROR(i_TEXT_" reading failed due to a system interrupt",RE_INTERRUPT)\
            i_ERROR_ = RE_FILE_MODE;\
        break;\
        case EIO:case ENXIO:\
            LOG_ERROR(i_TEXT_" reading failed due a physical or implementation defined I/O error",RE_FILE_IO)\
            i_ERROR_ = RE_FILE_IO;\
        break;\
        case ENOMEM:\
            LOG_ERROR(i_TEXT_" reading failed due to insufficient storage space",RE_FILE_NOSPACE)\
            i_ERROR_ = RE_FILE_NOSPACE;\
        break;\
        default:\
            LOG_ERROR(i_TEXT_" reading failed due to an unknown reading error",RE_FILE_READ)\
            i_ERROR_ = RE_FILE_READ;\
        break;\
    }\
    goto i_LABEL_;\
}

//! A macro to check the result of file writting functions for I/O
#define i_WRITE_CHECK(i_FILE_,i_TEXT_) \
/*depending or errno*/\
if(ferror(i_FILE_) != 0){\
    switch(errno)\
    {\
        case EAGAIN:\
            RETURN_LOG_ERROR("While "i_TEXT_", the write failed because the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK)\
        break;\
        case EBADF:\
            RETURN_LOG_ERROR("While "i_TEXT_", the file descriptor was found to be corrupt",RE_FILE_BAD)\
        break;\
        case EFBIG:\
            RETURN_LOG_ERROR("While "i_TEXT_", the file's size exceeded the limit",RE_FILE_TOOBIG)\
        break;\
        case EINTR:\
            RETURN_LOG_ERROR(i_TEXT_", failed due to a system interrupt",RE_INTERRUPT)\
        break;\
        case EIO:\
            RETURN_LOG_ERROR("While "i_TEXT_", a physical I/O error was encountered",RE_FILE_IO)\
        break;\
        case ENOSPC:\
        case ENOMEM:\
            RETURN_LOG_ERROR(i_TEXT_", failed due to the device containing the file having no free space",RE_FILE_NOSPACE)\
        break;\
        case ENXIO:\
            RETURN_LOG_ERROR(i_TEXT_", failed due to the device being non-existant",RE_FILE_NOTFILE)\
        break;\
        default:\
            RETURN_LOG_ERROR("There was a generic write error when "i_TEXT_,RE_FILE_WRITE)\
        break;\
    }}

//! A macro to check the result of file writting functions for I/O and jump to a label with goto
#define i_WRITE_CHECK_GOTO(i_FILE_,i_TEXT_,i_ERROR_,i_LABEL_) \
/*depending or errno*/\
if(ferror(i_FILE_) != 0){\
    switch(errno)\
    {\
        case EAGAIN:\
            LOG_ERROR("While "i_TEXT_", the write failed because the file was occupied by another thread and the no block flag was set",RE_FILE_WRITE_BLOCK)\
            i_ERROR_ = RE_FILE_WRITE_BLOCK;\
        break;\
        case EBADF:\
            LOG_ERROR("While "i_TEXT_", the file descriptor was found to be corrupt",RE_FILE_BAD)\
            i_ERROR_ = RE_FILE_BAD;\
        break;\
        case EFBIG:\
            LOG_ERROR("While "i_TEXT_", the file's size exceeded the limit",RE_FILE_TOOBIG)\
            i_ERROR_ = RE_FILE_TOOBIG;\
        break;\
        case EINTR:\
            LOG_ERROR(i_TEXT_", failed due to a system interrupt",RE_INTERRUPT)\
            i_ERROR_ = RE_INTERRUPT;\
        break;\
        case EIO:\
            LOG_ERROR("While "i_TEXT_", a physical I/O error was encountered",RE_FILE_IO)\
            i_ERROR_ = RE_FILE_IO;\
        break;\
        case ENOSPC:\
        case ENOMEM:\
            LOG_ERROR(i_TEXT_", failed due to the device containing the file having no free space",RE_FILE_NOSPACE)\
            i_ERROR_ = RE_FILE_NOSPACE;\
        break;\
        case ENXIO:\
            LOG_ERROR(i_TEXT_", failed due to the device being non-existant",RE_FILE_NOTFILE)\
            i_ERROR_ = RE_FILE_NOTFILE;\
        break;\
        default:\
            LOG_ERROR("There was a generic write error when "i_TEXT_,RE_FILE_WRITE)\
            i_ERROR_ = RE_FILE_WRITE;\
        break;\
    }\
    goto i_LABEL_;\
}
