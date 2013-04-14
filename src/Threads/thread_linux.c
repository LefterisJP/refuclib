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
#include <Definitions/defarg.h> //for enabling default arguments
#include <pthread.h> //for pthread_t
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
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h>
//*----------------------------End of Includes------------------------------------------


// The function that serves as the starting address for an RF_Thread in Linux
void* RF_THREAD_FUNCTION(void* param)
{
    void* ret;//the return value
    //the parameter is actually a pointer to the thread. Get it
    RF_Thread* t = (RF_Thread*)param;
    //initialize the local memory stack of the thread
    RF_LocalMemoryStack lms;
    rfLMS_Init(&lms,t->lmsSize);
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
                RETURN_LOG_ERROR("pthread_join failed due to the thread value not being joinable",RE_THREAD_NOTJOINABLE)
            break;
            case ESRCH:
                RETURN_LOG_ERROR("pthread_join failed due to the thread id not corresponding to any existing thread",RE_THREAD_INVALID)
            break;
            case EDEADLK:
                RETURN_LOG_ERROR("pthread_join failed due to a deadlock being detected",RE_THREAD_DEADLOCK)
            break;
            default:
                RETURN_LOG_ERROR("pthread_join failed with unknonwn error code: %d",RE_THREAD_JOIN,err)
            break;
        }
    }
    //else
    return RF_SUCCESS;
}
