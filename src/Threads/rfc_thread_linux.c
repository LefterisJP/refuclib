/**
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include <Threads/rfc_thread.h>

#include <rf_options.h>
#ifdef RF_MODULE_THREADX//module check
    #include <Threads/rfc_threadx.h>
#endif
#include <rf_utils.h>

#include <time.h>//for the time functions
#include <stdlib.h>//for malloc e.t.c.
#include <string.h> //for memcpy e.t.c.

//include the local memory stack
#include <rf_localmem.h>


//  The function that serves as the starting address for a RF_Thread in Linux
void* RF_THREAD_FUNCTION(void* param)
{
    void* ret;//the return value
    //the parameter is actually a pointer to the thread. Get it
    RF_Thread* t = (RF_Thread*)param;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,t->lmsSize);
    //run the function
    ret = t->ptr2onExecution(t->data);
    //free the Local memory stack and return
    free(lms.stack);
    return ret;
}

//Allocates and returns a thread
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_Thread* rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize )
#else
RF_Thread* i_rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize )
#endif
{
    RF_Thread* ret;
    RF_MALLOC(ret,sizeof(RF_Thread));
    //initialize the thread
    if(rfThread_Init(ret,flags,ptr2onExecution,data,lmsSize) == false)
    {
        free(ret);
        return 0;
    }
    //return
    return ret;
}
//Initialises a thread
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize )
#else
char i_rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize )
#endif
{
    //get the data and the lms size
    t->data = data;
    t->lmsSize = lmsSize;
    //get the function given by the user
    if(ptr2onExecution != 0)
        t->ptr2onExecution = ptr2onExecution;
    else
    {
        LOG_ERROR("Passed a null pointer for the thread's execution. The thread will be doing nothing, so it is meaningless",RE_THREAD_NULL_EXECUTION_FUNCTION);
        return false;
    }

    //flags processing should happen here
    t->flags = 0;
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
    if(pthread_create( &t->tHandle, &attributes, RF_THREAD_FUNCTION,data) != 0)
    {
        LOG_ERROR("Error during POSIX thread creation",RE_THREAD_CREATION);
        return false;
    }
    //cleanup the attributes
    pthread_attr_destroy(&attributes);
    //success
    return true;
}

void rfThread_Destroy(RF_Thread* t)
{
    pthread_exit(&t->tHandle);
    //also free the thread's memory
    free(t);

}
//Deinitialises a thread
void rfThread_Deinit(RF_Thread* t)
{
    //cast the void* to an RF_Thread
    pthread_exit(&t->tHandle);

}

// Kills a thread. Same functionality as Deinit but wraps a pthread_kill function call too
char rfThread_Kill(RF_Thread* t)
{
    int32_t err = pthread_cancel(t->tHandle);
    switch(err)
    {
        case ESRCH:
            LOG_ERROR("No thread could be found with the given ID to kill",RE_THREAD_KILL)
        break;
        case 0:
            //all okay
            pthread_exit(&t->tHandle);
            return true;
        break;
        default:
            LOG_ERROR("pthread_cancel returned error %d",RE_THREAD_KILL,err)
        break;
    }

    //error
    pthread_exit(&t->tHandle);
    return false;

}

//Suspends execution of the calling thread until the target thread has terminated. If the target thread is detached this fails.
int32_t rfThread_Join(void* thread)
{
    //cast the void* to an RF_Thread
    RF_Thread* t = (RF_Thread*)thread;
    int err = pthread_join(t->tHandle,NULL);
    if(err != 0)
    {
        switch(err)
        {
            case EINVAL:
                LOG_ERROR("pthread_join failed due to the thread value not being joinable",RE_THREAD_NOTJOINABLE);
                return RE_THREAD_NOTJOINABLE;
            break;
            case ESRCH:
                LOG_ERROR("pthread_join failed due to the thread id not corresponding to any existing thread",RE_THREAD_INVALID);
                return RE_THREAD_INVALID;
            break;
            case EDEADLK:
                LOG_ERROR("pthread_join failed due to a deadlock being detected",RE_THREAD_DEADLOCK);
                return RE_THREAD_DEADLOCK;
            break;
            default:
                LOG_ERROR("pthread_join failed with unknonwn error code: %d",RE_THREAD_JOIN,err);
                return RE_THREAD_JOIN;
            break;
        }
    }
    //else
    return RF_SUCCESS;
}


#ifdef RF_MODULE_THREADX //compile only if the module is requested
/***************************************************************************************RF_THREADX FUNCTIONS*****************************************************************/
//  The function that serves as the starting address for a RF_ThreadX in Linux
void* RF_THREADX_FUNCTION(void* param)
{
    void* ret;//the return value
    //get a pointer to the thread
    RF_ThreadX* t = (RF_ThreadX*)param;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,t->lmsSize);
    //run the thread function
    ret = t->ptr2onExecution(t);
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
    t->data = data;
    t->lmsSize = lmsSize;
    //get the function given by the user
    if(ptr2onExecution != 0)
        t->ptr2onExecution = (void*(*)(void*))ptr2onExecution;
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
    t->flags = 0;
    t->flags|= RF_THRATT_THREADX;
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
    if(pthread_create( &t->tHandle, &attributes, RF_THREADX_FUNCTION, t) != 0)
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
    pthread_exit(&t->tHandle);
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
    pthread_exit(&t->tHandle);
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
    int32_t err = pthread_cancel(t->tHandle);
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
    pthread_exit(&t->tHandle);
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
#endif//end of the RF_ThreadX module check
/***************************************************************************************RF_MUTEX FUNCTIONS*****************************************************************/

