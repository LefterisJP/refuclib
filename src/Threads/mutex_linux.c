/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
*/
/*------------- Corrensponding Header inclusion -------------*/
#include <Threads/mutex.h>
/*------------- Outside Module inclusion -------------*/
// for RF_BITFLAG_ON()
    #include <Utils/bits.h>
// for return codes
    #include <Definitions/retcodes.h> 
// for error logging
    #include <Utils/error.h>
// for memory allocation
    #include <Utils/memory.h> //for refu memory allocation
/*------------- End of includes -------------*/

// Allocates and returns a mutex object
RF_Mutex* rfMutex_Create(uint32_t flags)
{
    RF_Mutex* ret;
    RF_MALLOC(ret, sizeof(RF_Mutex), NULL);
    if(!rfMutex_Init(ret,flags))
    {
        free(ret);
        return NULL;
    }
    //success
    return ret;
}
// Initializes a mutex object
char rfMutex_Init(RF_Mutex* m, uint32_t flags)
{
    pthread_mutexattr_t attributes;
    int error;
     //get attributes
    if(pthread_mutexattr_init(&attributes) != 0)
    {
        //according to opengroup only ENOMEM can return so
        RF_ERROR("Failed to initialize pthread_mutexattr_t due to insufficient "
                 "memory");
        return false;
    }
    //check for pthread attributes (no error checking since valid are given by us)
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
    if((error=pthread_mutex_init(&m->mutexObj,&attributes)) != 0)
    {
        RF_ERROR("Failed to initialize a pthread mutex due to "
                 "pthread_mutex_init() errno %d", error);
        return false;
    }

    //free the pthread_mutex_attributes
    pthread_mutexattr_destroy(&attributes);
    //finally if we want it owned by the caller also attempt to lock it
    if(RF_BITFLAG_ON(flags,RF_MUTEX_LOCK_AT_INIT))
    {
        rfMutex_Lock(m);
    }
    //success
    return true;
}

// Destroys the mutex and releases its memory.
char rfMutex_Destroy(RF_Mutex* m)
{
    if(!rfMutex_Deinit(m))
    {
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
        RF_ERROR("Destroying a pthread mutex failed due to "
                 "pthread_mutex_destroy() errno %d", error);
        return false;
    }
    return true;
}


//Waits in order to acquire ownership of a mutex
char rfMutex_Lock(RF_Mutex* m)
{
   int error = pthread_mutex_lock(&m->mutexObj);
   if(error != 0)
   {
       RF_ERROR("Failed to lock mutex due to pthread_mutex_lock() "
                "errno %d", error);
       return false;

   }
   //success
   return true;
}

char rfMutex_TimedLock(RF_Mutex* m, uint32_t ms, char* expire)
{
    int error;
    struct timespec ts;

    *expire = false;
    //let's create the appropriate time structure
    if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        RF_ERROR(
                 "Getting the system time failed. Can't perform a posix mutex "
                 "timedwait");
        return false;
    }

    //get the seconds
    time_t secs = ms/1000;
    //get the nano seconds
    uint32_t nanoSecs = (ms%1000)*1000000;
    //increase the time
    ts.tv_sec +=  secs;
    ts.tv_nsec += nanoSecs;
    if((error = pthread_mutex_timedlock(&m->mutexObj,&ts)) != 0)
    {
        switch(error)
        {
            //timeout
            case ETIMEDOUT:
                *expire = true;
            break;
            case EINVAL:
                RF_ERROR(
                    0,
                    "Error during timed locking a posix muter either because "
                    "the mutex was created with the protocol attribute having "
                    "the value PTHREAD_PRIO_PROTECT and the calling thread's "
                    "priority is higher than the mutex's current priority ceiling or"
                    " the process or thread would have blocked, and the "
                    "abs_timeout parameter specified a nanoseconds field value "
                    "less than zero or greater than or equal to 1000 million or "
                    "the value specified by mutex does not refer to an "
                    "initialized mutex object");
            break;
            case EAGAIN:
                RF_ERROR(
                    0, "Error during timed locking a posix mutex because the"
                    " maximum number of recursive locks for the mutex has"
                    " been reached.");
            break;
            case EDEADLK:
                RF_ERROR("Error during timed locking a posix "
                         "mutex because the current thread "
                         "already owns the mutex or a deadlock condition"
                         "was detected");
            break;
            default:
                RF_ERROR("Error during timed locking a posix "
                         "mutex. Error code: %d",
                         error);
            break;
        }
        return false;
    }
    //success
    return true;
}


char rfMutex_TryLock(RF_Mutex* m, char* busy)
{
   int32_t error;
   *busy = false;
   error = pthread_mutex_trylock(&m->mutexObj);
   if(!error) {return true;} /* success */

   if(error == EBUSY)
   {
       *busy = true;
       return false;
   }
   RF_ERROR("Locking a pthread mutex failed with due to "
            "pthread_mutex_trylock() errno %d", error);
   return false;
}

// Releases ownership of a mutex object
char rfMutex_Unlock(RF_Mutex* m)
{
    int32_t error;
    error = pthread_mutex_unlock(&m->mutexObj);
    if(error != 0 )
    {
        RF_ERROR("Releasing ownership of a mutex failed due to "
                 "pthread_mutex_unlock() errno %d", error);
        return false;
    }
    return true;
}
