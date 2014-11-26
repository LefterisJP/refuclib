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
#include <Utils/log.h>
/*------------- Outside Module inclusion -------------*/
#include <String/rf_str_core.h> //for static RFstring init
#include <String/rf_str_retrieval.h> //for string accessors
#include <Persistent/buffers.h> // for TSBUFFA
#include <Parallel/rf_threading.h> //for thread id and RFmutex
#include <Utils/sanity.h> //for RF_ASSERT
#include <Utils/memory.h> //for RF_MALLOC
#include <refu.h>         //for refu_clib_get_log()
/*------------- libc inclusion -------------*/
#include <stdio.h> //for printf
#include <string.h> //for memcpy
#include <stdlib.h> //for malloc
#include <sys/time.h> //for timestamps
#include <time.h> //for strftime()
#include <assert.h>
/*------------- End of includes -------------*/

struct RFlog {
    //! The buffer where the log will be kept
    char* buffer;
    //! The index to the buffer
    char *index;
    //! The current size of the log buffer
    uint64_t buff_size;
    //! The log level that we have to keep
    enum RFlog_level level;
    //! The file to which the log is written
    FILE *file;
    //! Mutex to protect the buffer when writting from multiple threads
    //! TODO: Maybe figure out a better synchronization method?
    struct RFmutex lock;
};
static struct RFlog _log;

static const struct RFstring severity_level_string[] = {
    RF_STRING_STATIC_INIT(" [Emergency] "),
    RF_STRING_STATIC_INIT(" [Alert] "),
    RF_STRING_STATIC_INIT(" [Critical] "),
    RF_STRING_STATIC_INIT(" [Error] "),
    RF_STRING_STATIC_INIT(" [Warning] "),
    RF_STRING_STATIC_INIT(" [Debug] "),
    RF_STRING_STATIC_INIT(" [Notice] "),
    RF_STRING_STATIC_INIT(" [Info] "),
    RF_STRING_STATIC_INIT(" [Debug] ")
};

/* Remaining buffer size */
#define REM(i_log_) ((i_log_)->buff_size - OCCUPIED(i_log_) - 1)

/* The buffer position we write at */
#define OCCUPIED(i_log_) ((i_log_)->index - (i_log_)->buffer)

static bool rf_log_init(struct RFlog *log,
                        enum RFlog_level level,
                        char *log_file_name)
{
    log->buff_size = RF_OPTION_LOG_BUFFER_SIZE;
    log->buffer = malloc(RF_OPTION_LOG_BUFFER_SIZE);
    if (!log->buffer) {
        assert(0);
        return false;
    }
    log->index = log->buffer;
    log->level = level;
    if (!rf_mutex_init(&log->lock)) {
        assert(0);
        return false;
    }
    log->file = fopen(log_file_name, "wb+");
    if (!log->file) {
        RF_ASSERT(0, "Log file could not be initialized");
        return false;
    }
    return true;
}

static void rf_log_deinit(struct RFlog *log)
{
    fclose(log->file);
    rf_mutex_deinit(&log->lock);
    free(log->buffer);
}

struct RFlog *rf_log_create(enum RFlog_level level,
                            char *log_file_name)
{
    struct RFlog *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);

    if (!rf_log_init(ret, level, log_file_name)) {
        free(ret);
        return NULL;
    }

    return ret;
}

void rf_log_destroy(struct RFlog *log)
{
    rf_log_deinit(log);
    free(log);
}

bool rf_log_flush(struct RFlog *log)
{
    size_t rc;
    bool ret = true;

    if (!log) {
        // if log has already been freed, e.g.: in an at_exit() of
        return false;
    }

    rf_mutex_lock(&log->lock);
    rc = fwrite(log->buffer, 1, OCCUPIED(log), log->file);
    if (rc != OCCUPIED(log)) {
        ret = false;
    }
    fflush(log->file);
    log->index = log->buffer;
    rf_mutex_unlock(&log->lock);
    return ret;
}

