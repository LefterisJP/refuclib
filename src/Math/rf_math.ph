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
** -- Math/rf_math.ph
** This file contains some math functions that I wanted
** to implement for the library. Maybe some day I would like to
** work towards my own implementation of the stdlib's math functions
** but for now this contains just things needed internally in the library
** so it's a private header.
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/inline.h>//for the inline declaration
#include <Definitions/types.h> //for the fixed size data types
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_MATH_PH
#define RF_MATH_PH



/**
 ** @brief Calculates the logarithm base 2 of an @c uint32_t
 **
 ** You can read more about this implementation at this page:
 ** http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 ** @param v Pass a @c uint32_t whose log2 is needed
 ** @return The log2 of @c v
 **
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
 **
 **/
#define IS_POWEROF2(X_) ((X_&(X_-1))==0)


#endif //include guards end
