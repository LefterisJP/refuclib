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

#include <rf_numconst.h>

#include <rf_localmem.h>

//Saves an int8_t into the local stack memory space
void* i_RFI8_(int8_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    int8_t * ptr = rfLMS_Push(RF_LMS,1);
    (*ptr) = i;
    return ptr;
}
//Saves a uint8_t into the local stack memory space
void* i_RFUI8_(uint8_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    uint8_t * ptr = rfLMS_Push(RF_LMS,1);
    (*ptr) = i;
    return ptr;
}
// Saves an int16_t into the local stack memory space
void* i_RFI16_(int16_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    int16_t * ptr = rfLMS_Push(RF_LMS,2);
    (*ptr) = i;
    return ptr;
}
//Saves a uint16_t into the local stack memory space
void* i_RFUI16_(uint16_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    uint16_t * ptr = rfLMS_Push(RF_LMS,2);
    (*ptr) = i;
    return ptr;
}
// Saves an int32_t into the local stack memory space
void* i_RFI32_(int32_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    int32_t * ptr = rfLMS_Push(RF_LMS,4);
    (*ptr) = i;
    return ptr;
}
//Saves a uint32_t into the local stack memory space
void* i_RFUI32_(uint32_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    uint32_t * ptr = rfLMS_Push(RF_LMS,4);
    (*ptr) = i;
    return ptr;
}
//Saves an int64_t into the local stack memory space
void* i_RFI64_(int64_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    int64_t * ptr = rfLMS_Push(RF_LMS,8);
    (*ptr) = i;
    return ptr;
}
//Saves a uint64_t into the local stack memory space
void* i_RFUI64_(uint64_t i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    uint64_t * ptr = rfLMS_Push(RF_LMS,8);
    (*ptr) = i;
    return ptr;
}

//Saves an  int type integer into the local stack memory spaceer
void* i_RFI_(int i)
{
    //remember the stack pointer before this macro evaluation
    rfLMS_MacroEvalPtr(RF_LMS);
    int* ptr = rfLMS_Push(RF_LMS,sizeof(int));
    (*ptr)=i;
    return ptr;
}
