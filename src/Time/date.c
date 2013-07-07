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
#include <Definitions/imex.h>//for import export macro
#include <Definitions/types.h> //for fixed size types
#include <Time/date_decl.h> //for RF_Date
#include <Time/date.h>
//*---------------------Module related inclusion----------------------------------------
#include "common.ph"//private time macros
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <limits.h> //for the limit of ullong
//*----------------------------End of Includes------------------------------------------


//some definitions of accumulated days in the year after each month, used in various calculations
#define ACCUM_DAYS_JANUARY      31
#define ACCUM_DAYS_FEBRUARY     59 //+28
#define ACCUM_DAYS_MARCH        90 //+31
#define ACCUM_DAYS_APRIL        120 //+30
#define ACCUM_DAYS_MAY          151 //+31
#define ACCUM_DAYS_JUNE         181 //+30
#define ACCUM_DAYS_JULY         212 //+31
#define ACCUM_DAYS_AUGUST       243 //+31
#define ACCUM_DAYS_SEPTEMBER    273 //+30
#define ACCUM_DAYS_OCTOBER      304 //+31
#define ACCUM_DAYS_NOVEMBER     334 //+30
#define ACCUM_DAYS_DECEMBER     365 //+31


// Initializes a Date object with a specific date
char rfDate_Init(RF_Date* d,
                     unsigned char wDay,
                     unsigned char mDay,
                     unsigned char month,
                     uint16_t year,
                     unsigned char hour,
                     unsigned char minutes,
                     unsigned char sec)
{
    if(wDay > 6)
    {
        LOG_ERROR(
        "During initializing a Date Object an illegal value of %d was "
        "given the day of the week. Legal values are from 0 to 6",
            RE_DATE_ILLEGAL_PARAM, wDay);
        return false;
    }
    d->wDay = wDay;

    if(mDay < 1 || mDay > 31)
    {
        LOG_ERROR(
        "During initializing a Date Object an illegal value of %d was "
        "given for the day of the month. Legal values are from 1 to 31",
            RE_DATE_ILLEGAL_PARAM,mDay);
        return false;
    }
    d->mDay = mDay;

    if(month < 1 || month > 12 )
    {
        LOG_ERROR(
        "During initializing a Date Object an illegal value of %d was "
        "given for the month. Legal values are from 1 to 12",
        RE_DATE_ILLEGAL_PARAM,month);
        return false;
    }
    d->month = month;

    if(year < 1601 || year > 30827)
    {
        LOG_ERROR("During initializing a Date Object an illegal value of"
                  " %d was given for the year. Legal values are from 1601"
                  " to 30827 due to limitations of the Windows version of"
                  " the library", RE_DATE_ILLEGAL_PARAM, year);
        return false;
    }
    d->year = year;

    if(hour > 23)
    {
        LOG_ERROR(
            "During initializing a Date Object an illegal value of %d was"
            " given for the hour. Legal values are from 0 to 23",
            RE_DATE_ILLEGAL_PARAM,hour);
        return false;
    }
    d->hours = hour;

    if(minutes > 59)
    {
        LOG_ERROR(
            "During initializing a Date Object an illegal value of %d was"
            " given for the minutes. Legal values are from 0 to 59",
            RE_DATE_ILLEGAL_PARAM, minutes);
        return false;
    }
    d->minutes = minutes;

    if(sec > 59)
    {
        LOG_ERROR(
            "During initializing a Date Object an illegal value of %d was"
            " given for the seconds. Legal values are from 0 to 59",
            RE_DATE_ILLEGAL_PARAM, sec);
        return false;
    }
    d->seconds = sec;
    //also get leap year or not and number of days in the year
    LEAP_YEAR_CHECK(d->year,d->isLeap);
    GET_DAY_OF_YEAR(d->month,d->mDay,d->isLeap,d->yDay)
    //success
    return true;
}

