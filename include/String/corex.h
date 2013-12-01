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
** This header includes core RFstringx functionality (initialization, deinitialization, copying e.t.c.)
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

//! @name RFstringx Creation
//! @{

/* --- Functions that already exist for RFstring - START --- */
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create(const char* lit);
i_DECLIMEX_ RFstringx* rf_stringx_createv(const char* lit,...);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init(RFstringx* str,const char* lit);
i_DECLIMEX_ bool rf_stringx_initv(RFstringx* str,const char* lit,...);


/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_cp(uint32_t code);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_cp(RFstringx* str, uint32_t code);
/**
 ** @memberof RFstringx
 ** @brief Allocates and returns an extended String without any checks.
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_unsafe(const char* lit);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_unsafe(RFstringx* str, const char* lit);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_unsafe_nnt(RFstringx* str, const char* s,
                                           size_t length);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_unsafe_bnnt(
    RFstringx* str, const char* s,
    size_t length, size_t buff_size);

/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_i(int32_t i);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_i(RFstringx* str, int32_t i);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_f(float f);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_f(RFstringx* str, float f);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_utf16(const uint16_t* s,
                                               unsigned int len);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_utf16(RFstringx* str,
                                      const uint16_t* s,
                                      unsigned int len);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_utf32(const uint32_t* s,
                                               unsigned int len);
/**
 ** @memberof RFstringx
 **/
i_DECLIMEX_ bool rf_stringx_init_utf32(RFstringx* str, const uint32_t* s,
                                      unsigned int len);


/* --- Functions that already exist for RFstring - END --- */

/**
 ** @memberof RFstringx
 ** @brief Creates an RFsting_x with a specified buffer size
 **
 ** Allocates and returns an extended String allocating buffSize
 ** bytes buffer and puts @c lit inside that buffer. If the contents
 ** of the string are bigger than the buffer Size then an error is logged
 ** but the String is initialized correctly.
 ** @lmsFunction
 ** @param buffSize               A positive value of bytes for the size of the
 **                               buffer to allocate
 ** @param lit                    The string literal with which to initialize.
 **                               A check to see if it is
 **                               a valid UTF-8 sequence is performed
 ** @return                       The newly initialized string
 ** @see rf_stringx_init_buff()
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_buff(uint32_t buffSize,
                                              const char* lit);

/**
 ** @memberof RFstringx
 ** @brief Creates an RFsting_x with a specified buffer size (varargs)
 **/
i_DECLIMEX_ RFstringx* rf_stringx_create_buffv(uint32_t buffSize,
                                              const char* lit,
                                               ...);


/**
 ** @memberof RFstringx
 ** @brief Initializes an RFsting_x with a specified buffer size
 ** @lmsFunction
 ** @see rf_stringx_create_buff() for details
 **/
i_DECLIMEX_ bool rf_stringx_init_buff(RFstringx* str,
                                     uint32_t buffSize,
                                     const char* lit);

/**
 ** @memberof RFstringx
 ** @brief Initializes an RFsting_x with a specified buffer size
 ** @lmsFunction
 ** @see rf_stringx_init_buffv() for details
 **/
i_DECLIMEX_ bool rf_stringx_init_buffv(RFstringx* str,
                                     uint32_t buffSize,
                                     const char* lit,
                                      ...);

//! @}

//! @name RFstringx Copying-Assigning
//! @{

/**
 ** @memberof RFstringx
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
 ** @see rf_stringx_assign_char()
 **/
i_DECLIMEX_ bool rf_stringx_assign(RFstringx* dest, const void* source);

/**
 ** @memberof RFstringx
 ** @brief Assigns the value of a unicode character to the string
 **
 ** @param thisstr The string to assign to
 ** @param character The unicode character codepoint to assign to the String
 ** @return Returns @c true for succesfull assignment and @c false
 ** if the given @c character was not a valid unicode codepoint
 ** @see rf_stringx_assign()
 **/
