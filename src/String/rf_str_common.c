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
#include <Utils/rf_unicode.h> //for rf_utf8_verify()
#include <Persistent/buffers.h> //for the internal persistent buffer
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
    ret = rf_buffer_malloc(TSBUFFA, sizeof(*ret));
    if (!ret) {
        //pop back the buffer
        rf_buffer_set_index(TSBUFFA, bIndex, char);
        return NULL;
    }
    rf_string_data(ret) = buffPtr;
    rf_string_length_bytes(ret) = size;

    return ret;
}

struct RFstring* i_NVrf_string_create_local(const char* s)
{
    struct RFstring* ret;
    uint32_t byteLength;

    //check for validity of the given sequence and get the character length
    if(!rf_utf8_verify(s, &byteLength, 0))
    {
        RF_ERROR("Error at String Allocation due to invalid "
                 "UTF-8 byte sequence");
        return NULL;
    }

    //allocate the string and its buffer in the local memory stack
    ret = rf_buffer_malloc(TSBUFFA, sizeof(*ret) + byteLength);
    if (!ret) {
        return NULL;
    }
    rf_string_data(ret) = (char*)ret + sizeof(*ret);
    memcpy(rf_string_data(ret), s, byteLength);
    rf_string_length_bytes(ret) = byteLength;

    return ret;
}
