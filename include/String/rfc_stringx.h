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



#ifndef REFU_USTRINGX_H
#define REFU_USTRINGX_H

#include <rf_options.h> //read in the options

#ifdef RF_MODULE_STRINGS//string module check

//include the simple string
#include "rfc_string.h"


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @internal
** @date 16/05/2010
** @author Lefteris
** @endinternal
**
** @brief An extended version of the normal String which offers a movable index and dynamic buffer allocation
**
** The Extended String is an extension over the simple @ref RF_String that adds functionalities that enable moving
** inside the String itself and allocating its size dynamically. This is accomplished by adding two more members to
** the structure:
**
**  <ul><li>A buffer index allowing operations of moving inside the String to happen</li>
**  <li>A buffer size which keeps the size of the buffer allocated for the string</li></ul>
**
** Whenever the size of this String gets over the buffer size, it gets increased. Some functions even decrease the size
** allocated for the String buffer.
**
** This String version has functions that extend it greatly especially for the purposes
** of parsing files/streams and editable text. This String is not intended for being
** used en mass, saved in arrays/vectors e.t.c. For that use the original @ref RF_String
**
** As far as function calling is concerned if an extended version of the function exists (starting with rfStringX)
** then you should use that so that the string's members get updated correctly. If not then just using the normal
** function with an extended string as a parameter will suffice.
** All the normal String functions which have <em class="rf_ability1">Can be used with StringX</em> on their description can be used with extended strings safely, since no specific
** version of the function exists, or needs to exist to manipulate Extended Strings.
**
** @see RF_String
** @internal
** @todo Generalize most of the constructors below and let the wrapper generate them
** @cppcode
** //default constructor
** StringX(){this->i_StringCHandle = rfStringX_Create("");}
** @endcpp
** @endinternal
** @inherit RF_String
**/
typedef struct RF_StringX
{
    //! The RF_String inherited members
    RF_String INH_String;
    //! The buffer index, denotes how far from the start of the buffer the start of the string has moved
    uint32_t bIndex;
    //! The size of the buffer allocated for this extended String in bytes.
    uint32_t bSize;
}RF_StringX;


/*-------------------------------------------------------------------------Methods to create an RF_StringX-------------------------------------------------------------------------------*/
//! @name RF_StringX Creation
//! @{


//! @memberof RF_StringX
//! @brief Allocates and returns an extended String
//!
//! Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
//! @param lit The string literal with which to initialize.
//! Can also follow a printf-like format which will be formatted with
//! the variables that follow it. A check to see if it is a valid UTF-8 sequence is performed
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c lit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return The newly initialized string or null pointer in case of failure
//! @see rfStringX_Init()
//! @see rfStringX_Create_cp()
//! @see rfStringX_Create_i()
//! @see rfStringX_Create_f()
//! @see rfStringX_Create_buff()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ RF_StringX* rfStringX_Create(const char* lit,...);
#else
i_DECLIMEX_ RF_StringX* i_rfStringX_Create(const char* lit,...);
i_DECLIMEX_ RF_StringX* i_NVrfStringX_Create(const char* lit);
#define rfStringX_Create(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_CREATE,1,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE1(...) i_NVrfStringX_Create(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE0(...) i_rfStringX_Create(__VA_ARGS__)
#endif


//! @memberof RF_StringX
//! @brief Initializes an extended String
//!
//! Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
//! @param str The String to initialize
//! @param lit The string literal with which to initialize.
//! Can also follow a printf-like format which will be formatted with
//! the variables that follow it. A check to see if it is a valid UTF-8 sequence is performed
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c lit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return The newly initialized string or null pointer in case of failure
//! @return True for succesfull initialization and false otherwise
//! @see rfStringX_Create()
//! @see rfStringX_Init_cp()
//! @see rfStringX_Init_i()
//! @see rfStringX_Init_f()
//! @see rfStringX_Init_buff()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_Init(RF_StringX* str,const char* lit,...);
#else
i_DECLIMEX_ char i_rfStringX_Init(RF_StringX* str,const char* lit,...);
i_DECLIMEX_ char i_NVrfStringX_Init(RF_StringX* str,const char* lit);
#define rfStringX_Init(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_INIT,2,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT1(...) i_NVrfStringX_Init(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT0(...) i_rfStringX_Init(__VA_ARGS__)
#endif

//! @memberof RF_StringX
//! @brief Turns a unicode code point to an extended String and allocates and returns it
//!
//! @param code The unicode code point to encode, must be an uint32_t
//! @return An extended String with the code point encoded in it or a null pointer in case of an illegal code point value
//! @see rfStringX_Init_cp()
//! @see rfStringX_Create()
//! @see rfStringX_Create_i()
//! @see rfStringX_Create_f()
//! @see rfStringX_Create_buff()
i_DECLIMEX_ RF_StringX* rfStringX_Create_cp(uint32_t code);
//! @memberof RF_StringX
//! @cppnotctor
//! @brief Turns a unicode code point to an extended String at initialization
//!
//! @param str The String to initialize
//! @param code The unicode code point to encode, must be an uint32_t
//! @return true for success or false in case of an illegal code point value
//! @see rfStringX_Create_cp()
//! @see rfStringX_Init()
//! @see rfStringX_Init_i()
//! @see rfStringX_Init_f()
//! @see rfStringX_Init_buff()
i_DECLIMEX_ char rfStringX_Init_cp(RF_StringX* str,uint32_t code);


