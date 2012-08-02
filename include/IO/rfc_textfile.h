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

#ifndef RF_TEXTFILE_H
#define RF_TEXTFILE_H

#include <rf_options.h> // to include the options

#ifdef RF_MODULE_IO_TEXTFILE //module check


#include <rf_io.h>
#include <String/rfc_stringx.h>


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//The modes of file opening
//! The file is open for reading
#define RF_FILE_READ        1
//! The file is open for writing
#define RF_FILE_WRITE       2
//! The file is open for both reading and writing
#define RF_FILE_READWRITE   3
//! The file opens for writting and if it already exists its contents are erased
#define RF_FILE_NEW         4
//! Creates a new file for reading and writting. If the file already exists its contents are erased
#define RF_FILE_READWRITE_NEW 5



/**
** @internal
** @date 23/05/12
** @author Lefteris
** @endinternal
** @brief TextFile handler
**
** This is a TextFile handler that allows for the manipulation of files that contain
** text in any of the UTF encodings.
**
** It allows for manipulation of a text file without placing the whole file in memory
** and contains many different functions for moving inside the file in regards to specific line positions
** or even retrieving said lines themselves inside an @ref RF_String.
** For individual line parsing it is recommended to use the @ref RF_String and @ref RF_StringX functions.
**/
typedef struct RF_TextFile
{
    //! The file descriptor
    FILE* f;
    //! The name of the file
    RF_String name;
    //! The current line number. Basically represents the line that was last read by any of the file's functions
    uint64_t line;
    //! The encoding. Can be one of @c RF_UTF8, @c RF_UTF16 and @c RF_UTF32
    char encoding;
    //! A boolean flag denoting if the end of file was reached
    char eof;
    //! the mode of textfile opening. Can be one of @c RF_FILE_READ, @c RF_FILE_WRITE and @c RF_FILE_READWRITE
    char mode;
    //! the previous i/o operation on the file. Can be one of @c RF_FILE_READ, @c RF_FILE_WRITE
    char previousOp;
    //! A boolean flag denoting if the TextFile has a Byte Order Mark in the beginning or not
    char hasBom;
}RF_TextFile;

//! @name Creation functions
//! @{

