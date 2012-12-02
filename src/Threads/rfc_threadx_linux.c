#include <Threads/rfc_threadx.h>

#include <rf_error.h>
#include <rf_memory.h>
#include <rf_utils.h> //for RF_BITFLAG_ON()

#include <rf_localmem.h>//for LMS initialization
#include <rf_stdio.h>//stdio buffer thread-specific initialization

//  The function that serves as the starting address for a RF_ThreadX in Linux
void* RF_THREADX_FUNCTION(void* param)
{
    void* ret;//the return value
    //get a pointer to the thread
    RF_ThreadX* thread = (RF_ThreadX*)param;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,thread->INH_Thread.lmsSize);
    //initialize the stdio for this thread
    rfInitStdio();
    //run the thread function
    ret = thread->INH_Thread.ptr2onExecution(thread);
    //free the local memory stack and return
    free(lms.stack);
    return ret;
}

// Allocates and returns a ThreadX
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_ThreadX* rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxSignals )
#else
RF_ThreadX* i_rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxSignals )
#endif
{
    RF_ThreadX* ret;
    RF_MALLOC(ret,sizeof(RF_ThreadX));
    if( rfThreadX_Init(ret,flags,ptr2onExecution,data,lmsSize,maxSignals)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
// Initializes a ThreadX
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxSignals)
#else
char i_rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxSignals)
#endif
{
    //get the data and the thread's local memory stack size
    t->INH_Thread.data = data;
    t->INH_Thread.lmsSize = lmsSize;
    //get the function given by the user
    if(ptr2onExecution != 0)
        t->INH_Thread.ptr2onExecution = (void*(*)(void*))ptr2onExecution;
    else
    {
        LOG_ERROR("Passed a null pointer for the thread's execution. The thread will be doing nothing, so it is meaningless",RE_THREAD_NULL_EXECUTION_FUNCTION);
        return false;
    }

    //Initializing ThreadX data here
    t->msgIndex = 0;
    t->maxSignals = maxSignals;
    RF_MALLOC(t->msgQueue,sizeof(RF_SignalThread)*t->maxSignals);
    rfMutex_Init(&t->lock,RF_MUTEX_NORMAL);
    //End of ThreadX data initialization


    //flags processing should happen here
    t->INH_Thread.flags = 0;
    t->INH_Thread.flags|= RF_THRATT_THREADX;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    //joinable or detached threads
    if(RF_BITFLAG_ON(flags,RF_THREAD_DETACHED))
    {
        pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);
    }
    else
    {
        pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_JOINABLE);
    }

    //create the thread
    if(pthread_create( &t->INH_Thread.tHandle, &attributes, RF_THREADX_FUNCTION, t) != 0)
    {
        LOG_ERROR("Error during POSIX thread creation",RE_THREAD_CREATION);
        return false;
    }
    //cleanup the attributes
    pthread_attr_destroy(&attributes);

    //success
    return true;
}

// Destroys an RF_ThreadX pointer
void rfThreadX_Destroy(RF_ThreadX* t)
{
    //exit the thread
    pthread_exit(&t->INH_Thread.tHandle);
    //destroy the mutex lock
    rfMutex_Deinit(&t->lock);
    //free anything the message queue might have left
    uint32_t i;
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);

    //also free the thread's memory
    free(t);
}

// Deinitializes an RF_ThreadX
void rfThreadX_Deinit(RF_ThreadX* t)
{
    //exit the thread
    pthread_exit(&t->INH_Thread.tHandle);
    //destroy the mutex lock
    rfMutex_Deinit(&t->lock);
    //free anything the message queue might have left
    uint32_t i;
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);
}

// Kills a thread. Same functionality as Deinit but wraps a pthread_kill function call too
char rfThreadX_Kill(RF_ThreadX* t)
{
    int32_t err = pthread_cancel(t->INH_Thread.tHandle);
    int32_t ret;
    switch(err)
    {
        case ESRCH:
            LOG_ERROR("No thread could be found with the given ID to kill",RE_THREAD_KILL)
            ret = false;
        break;
        case 0:
            ret = true;
        break;
        default:
            LOG_ERROR("pthread_cancel returned error %d",RE_THREAD_KILL,err)
            ret = false;
        break;
    }

    //error
    pthread_exit(&t->INH_Thread.tHandle);
    //destroy the mutex lock
    rfMutex_Deinit(&t->lock);
    //free anything the message queue might have left
    uint32_t i;
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);
    return ret;
}


