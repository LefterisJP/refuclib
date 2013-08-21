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
** --Definitions/defarg.h
** This header file simply includes all the preprocessor files needed to have functions
** with default arguments
*/
#ifndef RF_DEFARG_H
#define RF_DEFARG_H

#include <rf_options.h> //to get the existence or not of default arguments

/*-- A macro that hopefully generates an intelligible compiler error --*/
#if defined(__GNUC__) || defined(__TINYC__) //GCC and TCC
    #define RF_COMPILE_ERROR(i_ERROR_STR)   ;_Pragma(i_ERROR_STR);\
                                        COMPILE_ERROR
#elif _MSC_VER //MSVC
    #define __STR2__(x) #x
    #define __STR1__(x) __STR2__(x)
    #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Error E0000: #pragma VSERROR: "
    #define RF_COMPILE_ERROR(x)  message(__LOC__ x)
#else
    #error Unsupported Compiler
#endif

#include "Preprocessor/rf_xmacro_argcount.h" //for RF_NARG argument count macro
#include "Preprocessor/rf_xmacro_gt.h"    //for the greater than macro token comparison

#endif//include guards end
