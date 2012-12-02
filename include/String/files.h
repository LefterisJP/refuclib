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
#ifndef RF_STRING_FILES_H
#define RF_STRING_FILES_H


#include <rf_setup.h> //for the macro flags
#include <String/string_decl.h>

#include <rf_io.h> //for RF_EOL macros
#include <IO/rf_unicode.h> //for the unicode macros RF_UTF8 and friends

#include <stdio.h> //for FILE*

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/*------------------------------------------------------------------------ RF_String File Descriptor I/O functions-------------------------------------------------------------------------------*/
//! @name String File Descriptor I/O functions
//! @{

//! @memberof RF_String
//! @brief Allocates and returns a string from UTF-8 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_String
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valide sequence of bytes is performed.
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return The initialized string or null pointer in case of failure to read the file, or unexpected data (non-UTF8 encoded string)
//! @see rfString_Init_fUTF8()
//! @see rfString_Assign_fUTF8()
//! @see rfString_Append_fUTF8()
i_DECLIMEX_ RF_String* rfString_Create_fUTF8(FILE* f, char* eof,char eol);
//! @memberof RF_String
//! @brief Initializes a string from UTF-8 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_String
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valide sequence of bytes is performed.
//! @param str The extended string to initialize
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull initialization that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF8() can produce.
//! @see rfString_Create_fUTF8()
//! @see rfString_Assign_fUTF8()
//! @see rfString_Append_fUTF8()
i_DECLIMEX_ int32_t rfString_Init_fUTF8(RF_String* str,FILE* f, char* eof,char eol);

//! @memberof RF_String
//! @brief Assigns to a string from UTF-8 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and assigns it to an RF_StringX
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valide sequence of bytes is performed.
//! @param str The extended string to assign to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this assignment
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull assignment that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF8() can produce.
//! @see rfString_Init_fUTF8()
//! @see rfString_Create_fUTF8()
//! @see rfString_Append_fUTF8()
i_DECLIMEX_ int32_t rfString_Assign_fUTF8(RF_String* str,FILE* f, char* eof,char eol);
//! @memberof RF_String
//! @brief Appends to a string from UTF-8 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and appends it to an RF_StringX
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valid sequence of bytes is performed.
//! @param str The extended string to append to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this appending
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull appending that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF8() can produce.
//! @see rfString_Init_fUTF8()
//! @see rfString_Create_fUTF8()
//! @see rfString_Assign_fUTF8()
i_DECLIMEX_ int32_t rfString_Append_fUTF8(RF_String* str,FILE* f, char* eof,char eol);

//! @memberof RF_String
//! @cppnotctor
//! @brief Allocates and returns a string from UTF-16 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return The initialized string or null pointer in case of failure to read the file
//! @see rfString_Init_fUTF16()
//! @see rfString_Append_fUTF16()
//! @see rfString_Assign_fUTF16()
i_DECLIMEX_ RF_String* rfString_Create_fUTF16(FILE* f, char endianess,char* eof,char eol);
//! @memberof RF_String
//! @brief Initializes a string from UTF-16 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to initialize
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull initialization that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF16LE() can produce.
//! @see rfString_Create_fUTF16()
//! @see rfString_Append_fUTF16()
//! @see rfString_Assign_fUTF16()
i_DECLIMEX_ int32_t rfString_Init_fUTF16(RF_String* str,FILE* f, char endianess,char* eof,char eol);

//! @memberof RF_String
//! @brief Appends the contents of a UTF-16 file a String
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and appends it to an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to append to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this appending
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull appending that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF16LE() can produce.
//! @see rfString_Init_fUTF16()
//! @see rfString_Create_fUTF16()
//! @see rfString_Assign_fUTF16()
i_DECLIMEX_ int32_t rfString_Append_fUTF16(RF_String* str,FILE* f, char endianess,char* eof,char eol);
//! @memberof RF_String
//! @brief Assigns the contents of a UTF-16 file to an already initialized string
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and assigns it to an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to assign to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this assignment
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull assignment that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF16LE() can produce.
//! @see rfString_Init_fUTF16()
//! @see rfString_Create_fUTF16()
//! @see rfString_Append_fUTF16()
i_DECLIMEX_ int32_t rfString_Assign_fUTF16(RF_String* str,FILE* f, char endianess,char* eof,char eol);