// Create a Date object with a specific date
RF_Date* rfDate_Create(unsigned char wDay,
                       unsigned char mDay,
                       unsigned char month,
                       uint16_t year,
                       unsigned char hour,
                       unsigned char minutes,
                       unsigned char sec)
{
    RF_Date* ret;
    RF_MALLOC(ret, sizeof(RF_Date), NULL);
    if(!rfDate_Init(ret, wDay, mDay, month, year, hour, minutes, sec))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

//Destroys a date
void rfDate_Destroy(RF_Date* c)
{
    free(c);
}


// Adds a number of years to the Date
void rfDate_AddYears(RF_Date* d, uint32_t years)
{
    d->year += years;
    //also perform the leap year check
    LEAP_YEAR_CHECK(d->year,d->isLeap)
}

// Removes a number of years from the Date
void rfDate_SubYears(RF_Date* d, uint32_t years)
{
    d->year -= years;
    LEAP_YEAR_CHECK(d->year,d->isLeap);
}

// Adds a number of months to the Date
void rfDate_AddMonths(RF_Date* d, uint32_t g_months)
{
    int32_t months = g_months;
    //if the addition would take us over a year
    while(d->month + months > 12)
    {
        //keep adding years and shrinking the remaining months until we get to an amount of remaining months that would not makes us cross over a year
        months -= 13 - d->month;
        rfDate_AddYears(d,1);
        d->month = 1;//got to January since we just changed year
    }
    //by now the number of remaining months to add should not make us cross over the year threshold so
    d->month += months;
}
// Removes a number of months frp, the Date
void rfDate_SubMonths(RF_Date* d, uint32_t g_months)
{
    int32_t months = g_months;
    //if the subtraction would take us more than a year in the past
    while( (int32_t)d->month - months <= 0)
    {
        //keep removing years and shrinking the remaining months until we get to an amount of remaining months that would not makes us cross over a year
        months -=  d->month;
        rfDate_SubYears(d,1);
        d->month = 12; //go to december since we just changed a year
    }
    //by now the number of remaining months to remove should not make us cross over the year threshold so
    d->month -= months;
}

//Adds a number of days
void rfDate_AddDays(RF_Date* d, uint32_t g_days)
{
    int32_t days = g_days;
    int32_t thisYearDays = 365;
    if(d->isLeap == true)
    {
        thisYearDays = 366;
    }

    //if the addition would take us over a year
    while(d->yDay + days > thisYearDays)
    {
        //keep adding years and shrinking the remaining days until we get to an amount of remaining days that would not makes us cross over a year
        if(d->isLeap==false)
        {
            days -= (365-d->yDay)+1;
        }
        else
        {
            days -= (366-d->yDay)+1;
        }

        rfDate_AddYears(d,1);
        //now calculate the year days
        thisYearDays = 365;
        if(d->isLeap == true)
        {
            thisYearDays = 366;
        }

        //after adding a year we start at the next year's 1st day
        d->yDay = 1;
        d->mDay = 1;
        d->month = 1;
    }

    //by now the number of remaining days should not affect the year outcome but still calculations are needed for months
    while(days >= 0)
    {
        unsigned char remMonthDays;
        //depending on the month calculate the remaining days. Remember the knuckles rule mnemonic? :D
        switch(d->month)
        {
            case RF_JANUARY:
                remMonthDays = 31-d->mDay;
            break;
            case RF_FEBRUARY:
                if(d->isLeap==true)
                    remMonthDays = 29-d->mDay;
                else
                    remMonthDays = 28-d->mDay;
            break;
            case RF_MARCH:
                remMonthDays = 31-d->mDay;
            break;
            case RF_APRIL:
                remMonthDays = 30-d->mDay;
            break;
            case RF_MAY:
                remMonthDays = 31-d->mDay;
            break;
            case RF_JUNE:
                remMonthDays = 30-d->mDay;
            break;
            case RF_JULY:
                remMonthDays = 31-d->mDay;
            break;
            case RF_AUGUST:
                remMonthDays = 31-d->mDay;
            break;
            case RF_SEPTEMBER:
                remMonthDays = 30-d->mDay;
            break;
            case RF_OCTOBER:
                remMonthDays = 31-d->mDay;
            break;
            case RF_NOVEMBER:
                remMonthDays = 30-d->mDay;
            break;
            case RF_DECEMBER:
                remMonthDays = 31-d->mDay;
            break;
            default:
                LOG_ERROR(
                    "Unrecognized month value encountered. Should never "
                    "come here. The date object must be corrupt",
                    RE_DATE_ILLEGAL_MONTH);
                remMonthDays = 0;
            break;
        }//end of month switch
        remMonthDays +=1; //since days start from 1
        //now if this will take us over the month remove the remaining month days from the remaining days and change month, else just add the days
        if(days > remMonthDays)
        {
            days -= remMonthDays;
            d->month +=1;
            d->mDay = 1;
        }
        else
        {
            d->mDay+=days;
            break;
        }
    }//end of iterating through the remaining days

    //now calculate the day of the year depending on the month we ended up in
    switch(d->month)
    {
        case RF_JANUARY:
            d->yDay = d->mDay;
        break;
        case RF_FEBRUARY:
            d->yDay = d->mDay+ ACCUM_DAYS_JANUARY;
        break;
        case RF_MARCH:
            d->yDay = d->mDay+ ACCUM_DAYS_FEBRUARY;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_APRIL:
            d->yDay = d->mDay+ ACCUM_DAYS_MARCH;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_MAY:
            d->yDay = d->mDay+ ACCUM_DAYS_APRIL;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_JUNE:
            d->yDay = d->mDay + ACCUM_DAYS_MAY;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_JULY:
            d->yDay = d->mDay+ ACCUM_DAYS_JUNE;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_AUGUST:
            d->yDay = d->mDay + ACCUM_DAYS_JULY;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_SEPTEMBER:
            d->yDay = d->mDay+ ACCUM_DAYS_AUGUST;
            if(d->isLeap == true)
                d->yDay +=1;
        break;
        case RF_OCTOBER:
            d->yDay = d->mDay+ ACCUM_DAYS_SEPTEMBER;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_NOVEMBER:
            d->yDay = d->mDay+ ACCUM_DAYS_OCTOBER;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_DECEMBER:
            d->yDay = d->mDay+ ACCUM_DAYS_NOVEMBER;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
    }

    //finally calculate what day of the week it should be
    d->wDay = d->wDay + (g_days%7);
    if(d->wDay > 6)
    {
        d->wDay -=7;
    }
}

//Removes a number of days
void rfDate_SubDays(RF_Date* d, uint32_t g_days)
{
    int32_t days = g_days;

    //if the subtraction would take us over a year in the past
    while( (int32_t)d->yDay - days <= 0)
    {
        //keep removing years and shrinking the remaining days until we get to an amount of remaining days that would not makes us cross over a year
        days -= d->yDay;
        rfDate_SubYears(d,1);
        //after removing a year we start at the previous year's last day
        if(d->isLeap == true)
        {
            d->yDay = 366;
        }
        else
        {
            d->yDay = 365;
        }
        d->mDay = 31;
        d->month = 12;
    }


    //by now the number of remaining days should not affect the year outcome but still calculations are needed for months
    while(days >= 0)
    {
        //now if this will take us over the month remove the remaining month days from the remaining days and change month, else just remove the days from the current month
        if(days >= d->mDay)
        {
            days -= d->mDay;
            d->month -=1;
            //depending on the month calculate the value of the d->mDay which is actually the last day of said month
            switch(d->month)
            {
                case RF_JANUARY:
                case RF_MARCH:
                case RF_MAY:
                case RF_JULY:
                case RF_AUGUST:
                case RF_OCTOBER:
                case RF_DECEMBER:
                    d->mDay = 31;
                break;
                case RF_FEBRUARY:
                    if(d->isLeap==true)
                    {
                        d->mDay = 29;
                    }
                    else
                    {
                        d->mDay = 28;
                    }
                break;
                default://rest of the months are 30
                    d->mDay = 30;
                break;
            }//end of month switch
        }//end of the case of going over a month in the past
        else
        {
            d->mDay-=days;
            break;
        }
    }//end of iterating through the remaining days

    //now calculate the day of the year depending on the month we ended up in
    switch(d->month)
    {
        case RF_JANUARY:
            d->yDay = d->mDay;
        break;
        case RF_FEBRUARY:
            d->yDay = d->mDay+ ACCUM_DAYS_JANUARY;
        break;
        case RF_MARCH:
            d->yDay = d->mDay+ ACCUM_DAYS_FEBRUARY;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_APRIL:
            d->yDay = d->mDay+ ACCUM_DAYS_MARCH;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_MAY:
            d->yDay = d->mDay+ ACCUM_DAYS_APRIL;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_JUNE:
            d->yDay = d->mDay + ACCUM_DAYS_MAY;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_JULY:
            d->yDay = d->mDay+ ACCUM_DAYS_JUNE;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_AUGUST:
            d->yDay = d->mDay + ACCUM_DAYS_JULY;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_SEPTEMBER:
            d->yDay = d->mDay+ ACCUM_DAYS_AUGUST;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_OCTOBER:
            d->yDay = d->mDay+ ACCUM_DAYS_SEPTEMBER;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_NOVEMBER:
            d->yDay = d->mDay+ ACCUM_DAYS_OCTOBER;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
        case RF_DECEMBER:
            d->yDay = d->mDay+ ACCUM_DAYS_NOVEMBER;
            if(d->isLeap == true)
            {
                d->yDay +=1;
            }
        break;
    }

    //finally calculate what day of the week it should be
    char weekDay = (char)d->wDay - (g_days%7);
    if(weekDay < 0)
    {
        weekDay = 7-weekDay;
    }
    d->wDay = weekDay;
}

// Adds a number of hours to the Date
void rfDate_AddHours(RF_Date* d, uint32_t hours)
{
    unsigned char remDayHours = 24-d->hours;
    //if the given hours take us over a day
    if(hours > remDayHours)
    {
        //shrink the hours by the day's remaining hours and add a day
        hours -= remDayHours;
        d->hours = 0; //since we just changed the day

        //find out how many days these hours would be and add them
        rfDate_AddDays(d,(hours/24)+1); //+1 is for the extra day we added due to day change
        //now find the remaining hours to add
        hours = hours%24;
    }
    //finally add the remaining hours
    d->hours += hours;
}

// Removes a number of hours from the Date
void rfDate_SubHours(RF_Date* d, uint32_t hours)
{
    //if the given hours will take us to yesterday and beyond
    if(hours > d->hours)
    {
        //shrink the hours by the day's remaining hours and remove a day
        hours -= d->hours+1; //+1 is since hours count from 0
        d->hours = 23; //since we just changed the day

        //find out how many days these hours would be and remove them
        rfDate_SubDays(d,(hours/24)+1);//+1 is for the extra day we remove due to day change
        //now find the remaining hours to remove
        hours = hours%24;
    }
    //finally add the remaining hours
    d->hours -= hours;
}

// Adds a number of minutes to the Date
void rfDate_AddMinutes(RF_Date* d, uint32_t minutes)
{
    unsigned char remHoursMins = 60 - d->minutes;
    //if the given minutes take us over the hour
    if(minutes > remHoursMins)
    {
        //shrink the minutes by the hour's remaining minutes and add an hour
        minutes -= remHoursMins;
        d->minutes = 0; //since we just changed the hour

        //find out how many hours these minutes would be and add them
        rfDate_AddHours(d,(minutes/60)+1); //+1 is for the extra hour we added due to hour change
        //now find the remaining minutes to add
        minutes = minutes%60;
    }
    //finally add the remaining minutes
    d->minutes += minutes;
}
// Removes a number of minutes from the Date
void rfDate_SubMinutes(RF_Date* d, uint32_t minutes)
{
    //if the given minutes will take us to the previous hour and beyond
    if(minutes > d->minutes)
    {
        //shrink the minutes by the hour's remaining minutes and remove an hour
        minutes -= d->minutes+1; //+1 is since minutes count from 0
        d->minutes = 59; //since we just changed the hour

        //find out how many hours these minutes would be and remove them
        rfDate_SubHours(d,(minutes/60)+1);//+1 is for the extra hour we remove due to hour change
        //now find the remaining minutes to remove
        minutes = minutes%60;
    }
    //finally remove the remaining minutes
    d->minutes -= minutes;
}


// Adds a number of seconds to the Date
void rfDate_AddSeconds(RF_Date* d, uint32_t seconds)
{
    unsigned char remMinSeconds = 60 - d->seconds;
    //if the given seconds take us over the minute
    if(seconds > remMinSeconds)
    {
        //shrink the seconds by the minute's remaining seconds and add a minute
        seconds -= remMinSeconds;
        d->seconds = 0; //since we just changed the minute

        //find out how many minutes these seconds would be and add them
        rfDate_AddMinutes(d, (seconds/60)+1); //+1 is for the extra minutes we added due to minute change
        //now find the remaining seconds to add
        seconds = seconds%60;
    }
    //finally add the remaining seconds
    d->seconds += seconds;
}
// Removes a number of seconds from the Date
void rfDate_SubSeconds(RF_Date* d, uint32_t seconds)
{
    //if the given seconds will take us to the previous minute and beyond
    if(seconds > d->seconds)
    {
        //shrink the seconds by the minute's remaining seconds and remove a minute
        seconds -= d->seconds+1; //+1 is since seconds count from 0
        d->seconds = 59; //since we just changed the minute

        //find out how many minutes these seconds would be and remove them
        rfDate_SubMinutes(d,(seconds/60)+1);//+1 is for the extra minute we remove due to minute change
        //now find the remaining seconds to remove
        seconds = seconds%60;
    }
    //finally remove the remaining seconds
    d->seconds -= seconds;
}

#define UNDETERMINED 0
// Returns the difference between two dates
char rfDate_Diff(RF_Date* tDate, RF_Date* oDate, RF_Date* res)
{
    //char relation = UNDETERMINED;
    //get the differences
    int32_t yDiff = (int32_t)oDate->year-(int32_t)tDate->year;
    int32_t mDiff = (char)oDate->month-(char)tDate->month;
    int32_t mDayDiff = (char)oDate->mDay - (char)tDate->mDay;
    int32_t hoursDiff = (char)oDate->hours - (char)tDate->hours;
    int32_t minsDiff = (char)oDate->minutes - (char)tDate->minutes;
    int32_t secsDiff = (char)oDate->seconds - (char)tDate->seconds;

    //if the year can determine the result
    if(yDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        rfDate_SubYears(res,oDate->year);
        rfDate_SubMonths(res,oDate->month);
        rfDate_SubDays(res,oDate->mDay);
        rfDate_SubHours(res,oDate->hours);
        rfDate_SubMinutes(res,oDate->minutes);
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(yDiff > 0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        rfDate_SubYears(res,tDate->year);
        rfDate_SubMonths(res,tDate->month);
        rfDate_SubDays(res,tDate->mDay);
        rfDate_SubHours(res,tDate->hours);
        rfDate_SubMinutes(res,tDate->minutes);
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //continue on to month checking
    if(mDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        res->year = 0;
        rfDate_SubMonths(res,oDate->month);
        rfDate_SubDays(res,oDate->mDay);
        rfDate_SubHours(res,oDate->hours);
        rfDate_SubMinutes(res,oDate->minutes);
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(mDiff >0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        res->year = 0;
        rfDate_SubMonths(res,tDate->month);
        rfDate_SubDays(res,tDate->mDay);
        rfDate_SubHours(res,tDate->hours);
        rfDate_SubMinutes(res,tDate->minutes);
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //continue on to month day checking
    if(mDayDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        res->year = 0;
        res->month = 0;
        rfDate_SubDays(res,oDate->mDay);
        rfDate_SubHours(res,oDate->hours);
        rfDate_SubMinutes(res,oDate->minutes);
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(mDayDiff >0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        res->year = 0;
        res->month = 0;
        rfDate_SubDays(res,tDate->mDay);
        rfDate_SubHours(res,tDate->hours);
        rfDate_SubMinutes(res,tDate->minutes);
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //continue on to hour checking
    if(hoursDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        rfDate_SubHours(res,oDate->hours);
        rfDate_SubMinutes(res,oDate->minutes);
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(hoursDiff >0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        rfDate_SubHours(res,tDate->hours);
        rfDate_SubMinutes(res,tDate->minutes);
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //continue on to minute checking
    if(minsDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        res->hours = 0;
        rfDate_SubMinutes(res,oDate->minutes);
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(minsDiff >0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        res->hours = 0;
        rfDate_SubMinutes(res,tDate->minutes);
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //continue on to minute checking
    if(minsDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        res->hours = 0;
        rfDate_SubMinutes(res,oDate->minutes);
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(minsDiff >0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        res->hours = 0;
        rfDate_SubMinutes(res,tDate->minutes);
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //cutting it really close, continue on to second checking
    if(secsDiff < 0)
    {
        //PAST
        RF_DATE_ASSIGN_PTR(res,tDate);
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        res->hours = 0;
        res->minutes = 0;
        rfDate_SubSeconds(res,oDate->seconds);
        return RF_PAST;
    }
    else if(secsDiff >0)
    {
        //FUTURE
        RF_DATE_ASSIGN_PTR(res,oDate)
        res->year = 0;
        res->month = 0;
        res->mDay = 0;
        res->hours = 0;
        res->minutes = 0;
        rfDate_SubSeconds(res,tDate->seconds);
        return RF_FUTURE;
    }

    //if we get here it means that the two dates are identical to the second so return that
    return RF_DATE_SAME;
}

/*
needs thinking
// brief Turns a time interval into seconds
char rfDate_I_ToSeconds(const RF_Date* interval,uint64_t* result)
{
    *result = 0;
    uint64_t maxYears = ULLONG_MAX/31556926ULL;
    if(interval->year >= maxYears)
    {
        LOG_ERROR("")
        *result = ULLONG_MAX;
        return false;
    }
    *result = (uint64_t)interval->year* ;
    //if(*result )

}
*/
