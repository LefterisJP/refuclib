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
#include <Threads/thread.h>
/*------------- Module related inclusion -------------*/
#include <Threads/common.h>
/*------------- Outside Module inclusion -------------*/
// for error logging
    #include <Utils/log.h>
// for memory allocation
    #include <Utils/memory.h> //for refu memory allocation
// for RF_BITFLAG_ON()
    #include <Utils/bits.h> 
// for return codes
    #include <Definitions/retcodes.h> 
// for initializing the io buffer for this thread
    #include <Definitions/threadspecific.h> // for the thread specific keyword used in the ioBuffer
    #include <String/string_decl.h> //for RF_String (ioBuffer type)
    #include <String/stringx_decl.h> //for RF_StringX (ioBuffer type)
    #include "../IO/buff.ph"//stdio buffer thread-specific initialization
//for local memory initialization
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <Utils/localmem.h> //for LMS_Initialization
//for getting the id of a thread via system call
#include <System/rf_system.h> 
/*------------- End of includes -------------*/

static i_THREAD__ threadid_t i_thread_id;

threadid_t rfThread_GetID()
{
    return i_thread_id;
}

char rfThreads_Init()
{
    i_thread_id = rfSystem_GetThreadID();
    return true;
}

// The function that serves as the starting address for an RF_Thread in Linux
void* RF_THREAD_FUNCTION(void* param)
{
    void* ret;//the return value
    //the parameter is actually a pointer to the thread. Get it
    RF_Thread* t = (RF_Thread*)param;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    if(!rfLMS_Init(&lms,t->lmsSize))
    {
        //TODO: Add the thread's address in the msg
        RF_ERROR(
                 "Failure to initialize a thread because its local memory "
                 "stack could not be initialized");
        return (void*)RE_LOCALMEMSTACK_INIT;
    }
    i_thread_id = rfSystem_GetThreadID();

    //initialize the stdio for this thread
    rfInitStdio();
    //run the function
    ret = t->ptr2onExecution(t->data);
    //free the Local memory stack and return
    free(lms.stack);
    return ret;
}

//Allocates and returns a thread
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_Thread* rfThread_Create(uint32_t flags,
                               void* (*ptr2onExecution)(void*),
                               void* data,
                               uint64_t lmsSize )
#else
RF_Thread* i_rfThread_Create(uint32_t flags,
                                 void* (*ptr2onExecution)(void*),
                                 void* data,
                                 uint64_t lmsSize )
#endif
{
    RF_Thread* ret;
    RF_MALLOC(ret, sizeof(RF_Thread), NULL);
    //initialize the thread
    if(rfThread_Init(ret,flags,ptr2onExecution,data,lmsSize) == false)
    {
        free(ret);
        return NULL;
    }
    //return
    return ret;
}
//Initialises a thread
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfThread_Init(RF_Thread* t, uint32_t flags,
                   void* (*ptr2onExecution)(void*), void* data,
                   uint64_t lmsSize )
#else
char i_rfThread_Init(RF_Thread* t, uint32_t flags,
                     void* (*ptr2onExecution)(void*), void* data,
                     uint64_t lmsSize )
#endif
{
    pthread_attr_t attributes;
    //get the data and the lms size
    t->data = data;
    t->lmsSize = lmsSize;
    //get the function given by the user
    if(ptr2onExecution != 0)
    {
        t->ptr2onExecution = ptr2onExecution;
    }
    else
    {
        RF_ERROR(
                 "Passed a null pointer for the thread's execution. The thread"
                 " will be doing nothing, so it is meaningless");
        return false;
    }

    //flags processing should happen here
    t->flags = 0;
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
    if(pthread_create(&t->tHandle, &attributes,
                      RF_THREAD_FUNCTION, data) != 0)
    {
        RF_ERROR("Error during POSIX thread creation");
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
            RF_ERROR(
                     "No thread could be found with the given ID to kill");
        break;
        case 0:
            //all okay
            pthread_exit(&t->tHandle);
            return true;
        break;
        default:
            RF_ERROR(
                     "pthread_cancel returned error %d", err);
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
                RF_ERROR(
                    "pthread_join failed due to the thread value not "
                         "being joinable");
                return RE_THREAD_NOTJOINABLE;
            break;
            case ESRCH:
                RF_ERROR(
                         "pthread_join failed due to the thread id not "
                         "corresponding to any existing thread");
                return RE_THREAD_INVALID;
            break;
            case EDEADLK:
                RF_ERROR(
                         "pthread_join failed due to a deadlock being detected");
                return RE_THREAD_DEADLOCK;
            break;
            default:
                RF_ERROR(
                         "pthread_join failed with unknonwn error code: %d",
                         err);
                return -1;
            break;
        }
    }
    //else
    return RF_SUCCESS;
}
