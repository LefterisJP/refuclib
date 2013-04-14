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
**
**
**
** --Utils/libinit.h
**
** The initialization function of the library.
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/imex.h> //import export macro
#include <Definitions/types.h> //fixed size data types
#include <Definition/defarg.h> //to enable default arguments
#include <Utils/libinit.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_LIBINIT_H
#define RF_LIBINIT_H

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @defgroup RF_GeneralGRP Initialization function
** @addtogroup RF_GeneralGRP
** @{
**/

//! @brief Initializes the ReFu library
//!
//! This function needs to be called in the very beginning of any program using the library. It initializes everything that has to do with the library itself. Some of the things it does is:
//! + Initialize the error and info logging files
//! + Detect the endianess of the system and keep it so that the user can retrieve it with @ref rfEndianess()
//! + Initialize the local stack memory for the main thread
//! @param errstr \rfoptional{"refuErrorLog"} The filename of the error log to create and output the refu error log. You can provide any filename except for
//! @c "stdout" and @c "stderr" because they will be interpreted as special values for which the standard output stream or the standard error stream are used respectively
//! If no value is provided the default value is @c "refuErrorLog"
//! @param logstr \rfoptional{"refuInfoLog"} The filename of the info log to creeat and output the refu info log.You can provide any filename except for
//! @c "stdout" and @c "stderr" because they will be interpreted as special values for which the standard output stream or the standard error stream are used respectively
//! If no value is provided the default value is @c "refuInfoLog"
//! @param lmsSize \rfoptional{RF_OPTION_LOCALSTACKMEMORY_SIZE} The size of the main thread's local memory stack. This will be the size by which the
//! main thread's local memory stack will be initialized. It allows the usage of macros for temporary object creation such as @ref RFS_() or @ref RFXML_()
//! If no value is given the default value is to use the one provided at compile time by the Refu Builder and the
//! preprocessor directive @c RF_OPTION_LOCALSTACKMEMORY_SIZE.
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_  void rfInit(char* errstr,char* logstr,uint64_t size);
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_  void i_rfInit(char* errstr,char* logstr,uint64_t size);
    #define rfInit(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_INIT,3,__VA_ARGS__)
    #define i_NPSELECT_RF_INIT1(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfStringInit() accepts from 0 to 3 arguments\"")
    #define i_NPSELECT_RF_INIT0(...)  RF_SELECT_FUNC(i_SELECT_RF_INIT,__VA_ARGS__)
    #define i_SELECT_RF_INIT3(...)  i_rfInit(__VA_ARGS__)
    #define i_SELECT_RF_INIT2(...)  i_rfInit(__VA_ARGS__,RF_OPTION_LOCALSTACKMEMORY_SIZE)
    #define i_SELECT_RF_INIT1(...)  i_rfInit(__VA_ARGS__,"refuInfoLog",RF_OPTION_LOCALSTACKMEMORY_SIZE)
    #define i_SELECT_RF_INIT0(...)  i_rfInit("refuErrorLog","refuInfoLog",RF_OPTION_LOCALSTACKMEMORY_SIZE)
#else
    i_DECLIMEX_  void rfInit(char* errstr,char* logstr,uint64_t size);
#endif
#endif
//! @}
//closing RF_GeneralGRP doxygen group


#ifdef __cplusplus
}//closing bracket for calling from C++
#endif

#endif //include guards
