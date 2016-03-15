/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef REFU_SETUP_H
#define REFU_SETUP_H

#include <rflib/rf_options.h>
#include <rflib/utils/log.h>
#include <rflib/defs/retcodes.h>

#ifdef __cplusplus
extern "C" {
#endif

struct refu_clibctx;
struct RFlog;

#define RF_DEFAULT_TS_MBUFF_INITIAL_SIZE 1024
#define RF_DEFAULT_TS_SBUFF_INITIAL_SIZE 1024

/**
 * @defgroup RFgeneral_g_r_p Initialization function
 * @addtogroup RFgeneral_g_r_p
 * @{
 */

/**
 * @brief Initializes the ReFu library
 *
 * This function needs to be called in the very beginning of any program
 * using the library.
 *
 * @param log_type             The type of target for all logging functionality.
 *                             Choose between a file, stdout or stderr.
 *                             For more details: @ref RFlog_target_type
 * @param log_file_name        A c string literal with the filename of the log
 *                             to create.
 * @param level                The level of the logging system. Possible values
 *                             are enumerated by @ref RFlog_level
 * @param ts_mbuff_size        The initial buffer size in bytes that will be
 *                             given to the thread specific multi block buffer
 * @param ts_sbuff_size        The initial buffer size in bytes that will be
 *                             given to the thread specific single block buffer
 * @return                     Returns @c true in success
 */
i_DECLIMEX_ bool rf_init(enum RFlog_target_type log_type,
                         const char *log_file_name,
                         enum RFlog_level level,
                         size_t ts_mbuff_size,
                         size_t ts_sbuff_size);

/**
 * Deinitializes the library. Frees the constructs of all the modules
 */
i_DECLIMEX_ void rf_deinit();



/* Methods to get specific handlers of the library */
i_DECLIMEX_ struct refu_clibctx *refu_clib_get_ctx();

i_DECLIMEX_ struct RFlog *refu_clib_get_log();

//! @}
//closing RFgeneral_g_r_p doxygen group


#ifdef __cplusplus
}
#endif


#endif//include guards end
