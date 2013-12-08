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
 * These macros checks if some pointers are null and fail if they are
 */
#if RF_OPTION_DEBUG
#define RF_CHECK_NOT_NULL_DEBUG_1(ptr_, argname_, fail_)      \
    do{                                                       \
        if(!ptr_)                                             \
        {                                                     \
            RF_ERROR("Provided a NULL pointer for "argname_); \
            fail_;                                            \
        }                                                     \
    }while(0)

#define RF_CHECK_NOT_NULL_DEBUG_2(ptr1_, ptr2_, fail_)  \
    do{                                         \
        if(!ptr1_ || !ptr2_)                    \
        {                                       \
            RF_ERROR("NULL pointer detected") ; \
            fail_;                              \
        }                                       \
    }while(0)


#define RF_CHECK_NOT_NULL_DEBUG_3(ptr1_, ptr2_, ptr3_, fail_) \
    do{                                               \
        if(!ptr1_ || !ptr2_ || !ptr3_)                \
        {                                             \
            RF_ERROR("NULL pointer detected") ;       \
            fail_;                                    \
        }                                             \
    }while(0)

#define RF_CHECK_NOT_NULL_DEBUG_4(ptr1_, ptr2_, ptr3_, ptr4_, fail_)  \
    do{                                                       \
        if(!ptr1_ || !ptr2_ || !ptr3_ || !ptr4_)              \
        {                                                     \
            RF_ERROR("NULL pointer detected") ;               \
            fail_;                                            \
        }                                                     \
    }while(0)

#else
#define RF_CHECK_NOT_NULL_DEBUG_1(ptr_, argname_, ret_) 
#define RF_CHECK_NOT_NULL_DEBUG_2(ptr1_, ptr2_, fail_)      
#define RF_CHECK_NOT_NULL_DEBUG_3(ptr1_, ptr2_, ptr3_, fail_)      
#define RF_CHECK_NOT_NULL_DEBUG_4(ptr1_, ptr2_, ptr3_, ptr4_, fail_)      
#endif



#if RF_OPTION_INSANITY_CHECKS
#define RF_INSANE_CHECK_NOT_NULL_1(ptr_, argname_, fail_)     \
    do{                                                       \
        if(!ptr_)                                             \
        {                                                     \
            RF_ERROR("Provided a NULL pointer for "argname_); \
            fail_;                                            \
        }                                                     \
    }while(0)

#define RF_INSANE_CHECK_NOT_NULL_2(ptr1_, ptr2_, fail_) \
        do{                                             \
            if(!ptr1_ || !ptr2_)                        \
            {                                           \
                RF_ERROR("NULL pointer detected") ;     \
                fail_;                                  \
            }                                           \
        }while(0)


#define RF_INSANE_CHECK_NOT_NULL_3(ptr1_, ptr2_, ptr3_, fail_)  \
        do{                                                     \
            if(!ptr1_ || !ptr2_ || !ptr3_)                      \
            {                                                   \
                RF_ERROR("NULL pointer detected") ;             \
                fail_;                                          \
            }                                                   \
        }while(0)

#define RF_INSANE_CHECK_NOT_NULL_4(ptr1_, ptr2_, ptr3_, ptr4_, fail_) \
        do{                                                           \
            if(!ptr1_ || !ptr2_ || !ptr3_ || !ptr4_)                  \
            {                                                         \
                RF_ERROR("NULL pointer detected") ;                   \
                fail_;                                                \
            }                                                         \
        }while(0)

#else
#define RF_INSANE_CHECK_NOT_NULL_1(ptr_, argname_, ret_) 
#define RF_INSANE_CHECK_NOT_NULL_2(ptr1_, ptr2_, fail_)      
#define RF_INSANE_CHECK_NOT_NULL_3(ptr1_, ptr2_, ptr3_, fail_)      
#define RF_INSANE_CHECK_NOT_NULL_4(ptr1_, ptr2_, ptr3_, ptr4_, fail_)      
#endif



/* just a wrapper over stdlib's assert */
#ifdef RF_OPTION_DEBUG
#define RF_ASSERT(condition_)                       \
        do {                                         \
            if (!condition_) {                       \
                RF_CRITICAL("Assertion triggered");  \
                assert(condition_);                  \
            }                                        \
        }while(0)
#else
#define RF_ASSERT(condition_)           
#endif
#endif /* include guards end */
