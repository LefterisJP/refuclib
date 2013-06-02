#include <RFtimer.h>
#include <stdio.h>
#include <refu.h>

int main()
{
    int i;
    rfInit("refuErrorLog.txt","refuInfoLog.txt");
/**
 ** [ALL]
 **
 **/
    float a;
	//initialize a timer with initial resolution at milliseconds
/**
 ** [INIT]
 **
 **/
    RF_Timer timer;
    rfTimer_Init(&timer,RF_TIMER_MILLISECONDS);
/**
 ** [INIT]
 **
 **/
	//perform some floating point calculations
    for(i=0;i<10000;i++)
        a = 5.3*6.2;
	//here let's change the resolution and query the timer
/**
 ** [QUERY]
 **
 **/
	char resolution = RF_TIMER_MICROSECONDS;
    double units  = rfTimer_Query(&timer,resolution);
/**
 ** [QUERY]
 **
 **/
	//depending on the result let's print the elapsed time
    switch(resolution)
    {
        case RF_TIMER_SECONDS:
            printf("%f seconds elapsed\n",units);
        break;
        case RF_TIMER_MILLISECONDS:
            printf("%f milliseconds elapsed\n",units);
        break;
        case RF_TIMER_MICROSECONDS:
            printf("%f microseconds elapsed\n",units);
        break;
        case RF_TIMER_NANOSECONDS:
            printf("%f nanoseconds elapsed\n",units);
        break;
    }
/**
 ** [ALL]
 ** [NORES1]
 **
 **/
	units = rfTimer_Query(&timer,0);
/**
 ** [NORES1]
 ** [NORES2]
 **
 **/
	units = rfTimer_Query(&timer,RF_NONE);
/**
 ** [NORES2]
 **
 **/
	return 0;
}