//! @memberof RF_StringX
//! @cppnotctor
//! @brief Allocates and returns an extended String without any checks.
//!
//! @warning No valid UTF-8 check is performed
//! @param lit The string literal with which to initialize.
//! Can also follow a printf-like format which will be formatted with
//! the variables that follow it. No valid UTF-8 check performed here.
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c lit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return The newly initialized string or null pointer in case of failure
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ RF_StringX* rfStringX_Create_nc(const char* lit,...);
#else
i_DECLIMEX_ RF_StringX* i_rfStringX_Create_nc(const char* lit,...);
i_DECLIMEX_ RF_StringX* i_NVrfStringX_Create_nc(const char* lit);
#define rfStringX_Create_nc(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_CREATE_NC,1,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE_NC1(...) i_NVrfStringX_Create_nc(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE_NC0(...) i_rfStringX_Create_nc(__VA_ARGS__)
#endif


//! @memberof RF_StringX
//! @brief Initializes an extended String without any checks
//!
//! @warning No valid UTF-8 check is performed
//! @param str The String to initialize
//! @param lit The string literal with which to initialize.
//! Can also follow a printf-like format which will be formatted with
//! the variables that follow it. No valid UTF-8 check performed here.
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c lit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return True fo succesfull initialization and false otherwise
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_Init_nc(RF_StringX* str,const char* lit,...);
#else
i_DECLIMEX_ char i_rfStringX_Init_nc(RF_StringX* str,const char* lit,...);
i_DECLIMEX_ char i_NVrfStringX_Init_nc(RF_StringX* str,const char* lit);
#define rfStringX_Init_nc(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_INIT_NC,2,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT_NC1(...) i_NVrfStringX_Init_nc(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT_NC0(...) i_rfStringX_Init_nc(__VA_ARGS__)
#endif

//! @memberof RF_StringX
//! @brief Creates an RF_StringX from an integer
//!
//! Allocates and returns an extended string with the given integer.
//! @param i The integer to turn into a string
//! @return Returns the initialized RF_stringX
//! @see rfStringX_Init_i()
//! @see rfStringX_Create()
//! @see rfStringX_Create_cp()
//! @see rfStringX_Create_f()
//! @see rfStringX_Create_buff()
i_DECLIMEX_ RF_StringX* rfStringX_Create_i(int32_t i);
//! @memberof RF_StringX
//! @brief Initializes an RF_StringX from an integer
//!
//! Initializes an extended string with the given integer.
//! @param str the String to initialize
//! @param i The integer to turn into a string
//! @return Returns true for success and false otherwise
//! @see rfStringX_Create_i()
//! @see rfStringX_Init()
//! @see rfStringX_Init_cp()
//! @see rfStringX_Init_f()
//! @see rfStringX_Init_buff()
i_DECLIMEX_ char rfStringX_Init_i(RF_StringX* str,int32_t i);
//! @memberof RF_StringX
//! @brief Creates an RF_StringX from a float
//!
//! Allocates and returns an extended string with the given float.
//! @param f The float to turn into a string
//! @return Returns the initialized RF_stringX
//! @see rfStringX_Init_f()
//! @see rfStringX_Create()
//! @see rfStringX_Create_cp()
//! @see rfStringX_Create_i()
//! @see rfStringX_Create_buff()
i_DECLIMEX_ RF_StringX* rfStringX_Create_f(float f);
//! @memberof RF_StringX
//! @brief Initializes an RF_StringX from a float
//!
//! Initializes an extended string with the given float.
//! @param str the String to initialize
//! @param f The float to turn into a string
//! @return Returns true for success and false otherwise
//! @see rfStringX_Create_f()
//! @see rfStringX_Init()
//! @see rfStringX_Init_cp()
//! @see rfStringX_Init_i()
//! @see rfStringX_Init_buff()
i_DECLIMEX_ char rfStringX_Init_f(RF_StringX* str,float f);


//! @memberof RF_StringX
//! @brief Creates an RF_StingX with a specified buffer size
//!
//! Allocates and returns an extended String allocating buffSize bytes buffer and puts str inside the buffer
//! If the contents of the string are bigger than the buffer Size then an error is logged but the String is initialized correctly
//! Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
//! @param buffSize A positive value of bytes for the size of the buffer to allocate
//! @param lit The string literal with which to initialize.A check to see if it is a valid UTF-8 sequence is performed
//! Can also follow a printf-like format which will be formatted with
//! the variables that follow it.
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c lit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return The newly initialized string
//! @see rfStringX_Init_buff()
//! @see rfStringX_Create()
//! @see rfStringX_Create_cp()
//! @see rfStringX_Create_i()
//! @see rfStringX_Create_f()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ RF_StringX* rfStringX_Create_buff(uint32_t buffSize,const char* lit,...);
#else
i_DECLIMEX_ RF_StringX* i_rfStringX_Create_buff(uint32_t buffSize,const char* lit,...);
i_DECLIMEX_ RF_StringX* i_NVrfStringX_Create_buff(uint32_t buffSize,const char* lit);
#define rfStringX_Create_buff(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_CREATE_BUFF,2,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE_BUFF1(...) i_NVrfStringX_Create_buff(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE_BUFF0(...) i_rfStringX_Create_buff(__VA_ARGS__)
#endif


