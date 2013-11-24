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
** --String/corex.h
** This header includes core RF_StringX functionality (initialization, deinitialization, copying e.t.c.)
**
**/
#ifndef RF_STRING_COREX_H
#define RF_STRING_COREX_H

// for string decl
    #include <String/stringx_decl.h>
// for exact sized types
    #include <Definitions/types.h>
// for bool
    #include <Definitions/retcodes.h>
// for size_t
    #include <string.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name RF_StringX Creation
//! @{

/* --- Functions that already exist for RF_String - START --- */
/**
 ** @memberof RF_StringX
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ RF_StringX* rfStringX_Create(const char* lit,...);
#else
i_DECLIMEX_ RF_StringX* i_rfStringX_Create(const char* lit,...);
i_DECLIMEX_ RF_StringX* i_NVrfStringX_Create(const char* lit);
#define rfStringX_Create(...)  RP_SELECT_FUNC_IF_NARGIS(  \
        i_SELECT_RF_STRINGX_CREATE,1,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE1(...) i_NVrfStringX_Create(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE0(...) i_rfStringX_Create(__VA_ARGS__)
#endif
/**
 ** @memberof RF_StringX
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ bool rfStringX_Init(RF_StringX* str,const char* lit,...);
#else
i_DECLIMEX_ bool i_rfStringX_Init(RF_StringX* str,const char* lit,...);
i_DECLIMEX_ bool i_NVrfStringX_Init(RF_StringX* str,const char* lit);
#define rfStringX_Init(...)  RP_SELECT_FUNC_IF_NARGIS(  \
        i_SELECT_RF_STRINGX_INIT,2,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT1(...) i_NVrfStringX_Init(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT0(...) i_rfStringX_Init(__VA_ARGS__)
#endif
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Create_cp(uint32_t code);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_cp(RF_StringX* str, uint32_t code);
/**
 ** @memberof RF_StringX
 ** @brief Allocates and returns an extended String without any checks.
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Create_unsafe(const char* lit);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_unsafe(RF_StringX* str, const char* lit);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_unsafe_nnt(RF_StringX* str, const char* s,
                                           size_t length);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_unsafe_bnnt(
    RF_StringX* str, const char* s,
    size_t length, size_t buff_size);

/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Create_i(int32_t i);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_i(RF_StringX* str, int32_t i);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Create_f(float f);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_f(RF_StringX* str, float f);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Create_UTF16(const uint16_t* s,
                                               unsigned int len);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_UTF16(RF_StringX* str,
                                      const uint16_t* s,
                                      unsigned int len);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Create_UTF32(const uint32_t* s,
                                               unsigned int len);
/**
 ** @memberof RF_StringX
 **/
i_DECLIMEX_ bool rfStringX_Init_UTF32(RF_StringX* str, const uint32_t* s,
                                      unsigned int len);


/* --- Functions that already exist for RF_String - END --- */

/**
 ** @memberof RF_StringX
 ** @brief Creates an RF_StingX with a specified buffer size
 **
 ** Allocates and returns an extended String allocating buffSize
 ** bytes buffer and puts @c lit inside that buffer. If the contents
 ** of the string are bigger than the buffer Size then an error is logged
 ** but the String is initialized correctly.
 ** @lmsFunction
 ** @param buffSize A positive value of bytes for the size of the buffer to allocate
 ** @param lit The string literal with which to initialize.A check to see
 ** if it is a valid UTF-8 sequence is performed
 ** Can also follow a printf-like format which will be formatted with
 ** the variables that follow it.
 ** @param ... \rfoptional{nothing}  Depending on the string literal,
 ** the function may expect a sequence of additional arguments,
 ** each containing one value to be inserted instead of each %-tag specified
 * in the @c lit parameter, if any. There should be
 ** the same number of these arguments as the number of %-tags that expect a value.
 ** @return The newly initialized string
 ** @see rfStringX_Init_buff()
 ** @see rfStringX_Create()
 ** @see rfStringX_Create_cp()
 ** @see rfStringX_Create_i()
 ** @see rfStringX_Create_f()
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ RF_StringX* rfStringX_Create_buff(uint32_t buffSize,
                                              const char* lit, ...);
#else
i_DECLIMEX_ RF_StringX* i_rfStringX_Create_buff(uint32_t buffSize,
                                                const char* lit,
                                                ...);
i_DECLIMEX_ RF_StringX* i_NVrfStringX_Create_buff(uint32_t buffSize,
                                                  const char* lit);
#define rfStringX_Create_buff(...)  \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_CREATE_BUFF,2,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE_BUFF1(...) \
    i_NVrfStringX_Create_buff(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_CREATE_BUFF0(...) \
    i_rfStringX_Create_buff(__VA_ARGS__)
#endif


/**
 ** @memberof RF_StringX
 ** @brief Initializes an RF_StingX with a specified buffer size
 ** @lmsFunction
 ** @return true for success and false for failure
 ** @see rfStringX_Create_buff()
 ** @see rfStringX_Init()
 ** @see rfStringX_Init_cp()
 ** @see rfStringX_Init_i()
 ** @see rfStringX_Init_f()
 **/
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ bool rfStringX_Init_buff(RF_StringX* str,
                                     uint32_t buffSize,
                                     const char* lit, ...);