i_DECLIMEX_ bool rf_stringx_assign_char(RFstringx* thisstr, uint32_t character);

/**
 ** @memberof RFstringx
 ** @brief Nullifies a string
 ** @warning Use null strings at your own risk. None of the RF_String/X
 ** functions currently test for them
 ** A safer and easier alternative is to assign an empty string and
 * check for it with @ref rf_string_is_empty()
 **/
#define rf_stringx_null(i_STRING) do{            \
        rf_string_length_bytes(i_STRING) = 0;      \
        rf_string_data(i_STRING) = NULL;         \
        (i_STRING)->bIndex = 0;                 \
        (i_STRING)->bSize = 0;                  \
}while(0)

/**
 ** @memberof RFstringx
 ** @brief Checks that a string is null
 **/
#define rf_stringx_is_null(i_STRING)              \
    ((rf_string_length_bytes(i_STRING) == 0) &&    \
     (rf_string_data(i_STRING) == NULL))

/**
 ** @memberof RFstringx
 ** @cppignore
 ** @brief Creates an RFstringx from an RF_string
 **
 ** Returns an RFstringx version of the parameter RFstring
 ** @param s The RFstring from which to create the RFstringx
 ** @return The RFstringx version of the parameter string
 ** @see rf_stringx_from_string_in()
 ** @see rf_stringx_copy_out()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ RFstringx* rf_stringx_from_string_out(const RFstring* s);
/**
 ** @memberof RFstringx
 ** @cppignore
 ** @brief Initializes an RFstringx from an RF_string
 **
 ** @param dst The RFstringx that will get initialize
 ** @param src The RFstring to copy from
 ** @return Returns @c true for success and @c false otherwise
 ** @see rf_stringx_from_string_out()
 ** @see rf_stringx_copy_in()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ bool rf_stringx_from_string_in(RFstringx* dst,
                                         const RFstring* src);


/**
 ** @memberof RFstringx
 ** @cppignore
 ** @brief Creates a copy of an extended String and returns it
 **
 ** Also note that the source RFstringx is not @c const unlike the
 ** RFstring copying functions.
 ** The reason for this is because we actually have to copy all of it,
 ** even if its internal pointer has been moved, in which case
 ** its contents are indeed tampered with in the function so as to allow
 ** reading the whole string. But despite that after the function finishes
 ** the contents of the source string are guaranteed to be the same.
 ** @param  s The  string to copy
 ** @return A copy of the string
 ** @see rf_stringx_from_string_out()
 ** @see rf_stringx_copy_in()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ RFstringx* rf_stringx_copy_out(RFstringx* s);
/**
 ** @memberof RFstringx
 ** @brief Copies the given source RFstringx into the destination RFstringx
 ** @return Returns @c true for success and @c false otherwise
 ** @see rf_stringx_from_string_in()
 ** @see rf_stringx_copy_out()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ bool rf_stringx_copy_in(RFstringx* dst, RFstringx* src);
/**
 ** @memberof RFstringx
 ** @brief Copies a certain number of characters from an RFstringx
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
 ** @see rf_stringx_copy_out()
 ** @see rf_stringx_copy_in()
 **/
i_DECLIMEX_ bool rf_stringx_copy_chars(RFstringx* dst, RFstringx* src,
                                      uint32_t n);


//! @}

//! @name Getting rid of an RFstringx
//! @{

/**
 ** @memberof RFstringx
 ** @brief Destroys an extended String and releases its memory.
 **
 ** Use it for strings made with _Create
 ** @param s The exended String to destroy
 ** @see rf_stringx_deinit()
 **/
i_DECLIMEX_ void rf_stringx_destroy(RFstringx* s);
/**
 ** @memberof RFstringx
 ** @brief Destroys only the contents of the Extended string
 ** without freeing the pointer itself.
 **
 ** Use it for strings made with _Init
 ** @param s The exended String to destroy
 ** @see rf_stringx_destroy()
 **/
i_DECLIMEX_ void rf_stringx_deinit(RFstringx* s);

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