/* TODO: handle no memory case a bit better */
#define INCREASE_BUFFER(i_log_)                               \
    do{                                                       \
        char* tmp;                                            \
        uint64_t index = (i_log_)->index - (i_log_)->buffer;  \
        (i_log_)->buff_size *= 2;                             \
        tmp = realloc((i_log_)->buffer, (i_log_)->buff_size); \
        if(!tmp)                                              \
        {                                                     \
            printf("No memory");                              \
            exit(1);                                          \
        }                                                     \
        (i_log_)->buffer = tmp;                               \
        (i_log_)->index = (i_log_)->buffer + index;           \
    }while(0)

#define CHECK_BUFFER(i_log_, add_size)                  \
    do{                                                 \
        if (add_size >= REM(i_log_))  \
        {                                               \
            INCREASE_BUFFER(i_log_);                    \
        }                                               \
    }while(0);


static inline bool rf_log_add_location(
    struct RFlog *log,
    const char* file,
    const char* func,
    int line)
{
    int file_len, func_len, ret, max;

    file_len = strlen(file);
    func_len = strlen(func);

    /* the maximum amount of bytes for location. +100 is for the rest and '/0' */
    max = file_len + func_len + 100;
    CHECK_BUFFER(log, max);
    ret = snprintf(log->index, max,
                   "[%.*s:%.*s():%d] ",
                   file_len, file, func_len, func, line
    );

    if(ret < 0 || ret >= max) {
        return false;
    }

    log->index += ret;
    return true;
}
static bool format_log_message(struct RFlog *log,
                               enum RFlog_level level,
                               const char* file,
                               const char* func,
                               int line, struct RFstring* msg)
{
    struct timeval tv;
    struct tm *now_tm;
    size_t s;
    int ret;

    gettimeofday(&tv, NULL);
    now_tm = localtime(&tv.tv_sec);

    s = strftime(log->index, REM(log), "%Y-%m-%d %H:%M:%S", now_tm);
    if(!s)
    {
        INCREASE_BUFFER(log);
        s = strftime(log->index, REM(log), "%Y-%m-%d %H:%M:%S", now_tm);
        if(!s)
        {
            RF_ERROR("Can't add timestamp to log");
            return false;
        }
    }
    log->index += s;

    /* Log type */
    CHECK_BUFFER(log, rf_string_length_bytes(&severity_level_string[level]));
    memcpy(
        log->index,
        rf_string_data(&severity_level_string[level]),
        rf_string_length_bytes(&severity_level_string[level])
    );
    log->index += rf_string_length_bytes(&severity_level_string[level]);

    /* Thread ID */
    CHECK_BUFFER(log, 100);
    ret = snprintf(log->index, 100, "(Thread %#010x)", rf_thread_get_id());
    if(ret < 0 || ret >= 100) {
        return false;
    }
    log->index += ret;

    /* Location */
    if (!rf_log_add_location(log, file, func, line)) {
        return false;
    }

    /* Message */
    CHECK_BUFFER(log, rf_string_length_bytes(msg) + 1);
    memcpy(log->index, rf_string_data(msg), rf_string_length_bytes(msg));
    log->index += rf_string_length_bytes(msg);
    *log->index = '\n';
    log->index++;

    return true;
}

static void log_add(struct RFlog *log, enum RFlog_level level,
                    const char* file, const char* func,
                    int line, struct RFstring* msg)
{
    RFS_buffer_push();
    if (log->level < level) {
        RFS_buffer_pop();
        return;
    }


    /* TODO: Maybe use different synchronization strategy here? */
    rf_mutex_lock(&log->lock);
    if (!format_log_message(log, level, file, func, line, msg)) {
        //TODO: how to handle this?
        RF_ASSERT(0, "Could not add a log message");
    }
    rf_mutex_unlock(&log->lock);
    RFS_buffer_pop();
    return;
}



void rf_log(enum RFlog_level level,
            const char* file,
            const char* func,
            int line, struct RFstring* msg)
{
    log_add(refu_clib_get_log(), level, file, func, line, msg);
}