//! @memberof RF_String
//! @cppnotctor
//! @brief Allocates and returns a string from UTF-32 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return The initialized string or null pointer in case of failure to read the file
//! @see rfString_Init_fUTF32()
//! @see rfString_Append_fUTF32()
//! @see rfString_Assign_fUTF32()
i_DECLIMEX_ RF_String* rfString_Create_fUTF32(FILE* f,char endianess, char* eof, char eol);
//! @memberof RF_String
//! @brief Initializes a string from UTF-32 file parsing
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to initialize
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull initialization that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF32LE() can produce.
//! @see rfString_Create_fUTF32()
//! @see rfString_Append_fUTF32()
//! @see rfString_Assign_fUTF32()
i_DECLIMEX_ int32_t rfString_Init_fUTF32(RF_String* str,FILE* f,char endianess, char* eof,char eol);
//! @memberof RF_String
//! @brief Assigns the contents of a UTF-32 file to a string
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and assigns it as the contents of the given RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to assign to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this assignment
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull assignment that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF32LE() can produce.
//! @see rfString_Init_fUTF32()
//! @see rfString_Create_fUTF32()
//! @see rfString_Append_fUTF32()
i_DECLIMEX_ int32_t rfString_Assign_fUTF32(RF_String* str,FILE* f,char endianess, char* eof,char eol);
//! @memberof RF_String
//! @brief Appends the contents of a UTF-32 file to a string
//!
//! @notinherited{StringX}
//! Read the file stream @c f until either a newline character or the EOF is reached and appends to the given RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to append to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this appending
//! @param[in] eol The End Of Line type that this file uses. Can be one of:
//! + @c RF_EOL_LF: For Unix-style line endings, taking @c '\n' as the end of line signal
//! + @c RF_EOL_CR: For Macintosh-style line endings, taking @c '\r' as the end of line signal
//! + @c RF_EOL_CRLF: For Windows-style line endings, taking @c "\r\n" as the end of line signal
//!
//! @return Returns either a positive number for succesfull appending that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF32LE() can produce.
//! @see rfString_Init_fUTF32()
//! @see rfString_Create_fUTF32()
//! @see rfString_Assign_fUTF32()
i_DECLIMEX_ int32_t rfString_Append_fUTF32(RF_String* str,FILE* f,char endianess, char* eof,char eol);

//! @memberof RF_String
//! @brief Writes a string to a file depending on the given encoding
//!
//! @isinherited{StringX}
//! This function shall output the string @c s into the file descriptor @c f in the given @c encoding .
//! @lmsFunction
//! @param s The string to write to the file @inhtype{String,StringX} @tmpSTR
//! @param f A valid and open file pointer into which to write the string.
//! @param encoding \rfoptional{@c RF_UTF8} The encoding of the file. Default is @c RF_UTF8. Can be one of:
//! + @c RF_UTF8: For Unicode UTF-8 encoding
//! + @c RF_UTF16_BE: For Unicode UTF-16 encoding in Big Endian endianess
//! + @c RF_UTF16_LE: For Unicode UTF-16 encoding in Little Endian endianess
//! + @c RF_UTF32_BE: For Unicode UTF-32 encoding in Big Endian endianess
//! + @c RF_UTF32_LE: For Unicode UTF-32 encoding in Little Endian endianess
//! @return Returns @c RF_SUCCESS for succesfull writting and error otherwise. Possible errors are:
//! + @c RE_FILE_WRITE: There was an unknown write error
//! + @c RE_FILE_WRITE_BLOCK: The write failed because the file was occupied by another thread and the no block flag was set
//! + @c RE_FILE_BAD: The file descriptor @c f was corrupt
//! + @c RE_FILE_TOOBIG: The file's size exceeds the system limiti
//! + @c RE_INTERRUPT: Writting failed due to a system interrupt
//! + @c RE_FILE_IO: Writting failed because of a physical I/O error
//! + @c RE_FILE_NOSPACE: Writting failed because the device containing the file had no free space
//! + @c RE_FILE_NOT_FILE: Writting failed because the given file descriptor @c f is either non existen or not a file
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfString_Fwrite(void* s,FILE* f,char encoding);
#else
i_DECLIMEX_ int32_t i_rfString_Fwrite(void* s,FILE* f,char encoding);
#define rfString_Fwrite(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRING_FWRITE,3,__VA_ARGS__)
#define i_NPSELECT_RF_STRING_FWRITE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Fwrite() accepts from 2 to 3 arguments\"")
#define i_NPSELECT_RF_STRING_FWRITE0(...) RF_SELECT_FUNC(i_SELECT_RF_STRING_FWRITE,__VA_ARGS__)
#define i_SELECT_RF_STRING_FWRITE3(i_STR_,i_FILE_,i_ENCODING_) i_rfString_Fwrite(i_STR_,i_FILE_,i_ENCODING_)
#define i_SELECT_RF_STRING_FWRITE2(i_STR_,i_FILE_) i_rfString_Fwrite(i_STR_,i_FILE_,RF_UTF8))
#define i_SELECT_RF_STRING_FWRITE1(i_STR_,i_FILE_) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Fwrite() accepts from 2 to 3 arguments\"")
#define i_SELECT_RF_STRING_FWRITE0(i_STR_,i_FILE_) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfString_Fwrite() accepts from 2 to 3 arguments\"")
#endif

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
