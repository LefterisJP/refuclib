#include <RFthread.h>
#include <RFsemaphore.h>
#include <RFstring.h>
#include <RFtextfile.h>
#include <RFtimer.h>
#include <refu.h>

/**
 ** [THREAD_DATA]
 **
 **/
//Packing all the data to give as input to the Thread in a structure
typedef struct i_Thread_Data
{
    //Just a number to give to the thread for the printfs
    uint8_t num;
    //The semaphore for the example
    RF_Semaphore* s;
} i_Thread_Data;
/**
 ** [THREAD_DATA]
 **
 **/

/**
 ** [THREAD_FUNCTION]
 **
 **/
//The function that safely writes to the file using the semaphore from this thread
void* semTest(void* data)
{
    i_Thread_Data* d = (i_Thread_Data*)data;
    //attempt to access the common resource
    while(rfSemaphore_TryWait(d->s) != RF_SUCCESS)
    {
        printf("Thread %d is attempting to access the common resource but currently no more resources are available\n",d->num);
    }
    //using the resource by ehm..sleeping for 500 ms
    printf("Thread %d got ACCESS to the resource and will now use it\n",d->num);
    rfSleep_ms(500);

    //release the resource
    if(rfSemaphore_Post(d->s) != RF_SUCCESS)
    {
        printf("ERROR in posting the semaphore from thread %d\n",d->num);
        return -1;
    }
    printf("Thread %d finished using the resource and posted to the semaphore\n",d->num);
    return 0;
}
/**
 ** [THREAD_FUNCTION]
 **
 **/


int main()
{
    int i;
    //Two threads
    RF_Thread threads[10];
    //and their data
    i_Thread_Data d[10];
    //The semaphore
    RF_Semaphore s;
    rfInit("errorLog","infoLog");
/**
 ** [INIT_STUFF]
 **
 **/
    //The main thread creates the semaphore
    //Initialize the semaphore
    if(rfSemaphore_Init(&s,5,5) != RF_SUCCESS)
        exit(-1);
    //populate the thread data
    for(i = 0; i < 10; i ++)
    {
        d[i].s = &s; //just point to the semaphore
        d[i].num = i+1; //and give each thread an id
    }
/**
 ** [INIT_STUFF]
 **
 **/

/**
 ** [INIT_THREADS]
 **
 **/
    //init 10 joinable threads and give them the data
    for(i=0; i< 10; i ++)
        rfThread_Init(&threads[i],0,&semTest,&d[i]);
/**
 ** [INIT_THREADS]
 **
 **/

/**
 ** [CLEANUP]
 **
 **/
    //wait for all the threads to finish
    for(i=0; i< 10; i ++)
        rfThread_Join(&threads[i]);
    printf("All threads have had their turn in using the resource and got joined back to the main thread\n");
    //cleanup
    rfSemaphore_Deinit(&s);
    return 0;
/**
 ** [CLEANUP]
 **
 **/
}
