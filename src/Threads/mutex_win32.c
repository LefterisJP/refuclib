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
#include <windows.h> //for the MUTEX HANDLE definition
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
//*----------------------------End of Includes------------------------------------------

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
                RETURN_LOG_ERROR("During winapi mutex locking, ownership of the parameter mutex object was given succesfully to the caller,\
                           but the mutex itself was not released properly by its previous owner. Check for incosistencies",RE_MUTEX_LOCK_INVALID)
            break;
            case WAIT_TIMEOUT:
                RETURN_LOG_ERROR("During winapi mutex locking the timeout limit elapsed.This should never happen. Serious error!",RE_MUTEX_LOCK)
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
                RETURN_LOG_ERROR("During winapi mutex locking, ownership of the parameter mutex object was given succesfully to the caller,\
                           but the mutex itself was not released properly by its previous owner. Check for incosistencies",RE_MUTEX_LOCK_INVALID)
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
                RETURN_LOG_ERROR("During winapi mutex locking, ownership of the parameter mutex object was given succesfully to the caller,\
                           but the mutex itself was not released properly by its previous owner. Check for incosistencies",RE_MUTEX_LOCK_INVALID)
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
