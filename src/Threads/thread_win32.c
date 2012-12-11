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
#include <Definitions/defarg.h> //for enabling default arguments
#include <windows.h> //for the WinThread HANDLE definition
#include <Threads/thread_flags.h> //for common Thread flags
#include <Threads/thread_decl.h> //for RF_Thread
#include <Threads/thread.h>
//*---------------------Outside module inclusion----------------------------------------
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Utils/memory.h> //for refu memory allocation

#include <Utils/bits.h> //for RF_BITFLAG_ON()
#include <Definitions/retcodes.h> //for return codes
//for initializing the io buffer for this thread
    #include <Definitions/threadspecific.h> // for the thread specific keyword used in the ioBuffer
    #include <String/string_decl.h> //for RF_String (ioBuffer type)
    #include <String/stringx_decl.h> //for RF_StringX (ioBuffer type)
    #include "../IO/buff.ph"//stdio buffer thread-specific initialization
//for local memory initialization
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <Utils/localmem.h> //for LMS_Initialization
//*----------------------------End of Includes------------------------------------------

//The function that serves as the starting address for a thread in win32
static DWORD WINAPI RF_THREAD_FUNCTION(LPVOID  t)
{
    DWORD ret;//the return value
    //the void pointer is actually a thread
    RF_Thread* thread = (RF_Thread*)t;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,thread->lmsSize);
    //initialize the stdio for this thread
    rfInitStdio();
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
        RETURN_LOG_ERROR("Join failed due to the thread value not being joinable",RE_THREAD_NOTJOINABLE)

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
