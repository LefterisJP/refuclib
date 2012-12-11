/*
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
*/
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <pthread.h> //for pthread_mutex_t
#include <Threads/mutex_decl.h> //for RF_Mutex
#include <Threads/mutex.h>
//*---------------------Outside module inclusion----------------------------------------
#include <Utils/bits.h> //for RF_BITFLAG_ON()
#include <Definitions/retcodes.h> //for return codes
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h>
//*----------------------------End of Includes------------------------------------------

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
                RETURN_LOG_ERROR("Failed to initialize a posix mutex object because the system lacked the necessary resources (other than memory) to initialize another mutex",
                          RE_INSUFFICIENT_RESOURCES)
            break;
            case ENOMEM:
                RETURN_LOG_ERROR("Failed to initialize a posix mutex object because of insufficient memory",RE_INSUFFICIENT_MEMORY)
            break;
            case EPERM:
                RETURN_LOG_ERROR("Failed to initialize a posix mutex object because The caller does not have the privilege to perform the operation",RE_PERMISSION)
            break;
            case EBUSY:
                RETURN_LOG_ERROR("Failed to initialize a posix mutex object because the implementation detected that argument mutex has already been initialized and not destroyed",
                          RE_MUTEX_INVALID)
            break;
            case EINVAL:
                RETURN_LOG_ERROR("Failed to initialize a posix mutex object because the flags/attributes passed to the function are invalid",RE_MUTEX_INVALID)
            break;
            default:
                RETURN_LOG_ERROR("Failed to initialize a posix mutex object with error code %d",RE_MUTEX_INIT,error)
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
                RETURN_LOG_ERROR("Error during locking a posix mutex because the calling thread's priority is higher than the mutex's current\
                          priority ceiling or due to invalid argument",RE_MUTEX_LOCK_INVALID)
            break;
            case EAGAIN:
                RETURN_LOG_ERROR("Error during locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.",RE_MUTEX_LOCK_NUM)
            break;
            case EDEADLK:
                RETURN_LOG_ERROR("Error during locking a posix mutex because the current thread already owns the mutex",RE_MUTEX_DEADLOCK)
            break;
            default:
                RETURN_LOG_ERROR("Unknonwn Error during locking a posix mutex. Error code: %d",RE_MUTEX_LOCK,error)
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
        RETURN_LOG_ERROR("Getting the system time failed. Can't perform a posix mutex timedwait. Returning error",RE_MUTEX_LOCK)

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
                RETURN_LOG_ERROR("Error during timed locking a posix mutex because the calling thread's priority is higher\
                          than the mutex's current priority ceiling or due to invalid argument",RE_MUTEX_LOCK_INVALID)
            break;
            case EAGAIN:
                RETURN_LOG_ERROR("Error during timed locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.",RE_MUTEX_LOCK_NUM)
            break;
            case EDEADLK:
                RETURN_LOG_ERROR("Error during timed locking a posix mutex because the current thread already owns the mutex",RE_MUTEX_DEADLOCK)
            break;
            default:
                RETURN_LOG_ERROR("Error during timed locking a posix mutex. Error code: %d",RE_MUTEX_LOCK,error)
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
            RETURN_LOG_ERROR("Error during try locking a posix mutex because the calling thread's priority is higher than the\
                      mutex's current priority ceiling or due to invalid argument",RE_MUTEX_LOCK_INVALID)
        break;
        //already locked
        case EBUSY:
            return RE_MUTEX_BUSY;
        break;
        case EAGAIN:
            RETURN_LOG_ERROR("Error during try locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.",RE_MUTEX_LOCK_NUM)
        break;
        case EDEADLK:
            RETURN_LOG_ERROR("Error during try locking a posix mutex because the current thread already owns the mutex",RE_MUTEX_DEADLOCK)
        break;
        default:
            RETURN_LOG_ERROR("Error during try locking a posix mutex. Error code: %d",RE_MUTEX_LOCK,error)
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
        RETURN_LOG_ERROR("Error during attempting to unlock a pthread_mutex. Error code: %d",RE_MUTEX_UNLOCK,error)

    //success
    return RF_SUCCESS;
}
