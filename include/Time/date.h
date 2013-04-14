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
** --Time/date.h
** This header includes macros and functions that operate on RF_Date
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h>//for import export macro
#include <Definitions/types.h> //for fixed size types
#include <Time/date_decl.h> //for RF_Date
#include <Time/date.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_DATE_H
#define RF_DATE_H


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif



///Days of the week for the RF_Date
#define RF_SUNDAY       0
#define RF_MONDAY       1
#define RF_TUESDAY      2
#define RF_WEDNESDAY    3
#define RF_THURSDAY     4
#define RF_FRIDAY       5
#define RF_SATURDAY     6

///Months for the RF_Date
#define RF_JANUARY      1
#define RF_FEBRUARY     2
#define RF_MARCH        3
#define RF_APRIL        4
#define RF_MAY          5
#define RF_JUNE         6
#define RF_JULY         7
#define RF_AUGUST       8
#define RF_SEPTEMBER    9
#define RF_OCTOBER      10
#define RF_NOVEMBER     11
#define RF_DECEMBER     12
/*-------------------------------------------------------------------------Methods to Create a Date-------------------------------------------------------------------------------*/
//! @name Creating a Date Object
//! @{


//! @memberof RF_Date
//! @brief Initializes a Date object with the current date
//!
//! @param d The date to initialize
//! @param local A boolean flag denoting if the date should be in local system time. If not it is considered to be in UTC (Coordinated Universal Time)
//! @return Returns true for success and false for failure
i_DECLIMEX_ char rfDate_Init_Now(RF_Date* d,char local);

//! @memberof RF_Date
//! @brief Allocates and returns a Date object with the current date
//!
//! @param local A boolean flag denoting if the date should be in local system time. If not it is considered to be in UTC (Coordinated Universal Time)
//! @return The allocated date
i_DECLIMEX_ RF_Date* rfDate_Create_Now(char local);

//! @memberof RF_Date
//! @brief Initializes a Date object with a specific date
//!
//! @param d The date to initialize
//! @param wDay The day of the week. Legal values are @c RF_SUNDAY, @c RF_MONDAY, @c RF_TUESDAY, @c RF_WEDNESDAY, @c RF_THURSDAY, @c RF_FRIDAY and @c RF_SATURDAY
//! @param mDay The day of the month. Legal values range between  1-31
//! @param month The month. Legal values are (1-12) @c RF_JANUARY, @c RF_FEBRUARY, @c RF_MARCH, @c RF_APRIL, @c RF_MAY, @c RF_JUNE, @c RF_JULY, @c RF_AUGUST, @c RF_SEPTEMBER, @c RF_OCTOBER, @c RF_NOVEMBER and @c RF_DECEMBER
//! @param year The year. Legal values are 1601 to 30827. And this limitation is only due to Windows.
//! @param hour The hour of the day. Legal values are from 0 to 23
//! @param min The minutes of the hour. Legal values are from 0 to 59
//! @param sec The seconds after the minutes. Legal values are from 0 to 59
//! @return Returns true for success and false for failure
i_DECLIMEX_ char rfDate_Init(RF_Date* d,unsigned char wDay,unsigned char mDay,unsigned char month,uint16_t year,unsigned char hour,unsigned char min,unsigned char sec);

//! @memberof RF_Date
//! @brief Create a Date object with a specific date
//!
//! @param wDay The day of the week. Legal values are @c RF_SUNDAY, @c RF_MONDAY, @c RF_TUESDAY, @c RF_WEDNESDAY, @c RF_THURSDAY, @c RF_FRIDAY and @c RF_SATURDAY
//! @param mDay The day of the month. Legal values range between 1-31
//! @param month The month. Legal values are from (1-12) @c RF_JANUARY, @c RF_FEBRUARY, @c RF_MARCH, @c RF_APRIL, @c RF_MAY, @c RF_JUNE, @c RF_JULY, @c RF_AUGUST, @c RF_SEPTEMBER, @c RF_OCTOBER, @c RF_NOVEMBER and @c RF_DECEMBER
//! @param year The year. Legal values are 1601 to 30827. And this limitation is only due to Windows.
//! @param hour The hour of the day. Legal values are from 0 to 23
//! @param min The minutes of the hour. Legal values are from 0 to 59
//! @param sec The seconds after the minutes. Legal values are from 0 to 59
//! @return Returns true for success and false for failure
i_DECLIMEX_ RF_Date* rfDate_Create(unsigned char wDay,unsigned char mDay,unsigned char month,uint16_t year,unsigned char hour,unsigned char min,unsigned char sec);

