/* Macros checking for sanity of stuff in debug mode*/

#ifndef RF_UTILS_SANITY_H
#define RF_UTILS_SANITY_H

#include <rf_options.h>
#include <Utils/log.h>


/**
 * These macros checks if some pointers are null and fail if they are
 */
#if RF_OPTION_DEBUG
#define i_NULLPTR_CHECK_1(ptr_, argname_, fail_)              \
    do{                                                       \
        if(!ptr_)                                             \
        {                                                     \
            RF_ERROR("Provided a NULL pointer for "argname_); \
            fail_;                                            \
        }                                                     \
    }while(0)

#define i_NULLPTR_CHECK_2(ptr1_, ptr2_, fail_)      \
    do{                                             \
        if(!ptr1 || !ptr2_)                         \
        {                                           \
            RF_ERROR("NULL pointer detected") ;     \
            fail_;                                  \
        }                                           \
    }while(0)


#define i_NULLPTR_CHECK_3(ptr1_, ptr2_, ptr3_, fail_) \
    do{                                               \
        if(!ptr1 || !ptr2_ || !ptr3_)                 \
        {                                             \
            RF_ERROR("NULL pointer detected") ;       \
            fail_;                                    \
        }                                             \
    }while(0)

#define i_NULLPTR_CHECK_4(ptr1_, ptr2_, ptr3_, ptr_4, fail_)  \
    do{                                                       \
        if(!ptr1 || !ptr2_ || !ptr3_ || !ptr4_)               \
        {                                                     \
            RF_ERROR("NULL pointer detected") ;               \
            fail_;                                            \
        }                                                     \
    }while(0)

#else
#define i_NULLPTR_CHECK_1(ptr_, argname_, ret_) 
#define i_NULLPTR_CHECK_2(ptr1_, ptr2_, fail_)      
#define i_NULLPTR_CHECK_3(ptr1_, ptr2_, ptr3_, fail_)      
#define i_NULLPTR_CHECK_4(ptr1_, ptr2_, ptr3_, ptr4_, fail_)      
#endif

#endif /* include guards end */
