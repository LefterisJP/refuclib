/**
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#ifndef RF_TIMER_H
#define RF_TIMER_H

#include <rf_options.h>
#ifdef RF_MODULE_TIME_TIMER //timer module check

#include <rf_setup.h>

#ifdef REFU_LINUX_VERSION
    #include <time.h>
#endif

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @internal
** @author Lefteris
** @date 17/05/2012
** @endinternal
** @brief A cross-platform high resolution Timer structure
**
** This is a cross-platform Timer structure that allows the user to implement a timer with different resolutions
** on any system that he might need to. This timer is supposed to be used for high resolution counting such as milliseconds
** microseconds, or nanoseconds.
**/
typedef struct RF_Timer
{
    //! The resolution of the timer. Will be the value given in the initialization
    char resolution;
#ifdef REFU_WIN32_VERSION
    //! The variable holding the start of the timer or the time of the last query
    int64_t lastQuery;
    //! The amount that the difference should be multiplied by to provide the desired result. (Seconds, milliseconds, microseconds e.t.c.)
    double div;
#elif defined(REFU_LINUX_VERSION)
    //! The struct holding the start ofthe timer of the time of the last query
    struct timespec lastQuery;
    //! Holds the type of clockid_t to be given to the posix timer clock functions
    clockid_t type;
#else
    #error Unsuported timer system
    //!@todo
#endif
}RF_Timer;

//! Denotes that the timer gives the results in seconds
#define RF_TIMER_SECONDS        1
//! Denotes that the timer gives the results in milliseconds
#define RF_TIMER_MILLISECONDS   2
//! Denotes that the timer gives the results in microseconds
#define RF_TIMER_MICROSECONDS   3
//! Denotes that the timer gives the results in nanoseconds
#define RF_TIMER_NANOSECONDS    4

//! @memberof RF_Timer
//! @brief Initializes a timer
//!
//! @param t The timer object to initialize
//! @param resolution The desired resolution of the timer. Legal values are:@c RF_TIMER_SECONDS, @c RF_TIMER_MILLISECONDS, @c RF_TIMER_MICROSEONDS and @c RF_TIMER_NANOSECONDS
//! @return Returns true for correct initialization or false while logging an error otherwise
i_DECLIMEX_ char rfTimer_Init(RF_Timer* t,char resolution);
//! @memberof RF_Timer
//! @brief Allocates and returns a timer object
//!
//! @param resolution The desired resolution of the timer. Legal values are: @c RF_TIMER_SECONDS, @c RF_TIMER_MILLISECONDS, @c RF_TIMER_MICROSEONDS and @c RF_TIMER_NANOSECONDS
//! @return Returns the timer object
i_DECLIMEX_ RF_Timer* rfTimer_Create(char resolution);

//! @memberof RF_Timer
//! @brief Destroys a timer
//!
//! Use it only for timers that were made with @ref rfTimer_Create. Timers made with init need no freeing
//! @param t The timer to destroy
i_DECLIMEX_ void rfTimer_Destroy(RF_Timer* t);

//! @memberof RF_Timer
//! @brief Queries a timer
//!
//! Using this function the timer can be queried. There is a choice between querying with the previously given resolution or with a new resolution. If no change in resolution is
//! requested then provide 0 (@c RF_NONE) for the resolution. Otherwise provide one of   @c RF_TIMER_SECONDS, @c RF_TIMER_MILLISECONDS, @c RF_TIMER_MICROSEONDS or @c RF_TIMER_NANOSECONDS.
//! If an illegal resolution value is given here then an error is logged and the query's result is given in the previously saved resolution. For an example query usage look at the following
//! snippet from the overall timer example.
//! @snippet Time/timer1.c  QUERY
//! @param t The timer object to query
//! @param resolution With this parameter you have the choice to either query the timer with the saved resolutiont in which case you should give @c RF_NONE, or change the resolution
//! and get the result of the query in the new resolution by giving  @c RF_TIMER_SECONDS, @c RF_TIMER_MILLISECONDS, @c RF_TIMER_MICROSEONDS or @c RF_TIMER_NANOSECONDS
//! @return The time difference between this query and the previous one. Or this query and the initialization in the case of this being the first
i_DECLIMEX_ double rfTimer_Query(RF_Timer* t,char resolution);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#else //end of the timer module include
    #error Attempted to include Refu Timer module with the module flag off. Rebuild the library with that option added if you need to include it
#endif

#endif//include guards end