// Allocates and returns a mutex object
RF_Mutex* rfMutex_Create(uint32_t flags)
{
    RF_Mutex* ret;
    RF_MALLOC(ret,sizeof(RF_Mutex));
    if( rfMutex_Init(ret,flags) != RF_SUCCESS)
    {
        free(ret);
        return 0;
    }
    //success
    return ret;
}
// Initializes a mutex object
int32_t rfMutex_Init(RF_Mutex* m,uint32_t flags)
{
    //get attributes
    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);

    //check for pthread attributes
    if(RF_BITFLAG_ON(flags,RF_MUTEX_NORMAL))
    {
        pthread_mutexattr_settype(&attributes,PTHREAD_MUTEX_NORMAL);
    }
    if(RF_BITFLAG_ON(flags,RF_MUTEX_RECURSIVE))
    {
        pthread_mutexattr_settype(&attributes,PTHREAD_MUTEX_RECURSIVE);
    }
    else if(RF_BITFLAG_ON(flags,RF_MUTEX_ERRORCHECK))
    {
        pthread_mutexattr_settype(&attributes,PTHREAD_MUTEX_ERRORCHECK);
    }

    //initialize the mutex
    int32_t error  = pthread_mutex_init(&m->mutexObj,&attributes);
    if( error != 0)
    {
        switch(error)
        {
            case EAGAIN:
                LOG_ERROR("Failed to initialize a posix mutex object because the system lacked the necessary resources (other than memory) to initialize another mutex",
                          RE_INSUFFICIENT_RESOURCES);
                return RE_INSUFFICIENT_RESOURCES;
            break;
            case ENOMEM:
                LOG_ERROR("Failed to initialize a posix mutex object because of insufficient memory",RE_INSUFFICIENT_MEMORY);
                return RE_INSUFFICIENT_MEMORY;
            break;
            case EPERM:
                LOG_ERROR("Failed to initialize a posix mutex object because The caller does not have the privilege to perform the operation",RE_PERMISSION);
                return RE_PERMISSION;
            break;
            case EBUSY:
                LOG_ERROR("Failed to initialize a posix mutex object because the implementation detected that argument mutex has already been initialized and not destroyed",
                          RE_MUTEX_INVALID);
                return RE_MUTEX_INVALID;
            break;
            case EINVAL:
                LOG_ERROR("Failed to initialize a posix mutex object because the flags/attributes passed to the function are invalid",RE_MUTEX_INVALID);
                return RE_MUTEX_INVALID;
            break;
            default:
                LOG_ERROR("Failed to initialize a posix mutex object with error code %d",RE_MUTEX_INIT,error);
                return RE_MUTEX_INIT;
            break;
        }
    }

    //free the pthread_mutex_attributes
    pthread_mutexattr_destroy(&attributes);
    //finally if we want it owned by the caller also attempt to lock it
    if(RF_BITFLAG_ON(flags,RF_MUTEX_LOCK_AT_INIT))
    {
        rfMutex_Lock(m);
    }

    //success
    return RF_SUCCESS;
}

