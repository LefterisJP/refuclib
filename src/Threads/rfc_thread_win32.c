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


#include <stdlib.h>//for malloc

#include <Threads/rfc_thread.h>

#include <rf_options.h>
#ifdef RF_MODULE_THREADX//module check
    #include <Threads/rfc_threadx.h>
#endif
#include <rf_utils.h>

//include the local memory stack
#include <rf_localmem.h>

//#include <windows.h> //for DWORD

//The function that serves as the starting address for a thread in win32
DWORD WINAPI RF_THREAD_FUNCTION(LPVOID  t)
{
    DWORD ret;//the return value
    //the void pointer is actually a thread
    RF_Thread* thread = (RF_Thread*)t;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,thread->lmsSize);
    //in RF_Thread the parameter to the main thread function is a pointer to the data passed at init
    ret = (DWORD)thread->ptr2onExecution(thread->data);
    //free the local memory stack and return
    free(lms.stack);
    return ret;
}

//Allocates and returns a thread
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_Thread* rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data ,uint64_t lmsSize)
#else
RF_Thread* i_rfThread_Create(uint32_t flags,void* (*ptr2onExecution)(void*),void* data ,uint64_t lmsSize)
#endif
{
    RF_Thread* ret;
    RF_MALLOC(ret,sizeof(RF_Thread));
    //initialize the pointer
    if( rfThread_Init(ret,flags,ptr2onExecution,data,lmsSize)== false)
    {
        free(ret);
        return 0;
    }
    //return the pointer
    return ret;
}
//Initialises a thread
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize )
#else
char i_rfThread_Init(RF_Thread* t,uint32_t flags,void* (*ptr2onExecution)(void*),void* data,uint64_t lmsSize )
#endif
{
    //get the data
    t->data = data;
    //get the function given by the user
    if(ptr2onExecution != 0)
        t->ptr2onExecution = ptr2onExecution;
    else
    {
        LOG_ERROR("Passed a null pointer for the thread's execution. The thread will be doing nothing, so it is meaningless",RE_THREAD_NULL_EXECUTION_FUNCTION);
        return false;
    }
    //initialize flags and lmsSize
    t->flags = flags;
    t->lmsSize = lmsSize;
    //passing "this" as a parameter to the function because we are using it in the thread's running life
    t->tHandle = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE) RF_THREAD_FUNCTION,(LPVOID) t,0, NULL);
    if(!t->tHandle)
    {
        LOG_ERROR("CreateThread() in Window failed. Failed to initialize Thread",RE_THREAD_CREATION);
        return false;
    }
    //success
    return true;
}

//Destroys a thread
void rfThread_Destroy(RF_Thread* t)
{
    //also free the handle
    CloseHandle(t->tHandle);
    //also free the thread's memory
    free(t);

}
//Deinitialises a thread
void rfThread_Deinit(RF_Thread* t)
{
    //also free the handle
    CloseHandle(t->tHandle);
}
// Frees a thread but also immediately causes it to exit.
char rfThread_Kill(RF_Thread* t)
{
    int32_t ret = TerminateThread(t->tHandle,1); //1 is the exit code, @todo make it so that I can input my own exit codes
    //also free the handle
    CloseHandle(t->tHandle);

    //return
    if(ret != 0)
        return true;

    RF_WIN32_GETSYSERROR(strBuff);
    LOG_ERROR("Failed due to TerminateThread failing with Windows Error(%lu):\n%s",RE_THREAD_KILL,i_ERROR_CODE,strBuff);
    LocalFree(strBuff);
    return false;
}

//Suspends execution of the calling thread until the target thread has terminated. If the target thread is detached this fails.
int32_t rfThread_Join(void* thread)
{
    //cast the void* to an RF_Thread
    RF_Thread* t = (RF_Thread*)thread;
    //check if it's joinable
    if(RF_BITFLAG_ON(t->flags,RF_THREAD_DETACHED))
    {
        LOG_ERROR("Join failed due to the thread value not being joinable",RE_THREAD_NOTJOINABLE);
        return RE_THREAD_NOTJOINABLE;
    }
    DWORD err = WaitForSingleObject(t->tHandle,INFINITE);
    if(err == WAIT_FAILED)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("WaitForSingleObject failed with Windows Error(%lu):\n%s",RE_WAITFORSINGLEOBJECT,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RE_THREAD_JOIN;
    }
    //also free the handle
    CloseHandle(t->tHandle);
    return RF_SUCCESS;
}


