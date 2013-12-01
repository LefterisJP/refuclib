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
**/
//*---------------------Corresponding Header inclusion---------------------------------
#include <rf_options.h>//for the module check
#include <Definitions/imex.h> //import export macro
#include <Definitions/types.h> //for the fixed size data types
#include <Time/timer_decl.h> //for RFtimer
#include <Time/timer.h>
//*---------------------Module related inclusion----------------------------------------
#include <Time/sleep.h> //for the refu sleep functions
//*---------------------Outside module inclusion----------------------------------------
#include "../System/info.ph" //to check if we have a high-res timer
//for error logging
    #include <Utils/log.h>
//for memory allocation
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------System specific inclusion---------------------------------------
#include <windows.h> //for all the Winapi time structures and functions
//*----------------------------End of Includes------------------------------------------


//*---------------------Sleep Functions----------------------------

// Suspends the calling thread for a number of seconds
void rfSleep(uint32_t seconds)
{
    Sleep(seconds*1000);
}
//Suspends the calling thread for a number of milliseconds
void rf_sleep_ms(uint32_t milliseconds)
{
    Sleep(milliseconds);
}

/*-------------------------------------------------------Timer functions-----------------------------------------------------------------------------------------*/
#ifdef RF_MODULE_TIME_TIMER
//initializes a timer
char rf_timer_init(RFtimer* t,char resolution)
{
    //check if we can actually have such a timer
    if(rfSysInfo.hasHighResTimer==false)
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
            t->div = (double)rfSysInfo.pcFrequency;
        break;
        case RF_TIMER_MILLISECONDS:
            t->div = (double)rfSysInfo.pcFrequency/1E3;
        break;
        case RF_TIMER_MICROSECONDS:
            t->div = (double)rfSysInfo.pcFrequency/1E6;
        break;
        case RF_TIMER_NANOSECONDS:
            t->div = (double)rfSysInfo.pcFrequency/1E9;
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
RFtimer* rf_timer_create(char resolution)
{
    RFtimer* ret;
    //check if we can actually have such a timer
    if(rfSysInfo.hasHighResTimer==false)
    {
        LOG_ERROR("Creating a timer failed due to the system not supporting high resolution performance counter. Windows ONLY",RE_TIMER_HIGHRES_UNSUPPORTED);
        return 0;
    }
    RF_MALLOC(ret,sizeof(RF_Timer))
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
            ret->div = (double)rfSysInfo.pcFrequency;
        break;
        case RF_TIMER_MILLISECONDS:
            ret->div = (double)rfSysInfo.pcFrequency/1E3;
        break;
        case RF_TIMER_MICROSECONDS:
            ret->div = (double)rfSysInfo.pcFrequency/1E6;
        break;
        case RF_TIMER_NANOSECONDS:
            ret->div = (double)rfSysInfo.pcFrequency/1E9;
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
void rf_timer_destroy(RFtimer* t)
{
    free(t);
}

// Queries a timer
double rf_timer_query(RFtimer* t,char resolution)
{
    int64_t query;
    //if a new resolution has been requested
    if(resolution != 0)
    {
        switch(resolution)
        {
            case RF_TIMER_SECONDS:
                t->div = (double)rfSysInfo.pcFrequency;
            break;
            case RF_TIMER_MILLISECONDS:
                t->div = (double)rfSysInfo.pcFrequency/1E3;
            break;
            case RF_TIMER_MICROSECONDS:
                t->div = (double)rfSysInfo.pcFrequency/1E6;
            break;
            case RF_TIMER_NANOSECONDS:
                t->div = (double)rfSysInfo.pcFrequency/1E9;
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

