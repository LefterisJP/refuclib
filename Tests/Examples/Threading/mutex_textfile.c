#include <rf_threads.h>
#include <rf_string.h>
#include <IO/rfc_textfile.h>

//! [THREAD_DATA]
//Packing all the data to give as input to the Thread in a structure
typedef struct i_Thread_Data
{
    //The textfile handle
    RF_TextFile* file;
    //Just a number to give to the thread for the printfs
    uint8_t num;
    //The file's mutex
    RF_Mutex* m;
    //The string literal to write to the file
    RF_String s;
} i_Thread_Data;
//! [THREAD_DATA]

//! [THREAD_FUNCTION]
//The function that safely writes to the file using the mutex from this thread
void* safeWrite(void* data)
{
    int i;
    i_Thread_Data* d = (i_Thread_Data*)data;
    //lock the file's mutex
    while(rfMutex_TryLock(d->m) != RF_SUCCESS)
    {
        printf("Thread %d is attempting to access the file but the file is busy with another thread\n",d->num);
    }
    //write to the file 10000 times just in order for the mutex locking to work and show us the message
    for(i = 0; i < 10000; i ++)
        rfTextFile_Write(d->file,&d->s);
    //unlock the file's mutex
    rfMutex_Unlock(d->m);
    printf("Thread %d finished writing to the file and unlocked the mutex\n",d->num);
    return 0;
}
//! [THREAD_FUNCTION]

//the main function
int main()
{
    //Two threads
    RF_Thread t1,t2;
    //and their data
    i_Thread_Data d1,d2;
    //The mutex of the file
    RF_Mutex m;
    //The file
    RF_TextFile file;
//! [INIT_STUFF]
    rfInit();//init the library
    //The main thread creates and opens a file for writing
    rfTextFile_Init(&file,RFS_("testfile"),RF_FILE_NEW,RF_UTF8);
    //Initialize the mutex
    if(rfMutex_Init(&m,0) != RF_SUCCESS)
        exit(-1);
    //populate the two thread's data
    d1.file = d2.file = &file;
    d1.num = 1; d2.num = 2;
    d1.m = d2.m = &m;
    rfString_Init(&d1.s,"I am the string of the first thread\n");
    rfString_Init(&d2.s,"I am the string of the second thread\n");
//! [INIT_STUFF]

//! [INIT_THREADS]
    //initialize (and run) the threads
    rfThread_Init(&t1,0,&safeWrite,&d1);
    rfThread_Init(&t2,0,&safeWrite,&d2);
//! [INIT_THREADS]

//! [CLEANUP]
    //wait for the threads to finish
    rfThread_Join(&t1);
    rfThread_Join(&t2);
    //cleanup
    rfMutex_Deinit(&m);
    rfTextFile_Deinit(&file);
    return 0;
//! [CLEANUP]
}