#ifdef RF_MODULE_THREADX //only if we include it compile it

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
#endif//end of the RF_ThreadX module check
/***************************************************************************************RF_CRITICALSECTION FUNCTIONS*****************************************************************/
// Allocates and returns a critical section
RF_CriticalSection* rfCriticalSection_Create()
{
    RF_CriticalSection* ret;
    RF_MALLOC(ret,sizeof(RF_CriticalSection));
    //initialize the OS critical section
    InitializeCriticalSection(&ret->lockObject);

    return ret;
}
// Initializes a critical section
char rfCriticalSection_Init(RF_CriticalSection* c)
{
    //initialize the OS critical section
    InitializeCriticalSection(&c->lockObject);
    //seems that this initialization can't fail in windows (?)
    return true;
}


// Destroys a critical section and frees its memory
void rfCriticalSection_Destroy(RF_CriticalSection* C)
{
    DeleteCriticalSection(&C->lockObject);
    //also free own memory
    free(C);
}
// Deinitializes a critical section
void rfCriticalSection_Deinit(RF_CriticalSection* C)
{
    DeleteCriticalSection(&C->lockObject);
}

// Enters a Critical Section. The function returns only when the calling thread is granted ownership of the section
void rfCriticalSection_Enter(RF_CriticalSection* C)
{
    EnterCriticalSection(&C->lockObject);
}

// Attempts to enter a critical section without locking the calling thread.
char rfCriticalSection_Try(RF_CriticalSection* C)
{
    return TryEnterCriticalSection(&C->lockObject);
}

//Leaves a critical Section
void rfCriticalSection_Leave(RF_CriticalSection* C)
{
    LeaveCriticalSection(&C->lockObject);
}

/***************************************************************************************RF_MUTEX FUNCTIONS*****************************************************************/