//! @memberof RF_StringX
//! @brief Initializes an RF_StingX with a specified buffer size
//!
//! Initializes an extended String allocating buffSize bytes buffer and puts str inside the buffer
//! If the contents of the string are bigger than the buffer Size then an error is logged but the String is initialized correctly
//! Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
//! @param str the String to initialize
//! @param buffSize A positive value of bytes for the size of the buffer to allocate
//! @param lit The string literal with which to initialize.A check to see if it is a valid UTF-8 sequence is performed
//! Can also follow a printf-like format which will be formatted with
//! the variables that follow it.
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c lit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return true for success and false for failure
//! @see rfStringX_Create_buff()
//! @see rfStringX_Init()
//! @see rfStringX_Init_cp()
//! @see rfStringX_Init_i()
//! @see rfStringX_Init_f()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit,...);
#else
i_DECLIMEX_ char i_rfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit,...);
i_DECLIMEX_ char i_NVrfStringX_Init_buff(RF_StringX* str,uint32_t buffSize,const char* lit);
#define rfStringX_Init_buff(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_INIT_BUFF,3,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT_BUFF1(...) i_NVrfStringX_Init_buff(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT_BUFF0(...) i_rfStringX_Init_buff(__VA_ARGS__)
#endif
//! @memberof RF_StringX
//! @brief Initializes an RF_StingX buffer for textfile reading
//!
//! This is simply a wrapper macro for @ref rfStringX_Init_buff that initializes an @ref RF_StringX
//! as a file reading buffer using the compile time option @c RF_OPTION_FGETS_READBYTESN. It is intended
//! to be used with @ref RF_TextFile reading as can be seen in this code snippet:
//! @snippet IO/textfile1.c READ_LINES
//!
//! @warning This macro is not available if the library has not been compiled with default arguments on
//! since then variadic macros do not exist. Replace it with the full call of
//! @code rfStringX_Init_buff(&str,RF_OPTION_FGETS_READBYTESN+1,"") @endcode
//! in that case
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
#define rfStringX_Init_txtbuff(i_STRINGX_,...) rfStringX_Init_buff((i_STRINGX_),RF_OPTION_FGETS_READBYTESN+1,__VA_ARGS__)
#endif


//! @memberof RF_StringX
//! @brief Allocates and returns an RF_StringX with the given UTF-16 byte sequence
//!
//! Given characters have to be in UTF-16 and in the endianess of the system. They also have to be null terminated.
//! @param s A buffer of 2-byte words representing the utf-16 byte sequence. Needs to be null terminated
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @return Returns the initialized RF_stringX or null in case of failure to initialize, due to invalid utf-16 sequence or illegal endianess value
//! @see rfStringX_Init_UTF16()
//! @see rfStringX_Create_UTF32()
i_DECLIMEX_ RF_StringX* rfStringX_Create_UTF16(const uint16_t* s);
//! @memberof RF_StringX
//! @brief Initializes an RF_StringX with the given UTF-16 byte sequence
//!
//! Given characters have to be in UTF-16 and in the endianess of the system. They also have to be null terminated.
//! @param str the String to initialize
//! @param s A buffer of 2-byte words representing the utf-16 byte sequence. Needs to be null terminated
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @return Returns true for success or false in case of failure to initialize, due to invalid utf-16 sequence or illegal endianess value
//! @see rfStringX_Create_UTF16()
//! @see rfStringX_Init_UTF32()
i_DECLIMEX_ char rfStringX_Init_UTF16(RF_StringX* str,const uint16_t* s);

//! @memberof RF_StringX
//! @cppnotctor
//! @brief Allocates and returns an RF_StringX with the given UTF-32 byte sequence
//!
//! Given characters have to be in UTF-32 and in the endianess of the system.
//! No endianess swapping occurs in the function
//! @param s A buffer of 4-byte words representing the utf-32 byte sequence. Needs to be null terminated.
//! @return Returns the initialized RF_stringX or null in case of failure to initialize
//! @see rfStringX_Init_UTF32()
//! @see rfStringX_Create_UTF16()
i_DECLIMEX_ RF_StringX* rfStringX_Create_UTF32(const uint32_t* s);
//! @memberof RF_StringX
//! Initializes an RF_StringX with the given UTF-32 byte sequence
//!
//! Given characters have to be in UTF-32 and in the endianess of the system.
//! No endianess swapping occurs in the function
//! @param str the String to initialize
//! @param s A buffer of 4-byte words representing the utf-32 byte sequence. Needs to be null terminated.
//! @return Returns true for succes or false in case of failure to initialize
//! @see rfStringX_Create_UTF32()
//! @see rfStringX_Init_UTF16()
i_DECLIMEX_ char rfStringX_Init_UTF32(RF_StringX* str,const uint32_t* s);


//! @}
/*-------------------------------------------------------------------------Methods to copy/assign an RF_StringX-------------------------------------------------------------------------------*/
//! @name RF_StringX Copying-Assigning
//! @{

//! @memberof RF_StringX
//! @brief Assigns the value of the source string to the destination extended string
//!
//! Both strings should already be initialized and hold a value. It is an error to give null parameters.
//! @lmsFunction
//! @param dest The destination string, which should get assigned
//! @param source The source string, whose values to copy. @inhtype{String,StringX} @tmpSTR
//! @see rfStringX_Assign_char()
i_DECLIMEX_ void rfStringX_Assign(RF_StringX* dest,const void* source);


