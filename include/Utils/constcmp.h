/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_UTILS_CONSTCMP_H
#define RF_UTILS_CONSTCMP_H

/*------------- libc inclusion --------------*/
#include <float.h> //needed for EPSILON
#include <math.h> //for fabs
/*------------- End of includes -------------*/

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
#define DBLCMP_EQ(a__,b__)   fabs((a__) -(b__)) < DBL_EPSILON


/* ------ Safe Hex Literal comparisons ------ */
///chars

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for equality.
 **
 **/
#define RF_HEXEQ_C(c__,h__)    ((c__)==((char)h__))
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for equality.
 **
 **/
#define RF_HEXEQ_UC(c__,h__)    ((c__)==((unsigned char)h__))

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_C(c__,h__)    ((c__) >= ((char)h__))
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for greater or equal.
 **
 **/
#define RF_HEXGE_UC(c__,h__)    ((c__) >=((unsigned char)h__))

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for less or equal.
 **
 **/
#define RF_HEXLE_C(c__,h__)    ((c__) <=((char)h__))
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for less or equal.
 **
 **/
#define RF_HEXLE_UC(c__,h__)    ((c__) <= ((unsigned char)h__))

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for greater.
 **
 **/
#define RF_HEXG_C(c__,h__)    ((c__) > ((char)h__))
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for greater.
 **
 **/
#define RF_HEXG_UC(c__,h__)    ((c__) > ((unsigned char)h__))

/**
 ** A convenience macro to allow us to compare easily char bytes with hex values for less.
 **
 **/
#define RF_HEXL_C(c__,h__)    ((c__) < ( (char)h__))
/**
 ** A convenience macro to allow us to compare easily unsigned char bytes with hex values for less.
 **
 **/
#define RF_HEXL_UC(c__,h__)    ((c__) < ((unsigned char)h__))

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