//! @}
//closing the doxygen group of date creation

//! @memberof RF_Date
//! @brief Destroys a Date made with rfDate_Create
//!
//! @param d The date to destroy
i_DECLIMEX_ void rfDate_Destroy(RF_Date* d);

/*-------------------------------------------------------------------------Methods to manipulate a Date-------------------------------------------------------------------------------*/
//! @name Manipulating a Date
//! @{

//! @memberof RF_Date
//! @brief Adds a number of years to the Date
//!
//! @param d The date to work with
//! @param years The number of years to add
i_DECLIMEX_ void rfDate_AddYears(RF_Date* d,uint32_t years);
//! @memberof RF_Date
//! @brief Removes a number of years from the Date
//!
//! @param d The date to work with
//! @param years The number of years to remove
i_DECLIMEX_ void rfDate_SubYears(RF_Date* d,uint32_t years);

//! @memberof RF_Date
//! @brief Adds a number of months to the Date
//!
//! @param d The date to work with
//! @param months The number of months to add
i_DECLIMEX_ void rfDate_AddMonths(RF_Date* d,uint32_t months);
//! @memberof RF_Date
//! @brief Removes a number of months from the Date
//!
//! @param d The date to work with
//! @param months The number of months to remove
i_DECLIMEX_ void rfDate_SubMonths(RF_Date* d,uint32_t months);

//! @memberof RF_Date
//! @brief Adds a number of days to the Date
//!
//! @param d The date to work with
//! @param days The number of days to add
i_DECLIMEX_ void rfDate_AddDays(RF_Date* d,uint32_t days);
//! @memberof RF_Date
//! @brief Removes a number of days from the Date
//!
//! @param d The date to work with
//! @param days The number of days to remove
i_DECLIMEX_ void rfDate_SubDays(RF_Date* d,uint32_t days);

//! @memberof RF_Date
//! @brief Adds a number of hours to the Date
//!
//! @param d The date to work with
//! @param hours The number of hours to add
i_DECLIMEX_ void rfDate_AddHours(RF_Date* d,uint32_t hours);
//! @memberof RF_Date
//! @brief Removes a number of hours from the Date
//!
//! @param d The date to work with
//! @param hours The number of hours to remove
i_DECLIMEX_ void rfDate_SubHours(RF_Date* d,uint32_t hours);

//! @memberof RF_Date
//! @brief Adds a number of minutes to the Date
//!
//! @param d The date to work with
//! @param minutes The number of minutes to add
i_DECLIMEX_ void rfDate_AddMinutes(RF_Date* d,uint32_t minutes);
//! @memberof RF_Date
//! @brief Removes a number of minutes from the Date
//!
//! @param d The date to work with
//! @param minutes The number of minutes to remove
i_DECLIMEX_ void rfDate_SubMinutes(RF_Date* d,uint32_t minutes);

//! @memberof RF_Date
//! @brief Adds a number of seconds to the Date
//!
//! @param d The date to work with
//! @param seconds The number of seconds to add
i_DECLIMEX_ void rfDate_AddSeconds(RF_Date* d,uint32_t seconds);
//! @memberof RF_Date
//! @brief Removes a number of seconds from the Date
//!
//! @param d The date to work with
//! @param seconds The number of seconds to remove
i_DECLIMEX_ void rfDate_SubSeconds(RF_Date* d,uint32_t seconds);
//! @}
//closing the Date Manipulation doxygen group

//!

/*------------------------------------------------------------------------- System Methods concerning Dates-------------------------------------------------------------------------------*/
//! @name Functions that affect the System
//! @{

//! @memberof RF_Date
//! @brief Sets this date as the current system date and time
//!
//! This function set the system's date and time which is always considered to be
//! in the UTC timezone. If you desire to set the local time then use @ref rfDate_SetToLocal.
//! When used under Linux the user of the application that calls this function must make certain that
//! it has super user privileges. If the application does not have super user privileges then the function will
//! return @c RE_PERMISSION
//! @param d The date to set as system date and time
//! @return Returns @c true for succesfull setting or @c false otherwise with appropriate error logging
i_DECLIMEX_ char rfDate_SetToSystem(RF_Date* d);

