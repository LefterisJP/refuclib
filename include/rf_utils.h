/**
** @internal
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
** @endinternal
**/

#ifndef REFU_UTILS_H
#define REFU_UTILS_H

#include <rf_setup.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

///Macros for endianess
//! Defines little endian system
#define RF_LITTLE_ENDIAN    0
//! Defines big endian system
#define RF_BIG_ENDIAN       1

//! Gets the value (0) or (1) of the __n__ bit of __byte__
#define RF_GET_BIT(__byte__,__n__)  ((__byte__& (0x01<<__n__) )>>__n__ )




//! @internal
//!Globals for Refu Utils
typedef struct RF_UTILS
{
    //! A byte holding the system's endianess. Is @c 0 (@c RF_LITTLE_ENDIAN) for little endian and @c 1 (@c RF_BIG_ENDIAN) for big endian
    char endianess;
    //! A boolean flag indicating whether the system supports high resolution performance counter in windows and clock_gettime in Linux
    char hasHighResTimer;
#ifdef REFU_WIN32_VERSION
    //! In Windows we should also keep the PCFrequency in case we want to use a performance counter
    int64_t pcFrequency;
#elif defined(REFU_LINUX_VERSION)
    //! In Linux we will keep the type of the high res counter to be used by the timers
    clockid_t   timerType;
#else
    //! @todo
#endif
}RF_UTILS;
//define the utlil globals
extern RF_UTILS i_DECLIMEX_  rfUTILS;


//! A function which detects system's endianess, used only once during initialization of the library
i_DECLIMEX_ void rfUTILS_DetectEndianess();

//! A function that returns the endianess of the system
//! @return The endianess of the system
i_DECLIMEX_ char rfUTILS_Endianess();

//! Swaps the endianness of the variable
//! @warning the & 0xXXXX makes sure that in the signed case we DO NOT get a wrong number.
//! (Because when you shift right >> a signed number the bits get filled by the sign and not zeros
//! @param v The Variable whose endianness we wanna change
i_DECLIMEX_ void rfUTILS_SwapEndianUS(uint16_t* v);
//! Swaps the endianness of the variable
//! @param v The Variable whose endianness we wanna change
i_DECLIMEX_ void rfUTILS_SwapEndianS(int16_t* v);
//! Swaps the endianness of the variable
//! @param v The Variable whose endianness we wanna change
i_DECLIMEX_ void rfUTILS_SwapEndianUI(uint32_t*   v);
//! Swaps the endianness of the variable
//! @param v The Variable whose endianness we wanna change
i_DECLIMEX_ void rfUTILS_SwapEndianI(int32_t*   v);
//! Swaps the endianness of the variable
//! @param v The Variable whose endianness we wanna change
i_DECLIMEX_ void rfUTILS_SwapEndianUL(uint32_t*   v);


//! Checks if the bit @c BIT_ is set in the variable @c VAR_
#define RF_BIT_ON(VAR_,BIT_)    ((VAR_) & (1<<(BIT_)))
//! Checks if the bitflag @c FLAG_ is set in the variable @c VAR_
#define RF_BITFLAG_ON(VAR_,FLAG_)    (((VAR_) & (FLAG_))!=0)
//! Sets the bitflag @c FLAG_ in the variable @c VAR_
#define RF_BITFLAG_SET(VAR_,FLAG_)  (VAR_) |= (FLAG_)
//! Unsets the bitflag @c FLAG_ in the variable @c VAR_
#define RF_BITFLAG_UNSET(VAR_,FLAG_)  (VAR_) &= (~(FLAG_))


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
