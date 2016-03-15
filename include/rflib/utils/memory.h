/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_MEMORY_H
#define RF_MEMORY_H

#include <rf_options.h>
#include <rflib/utils/log.h>
#include <stdlib.h>

//Here are some macro wrappers of malloc,calloc and realloc that depending
//on the flag @c RF_OPTION_SAFE_MEMORY_ALLOCATION check their return
//value or not

//for realloc I check no matter what since it's a bit more complicated case than the other two

/**
 ** Wrapper macro of the realloc() function that does check for memory
 ** allocation failure.
 ** @param REALLOC_RETURN_     Give the pointer that you need to realloc here
 ** @param TYPE_               give the type of the pointer here
 ** @param SIZE_               Give the size parameter of realloc here
 ** @param STMT_               Statement/s to execute if the memory
 **                              allocation fails
 **/
#define RF_REALLOC(REALLOC_RETURN_, TYPE_, SIZE_, STMT_)          \
    do{                                                           \
        TYPE_* i_TEMPPTR_ = realloc((REALLOC_RETURN_), (SIZE_));  \
        if (i_TEMPPTR_ == NULL) {                                 \
            RF_ERROR("realloc() failure");                        \
            STMT_;                                                \
        }                                                         \
        REALLOC_RETURN_ = i_TEMPPTR_;                             \
    }while(0)

/* ---- SAFE MEMORY ALLOCATION macros ---- */
#if defined(RF_OPTION_SAFE_MEMORY_ALLOCATION) || defined(RF_OPTION_DEBUG)

/**
 ** Wrapper macro of the malloc() function that does check for memory
 ** allocation failure. The function that calls it must return value of
 ** type @c RETVAL_
 ** @param MALLOC_RETURN_       Give the pointer to hold the return of malloc
 ** @param MALLOC_SIZE_         Give the size parameter of malloc
 ** @param STMT_                Statement/s to execute if the memory
 **                             allocation fails
 **/
#define RF_MALLOC(MALLOC_RETURN_, MALLOC_SIZE_, STMT_)  \
    do{                                                 \
        MALLOC_RETURN_ = malloc((MALLOC_SIZE_));        \
        if (MALLOC_RETURN_ == NULL) {                   \
            RF_ERROR("malloc() failure");               \
            STMT_;                                      \
        }                                               \
    }while(0)

/**
 ** Wrapper macro of the calloc() function that does check for memory
 ** allocation failure.
 ** @param CALLOC_RETURN_        Give the pointer to hold the return of malloc
 ** @param CALLOC_NUM_           Give the number parameter of calloc
 ** @param CALLOC_SIZE_          Give the size parameter of calloc
 ** @param STMT_                 Statement/s to execute if the memory
 **                              allocation fails
 **/
#define RF_CALLOC(CALLOC_RETURN_,CALLOC_NUM_,CALLOC_SIZE_, STMT_) \
    do{                                                           \
        CALLOC_RETURN_ = calloc((CALLOC_NUM_), (CALLOC_SIZE_));   \
        if (CALLOC_RETURN_ == NULL) {                             \
            RF_ERROR("calloc() failure");                         \
            STMT_;                                                \
        }                                                         \
    }while(0)

/* ---- NOT SAFE MEMORY ALLOCATION macros ---- */
#else

#define RF_MALLOC(MALLOC_RETURN_,MALLOC_SIZE_, RETVAL_) \
    MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) )
#define RF_CALLOC(CALLOC_RETURN_, CALLOC_NUM_, CALLOC_SIZE_, RETVAL_) \
    CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) )
#endif



/**
 * Initialize all members of a structure to zero
 *
 * Note: A nice way would be to do something like:
 * struct a = (const struct a) {0};
 * but in GCC this induces a warning which is a known bug.
 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
 */
#define RF_STRUCT_ZERO(i_struct_) memset((i_struct_), 0, sizeof(*(i_struct_)))

#endif//include guards end
