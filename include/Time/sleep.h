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
**
** --Time/sleep.h
** This header declares functions that suspend the calling thread's operation for a given amount of time
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h>//for import export macro
#include <Definitions/types.h> //for fixed size types
---------------------For internal library include make sure to have----------------------------
*/

#ifndef RF_TIME_SLEEP_H
#define RF_TIME_SLEEP_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @defgroup RF_TimeGRP Initialization function
** @addtogroup RF_TimeGRP
** @{
**/

/**
 ** @brief Suspends the calling thread for a number of seconds
 **
 ** For a function that suspends the thread's operation for milliseconds look at @ref rfSleep_ms()
 ** Under linux <b>DO NOT</b> mix with @ref rfSleep_ms() or any of the timer functions.
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
i_DECLIMEX_ void rfSleep_ms(uint32_t milliseconds);

//! @} //timegroup ends

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
