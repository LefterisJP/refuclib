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
**
**
** -- Time/common.ph
** This header file contains macros to be used privately by the
** Time module of the library
**
**/


//calculates if the year is leap or not
#define LEAP_YEAR_CHECK(i_Year,i_isLeap) \
                i_isLeap = false;\
                char i_div4 =  ((i_Year)%4)==0;\
                char i_div100 = ((i_Year)%100)==0;\
                /*if it's divisible by 4 but not by 100 it's a leap year*/\
                if(i_div4 ==true && i_div100 == false)\
                    i_isLeap = true;\
                else if(i_div4 ==true && i_div100 ==true && (((i_Year)%400)==0))\
                    i_isLeap = true;

//Calculates the day of the year
#define GET_DAY_OF_YEAR(i_Month,i_mDay,i_isLeap,i_yDay)  \
    i_yDay = 0;\
    switch(i_Month)\
    {\
        case RF_JANUARY: /*has 31*/\
            i_yDay = i_mDay;\
        break;\
        case RF_FEBRUARY: /*has 28 or 29*/\
            i_yDay = 31+i_mDay;\
        break;\
        case RF_MARCH: /*has 31*/\
            i_yDay = 59+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_APRIL: /*has 30*/\
            i_yDay = 90+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_MAY: /*has 31*/\
            i_yDay = 120+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_JUNE:/*has 30*/\
            i_yDay = 151+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_JULY:/*has 31*/\
            i_yDay = 181+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_AUGUST:/*has 31*/\
            i_yDay = 212+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_SEPTEMBER:/*has 30*/\
            i_yDay = 243+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_OCTOBER:/*has 31*/\
            i_yDay = 273+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_NOVEMBER:/*has 30*/\
            i_yDay = 304+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
        case RF_DECEMBER:/*has 31*/\
            i_yDay = 334+i_mDay;\
            if(i_isLeap)\
                i_yDay +=1;\
        break;\
    }

//! Assigns the value of src RF_Date pointer to the i_dst poiner
#define RF_DATE_ASSIGN_PTR(i_dst,i_src)     \
    i_dst->seconds = i_src->seconds;\
    i_dst->minutes = i_src->minutes;\
    i_dst->hours = i_src->hours;\
    i_dst->wDay = i_src->wDay;\
    i_dst->mDay = i_src->mDay;\
    i_dst->yDay = i_src->yDay;\
    i_dst->yDay = i_src->yDay;\
    i_dst->month = i_src->month;\
    i_dst->isLeap = i_src->isLeap;\
    i_dst->year = i_src->year;