//! @memberof RF_TextFile
//! @brief Initializes a new text file
//!
//! This function initializes a new textfile object. You have the option to either open an already existing TextFile by using
//! either @c RF_FILE_READ , @c RF_FILE_WRITE or @c RF_FILE_READWRITE for the @c mode argument. In that case the file shall also
//! be tested for the existence of a Byte Order Mark.
//!
//! To create a new file use either @c RF_FILE_NEW or @c RF_FILE_READWRITE_NEW. A Byte Order Mark will be appened to the file
//! depending on the given @c encoding argument unless the library has been compiled with the @c RF_OPTION_FILE_ADDBOM off.
//! Note that if a specific endianess for UTF-16 or UTF-32 is not given then the file will be created in the system's endianess.
//! @lmsFunction
//! @param t The textfile to initialize
//! @param name The name of the file. @inhtype{String,StringX} @tmpSTR
//! @param mode The mode with which to access the file. Should be one of the following values:
//! + @c RF_FILE_READ: Opens an already existing file for writting.
//! + @c RF_FILE_WRITE: Opens an already existing file for writting. If the file does not exist
//! it is created. If it exists its contents <b>are not</b> erased.
//! + @c RF_FILE_NEW: Opens a file for writing. If the file already exists
//!   its contents are erased.
//! + @c RF_FILE_READWRITE: Opens a file for both reading and writting. The file must exist.
//! + @c RF_FILE_READWRITE_NEW: Opens a new file both both reading and writting.
//!   If the file already exists its contents are erased.
//!
//! This parameter serves as but a hint of what operation should follow the initialization. The reason is that each
//! of the functions that operate on an @ref RF_TextFile check the current mode and if it is not the legal mode for
//! that operation the file is reopened with the proper mode, or has its file pointer rewinded and then put back if it is in read/write mode.
//! @warning You should note though, that there is a considerable overhead in reopening the file in a different mode
//! so you should make sure to pack the reading and writing operations in different places, or just use a different @ref RF_TextFile handler
//! for writing and a different one for reading.
//! @param encoding \rfoptional{@c RF_UTF8} The encoding of the file. Default is @c RF_UTF8. Can be one of:
//! + @c RF_UTF8: For Unicode UTF-8 encoding
//! + @c RF_UTF16_BE: For Unicode UTF-16 encoding in Big Endian endianess
//! + @c RF_UTF16_LE: For Unicode UTF-16 encoding in Little Endian endianess
//! + @c RF_UTF16: For Unicode UTF-16 encoding of unknown endianess. Will attempt to read the Byte Order Mark (BOM) character at the file's
//! beginning to determine the endianess and if that fails, search for a space character in the text file.
//! + @c RF_UTF32_BE: For Unicode UTF-32 encoding in Big Endian endianess
//! + @c RF_UTF32_LE: For Unicode UTF-32 encoding in Little Endian endianess
//! + @c RF_UTF32: For Unicode UTF-32 encoding of unknown endianess. Will attempt to read the Byte Order Mark (BOM) character at the file's
//! beginning to determine the endianess and if that fails, search for a space character in the text file.
//! @return Returns RF_SUCCESS if the initialization was succesfull. Otherwise it can return:
//! + @c RE_FILEMODE: If an illegal @c mode value was given
//! + @c RE_FILE_PERMISSION: If the file or some part of its path require permissions for the operation requested in mode that the calling program does not satisfy
//! + @c RE_INTERRUPT: If there is an interrupt signal by the OS while the attempted file opening
//! + @c RE_FILE_NOTFILE: If the given name is not actually a file. Either wrong format or name belongs to a directory.
//! + @c RE_FILE_MAXFILES: If the process has already reached the maximum number of open files/filedescriptors allowed by the system
//! + @c RE_FILE_NAMELENGTH: If the name of the file or the directory the file is in exceeds the maximum name length allowed by the system
//! + @c RE_FILE_NOSPACE: If we requested a creation of a new file and there is no space for it in the directory.
//! + @c RE_FILE_TOOBIG: If the requested file is of too big a size to be read
//! + @c RF_FAILURE: For a general unrecognized error
//! + @c RE_FILE_EOF: The end of file has been found in the beginning of the file and the file was opened for reading
//! Can also happen if @c RF_UTF16 is given for encoding and the file is scanned for a space character and none is found before EOF.
//! + @c RE_FILE_READ: If during reading the file there was an unknown read error
//! + @c RE_FILE_READ_BLOCK: If the read operation failed due to the file descriptor being occupied by another thread
//! + @c RE_FILE_POS_OVERFLOW: If during reading, the current file position can't be represented by the system
//! + @c RE_FILE_IO: If there was a physical I/O error
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfTextFile_Init(RF_TextFile* t,const void* name,char mode,char encoding);
#else
i_DECLIMEX_ int32_t i_rfTextFile_Init(RF_TextFile* t,const void* name,char* mode,char* encoding);
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfTextFile_Init(...)  RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_TEXTFILE_INIT,4,__VA_ARGS__)
        #define i_NPSELECT_RF_TEXTFILE_INIT1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Init() accepts from 3 to 4 arguments\"")
        #define i_NPSELECT_RF_TEXTFILE_INIT0(...) RF_SELECT_FUNC(i_SELECT_RF_TEXTFILE_INIT,__VA_ARGS__)
        #define i_SELECT_RF_TEXTFILE_INIT4(i_TEXTFILE_,i_NAME_,i_MODE_,i_ENCODING_) \
            i_rfLMSX_WRAP4(int32_t,i_rfTextFile_Init,i_TEXTFILE_,i_NAME_,i_RFI8_(i_MODE_),i_RFI8_(i_ENCODING_))
        #define i_SELECT_RF_TEXTFILE_INIT3(i_TEXTFILE_,i_NAME_,i_MODE_) \
            i_rfLMSX_WRAP4(int32_t,i_rfTextFile_Init,i_TEXTFILE_,i_NAME_,i_RFI8_(i_MODE_),i_RFI8_(RF_UTF8))
        #define i_SELECT_RF_TEXTFILE_INIT2(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Init() accepts from 3 to 4 arguments\"")
        #define i_SELECT_RF_TEXTFILE_INIT1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Init() accepts from 3 to 4 arguments\"")
        #define i_SELECT_RF_TEXTFILE_INIT0(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Init() accepts from 3 to 4 arguments\"")
    #else
        #define rfTextFile_Init(i_TEXTFILE_,i_NAME_,i_MODE_,i_ENCODING_)    \
            i_rfLMSX_WRAP4(int32_t,i_rfTextFile_Init,i_TEXTFILE_,i_NAME_,i_RFI8_(i_MODE_),i_RFI8_(i_ENCODING_))
    #endif
