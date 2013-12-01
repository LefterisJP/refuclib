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
*/
/*------------- Corrensponding Header inclusion -------------*/
#include <rf_options.h>//for the module check
#include <Definitions/imex.h> //import export macro
#include <Definitions/types.h> //for the fixed size data types
#include <time.h> //for clockid_t used in RFtimer
#include <Time/timer_decl.h> //for RFtimer
#include <Time/timer.h>
/*------------- Module related inclusion -------------*/
#include <Time/sleep.h> //for the refu sleep functions
/*------------- Outside Module inclusion -------------*/
#include "../System/info.ph" //to check if we have a high-res timer
//for error logging
    #include <Utils/log.h>
//for memory allocation
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation
/*------------- System specific -------------*/
#include <unistd.h> //for linux sleep() functions
#include <errno.h>//for linux errno
#include <stdlib.h> //for setenv()
/*------------- End of includes -------------*/


/*--- Sleep functions ---*/

// Suspends the calling thread for a number of seconds
void rfSleep(uint32_t seconds)
{
    sleep(seconds);
}
//Suspends the calling thread for a number of milliseconds
void rf_sleep_ms(uint32_t milliseconds)
{
    usleep(milliseconds*1000);
}

/*--- Timer functions ---*/
#ifdef RF_MODULE_TIME_TIMER
//initializes a timer
char rf_timer_init(RFtimer* t,char resolution)
{
    //check if we can have high res timer
    if(rfSysInfo.hasHighResTimer == false)
    {
        RF_ERROR(
                 "Error initializing a timer, the system does not support a high "
                 "resolution timer");
        return false;
    }
    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &t->lastQuery) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
        RF_ERROR(
                 "Error initializing a timer using POSIX, clock_gettime failed");
      return false;
    }
    //keep the resolution
    switch(resolution)
    {
        case RF_TIMER_SECONDS:
        case RF_TIMER_MILLISECONDS:
        case RF_TIMER_MICROSECONDS:
        case RF_TIMER_NANOSECONDS:
            t->resolution = resolution;
        break;
        default:
            RF_ERROR(
                     "Attempted to initialize a timer with an illegal "
                     "resolution");
            return false;
        break;
    }
    //success
    return true;
}

// Allocates and returns a timer object
RFtimer* rf_timer_create(char resolution)
{
    RFtimer* ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    if(rf_timer_init(ret, resolution) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

// Destroys a timer
void rf_timer_destroy(RFtimer* t)
{
    free(t);
}

// Queries a timer
double rf_timer_query(RFtimer* t,char resolution)
{
    double result=0;
    struct timespec query;

    //if a change in resolution has been requested
    if(resolution != 0)
    {
        switch(resolution)
        {
            case RF_TIMER_SECONDS:
            case RF_TIMER_MILLISECONDS:
            case RF_TIMER_MICROSECONDS:
            case RF_TIMER_NANOSECONDS:
                t->resolution = resolution;
            break;
            default:
                RF_ERROR(
                         "Attempted to query a timer with an illegal "
                         "resolution. The query will be performed with the "
                         "previously saved resolution");
            break;
        }
    }

    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &query) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
        RF_ERROR(
                 "Error querying a timer using POSIX, clock_gettime failed");
        return 0;
    }

    struct timespec diff;
	if ((query.tv_nsec-t->lastQuery.tv_nsec)<0)
	{
		diff.tv_sec = query.tv_sec-t->lastQuery.tv_sec-1;
		diff.tv_nsec = 1000000000+query.tv_nsec-t->lastQuery.tv_nsec;
	}
	else
	{
		diff.tv_sec = query.tv_sec-t->lastQuery.tv_sec;
		diff.tv_nsec = query.tv_nsec-t->lastQuery.tv_nsec;
	}

	switch(t->resolution)
	{
	    case RF_TIMER_SECONDS:
            result = (double)diff.tv_sec + (double)diff.tv_nsec/1E9;
	    break;
        case RF_TIMER_MILLISECONDS:
            result = (double)diff.tv_sec*1E3 + (double)diff.tv_nsec/1E6;
	    break;
	    case RF_TIMER_MICROSECONDS:
            result = (double)diff.tv_sec*1E6 + (double)diff.tv_nsec/1E3;
	    break;
	    case RF_TIMER_NANOSECONDS:
            result = (double)diff.tv_sec*1E9 + (double)diff.tv_nsec;
	    break;
	}

    t->lastQuery = query;
    return result;

}

#endif