//! @memberof RF_Date
//! @brief Sets this date as the current local date and time
//!
//! This function sets the system's local date and time depending on the timezone that
//! the system is in. Since the system is always in UTC time, the current timezone information
//! is used and the system time is also changed in respect to the difference with the local time.
//! When used under Linux the user of the application that calls this function must make certain that
//! it has super user privileges. If the application does not have super user privileges then the function will
//! return @c RE_PERMISSION
//! @param d The date to set as system's local date and time
//! @return Returns @c true for succesfull setting or @c false otherwise with appropriate error logging
i_DECLIMEX_ char rfDate_SetToLocal(RF_Date* d);

//! @}
//closing the System Methods doxygen group

/*-------------------------------------------------------------------------Methods dealing with time intervals-------------------------------------------------------------------------------*/
#define RF_FUTURE       1
#define RF_PAST         2
#define RF_DATE_SAME    3
//! @name Time Intervals
//! @{

//! @memberof RF_Date
//! @brief Returns the difference between two dates
//!
//! This function calculates the difference of the other date @c oDate minus this date @c tDate and the result is returned inside @c res.
//! To determine whether the difference is on the future or in the past all that has to be done is to check the returned char which will either be
//! @c RF_FUTURE if it is in the future, @c RF_PAST if it is in the past and @c RF_DATE_SAME if they are the same date.
//! For an example take a look at the following:
//! @snippet Time/datediff1.c ALL
//! Here we can see the general example usage of the Diff function. The resulting print from the Diff on the screen would be:
//! @code The second date is 10 YEARS, 10 MONTHS, 0 DAYS, 4 HOURS, 25 MINUTES and 29 SECONDS in the PAST @endcode
//! As we can see from the example the resulting date object @c res is not an actual date and under
//! no circumstances should it be interpreted as such. A typical function call would look something like this part of the example
//! @snippet Time/datediff1.c FUNCTION CALL
//! We can clearly see the 3 cases that can occur. The comparing date can either be in the future, the past or the same as the one we compare it to. We should also
//! take errors into account even though as of the time of this writing the function does not actually return an error, because it is assumed that both Date objects
//! are properly initialized before given to the function.
//! Finally from the example's code here
//! @snippet Time/datediff1.c ACCESSED MEMBERS
//! we can see which members of the resulting structure should be read. The members of RF_Date:
//! + @c year
//! + @c month
//! + @c mDay
//! + @c hours
//! + @c minutes
//! + @c seconds
//!
//! are the only ones that make sense in the calculated result and should be read in order to determine the difference just like in the example.
//! This example was pretty straighforward since not very big amounts of units were added. Picture an example like below where we provide a date that is a very big number
//! of hours and minutes into the future.
//! @snippet Time/datediff2.c ADD BIG
//! The function will correctly interpret the difference later and if called in the same way as the first example it will return
//! @code The second date is 4 YEARS, 3 MONTHS, 16 DAYS, 20 HOURS, 0 MINUTES and 0 SECONDS in the FUTURE @endcode
//! @note The weekday parameter is meaningless in such an operation and is not taken into account for the calculation
//! @param[in] tDate This date
//! @param[in] oDate The date with which we want to compare this date and get a difference
//! @param[out] res Give a reference to a date object to receive the resulting date difference
//! @return Returns either @c RF_FUTURE if the second date is in the future or @c RF_PAST if it is in the past. The function will also return
//! @c RF_DATE_SAME if the two dates are identical to the second. If @c RF_DATE_SAME is returned then the result inside res is undefined and should not be used.
//!  If an error happens then @c RF_FAILURE is returned.
i_DECLIMEX_ char rfDate_Diff(RF_Date* tDate,RF_Date* oDate,RF_Date* res);
/*
needs thinking
//! @memberof RF_Date
//! @brief Turns a time interval into seconds
//!
//! @param interval Accepts a time interval RF_Date.
//! @return Returns true for success and false for failure. Failure can occur mainly if the interval is too long to be represented by an uint64_t in the particular
//! system that the function is ran at. In that case the result will contain the value of ULLONG_MAX from limits.h and the return will be false. An error will additionally be logged.
char rfDate_I_ToSeconds(const RF_Date* interval,uint64_t* result);
*/
//! @}
//closing the time intervals doxygen group


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif



#endif//include guards end
