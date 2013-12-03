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

#ifndef REFU_THREADING_LINUX_H
#define REFU_THREADING_LINUX_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/inline.h>
#include <Definitions/retcodes.h>
/*------------- System specific inclusion -------------*/
#include <pthread.h>
/*------------- End of includes -------------*/

struct RFmutex {
    pthread_mutex_t m;
};

i_INLINE_DECL bool rf_mutex_init(struct RFmutex *mutex)
{
    return pthread_mutex_init(&mutex->m, NULL) == 0;
}

i_INLINE_DECL bool rf_mutex_lock(struct RFmutex *mutex)
{
    return pthread_mutex_lock(&mutex->m) == 0;
}

i_INLINE_DECL bool rf_mutex_unlock(struct RFmutex *mutex)
{
    return pthread_mutex_unlock(&mutex->m) == 0;
}

i_INLINE_DECL bool rf_mutex_deinit(struct RFmutex *mutex)
{
    return pthread_mutex_destroy(&mutex->m) == 0;
}

#endif
