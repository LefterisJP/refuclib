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
#include <String/rf_str_corex.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_traversalx.h> //for rf_stringx_reset()
#include <String/rf_str_core.h> //for some needed RFstring functions
#include "rf_str_common.ph" //for RF_STRINGX_REALLOC()
/*------------- Outside Module inclusion -------------*/
#include <Utils/endianess.h> //for endianess detection
#include <Utils/rf_unicode.h> //for unicode functions
#include <Utils/log.h>
#include <Definitions/retcodes.h> //for error codes
#include <Utils/constcmp.h>  //for RF_HEXLE_UI() macro and others
#include <Utils/localscope.h>//for local scope macros
#include <Utils/memory.h> //for refu memory allocation
#include <Utils/sanity.h> //for the sanity check macros
#include "../Internal/rf_internal_mod.ph" //for the internal buffer
/*------------- libc inclusion --------------*/
#include <stdarg.h> //needed for the va_list
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/

/* === Init functions === */

/* --- Functions that already exist for RFstring - START --- */
struct RFstringx* rf_stringx_createv(const char* lit, ...)
{
    struct RFstringx* ret;
    va_list args;

    va_start(args, lit);
    ret = rf_stringx_createvl(lit, args);
    va_end(args);
    return ret;
}

struct RFstringx* rf_stringx_createvl(const char* lit, va_list args)
{
    struct RFstringx* ret;

    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_stringx_initvl(ret, lit, args)) {
        free(ret);
        return NULL;
    }
    return ret;
}

struct RFstringx* rf_stringx_create(const char* lit)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_init(ret, lit))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_stringx_initv(struct RFstringx* str, const char* lit, ...)
{
    bool ret;
    va_list args;

    va_start(args, lit);
    ret = rf_stringx_initvl(str, lit, args);
    va_end(args);
    return ret;
}

bool rf_stringx_initvl(struct RFstringx* str, const char* lit, va_list args)
{
    bool ret = true;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(str, "got NULL string in function");

    if (!lit) {
        RF_ERROR("String initialization failed due to null pointer input");
        ret = false;
        goto cleanup_lscope;
    }

    //read the var args
    if (!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args)) {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = false;
        goto cleanup_lscope;
    }
    //initialize the string
    if (!rf_stringx_init_unsafe_nnt(str,
                                   (const char*)buff_ptr,
                                   size)) {

        ret = false;
    }


    rf_buffer_set_index(TSBUFFA, buff_index, char);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_init(struct RFstringx* str, const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    RF_ASSERT(str, "got NULL string in function");

    if (!lit) {
        RF_ERROR("Failed to initialize string due to null c string input");
        return false;
    }
    if(!rf_utf8_verify(lit ,&byteLength, 0))
    {
        RF_ERROR("Error at StringX initialization due to invalid UTF-8 "
                 "byte sequence");
        return false;
    }

    return rf_stringx_init_unsafe_nnt(str, lit, byteLength);
}

struct RFstringx* rf_stringx_create_cp(uint32_t codepoint)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_init_cp(ret, codepoint))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rf_stringx_init_cp(struct RFstringx* str, uint32_t codepoint)
{
    RF_ASSERT(str, "got NULL string in function");
    if(!rf_string_init_cp(&str->INH_String, codepoint))
    {
        return false;
    }
    str->bSize = MAX_UTF8C_BYTES;
    str->bIndex = 0;
    return true;
}

struct RFstringx* rf_stringx_create_int(int i)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    if (!rf_stringx_init_int(ret, i)) {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rf_stringx_init_int(struct RFstringx* str, int i)
{
    RF_ASSERT(str, "got NULL string in function");
    if (!rf_string_init_int(&str->INH_String, i)) {
        return false;
    }

