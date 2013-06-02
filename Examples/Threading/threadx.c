#include <RFthread.h>
#include <RFstring.h>
#include <RFtextfile.h>
#include <RFtimer.h>
#include <refu.h>

#define THREADS_NUM 5

/**
 ** [THREAD_DATA]
 **
 **/
//Packing all the data to give as input to the Thread in a structure
typedef struct i_Thread_Data
{
    //Just a number to give to the thread for the printfs
    uint8_t num;
    //A mutex for the common file
    RF_Mutex* m;
    //The commom file
    RF_TextFile* f;
} i_Thread_Data;
/**
 ** [THREAD_DATA]
 **
 **/

/**
 ** [THREAD_FUNCTION]
 **
 **/
//The function that demonstrates the usage of RF_ThreadX
void* threadXTest(RF_ThreadX* thisThread)
{
    int i;
    char stop;
    uint32_t size;
    //notice that we get the data from inside the thread itself
    i_Thread_Data* d = (i_Thread_Data*)rfThread_GetData(thisThread);
    //attempt to access the common file
    while(rfMutex_TryLock(d->m) != RF_SUCCESS)
    {
        printf("ThreadX %d is attempting to access the common file but it is currently locked\n",d->num);
    }
    for(i = 0; i < 100; i ++)
    {
        //demonstrating ThreadX ReadSignal, we attempt to read if something was sent by any other thread in this thread's queue
/**
 ** [READ_SIGNAL]
 **
 **/
        if(rfThreadX_TryReadSignal(thisThread,&stop,&size) == RF_SUCCESS)
        {
            if(stop == true)//the only thing that we sent in this example is a boolean so it's always gonna be true
            {//consider this a stop signal and stop the thread's operation
                printf("**Thread %d was given a STOP signal** from the main thread while writing line %d\n",d->num,i);
                rfMutex_Unlock(d->m);
                return 0;
            }
        }
/**
 ** [READ_SIGNAL]
 **
 **/
        //otherwise just write a line to the file
        rfTextFile_Write(d->f,RFS_("Thread %d writing the %d line in the file\n",d->num,i));
    }//file writing for closes

    //release the mutex
    if(rfMutex_Unlock(d->m) != RF_SUCCESS)
    {
        printf("ERROR in releasing the mutex from thread %d\n",d->num);
        return -1;
    }
    printf("Thread %d finished using the resource and unlocked the mutex\n",d->num);
    return 0;
}
/**
 ** [THREAD_FUNCTION]
 **
 **/


int main()
{
    int i;
    //Our Extended Threads
    RF_ThreadX threads[THREADS_NUM];
    //and their data
    i_Thread_Data d[THREADS_NUM];
    //The mutex to protect access to the common file
    RF_Mutex m;
    //A textfile that all threads will write in
    RF_TextFile t;
/**
 ** [INIT_STUFF]
 **
 **/
    //init the library
    rfInit("errorlog","infolog");
    //Initialize the text file
    rfTextFile_Init(&t,RFS_("testfile"),RF_FILE_NEW,RF_UTF8);
    //The main thread creates the mutex
    if(rfMutex_Init(&m,0) != RF_SUCCESS)
        exit(-1);
    //populate the thread data
    for(i = 0; i < THREADS_NUM; i ++)
    {
        d[i].m = &m;
        d[i].f = &t;
        d[i].num = i+1;
    }
/**
 ** [INIT_STUFF]
 **
 **/

/**
 ** [INIT_THREADS]
 **
 **/
    //init 10 joinable ThreadsX and give them the data
    for(i=0; i< THREADS_NUM; i ++)
        rfThreadX_Init(&threads[i],0,&threadXTest,&d[i],1000,10);//10 is the max messages that can wait in the message queue,1000 is LMS size
/**
 ** [INIT_THREADS]
 **
 **/

/**
 ** [SEND_SIGNAL]
 **
 **/
    //let's send a stop boolean to a thread to demonstrate ThreadX
    char stop = true;
    rfThreadX_SendSignal(&threads[2],&stop,1);
/**
 ** [SEND_SIGNAL]
 ** [CLEANUP]
 **
 **/
    //wait for all the threads to finish
    for(i=0; i< THREADS_NUM; i ++)
        rfThread_Join(&threads[i]);
    printf("All threads have had their turn in writing and got joined back to the main thread\n");
    //cleanup
    rfMutex_Deinit(&m);
    rfTextFile_Deinit(&t);
    return 0;
/**
 ** [CLEANUP]
 **
 **/
}