//! @memberof RF_StringX
//! @brief Assigns the value of a unicode character to the string
//!
//! @param thisstr The string to assign to
//! @param character The unicode character codepoint to assign to the String
//! @return Returns @c true for succesfull assignment and @c false if the given @c character was not a valid unicode codepoint
//! @see rfStringX_Assign()
i_DECLIMEX_ char rfStringX_Assign_char(RF_StringX* thisstr,uint32_t character);

//! @memberof RF_StringX
//! @cppignore
//! @brief Creates an RF_StringX from an RF_string
//!
//! Returns an RF_StringX version of the parameter RF_String
//! @param s The RF_String from which to create the RF_StringX
//! @return The RF_StringX version of the parameter string
//! @see rfStringX_FromString_IN()
//! @see rfStringX_Copy_OUT()
//! @see rfStringX_Copy_chars()
i_DECLIMEX_ RF_StringX* rfStringX_FromString_OUT(const RF_String* s);
//! @memberof RF_StringX
//! @cppignore
//! @brief Initializes an RF_StringX from an RF_string
//!
//! @param dst The RF_StringX that will get initialize
//! @param src The RF_String to copy from
//! @return The RF_StringX version of the parameter string
//! @see rfStringX_FromString_OUT()
//! @see rfStringX_Copy_IN()
//! @see rfStringX_Copy_chars()
i_DECLIMEX_ void rfStringX_FromString_IN(RF_StringX* dst,const RF_String* src);


//! @memberof RF_StringX
//! @cppignore
//! @brief Creates a copy of an extended String and returns it
//!
//! @note The Returned Substring needs to be freed by the user.
//! BEWARE when assigning to a string using this function since if any previous string exists there IS NOT getting freed. You have to free it explicitly.
//!
//! Also note that the source RF_StringX is not @c const unline the RF_String copying functions.
//! The reason for this is because we actually have to copy all of it, even if its internal pointer has been moved, in which case
//! its contents are indeed tampered with in the function so as to allow reading the whole string. But despite that after the function finishes the contents
//! of the source string are guaranteed to be the same.
//! @param  s The  string to copy
//! @return A copy of the string
//! @see rfStringX_FromString_OUT()
//! @see rfStringX_Copy_IN()
//! @see rfStringX_Copy_chars()
i_DECLIMEX_ RF_StringX* rfStringX_Copy_OUT(RF_StringX* s);
//! @memberof RF_StringX
//! @cppignore
//! @brief Copies the given source RF_StringX into the destination RF_StringX
//!
//! @note The Returned Substring needs to be freed by the user.
//! BEWARE when assigning to a string using this function since if any previous string exists there IS NOT getting freed. You have to free it explicitly
//!
//! Also note that the source RF_StringX is not @c const unline the RF_String copying functions.
//! The reason for this is because we actually have to copy all of it, even if its internal pointer has been moved, in which case
//! its contents are indeed tampered with in the function so as to allow reading the whole string. But despite that after the function finishes the contents
//! of the source string are guaranteed to be the same.
//! @param  dst The String to copy in.
//! @param  src The String to copy from.
//! @see rfStringX_FromString_IN()
//! @see rfStringX_Copy_OUT()
//! @see rfStringX_Copy_chars()
i_DECLIMEX_ void rfStringX_Copy_IN(RF_StringX* dst,RF_StringX* src);
//! @memberof RF_StringX
//! @brief Copies a certain number of characters from an RF_StringX
//!
//! Copies @c n characters from @c src StringX into the destination @c dst StringX.
//!
//! Also note that the source RF_StringX is not @c const unline the RF_String copying functions.
//! The reason for this is because we actually have to copy all of it, even if its internal pointer has been moved, in which case
//! its contents are indeed tampered with in the function so as to allow reading the whole string. But despite that after the function finishes the contents
//! of the source string are guaranteed to be the same.
//! @param  dst The String to copy in.
//! @param  src The String to copy from
//! @param n  The number of characters to copy from the @c src string.
//! If the value is bigger than the maximum number of characters then still all characters are copied.
//! @see rfStringX_Copy_OUT()
//! @see rfStringX_Copy_IN()
i_DECLIMEX_ void rfStringX_Copy_chars(RF_StringX* dst,RF_StringX* src,uint32_t n);


//! @}
/*-------------------------------------------------------------------------Methods to get rid of an RF_StringX-------------------------------------------------------------------------------*/
//! @name Getting rid of an RF_StringX
//! @{

//! @memberof RF_StringX
//! @brief Destroys an extended String and releases its memory.
//!
//! It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
//! Use it for strings made with _Create
//! @param s The exended String to destroy
//! @see rfStringX_Deinit()
i_DECLIMEX_ void rfStringX_Destroy(RF_StringX* s);
//! @memberof RF_StringX
//! @brief Destroys only the contents of the Extended string without freeing the pointer itself.
//!
//! It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
//! Use it for strings made with _Init
//! @param s The exended String to destroy
//! @see rfStringX_Destroy()
i_DECLIMEX_ void rfStringX_Deinit(RF_StringX* s);

//! @}

/*-------------------------------------------------------------------------Functions to  manipulate an RF_StringX-------------------------------------------------------------------------------*/
//! @name RF_StringX Manipulation
//! @{

//! @memberof RF_StringX
//! @brief Appends the parameter String to this extended string.
//!
//! @lmsFunction
//! @param thisstr The extended string to append to
//! @param other The string to add to this string. @inhtype{String,StringX} @tmpSTR
//! @see rfStringX_Append_i()
//! @see rfStringX_Append_f()
//! @see rfStringsX_Prepend()
i_DECLIMEX_ void rfStringX_Append(RF_StringX* thisstr,const void* other);


