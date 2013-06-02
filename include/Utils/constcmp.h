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
** --Utils/constcmp.h
**
** Some convenience macros for more safe constant comparison
** First the constant is cast to whatever you compare it to
** and then compared.
**
** For floating points, the minimum float or double representable by the
** system, the epsilon, is used in the comparison. To achieve that, <float.h>
** must be included
**
---------------------For internal library include make sure to have----------------------------
#include <float.h> //for EPSILON (only if you actually call the float or double macros)
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_UTILS_CONSTCMP_H
#define RF_UTILS_CONSTCMP_H

/* --------------------------------------------- Safe Float and Double comparisons (need to include <float.h> too------------------------------------------------------- */

/**
 **  Returns true if float a > float b
 **
 **/
#define FLCMP_G(a__,b__)  (a__)-(b__) > FLT_EPSILON
/**
 **  Retuns true if float a >= float b
 **
 **/
#define FLCMP_GE(a__,b__)   (a__)-(b__) >= 0
/**
 **  Returns true if float a < float b
 **
 **/
#define FLCMP_L(a__,b__)    (b__)-(a__) > FLT_EPSILON
/**
 **  Returns true if float a <= float b
 **
 **/
#define FLCMP_LE(a__,b__)   (b__)-(a__) >= 0
/**
 **  Returns true if float a == float b
 **
 **/
#define FLCMP_EQ(a__,b__)   fabs( (a__)-(b__))<FLT_EPSILON

/**
 **  Returns true if float a > float b
 **
 **/
#define DBLCMP_G(a__,b__)  a__-b__>DBL_EPSILON
/**
 **  Retuns true if float a >= float b
 **
 **/
#define DBLCMP_GE(a__,b__)   a__-b__>=0
/**
 **  Returns true if float a < float b
 **
 **/
#define DBLCMP_L(a__,b__)    b__-a__>DBL_EPSILON
/**
 **  Returns true if float a <= float b
 **
 **/
#define DBLCMP_LE(a__,b__)   b__-a__>=0
/**
 **  Returns true if float a == float b
 **
 **/
#define DBLCMP_EQ(a__,b__)   fabs(a__-b__)<DBL_EPSILON


/* --------------------------------------------- Safe Hex Literal comparisons ------------------------------------------------------- */
///chars

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for equality.
 **
 **/
#define RF_HEXEQ_C(c__,h__)    (c__==(char)h__)
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for equality.
 **
 **/
#define RF_HEXEQ_UC(c__,h__)    (c__==(unsigned char)h__)

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_C(c__,h__)    (c__>=(char)h__)
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_UC(c__,h__)    (c__>=(unsigned char)h__)

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for less or equal.
 **
 **/
#define RF_HEXLE_C(c__,h__)    (c__<=(char)h__)
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for less or equal.
 **
 **/
#define RF_HEXLE_UC(c__,h__)    (c__<=(unsigned char)h__)

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for greater.
 **
 **/
#define RF_HEXG_C(c__,h__)    (c__>(char)h__)
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for greater.
 **
 **/
#define RF_HEXG_UC(c__,h__)    (c__>(unsigned char)h__)

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for less.
 **
 **/
#define RF_HEXL_C(c__,h__)    (c__<(char)h__)
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for less.
 **
 **/
#define RF_HEXL_UC(c__,h__)    (c__<(unsigned char)h__)

///shorts

/**
 ** A convenience macro to allow us to compare easily shorts with hex values for equality.
 **
 **/
#define RF_HEXEQ_S(c__,h__)    (c__==(int16_t)h__)
/**
 ** A convenience macro to allow us to compare easily shorts with hex values for equality.
 **
 **/
#define RF_HEXEQ_US(c__,h__)    (c__==(uint16_t)h__)

/**
 ** A convenience macro to allow us to compare easily shorts with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_S(c__,h__)    (c__>=(int16_t)h__)
/**
 ** A convenience macro to allow us to compare easily shorts with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_US(c__,h__)    (c__>=(uint16_t)h__)

/**
 ** A convenience macro to allow us to compare easily shorts with hex values for less or equal.
 **
 **/
#define RF_HEXLE_S(c__,h__)    (c__<=(int16_t)h__)
/**
 ** A convenience macro to allow us to compare easily shorts with hex values for less or equal.
 **
 **/
#define RF_HEXLE_US(c__,h__)    (c__<=(uint16_t)h__)

/**
 ** A convenience macro to allow us to compare easily shorts with hex values for greater.
 **
 **/
#define RF_HEXG_S(c__,h__)    (c__>(int16_t)h__)
/**
 ** A convenience macro to allow us to compare easily shorts bytes with hex values for greater.
 **
 **/
#define RF_HEXG_US(c__,h__)    (c__>(uint16_t)h__)

/**
 ** A convenience macro to allow us to compare easily shorts with hex values for less.
 **
 **/
#define RF_HEXL_S(c__,h__)    (c__<(int16_t)h__)
/**
 ** A convenience macro to allow us to compare easily shorts with hex values for less.
 **
 **/
#define RF_HEXL_US(c__,h__)    (c__<(uint16_t)h__)


///ints

/**
 ** A convenience macro to allow us to compare easily ints with hex values for equality.
 **
 **/
#define RF_HEXEQ_I(c__,h__)    (c__==(int32_t)h__)
/**
 ** A convenience macro to allow us to compare easily ints with hex values for equality.
 **
 **/
#define RF_HEXEQ_UI(c__,h__)    (c__==(uint32_t)h__)

/**
 ** A convenience macro to allow us to compare easily ints with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_I(c__,h__)    (c__>=(int32_t)h__)
/**
 ** A convenience macro to allow us to compare easily ints with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_UI(c__,h__)    (c__>=(uint32_t)h__)

/**
 ** A convenience macro to allow us to compare easily ints with hex values for less or equal.
 **
 **/
#define RF_HEXLE_I(c__,h__)    (c__<=(int32_t)h__)
/**
 ** A convenience macro to allow us to compare easily ints with hex values for less or equal.
 **
 **/
#define RF_HEXLE_UI(c__,h__)    (c__<=(uint32_t)h__)

/**
 ** A convenience macro to allow us to compare easily ints with hex values for greater.
 **
 **/
#define RF_HEXG_I(c__,h__)    (c__>(int32_t)h__)
/**
 ** A convenience macro to allow us to compare easily ints bytes with hex values for greater.
 **
 **/
#define RF_HEXG_UI(c__,h__)    (c__>(uint32_t)h__)

/**
 ** A convenience macro to allow us to compare easily ints with hex values for less.
 **
 **/
#define RF_HEXL_I(c__,h__)    (c__<(int32_t)h__)
/**
 ** A convenience macro to allow us to compare easily ints with hex values for less.
 **
 **/
#define RF_HEXL_UI(c__,h__)    (c__<(uint32_t)h__)


#endif//include guards end
