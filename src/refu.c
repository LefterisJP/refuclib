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
#include <Utils/localmem.h> // local memory stack
#include <Utils/log.h> // logging system
#include <System/rf_system.h> // for rf_system_activate()
#include "Internal/rf_internal_mod.ph" // internal data initialization
/*------------- Modules activation/deactivation -------------*/
#include "String/rf_str_mod.ph"
/*------------- libc includes -------------*/
#include <string.h> //for strcmp
#include <stdlib.h> //for exit()
/*------------- End of includes -------------*/

//Initializes the Refu library
bool rf_init(char *logstr, uint64_t lmsSize, enum RFlog_level level)
{
    bool ret = false;
    /* activate all modules */
    if (!rf_log_activate(level, logstr)) {
        goto cleanup;
    }
    if (!rf_lms_activate(lmsSize, true)) {
        RF_ERROR("Failed to initialize the local memory stack");
        goto cleanup;
    }
    if (!rf_string_activate()) {
        RF_ERROR("Failed to initialize the string module");
        goto cleanup;
    }
    if (!rf_internal_activate()) {
        RF_ERROR("Failed to initialize the internal module");
        goto cleanup;
    }
    if (!rf_system_activate()) {
        RF_ERROR("Failed to initialize the system module");
        goto cleanup;
    }
    ret = true;

cleanup:
    return ret;
}

void rf_deinit()
{
    rf_system_deactivate();
    rf_internal_deactivate();
    rf_string_deactivate();
    rf_lms_deactivate(true);
    rf_log_deactivate();
}