//! @memberof RF_StringX
//! @brief Appends an integer to the extended string
//!
//! @param thisstr The extended string to append to
//! @param i The integer to add
//! @see rfStringX_Append()
//! @see rfStringX_Append_f()
//! @see rfStringX_Prepend()
i_DECLIMEX_ void rfStringX_Append_i(RF_StringX* thisstr,const int32_t i);
//! @memberof RF_StringX
//! @brief Appends a float to the extended string
//!
//! @param thisstr The extended string to append to
//! @param f The float to add
//! @see rfStringX_Append()
//! @see rfStringX_Append_i()
//! @see rfStringX_Prepend()
i_DECLIMEX_ void rfStringX_Append_f(RF_StringX* thisstr,const float f);

//! @memberof RF_StringX
//! @brief Prepends the parameter String to this extended string
//!
//! @lmsFunction
//! @param thisstr The extended string to prepend to
//! @param other The string to prepend to this string. @inhtype{String,StringX} @tmpSTR
//! @see rfStringX_Append()
i_DECLIMEX_ void rfStringX_Prepend(RF_StringX* thisstr,const void* other);



//! @memberof RF_StringX
//! @brief Inserts a string to this extended string at the parameter character position
//!
//! This function takes the given string @c thisstr and finds its @c pos character.
//! Then it inserts the @c other string in the position of that character pushing all
//! the rest of the string further ahead.
//! @lmsFunction
//! @param thisstr The extended string to insert to
//! @param pos     The character position in the string to add it. Should be a positive (or zero) integer. If the position is over the string's size nothing happens.
//! @param other   The string to add. @inhtype{String,StringX} @tmpSTR
//! @see rfStringX_Append()
//! @see rfStringX_Prepend()
i_DECLIMEX_ void rfStringX_Insert(RF_StringX* thisstr,uint32_t pos,const void* other);


//! @memberof RF_StringX
//! @brief Replaces all occurences of a String
//!
//! Replaces all of the specified @c sstr substring from the extended String with the string @c rstr
//! @lmsFunction
//! @param thisstr The extended string in which to do the replacing
//! @param sstr The string to locate and replace from the current string. @inhtype{String,StringX} @tmpSTR
//! @param rstr The string with which to replace it. @inhtype{String,StringX} @tmpSTR
//! @param number \rfoptional{0}. The number of occurences to replace. Give @e 0 for all the occurences.
//! If the given number is bigger than the actual number of occurences, still all occurences get replaced.
//! @param options \rfoptional{0}. Bitflag options denoting some options for the string to replace. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! @return Returns true in case of success, and false if the substring was not even found inside the string
//! @see rfStringX_ReplaceBetween()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_Replace(RF_StringX* thisstr,const void* sstr,const void* rstr,uint32_t number,char options);
#else
i_DECLIMEX_ char i_rfStringX_Replace(RF_StringX* thisstr,const void* sstr,const void* rstr,uint32_t number,const char options);
#define rfStringX_Replace(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_REPLACE,5,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_REPLACE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#define i_NPSELECT_RF_STRINGX_REPLACE0(...) RF_SELECT_FUNC(i_SELECT_RF_STRINGX_REPLACE,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_REPLACE5(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_) \
   i_rfStringX_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,i_OPTIONS_)
#define i_SELECT_RF_STRINGX_REPLACE4(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_) \
    i_rfStringX_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,i_NUMBER_,0)
#define i_SELECT_RF_STRINGX_REPLACE3(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_) i_rfStringX_Replace(i_THISSTR_,i_SEARCHSTR_,i_REPSTR_,0,0)
#define i_SELECT_RF_STRINGX_REPLACE2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACE1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACE0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_Replace() accepts from 3 to 5 arguments\"")
#endif

//! @memberof RF_StringX
//! @brief Replaces Strings between substrings for a given number of occurences
//!
//! Replaces what exists between the ith left and right substrings of this extended String. Utilizes the internal string pointer.
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param left The left substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
//! @param rstr The string to act as replacement. @inhtype{String,StringX} @tmpSTR
//! @param options \rfoptional{0}. Bitflag options denoting some options for the string to replace. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! @param i \rfoptional{0}. The specific between occurence to replace. Should range between 1 and infinity. If 0 all occurences will be replaced
//! @return Returns true if the replacing happened and false if either the left or the right strings were not found
//! @see rfStringX_Replace()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_ReplaceBetween(RF_StringX* thisstr,const void* left,const void* right,const void* rstr,char options,uint32_t i);
#else
i_DECLIMEX_ char i_rfStringX_ReplaceBetween(RF_StringX* thisstr,const void* left,const void* right,const void* rstr,char options,uint32_t i);
#define rfStringX_ReplaceBetween(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_REPLACEBETWEEN,6,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_REPLACEBETWEEN1(...)  RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_NPSELECT_RF_STRINGX_REPLACEBETWEEN0(...)  RF_SELECT_FUNC(i_SELECT_RF_STRINGX_REPLACEBETWEEN,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN6(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_,i_OCCURENCE_)  \
    i_rfStringX_ReplaceBetween(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_,i_OCCURENCE_)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN5(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_)  \
    i_rfStringX_ReplaceBetween(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,i_OPTIONS_,0)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN4(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_)  \
    i_rfStringX_ReplaceBetween(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_REPSTR_,0,0)
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN3(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_REPLACEBETWEEN0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_ReplaceBetween() accepts from 4 to 6 arguments\"")
#endif

