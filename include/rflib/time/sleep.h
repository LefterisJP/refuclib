/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_TIME_SLEEP_H
#define RF_TIME_SLEEP_H

#include <rflib/defs/types.h>
#include <rflib/defs/imex.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @defgroup RFtime_g_r_p Initialization function
** @addtogroup RFtime_g_r_p
** @{
**/

/**
 ** @brief Suspends the calling thread for a number of seconds
 **
 ** For a function that suspends the thread's operation for milliseconds look at @ref rf_sleep_ms()
 ** Under linux <b>DO NOT</b> mix with @ref rf_sleep_ms() or any of the timer functions.
 ** @param seconds The number of seconds to suspend the operation of the calling thread for
 **
 **/
i_DECLIMEX_ void rfSleep(uint32_t seconds);
/**
 ** @brief Suspends the calling thread for a number of milliseconds
 **
 ** For a function that suspends the thread's operation for seconds look at @ref rfSleep()
 ** Under linux <b>DO NOT</b> mix with @ref rfSleep() or any of the timer functions.
 ** @param milliseconds The number of milliseconds to suspend the operation of the calling thread for
 **
 **/
i_DECLIMEX_ void rf_sleep_ms(uint32_t milliseconds);

//! @} //timegroup ends

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
