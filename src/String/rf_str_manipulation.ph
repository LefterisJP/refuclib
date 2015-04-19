/* Code common for string manipulation sources */

#ifndef RF_STRING_MANIPULATION_PH
#define RF_STRING_MANIPULATION_PH

/*------------- Module related inclusion -------------*/
#include <String/rf_str_corex.h>
#include <String/rf_str_flags.h> //for RFstring_matching_options
#include "rf_str_common.ph" //for stringx realloc
/*------------- Outside Module inclusion -------------*/
#include <Definitions/inline.h> //for inline
#include <Utils/sanity.h> //for the sanity macros
/*------------- libc inclusion -------------*/
#include <limits.h> //for UINT_MAX
/*------------- End of includes -------------*/


#define ADD_TYPE_TO_STRING(str_, type_, typemax_, typefs_, rc_, ...)    \
    rc_ = snprintf(rf_string_data(str_) + rf_string_length_bytes(str_), \
                   typemax_,                                            \
                   typefs_, __VA_ARGS__);                               \
    if(rc_ < 0 || rc_ >= typemax_)                                      \
    {                                                                   \
        RF_ERROR("During appending "type_" to a string snprintf failed"); \
        return false;                                                   \
    }                                                                   \
    rf_string_length_bytes(str_) += rc_;                                \


i_INLINE_DECL bool rf_string_generic_prepend(void* thisstr, const char* other,
                                       unsigned int orig_size,
                                       unsigned int other_size)
{
    int i;
    //move the old string to the end of the buffer
    for(i = orig_size - 1; i >= 0 ; i--)
    {
        rf_string_data(thisstr)[i + other_size] = rf_string_data(thisstr)[i];
    }
    //add the string to the start
    memcpy(
        rf_string_data(thisstr),
        other,
        other_size
   );
   rf_string_length_bytes(thisstr) += other_size;
   return true;
}

/* checks if adding a string will need buffer reallocation */
i_INLINE_DECL bool rf_stringx_generic_append(struct RFstringx* s,
                                             const char* other,
                                             unsigned int bytes_to_copy)
{
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(s,
                       rf_string_length_bytes(s) + bytes_to_copy,
                       return false);
    rf_string_generic_append(s, other, bytes_to_copy);
    return true;
}

/**
 * Prepares necessary steps for replacing substrings inside a string
 *
 * This function calls rf_sbuffer_push(RF_TSBUFFS) without popping it since it
 * returns the buffer to be used by subsequent functions. After finishing always
 * remember to pop it.
 *
 * It is used in conjuction with @ref replace_greater(), @ref replace_lesser()
 * and @ref replace_equal(). Afterwards the buffer should be popped.
 *
 * @param[in] s              The string to replace
 * @param[in/out] number     Pointer to a uint32_t containing the number of first
 *                           substrings to replace. If 0 then all substrings are
 *                           replaced. At the end this will contain the actual
 *                           number of substrings found.
 * @param[in] sstr           The substring to replace
 * @param options            Replacement options. Check @ref enum RFstring_matching_options
 *                           for details
 * @return                   A pointer to a buffer containing the starting indices
 *                           of the found substrings in the string or NULL in
 *                           case of failure
 */
uint32_t *replace_intro(void *s,
                        uint32_t *number,
                        const void *sstr,
                        enum RFstring_matching_options options);

i_INLINE_DECL void replace_greater(void *s,
                                   uint32_t *buff,
                                   uint32_t number,
                                   const void *sstr,
                                   const void *rstr)
{
    unsigned int i;
    unsigned int j;
    unsigned int diff = rf_string_length_bytes(rstr) - rf_string_length_bytes(sstr);
    //now replace all the substrings one by one
    for (i = 0; i < number; i ++) {
        memmove(
            rf_string_data(s) + buff[i] + rf_string_length_bytes(sstr) + diff,
            rf_string_data(s) + buff[i] + rf_string_length_bytes(sstr),
            rf_string_length_bytes(s) - (buff[i] + rf_string_length_bytes(sstr))
        );
        //copy in the replacement
        memcpy(
            rf_string_data(s) + buff[i],
            rf_string_data(rstr),
            rf_string_length_bytes(rstr)
        );
        //also increase the original size
        rf_string_length_bytes(s) += diff;
        //also increase all the subsequent found byte positions
        for (j = i + 1; j < number; j ++) {
            buff[j] = buff[j] + diff;
        }
    }
}

i_INLINE_DECL void replace_lesser(void *s,
                                  uint32_t *buff,
                                  uint32_t number,
                                  const void *sstr,
                                  const void *rstr)
{
    unsigned int i;
    unsigned int j;
    unsigned int diff = rf_string_length_bytes(sstr) - rf_string_length_bytes(rstr);

    //now replace all the substrings one by one
    for (i = 0; i < number; ++i) {
        //copy in the replacement
        memcpy(
            rf_string_data(s) + buff[i],
            rf_string_data(rstr),
            rf_string_length_bytes(rstr)
        );
        //move all of the contents of the string to fit the replacement
        memmove(
            rf_string_data(s) + buff[i] + rf_string_length_bytes(rstr),
            rf_string_data(s) + buff[i] + rf_string_length_bytes(sstr),
            rf_string_length_bytes(s) - (buff[i] + rf_string_length_bytes(sstr))
        );
        //reduce bytelength
        rf_string_length_bytes(s) -= diff;
        //also decrease all the subsequent found byte positions
        for (j = i + 1; j < number; ++j) {
            buff[j] = buff[j] - diff;
        }
    }
}

i_INLINE_DECL void replace_equal(void *s,
                                 const uint32_t *buff,
                                 uint32_t number,
                                 const void *rstr)
{
    unsigned int i;
    for(i = 0; i < number; ++i) {
        memcpy(
            rf_string_data(s) + buff[i],
            rf_string_data(rstr),
            rf_string_length_bytes(rstr)
        );
    }
}

#endif
