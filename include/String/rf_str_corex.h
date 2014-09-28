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
#ifndef RF_STRING_COREX_H
#define RF_STRING_COREX_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstring/X
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
/*------------- libc inclusion --------------*/
#include <string.h> //for size_t
#include <stdarg.h>  //for va_XXX stuff
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name RFstringx Creation
//! @{



/* --- Functions that already exist for RFstring - START --- */
#define RF_STRINGX_STATIC_INIT(s_) {            \
        {RF_STRING_STATIC_INIT(s_)},            \
        .bIndex=0,                              \
         .bSize=sizeof(s_) - 1                  \
         }
i_DECLIMEX_ struct RFstringx* rf_stringx_create(const char* lit);
i_DECLIMEX_ struct RFstringx* rf_stringx_createv(const char* lit,...);
i_DECLIMEX_ struct RFstringx* rf_stringx_createvl(const char* lit,
                                                 va_list args);

i_DECLIMEX_ bool rf_stringx_init(struct RFstringx* str,const char* lit);
i_DECLIMEX_ bool rf_stringx_initv(struct RFstringx* str,const char* lit,...);
i_DECLIMEX_ bool rf_stringx_initvl(struct RFstringx* str,
                                   const char* lit,
                                   va_list args);



i_DECLIMEX_ struct RFstringx* rf_stringx_create_cp(uint32_t code);
i_DECLIMEX_ bool rf_stringx_init_cp(struct RFstringx* str, uint32_t code);

i_DECLIMEX_ struct RFstringx* rf_stringx_create_unsafe(const char* lit);
i_DECLIMEX_ bool rf_stringx_init_unsafe(struct RFstringx* str, const char* lit);

i_DECLIMEX_ bool rf_stringx_init_unsafe_nnt(struct RFstringx* str, const char* s,
                                           size_t length);
i_DECLIMEX_ bool rf_stringx_init_unsafe_bnnt(
    struct RFstringx* str, const char* s,
    size_t length, size_t buff_size);


i_DECLIMEX_ struct RFstringx* rf_stringx_create_int(int i);
i_DECLIMEX_ bool rf_stringx_init_int(struct RFstringx* str, int i);

i_DECLIMEX_ struct RFstringx* rf_stringx_create_double(double d,
                                                       unsigned int precision);
i_DECLIMEX_ bool rf_stringx_init_double(struct RFstringx* str,
                                        double d,
                                        unsigned int precision);

i_DECLIMEX_ struct RFstringx* rf_stringx_create_utf16(const uint16_t* s,
                                               unsigned int len);
i_DECLIMEX_ bool rf_stringx_init_utf16(struct RFstringx* str,
                                      const uint16_t* s,
                                      unsigned int len);

i_DECLIMEX_ struct RFstringx* rf_stringx_create_utf32(const uint32_t* s,
                                               unsigned int len);
i_DECLIMEX_ bool rf_stringx_init_utf32(struct RFstringx* str, const uint32_t* s,
                                      unsigned int len);


/* --- Functions that already exist for RFstring - END --- */

/**
 ** @brief Creates an RFstringx with a specified buffer size
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
i_DECLIMEX_ struct RFstringx* rf_stringx_create_buff(uint32_t buffSize,
                                              const char* lit);

/**
 ** @brief Creates an RFstringx with a specified buffer size (varargs)
 **/
i_DECLIMEX_ struct RFstringx* rf_stringx_create_buffv(uint32_t buffSize,
                                                      const char* lit,
                                                      ...);


/**
 ** @brief Initializes an RFstringx with a specified buffer size
 ** @lmsFunction
 ** @see rf_stringx_create_buff() for details
 **/
i_DECLIMEX_ bool rf_stringx_init_buff(struct RFstringx* str,
                                     uint32_t buffSize,
                                     const char* lit);

/**
 ** @brief Initializes an RFstringx with a specified buffer size
 ** @lmsFunction
 ** @see rf_stringx_init_buffv() for details
 **/
i_DECLIMEX_ bool rf_stringx_init_buffv(struct RFstringx* str,
                                     uint32_t buffSize,
                                     const char* lit,
                                      ...);

//! @}

//! @name RFstringx Copying-Assigning
//! @{