//! @}
/*-------------------------------------------------------------------------Functions to traverse an RF_StringX-------------------------------------------------------------------------------*/
//! @name Traversing an RF_StringX
//! @{

//! @memberof RF_StringX
//! @brief Moves the internal pointer right after the the first occurence of substring
//!
//! Searches the StringX for the @c sub substring and moves the internal pointer right after it
//! Optionally the string between the beginning and the end of the moving is returned in @c result parameter
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param sub The substring after which to move inside the current String. @inhtype{String,StringX} @tmpSTR
//! @param result \rfoptional{0} Pass a pointer to a @c String type to be initialized with the substring between the start of @c thisstr and the end of the moving.
//! If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument
//! If 0 nothing is returned @inhtype{String,StringX}
//! @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substrings inside the string. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
//! @return Returns the number of positions (bytes) moved or RF_FAILURE if the substring was not found in the String
//! @see rfStringX_MoveAfterv()
//! @see rfStringX_MoveAfterPair()
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_Reset()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ int32_t rfStringX_MoveAfter(RF_StringX* thisstr,const void* sub,void* result,const char options);
#else
i_DECLIMEX_ int32_t i_rfStringX_MoveAfter(RF_StringX* thisstr,const void* sub,void* result,const char options);
#define rfStringX_MoveAfter(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_MOVEAFTER,4,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_MOVEAFTER1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfter() accepts from 2 to 4 arguments\"")
#define i_NPSELECT_RF_STRINGX_MOVEAFTER0(...) RF_SELECT_FUNC(i_SELECT_RF_STRINGX_MOVEAFTER,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_MOVEAFTER4(i_THISSTR_,i_SUBSTR_,i_RESULT_,i_OPTIONS_) \
     i_rfStringX_MoveAfter(i_THISSTR_,i_SUBSTR_,i_RESULT_,i_OPTIONS_)
#define i_SELECT_RF_STRINGX_MOVEAFTER3(i_THISSTR_,i_SUBSTR_,i_RESULT_) \
     i_rfStringX_MoveAfter(i_THISSTR_,i_SUBSTR_,i_RESULT_,0)
#define i_SELECT_RF_STRINGX_MOVEAFTER2(i_THISSTR_,i_SUBSTR_) \
     i_rfStringX_MoveAfter(i_THISSTR_,i_SUBSTR_,0,0)
#define i_SELECT_RF_STRINGX_MOVEAFTER1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfter() accepts from 2 to 4 arguments\"")
#define i_SELECT_RF_STRINGX_MOVEAFTER0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfter() accepts from 2 to 4 arguments\"")
#endif

//! @memberof RF_StringX
//! @brief Moves the internal pointer n characters back
//!
//! It never goes before the original start of the buffer so there is no worry of memory corruption.
//! @param thisstr The extended string to work on
//! @param n The number of characters to move the internal pointer
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveAfter()
//! @see rfStringX_Reset()
i_DECLIMEX_ void rfStringX_MoveBack(RF_StringX* thisstr,uint32_t n);
//! @memberof RF_StringX
//! @brief Moves the internal pointer n characters forward
//!
//! It never goes beyond the buffer's length so there is no fear of memory corruption.
//! But since many times the buffer can be bigger than the string we can have it go to non-string buffer data so the user has to be careful.
//! @param thisstr The extended string to work on
//! @param n The number of character to move the internal pointer
//! @see rfStringX_MoveBack()
//! @see rfStringX_MoveAfter()
//! @see rfStringX_Reset()
i_DECLIMEX_ void rfStringX_MoveForward(RF_StringX* thisstr,uint32_t n);
//! @memberof RF_StringX
//! @brief Resets the internal pointer of the StringX
//!
//! @param thisstr The stringX whose internal pointer to reset
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_MoveAfter()
i_DECLIMEX_ void rfStringX_Reset(RF_StringX* thisstr);

//! @memberof RF_StringX
//! @brief Moves the internal pointer after the first occurence of any of the given substrings
//!
//! Searches for any of the given Substrings inside @c thisstr. Moves the internal pointer right after
//! the first occuring substring. Optionally and if the value given at @c result is not 0 the string between
//! the start and the end of the moving is returned. The parameters accepted can be of any string type, even temporary
//! strings initialized with the temporary string macro
//! @rfNoDefArgsWarn1
//! @warning if the library has been compiled with @c DEFAULT_ARGUMENTS off then arguments @c options and @c parN are actually pointers
//! to @c char and @c unsigned char respectively
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param result \rfoptional{0} Pass a pointer to a string type to have it initialized with the before part of the string before the found substring.
//! If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument
//! This is an optional parameter. Give 0 for nothing returned. @inhtype{String,StringX}
//! @param options Bitflag options denoting the method with which to search for the substrings inside the string. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to replace any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to replace only exact matches of the substring. For example an exact replace for @e "HELLO" in the string
//!     @e "HELLOWORLD" would replace nothing. Default is with this flag off.
//! + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
//! @param parN The number of parameters
//! @extraVarArgLim
//! @param ... The strings to search for. @inhtype{String,StringX} @tmpSTR
//! @return true if the substring got initialized and false if none of the parameters are found
//! @see rfStringX_MoveAfter()
//! @see rfStringX_MoveAfterPair()
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_Reset()
i_DECLIMEX_ char rfStringX_MoveAfterv(RF_StringX* thisstr,void* result,const char options,const unsigned char parN, ...);

