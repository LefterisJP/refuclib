/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

/* Macros checking for sanity of stuff in debug mode*/

#ifndef RF_UTILS_SANITY_H
#define RF_UTILS_SANITY_H

/*------------- Outside Module inclusion -------------*/
#include <rf_options.h>
#include <Utils/log.h>
/*------------- libc inclusion --------------*/
#include <assert.h>
/*------------- End of includes -------------*/


/**
 * Macros to check for NULL pointers. Should be active most of the time.
 * Some tests rely on them to be active so that some functions can be tested
 * even with invalid input
 */
#if RF_OPTION_NULLPTR_CHECKS
#define RF_NULLCHECK1(ptr_, argname_, fail_)                  \
    do{                                                       \
        if (!(ptr_)) {                                        \
            RF_ERROR("Provided a NULL pointer for "argname_); \
            fail_;                                            \
        }                                                     \
    }while(0)

#define RF_NULLCHECK2(ptr1_, ptr2_, fail_)          \
        do{                                         \
            if (!(ptr1_) || !(ptr2_)) {             \
                RF_ERROR("NULL pointer detected") ; \
                fail_;                              \
            }                                       \
        }while(0)
#else
#define RF_NULLCHECK1(ptr_, argname_, fail_)
#define RF_NULLCHECK2(ptr1_, ptr2_, fail_)
#endif


/* just a wrapper over stdlib's assert */
#ifdef RF_OPTION_DEBUG
#define RF_ASSERT(condition_, ...)                   \
        do {                                         \
            if (!(condition_)) {                     \
                RF_CRITICAL(__VA_ARGS__);            \
                assert(condition_);                  \
            }                                        \
        }while(0)
#else
#define RF_ASSERT(condition_, ...)
#endif

/* same as RF_ASSERT but in non debug mode it will log a critical error */
#if defined(RF_OPTION_DEBUG) && !defined(RF_UNIT_TESTS)
#define RF_ASSERT_OR_CRITICAL(condition_, _stmt, ...)    \
    RF_ASSERT(condition_, __VA_ARGS__)
#else
#define RF_ASSERT_OR_CRITICAL(condition_, _stmt, ...)   \
    do {                                                \
        if (!(condition_)) {                            \
            RF_CRITICAL(__VA_ARGS__);                   \
            _stmt;                                      \
        }                                               \
    }while(0)
#endif

/**
 * Will kill the program(release) or assert(debug) if condition_ is not true
 */
#define RF_ASSERT_OR_EXIT(condition_, ...)                  \
    RF_ASSERT_OR_CRITICAL(condition_, exit(1), __VA_ARGS__)

#define i_RF_CRITICAL_TEST(line, condition_, ...) \
    ({                                            \
        int i_return_val_ ## line = 0;            \
        if ((condition_)) {                       \
            RF_CRITICAL(__VA_ARGS__);             \
            i_return_val_ ## line = 1;            \
        }                                         \
        i_return_val_ ## line;                    \
    })                                            \

/**
 * Checks if a Critical condition occurs, and if it does then it logs
 * a critical error and returns true
 *
 * Possible TODO: Introduce different behaviour if the critical test comes
 *                back positive. Or even have this behaviour configurable
 *
 */
#define RF_CRITICAL_TEST(condition_, ...)                     \
        i_RF_CRITICAL_TEST(__LINE__, condition_, __VA_ARGS__)

#endif /* include guards end */
