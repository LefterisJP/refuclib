/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_INLINE_H
#define RF_INLINE_H

//kudos to this SO answer by Jens Gustedt http://stackoverflow.com/questions/5229343/how-to-declare-an-inline-function-in-c99-multi-file-project
#ifdef __GNUC__ //GCC
    #if !__GNUC_STDC_INLINE__ //old way to inline
        #define i_INLINE_DECL   static inline
        #define i_INLINE_INS    static inline
    #else //C99 way to inline
        #define i_INLINE_DECL   inline
        #define i_INLINE_INS    extern inline
    #endif
#else //GCC-end
    #error todo
#endif

#endif//include guards end