#endif
//! @memberof RF_TextFile
//! @brief Creates a new text file
//!
//! Allocates and returns a new text file
//! @lmsFunction
//! @param name The name of the file @inhtype{String,StringX} @tmpSTR
//! @param mode The mode with which to access the file. Should be one of the following values:
//! + @c RF_FILE_READ: Opens an already existing file for writting.
//! + @c RF_FILE_WRITE: Opens an already existing file for writting. If the file does not exist
//! it is created. If it exists its contents <b>are not</b> erased.
//! + @c RF_FILE_NEW: Opens a file for writing. If the file already exists
//!   its contents are erased.
//!
//! This parameter serves as but a hint of what operation should follow the initialization. The reason is that each
//! of the functions that operate on an @ref RF_TextFile check the current mode and if it is not the legal mode for
//! that operation the file is reopened with the proper mode, or has its file pointer rewinded and then put back if it is in read/write mode.
//! @warning You should note though, that there is a considerable overhead in reopening the file in a different mode
//! so you should make sure to pack the reading and writing operations in different places, or just use a different @ref RF_TextFile handler
//! for writing and a different one for reading.
//! @param encoding \rfoptional{@c RF_UTF8} The encoding of the file. Default is @c RF_UTF8. Can be one of:
//! + @c RF_UTF8: For Unicode UTF-8 encoding
//! + @c RF_UTF16_BE: For Unicode UTF-16 encoding in Big Endian endianess
//! + @c RF_UTF16_LE: For Unicode UTF-16 encoding in Little Endian endianess
//! + @c RF_UTF16: For Unicode UTF-16 encoding of unknown endianess. Will attempt to read the Byte Order Mark (BOM) character at the file's
//! beginning to determine the endianess and if that fails, search for a space character in the text file.
//! + @c RF_UTF32_BE: For Unicode UTF-32 encoding in Big Endian endianess
//! + @c RF_UTF32_LE: For Unicode UTF-32 encoding in Little Endian endianess
//! + @c RF_UTF32: For Unicode UTF-32 encoding of unknown endianess. Will attempt to read the Byte Order Mark (BOM) character at the file's
//! beginning to determine the endianess and if that fails, search for a space character in the text file.
//! @return Returns the newly allocated TextFile or 0 if there was a failure to initialize. In that case the reason is logged in the error log
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_TextFile* rfTextFile_Create(const void* name,char mode,char encoding);
#else
i_DECLIMEX_ RF_TextFile* i_rfTextFile_Create(const void* name,char* mode,char* encoding);
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfTextFile_Create(...)  RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_TEXTFILE_CREATE,3,__VA_ARGS__)
        #define i_NPSELECT_RF_TEXTFILE_CREATE1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Create() accepts from 2 to 3 arguments\"")
        #define i_NPSELECT_RF_TEXTFILE_CREATE0(...) RF_SELECT_FUNC(i_SELECT_RF_TEXTFILE_CREATE,__VA_ARGS__)
        #define i_SELECT_RF_TEXTFILE_CREATE3(i_NAME_,i_MODE_,i_ENCODING_) \
            i_rfLMS_WRAP3(RF_TextFile*,i_rfTextFile_Create,i_NAME_,i_RFI8_(i_MODE_),i_RFI8_(i_ENCODING_))
        #define i_SELECT_RF_TEXTFILE_CREATE2(i_NAME_,i_MODE_) \
            i_rfLMS_WRAP3(RF_TextFile*,i_rfTextFile_Create,i_NAME_,i_RFI8_(i_MODE_),i_RFI8_(RF_UTF8))
        #define i_SELECT_RF_TEXTFILE_CREATE1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Create() accepts from 2 to 3 arguments\"")
        #define i_SELECT_RF_TEXTFILE_CREATE0(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Create() accepts from 2 to 3 arguments\"")
    #else
        #define rfTextFile_Create(i_NAME_,i_MODE_,i_ENCODING_) \
            i_rfLMS_WRAP3(RF_TextFile*,i_rfTextFile_Create,i_NAME_,i_RFI8_(i_MODE_),i_RFI8_(i_ENCODING_))
    #endif
#endif

//! @}
//! @name Copying functions
//! @{

//! @memberof RF_TextFile
//! @brief Copies one textfile handle into another
//!
//! Copies textfile @c src into textfile @c dst
//! This does not create a whole new textfile in disk. It just creates an identical
//! handler that points to the same textfile at the same position
//! @param dst The destination textfile
//! @param src The source textfile
i_DECLIMEX_ void rfTextFile_Copy_IN(RF_TextFile* dst,RF_TextFile* src);
//! @memberof RF_TextFile
//! @brief Allocates and returns a copy of an RF_TextFile handler
//!
//! This does not create a whole new textfile in disk. It just creates an identical
//! handler that points to the same textfile at the same position
//! @param src The source textfile
//! @return A newly allocated copy of @c src
i_DECLIMEX_ RF_TextFile* rfTextFile_Copy_OUT(RF_TextFile* src);

//!@}
//!@name Destruction functions
//!@{

//! @memberof RF_TextFile
//! @brief Deinitalizes a text file
//!
//! Use it to cleanup text files created with @ref rfTextFile_Init
//! @param t The text file to cleanup
i_DECLIMEX_ void rfTextFile_Deinit(RF_TextFile* t);
//! @memberof RF_TextFile
//! @brief Destroys a text file
//!
//! Use it to cleanup text files created with @ref rfTextFile_Create
//! @param t The text file to cleanup
i_DECLIMEX_ void rfTextFile_Destroy(RF_TextFile* t);

//! @}

//! @memberof RF_TextFile
//! @brief Changes the file access mode of the TextFile
//!
//! @param mode Give the new file access mode here. The only legal values are:
//! + @c RF_FILE_READ: So that the file stream goes into reading mode
//! + @c RF_FILE_WRITE: So that the file stream goes into writting mode
//! + @c RF_FILE_READWRITE: So that the file stream goes into read/write mode.
//!
//! If another value is given this function returns @c RE_INPUT.
//! @return Returns @c RF_SUCCESS for succesfull change of file access mode or if the same one as the current
//! has been given. In failure it can return one of :
//! + @c RE_INPUT: If illegal mode value has been provided
//! + @c RE_FILE_PERMISSION: If the file or some part of its path require permissions for the operation requested in mode that the calling program does not satisfy
//! + @c RE_INTERRUPT: If there is an interrupt signal by the OS while the attempted file opening
//! + @c RE_FILE_NOTFILE: If the given name is not actually a file. Either wrong format or name belongs to a directory.
//! + @c RE_FILE_MAXFILES: If the process has already reached the maximum number of open files/filedescriptors allowed by the system
//! + @c RE_FILE_NAMELENGTH: If the name of the file or the directory the file is in exceeds the maximum name length allowed by the system
//! + @c RE_FILE_NOSPACE: If we requested a creation of a new file and there is no space for it in the directory.
//! + @c RE_FILE_TOOBIG: If the requested file is of too big a size to be read
//! + @c RF_FAILURE: For a general unrecognized error
i_DECLIMEX_ int32_t rfTextFile_SetMode(RF_TextFile* t,char mode);


//! @name Retrieval functions
//! @{

