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
/*------------- Module related inclusion -------------*/
#include "rf_str_common.ph" //for fill_fmt_buffer()
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h> //for error logging
#include <String/rf_str_unicode.h> //for rf_utf8_verify_cstr()
#include "../Utils/localmem.ph" //for the private local memory macros
#include "../Internal/rf_internal_mod.ph" //for the internal buffer
/*------------- libc inclusion --------------*/
#include <stdarg.h> //for va_list
#include <stdlib.h> //for exit()
/*------------- End of includes -------------*/

struct RFstring* i_rf_string_create_local1(const char* s,...)
{
    struct RFstring* ret;
    va_list args;
    unsigned int size, bIndex;
    char *buffPtr;
    //remember the stack pointer before this macro evaluation
    i_rf_lms_args_eval(return NULL);

    //read the var args
    va_start(args, s);
    if(!fill_fmt_buffer(s, &size, &buffPtr, &bIndex, args))
    {
        RF_ERROR("Local string creation failure due to failing at reading the"
                 " formatted string");
        return NULL;
    }
    va_end(args);

    //allocate the string in the local memory stack
    i_rf_lms_push(ret, sizeof(*ret), ret = NULL; goto cleanup_buffer);
    //get length
    rf_string_length_bytes(ret) = size;
    //now that we know the length we can allocate the buffer and copy the bytes
    i_rf_lms_push(rf_string_data(ret), ret->length,
                 ret = NULL; goto cleanup_buffer);
    memcpy(rf_string_data(ret), buffPtr, rf_string_length_bytes(ret));

cleanup_buffer:
    //pop back the buffer
    rf_buffer_set_index(TSBUFFA, bIndex);
    return ret;
}
struct RFstring* i_NVrf_string_create_local(const char* s)
{
    struct RFstring* ret;
    uint32_t byteLength;
    //remember the stack pointer before this macro evaluation
    i_rf_lms_args_eval(return NULL);
    //check for validity of the given sequence and get the character length
    if(!rf_utf8_verify_cstr(s, &byteLength))
    {
        RF_ERROR("Error at String Allocation due to invalid "
                 "UTF-8 byte sequence");
        return NULL;
    }

    //allocate the string in the local memory stack
    i_rf_lms_push(ret, sizeof(*ret), return NULL);
    //get length
    rf_string_length_bytes(ret) = byteLength;
    i_rf_lms_push(rf_string_data(ret), byteLength, return NULL);
    memcpy(rf_string_data(ret), s, byteLength);

    return ret;
}

