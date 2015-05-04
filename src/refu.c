/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <refu.h>
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h> // logging system
#include <System/rf_system.h> // for rf_system_activate()
#include "Persistent/buffers.h" // persistent buffers initialization
/*------------- Modules activation/deactivation -------------*/
#include "String/rf_str_mod.ph"
/*------------- libc includes -------------*/
#include <string.h> //for strcmp
#include <stdlib.h> //for exit() and at_exit()
#include <time.h>
/*------------- End of includes -------------*/

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