//! @memberof RF_TextFile
//! @brief Moves the file pointer by a given number of lines relative to the current line
//!
//! Moves the file pointer at the beginning of the line at @c linesN forward if @c linesN is a positive
//! number or at the beginning of the line located @c linesN behind the current line if the number is negative.
//! If the @c linesN number is negative and its absolute value is equal to or greater to the current line then
//! nothing happens and @c RF_FAILURE is returned.
//!
//! Also if the given @c linesN is negative and takes the line too far back and going to that line
//! would be faster through the beginning of the file then the function understands that and starts
//! moving to that line from the file's beginning
//!
//! If @c linesN is 0 then the function will move the file pointer to the
//! beginning of the current line if it is not already there
//!
//! Should the function attempt to move further back than the first line of the file then @c RF_FAILURE will be returned
//! and the @c line will be the the same.
//!
//! If the function fails for any of the fatal reasons given at the return description then an attempt to restore the
//! file pointer to the position before the function call is made. That attempt may very well fail because
//! of a corrupt file that lead to the error in the first place.
//! @param[in] t The Text File to operator on
//! @param[in] linesN The number of lines to move the TextFile's internal pointer.
//! Can be either negative or positive number. If it is a positive number then the pointer is moved forward
//! and if it is negative it is moved backwards. If it is negative, its absolute value should be less than the current line.
//! Can't move further back than the first line. Should not be zero. If it is zero, nothing happens.
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer. In case of failure returns one of:
//! + @c RF_FAILURE: If during moving a negative amount of lines, the start of the file is encountered.
//! + @c RE_FILE_READ: If during reading the file there was an error
//! + @c RE_FILE_READ_BLOCK: If during reading the file the read operation failed due to the file being occupied by another thread
//! + @c RE_FILE_MODE: If during reading the file the underlying file descriptor's mode was not correctly set for reading
//! + @c RE_FILE_POS_OVERFLOW: If during trying to read the current file's position it can't be represented by the system
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
//! + @c RE_FILE_WRITE_BLOCK: While attempting to move the file pointer, it was occupied by another thread, and the no block flag was set
//! + @c RE_INTERRUPT: Operating on the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_EOF: If the end of file is reached while attempting to move the required amount of @c linesN
i_DECLIMEX_ int32_t rfTextFile_MoveLines(RF_TextFile* t,int64_t linesN);


//! @memberof RF_TextFile
//! @brief Moves the internal file pointer by a number of characters
//!
//! Moves the file pointer @c charsN characters forward if positive and backwards if negative.
//!
//!
//! Should the function attempt to move further back than the first character of the file then @c RF_FAILURE
//! will be returned and the file pointer will remain at the same position
//!
//! If the function fails for any of the fatal reasons given at the return description then an attempt to restore the
//! file pointer to the position before the function call is made. That attempt may very well fail because
//! of a corrupt file that lead to the error in the first place.
//! @param[in] t The Text File to operate on
//! @param[in] charsN The number of characters to move. Can be either negative or positive
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer. In case of failure returns one of:
//! + @c RF_FAILURE: If during moving a negative amount of lines, the start of the file is encountered.
//! + @c RE_FILE_READ: If during reading the file there was an error
//! + @c RE_FILE_READ_BLOCK: If during reading the file the read operation failed due to the file being occupied by another thread
//! + @c RE_FILE_MODE: If during reading the file the underlying file descriptor's mode was not correctly set for reading
//! + @c RE_FILE_POS_OVERFLOW: If during trying to read the current file's position it can't be represented by the system
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
//! + @c RE_FILE_WRITE_BLOCK: While attempting to move the file pointer, it was occupied by another thread, and the no block flag was set
//! + @c RE_INTERRUPT: Operating on the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_EOF: If the end of file is reached while attempting to move the required amount of @c linesN
i_DECLIMEX_ int32_t rfTextFile_MoveChars(RF_TextFile* t,int64_t charsN);

//! @memberof RF_TextFile
//! @brief Moves the internal file pointer forward by an amount of characters
//!
//! If the End of file is encountered while attempting to move a character forward then the function
//! will return @c RE_FILE_EOF
//!
//! @warning If a file reading error occurs at this function, unlike other functions of RF_TextFile
//! this one will not attempt to reset the pointer back to its original position
//! @param[in] t The Text File to operate on
//! @param[in] charsN The number of characters to move forward.
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer. In case of failure it can return
//! any one of the errors that are returned by @ref rfFgetc_UTF8() or @ref rfFgetc_UTF16LE() or
//! @red rfFgetc_UTF32LE() depending on the text file's encoding
i_DECLIMEX_ int32_t rfTextFile_MoveChars_f(RF_TextFile* t,uint64_t charsN);
//! @memberof RF_TextFile
//! @brief Moves the internal file pointer backward by an amount of characters
//!
//! @warning If a file reading error occurs at this function, unlike other functions of RF_TextFile
//! this one will not attempt to reset the pointer back to its original position
//! @param[in] t The Text File to operate on
//! @param[in] charsN The number of characters to move backward
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer. In case of failure it can return
//! any one of the errors that are returned by @ref rfFBack_UTF8() or @ref rfFBack_UTF16LE() or
//! @red rfFBack_UTF32LE() depending on the text file's encoding
i_DECLIMEX_ int32_t rfTextFile_MoveChars_b(RF_TextFile* t,uint64_t charsN);

