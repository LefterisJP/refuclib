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


#define ADD_TYPE_TO_STRING(str_, type_, typemax_, typefs_, rc_, val_)   \
    rc_ = snprintf(rf_string_data(str_) + rf_string_length_bytes(str_),     \
                   typemax_,                                            \
                   typefs_, val_);                                      \
    if(rc_ < 0 || rc_ >= typemax_)                                      \
    {                                                                   \
        RF_ERROR("During appending "type_" to a string snprintf failed"); \
        return false;                                                   \
    }                                                                   \
    rf_string_length_bytes(str_) += rc_;                                   \


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
                       false);
    rf_string_generic_append(s, other, bytes_to_copy);
    return true;
}

i_INLINE_DECL bool replace_intro(void* s, uint32_t* number, 
                                 const void* sstr,
                                 enum RFstring_matching_options options)
{
    //TODO: Change this with a thread local buffer or thread local string
    struct RFstringx temp;//just a temporary string for finding the occurences
    //will keep the number of found instances of the substring
    //if the substring string is not even found return false
    uint32_t foundN = 0;
    bool ret = true;
    if(rf_string_find_byte_pos(s, sstr, options) == RF_FAILURE)
    {
        return false;
    }

    //if the given num is 0 just make sure we replace all
    if(*number == 0)
    {
        *number = UINT_MAX;
    }
    //find how many occurences exist
    if(!rf_stringx_from_string_in(&temp, s))
    {
        return false;
    }
    while((rf_buffer_ptr_u32(TSBUFFA, foundN) = rf_string_find_byte_pos(
               &temp, sstr, options))  != RF_FAILURE)
    {
        int32_t move = (rf_buffer_ptr_u32(TSBUFFA, foundN) + 
                        rf_string_length_bytes(sstr)
        );
        rf_buffer_ptr_u32(TSBUFFA, foundN) = (
            rf_buffer_ptr_u32(TSBUFFA, foundN) + temp.bIndex
        );
        temp.bIndex += move;
        rf_string_data(&temp) += move;
        rf_string_length_bytes(&temp) -= move;
        foundN++;
        //if buffer is in danger of overflow realloc it
        if(foundN >= rf_buffer_size_u32(TSBUFFA))
        {
            if(rf_buffer_increase_u32(TSBUFFA, rf_buffer_size_u32(TSBUFFA) * 2))
            {
                RF_ERROR("Not enough memory to increase internal buffer");
                ret = false;
                goto cleanup;
            }
        }
        //if we found the required number of occurences break;
        if(foundN >= *number)
        {
            break;
        }
    }

    //don't replace more than possible
    if(*number > foundN)
    {
        *number = foundN;
    }

  cleanup:
    rf_stringx_deinit(&temp);
    return ret;
}

i_INLINE_DECL void replace_greater(void* s, uint32_t number,
                                   const void* sstr, const void* rstr)
{
    unsigned int i, j;
    unsigned int diff = rf_string_length_bytes(rstr) - rf_string_length_bytes(sstr);
    //now replace all the substrings one by one
    for(i = 0; i < number; i ++)
    {
        memmove(
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i) + rf_string_length_bytes(sstr)+ diff,
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i) + rf_string_length_bytes(sstr),
            rf_string_length_bytes(s) - (rf_buffer_ptr_u32(TSBUFFA, i) + rf_string_length_bytes(sstr))
        );
        //copy in the replacement
        memcpy(
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i),
            rf_string_data(rstr),
            rf_string_length_bytes(rstr)
        );
        //also increase the original size
        rf_string_length_bytes(s) += diff;
        //also increase all the subsequent found byte positions
        for(j = i + 1; j < number; j ++)
        {
            rf_buffer_ptr_u32(TSBUFFA, j) = rf_buffer_ptr_u32(TSBUFFA, j) + diff;
        }
    }
}


i_INLINE_DECL void replace_lesser(void* s, uint32_t number,
                                  const void* sstr, const void* rstr)
{
    unsigned int i, j;
    unsigned int diff = rf_string_length_bytes(sstr) - rf_string_length_bytes(rstr);

    //now replace all the substrings one by one
    for(i = 0; i < number; i ++)
    {
        //copy in the replacement
        memcpy(
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i),
            rf_string_data(rstr),
            rf_string_length_bytes(rstr)
        );
        //move all of the contents of the string to fit the replacement
        memmove(
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i) + rf_string_length_bytes(rstr),
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i) + rf_string_length_bytes(sstr),
            rf_string_length_bytes(s) - (rf_buffer_ptr_u32(TSBUFFA, i) + rf_string_length_bytes(sstr))
        );
        //reduce bytelength
        rf_string_length_bytes(s) -= diff;
        //also decrease all the subsequent found byte positions
        for(j = i + 1; j < number; j ++)
        {
            rf_buffer_ptr_u32(TSBUFFA, j) = rf_buffer_ptr_u32(TSBUFFA, j) - diff;
        }
    }
}

i_INLINE_DECL void replace_equal(void* s, uint32_t number, const void* rstr)
{
    unsigned int i;
    for(i = 0; i < number; i ++)
    {
        memcpy(
            rf_string_data(s) + rf_buffer_ptr_u32(TSBUFFA, i),
            rf_string_data(rstr),
            rf_string_length_bytes(rstr)
        );
    }
}

#endif
