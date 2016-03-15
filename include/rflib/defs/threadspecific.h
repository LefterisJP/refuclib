/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_THREADSPECIFIC_H
#define RF_THREADSPECIFIC_H

#if defined(__GNUC__) || defined(__TINYC__) //GCC and TCC
    #define i_THREAD__ __thread
#elif _MSC_VER //MSVC
    #define i_THREAD__ __declspec(thread)
#else
    #error Unsupported Compiler
#endif

#endif//include guards end