//! @memberof RF_TextFile
//! @brief Moves the file pointer at the start of the given lines
//!
//! Moves the file pointer at the beginning of the line at @c lineN and it is as if
//! the line has not been read. The outcome of the next @ref rfTextFile_ReadLine() will
//! provide the contents of that line. Line Indexing starts from @c 1.
//!
//! If the function fails for any of the reasons given at the return description then an attempt to restore the
//! file pointer to the position before the function call is made. That attempt may very well fail because
//! of a corrupt file that lead to the error in the first place.
//! @param[in] t The Text File to operator on
//! @param[in] lineN The line at the beginning of which to move the file pointer. If @c 0 is given
//! @c RE_INPUT is returned
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer. In case of failure returns one of:
//! + @c RE_INPUT: @c 0 value was given for the @c lineN argument
//! Any of the possible return values of the @ref rfTextFile_MoveLines() function
i_DECLIMEX_ int32_t rfTextFile_GoToLine(RF_TextFile* t,uint64_t lineN);

//! @memberof RF_TextFile
//! @brief Moves the file pointer at the given offset
//!
//! Moves the file pointer at the specified file offset also updating the internal data
//! of the text file. You can given the same flags given at fseek() function except @c SEEK_END.
//!
//! A subsequent readline after this functions would read from the current file position
//! until the next newline character is found.
//!
//! If invalid input is given then the function shall return @c RE_INPUT. Invalid input constitute an illegal value
//! at @c origin argument and illegal combinations of @c offset and @c origin. For instance you can't expect to
//! have a negative offset with @c origin starting from the start of the file
//!
//! If the offset would take the file pointer to the end of the file or beyond then @c RE_FILE_EOF is returned
//! and the position is returned to where it was before the function call.
//!
//! If @c offset would take the file pointer in between bytes that are not a legal value in the corresponding encoding
//! the file is in then the function will actually move enough additional bytes forward (or backwards) to encompass the whole
//! unicode codepoint.
//!
//! If the function fails for any of the reasons given at the return description then an attempt to restore the
//! file pointer to the position before the function call is made. That attempt may very well fail because
//! of a corrupt file that lead to the error in the first place.
//! @param[in] t The Text File to operate on
//! @param[in] offset The offset to move the internal file pointer to. Can be both negative and positive
//! and it depends on the position given at @c origin
//! @param[in] origin Just like fseek this denotes from where should the offset moving be performed.
//! + @c SEEK_SET: From the beginning of the file
//! + @c SEEK_CUR: From the current file position
//!
//! Unlike fseek this function does not accept offset moving from the end of the file. So @c SEEK_END
//! is not a legal value
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer. In case of failure returns one of:
//! + @c RF_FAILIRE: If the request would take the file pointer beyond the start of the file going backwards
//! + @c RE_FILE_READ: If during reading the file there was an error
//! + @c RE_FILE_READ_BLOCK: If during reading the file the read operation failed due to the file being occupied by another thread
//! + @c RE_FILE_MODE: If during reading the file the underlying file descriptor's mode was not correctly set for reading
//! + @c RE_FILE_POS_OVERFLOW: If during trying to read the current file's position it can't be represented by the system
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
//! + @c RE_FILE_WRITE_BLOCK: While attempting to move the file pointer, it was occupied by another thread, and the no block flag was set
//! + @c RE_INTERRUPT: Operating on the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_EOF: If the end of file is reached while attempting to move to the requested @c lineN
//! + @c RE_INPUT: If @c origin is not a legal value or the combination of @c offset and @c origin does not make sense
i_DECLIMEX_ int32_t rfTextFile_GoToOffset(RF_TextFile* t,foff_rft offset,int origin);

//! @memberof RF_TextFile
//! @brief Reads the next line of the text file
//!
//! Reads the next line of the Text File and moves the file pointer to the beginning of the next line.
//! If the user does not provide a @c characters value then the whole line is returned inside the
//! string @c line. If a value is provided then only the first @c characters characters are returned
//! inside line. No matter the amount of character returned in the string the file pointer moves a whole line.
//!
//! In order to use this function
//! you need to provide an <b>already initialized</b> RF_StringX that will act as the reading buffer. Providing
//! an uninitialized RF_StringX leads to a segmentation fault or worse.
//! After you are done with processing each line of the file, then and only then should you call rfStringX_Deinit or rfStringX_Destroy on
//! the @c line string.
//!
//! If the library has been compiled with @c DEFAULT_ARGUMENTS off then there is a second function called
//! @c rfTextFile_ReadLine2() that can be called when you are sure you want all the characters of the text file.
//! This function exists also if the compile option @c DEFAULT_ARGUMENTS is on, but can be called if the last argument @c characters
//! of @c rfTextFile_ReadLine() is omitted.
//! @param[in] TextFile A pointer to the text file
//! @param[in,out] line Give an already initialized @ref RF_StringX buffer that will act as the line buffer of the file.
//!  The reason this parameter is a StringX is due to the need for text processing.
//! @param[in] characters \rfoptional{0} Give the number of characters that you would like to read from the line into the String. If @c 0 is given the whole line is read into the string.
//! @note No matter the choice of characters here the file pointer will still move to the start of the next line.
//! @return Returns either RF_SUCCESS for successful read or:
//! + The same errors as @ref rfStringX_Assign_fUTF8(), rfStringX_Assign_fUTF16() or rfStringX_Assign_fUTF32() can produce
//! + @c RE_FILE_EOF: If the end of the file is reached.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfTextFile_ReadLine(RF_TextFile* t,RF_StringX* line,uint32_t characters);
i_DECLIMEX_ int32_t rfTextFile_ReadLine2(RF_TextFile* t,RF_StringX* line);
#else
//in the default arguments case. breaking down the function into two different ones since that would simplify the 2 args call
i_DECLIMEX_ int32_t i_rfTextFile_ReadLine3(RF_TextFile* t,RF_StringX* line,uint32_t characters);
i_DECLIMEX_ int32_t rfTextFile_ReadLine2(RF_TextFile* t,RF_StringX* line);
#define rfTextFile_ReadLine(...)  RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_TEXTFILE_READLINE,3,__VA_ARGS__)
#define i_NPSELECT_RF_TEXTFILE_READLINE1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_ReadLine() accepts from 2 to 3 arguments\"")
#define i_NPSELECT_RF_TEXTFILE_READLINE0(...) RF_SELECT_FUNC(i_SELECT_RF_TEXTFILE_READLINE,__VA_ARGS__)
#define i_SELECT_RF_TEXTFILE_READLINE3(...) i_rfTextFile_ReadLine3(__VA_ARGS__)
#define i_SELECT_RF_TEXTFILE_READLINE2(...) rfTextFile_ReadLine2(__VA_ARGS__)
#define i_SELECT_RF_TEXTFILE_READLINE1(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_ReadLine() accepts from 2 to 3 arguments\"")
#define i_SELECT_RF_TEXTFILE_READLINE0(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_ReadLine() accepts from 2 to 3 arguments\"")
#endif


