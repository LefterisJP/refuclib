/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_UTILS_MATH_H
#define RF_UTILS_MATH_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/inline.h>
#include <Definitions/types.h> //for the fixed size data types
#include <Utils/build_assert.h>
/*------------- End of includes -------------*/

#if RF_HAVE_STATEMENT_EXPR && RF_HAVE_TYPEOF
// rf_min, rf_max and rf_clamp have been taken from the CCANN project. License is Public domain.
#if RF_HAVE_BUILTIN_TYPES_COMPATIBLE_P
#define MINMAX_ASSERT_COMPATIBLE(a, b) \
	BUILD_ASSERT(__builtin_types_compatible_p(a, b))
#else
#define MINMAX_ASSERT_COMPATIBLE(a, b) \
	do { } while (0)
#endif

#define rf_min(a, b) \
	({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		MINMAX_ASSERT_COMPATIBLE(typeof(_a), typeof(_b)); \
		_a < _b ? _a : _b; \
	})

#define rf_max(a, b) \
	({ \
		typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		MINMAX_ASSERT_COMPATIBLE(typeof(_a), typeof(_b)); \
		_a > _b ? _a : _b; \
	})

#define rf_clamp(v, f, c)	(max(min((v), (c)), (f)))


#define rf_min_t(t, a, b) \
	({ \
		t _ta = (a); \
		t _tb = (b); \
		rf_min(_ta, _tb); \
	})
#define rf_max_t(t, a, b) \
	({ \
		t _ta = (a); \
		t _tb = (b); \
		rf_max(_ta, _tb); \
	})

#define rf_clamp_t(t, v, f, c) \
	({ \
		t _tv = (v); \
		t _tf = (f); \
		t _tc = (c); \
		rf_clamp(_tv, _tf, _tc); \
	})

#else

/*
 * Without statement expressions and typeof the above macros would be dangerous
 * as there would be no way to avoid unsafe double evaluation of the arguments.
 * 
 * Use a more modern compiler ... or create inline functions for your use case?
 */
#error Compiler does not support statement expressions and/or typeof. See comment.
#endif

/**
 ** @brief Calculates the logarithm base 2 of an @c uint32_t
 **
 ** You can read more about this implementation at this page:
 ** http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 ** @param v Pass a @c uint32_t whose log2 is needed
 ** @return The log2 of @c v
 **/
i_INLINE_DECL int rf_math_log2_u32(uint32_t* v)
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
