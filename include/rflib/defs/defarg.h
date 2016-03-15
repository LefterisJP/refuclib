/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
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

#include <rflib/preprocessor/rf_xmacro_argcount.h> //for RF_NARG argument count macro
#include <rflib/preprocessor/rf_xmacro_gt.h>    //for the greater than macro token comparison

#endif//include guards end