    str->bSize = rf_string_length_bytes(str);
    str->bIndex = 0;
    return true;
}
struct RFstringx* rf_stringx_create_double(double d, unsigned int precision)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_stringx_init_double(ret, d, precision)) {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rf_stringx_init_double(struct RFstringx* str,
                            double d,
                            unsigned int precision)
{
    RF_ASSERT(str, "got NULL string in function");
    if (!rf_string_init_double(&str->INH_String, d, precision)) {
        return false;
    }
    str->bSize = rf_string_length_bytes(str);
    return true;
}

struct RFstringx* rf_stringx_create_utf16(const uint16_t* s, unsigned int len)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_stringx_init_utf16(ret, s, len)) {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rf_stringx_init_utf16(struct RFstringx* str,
                           const uint16_t* s,
                           unsigned int len)
{
    RF_ASSERT(str, "got NULL string in function");
    if (!rf_string_init_utf16(&str->INH_String, s, len)) {
        return false;
    }
    str->bIndex = 0;
    str->bSize = rf_string_length_bytes(str);
    return true;
}
struct RFstringx* rf_stringx_create_utf32(const uint32_t* s, unsigned int len)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_init_utf32(ret, s, len))
    {
        free(ret);
        return 0;
    }
    return ret;
}
bool rf_stringx_init_utf32(struct RFstringx* str, const uint32_t* codeBuffer,
                          unsigned int len)
{
    RF_ASSERT(str, "got NULL string in function");
    if(!rf_string_init_utf32(&str->INH_String, codeBuffer, len))
    {
        return false;
    }
    str->bIndex = 0;
    str->bSize = rf_string_length_bytes(str);
    return true;
}
struct RFstringx* rf_stringx_create_unsafe(const char* lit)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_init_unsafe(ret, lit))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
bool rf_stringx_init_unsafe(struct RFstringx* str, const char* lit)
{
    if(!rf_string_init_unsafe(&str->INH_String, lit))
    {
        return false;
    }
    str->bIndex = 0;
    str->bSize = rf_string_length_bytes(str);
    return true;
}
bool rf_stringx_init_unsafe_nnt(struct RFstringx* str, const char* s,
                               size_t length)
{
    RF_ASSERT(str, "got NULL string in function");
    str->bSize = length * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER;
    RF_MALLOC(rf_string_data(str), str->bSize, return false);
    memcpy(rf_string_data(str), s, length);
    rf_string_length_bytes(str) = length;
    str->bIndex = 0;
    return true;
}

bool rf_stringx_init_unsafe_bnnt(struct RFstringx* str, const char* s,
                                size_t length, size_t buff_size)
{
    RF_ASSERT(str, "got NULL string in function");
    str->bSize = buff_size;
    RF_MALLOC(rf_string_data(str), str->bSize, return false);
    memcpy(rf_string_data(str), s, length);
    rf_string_length_bytes(str) = length;
    str->bIndex = 0;
    return true;
}

/* --- Functions that already exist for RFstring - END --- */

struct RFstringx* rf_stringx_create_buffv(uint32_t buffSize, const char* lit, ...)
{
    struct RFstringx* ret;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();

    if (!lit) {
        RF_ERROR("Provided null pointer for the string literal");
        ret = NULL;
        goto cleanup_lscope;
    }

    va_start(args, lit);
    //read the var args
    if(!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = NULL;
        goto cleanup_lscope;
    }
    va_end(args);

    //Make sure that the buff size fits the string
    if(buffSize < size)
    {
        buffSize = size;
    }
    RF_MALLOC(ret, sizeof(*ret), ret = NULL; goto cleanup_buffer);
    if(!rf_stringx_init_unsafe_bnnt(ret, buff_ptr, size, buffSize))
    {
        free(ret);
        ret = NULL;
    }

#ifdef RF_OPTION_DEBUG
cleanup_buffer:
#endif
    rf_buffer_set_index(TSBUFFA, buff_index, char);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
struct RFstringx* rf_stringx_create_buff(uint32_t buffSize,const char* lit)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_init_buff(ret, buffSize, lit))
    {
        free(ret);
        return NULL;
    }
    return ret;
}