//! @memberof RF_TextFile
//! @brief Gets the current byte offset of the file
//!
//! @param[in] t The textfile whose offset to get
//! @param[out] offset Give a reference to an @c foff_rft type to receive the
//! file offset of the Textfile
//! @return Returns RF_SUCCESS for succesfull offset retrival or error otherwise. The possible errors are:
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
i_DECLIMEX_ int32_t rfTextFile_GetOffset(RF_TextFile* t,foff_rft* offset);

//! @memberof RF_TextFile
//! @brief Gets a specific line from a Text File starting from the beginning of the file
//!
//! Searches the text file for line @c lineN with the first line being @c 1. When found it returns the line
//! by assigning @c line's contents to the given string. The user is responsible
//! of freeing that string later himself.
//!
//! The function starts a search for the appropriate line from the beginning of the file retrieving the lines
//! one by one until it reaches the required line at @c lineN. For a function that deduces how to search for the line
//! by comparing the current line with the requested one take a look at @ref rfTextFile_GetLine()
//!
//! If no line @c lineN exists and the file ends before that line is found then the functions returns
//! @c RE_FILE_EOF and nothing happens to the passed string at @c line.
//!
//! An important thing to note about the usage of this function is that when a line is found
//! the internal file pointer and the current line are not changed. This just retrieves the lines
//! without moving the file pointer or changing the line.
//! @param[in] t The textfile to get the line from
//! @param[in] lineN The number of line to retrieve. Indexing starts from @c 1 for the first line.
//! Do not give 0 here. If 0 is given then @c RE_INPUT error is returned
//! @param[out] line Here give an @ref RF_StringX that will receive the read line. The string must
//! <b>already be initialized</b> before being passed to the function. It is recommended to initialize a string
//! with @ref rfStringX_Init_txtbuff() macro
//! @return Returns @c RF_SUCCESS if everything went all right. If there was a failure it can return any
//! of the errors returned by @ref rfTextFile_MoveLines(). It can also return @c RE_INPUT if the given @c lineN
//! is 0.
i_DECLIMEX_ int32_t rfTextFile_GetLine_begin(RF_TextFile* t,uint64_t lineN,RF_StringX* line);
//! @memberof RF_TextFile
//! @brief Gets a specific line from a Text File
//!
//! Searches the text file for line @c lineN with the first line being @c 1. When found it returns the line
//! by assigning @c line's contents to the given string.
//!
//! The function compares the requested line with the line that the file pointer is
//! currently in. Then it deduces which method to use to find it. Either search from the beginning
//! of the file or search from the current line until the given line is found.
//!
//! If no line @c lineN exists and the file ends before that line is found then the functions returns
//! @c RE_FILE_EOF and nothing happens to the passed string at @c line.
//!
//! An important thing to note about the usage of this function is that when a line is found
//! the internal file pointer and the current line are not changed. This just retrieves the lines
//! without moving the file pointer or changing the line.
//! @param[in] t The textfile to get the line from
//! @param[in] lineN The number of line to retrieve. Indexing starts from @c 1.
//!  Do not give 0 here. If 0 is given then @c RE_INPUT error is returned
//! @param[out] line Here give an @ref RF_StringX that will receive the read line. The string must
//! <b>already be initialized</b> before being passed to the function. It is recommended to initialize a string
//! with @ref rfStringX_Init_txtbuff() macro
//! @return Returns @c RF_SUCCESS if everything went all right. If there was a failure it can return any
//! of the errors returned by @ref rfTextFile_MoveLines(). It can also return @c RE_INPUT if the given @c lineN
//! is 0.
i_DECLIMEX_ int32_t rfTextFile_GetLine(RF_TextFile* t,uint64_t lineN,RF_StringX* line);

