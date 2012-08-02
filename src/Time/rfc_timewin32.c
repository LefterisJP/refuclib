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

#include <windows.h>

#include <rf_options.h>
#ifdef RF_MODULE_TIME_TIMER//module check
#include <Time/rfc_timer.h>
#endif
#ifdef RF_MODULE_TIME_DATE
#include <Time/rfc_date.h>
#endif
#include <rf_utils.h>
#include "time_private.h"
#include <rf_time.h> //for the refu sleep functions

/*-------------------------------------------------------Sleep functions-----------------------------------------------------------------------------------------*/

// Suspends the calling thread for a number of seconds
void rfSleep(uint32_t seconds)
{
    Sleep(seconds*1000);
}
//Suspends the calling thread for a number of milliseconds
void rfSleep_ms(uint32_t milliseconds)
{
    Sleep(milliseconds);
}

/*-------------------------------------------------------Timer functions-----------------------------------------------------------------------------------------*/
#ifdef RF_MODULE_TIME_TIMER
//initializes a timer
char rfTimer_Init(RF_Timer* t,char resolution)
{
    //check if we can actually have such a timer
    if(rfUTILS.hasHighResTimer==false)
    {
        LOG_ERROR("Initialization of a timer failed due to the system not supporting high resolution performance counter. Windows ONLY",RE_TIMER_HIGHRES_UNSUPPORTED);
        return false;
    }
    //query the timer
    if(QueryPerformanceCounter((LARGE_INTEGER*)&t->lastQuery) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("Initialization of a timer failed due to QueryPerformanceCounter failing with Windows Error(%lu):\n%s",RE_TIMER_QPC,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return false;
    }
    //depending on the desired resolution
    switch(resolution)
    {
        case RF_TIMER_SECONDS:
            t->div = (double)rfUTILS.pcFrequency;
        break;
        case RF_TIMER_MILLISECONDS:
            t->div = (double)rfUTILS.pcFrequency/1E3;
        break;
        case RF_TIMER_MICROSECONDS:
            t->div = (double)rfUTILS.pcFrequency/1E6;
        break;
        case RF_TIMER_NANOSECONDS:
            t->div = (double)rfUTILS.pcFrequency/1E9;
        break;

        //illegal flag
        default:
            LOG_ERROR("Attempted to initialize a timer with an illegal resolution",RE_TIMER_ILLEGAL_RESOLUTION);
            return false;
        break;
    }
    t->resolution = resolution;
    //success
    return true;
}
// Allocates and returns a timer object
RF_Timer* rfTimer_Create(char resolution)
{
    //check if we can actually have such a timer
    if(rfUTILS.hasHighResTimer==false)
    {
        LOG_ERROR("Creating a timer failed due to the system not supporting high resolution performance counter. Windows ONLY",RE_TIMER_HIGHRES_UNSUPPORTED);
        return 0;
    }
    RF_Timer* ret = malloc(sizeof(RF_Timer));
    //query the timer
    if(QueryPerformanceCounter((LARGE_INTEGER*)&ret->lastQuery) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("Creation of a timer failed due to QueryPerformanceCounter failing with Windows Error(%lu):\n%s",RE_TIMER_QPC,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return false;
    }
    //depending on the desired resolution
    switch(resolution)
    {
        case RF_TIMER_SECONDS:
            ret->div = (double)rfUTILS.pcFrequency;
        break;
        case RF_TIMER_MILLISECONDS:
            ret->div = (double)rfUTILS.pcFrequency/1E3;
        break;
        case RF_TIMER_MICROSECONDS:
            ret->div = (double)rfUTILS.pcFrequency/1E6;
        break;
        case RF_TIMER_NANOSECONDS:
            ret->div = (double)rfUTILS.pcFrequency/1E9;
        break;
        //illegal flag
        default:
            LOG_ERROR("Attempted to initialize a timer with an illegal resolution",RE_TIMER_ILLEGAL_RESOLUTION);
            return false;
        break;
    }
    ret->resolution = resolution;
    //success
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
    int64_t query;
    //if a new resolution has been requested
    if(resolution != 0)
    {
        switch(resolution)
        {
            case RF_TIMER_SECONDS:
                t->div = (double)rfUTILS.pcFrequency;
            break;
            case RF_TIMER_MILLISECONDS:
                t->div = (double)rfUTILS.pcFrequency/1E3;
            break;
            case RF_TIMER_MICROSECONDS:
                t->div = (double)rfUTILS.pcFrequency/1E6;
            break;
            case RF_TIMER_NANOSECONDS:
                t->div = (double)rfUTILS.pcFrequency/1E9;
            break;
            //illegal flag
            default:
                LOG_ERROR("Attempted to query a timer with an illegal resolution",RE_TIMER_ILLEGAL_RESOLUTION);
                return false;
            break;
        }
        //save the new resolution
        t->resolution = resolution;
    }

    //query the timer
    if(QueryPerformanceCounter((LARGE_INTEGER*)&query) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Timer query failed due to QueryPerformanceCounter producing Windows Error(%lu):\n%s",RE_TIMER_QPC,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return false;
    }
    //calculate the result
    double result = ((double)(query-t->lastQuery)/t->div);
    t->lastQuery = query;
    return result;
}
#endif


#ifdef RF_MODULE_TIME_DATE

/**==================================================================RF_Dates Functions=========================================================================================================*/

//include private time related macros that don't need to be visible to anyone else


//Reads a SYSTEMTIME structure and populates the RF_Date
#define SYSTEMTIME_TO_RF_DATE(i_st,i_date)\
    i_date->seconds = i_st.wSecond;\
    i_date->minutes = i_st.wMinute;\
    i_date->hours = i_st.wHour;\
    i_date->wDay = i_st.wDayOfWeek; /*is the same as the #defines*/\
    i_date->mDay = i_st.wDay;/*same because windows has it 1-31*/\
    i_date->month = i_st.wMonth;/*because windows range is 1-12*/\
    i_date->year = i_st.wYear;

//Reads an RF_DATE structure and populates a SYSTEMTIME Structure
#define RF_DATE_TO_SYSTEMTIME(i_date,i_st)\
    i_st.wSecond = i_date->seconds;\
    i_st.wMinute = i_date->minutes;\
    i_st.wHour = i_date->hours;\
    i_st.wDayOfWeek = i_date->wDay;\
    i_st.wDay = i_date->mDay;\
    i_st.wMonth = i_date->month;\
    i_st.wYear = i_date->year;\
    i_st.wMilliseconds = 0;/*RF_Date does not have milliseconds so just 0 it*/

// Initializes a Date
char rfDate_Init_Now(RF_Date* d,char local)
{
    //get the time
    SYSTEMTIME st;
    if(local == true)
        GetLocalTime(&st);
    else
        GetSystemTime(&st);
    //read in the values
    SYSTEMTIME_TO_RF_DATE(st,d)
    //check for leap year
    LEAP_YEAR_CHECK(d->year,d->isLeap)
    //calculate day of the year
    GET_DAY_OF_YEAR(d->month,d->mDay,d->isLeap,d->yDay)
    //success
    return true;
}

//Creates a date
RF_Date* rfDate_Create_Now(char local)
{
    RF_Date* ret = malloc(sizeof(RF_Date));
    //get the time
    SYSTEMTIME st;
    if(local == true)
        GetLocalTime(&st);
    else
        GetSystemTime(&st);
    //read in the values
    SYSTEMTIME_TO_RF_DATE(st,ret)
    //check for leap year
    LEAP_YEAR_CHECK(ret->year,ret->isLeap);
    //calculate day of the year
    GET_DAY_OF_YEAR(ret->month,ret->mDay,ret->isLeap,ret->yDay)
    //success
    return ret;
}

// Sets this date as the current system date and time
int32_t rfDate_SetToSystem(RF_Date* d)
{
    SYSTEMTIME st;
    RF_DATE_TO_SYSTEMTIME(d,st)

    if(SetSystemTime(&st) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Setting the system time from an RF_Date in Windows failed with Windows Error(%lu):\n%s",RE_DATE_SET_SYSTEMTIME,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RF_FAILURE;
    }
    return RF_SUCCESS;
}

// Sets this date as the current local date and time
int32_t rfDate_SetToLocal(RF_Date* d)
{
    SYSTEMTIME st;
    RF_DATE_TO_SYSTEMTIME(d,st)

    if(SetLocalTime(&st) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Setting the local time from an RF_Date in Windows failed with Windows Error(%lu):\n%s",RE_DATE_SET_SYSTEMTIME,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RF_FAILURE;
    }
    return RF_SUCCESS;
}
#endif
