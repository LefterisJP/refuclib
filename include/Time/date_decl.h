

/*
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
*/
#ifndef RF_DATE_DECL_H
#define RF_DATE_DECL_H


/**
** @internal
** @author Lefteris
** @date 17/05/2012
** @endinternal
** @brief A cross-platform Date structure
**
** This structure provides a cross-platform way to measure Date and time and its intervals. Its highest resolution is in seconds. It should not be used as a
** timer structure but as a way to retrieve, set or manipulate dates and to measure their intervals. Contains functions that can retrieve the date from the system or set it.
** Also contains functions that can manipulate a date such as @ref rfDate_AddDays or @ref rfDate_SubMonths which can add days or subtract months from a date
** In some cases the structure can also be interpreted as a time interval. Examples of such usage are in the function that returns the difference between two dates @ref rfDate_Diff.
** In there the third parameter is not interpreted as a Date but as a time interval. For more information look at the individual funtions documentation.
**/
typedef struct RF_Date
{
    //! The current number of seconds. Values can be in the range of 0-59 (in Linux can be 60 for leap seconds)
    unsigned char seconds;
    //! The current number of minutes. Values can be in the range of 0-59
    unsigned char minutes;
    //! The number of hours past midgnight. Values can be in the range of 0-23.
    unsigned char hours;
    //! The day of the week. Ranges between 0-6. Legal values are @c RF_SUNDAY, @c RF_MONDAY, @c RF_TUESDAY, @c RF_WEDNESDAY, @c RF_THURSDAY, @c RF_FRIDAY and @c RF_SATURDAY
    unsigned char wDay;
    //! The day of the month. Values can be in the range of 1-31
    unsigned char mDay;
    //! The day of the year. Ranges from 1 to 365(366 for leap years)
    uint16_t yDay;
    //! The month. Values can be in the range of 1-12. Legal values are @c RF_JANUARY, @c RF_FEBRUARY, @c RF_MARCH, @c RF_APRIL, @c RF_MAY, @c RF_JUNE, @c RF_JULY, @c RF_AUGUST, @c RF_SEPTEMBER, @c RF_OCTOBER, @c RF_NOVEMBER and @c RF_DECEMBER
    unsigned char month;
    //! A boolean denoting if the year is a leap or not
    char isLeap;
    //! The year
    uint16_t year;
}RF_Date;


#endif//include guards end