//! @}

//! @name File Editing Functions
//! @{

//! @memberof RF_TextFile
//! @brief Writes an RF_String to the current file position
//!
//! Writes the given @c string at the current file position
//!
//! The given string may contain   new lines inside and hence
//! write more than one line with a single function call.
//! The given string's newlines must be only represented
//! by the '\n' character. The function shall handle the file line endings separately by editing
//! the string before inputting it into the file if so needed.
//! @lmsFunction
//! @note In this function if the given @c string does not end with a newline then one is not
//! automatically appended by the function.
//! @param t The textfile to write to
//! @param string The string to add to the end of the textfile  @inhtype{String,StringX} @tmpSTR
//! @return Returns RF_SUCCESS in succeful writing and an error otherwise. The possible errors are:
//! + @c  RE_FILE_MODE_CHANGE: If the text file was in reading mode and failure occured during attempt to change it to writting
//! + @c RE_FILE_WRITE_BLOCK: While writing to the file, it was occupied by another thread, and the no block flag was set
//! + @c RE_FILE_BAD: The file descriptor provided is corrupt
//! + @c RE_INTERRUPT: Writing to the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file, left for a write operation at the point we attempted to write.
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_WRITE: There was a generic write error
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfTextFile_Write(RF_TextFile* t,void* string);
#else
i_DECLIMEX_ int32_t i_rfTextFile_Write(RF_TextFile* t,void* string);
#define rfTextFile_Write(i_FILE_,i_STR_) i_rfLMSX_WRAP2(int32_t,i_rfTextFile_Write,i_FILE_,i_STR_)
#endif

//! @memberof RF_TextFile
//! @brief Inserts a line into a specific part of the Text File
//!
//! Inserts the contents of @c string inside the text file  either right after
//! @c lineN line if @c after is @c true or right before it if it is @c false.
//!
//! This function inserts a line, so it will append a newline character at the end
//! of the @c string before inserting. The given string may contain many new lines inside and hence
//! replace one line with multiple one. The given string's newlines must be only represented
//! by the '\n' character. The function shall handle the file line endings separately by editing
//! the string before inputting it into the file if so needed.
//! The given string must not end with newline, it is automatically appended by the function.
//!
//! If the End Of File is encountered before the line is found then the function returns
//! @c RE_FILE_EOF
//!
//! Also notice that no matter the outcome of this function (success or failure) the file
//! position pointer is not put back to where it was before the function call. Only in case
//! of success the file pointer is put to the beginning of the newly inserted line
//! @lmsFunction
//! @param t The TextFile to operate on
//! @param lineN Give the line number before of after which the @c string will be placed.
//! Line indexing starts from @c 1 and up until the last line of the file. If a bigger line
//! than the file's last line is given then an error will occur since the requested line will
//! not be found.
//! Do not give 0 here. If 0 is given the function shall return @c RF_FAILURE
//! @param string The string to insert to the textfile either after or before @c lineN
//! A newline character will be appended by this function inside the string to make it a line
//! but you can also contain other newline characters inside, in essence entering multiple lines to
//! the file.  @inhtype{String,StringX} @tmpSTR
//! @param after \rfoptional{false} A boolean denoting where we want to place the new line. If @c true then
//! the new line is placed after the line at @c lineN and if false beforee
//! @return Returns RF_SUCCESS for succesful insertion. If the function fails it can return one of:
//! + @c RF_FAILURE: If the given @c lineN is 0
//! + @c RE_FILE_POS_OVERFLOW: If during trying to read the current file's position it can't be represented by the system
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
//! + @c RE_FILE_WRITE_BLOCK: While attempting to move the file pointer, it was occupied by another thread, and the no block flag was set
//! + @c RE_INTERRUPT: Operating on the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_EOF: If the end of file is reached before finding the required line
//! + @c RE_FILE_DELETE: If after finishing the operation of insertion the temporary file could not be deleted
//! + @c RE_FILE_RENAME: If while trying to rename the temporary file to replace this one the renaming failed
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfTextFile_Insert(RF_TextFile* t,uint64_t lineN,void* string,char after);
#else
i_DECLIMEX_ int32_t i_rfTextFile_Insert(RF_TextFile* t,uint64_t* lineN,void* string,char* after);
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfTextFile_Insert(...)  RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_TEXTFILE_INSERT,4,__VA_ARGS__)
        #define i_NPSELECT_RF_TEXTFILE_INSERT1(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Insert() accepts from 3 to 4 arguments\"")
        #define i_NPSELECT_RF_TEXTFILE_INSERT0(...)  RF_SELECT_FUNC(i_SELECT_RF_TEXTFILE_INSERT,__VA_ARGS__)
        #define i_SELECT_RF_TEXTFILE_INSERT4(i_TEXTFILE_,i_LINEN_,i_STR_,i_AFTER_) \
            i_rfLMSX_WRAP4(int32_t,i_rfTextFile_Insert,i_TEXTFILE_,i_RFUI64_(i_LINEN_),i_STR_,i_RFI8_(i_AFTER_))
        #define i_SELECT_RF_TEXTFILE_INSERT3(i_TEXTFILE_,i_LINEN_,i_STR_) \
            i_rfLMSX_WRAP4(int32_t,i_rfTextFile_Insert,i_TEXTFILE_,i_RFUI64_(i_LINEN_),i_STR_,i_RFI8_(true))
        #define i_SELECT_RF_TEXTFILE_INSERT2(...)   RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Insert() accepts from 3 to 4 arguments\"")
        #define i_SELECT_RF_TEXTFILE_INSERT1(...)   RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Insert() accepts from 3 to 4 arguments\"")
        #define i_SELECT_RF_TEXTFILE_INSERT0(...)   RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_Insert() accepts from 3 to 4 arguments\"")
    #else
        #define rfTextFile_Insert(i_TEXTFILE_,i_LINEN_,i_STR_,i_AFTER_) \
            i_rfLMSX_WRAP4(int32_t,i_rfTextFile_Insert,i_TEXTFILE_,i_RFUI64_(i_LINEN_),i_STR_,i_RFI8_(i_AFTER_))
    #endif
