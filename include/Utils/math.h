#ifndef RF_UTILS_MATH_H
#define RF_UTILS_MATH_H

#include <Definitions/inline.h>
#include <Definitions/types.h> //for the fixed size data types

/**
 ** @brief Returns the maximum of 2 ints
 **/
i_INLINE_DECL int rfMax(int a, int b)
{
    if(a >= b)
    {
        return a;
    }
    return b;
}

/**
 ** @brief Calculates the logarithm base 2 of an @c uint32_t
 **
 ** You can read more about this implementation at this page:
 ** http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 ** @param v Pass a @c uint32_t whose log2 is needed
 ** @return The log2 of @c v
 **/
i_INLINE_DECL int log2_int32(uint32_t* v)
{
    static const int MultiplyDeBruijnBitPosition[32] =
    {
      0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
      8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
    };

    *v |= *v >> 1; // first round down to one less than a power of 2
    *v |= *v >> 2;
    *v |= *v >> 4;
    *v |= *v >> 8;
    *v |= *v >> 16;
    return MultiplyDeBruijnBitPosition[(uint32_t)((*v) * 0x07C4ACDDU) >> 27];
}


/**
 ** Should only be used with positive integers and does not work
 ** correctly for 0
 **/
#define IS_POWEROF2(X_) ((X_&(X_-1))==0)


#endif //include guards end
