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
#include <String/rf_str_common.h>
/*------------- Outside Module inclusion -------------*/
#include <Utils/sanity.h>
#include <Utils/log.h> //for error logging
#include <Utils/rf_unicode.h> //for rf_utf8_verify()
/*------------- libc inclusion --------------*/
#include <stdio.h>
#include <stdarg.h> //for va_list
#include <stdlib.h> //for exit()
/*------------- End of includes -------------*/

struct RFstring *i_rf_string_create_localv(const char *s, ...)
{
    va_list args;
    unsigned int size;
    char *buffPtr;
    bool rc;
    struct RFstring *ret;

    ret = rf_mbuffer_alloc(RF_TSBUFFM, sizeof(*ret));
    if (!ret) {
        return NULL;
    }

    // read the var args into the buffer
    va_start(args, s);
    rc = rf_strings_buffer_fillfmt(s, &size, &buffPtr, args);
    va_end(args);
    if (!rc) {
        return NULL;
    }
    rf_string_data(ret) = buffPtr;
    rf_string_length_bytes(ret) = size;

    return ret;
}

// to avoid duplication I could have a function accepting va_list and in both
// cases create it and call said function
struct RFstring *i_rf_string_create_localv_or_die(const char *s, ...)
{
    va_list args;
    unsigned int size;
    char *buffPtr;
    bool rc;
    struct RFstring *ret;

    ret = rf_mbuffer_alloc(RF_TSBUFFM, sizeof(*ret));
    if (!ret) {
        goto fail;
    }

    // read the var args into the buffer
    va_start(args, s);
    rc = rf_strings_buffer_fillfmt(s, &size, &buffPtr, args);
    va_end(args);
    if (!rc) {
        goto fail;
    }
    rf_string_data(ret) = buffPtr;
    rf_string_length_bytes(ret) = size;

    return ret;
fail:
    RF_CRITICAL("RFS() failure");
    exit(1);
}

struct RFstring *i_rf_string_create_local(const char *s)
{
    uint32_t byteLength;
    struct RFstring *ret = NULL;
    //check for validity of the given sequence and get the character length
    if (!rf_utf8_verify(s, &byteLength, 0)) {
        RF_ERROR("Error at String Allocation due to invalid "
                 "UTF-8 byte sequence");
        goto end;
    }

    //allocate the string and its buffer in the local memory stack
    ret = rf_mbuffer_alloc(RF_TSBUFFM, sizeof(*ret) + byteLength);
    if (!ret) {
        goto end;
    }
    rf_string_data(ret) = (char*)ret + sizeof(*ret);
    memcpy(rf_string_data(ret), s, byteLength);
    rf_string_length_bytes(ret) = byteLength;

end:
    return ret;
}

struct RFstring *i_rf_string_create_local_or_die(const char *s)
{
    struct RFstring *ret = i_rf_string_create_local(s);
    if (!ret) {
        RF_CRITICAL("RFS() failure");
        exit(1);
    }
    return ret;
}

char *rf_string_cstr_from_buff(const void *s)
{
    char *ret;
    unsigned int length = rf_string_length_bytes(s);
    ret = rf_mbuffer_alloc(RF_TSBUFFM, length + 1);
    if (!ret) {
        return NULL;
    }
    memcpy(ret, rf_string_data(s), length);
    ret[length] = '\0';
    return ret;
}

char *rf_string_cstr_from_buff_or_die(const void* s)
{
    char *ret = rf_string_cstr_from_buff(s);
    if (!ret) {
        RF_CRITICAL("Failure to create null terminated cstring from RFstring");
        exit(1);
    }
    return ret;
}

bool rf_strings_buffer_fillfmt(const char *fmt,
                               unsigned int *size,
                               char **buff_ptr,
                               va_list args)
{
    int rc;
    va_list copy_va_list;
    size_t n = rf_mbuffer_currblock_remsize(RF_TSBUFFM);
    *buff_ptr = rf_mbuffer_alloc(RF_TSBUFFM, n);
    if (!*buff_ptr) {
        return false;
    }
    va_copy(copy_va_list, args); /* C99 only */
    rc = vsnprintf(*buff_ptr, n, fmt, copy_va_list);
    va_end(copy_va_list);
    if (rc < 0) {
        return false;
    } else if (rc > (int)n) {
        size_t needed_size = rc + 1; // +1 is for the null terminating character
        rf_mbuffer_shrink(RF_TSBUFFM, n);
        *buff_ptr = rf_mbuffer_alloc(RF_TSBUFFM, needed_size);
        if (!*buff_ptr) {
            return false;
        }
        rc = vsnprintf(*buff_ptr, needed_size, fmt, args);
        if (rc < 0 || rc > (int)needed_size) {
            return false;
        }
    } else {
        rf_mbuffer_shrink(RF_TSBUFFM, n - rc);
    }
    *size = rc;
    return true;
}
