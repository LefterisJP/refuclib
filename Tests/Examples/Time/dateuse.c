#include <rf_time.h>

int main()
{
    initRefu("refuErrorLog.txt","refuInfoLog.txt");
//! [ALL]
	//Initialize a date with the current local time
	RF_Date date;
    rfDate_Init_Now(&date,true);
	//add 3 months,2 days and 6 hours
	rfDate_AddMonths(&date,3);
	rfDate_AddDays(&date,2);
	rfDate_AddHours(&date,6);
	//and now attempt to set this new date as the current local time
	if(rfDate_SetToLocal(&date) != OPERATION_SUCCESS)
		printf("The system did not allow the date change");
	
	printf("The date is now changed!");
//! [ALL]
	return 0;
}