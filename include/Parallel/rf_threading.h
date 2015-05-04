/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef REFU_THREADING_H
#define REFU_THREADING_H

#ifdef REFU_WIN32_VERSION
#include <Parallel/rf_threading_win32.h
#else
#include <Parallel/rf_threading_linux.h>
#endif

/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for import export macro
/*------------- End of includes -------------*/

/**
 ** Initializes all thread specific data of the library
 **/
i_DECLIMEX_ bool rf_init_thread_specific();

/**
 ** Deinitializes all thread specific data of the library
 **/
i_DECLIMEX_ void rf_deinit_thread_specific();

/**
 ** @brief Gives an id unique to the calling thread
 **/
int rf_thread_get_id();

#endif
