#include <Threads/rfc_threadx.h>
//include the local memory stack
#include <rf_localmem.h>

/***************************************************************************************RF_THREADX FUNCTIONS*****************************************************************/

//The function that serves as the starting address for a threadX in win32
DWORD WINAPI RF_THREADX_FUNCTION(LPVOID  t)
{
    DWORD ret;//the return value
    //the void pointer is actually a thread
    RF_ThreadX* thread = (RF_ThreadX*)t;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,thread->INH_Thread.lmsSize);
    //in RF_ThreadX the parameter to the main thread function is a pointer to the thread itself
    ret = (DWORD) thread->INH_Thread.ptr2onExecution(thread);
    //free the local memory stack and return
    free(lms.stack);
    return ret;
}

// Allocates and returns a ThreadX
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_ThreadX* rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxMsgQueue)
#else
RF_ThreadX* i_rfThreadX_Create(uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data,uint64_t lmsSize,uint32_t maxMsgQueue)
#endif
{
    RF_ThreadX* ret;
    RF_MALLOC(ret,sizeof(RF_ThreadX));
    if( rfThreadX_Init(ret,flags,ptr2onExecution,data,lmsSize,maxMsgQueue)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
// Initializes a ThreadX
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data ,uint64_t lmsSize,uint32_t maxMsgQueue)
#else
char i_rfThreadX_Init(RF_ThreadX* t,uint32_t flags,void* (*ptr2onExecution)(RF_ThreadX*),void* data ,uint64_t lmsSize,uint32_t maxMsgQueue)
#endif
{
    //get the data and the lms size
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

    //this is an RF_ThreadX so
    t->INH_Thread.flags = flags;
    t->INH_Thread.flags|= RF_THRATT_THREADX;

    //passing "this" as a parameter to the function because we are using it in the thread's running life
    t->INH_Thread.tHandle = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) RF_THREADX_FUNCTION,(LPVOID) t,0, NULL);

    if(!t->INH_Thread.tHandle)
    {
        LOG_ERROR("CreateThread() in Windows failed. Failed to initialize Thread",RE_THREAD_CREATION);
        return false;
    }
    ///Initializing ThreadX data here
    t->msgIndex = 0;
    t->maxSignals = maxMsgQueue;
    RF_MALLOC(t->msgQueue,sizeof(RF_SignalThread)*t->maxSignals);
    rfCriticalSection_Init(&t->lock);

    //success
    return true;
}

// Destroys an RF_ThreadX pointer
void rfThreadX_Destroy(RF_ThreadX* t)
{
    //also free the handle
    CloseHandle(t->INH_Thread.tHandle);
    //destroy the Critical section
    rfCriticalSection_Deinit(&t->lock);
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
    //also free the handle
    CloseHandle(t->INH_Thread.tHandle);
    //destroy the Critical section
    rfCriticalSection_Deinit(&t->lock);
    //free anything the message queue might have left
    uint32_t i;
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);
}

// Frees a thread but also immediately causes it to exit.
char rfThreadX_Kill(RF_ThreadX* t)
{
    //destroy the thread
    int32_t ret = TerminateThread(t->INH_Thread.tHandle,1); //1 is the exit code, @todo make it so that I can input my own exit codes
    //also free the handle
    CloseHandle(t->INH_Thread.tHandle);
    //destroy the Critical section
    rfCriticalSection_Deinit(&t->lock);
    //free anything the message queue might have left
    uint32_t i;
    for(i=0;i<t->msgIndex;i++)
    {
        free(t->msgQueue[i].data);
    }
    //free the queue itself
    free(t->msgQueue);

    //return
    if(ret != 0)
        return true;
    return false;
}

// Attempts to send a signal from the current thread to the target RF_ThreadX. If the message queue is currently being used then RF_THREADX_QUEUE_BUSY is returned
int32_t rfThreadX_TrySendSignal(RF_ThreadX* t,void* data,uint32_t size)
{
    //attempt to access the data
    if(rfCriticalSection_Try(&t->lock) == false)
        return RF_THREADX_QUEUE_BUSY;

    //check if the queue is full
    if(t->msgIndex == t->maxSignals)
    {
        rfCriticalSection_Leave(&t->lock);
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
    rfCriticalSection_Leave(&t->lock);
    return RF_SUCCESS;
}

// Attempts to send a signal from the current thread to the target RF_ThreadX. If the message queue is currently being used the thrad will hang and wait until it's unlocked and its turn comes
int32_t rfThreadX_SendSignal(RF_ThreadX* t,void* data,uint32_t size)
{
    //attempt to access the data
    rfCriticalSection_Enter(&t->lock);

    //check if the queue is full
    if(t->msgIndex == t->maxSignals)
    {
        rfCriticalSection_Leave(&t->lock);
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
    rfCriticalSection_Leave(&t->lock);
    return RF_SUCCESS;
}

// Attempts to read the message queue of the thread for signals. If the message queue is currently being used the thread will hang and wait until it's unlocked and its turn comes.
int32_t rfThreadX_ReadSignal(RF_ThreadX* t,void* data,uint32_t* size)
{
    //attempt to access the data
    rfCriticalSection_Enter(&t->lock);

    //check if the queue is empty
    if(t->msgIndex == 0)
    {
        rfCriticalSection_Leave(&t->lock);
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
    rfCriticalSection_Leave(&t->lock);
    return RF_SUCCESS;
}

// Attempts to read the message queue of the thread for signals. If the message queue is currently being used the thread then RF_THREADX_QUEUE_BUSY is returned
int32_t rfThreadX_TryReadSignal(RF_ThreadX* t,void* data,uint32_t* size)
{
    //attempt to access the data
    if(rfCriticalSection_Try(&t->lock) == false)
    {
        return RF_THREADX_QUEUE_BUSY;
    }

    //check if the queue is empty
    if(t->msgIndex == 0)
    {
        rfCriticalSection_Leave(&t->lock);
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
    rfCriticalSection_Leave(&t->lock);
    return RF_SUCCESS;
}

