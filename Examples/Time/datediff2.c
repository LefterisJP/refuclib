#include <RFdate.h>
#include <stdio.h>
#include <refu.h>


int main()
{
	rfInit("refuErrorLog.txt","refuInfoLog.txt");
    RF_Date date,oDate,result;
    rfDate_Init_Now(&date,true);
    printf("The starting date is (%u) %u/%u/%u\t%u:%u \n",date.wDay,date.mDay,date.month,date.year,date.hours,date.minutes);
    oDate = date;
 //! [ADD BIG]
	rfDate_AddHours(&oDate,36526);
    rfDate_AddMinutes(&oDate,69000);
/**
 ** [ADD BIG]
 **
 **/
    printf("The final date is (%u) %u/%u/%u\t%u:%u \n",oDate.wDay,oDate.mDay,oDate.month,oDate.year,oDate.hours,oDate.minutes);

    char relation = rfDate_Diff(&date,&oDate,&result);
    switch(relation)
    {
        case RF_FUTURE:
            printf("The second date is %u YEARS, %u MONTHS, %u DAYS, %u HOURS, %u MINUTES and %u SECONDS in the FUTURE\n",result.year,result.month,result.mDay,result.hours,result.minutes,result.seconds);
        break;
        case RF_PAST:
            printf("The second date is %u YEARS, %u MONTHS, %u DAYS, %u HOURS, %u MINUTES and %u SECONDS in the PAST\n",result.year,result.month,result.mDay,result.hours,result.minutes,result.seconds);
        break;
        case RF_DATE_SAME:
            printf("The two dates are identical\n");
        break;
    }
/**
 ** [CONFIRM]
 **
 **/
   rfDate_SubYears(&oDate,4);
   rfDate_SubMonths(&oDate,3);
   rfDate_SubDays(&oDate,16);
   rfDate_SubHours(&oDate,20);
/**
 ** [CONFIRM]
 **
 **/
    relation = rfDate_Diff(&date,&oDate,&result);
    switch(relation)
    {
        case RF_FUTURE:
            printf("The second date is %u YEARS, %u MONTHS, %u DAYS, %u HOURS, %u MINUTES and %u SECONDS in the FUTURE\n",result.year,result.month,result.mDay,result.hours,result.minutes,result.seconds);
        break;
        case RF_PAST:
            printf("The second date is %u YEARS, %u MONTHS, %u DAYS, %u HOURS, %u MINUTES and %u SECONDS in the PAST\n",result.year,result.month,result.mDay,result.hours,result.minutes,result.seconds);
        break;
        case RF_DATE_SAME:
            printf("The two dates are identical\n");
        break;
    }
	return 0;
}