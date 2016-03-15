/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/utils/log.h>

#include <rflib/string/core.h>
#include <rflib/string/retrieval.h>
#include <rflib/string/conversion.h>
#include <rflib/parallel/rf_threading.h>
#include <rflib/utils/sanity.h>
#include <rflib/utils/memory.h>
#include <rflib/refu.h>        

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

/* -- RFlog_target functionality -- */
bool rflog_target_init(struct RFlog_target *t,
                       enum RFlog_target_type type,
                       const char *file_name)
{
    t->type = type;
    if (type == LOG_TARGET_FILE) {
        if (!file_name) {
            return false;
        }

        if (!rf_string_init(&t->file_name, file_name)) {
            return false;
        }

        // create a new file for the log
        FILE *f = fopen(file_name, "w");
        fclose(f);
    }
    return true;
}

void rflog_target_deinit(struct RFlog_target *t)
{
    if (t->type == LOG_TARGET_FILE) {
        rf_string_deinit(&t->file_name);
    }
}

/* -- RFlog functionality -- */

struct RFlog {
    //! The buffer where the log will be kept
    char* buffer;
    //! The index to the buffer
    char *index;
    //! The current size of the log buffer
    uint64_t buff_size;
    //! The log level that we have to keep
    enum RFlog_level level;
    //! Description of the log target
    struct RFlog_target target;
    //! Mutex to protect the buffer when writting from multiple threads
    //! TODO: Maybe figure out a better synchronization method?
    struct RFmutex lock;
};

/* Keep in sync with @c enum RFlog_level */
static const struct RFstring severity_level_string[] = {
    RF_STRING_STATIC_INIT(" [Emergency] "),
    RF_STRING_STATIC_INIT(" [Alert] "),
    RF_STRING_STATIC_INIT(" [Critical] "),
    RF_STRING_STATIC_INIT(" [Error] "),
    RF_STRING_STATIC_INIT(" [Warning] "),
    RF_STRING_STATIC_INIT(" [Notice] "),
    RF_STRING_STATIC_INIT(" [Info] "),
    RF_STRING_STATIC_INIT(" [Debug] "),
};

/* Remaining buffer size */
#define REM(i_log_) ((i_log_)->buff_size - OCCUPIED(i_log_) - 1)

/* The buffer position we write at */
#define OCCUPIED(i_log_) ((i_log_)->index - (i_log_)->buffer)

static bool rf_log_init(struct RFlog *log,
                        enum RFlog_target_type type,
                        const char *log_file_name,
                        enum RFlog_level level)
{
    log->buff_size = RF_OPTION_LOG_BUFFER_SIZE;
    log->buffer = malloc(RF_OPTION_LOG_BUFFER_SIZE);
    if (!log->buffer) {
        assert(0);
        return false;
    }
    log->index = log->buffer;
    log->level = level;

    if (!rflog_target_init(&log->target, type, log_file_name)) {
        assert(0);
        return false;
    }

    if (!rf_mutex_init(&log->lock)) {
        assert(0);
        return false;
    }

    return true;
}

static void rf_log_deinit(struct RFlog *log)
{
    rflog_target_deinit(&log->target);
    rf_mutex_deinit(&log->lock);
    free(log->buffer);
}

struct RFlog *rf_log_create(enum RFlog_target_type type,
                            const char *log_file_name,
                            enum RFlog_level level)
{
    struct RFlog *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);

    if (!rf_log_init(ret, type, log_file_name, level)) {
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

// called only on a log that has a file target and only after holding log mutex
static bool rf_log_flush_file(struct RFlog *log)
{
    int rc;
    FILE *f;

    RFS_PUSH();
    f = fopen(rf_string_cstr_from_buff_or_die(&log->target.file_name), "ab");
    RFS_POP();
    if (!f) {
        return false;
    }

    rc = fwrite(log->buffer, 1, OCCUPIED(log), f);
    if (rc != OCCUPIED(log)) {
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

// called only on a log that has an std stream target and only after holding log mutex
static bool rf_log_flush_stdstream(struct RFlog *log)
{
    int rc;
    if (log->target.type == LOG_TARGET_STDOUT) {
        rc = fwrite(log->buffer, 1, OCCUPIED(log), stdout);
        fflush(stdout);
    } else {
        RF_ASSERT(LOG_TARGET_STDERR, "at this point log target should only be stderr");
        rc = fwrite(log->buffer, 1, OCCUPIED(log), stderr);
        fflush(stderr);
    }

    if (rc != OCCUPIED(log)) {
        return false;
    }

    return true;
}

bool rf_log_flush(struct RFlog *log)
{
    bool ret = true;

    if (!log) {
        // if log has already been freed, e.g.: in an at_exit() of
        return false;
    }

    rf_mutex_lock(&log->lock);

    if (log->target.type == LOG_TARGET_FILE) {
        ret = rf_log_flush_file(log);
    } else {
        ret = rf_log_flush_stdstream(log);
    }

    // reset buffer index
    log->index = log->buffer;

    rf_mutex_unlock(&log->lock);
    return ret;
}

/* TODO: handle no memory case a bit better */
#define INCREASE_BUFFER(i_log_)                                 \
    do {                                                        \
        char* tmp;                                              \
        uint64_t index = (i_log_)->index - (i_log_)->buffer;    \
        (i_log_)->buff_size *= 2;                               \
        tmp = realloc((i_log_)->buffer, (i_log_)->buff_size);   \
        if(!tmp) {                                              \
            printf("No memory");                                \
            exit(1);                                            \
        }                                                       \
        (i_log_)->buffer = tmp;                                 \
        (i_log_)->index = (i_log_)->buffer + index;             \
    } while(0)

#define CHECK_BUFFER(i_log_, add_size)          \
    do {                                        \
        if (add_size >= REM(i_log_)) {          \
            INCREASE_BUFFER(i_log_);            \
        }                                       \
    } while(0)


static inline bool rf_log_add_location(
    struct RFlog *log,
    const char* file,
    const char* func,
    int line)
{
    unsigned int file_len;
    unsigned int func_len;
    unsigned int max;
    int ret;

    file_len = strlen(file);
    func_len = strlen(func);

    /* the maximum amount of bytes for location. +100 is for the rest and '/0' */
    max = file_len + func_len + 100;
    CHECK_BUFFER(log, max);
    ret = snprintf(log->index, max,
                   "[%.*s:%.*s():%d] ",
                   file_len, file, func_len, func, line
    );

    if (ret < 0 || ret >= (int)max) {
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
    if (log->level < level) {
        return;
    }
    RFS_PUSH();

    /* TODO: Maybe use different synchronization strategy here? */
    rf_mutex_lock(&log->lock);
    if (!format_log_message(log, level, file, func, line, msg)) {
        //TODO: how to handle this?
        RF_ASSERT(0, "Could not add a log message");
    }
    rf_mutex_unlock(&log->lock);
    RFS_POP();
    return;
}



void rf_log(enum RFlog_level level,
            const char* file,
            const char* func,
            int line, struct RFstring* msg)
{
    log_add(refu_clib_get_log(), level, file, func, line, msg);
}

