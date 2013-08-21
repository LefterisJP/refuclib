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
#ifdef RF_MODULE_TIME_TIMER//module check
    #include <time.h> //for clockid_t used in RF_Timer
    #include <Time/timer_decl.h> //for RF_Timer
    #include <Time/timer.h>
#endif
#ifdef RF_MODULE_TIME_DATE
    #include <Time/date_decl.h> //for RF_Date
    #include <Time/date.h>
#endif
/*------------- Module related inclusion -------------*/
#include <Time/sleep.h> //for the refu sleep functions
#include "common.ph"//private time macros
/*------------- Outside Module inclusion -------------*/
#include "../System/info.ph" //to check if we have a high-res timer
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Threads/common.h> //for rfThread_GetID()
    #include <Utils/error.h>
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
void rfSleep_ms(uint32_t milliseconds)
{
    usleep(milliseconds*1000);
}

/*--- Timer functions ---*/
#ifdef RF_MODULE_TIME_TIMER
//initializes a timer
char rfTimer_Init(RF_Timer* t,char resolution)
{
    //check if we can have high res timer
    if(rfSysInfo.hasHighResTimer == false)
    {
        RF_ERROR(0,
                 "Error initializing a timer, the system does not support a high "
                 "resolution timer");
        return false;
    }
    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &t->lastQuery) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
        RF_ERROR(0,
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
            RF_ERROR(0,
                     "Attempted to initialize a timer with an illegal "
                     "resolution");
            return false;
        break;
    }
    //success
    return true;
}

// Allocates and returns a timer object
RF_Timer* rfTimer_Create(char resolution)
{
    RF_Timer* ret;
    RF_MALLOC(ret, sizeof(RF_Timer), NULL);
    if(rfTimer_Init(ret, resolution) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

// Destroys a timer
void rfTimer_Destroy(RF_Timer* t)
{
    free(t);
}

// Queries a timer
double rfTimer_Query(RF_Timer* t,char resolution)
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
                RF_ERROR(0,
                         "Attempted to query a timer with an illegal "
                         "resolution. The query will be performed with the "
                         "previously saved resolution");
            break;
        }
    }

    //query the timer
    if( clock_gettime( rfSysInfo.timerType, &query) == -1 ) /// Do We really need this if check here? In refu_init we do check that the timer type works
    {
        RF_ERROR(0,
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



#ifdef RF_MODULE_TIME_DATE
/*--- RF_Dates Functions ---*/

// Initializes a Date
char rfDate_Init_Now(RF_Date* d,char local)
{
    struct tm* date;
    //get the time
    time_t t = time(NULL);
    if(local == true)
    {
        date = localtime(&t);
    }
    else
    {
        date = gmtime(&t);
    }
    //error check
    if(date == NULL)
    {
        RF_ERROR(0,
            "There was an error in initializing a date Object in Linux. "
            "The system function returned NULL", RE_DATE_SYSTEM_ERROR);
        return false;
    }
    //read in the values
    if(date->tm_sec == 60)
    {
        d->seconds = 59;
    }
    else
    {
        d->seconds = date->tm_sec;
    }
    d->minutes = date->tm_min;
    d->hours = date->tm_hour;
    d->wDay = date->tm_wday; //is the same as the #defines
    d->mDay = date->tm_mday;//because linux has it 1-31
    d->yDay = date->tm_yday;
    d->month = date->tm_mon+1;//because linux has it from 0 to 11
    d->year = 1900+date->tm_year;//since in Linux it's the number of year since 1900
    //check for leap year
    LEAP_YEAR_CHECK(d->year, d->isLeap)

    //success
    return true;
}

//Creates a date
RF_Date* rfDate_Create_Now(char local)
{
    RF_Date* ret;
    RF_MALLOC(ret, sizeof(RF_Date), NULL);
    if(rfDate_Init_Now(ret, local) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

// Sets this date as the current system date and time
char rfDate_SetToSystem(RF_Date* d)
{
    struct tm tm;
    tm.tm_sec = d->seconds;
    tm.tm_min = d->minutes;
    tm.tm_hour = d->hours;
    tm.tm_wday = d->wDay;
    tm.tm_mday = d->mDay;
    tm.tm_yday = d->yDay;
    tm.tm_mon = d->month-1;//because linux months are 0 to 11
    if(d->year >= 1900)
    {
        tm.tm_year = d->year-1900;
    }
    else
    {
        tm.tm_year = 1900;
    }
    tm.tm_isdst = 0; //temporary .. should also add it to my dates

    //create time_t object for system (UTC) time
    time_t t = mktime(&tm);
    if(t == -1)
    {
        RF_ERROR(0,
                 "Setting the system time from an RF_Date in Linux failed "
                 "because mktime() returned -1");
        return false;
    }
    //now set the system time
    if(stime(&t) != 0)
    {
        switch(errno)
        {
            case EFAULT:
                RF_ERROR(0,
                    "Setting the system time from an RF_Date in Linux "
                    "failed in stime(). Error in getting information from"
                    " user space.", RE_DATE_SET_SYSTEMTIME);
            break;
            case EPERM:
                RF_ERROR(0,
                         "Setting the system time from an RF_Date in Linux "
                         "failed in stime().The calling process has "
                         "insufficient privilege. Under Linux the CAP_SYS_TIME"
                         " privilege is required.");
            break;
            default:
                RF_ERROR(0,
                         "Setting the system time from an RF_Date in Linux "
                         "failed in stime(). errno has unexpected value of %d",
                         errno);
            break;
        }
        return false;
    }
    //success
    return true;
}

// Sets this date as the current local date and time
char rfDate_SetToLocal(RF_Date* d)
{
    struct tm tm;
    char *tz;
    tm.tm_sec = d->seconds;
    tm.tm_min = d->minutes;
    tm.tm_hour = d->hours;
    tm.tm_wday = d->wDay;
    tm.tm_mday = d->mDay;
    tm.tm_yday = d->yDay;
    tm.tm_mon = d->month-1;//because linux months are 0 to 11
    if(d->year >= 1900)
    {
        tm.tm_year = d->year-1900;
    }
    else
    {
        tm.tm_year = 1900;
    }
    tm.tm_isdst = 0; //temporary .. should also add it to my dates

    //need to use the tz environment variable to get the time zone, so that we can create a local time with mktime
    tz = getenv("TZ");
    setenv("TZ", "", 1);
    tzset();
    //create time_t object for Local Time
    time_t t = mktime(&tm);
    if(t == -1)
    {
        RF_ERROR(0,
                 "Setting the Local System time from an RF_Date in Linux "
                 "failed because mktime() returned -1");
        return false;
    }
    if (tz)
    {
        setenv("TZ", tz, 1);
    }
    else
    {
        unsetenv("TZ");
    }
    tzset();
    //now set the local time
    if(stime(&t) != 0)
    {
        switch(errno)
        {
            case EFAULT:
                RF_ERROR(0,
                         "Setting the Local System time from an RF_Date in "
                         "Linux failed in stime(). Error in getting "
                         "information from user space.");
            break;
            case EPERM:
                RF_ERROR(0,
                         "Setting the Local System time from an RF_Date in "
                         "Linux failed in stime().The calling process has "
                         "insufficient privilege. Under Linux the CAP_SYS_TIME"
                         " privilege is required.");
            break;
            default:
                RF_ERROR(0,
                    "Setting the Local System time from an RF_Date in "
                    "Linux failed in stime(). errno has unexpected value "
                    "of %d", errno);
            break;
        }
        return false;
    }
    //success
    return true;
}
#endif
