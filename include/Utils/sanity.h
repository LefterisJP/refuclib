/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */

/* Macros checking for sanity of stuff in debug mode*/

#ifndef RF_UTILS_SANITY_H
#define RF_UTILS_SANITY_H

/*------------- Outside Module inclusion -------------*/
#include <rf_options.h>
#include <Utils/log.h>
#include <String/rf_str_common.h>
/*------------- libc inclusion --------------*/
#include <assert.h>
#include <stdlib.h> // for exit()
#include <stdio.h> // for printf()
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
                printf(__VA_ARGS__);                 \
                printf("\n");                        \
                fflush(stdout);                      \
                assert(condition_);                  \
            }                                        \
        }while(0)
#else
#define RF_ASSERT(condition_, ...)
#endif

/**
 * Will unconditionally terminate the program after emitting a critical message
 */
#define RF_CRITICAL_FAIL(...)                   \
    do {                                        \
        RF_CRITICAL(__VA_ARGS__);               \
        exit(1);                                \
    } while (0)

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
 * Behaves likes RF_ASSERT_OR_EXIT() but does not use any RFS() calls. Is
 * useful in only a handful of situations to avoid cyclic dependencies
 */
#define RF_ASSERT_OR_EXIT_NO_RFS(condition_, message_)  \
    do {                                                \
        assert((condition_) && message_);               \
        if (!(condition_)) {                            \
            printf("CRITICAL: "message_"\n");           \
            fflush(stdout);                             \
            exit(1);                                    \
        }                                               \
    } while (0)

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