// Destroys the mutex and releases its memory.
char rfMutex_Destroy(RF_Mutex* m)
{
   int32_t error = pthread_mutex_destroy(&m->mutexObj);
   if(error != 0)
   {
       switch(error)
       {
           case EBUSY:
                LOG_ERROR("Destroying a posix mutex failed because it is already locked or referenced by another thread",RE_MUTEX_DESTRUCTION);
           break;
           case EINVAL:
                LOG_ERROR("Destroying a posix mutex failed because the value given to the function is not a valide mutex value",RE_MUTEX_DESTRUCTION);
           break;
           default:
                LOG_ERROR("Destroying a posix mutex failed with error code: %d",RE_MUTEX_DESTRUCTION,error);
           break;
       }
       return false;
   }
   free(m);
   return true;
}
// Deinitializes the mutex
char rfMutex_Deinit(RF_Mutex* m)
{
   int32_t error = pthread_mutex_destroy(&m->mutexObj);
   if(error != 0)
   {
       switch(error)
       {
           case EBUSY:
                LOG_ERROR("Destroying a posix mutex failed because it is already locked or referenced by another thread",RE_MUTEX_DESTRUCTION);
           break;
           case EINVAL:
                LOG_ERROR("Destroying a posix mutex failed because the value given to the function is not a valide mutex value",RE_MUTEX_DESTRUCTION);
           break;
           default:
                LOG_ERROR("Destroying a posix mutex failed with error code: %d",RE_MUTEX_DESTRUCTION,error);
           break;
       }
       return false;
   }
   return true;
}


//Waits in order to acquire ownership of a mutex
int32_t rfMutex_Lock(RF_Mutex* m)
{
   int error = pthread_mutex_lock(&m->mutexObj);
   if(error != 0)
   {
       switch(error)
       {
            break;
            case EINVAL:
                LOG_ERROR("Error during locking a posix mutex because the calling thread's priority is higher than the mutex's current\
                          priority ceiling or due to invalid argument",RE_MUTEX_LOCK_INVALID);
                return RE_MUTEX_LOCK_INVALID;
            break;
            case EAGAIN:
                LOG_ERROR("Error during locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.",RE_MUTEX_LOCK_NUM);
                return RE_MUTEX_LOCK_NUM;
            break;
            case EDEADLK:
                LOG_ERROR("Error during locking a posix mutex because the current thread already owns the mutex",RE_MUTEX_DEADLOCK);
                return RE_MUTEX_DEADLOCK;
            break;
            default:
                LOG_ERROR("Unknonwn Error during locking a posix mutex. Error code: %d",RE_MUTEX_LOCK,error);
                return RE_MUTEX_LOCK;
            break;
       }
   }
   //success
   return RF_SUCCESS;
}

