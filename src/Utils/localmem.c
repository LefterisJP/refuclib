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
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <Utils/localmem.h>
/*------------- Module related inclusion -------------*/
#include "localmem.ph" //for the private definitions of rfLMS functions
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <Utils/log.h>
//for memory allocation
    #include <Utils/memory.h> //for refu memory allocation
/*------------- libc inclusion -------------*/
#include <string.h> //for memset() used in the Local scope macros
/*------------- End of includes -------------*/


//define The main thread's local stack memory
RF_LocalMemoryStack RF_MainLMS;
//define the pointer to the thread specific memory stack
i_THREAD__ RF_LocalMemoryStack* RF_LMS;



// Initializes the local memory stack
bool rfLMS_Init(RF_LocalMemoryStack* lms, uint64_t size)
{
    lms->stackPtr = 0;
    lms->macroEvalsI = 0;
    RF_MALLOC(lms->stack, size, false);
    memset(lms->macroEvals,0,4*RF_MAX_FUNC_ARGS);
    //also make the thread specific pointer point to this local memory stack
    RF_LMS = lms;
    return true;
}


//Allocates some memory from the local memory stack
void* rfLMS_Push(uint64_t size)
{
    uint32_t temp;
    if(RF_LMS->stackPtr+size > RF_OPTION_LOCALSTACK_MEMORY_SIZE)
    {
         RF_ERROR("Local Stack Memory pushing error. Attempted to "
         "allocate more memory than currently available");
        return NULL;
    }
    temp = RF_LMS->stackPtr;
    RF_LMS->stackPtr+=size;
    return RF_LMS->stack+temp;
}


//Frees some memory from the local memory stack
bool rfLMS_Pop(uint64_t size)
{
    if(size > RF_LMS->stackPtr)
    {
         RF_ERROR("Local Stack Memory popping error. Attempted to"
                  " pop the memory to a point in the stack that's not "
                  "allocated yet");
        return false;
    }
    RF_LMS->stackPtr=size;
    return true;
}


//Keeps the stack pointer before the specific macro evaluation
bool rfLMS_ArgsEval()
{
    if(RF_LMS->macroEvalsI+1 >= RF_MAX_FUNC_ARGS)
    {
        RF_ERROR("Local Stack Memory macro evaluation error. More "
                 "macros than the specified maximum number of function "
                 "arguments \"%d\" have been evaluated", RF_MAX_FUNC_ARGS);
        return false;

    }
    RF_LMS->macroEvals[RF_LMS->macroEvalsI] = RF_LMS->stackPtr;
    RF_LMS->macroEvalsI++;
    return true;
}
