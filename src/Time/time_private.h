/**
** @author Lefteris
** @date 17/05/2012
**
** Contains data (macros, functions e.t.c.) that are needed by multiple
** time related files and are not needed to be accessible to the users of
** the the library
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