#else
i_DECLIMEX_ bool i_rfStringX_Init_buff(RF_StringX* str,
                                       uint32_t buffSize,
                                       const char* lit, ...);
i_DECLIMEX_ bool i_NVrfStringX_Init_buff(RF_StringX* str,
                                         uint32_t buffSize,
                                         const char* lit);
#define rfStringX_Init_buff(...)  \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRINGX_INIT_BUFF,3,__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT_BUFF1(...) \
    i_NVrfStringX_Init_buff(__VA_ARGS__)
#define i_SELECT_RF_STRINGX_INIT_BUFF0(...) \
    i_rfStringX_Init_buff(__VA_ARGS__)
#endif
/**
 ** @memberof RF_StringX
 ** @brief Initializes an RF_StingX buffer for textfile reading
 **
 ** This is simply a wrapper macro for @ref rfStringX_Init_buff that 
 ** initializes an @ref RF_StringX as a file reading buffer using the
 ** compile time option @c RF_OPTION_FGETS_READ_BYTESN. It is intended
 ** to be used with @ref RF_TextFile reading as can be seen 
 ** in this code snippet:
 ** @snippet IO/textfile1.c READ_LINES
 **
 ** @warning This macro is not available if the library has not been 
 ** compiled with default arguments on since then variadic macros do not
 ** exist. In that case replace it with the full call of
 ** @code rfStringX_Init_buff(&str,RF_OPTION_FGETS_READ_BYTESN+1,"")
 ** @endcode
 **/
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
#define rfStringX_Init_txtbuff(i_STRINGX_, ...) \
    rfStringX_Init_buff((i_STRINGX_),RF_OPTION_FGETS_READ_BYTESN+1,__VA_ARGS__)
#endif


//! @}

//! @name RF_StringX Copying-Assigning
//! @{

/**
 ** @memberof RF_StringX
 ** @brief Assigns the value of the source string to the
 ** destination extended string
 **
 ** Both strings should already be initialized and hold a value.
 ** It is an error to give null parameters.
 ** @lmsFunction
 ** @param dest The destination string, which should get assigned
 ** @param source The source string, whose values to copy.
 ** @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true for success and @c false otherwise
 ** @see rfStringX_Assign_char()
 **/
i_DECLIMEX_ bool rfStringX_Assign(RF_StringX* dest, const void* source);

/**
 ** @memberof RF_StringX
 ** @brief Assigns the value of a unicode character to the string
 **
 ** @param thisstr The string to assign to
 ** @param character The unicode character codepoint to assign to the String
 ** @return Returns @c true for succesfull assignment and @c false
 ** if the given @c character was not a valid unicode codepoint
 ** @see rfStringX_Assign()
 **/
i_DECLIMEX_ bool rfStringX_Assign_char(RF_StringX* thisstr, uint32_t character);

/**
 ** @memberof RF_StringX
 ** @brief Nullifies a string
 ** @warning Use null strings at your own risk. None of the RF_String/X
 ** functions currently test for them
 ** A safer and easier alternative is to assign an empty string and
 * check for it with @ref rfString_IsEmpty()
 **/
#define rfStringX_Null(i_STRING) do{            \
        rfString_ByteLength(i_STRING) = 0;      \
        rfString_Data(i_STRING) = NULL;         \
        (i_STRING)->bIndex = 0;                 \
        (i_STRING)->bSize = 0;                  \
}while(0)