// Locks the mutex in the same fashion as the rfMutex_Lock function except that if the mutex is already locked then the calling thread is suspended only for the
// amount of time given as the argument to this function
int32_t rfMutex_TimedLock(RF_Mutex* m,uint32_t ms)
{
    int error;
    //let's create the appropriate time structure
    struct timespec ts;
    if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        LOG_ERROR("Getting the system time failed. Can't perform a posix mutex timedwait. Returning error",RE_MUTEX_LOCK);
        return RE_MUTEX_LOCK;
    }
    //get the seconds
    time_t secs = ms/1000;
    //get the nano seconds
    uint32_t nanoSecs = (ms%1000)*1000000;
    //increase the time
    ts.tv_sec +=  secs;
    ts.tv_nsec += nanoSecs;
    if( (error = pthread_mutex_timedlock(&m->mutexObj,&ts)) != 0)
    {
        switch(error)
        {
            //timeout
            case ETIMEDOUT:
                return RE_MUTEX_TIMEOUT;
            break;
            case EINVAL:
                LOG_ERROR("Error during timed locking a posix mutex because the calling thread's priority is higher\
                          than the mutex's current priority ceiling or due to invalid argument",RE_MUTEX_LOCK_INVALID);
                return RE_MUTEX_LOCK_INVALID;
            break;
            case EAGAIN:
                LOG_ERROR("Error during timed locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.",RE_MUTEX_LOCK_NUM);
                return RE_MUTEX_LOCK_NUM;
            break;
            case EDEADLK:
                LOG_ERROR("Error during timed locking a posix mutex because the current thread already owns the mutex",RE_MUTEX_DEADLOCK);
                return RE_MUTEX_DEADLOCK;
            break;
            default:
                LOG_ERROR("Error during timed locking a posix mutex. Error code: %d",RE_MUTEX_LOCK,error);
                return RE_MUTEX_LOCK;
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Tries to Lock the mutex. If the mutex is already locked by another thread then an error code is returned.
int32_t rfMutex_TryLock(RF_Mutex* m)
{
   int32_t error;
   if( (error = pthread_mutex_trylock(&m->mutexObj)) != 0)
   switch(error)
   {
        case EINVAL:
            LOG_ERROR("Error during try locking a posix mutex because the calling thread's priority is higher than the\
                      mutex's current priority ceiling or due to invalid argument",RE_MUTEX_LOCK_INVALID);
            return RE_MUTEX_LOCK_INVALID;
        break;
        //already locked
        case EBUSY:
            return RE_MUTEX_BUSY;
        break;
        case EAGAIN:
            LOG_ERROR("Error during try locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.",RE_MUTEX_LOCK_NUM);
            return RE_MUTEX_LOCK_NUM;
        break;
        case EDEADLK:
            LOG_ERROR("Error during try locking a posix mutex because the current thread already owns the mutex",RE_MUTEX_DEADLOCK);
            return RE_MUTEX_DEADLOCK;
        break;
        default:
            LOG_ERROR("Error during try locking a posix mutex. Error code: %d",RE_MUTEX_LOCK,error);
            return RE_MUTEX_LOCK;
        break;
   }
   //succcess
   return RF_SUCCESS;
}

// Releases ownership of a mutex object
int32_t rfMutex_Unlock(RF_Mutex* m)
{
    int32_t error;
    error = pthread_mutex_unlock(&m->mutexObj);
    if(error != 0 )
    {
        LOG_ERROR("Error during attempting to unlock a pthread_mutex. Error code: %d",RE_MUTEX_UNLOCK,error);
        return RE_MUTEX_UNLOCK;
    }
    //success
    return RF_SUCCESS;
}

/***************************************************************************************RF_SEMAPHORE FUNCTIONS*****************************************************************/

// Allocates and returns a semaphore object
RF_Semaphore* rfSemaphore_Create(uint32_t initialCount,uint32_t maxCount)
{
    RF_Semaphore* ret;
    RF_MALLOC(ret,sizeof(RF_Semaphore));
    if(rfSemaphore_Init(ret,initialCount,maxCount) != RF_SUCCESS)
    {
        free(ret);
        return 0;
    }
    //success
    return ret;
}
// Initializes a semaphore object. The maxcount argument is kepts for windows compatibility. Makes sesne only in windows
int32_t rfSemaphore_Init(RF_Semaphore* s,uint32_t initialCount,uint32_t maxCount)
{
    if(sem_init(&s->semaphoreObj,0,initialCount)!= 0)
    {
        switch(errno)
        {
            case EINVAL:
                LOG_ERROR("Failed to initialize a posix semaphore because the given initial count of %du\
                          exceeds the maximum semaphore count for the system",RE_SEMAPHORE_MAXCOUNT,initialCount);
                return RE_SEMAPHORE_MAXCOUNT;
            break;
            case ENOSPC:
                LOG_ERROR("Failed to initialize a posix semaphore because a resource required to initialise the semaphore has been exhausted, \
                          or the limit on semaphores (SEM_NSEMS_MAX) of the system has been reached",RE_INSUFFICIENT_RESOURCES);
                return RE_INSUFFICIENT_RESOURCES;
            break;
            case EPERM:
                LOG_ERROR("Failed to initialize a posix semaphore because the process lacks the appropriate\
                          privileges to initialise the semaphore. ",RE_PERMISSION);
                return RE_PERMISSION;
            break;
            case ENOSYS:
                LOG_ERROR("Failed to initialize a posix semaphore because the function sem_init is not supported in this system",RE_UNSUPPORTED);
                return RE_UNSUPPORTED;
            break;
            default:
                 LOG_ERROR("Failed to initialize a posix semaphore with error code: %d",RE_SEMAPHORE_INIT,errno);
                 return RE_SEMAPHORE_INIT;
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Destroys a semaphore object and frees its memory. @warning Be careful not to use this if the semaphore is still being waited for by any other active thread
char rfSemaphore_Destroy(RF_Semaphore* s)
{
    if(sem_destroy(&s->semaphoreObj) != 0)
    {
        switch(errno)
        {
            case EINVAL:
                LOG_ERROR("Failure during destroying a posix semaphore, because the given argument is not a valid semaphore",RE_SEMAPHORE_DESTRUCTION);
            break;
            case ENOSYS:
                LOG_ERROR("Failure during destroying a posix semaphore, because the function sem_destroy is not supported in this system",RE_SEMAPHORE_DESTRUCTION);
            break;
            case EBUSY:
                LOG_ERROR("Failure during destroying a posix semaphore, because there are currently processes blocked on the semaphore",RE_SEMAPHORE_DESTRUCTION);
            break;
            default:
                LOG_ERROR("Failure during destroying a posix semaphore with error code: %d",RE_SEMAPHORE_DESTRUCTION,errno);
            break;
        }
        return false;
    }
    free(s);
    return true;
}
// Deinitializes a semaphore
char rfSemaphore_Deinit(RF_Semaphore* s)
{
    if(sem_destroy(&s->semaphoreObj) != 0)
    {
        switch(errno)
        {
            case EINVAL:
                LOG_ERROR("Failure during destroying a posix semaphore, because the given argument is not a valid semaphore",RE_SEMAPHORE_DESTRUCTION);
            break;
            case ENOSYS:
                LOG_ERROR("Failure during destroying a posix semaphore, because the function sem_destroy is not supported in this system",RE_SEMAPHORE_DESTRUCTION);
            break;
            case EBUSY:
                LOG_ERROR("Failure during destroying a posix semaphore, because there are currently processes blocked on the semaphore",RE_SEMAPHORE_DESTRUCTION);
            break;
            default:
                LOG_ERROR("Failure during destroying a posix semaphore with error code: %d",RE_SEMAPHORE_DESTRUCTION,errno);
            break;
        }
        return false;
    }
    return true;
}

// Waits for the semaphore. If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function hangs and waits
int32_t rfSemaphore_Wait(RF_Semaphore* s)
{
    int32_t ret = sem_wait(&s->semaphoreObj);
    if(ret != 0)
    {
        switch(ret)
        {
            case EINVAL:
                LOG_ERROR("Failure during waiting for a posix semaphore because the semaphore argument is invalid",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            case ENOSYS:
                LOG_ERROR("Failure during waiting for a posix semaphore because the function sem_wait is not supported in this system",RE_UNSUPPORTED);
                return RE_UNSUPPORTED;
            break;
            case EDEADLK:
                LOG_ERROR("Failure during waiting for a posix semaphore because a deadlock condition was detected",RE_SEMAPHORE_DEADLOCK);
                return RE_SEMAPHORE_DEADLOCK;
            break;
            case EINTR:
                LOG_ERROR("Failure during waiting for a posix semaphore because of a signal interruption",RE_INTERRUPT);
                return RE_INTERRUPT;
            break;
            default:
                LOG_ERROR("Failure during waiting for a posix semaphore with error code: %d",RE_SEMAPHORE_WAIT,ret);
                return RE_SEMAPHORE_WAIT;
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Same as the rfSemaphore_Wait function except that there is a timeout to waiting given as an argument
int32_t rfSemaphore_TimedWait(RF_Semaphore* s,uint32_t ms)
{
    //let's create the appropriate time structure
    struct timespec ts;
    if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        LOG_ERROR("Getting the system time failed. Can't perform a posix semaphore timedwait. Returning error",RE_SEMAPHORE_WAIT);
        return RE_SEMAPHORE_WAIT;
    }
    //get the seconds
    time_t secs = ms/1000;
    //get the nano seconds
    uint32_t nanoSecs = (ms%1000)*1000000;
    //increase the time
    ts.tv_sec +=  secs;
    ts.tv_nsec += nanoSecs;
    //perform the timed wait
    int32_t ret = sem_timedwait(&s->semaphoreObj,&ts);
    if(ret != 0)
    {
        switch(ret)
        {
            //waiting timed out
            case ETIMEDOUT:
                return RE_SEMAPHORE_TIMEOUT;
            break;
            case EINVAL:
                LOG_ERROR("Failure during waiting for a posix semaphore because the semaphore argument is invalid",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            case ENOSYS:
                LOG_ERROR("Failure during waiting for a posix semaphore because the function sem_timedwait is not supported in this system",RE_UNSUPPORTED);
                return RE_UNSUPPORTED;
            break;
            case EDEADLK:
                LOG_ERROR("Failure during waiting for a posix semaphore because a deadlock condition was detected",RE_SEMAPHORE_DEADLOCK);
                return RE_SEMAPHORE_DEADLOCK;
            break;
            case EINTR:
                LOG_ERROR("Failure during waiting for a posix semaphore because of a signal interruption",RE_INTERRUPT);
                return RE_INTERRUPT;
            break;
            default:
                LOG_ERROR("Failure during waiting for a posix semaphore with error code: %d",RE_SEMAPHORE_WAIT,ret);
            break;
        }
    }//if closes
    //success
    return RF_SUCCESS;
}

// Tries to wait for the semaphore. If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function returns RF_SEMAPHORE_BUSY
int32_t rfSemaphore_TryWait(RF_Semaphore* s)
{
    int32_t ret = sem_trywait(&s->semaphoreObj);
    if(ret != 0)
    {
        switch(ret)
        {
            //means that the semaphore is already locked by another thread
            case EAGAIN:
                return RE_SEMAPHORE_BUSY;
            break;
            case EINVAL:
                LOG_ERROR("Failure during waiting for a posix semaphore because the semaphore argument is invalid",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            case ENOSYS:
                LOG_ERROR("Failure during waiting for a posix semaphore because the function sem_wait is not supported in this system",RE_UNSUPPORTED);
                return RE_UNSUPPORTED;
            break;
            case EDEADLK:
                LOG_ERROR("Failure during waiting for a posix semaphore because a deadlock condition was detected",RE_SEMAPHORE_DEADLOCK);
                return RE_SEMAPHORE_DEADLOCK;
            break;
            case EINTR:
                LOG_ERROR("Failure during waiting for a posix semaphore because of a signal interruption",RE_INTERRUPT);
                return RE_INTERRUPT;
            break;
            default:
                LOG_ERROR("Failure during waiting for a posix semaphore with error code: %d",RE_SEMAPHORE_WAIT,ret);
                return RE_SEMAPHORE_WAIT;
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Releases ownerhship of a semaphore object, increasing its count by one
int32_t rfSemaphore_Post(RF_Semaphore* s)
{
    //attempt to post, and if fail occurs error log
    if(sem_post(&s->semaphoreObj) != 0)
    {
        switch(errno)
        {
            case EINVAL:
                LOG_ERROR("Failure during posting to a posix semaphore due to the argument not being a valid semaphore",RE_SEMAPHORE_POST_INVALID);
                return RE_SEMAPHORE_POST_INVALID;
            break;
            case ENOSYS:
                LOG_ERROR("Failure during posting to a posix semaphore because the sem_post function is not supported by the system",RE_UNSUPPORTED);
                return RE_UNSUPPORTED;
            break;
            default:
                LOG_ERROR("Failure during posting to a posix semaphore with error code: %d",RE_SEMAPHORE_POST,errno);
                return RE_SEMAPHORE_POST;
            break;
        }
    }
    //success
    return RF_SUCCESS;
}