/**
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
i_DECLIMEX_ bool rf_stringx_assign(struct RFstringx* dest, const void* source);

/**
 ** @brief Assigns to a stringx with the given format (varargs)
 ** Assigns to the current stringx position.
 **
 ** @lmsFunction
 **
 **/
i_DECLIMEX_ bool rf_stringx_assignv(struct RFstringx* str, const char* s, ...);
i_DECLIMEX_ bool rf_stringx_assignvl(struct RFstringx* str,
                                     const char* s,
                                     va_list args);

/**
 ** @brief Assigns the value of a unicode character to the string
 **
 ** @param thisstr The string to assign to
 ** @param character The unicode character codepoint to assign to the String
 ** @return Returns @c true for succesfull assignment and @c false
 ** if the given @c character was not a valid unicode codepoint
 ** @see rf_stringx_assign()
 **/
i_DECLIMEX_ bool rf_stringx_assign_char(struct RFstringx* thisstr, uint32_t character);

/**
 ** @brief Assigns to a stringx from a non-null terminated char* buffer
 ** and for the given bytelength
 **
 **/
i_DECLIMEX_ bool rf_stringx_assign_unsafe_nnt(struct RFstringx* str, const char* s,
                                              size_t length);

/**
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
 ** @brief Checks that a string is null
 **/
#define rf_stringx_is_null(i_STRING)              \
    ((rf_string_length_bytes(i_STRING) == 0) &&    \
     (rf_string_data(i_STRING) == NULL))

/**
 ** @brief Creates an RFstringx from an RF_string
 **
 ** Returns an RFstringx version of the parameter RFstring
 ** @param s The RFstring from which to create the RFstringx
 ** @return The RFstringx version of the parameter string
 ** @see rf_stringx_from_string_in()
 ** @see rf_stringx_copy_out()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ struct RFstringx* rf_stringx_from_string_out(
    const struct RFstring* s
);
/**
 ** @brief Initializes an RFstringx from an RF_string
 **
 ** @param dst The RFstringx that will get initialize
 ** @param src The RFstring to copy from
 ** @return Returns @c true for success and @c false otherwise
 ** @see rf_stringx_from_string_out()
 ** @see rf_stringx_copy_in()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ bool rf_stringx_from_string_in(struct RFstringx* dst,
                                           const struct RFstring* src);


/**
 ** @brief Shallow copy of 2 RFstringx
 **/
#define RF_STRINGX_SHALLOW_COPY(i_dst, i_src)                           \
    do {                                                                \
        rf_string_data(i_dst) = rf_string_data(i_src);                  \
        rf_string_length_bytes(i_dst) = rf_string_length_bytes(i_src);  \
        (i_dst)->bSize = (i_src)->bSize;                                \
        (i_dst)->bIndex = (i_src)->bIndex;                              \
    }while(0)

/**
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
i_DECLIMEX_ struct RFstringx* rf_stringx_copy_out(struct RFstringx* s);
/**
 ** @brief Copies the given source RFstringx into the destination RFstringx
 ** @return Returns @c true for success and @c false otherwise
 ** @see rf_stringx_from_string_in()
 ** @see rf_stringx_copy_out()
 ** @see rf_stringx_copy_chars()
 **/
i_DECLIMEX_ bool rf_stringx_copy_in(struct RFstringx* dst, struct RFstringx* src);
/**
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
i_DECLIMEX_ bool rf_stringx_copy_chars(struct RFstringx* dst, struct RFstringx* src,
                                      uint32_t n);


//! @}

//! @name Getting rid of an RFstringx
//! @{

/**
 ** @brief Destroys an extended String and releases its memory.
 **
 ** Use it for strings made with _Create
 ** @param s The exended String to destroy
 ** @see rf_stringx_deinit()
 **/
i_DECLIMEX_ void rf_stringx_destroy(struct RFstringx* s);
/**
 ** @brief Destroys only the contents of the Extended string
 ** without freeing the pointer itself.
 **
 ** Use it for strings made with _Init
 ** @param s The exended String to destroy
 ** @see rf_stringx_destroy()
 **/
i_DECLIMEX_ void rf_stringx_deinit(struct RFstringx* s);

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
