/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
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
/*------------- libc inclusion -------------*/
#include <stdio.h> //for printf
#include <string.h> //for memcpy
#include <stdlib.h> //for malloc
#include <sys/time.h> //for timestamps
#include <time.h> //for strftime()
/*------------- End of includes -------------*/


/* temporary just to compile. Move and implement separately */
int rf_thread_get_id()
{
    return 0;
}

typedef struct error_context
{
    char* buffer;
    uint64_t index;
    uint64_t buff_size;
    log_level_t level;
}error_context;
static error_context _ctx;

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
#define REM (_ctx.buff_size - _ctx.index - 1)

/* The buffer position we write at */
#define BPOS (_ctx.buffer + _ctx.index)

void rfLog_Init(log_level_t level)
{
    _ctx.buff_size = RF_OPTION_LOG_BUFFER_SIZE;
    _ctx.buffer = malloc(RF_OPTION_LOG_BUFFER_SIZE);
    _ctx.index = 0;
    _ctx.level = level;
}

/* TODO: handle no memory case a bit better */
#define INCREASE_BUFFER() \
    do{                                             \
        char* tmp;                                  \
        _ctx.buff_size *= 2;                        \
        tmp = realloc(_ctx.buffer, _ctx.buff_size); \
        if(!tmp)                                    \
        {                                           \
            printf("No memory");                    \
            exit(1);                                \
        }                                           \
        _ctx.buffer = tmp;                          \
    }while(0)

#define CHECK_BUFFER(add_size)                  \
    do{                                         \
        if (_ctx.index + add_size >= REM)       \
        {                                       \
            INCREASE_BUFFER();                  \
        }                                       \
    }while(0);


static inline bool add_location(const char* file,
                                const char* func,
                                int line)
{
    int file_len, func_len, ret, max;
    
    file_len = strlen(file);
    func_len = strlen(func);
    
    /* the maximum amount of bytes for location. +100 is for the rest and '/0' */
    max = file_len + func_len + 100;
    CHECK_BUFFER(max);
    ret = snprintf(BPOS, max,
                   "[%.*s:%.*s (%d)]",
                   file_len, file, func_len, func, line
    );

    if(ret < 0 || ret >= max) {return false;}
    
    _ctx.index += ret;
    return true;
}
static bool format_log_message(log_level_t level,
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

    s = strftime(BPOS, REM, "%Y-%m-%d %H:%M:%S", now_tm);
    if(!s)
    {
        INCREASE_BUFFER();
        s = strftime(BPOS, REM, "%Y-%m-%d %H:%M:%S", now_tm);
        if(!s)
        {
            RF_ERROR("Can't add timestamp to log");
            return false;
        }
    }
    _ctx.index += s;

    /* Log type */
    CHECK_BUFFER(rfString_ByteLength(&severity_level_string[level]));
    memcpy(BPOS, rfString_Data(&severity_level_string[level]),
           rfString_ByteLength(&severity_level_string[level]));

    /* Thread ID */
    CHECK_BUFFER(100);
    ret = snprintf(BPOS, 100, "(Thread %d)", rf_thread_get_id());
    if(ret < 0 || ret >= 100) {return false;}
    _ctx.index += ret;

    /* Location */
    if(!add_location(file, func, line)) {return false;}

    /* Message */
    CHECK_BUFFER(rfString_ByteLength(msg) + 1);
    memcpy(BPOS, rfString_Data(msg), rfString_ByteLength(msg));
           
    return true;
}


void rfLog_Add(log_level_t level, const char* file,
               const char* func,
               int line, RF_String* msg)
{
    RF_ENTER_LOCAL_SCOPE();
    if(_ctx.level < level)
    {
        RF_EXIT_LOCAL_SCOPE();
        return;
    }


    /* TODO: add a lock here to protect for multithreading */

    if(!format_log_message(level, file, func, line, msg))
    {
        //TODO: how to handle this?
    }
    RF_EXIT_LOCAL_SCOPE();
    return;
}