//! @memberof RF_StringX
//! @brief Moves the internal string pointer after the substring formed by the @c left and @c right substrings
//!
//! Searches @c thisstr for the occurence of both the @c left and @c right substrings and if found moves the internal pointer right after them.
//! If the number given at @c occurence does not exist in the string the function returns @c false. Finally if @c result is not zero then what is between
//! the @c left and @c right substrings is returned inside @c result.
//! @lmsFunction
//! @param thisstr The extended string to work on
//! @param left The left substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
//! @param right The right substring that will define the new substring. @inhtype{String,StringX} @tmpSTR
//! @param result \rfoptional{0} If this is not 0 then pass a pointer to a string type here to have it initialized with what's in between the @c left and @c right substring
//! after which the internal string pointer gets moved.
//! If the passed pointer is of RF_StringX type also pass the @c RF_STRINGX_ARGUMENT bitflag argument in the @c options argument
//! @param options \rfoptional{0} Bitflag options denoting the method with which to search for the substring literals inside the string. Give 0 for the defaults. Can have values:
//! + @c RF_CASE_IGNORE: If you want to search for any occurence of the substring disregarding CAPS or not.
//!     Default search option is to @b match the case. For now this works only for characters of the english language.
//! + @c RF_MATCH_WORD: If you to find only exact matches of the substring. For example an exact search for @e "HELLO" in the string
//!     @e "HELLOWORLD" would find nothing. Default is with this flag off.
//! + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the pointer you gave for initialization at @c result is of RF_StringX type
//! @param occurence \rfoptional{0} If this is not 0 then the function will search for the number of occurence given in this parameter.
//! If it is 0 it will search for the first occurence. If it is not found then the function shall return false
//! @return Returns true if the substring is found and false if not
//! @see rfStringX_MoveAfter()
//! @see rfStringX_MoveAfterv()
//! @see rfStringX_MoveForward()
//! @see rfStringX_MoveBack()
//! @see rfStringX_Reset()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfStringX_MoveAfterPair(RF_StringX* thisstr,const void* left,const void* right,void* result,char options,uint32_t occurence);
#else
i_DECLIMEX_ char i_rfStringX_MoveAfterPair(RF_StringX* thisstr,const void* left,const void* right,void* result,char options,uint32_t occurence);
#define rfStringX_MoveAfterPair(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_STRINGX_MOVEAFTERPAIR,6,__VA_ARGS__)
#define i_NPSELECT_RF_STRINGX_MOVEAFTERPAIR1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#define i_NPSELECT_RF_STRINGX_MOVEAFTERPAIR0(...) RF_SELECT_FUNC(i_SELECT_RF_STRINGX_MOVEAFTERPAIR,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR6(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_,i_OCCURENCE_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_,i_OCCURENCE_)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR5(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,i_OPTIONS_,0)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR4(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,i_RESULT_,0,0)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR3(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_) \
    i_rfStringX_MoveAfterPair(i_THISSTR_,i_LEFTSTR_,i_RIGHTSTR_,0,0,0)
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#define i_SELECT_RF_STRINGX_MOVEAFTERPAIR0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfStringX_MoveAfterPair() accepts from 3 to 6 arguments\"")
#endif

//! @}
/*------------------------------------------------------------------------- File I/O RF_StringX Functions -------------------------------------------------------------------------------*/
//! @name RF_StringX File I/O
//! @{

//! @memberof RF_StringX
//! @brief Allocates and returns an extended string from UTF-8 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valide sequence of bytes is performed.
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @return The initialized string or null pointer in case of failure to read the file, or unexpected data (non-UTF8 encoded string)
//! @see rfStringX_Init_fUTF8()
//! @see rfStringX_Assign_fUTF8()
//! @see rfStringX_Append_fUTF8()
i_DECLIMEX_ RF_StringX* rfStringX_Create_fUTF8(FILE* f, char* eof);
//! @memberof RF_StringX
//! @brief Initializes an extended string from UTF-8 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valide sequence of bytes is performed.
//! @param str The extended string to initialize
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @return Returns either a positive number for succesfull initialization that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF8() can produce.
//! @see rfStringX_Create_fUTF8()
//! @see rfStringX_Assign_fUTF8()
//! @see rfStringX_Append_fUTF8()
i_DECLIMEX_ int32_t rfStringX_Init_fUTF8(RF_StringX* str,FILE* f, char* eof);

//! @memberof RF_StringX
//! @brief Assigns to an extended string from UTF-8 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and assigns it to an RF_StringX
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valide sequence of bytes is performed.
//! @param str The extended string to assign to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this assignment
//! @return Returns either a positive number for succesfull assignment that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF8() can produce.
//! @see rfStringX_Init_fUTF8()
//! @see rfStringX_Create_fUTF8()
//! @see rfStringX_Append_fUTF8()
i_DECLIMEX_ int32_t rfStringX_Assign_fUTF8(RF_StringX* str,FILE* f, char* eof);
//! @memberof RF_StringX
//! @brief Appends to an extended string from UTF-8 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and appends it to an RF_StringX
//! The file's encoding must be UTF-8.If for some reason (like EOF reached) no string can be read then null is returned
//! Given file character stream must be encoded in UTF-8. A check for valid sequence of bytes is performed.
//! @param str The extended string to append to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-8.A check for valide sequence of bytes is performed.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this appending
//! @return Returns either a positive number for succesfull appending that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF8() can produce.
//! @see rfStringX_Init_fUTF8()
//! @see rfStringX_Create_fUTF8()
//! @see rfStringX_Assign_fUTF8()
i_DECLIMEX_ int32_t rfStringX_Append_fUTF8(RF_StringX* str,FILE* f, char* eof);

