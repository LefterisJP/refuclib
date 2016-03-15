/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/refu.h>

#include <rflib/utils/log.h>
#include <rflib/system/rf_system.h>
#include <rflib/persistent/buffers.h>

#include "string/rf_str_mod.ph"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void rf_termination_handler(int sig);

struct refu_clibctx {
    struct RFlog *log;
};
static struct refu_clibctx i_refu_clibctx;

static bool refu_clibctx_init(struct refu_clibctx *ctx,
                              enum RFlog_target_type type,
                              const char *log_file_name,
                              enum RFlog_level level)
{
    ctx->log = rf_log_create(type, log_file_name, level);
    if (!ctx->log) {
        return false;
    }
    return true;
}

static void refu_clibctx_deinit(struct refu_clibctx *ctx)
{
    rf_log_destroy(ctx->log);
    ctx->log = NULL;
}


static void rf_atexit()
{
    rf_log_flush(i_refu_clibctx.log);
}

//Initializes the Refu library
bool rf_init(enum RFlog_target_type log_type,
             const char *log_file_name,
             enum RFlog_level level,
             size_t ts_mbuff_size,
             size_t ts_sbuff_size)
{
    bool ret = false;
    /* create the refuclib ctx */
    if (!refu_clibctx_init(&i_refu_clibctx, log_type, log_file_name, level)) {
        return false;
    }

    /* activate all modules */
    if (!rf_string_activate()) {
        RF_ERROR("Failed to initialize the string module");
        goto cleanup;
    }
    if (!rf_persistent_buffers_activate(ts_mbuff_size, ts_sbuff_size)) {
        RF_ERROR("Failed to initialize the persisten data");
        goto cleanup;
    }
    if (!rf_system_activate()) {
        RF_ERROR("Failed to initialize the system module");
        goto cleanup;
    }

    /* initialize random seed: */
    srand(time(NULL));

    /* setup signal handlers */
    signal(SIGSEGV, rf_termination_handler);
    signal(SIGTERM, rf_termination_handler);
    signal(SIGABRT, rf_termination_handler);


    /* register a function to exeute at exit() */
    if (atexit(rf_atexit) != 0) {
        RF_ERROR("Failed to register the refu library exit function");
        goto cleanup;
    }

    ret = true;
cleanup:
    return ret;
}

void rf_deinit()
{
    /* before deinitializing anything make sure log is flushed */
    rf_log_flush(i_refu_clibctx.log);

    /* stop all modules */
    rf_system_deactivate();
    rf_persistent_buffers_deactivate();
    rf_string_deactivate();

    /* destroy the refuclib context */
    refu_clibctx_deinit(&i_refu_clibctx);
}

/* Methods to get specific handlers of the library */
struct refu_clibctx *refu_clib_get_ctx()
{
    return &i_refu_clibctx;
}

struct RFlog *refu_clib_get_log()
{
    return i_refu_clibctx.log;
}

volatile sig_atomic_t fatal_error_in_progress = false;
void rf_termination_handler(int sig)
{
    /*
     * Since this handler is established for more than one kind of signal,
     * it might still get invoked recursively by delivery of some other kind
     * of signal.  Use a static variable to keep track of that.
     */
    if (fatal_error_in_progress) {
        raise(sig);
    }
    fatal_error_in_progress = true;

    // actual signal cleanup action
    rf_deinit();

    // reraise signal's default handler to terminate process properly
    signal(sig, SIG_DFL);
    raise(sig);
}
