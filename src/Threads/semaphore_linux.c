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
#include <semaphore.h> //for pthread_semaphores
#include <Threads/semaphore_decl.h> //for RF_Semaphore
#include <Threads/semaphore.h>
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>
//for memory allocation
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <Utils/memory.h> //for refu memory allocation
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h>
#include <time.h> //for CLOCK_REALTIME and clock_gettime()
//*----------------------------End of Includes------------------------------------------

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
                RETURN_LOG_ERROR("Failed to initialize a posix semaphore because the given initial count of %du\
                          exceeds the maximum semaphore count for the system",RE_SEMAPHORE_MAXCOUNT,initialCount)
            break;
            case ENOSPC:
                RETURN_LOG_ERROR("Failed to initialize a posix semaphore because a resource required to initialise the semaphore has been exhausted, \
                          or the limit on semaphores (SEM_NSEMS_MAX) of the system has been reached",RE_INSUFFICIENT_RESOURCES)
            break;
            case EPERM:
                RETURN_LOG_ERROR("Failed to initialize a posix semaphore because the process lacks the appropriate\
                          privileges to initialise the semaphore. ",RE_PERMISSION)
            break;
            case ENOSYS:
                RETURN_LOG_ERROR("Failed to initialize a posix semaphore because the function sem_init is not supported in this system",RE_UNSUPPORTED)
            break;
            default:
                 RETURN_LOG_ERROR("Failed to initialize a posix semaphore with error code: %d",RE_SEMAPHORE_INIT,errno)
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
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because the semaphore argument is invalid",RE_SEMAPHORE_WAIT_INVALID)
            break;
            case ENOSYS:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because the function sem_wait is not supported in this system",RE_UNSUPPORTED)
            break;
            case EDEADLK:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because a deadlock condition was detected",RE_SEMAPHORE_DEADLOCK)
            break;
            case EINTR:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because of a signal interruption",RE_INTERRUPT)
            break;
            default:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore with error code: %d",RE_SEMAPHORE_WAIT,ret)
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
        RETURN_LOG_ERROR("Getting the system time failed. Can't perform a posix semaphore timedwait. Returning error",RE_SEMAPHORE_WAIT)

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
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because the semaphore argument is invalid",RE_SEMAPHORE_WAIT_INVALID)
            break;
            case ENOSYS:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because the function sem_timedwait is not supported in this system",RE_UNSUPPORTED)
            break;
            case EDEADLK:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because a deadlock condition was detected",RE_SEMAPHORE_DEADLOCK)
            break;
            case EINTR:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because of a signal interruption",RE_INTERRUPT)
            break;
            default:
                RETURN_LOG_ERROR("Generic Failure during waiting for a posix semaphore with error code: %d",RE_SEMAPHORE_WAIT,ret)
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
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because the semaphore argument is invalid",RE_SEMAPHORE_WAIT_INVALID)
            break;
            case ENOSYS:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because the function sem_wait is not supported in this system",RE_UNSUPPORTED)
            break;
            case EDEADLK:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because a deadlock condition was detected",RE_SEMAPHORE_DEADLOCK)
            break;
            case EINTR:
                RETURN_LOG_ERROR("Failure during waiting for a posix semaphore because of a signal interruption",RE_INTERRUPT)
            break;
            default:
                RETURN_LOG_ERROR("Generic Failure during waiting for a posix semaphore with error code: %d",RE_SEMAPHORE_WAIT,ret)
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
                RETURN_LOG_ERROR("Failure during posting to a posix semaphore due to the argument not being a valid semaphore",RE_SEMAPHORE_POST_INVALID)
            break;
            case ENOSYS:
                RETURN_LOG_ERROR("Failure during posting to a posix semaphore because the sem_post function is not supported by the system",RE_UNSUPPORTED)
            break;
            default:
                RETURN_LOG_ERROR("Generic Failure during posting to a posix semaphore with error code: %d",RE_SEMAPHORE_POST,errno)
            break;
        }
    }
    //success
    return RF_SUCCESS;
}