#endif

//! @memberof RF_TextFile
//! @brief Removes a specific line from the text file
//!
//! Searches the text file for line @c lineN and removes the line
//! if it manages to find it.
//!
//! If the End Of File is encountered before the line is found then the function returns
//! @c RE_FILE_EOF
//!
//! @param t The TextFile to operate on
//! @param lineN Give the number of the line to remove. Indexing starts from @c 0. If 0 is given the function shall return @c RF_FAILURE
//! @return Returns RF_SUCCESS for succesful removal. If the function fails it can return one of:
//! + @c RF_FAILURE: If the given @c lineN is 0
//! + @c RE_FILE_POS_OVERFLOW: If during trying to read the current file's position it can't be represented by the system
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
//! + @c RE_FILE_WRITE_BLOCK: While attempting to move the file pointer, it was occupied by another thread, and the no block flag was set
//! + @c RE_INTERRUPT: Operating on the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_EOF: If the end of file is reached before finding the required line
//! + @c RE_FILE_DELETE: If after finishing the operation of insertion the temporary file could not be deleted
//! + @c RE_FILE_RENAME: If while trying to rename the temporary file to replace this one the renaming failed
i_DECLIMEX_ int32_t rfTextFile_Remove(RF_TextFile* t,uint64_t lineN);

//! @memberof RF_TextFile
//! @brief Replaces a line of the textfile with another one
//!
//! Searches the text file for line @c lineN and replaces it with
//! @c string.
//!
//! The given string may contain many new lines inside and hence
//! replace one line with multiple one. The given string's newlines must be only represented
//! by the '\n' character. The function shall handle the file line endings separately by editing
//! the string before inputting it into the file if so needed.
//! The given string must not end with newline, it is automatically appended by the function.
//!
//! If the End Of File is encountered before the line is found then the function returns
//! @c RE_FILE_EOF
//!
//! Also notice that no matter the outcome of this function (success or failure) the file
//! position pointer is not put back to where it was before the function call.  Only in case
//! of success the file pointer is put to the beginning of the newly replaced line
//! @lmsFunction
//! @param t The TextFile to operate on
//! @param lineN Give the number of the line to replace. Indexing starts from @c 1.
//! Do not give 0 here. If 0 is given the function shall return @c RF_FAILURE
//! @param string The string to replace the line located at @c lineN.
//! A newline character will be appended by this function inside the string to make it a line
//! but you can also contain other newline characters inside, in essence replacing the line with multiple lines
//! @inhtype{String,StringX} @tmpSTR
//! @return Returns RF_SUCCESS for succesful replacement. If the function fails it can return one of:
//! + @c RF_FAILURE: If the given @c lineN is 0
//! + @c RE_FILE_POS_OVERFLOW: If during trying to read the current file's position it can't be represented by the system
//! + @c RE_FILE_BAD: If The file descriptor is corrupt/illegal
//! + @c RE_FILE_NOTFILE: If the file descriptor is not a file but something else. e.g. socket. Means that the @c RF_TextFile is either uninitialized or corrupted
//! + @c RE_FILE_GETFILEPOS: If the file's position could not be retrieved for some unknown reason
//! + @c RE_FILE_WRITE_BLOCK: While attempting to move the file pointer, it was occupied by another thread, and the no block flag was set
//! + @c RE_INTERRUPT: Operating on the file failed due to a system interrupt
//! + @c RE_FILE_IO: There was a physical I/O error
//! + @c RE_FILE_NOSPACE: There was no space on the device holding the file
//! + @c RE_FILE_NOTFILE: The device we attempted to write to is non-existent
//! + @c RE_FILE_EOF: If the end of file is reached before finding the required line
//! + @c RE_FILE_DELETE: If after finishing the operation of insertion the temporary file could not be deleted
//! + @c RE_FILE_RENAME: If while trying to rename the temporary file to replace this one the renaming failed
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfTextFile_Replace(RF_TextFile* t,uint64_t lineN,void* string);
#else
i_DECLIMEX_ int32_t i_rfTextFile_Replace(RF_TextFile* t,uint64_t* lineN,void* string);
#define rfTextFile_Replace(i_TEXTFILE_,i_LINEN_,i_STR_) \
i_rfLMSX_WRAP3(int32_t,i_rfTextFile_Replace,i_TEXTFILE_,i_RFUI64_(i_LINEN_),i_STR_)
#endif

//! @}
#ifdef __cplusplus
}//closing bracket for calling from C++
#endif

#else//end of module check
    #error Attempted to include Refu TextFile module with the module flag off. Rebuild the library with that option added if you need to include it
#endif

#endif //include guards end
