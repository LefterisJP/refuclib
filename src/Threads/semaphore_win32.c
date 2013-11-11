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
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <windows.h> //for the Semaphore HANDLE definition
#include <Threads/semaphore_decl.h> //for RF_Semaphore
#include <Threads/semaphore.h>
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <String/string_decl.h> //for RF_String
    #include <String/common.h> //for RFS_() macro
    #include <Utils/error.h>
//for memory allocation
#include <stdlib.h> //for malloc, calloc,realloc and exit()
#include <Definitions/retcodes.h> //for error codes, logged in allocation failure
#include <Utils/memory.h> //for refu memory allocation
//*----------------------------End of Includes------------------------------------------

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
                RETURN_LOG_ERROR("During winapi semaphore waiting ownership of the parameter mutex object was given succesfully to the caller,\
                           but the semaphore itself was not released properly by its previous owner. Check for incosistencies",RE_SEMAPHORE_WAIT_INVALID)
            break;
            case WAIT_TIMEOUT:
                RETURN_LOG_ERROR("During winapi semaphore waiting the timeout limit elapsed.This should never happen. Serious error!",RE_SEMAPHORE_WAIT_INVALID)
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
                RETURN_LOG_ERROR("During winapi semaphore waiting ownership of the parameter mutex object was given succesfully to the caller,\
                           but the semaphore itself was not released properly by its previous owner. Check for incosistencies",RE_SEMAPHORE_WAIT_INVALID)
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
                RETURN_LOG_ERROR("During winapi semaphore waiting ownership of the parameter mutex object was given succesfully to the caller,\
                           but the semaphore itself was not released properly by its previous owner. Check for incosistencies",RE_SEMAPHORE_WAIT_INVALID)
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
