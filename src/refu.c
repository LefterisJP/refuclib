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
             size_t string_buffer_size)
{
    bool ret = false;
    /* create the refuclib ctx */
    if (!refu_clibctx_init(&i_refu_clibctx, log_type, log_file_name, level)) {
        return false;
    }

    /* activate all modules */
    if (!rf_string_activate(string_buffer_size)) {
        RF_ERROR("Failed to initialize the string module");
        goto cleanup;
    }
    if (!rf_persistent_buffers_activate()) {
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
