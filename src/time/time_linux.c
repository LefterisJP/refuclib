/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/time/sleep.h>

#include <unistd.h>
#include <errno.h>//for linux errno
#include <stdlib.h>

// Suspends the calling thread for a number of seconds
void rfSleep(uint32_t seconds)
{
    sleep(seconds);
}
//Suspends the calling thread for a number of milliseconds
void rf_sleep_ms(uint32_t milliseconds)
{
    usleep(milliseconds*1000);
}