// Allocates and returns a mutex object
RF_Mutex* rfMutex_Create(uint32_t flags)
{
    RF_Mutex* ret;
    RF_MALLOC(ret,sizeof(RF_Mutex));
    if( rfMutex_Init(ret,flags)!= RF_SUCCESS)
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
    if(RF_BITFLAG_ON(flags, RF_MUTEX_LOCK_AT_INIT))
        m->mutexObj = CreateMutex( NULL,TRUE,NULL);
    else
        m->mutexObj = CreateMutex( NULL,FALSE,NULL);

    if(m->mutexObj == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("Failed to initialize a winthread mutex object with Windows Error(%lu):\n%s",RE_MUTEX_INIT,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RE_MUTEX_INIT;
    }
    //success
    return RF_SUCCESS;
}

// Destroys the mutex and releases its memory.
char rfMutex_Destroy(RF_Mutex* m)
{
   if(CloseHandle(m->mutexObj)==0)
   {
       RF_WIN32_GETSYSERROR(strBuff);
       LOG_ERROR("There was an error at destruction of a winapi mutex. Windows Error Code(%lu):\n%s",RE_MUTEX_DESTRUCTION,i_ERROR_CODE,strBuff);
       LocalFree(strBuff);
       return false;
   }
   free(m);
   return true;
}
// Deinitializes the mutex
char rfMutex_Deinit(RF_Mutex* m)
{
   if(CloseHandle(m->mutexObj)==0)
   {
       RF_WIN32_GETSYSERROR(strBuff);
       LOG_ERROR("There was an error at destruction of a winapi mutex. Windows Error code(%lu):\n%s",RE_MUTEX_DESTRUCTION,i_ERROR_CODE,strBuff);
       LocalFree(strBuff);
       return false;
   }
   return true;
}

//Waits in order to acquire ownership of a mutex
int32_t rfMutex_Lock(RF_Mutex* m)
{
   uint32_t ret = WaitForSingleObject(m->mutexObj,INFINITE);
   if(ret != WAIT_OBJECT_0)
   {
        switch(ret)
        {
            case WAIT_ABANDONED:
                LOG_ERROR("During winapi mutex locking, ownership of the parameter mutex object was given succesfully to the caller,\
                           but the mutex itself was not released properly by its previous owner. Check for incosistencies",RE_MUTEX_LOCK_INVALID);
                return RE_MUTEX_LOCK_INVALID;
            break;
            case WAIT_TIMEOUT:
                LOG_ERROR("During winapi mutex locking the timeout limit elapsed.This should never happen. Serious error!",RE_MUTEX_LOCK);
            break;
            default:
            {
                RF_WIN32_GETSYSERROR(strBuff);
                LOG_ERROR("During winapi mutex locking there was a Windows Error(%lu):\n%s",RE_MUTEX_LOCK,i_ERROR_CODE,strBuff);
                LocalFree(strBuff);
                return RE_MUTEX_LOCK;
            }
            break;
        }
   }
   //successfully waited for the semaphore
   return RF_SUCCESS;
}

// Locks the mutex in the same fashion as the rfMutex_Lock function except that if the mutex is already locked then the calling thread is suspended only for the
// amount of time given as the argument to this function
int32_t rfMutex_TimedLock(RF_Mutex* m,uint32_t ms)
{
    uint32_t ret = WaitForSingleObject(m->mutexObj,ms);
    if(ret != WAIT_OBJECT_0)
    {
        switch(ret)
        {
            case WAIT_ABANDONED:
                LOG_ERROR("During winapi mutex locking, ownership of the parameter mutex object was given succesfully to the caller,\
                           but the mutex itself was not released properly by its previous owner. Check for incosistencies",RE_MUTEX_LOCK_INVALID);
                return RE_MUTEX_LOCK_INVALID;
            break;
            //means that waiting to lock the mutex timed out
            case WAIT_TIMEOUT:
                return RE_MUTEX_TIMEOUT;
            break;
            default:
            {
                RF_WIN32_GETSYSERROR(strBuff);
                LOG_ERROR("During winapi mutex locking there was a Windows Error(%lu):\n%s",RE_MUTEX_LOCK,i_ERROR_CODE,strBuff);
                LocalFree(strBuff);
                return RE_MUTEX_LOCK;
            }
            break;
        }
    }
    //success
    return RF_SUCCESS;
}


// Tries to Lock the mutex. If the mutex is already locked by another thread then an error code is returned.
int32_t rfMutex_TryLock(RF_Mutex* m)
{
    //in windows I emulate trylock by giving a timeout to waiting of 1 ms
    uint32_t ret = WaitForSingleObject(m->mutexObj,1);
    if(ret != WAIT_OBJECT_0)
    {
        switch(ret)
        {
            case WAIT_ABANDONED:
                LOG_ERROR("During winapi mutex locking, ownership of the parameter mutex object was given succesfully to the caller,\
                           but the mutex itself was not released properly by its previous owner. Check for incosistencies",RE_MUTEX_LOCK_INVALID);
                return RE_MUTEX_INVALID;
            break;
            //means that the mutex is busy (already locked) by another thread
            case WAIT_TIMEOUT:
                return RE_MUTEX_BUSY;
            break;
            default:
            {
                RF_WIN32_GETSYSERROR(strBuff);
                LOG_ERROR("During winapi mutex locking there was a Windows Error(%lu):\n%s",RE_MUTEX_LOCK,i_ERROR_CODE,strBuff);
                LocalFree(strBuff);
                return RE_MUTEX_LOCK;
            }
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Releases ownership of a mutex object
int32_t rfMutex_Unlock(RF_Mutex* m)
{
    if(ReleaseMutex(m->mutexObj) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("Error during attempting to unlock a winthread mutex with Windows Error(%lu):\n%s",RE_MUTEX_UNLOCK,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
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
    ret->semaphoreObj =  CreateSemaphore(NULL,initialCount,maxCount,NULL);

    if(ret->semaphoreObj == 0)
    {
        LOG_ERROR("Failed to create a semaphore object",RE_SEMAPHORE_INIT);
        free(ret);
        return 0;
    }
    return ret;
}
// Initializes a semaphore object
int32_t rfSemaphore_Init(RF_Semaphore* s,uint32_t initialCount,uint32_t maxCount)
{
    s->semaphoreObj =  CreateSemaphore(NULL,initialCount,maxCount,NULL);

    if(s->semaphoreObj == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("Failed to initialize a semaphore object with Windows Error(%lu):\n%s",RE_SEMAPHORE_INIT,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RE_SEMAPHORE_INIT;
    }//else success
    return RF_SUCCESS;
}

// Destroys a semaphore object and frees its memory. @warning Be careful not to use this if the semaphore is still being waited for by any other active thread
char rfSemaphore_Destroy(RF_Semaphore* s)
{
    if(CloseHandle(s->semaphoreObj) == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("An error was encountered during destroying a winapi semaphore with Windows Error(%lu):\n%s",RE_SEMAPHORE_DESTRUCTION,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return false;
    }
    free(s);
    return true;
}
// Deinitializes a semaphore
char rfSemaphore_Deinit(RF_Semaphore* s)
{
    if(CloseHandle(s->semaphoreObj)==0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("An error was encountered during deinitializing a winapi semaphore with Windows Error(%lu):\n%s",RE_SEMAPHORE_DESTRUCTION,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return false;
    }
    return true;
}

// Waits for the semaphore. If succesfull decreases the count of the semaphore
int32_t rfSemaphore_Wait(RF_Semaphore* s)
{
    uint32_t ret = WaitForSingleObject(s->semaphoreObj,INFINITE);
    if(ret != WAIT_OBJECT_0)
    {
        switch(ret)
        {
            case WAIT_ABANDONED:
                LOG_ERROR("During winapi semaphore waiting ownership of the parameter mutex object was given succesfully to the caller,\
                           but the semaphore itself was not released properly by its previous owner. Check for incosistencies",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            case WAIT_TIMEOUT:
                LOG_ERROR("During winapi semaphore waiting the timeout limit elapsed.This should never happen. Serious error!",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            default:
            {
                RF_WIN32_GETSYSERROR(strBuff);
                LOG_ERROR("During winapi semaphore waiting there was a Windows Error(%lu):\n%s",RE_SEMAPHORE_WAIT,i_ERROR_CODE,strBuff);
                LocalFree(strBuff);
                return RE_SEMAPHORE_WAIT;
            }
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Same as the rfSemaphore_Wait function except that there is a timeout to waiting given as an argument
int32_t rfSemaphore_TimedWait(RF_Semaphore* s,uint32_t ms)
{
    uint32_t ret = WaitForSingleObject(s->semaphoreObj,ms);
    if(ret != WAIT_OBJECT_0)
    {
        switch(ret)
        {
            case WAIT_ABANDONED:
                LOG_ERROR("During winapi semaphore waiting ownership of the parameter mutex object was given succesfully to the caller,\
                           but the semaphore itself was not released properly by its previous owner. Check for incosistencies",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            //the timeout limit was reached
            case WAIT_TIMEOUT:
                return RE_SEMAPHORE_TIMEOUT;
            break;
            default:
            {
                RF_WIN32_GETSYSERROR(strBuff);
                LOG_ERROR("During winapi timed semaphore waiting there was a Windows Error(%lu):\n%s",RE_SEMAPHORE_WAIT,i_ERROR_CODE,strBuff);
                LocalFree(strBuff);
                return RE_SEMAPHORE_WAIT;
            }
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Tries to wait for the semaphore. If succesfull decreases the count of the semaphore. If the count of the semaphore is at zero then this function returns RF_SEMAPHORE_BUSY
int32_t rfSemaphore_TryWait(RF_Semaphore* s)
{
   //emulating trywait by putting a delay of 1 ms
    uint32_t ret = WaitForSingleObject(s->semaphoreObj,1);
    if(ret != WAIT_OBJECT_0)
    {
        switch(ret)
        {
            case WAIT_ABANDONED:
                LOG_ERROR("During winapi semaphore waiting ownership of the parameter mutex object was given succesfully to the caller,\
                           but the semaphore itself was not released properly by its previous owner. Check for incosistencies",RE_SEMAPHORE_WAIT_INVALID);
                return RE_SEMAPHORE_WAIT_INVALID;
            break;
            //the semaphore is busy/locked by another thread
            case WAIT_TIMEOUT:
                return RE_SEMAPHORE_BUSY;
            break;
            default:
            {
                RF_WIN32_GETSYSERROR(strBuff);
                LOG_ERROR("During winapi semaphore attempted waiting there was a Windows Error(%lu):\n%s",RE_SEMAPHORE_WAIT,i_ERROR_CODE,strBuff);
                LocalFree(strBuff);
                return RE_SEMAPHORE_WAIT;
            }
            break;
        }
    }
    //success
    return RF_SUCCESS;
}

// Releases ownerhship of a semaphore object, increasing its count by one
int32_t rfSemaphore_Post(RF_Semaphore* s)
{
    char ret = ReleaseSemaphore(s->semaphoreObj,1,NULL);
    if(ret == 0)
    {
        RF_WIN32_GETSYSERROR(strBuff);
        LOG_ERROR("During winapi semaphore releasing there was a Windows Error(%lu):\n%s",RE_SEMAPHORE_POST,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RE_SEMAPHORE_POST;
    }
    //success
    return RF_SUCCESS;
}

