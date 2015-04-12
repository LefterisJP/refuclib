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
#include <Persistent/buffers.h> //for the internal persistent buffer
#include <Data_Structures/darray.h>
/*------------- libc inclusion --------------*/
#include <stdio.h>
#include <stdarg.h> //for va_list
#include <stdlib.h> //for exit()
/*------------- End of includes -------------*/

i_THREAD__ struct RFbuffer g_tsbuff;
#define TSBUFF (&g_tsbuff)

static void rf_strings_buffer_add_string_ptr(struct RFstring *str, uint64_t index);
static void rf_strings_buffer_add_string_buffer_ptr(char *buf, uint64_t index);

struct RFstring* i_rf_string_create_local1(const char* s,...)
{
    struct RFstring* ret;
    va_list args;
    unsigned int size, bIndex;
    uint32_t index_before_string_alloc;
    char *buffPtr;

    //read the var args
    va_start(args, s);
    if (!rf_strings_buffer_fillfmt(s, &size, &buffPtr, &bIndex, args)) {
        RF_ERROR("Local string creation failure due to failing at reading the"
                 " formatted string");
        return NULL;
    }
    va_end(args);

    //allocate the string in its buffer
    index_before_string_alloc = rf_buffer_index(TSBUFF);
    ret = rf_buffer_malloc(TSBUFF, sizeof(*ret));
    if (!ret) {
        //pop back the buffer
        rf_buffer_set_index(TSBUFF, bIndex, char);
        return NULL;
    }
    rf_string_data(ret) = buffPtr;
    rf_string_length_bytes(ret) = size;
    // add it to the context
    rf_strings_buffer_add_string_ptr(ret, index_before_string_alloc);

    return ret;
}

struct RFstring* i_NVrf_string_create_local(const char* s)
{
    struct RFstring* ret;
    uint32_t byteLength;
    uint32_t index_before;
    //check for validity of the given sequence and get the character length
    if (!rf_utf8_verify(s, &byteLength, 0)) {
        RF_ERROR("Error at String Allocation due to invalid "
                 "UTF-8 byte sequence");
        return NULL;
    }

    index_before = rf_buffer_index(TSBUFF);
    //allocate the string and its buffer in the local memory stack
    ret = rf_buffer_malloc(TSBUFF, sizeof(*ret) + byteLength);
    if (!ret) {
        return NULL;
    }
    rf_string_data(ret) = (char*)ret + sizeof(*ret);
    memcpy(rf_string_data(ret), s, byteLength);
    rf_string_length_bytes(ret) = byteLength;
    // add it to the context
    rf_strings_buffer_add_string_ptr(ret, index_before);
    rf_strings_buffer_add_string_buffer_ptr(rf_string_data(ret), index_before + sizeof(*ret));

    return ret;
}

struct rf_strings_buffer_ctx {
    // these are used to remember positions in the buffer for reallocing
    struct {darray(struct RFstring*);} string_ptrs;
    struct {darray(uint64_t);} string_indices;
    struct {darray(char*);} string_buffer_ptrs;
    struct {darray(uint64_t);} string_buffer_indices;

    // these are used when pushing/popping the context
    struct {darray(uint64_t);} mark_str_indices;
    struct {darray(uint64_t);} mark_str_buff_indices;
    struct {darray(uint64_t);} mark_buff_index;
};
i_THREAD__ struct rf_strings_buffer_ctx g_strctx;

bool rf_strings_buffer_realloc_cb(struct RFbuffer *b)
{
    struct RFstring **s;
    char **s_buff;
    size_t i = 0;
    darray_foreach(s, g_strctx.string_ptrs) {
        *s = (struct RFstring*)(b->buff + darray_item(g_strctx.string_indices, i));
        ++i;
    }
    i = 0;
    darray_foreach(s_buff, g_strctx.string_buffer_ptrs) {
        *s_buff = b->buff + darray_item(g_strctx.string_indices, i);
        ++i;
    }
    return true;
}

bool rf_strings_buffer_ctx_init(size_t string_buffer_size)
{
    if (!rf_buffer_init(&g_tsbuff, string_buffer_size, rf_strings_buffer_realloc_cb)) {
        return false;
    }
    darray_init(g_strctx.string_indices);
    darray_init(g_strctx.string_buffer_indices);
    darray_init(g_strctx.string_ptrs);
    darray_init(g_strctx.string_buffer_ptrs);

    darray_init(g_strctx.mark_str_indices);
    darray_init(g_strctx.mark_str_buff_indices);
    darray_init(g_strctx.mark_buff_index);
    return true;
}