bool rf_stringx_init_buffv(struct RFstringx* str, uint32_t buffSize,
                         const char* lit, ...)
{
    bool ret = true;
    va_list args;
    char *buff_ptr;
    unsigned int size, buff_index;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(str, "got NULL string in function");

    if (!lit) {
        RF_ERROR("Provided null pointer for the string literal");
        ret = false;
        goto cleanup_lscope;
    }

    va_start(args, lit);
    //read the var args
    if(!fill_fmt_buffer(lit, &size, &buff_ptr, &buff_index, args))
    {
        RF_ERROR("StringX creation failure due to failing at reading the "
                  "formatted string");
        ret = false;
        goto cleanup_lscope;
    }
    va_end(args);

    //Make sure that the buff size fits the string
    if(buffSize < size)
    {
        buffSize = size;
    }
    ret = rf_stringx_init_unsafe_bnnt(str, buff_ptr, size, buffSize);

    rf_buffer_set_index(TSBUFFA, buff_index, char);
cleanup_lscope:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_init_buff(struct RFstringx* str, uint32_t buffSize,
                             const char* lit)
{
    //check the string literal for valid utf-8 byte sequence
    uint32_t byteLength;
    RF_ASSERT(str, "got NULL string in function");
    if (!lit) {
        RF_WARNING("provided null source string literal");
        return false;
    }
    if(!rf_utf8_verify(lit, &byteLength, 0))
    {
        RF_ERROR("Error at StringX initialization due to invalid UTF-8 "
                  "byte sequence");
        return false;
    }
    //make sure that the buff size fits the string
    if(buffSize < byteLength)
    {
        buffSize = byteLength;
    }
    return rf_stringx_init_unsafe_bnnt(str, lit, byteLength, buffSize);
}


/*---- Methods to copy/assign an RFstringx ----*/

bool rf_stringx_assign(struct RFstringx* dst, const void* source)
{
    bool ret = true;

    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(dst, "got NULL string in function");
    if (!source) {
        RF_WARNING("provided null source string");
        ret = false;
        goto cleanup;
    }
    //only if the new string value won't fit in the buffer reallocate the buffer
    RF_STRINGX_REALLOC(dst,
                       rf_string_length_bytes(source),
                       ret=false; goto cleanup);
    //now copy the value and the bytelength
    memcpy(rf_string_data(dst),
           rf_string_data(source),
           rf_string_length_bytes(source));
    rf_string_length_bytes(dst) = rf_string_length_bytes(source);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

i_DECLIMEX_ bool rf_stringx_assignv(struct RFstringx* str, const char* s, ...)
{
    va_list args;
    bool ret;

    va_start(args, s);
    ret = rf_stringx_assignvl(str, s, args);
    va_end(args);
    return ret;
}
i_DECLIMEX_ bool rf_stringx_assignvl(struct RFstringx* str,
                                     const char* s,
                                     va_list args)
{
    unsigned int size, buff_index;
    bool ret = true;
    char *buff_ptr;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(str, "got NULL string in function");

    if (!s) {
        RF_ERROR("Stringx assignment failed due to null pointer input");
        ret = false;
        goto cleanup;
    }
    //read the var args
    if (!fill_fmt_buffer(s, &size, &buff_ptr, &buff_index, args)) {
        RF_ERROR("Stringx assignment failure due to failing at reading the "
                 "formatted string");
        ret = false;
        goto cleanup;
    }
    rf_buffer_set_index(TSBUFFA, buff_index, char);

    RF_STRINGX_REALLOC(str, size, ret=false;goto cleanup);
    //get length
    rf_string_length_bytes(str) = size;
    memcpy(rf_string_data(str), buff_ptr, rf_string_length_bytes(str));
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_assign_char(struct RFstringx* str, uint32_t codepoint)
{
    int bytes;
    RF_ASSERT(str, "got NULL string in function");
    //reset the buffer if needed
    rf_stringx_reset(str);
    //realloc if needed
    if (str->bSize < 5) {
        RF_REALLOC(rf_string_data(str), char,
                   5 * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER, return false);
    }
    if((bytes=rf_utf8_encode_single(codepoint, rf_string_data(str))) <= 0)
    {
        RF_ERROR("Assigning a character to an RFstringx failed "
                 "due to utf8 encoding");
        return false;
    }
    rf_string_length_bytes(str) = bytes;
    return true;
}

bool rf_stringx_assign_unsafe_nnt(struct RFstringx* str, const char* s,
                                 size_t length)
{
    bool ret = true;
    RF_ASSERT(str, "got NULL string in function");
    if (!s) {
        RF_WARNING("Provided null pointer for assignment");
        return false;
    }

    /* make sure it fits in the string */
    rf_stringx_reset(str);
    RF_STRINGX_REALLOC(str, length, ret=false; goto cleanup);
    //now copy the value
    memcpy(rf_string_data(str), s, length);
    //and fix the lengths
    rf_string_length_bytes(str) = length;

  cleanup:
    return ret;
}

struct RFstringx* rf_stringx_from_string_out(const struct RFstring* s)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_from_string_in(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_stringx_from_string_in(struct RFstringx* dst,
                               const struct RFstring* src)
{
    RF_ASSERT(dst, "got NULL string in function");
    if (!src) {
        RF_WARNING("provided null source string");
        return false;
    }
    //allocate the stringX
    dst->bIndex = 0;
    dst->bSize = (
        rf_string_length_bytes(src) * RF_OPTION_STRINGX_CAPACITY_MULTIPLIER
    );
    RF_MALLOC(rf_string_data(dst), dst->bSize, return false);
    rf_string_length_bytes(dst) = rf_string_length_bytes(src);
    //copy the bytes from the String
    memcpy(rf_string_data(dst), rf_string_data(src), rf_string_length_bytes(src));
    return true;
}

struct RFstringx* rf_stringx_copy_out(struct RFstringx* s)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_copy_in(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_stringx_copy_in(struct RFstringx* dst, struct RFstringx* src)
{
    RF_ASSERT(dst, "got NULL string in function");
    if (!src) {
        RF_WARNING("Provided null source string");
        return false;
    }
    //assert that the source string is copied from the beginning of its buffer
    rf_string_data(src) -= src->bIndex;
    //copy the bytes
    RF_MALLOC(rf_string_data(dst), src->bSize, return false);
    memcpy(rf_string_data(dst), rf_string_data(src), src->bSize);
    //push the source bytes back and also the new copied bytes
    //buffer to the same index
    rf_string_data(src) += src->bIndex;
    rf_string_data(dst) += src->bIndex;
    //copy the parameters
    dst->bSize = src->bSize;
    dst->bIndex = src->bIndex;
    rf_string_length_bytes(dst) = rf_string_length_bytes(src);
    return true;
}

bool rf_stringx_copy_chars(struct RFstringx* dst, struct RFstringx* src,
                          uint32_t charsN)
{
    uint32_t i=0,bytePos;
    if (!rf_stringx_copy_in(dst, src)) {
        return false;
    }
    //find the byte position and end the string there
    RF_STRING_ITERATE_START(src, i, bytePos)
    if(i == charsN)
        break;
    RF_STRING_ITERATE_END(i, bytePos)
    rf_string_length_bytes(dst)= bytePos;
    return true;
}

/*--- Methods to get rid of an RFstringx ---*/

void rf_stringx_destroy(struct RFstringx* s)
{
    //an extended string can have moved its internal pointer forward
    //so we have to put it back at the origin to free properly
    rf_string_data(s) -= s->bIndex;
    free(rf_string_data(s));
    free(s);
}
void rf_stringx_deinit(struct RFstringx* s)
{
    //an extended string can have moved its internal pointer forward
    //so we have to put it back at the origin to free properly
    rf_string_data(s) -= s->bIndex;
    free(rf_string_data(s));
}
