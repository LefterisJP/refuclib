#ifndef RF_STRING_CONVERSION_PH
#define RF_STRING_CONVERSION_PH


//for inline
    #include <Definitions/inline.h>
//for internal buffer
    #include "../Internal/internal_mod.ph"
/**
 ** @memberof RF_String
 ** @brief Creates a Cstring representation of the string
 **
 ** @isinherited{StringX}
 ** Since RF_String is not not null terminated this uses an internal
 ** buffer for converting it to a null terminated c string. This avoids
 ** malloc().
 **
 ** After you are done with the string remember to call 
 ** @ref rfString_Cstr_ibuff_pop() so that the buffer can be freed
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
i_INLINE_DECL char* rfString_Cstr_ibuff_push(const void* s, unsigned int *index)
{
    char* ret;
    ret = rfBuffer_Ptr(TSBUFFA);
    if(index)
    {
        *index = TSBUFFA->index;
    }
    if(!rfBuffer_Copy(TSBUFFA, rfString_Data(s), rfString_ByteLength(s)))
    {
        return NULL;
    }
    rfBuffer_Ptr(TSBUFFA)[rfString_ByteLength(s)] = '\0';
    TSBUFFA->index += rfString_ByteLength(s) + 1;
    return ret;
}

/**
 ** Just a wrapper for rfBuffer_SetIndex()
 */
i_INLINE_DECL void rfString_Cstr_ibuff_pop(unsigned int index)
{
    rfBuffer_SetIndex(TSBUFFA, index);
}


#endif
