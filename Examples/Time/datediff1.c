#include <rf_time.h>

int main()
{
//! [ALL]
	initRefu("refuErrorLog.txt","refuInfoLog.txt");
	//initialize the date as the current local time
    RF_Date date,oDate,result;
    rfDate_Init_Now(&date,true);
    printf("The starting date is (%u) %u/%u/%u\t%u:%u \n",date.wDay,date.mDay,date.month,date.year,date.hours,date.minutes);
	//make the other date the same as this, but also subtract from it
    oDate = date;
    rfDate_SubYears(&oDate,10);
    rfDate_SubMonths(&oDate,10);
    rfDate_SubHours(&oDate,4);
    rfDate_SubMinutes(&oDate,25);
    rfDate_SubSeconds(&oDate,29);
    printf("The final date is (%u) %u/%u/%u\t%u:%u \n",oDate.wDay,oDate.mDay,oDate.month,oDate.year,oDate.hours,oDate.minutes);
//! [FUNCTION CALL]
	//compare the two dates
    char relation = rfDate_Diff(&date,&oDate,&result);
	//print the result depending on the returned relation (past,future) of the dates
    switch(relation)
    {
        case RF_FUTURE:
//! [ACCESSED MEMBERS]
            printf("The second date is %u YEARS, %u MONTHS, %u DAYS, %u HOURS, %u MINUTES and %u SECONDS in the PAST\n",
			result.year,result.month,result.mDay,result.hours,result.minutes,result.seconds);
//! [ACCESSED MEMBERS]
        break;
        case RF_PAST:
            printf("The second date is %u YEARS, %u MONTHS, %u DAYS, %u HOURS, %u MINUTES and %u SECONDS in the FUTURE\n",
			result.year,result.month,result.mDay,result.hours,result.minutes,result.seconds);
        break;
        case RF_DATE_SAME:
            printf("The two dates are identical\n");
        break;
		default:
			printf("There was an error");
		break;
    }
//! [FUNCTION CALL]
//! [ALL]
	return 0;
}