// Attempts to send a signal from the current thread to the target RF_ThreadX. If the message queue is currently being used then RF_THREADX_QUEUE_BUSY is returned
int32_t rfThreadX_TrySendSignal(RF_ThreadX* t,void* data,uint32_t size)
{
    //attempt to access the data
    int32_t code = rfMutex_TryLock(&t->lock);
    //check for errors
    if(code == RE_MUTEX_BUSY)
        return RF_THREADX_QUEUE_BUSY;
    if(code != RF_SUCCESS)
        return RF_THREADX_QUEUE_ERROR;

    //check if the queue is full
    if(t->msgIndex == t->maxSignals)
    {
        rfMutex_Unlock(&t->lock);
        return RF_THREADX_QUEUE_FULL;
    }

    //allocate the data for this signal
    RF_MALLOC(t->msgQueue[t->msgIndex].data,size)
    //copy the data
    memcpy(t->msgQueue[t->msgIndex].data,data,size);
    //save the size in the signal
    t->msgQueue[t->msgIndex].size = size;
    //increase the message index
    t->msgIndex++;
    //finally unlock the queue and return success
    rfMutex_Unlock(&t->lock);
    return RF_SUCCESS;
}

// Attempts to send a signal from the current thread to the target RF_ThreadX. If the message queue is currently being used the thrad will hang and wait until it's unlocked and its turn comes
int32_t rfThreadX_SendSignal(RF_ThreadX* t,void* data,uint32_t size)
{
    //attempt to access the data
    if(rfMutex_Lock(&t->lock) != RF_SUCCESS)
        return RF_THREADX_QUEUE_ERROR;

    //check if the queue is full
    if(t->msgIndex == t->maxSignals)
    {
        rfMutex_Unlock(&t->lock);
        return RF_THREADX_QUEUE_FULL;
    }


    //allocate the data for this signal
    RF_MALLOC(t->msgQueue[t->msgIndex].data,size)
    //copy the data
    memcpy(t->msgQueue[t->msgIndex].data,data,size);
    //save the size in the signal
    t->msgQueue[t->msgIndex].size = size;
    //increase the message index
    t->msgIndex++;
    //finally unlock the queue and return success
    rfMutex_Unlock(&t->lock);
    return RF_SUCCESS;
}

// Attempts to read the message queue of the thread for signals. If the message queue is currently being used the thread will hang and wait until it's unlocked and its turn comes.
int32_t rfThreadX_ReadSignal(RF_ThreadX* t,void* data,uint32_t* size)
{
   //attempt to access the data
   if(rfMutex_Lock(&t->lock) != RF_SUCCESS)
        return RF_THREADX_QUEUE_ERROR;

    //check if the queue is empty
    if(t->msgIndex == 0)
    {
        rfMutex_Unlock(&t->lock);
        return RF_THREADX_QUEUE_EMPTY;
    }

    //get the size
    *size = t->msgQueue[0].size;
    //copy the data
    memcpy(data,t->msgQueue[0].data,*size);
    //free the data from the queue
    free(t->msgQueue[0].data);
    //move the queue to make up for the lost data
    t->msgIndex--;
    memmove(t->msgQueue,t->msgQueue+1,t->msgIndex*sizeof(RF_SignalThread));
    //finally unlock message queue and return success
    rfMutex_Unlock(&t->lock);
    return RF_SUCCESS;
}

// Attempts to read the message queue of the thread for signals. If the message queue is currently being used the thread then RF_THREADX_QUEUE_BUSY is returned
int32_t rfThreadX_TryReadSignal(RF_ThreadX* t,void* data,uint32_t* size)
{
    //attempt to access the data
    int32_t code = rfMutex_TryLock(&t->lock);
    //check for errors
    if(code == RE_MUTEX_BUSY)
        return RF_THREADX_QUEUE_BUSY;
    if(code != RF_SUCCESS)
        return RF_THREADX_QUEUE_ERROR;

    //check if the queue is empty
    if(t->msgIndex == 0)
    {
        rfMutex_Unlock(&t->lock);
        return RF_THREADX_QUEUE_EMPTY;
    }

    //get the size
    *size = t->msgQueue[0].size;
    //copy the data
    memcpy(data,t->msgQueue[0].data,*size);
    //free the data from the queue
    free(t->msgQueue[0].data);
    //move the queue to make up for the lost data
    t->msgIndex--;
    memmove(t->msgQueue,t->msgQueue+1,t->msgIndex*sizeof(RF_SignalThread));
    //finally unlock message queue and return success
    rfMutex_Unlock(&t->lock);
    return RF_SUCCESS;
}
