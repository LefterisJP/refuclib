#ifndef RF_STRING_CONVERSION_PH
#define RF_STRING_CONVERSION_PH

/*------------- Outside Module inclusion -------------*/
#include <Definitions/inline.h> //for inline
#include "../Internal/internal_mod.ph" //for internal buffer
/*------------- End of includes -------------*/


/**
 ** @brief Creates a Cstring representation of the string
 **
 ** @isinherited{StringX}
 ** Since RFstring is not not null terminated this uses an internal
 ** buffer for converting it to a null terminated c string. This avoids
 ** malloc().
 **
 ** After you are done with the string remember to call 
 ** @ref rf_string_cstr_ibuff_pop() so that the buffer can be freed
 **
 ** @warning Make sure to copy the null terminated string if you want to use
 ** it somewhere else later, since the buffer will be overwritten by the next
 ** routine that uses it.
 **
 ** @param s         The string whose cstring repesentation to get.
 **                  @inhtype{String,StringX}
 ** @param index     Pass an index here to get the index the buffer should
 **                  be returned to. Can be NULL if you don't need it.
 ** @return          Returns the pointer to the allocated c string
 **/
i_INLINE_DECL char* rf_string_cstr_ibuff_push(const void* s, unsigned int *index)
{
    char* ret;
    ret = rf_buffer_ptr(TSBUFFA);
    if(index)
    {
        *index = TSBUFFA->index;
    }
    if(!rf_buffer_copy(TSBUFFA, rf_string_data(s), rf_string_length_bytes(s)))
    {
        return NULL;
    }
    rf_buffer_ptr(TSBUFFA)[rf_string_length_bytes(s)] = '\0';
    TSBUFFA->index += rf_string_length_bytes(s) + 1;
    return ret;
}

/**
 ** Just a wrapper for rf_buffer_set_index()
 */
i_INLINE_DECL void rf_string_cstr_ibuff_pop(unsigned int index)
{
    rf_buffer_set_index(TSBUFFA, index);
}


#endif
