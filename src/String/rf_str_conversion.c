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
#include <String/rf_str_conversion.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_retrieval.h> //for rf_string_count
#include <String/rf_str_common.h> //for RFS_()
#include "rf_str_conversion.ph" //for rf_string_cstr_ibuff_push()
#include "rf_str_common.ph" //for required string private macros and functions
#include "rf_str_defines.ph" //for some defines
/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for error codes
#include <Utils/log.h>
#include <Utils/localscope.h> //for local scope macros
#include <Utils/rf_unicode.h> //for unicode functions
#include <Utils/memory.h> //for refu memory allocation
#include <Utils/math.h> //for rf_math_max
#include <Utils/sanity.h> //for sanity macros
#include <Persistent/buffers.h> //for the internal persistent buffer
/*------------- libc inclusion --------------*/
#include <errno.h> //for errno
#include <math.h> //for HUGE_VAL
/*------------- End of includes -------------*/


uint16_t* rf_string_to_utf16(const void* s, uint32_t* length)
{
    uint32_t* codepoints,charsN;
    uint16_t* utf16;
    RF_ASSERT(s, "got null string in function");
    if (length == NULL) {
        RF_WARNING("Did not provide a length argument");
        return NULL;
    }
    RF_MALLOC(codepoints, rf_string_length_bytes(s) * 4, return NULL);
    //get the unicode codepoints
    if(!rf_utf8_decode(rf_string_data(s), rf_string_length_bytes(s), &charsN,
                      codepoints, rf_string_length_bytes(s) * 4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        free(codepoints);
        return NULL;
    }
    //encode them in UTF-16, no check here since it comes from an RFstring
    // which is always guaranteed to have valid UTF-8 and as such valid codepoints
    RF_MALLOC(utf16, rf_string_length_bytes(s) * 4, return NULL);
    if(!rf_utf16_encode(codepoints, charsN, length,
                       utf16, rf_string_length_bytes(s) * 4))
    {
        RF_ERROR("Error at encoding a buffer in UTF-16");
        free(utf16);
        free(codepoints);
        utf16 = NULL;
    }
    free(codepoints);
    return utf16;
}

uint32_t* rf_string_to_utf32(const void* s, uint32_t* length)
{
    uint32_t* cp;
    RF_ASSERT(s, "got null string in function");
    if (length == NULL) {
        RF_WARNING("Did not provide a length argument");
        return NULL;
    }
    RF_MALLOC(cp, rf_string_length_bytes(s) * 4, return NULL);
    //get the unicode codepoints
    if(!rf_utf8_decode(rf_string_data(s), rf_string_length_bytes(s), length,
                      cp, rf_string_length_bytes(s) * 4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        cp = NULL;
    }
    return cp;
}

char* rf_string_cstr(const void* str)
{
    char* ret;
    RF_ASSERT(str, "got null string in function");
    RF_MALLOC(ret, rf_string_length_bytes(str) + 1, return NULL);
    memcpy(ret, rf_string_data(str), rf_string_length_bytes(str));
    ret[rf_string_length_bytes(str)] = '\0';
    return ret;
}

bool rf_string_to_int(const void* str, int64_t* v, size_t *off)
{
    char *cstr;
    unsigned int index;
    char *end;
    size_t length;
    bool ret = true;
    RF_ASSERT(str, "got null string in function");
    if (!v) {
        RF_WARNING("Provided null pointer for the returned int");
        return false;
    }

    cstr = rf_string_cstr_ibuff_push(str, &index);
    errno = 0;
    *v = strtoull (cstr, &end, 10);
    length = end - cstr;
    if(length == 0 || errno) {
        ret = false;
    }

    if (off) {
        *off = length - 1;
    }

    rf_string_cstr_ibuff_pop(index);
    return ret;
}

bool rf_string_to_uint(const void* str,
                       size_t start_off,
                       uint64_t* v,
                       size_t *off,
                       int base)
{
    char *cstr;
    unsigned int index;
    char *end;
    bool ret = true;
    size_t length;
    RF_ASSERT(str, "got null string in function");
    if (!v) {
        RF_WARNING("Provided null pointer for the returned uint");
        return false;
    }

    cstr = rf_string_cstr_ibuff_push(str, &index);
    errno = 0;
    *v = strtoull (cstr + start_off, &end, base);
    length = end - cstr;
    if(length - start_off == 0 || errno) {
        ret = false;
    } else {
        if (off) {
            *off = length - 1;
        }
    }

    rf_string_cstr_ibuff_pop(index);
    return ret;
}

i_INLINE_INS bool rf_string_to_uint_dec(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off);
i_INLINE_INS bool rf_string_to_uint_hex(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off);
i_INLINE_INS bool rf_string_to_uint_bin(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off);
i_INLINE_INS bool rf_string_to_uint_oct(const void* thisstr,
                                        uint64_t* v,
                                        size_t *off);

bool rf_string_to_double(const void* str, double* f, size_t *off)
{
    char *cstr;
    unsigned int index;
    char *end;
    bool ret = true;
    size_t length;
    RF_ASSERT(str, "got null string in function");
    if (!f) {
        RF_WARNING("Provided null pointer for the returned double");
        return false;
    }

    cstr = rf_string_cstr_ibuff_push(str, &index);
    errno = 0;
    *f = strtod (cstr, &end);
    length = end - cstr;
    if (length == 0 || errno) {
        ret = false;
    }

    if (off) {
        *off = length - 1;
    }

    rf_string_cstr_ibuff_pop(index);
    return ret;
}

void rf_string_to_lower(void* s)
{
    uint32_t charI,byteI;
    RF_ASSERT(s, "got null string in function");

    RF_STRING_ITERATE_START(s, charI, byteI)
        //if the character is lowercase
        if(rf_string_data(s)[byteI] >= 65 && rf_string_data(s)[byteI] <= 90)
        {
            //turn it into uppercase
            rf_string_data(s)[byteI] += 32;
        }
    RF_STRING_ITERATE_END(charI, byteI)
}

void rf_string_to_upper(void* s)
{
    uint32_t charI, byteI;
    RF_STRING_ITERATE_START(s, charI, byteI)
        //if the character is lowercase
        if(rf_string_data(s)[byteI] >= 97 && rf_string_data(s)[byteI] <= 122)
        {
            //turn it into uppercase
            rf_string_data(s)[byteI] -= 32;
        }
    RF_STRING_ITERATE_END(charI, byteI)
}

bool rf_string_tokenize(const void* str, const void* sep,
                       uint32_t* ret_tokens_num, struct RFstring** tokens)
{
    uint32_t i, sepLen;
    char *s;
    char *e;
    bool ret = true;
    int32_t tokens_num;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(str, "got null string in function");

    if (!sep) {
        RF_WARNING("Did not provide a separator string");
        ret = false;
        goto cleanup_lscope;
    }

    if (!ret_tokens_num || !tokens) {
        RF_WARNING("Null pointers detected for the output data");
        ret = false;
        goto cleanup_lscope;
    }

    //first find the occurences of the separator, and then the number of tokens
    tokens_num = rf_string_count(str, sep, 0, 0, 0) + 1;
    //error checking
    if (tokens_num <= 1) {
        ret = false;
        goto cleanup_lscope;
    }
    //allocate the tokens
    RF_MALLOC(*tokens, sizeof(struct RFstring) * (tokens_num),
              ret = false; goto cleanup_lscope);
    //find the length of the separator
    sepLen = rf_string_length_bytes(sep);

    s = rf_string_data(str);
    e = rf_string_data(str) + rf_string_length_bytes(str);
    for (i = 0; i < tokens_num - 1; i ++) {
        //find each substring
        e = strstr_nnt(s, e - s,
                       rf_string_data(sep), rf_string_length_bytes(sep));
        rf_string_length_bytes(&(*tokens)[i]) = e - s;
        RF_MALLOC(rf_string_data(&(*tokens)[i]),
                  rf_string_length_bytes(&(*tokens)[i]),
                      ret = false; goto cleanup_lscope
        );
        //put in the data
        memcpy(rf_string_data(&(*tokens)[i]),
               s,
               rf_string_length_bytes(&(*tokens)[i])
        );
        //prepare for next sub-string
        s = e + sepLen;

    }
    ///make sure that if it's the last substring we change strategy
    rf_string_length_bytes(&(*tokens)[i]) = (
        rf_string_length_bytes(str) - (s - rf_string_data(str))
    );
    RF_MALLOC(rf_string_data(&(*tokens)[i]),
              rf_string_length_bytes(&(*tokens)[i]),
              ret = false; goto cleanup_lscope
    );
    //put in the data
    memcpy(rf_string_data(&(*tokens)[i]),
           s,
           rf_string_length_bytes(&(*tokens)[i])
    );

    *ret_tokens_num = tokens_num;
cleanup_lscope:
    //success
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