/**
 ** @memberof RF_StringX
 ** @brief Checks that a string is null
 **/
#define rfStringX_IsNull(i_STRING)              \
    ((rfString_ByteLength(i_STRING) == 0) &&    \
     (rfString_Data(i_STRING) == NULL))

/**
 ** @memberof RF_StringX
 ** @cppignore
 ** @brief Creates an RF_StringX from an RF_string
 **
 ** Returns an RF_StringX version of the parameter RF_String
 ** @param s The RF_String from which to create the RF_StringX
 ** @return The RF_StringX version of the parameter string
 ** @see rfStringX_FromString_IN()
 ** @see rfStringX_Copy_OUT()
 ** @see rfStringX_Copy_chars()
 **/
i_DECLIMEX_ RF_StringX* rfStringX_FromString_OUT(const RF_String* s);
/**
 ** @memberof RF_StringX
 ** @cppignore
 ** @brief Initializes an RF_StringX from an RF_string
 **
 ** @param dst The RF_StringX that will get initialize
 ** @param src The RF_String to copy from
 ** @return Returns @c true for success and @c false otherwise
 ** @see rfStringX_FromString_OUT()
 ** @see rfStringX_Copy_IN()
 ** @see rfStringX_Copy_chars()
 **/
i_DECLIMEX_ bool rfStringX_FromString_IN(RF_StringX* dst,
                                         const RF_String* src);


/**
 ** @memberof RF_StringX
 ** @cppignore
 ** @brief Creates a copy of an extended String and returns it
 **
 ** Also note that the source RF_StringX is not @c const unlike the
 ** RF_String copying functions.
 ** The reason for this is because we actually have to copy all of it,
 ** even if its internal pointer has been moved, in which case
 ** its contents are indeed tampered with in the function so as to allow
 ** reading the whole string. But despite that after the function finishes
 ** the contents of the source string are guaranteed to be the same.
 ** @param  s The  string to copy
 ** @return A copy of the string
 ** @see rfStringX_FromString_OUT()
 ** @see rfStringX_Copy_IN()
 ** @see rfStringX_Copy_chars()
 **/
i_DECLIMEX_ RF_StringX* rfStringX_Copy_OUT(RF_StringX* s);
/**
 ** @memberof RF_StringX
 ** @brief Copies the given source RF_StringX into the destination RF_StringX
 ** @return Returns @c true for success and @c false otherwise
 ** @see rfStringX_FromString_IN()
 ** @see rfStringX_Copy_OUT()
 ** @see rfStringX_Copy_chars()
 **/
i_DECLIMEX_ bool rfStringX_Copy_IN(RF_StringX* dst, RF_StringX* src);
/**
 ** @memberof RF_StringX
 ** @brief Copies a certain number of characters from an RF_StringX
 **
 ** Copies @c n characters from @c src StringX into
 ** the destination @c dst StringX.
 **
 ** @param  dst The String to copy in.
 ** @param  src The String to copy from
 ** @param n  The number of characters to copy from the @c src string.
 ** If the value is bigger than the maximum number of characters then 
 **still all characters are copied.
 ** @return Returns @c true for success and @c false otherwise
 ** @see rfStringX_Copy_OUT()
 ** @see rfStringX_Copy_IN()
 **/
i_DECLIMEX_ bool rfStringX_Copy_chars(RF_StringX* dst, RF_StringX* src,
                                      uint32_t n);


//! @}

//! @name Getting rid of an RF_StringX
//! @{

/**
 ** @memberof RF_StringX
 ** @brief Destroys an extended String and releases its memory.
 **
 ** Use it for strings made with _Create
 ** @param s The exended String to destroy
 ** @see rfStringX_Deinit()
 **/
i_DECLIMEX_ void rfStringX_Destroy(RF_StringX* s);
/**
 ** @memberof RF_StringX
 ** @brief Destroys only the contents of the Extended string
 ** without freeing the pointer itself.
 **
 ** Use it for strings made with _Init
 ** @param s The exended String to destroy
 ** @see rfStringX_Destroy()
 **/
i_DECLIMEX_ void rfStringX_Deinit(RF_StringX* s);

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