//! @memberof RF_StringX
//! @cppnotctor
//! @brief Allocates and returns an extended string from UTF-16 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @return The initialized string or null pointer in case of failure to read the file
//! @see rfStringX_Init_fUTF16()
//! @see rfStringX_Append_fUTF16()
//! @see rfStringX_Assign_fUTF16()
i_DECLIMEX_ RF_StringX* rfStringX_Create_fUTF16(FILE* f, char endianess,char* eof);
//! @memberof RF_StringX
//! @brief Initializes an extended string from UTF-16 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to initialize
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @return Returns either a positive number for succesfull initialization that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF16LE() can produce.
//! @see rfStringX_Create_fUTF16()
//! @see rfStringX_Append_fUTF16()
//! @see rfStringX_Assign_fUTF16()
i_DECLIMEX_ int32_t rfStringX_Init_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof);

//! @memberof RF_StringX
//! @brief Appends the contents of a UTF-16 file to the Extended String
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and appends it to an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to append to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this appending
//! @return Returns either a positive number for succesfull appending that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF16LE() can produce.
//! @see rfStringX_Create_fUTF16()
//! @see rfStringX_Init_fUTF16()
//! @see rfStringX_Assign_fUTF16()
i_DECLIMEX_ int32_t rfStringX_Append_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof);
//! @memberof RF_StringX
//! @brief Assigns the contents of a UTF-16 file to an already initialized StringX
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and assigns it to an RF_StringX
//! The file's encoding must be UTF-16.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to assign to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-16.
//! @param endianess A flag that determines in what endianess the UTF-16 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this assignment
//! @return Returns either a positive number for succesfull assignment that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF16LE() can produce.
//! @see rfStringX_Create_fUTF16()
//! @see rfStringX_Init_fUTF16()
//! @see rfStringX_Append_fUTF16()
i_DECLIMEX_ int32_t rfStringX_Assign_fUTF16(RF_StringX* str,FILE* f, char endianess,char* eof);

//! @memberof RF_StringX
//! @cppnotctor
//! @brief Allocates and returns an extended string from UTF-32 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @return The initialized string or null pointer in case of failure to read the file
//! @see rfStringX_Init_fUTF32()
//! @see rfStringX_Append_fUTF32()
//! @see rfStringX_Assign_fUTF32()
i_DECLIMEX_ RF_StringX* rfStringX_Create_fUTF32(FILE* f,char endianess, char* eof);
//! @memberof RF_StringX
//! @brief Initializes an extended string from UTF-32 file parsing
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and saves it as an RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to initialize
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this initialization
//! @return Returns either a positive number for succesfull initialization that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF32LE() can produce.
//! @see rfStringX_Create_fUTF32()
//! @see rfStringX_Append_fUTF32()
//! @see rfStringX_Assign_fUTF32()
i_DECLIMEX_ int32_t rfStringX_Init_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof);
//! @memberof RF_StringX
//! @brief Assigns the contents of a UTF-32 file to an extended string
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and assigns it as the contents of the given RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to assign to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this assignment
//! @return Returns either a positive number for succesfull assignment that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF32LE() can produce.
//! @see rfStringX_Create_fUTF32()
//! @see rfStringX_Init_fUTF32()
//! @see rfStringX_Append_fUTF32()
i_DECLIMEX_ int32_t rfStringX_Assign_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof);
//! @memberof RF_StringX
//! @brief Appends the contents of a UTF-32 file to an extended string
//!
//! Read the file stream @c f until either a newline character or the EOF is reached and appends to the given RF_StringX
//! The file's encoding must be UTF-32.If for some reason (like EOF reached) no string can be read then null is returned. A check for a valid sequence of bytes is performed.
//! @param str The extended string to append to
//! @param f A valid and open file pointer in read mode from which to read the string. The file's encoding must be UTF-32.
//! @param endianess A flag that determines in what endianess the UTF-32 file is encoded in. Possible values here are
//! @c RF_LITTLE_ENDIAN and @c RF_BIG_ENDIAN.
//! @param[out] eof Pass a pointer to a char to receive a true or false value in case the end of file was reached with this appending
//! @return Returns either a positive number for succesfull appending that represents the bytes read from the file.
//! If there was a problem an error is returned. Possible errors are any of those that @ref rfFReadLine_UTF32LE() can produce.
//! @see rfStringX_Create_fUTF32()
//! @see rfStringX_Init_fUTF32()
//! @see rfStringX_Assign_fUTF32()
i_DECLIMEX_ int32_t rfStringX_Append_fUTF32(RF_StringX* str,FILE* f,char endianess, char* eof);

//! @}
// close the RF_StringX file I/O name group


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#else //end of the strings module include
    #error Attempted to include Refu String manipulation with the String module flag off. Rebuild the library with that option added if you need to include them
#endif


#endif//inclue guards end
