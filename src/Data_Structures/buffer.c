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

#include <Data_Structures/buffer.h>
#include <Utils/memory.h>

bool rf_buffer_init(struct RFbuffer* b, size_t size, rf_buffer_realloc_cb cb)
{
    b->size = size;
    b->index = 0;
    b->realloc_cb = cb;
    RF_CALLOC(b->buff, size, 1, return false);
    return true;
}

i_INLINE_INS void rf_buffer_deinit(struct RFbuffer* b);
bool rf_buffer_increase_size(struct RFbuffer* b, size_t added_size)
{
    RF_REALLOC(b->buff, char, (b->size + added_size) * 2, return false);
    b->size = (b->size + added_size) * 2;
    return b->realloc_cb ? b->realloc_cb(b) : true;
}
i_INLINE_INS void* rf_buffer_current_ptr_(struct RFbuffer* b);
i_INLINE_INS size_t rf_buffer_remaining_size_(struct RFbuffer* b);
i_INLINE_INS bool rf_buffer_assert_remaining_size(struct RFbuffer *b, size_t size);
i_INLINE_INS bool rf_buffer_assert_remaining_size_detect_realloc(struct RFbuffer *b,
                                                                 size_t size,
                                                                 bool *had_realloc);
i_INLINE_INS size_t rf_buffer_size(struct RFbuffer* b);
i_INLINE_INS void rf_buffer_set_index_(struct RFbuffer* b, unsigned int index);
i_INLINE_INS unsigned int rf_buffer_index(struct RFbuffer* b);
i_INLINE_INS void *rf_buffer_malloc(struct RFbuffer *b, size_t len_bytes);
i_INLINE_INS void *rf_buffer_malloc_or_detect_realloc(struct RFbuffer *b,
                                                      size_t len_bytes,
                                                      bool *had_realloc);
