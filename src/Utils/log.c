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
//for bool
#include <Definitions/retcodes.h>
//for the local scope macros
#include <Utils/localscope.h>
//for static RF_String init
#include <String/core.h>
//for string accessors
#include <String/retrieval.h>
//for thread id
#include <Parallel/threading.h>
/*------------- libc inclusion -------------*/
#include <stdio.h> //for printf
#include <string.h> //for memcpy
#include <stdlib.h> //for malloc
#include <sys/time.h> //for timestamps
#include <time.h> //for strftime()
#include <assert.h>
/*------------- System specific inclusion -------------*/
#include <pthread.h>
/*------------- End of includes -------------*/


typedef struct Log
{
    //! The buffer where the log will be kept
    char* buffer;
    //! The index to the buffer
    char *index;
    //! The current size of the log buffer
    uint64_t buff_size;
    //! The log level that we have to keep
    RF_LogLevel level;
    //! The file to which the log is written
    FILE *file;
    //! Mutex to protect the buffer when writting from multiple threads
    //! TODO: Maybe figure out a better synchronization method?
    pthread_mutex_t lock;
}RF_Log;
static RF_Log _log;

static const RF_String severity_level_string[] = {
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

static bool rfLog_Init(RF_Log *log, RF_LogLevel level, char *log_file_name)
{
    log->buff_size = RF_OPTION_LOG_BUFFER_SIZE;
    log->buffer = malloc(RF_OPTION_LOG_BUFFER_SIZE);
    if (!log->buffer) {
        assert(0);
        return false;
    }
    log->index = log->buffer;
    log->level = level;
    if (pthread_mutex_init(&log->lock, NULL) != 0) {
        assert(0);
        return false;
    }
    log->file = fopen(log_file_name, "ab+");
    if (!log->file) {
        assert(0);
        return false;
    }
    return true;
}

static bool rfLog_Flush(RF_Log *log)
{
    size_t rc;
    bool ret = true;
    pthread_mutex_lock(&log->lock);
    rc = fwrite(log->buffer, 1, OCCUPIED(log), log->file);
    if (rc != OCCUPIED(log)) {
        ret = false;
    }
    log->index = log->buffer;
    pthread_mutex_unlock(&log->lock);
    return ret;
}

static void rfLog_Deinit(RF_Log *log)
{
    fclose(log->file);
    pthread_mutex_destroy(&log->lock);
    free(log->buffer);
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
        if (add_size <= REM(i_log_))  \
        {                                               \
            INCREASE_BUFFER(i_log_);                    \
        }                                               \
    }while(0);


static inline bool rfLog_AddLocation(
    RF_Log *log,
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
                   "[%.*s:%.*s (%d)]",
                   file_len, file, func_len, func, line
    );

    if(ret < 0 || ret >= max) {
        return false;
    }
    
    log->index += ret;
    return true;
}
static bool format_log_message(RF_Log *log,
                               RF_LogLevel level,
                               const char* file,
                               const char* func,
                               int line, RF_String* msg)
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
    CHECK_BUFFER(log, rfString_ByteLength(&severity_level_string[level]));
    memcpy(
        log->index,
        rfString_Data(&severity_level_string[level]),
        rfString_ByteLength(&severity_level_string[level])
    );

    /* Thread ID */
    CHECK_BUFFER(log, 100);
    ret = snprintf(log->index, 100, "(Thread %d)", rf_ThreadGetId());
    if(ret < 0 || ret >= 100) {
        return false;
    }
    log->index += ret;

    /* Location */
    if (!rfLog_AddLocation(log, file, func, line)) {
        return false;
    }

    /* Message */
    CHECK_BUFFER(log, rfString_ByteLength(msg) + 1);
    memcpy(log->index, rfString_Data(msg), rfString_ByteLength(msg));
           
    return true;
}


static void rfLog_Add(RF_Log *log, RF_LogLevel level,
                      const char* file, const char* func,
                      int line, RF_String* msg)
{
    RF_ENTER_LOCAL_SCOPE();
    if(log->level < level)
    {
        RF_EXIT_LOCAL_SCOPE();
        return;
    }


    /* TODO: Maybe use different synchronization strategy here? */
    pthread_mutex_lock(&log->lock);
    if(!format_log_message(log, level, file, func, line, msg))
    {
        //TODO: how to handle this?
        assert(0);
    }
    pthread_mutex_unlock(&log->lock);
    RF_EXIT_LOCAL_SCOPE();
    return;
}



bool rf_LogModule_Init(RF_LogLevel level, char *log_file_name)
{
    return rfLog_Init(&_log, level, log_file_name);
}

void rf_LogModule_Deinit()
{
    rfLog_Deinit(&_log);
}
void rf_Log(RF_LogLevel level, const char* file,
            const char* func,
            int line, RF_String* msg)
{
    rfLog_Add(&_log, level, file, func, line, msg);
}

bool rf_LogFlush()
{
    return rfLog_Flush(&_log);
}