void rf_strings_buffer_ctx_deinit()
{
    darray_free(g_strctx.mark_buff_index);
    darray_free(g_strctx.mark_str_buff_indices);
    darray_free(g_strctx.mark_str_indices);
    
    darray_free(g_strctx.string_buffer_ptrs);
    darray_free(g_strctx.string_ptrs);
    darray_free(g_strctx.string_buffer_indices);
    darray_free(g_strctx.string_indices);

    rf_buffer_deinit(&g_tsbuff);
}

char *rf_string_cstr_from_buff(const void *s)
{
    char* ret;
    unsigned int length = rf_string_length_bytes(s);
    ret = rf_buffer_current_ptr(TSBUFF, char);
    if (!rf_buffer_assert_remaining_size(TSBUFF, length)) {
        return NULL;
    }
    memcpy(rf_buffer_current_ptr(TSBUFF, char), rf_string_data(s), length);
    rf_buffer_from_current_at(TSBUFF, length, char) = '\0';
    rf_buffer_move_index(TSBUFF, length + 1, char);
    return ret;
}

bool rf_strings_buffer_fillfmt(const char *fmt,
                               unsigned int *size,
                               char **buffPtr,
                               unsigned int *bIndex,
                               va_list args)
{
    int rc;
    va_list copy_va_list;
    int64_t n = rf_buffer_remaining_size(TSBUFF, char);
    *bIndex = rf_buffer_index(TSBUFF);
    *buffPtr = rf_buffer_current_ptr(TSBUFF, char);
    va_copy(copy_va_list, args); /* C99 only */
    rc = vsnprintf(rf_buffer_current_ptr(TSBUFF, char), n, fmt, copy_va_list);
    va_end(copy_va_list);
    if (rc < 0) {
        return false;
    } else if (rc >= n) {
        if (!rf_buffer_increase_size(TSBUFF, rc * 2)) {
            return false;
        }
        n = rf_buffer_size(TSBUFF);
        *buffPtr = rf_buffer_current_ptr(TSBUFF, char);
        *bIndex = rf_buffer_index(TSBUFF);
        
        rc = vsnprintf(rf_buffer_current_ptr(TSBUFF, char), n, fmt, args);
        if (rc < 0 || rc >= n) {
            return false;
        }
    }
    // add it to the context
    rf_strings_buffer_add_string_buffer_ptr(*buffPtr, *bIndex);
    rf_buffer_move_index(TSBUFF, rc, char);
    *size = rc;

    return true;
}

static void rf_strings_buffer_add_string_ptr(struct RFstring *str, uint64_t index)
{
    RF_ASSERT(
        darray_size(g_strctx.string_ptrs) == darray_size(g_strctx.string_indices),
        "String stack size mismatch"
    );
    darray_append(g_strctx.string_ptrs, str);
    darray_append(g_strctx.string_indices, index);
}

static void rf_strings_buffer_add_string_buffer_ptr(char *buf, uint64_t index)
{
    RF_ASSERT(
        darray_size(g_strctx.string_buffer_ptrs) == darray_size(g_strctx.string_buffer_indices),
        "String buffer stack size mismatch"
    );
                                                                     
    darray_append(g_strctx.string_buffer_ptrs, buf);
    darray_append(g_strctx.string_buffer_indices, index);
}

void rf_strings_buffer_ctx_push()
{
    darray_append(g_strctx.mark_buff_index, rf_buffer_index(TSBUFF));
    darray_append(g_strctx.mark_str_indices, darray_size(g_strctx.string_ptrs));
    darray_append(g_strctx.mark_str_buff_indices, darray_size(g_strctx.string_buffer_ptrs));
}

void rf_strings_buffer_ctx_pop()
{
    uint64_t size;
    size = darray_pop(g_strctx.mark_str_buff_indices);
    while (darray_size(g_strctx.string_buffer_ptrs) > size) {
        (void)darray_pop(g_strctx.string_buffer_ptrs);
    }

    size = darray_pop(g_strctx.mark_str_indices);
    while (darray_size(g_strctx.string_ptrs) > size) {
        (void)darray_pop(g_strctx.string_ptrs);
    }
    rf_buffer_set_index_(TSBUFF, darray_pop(g_strctx.mark_buff_index));
}
