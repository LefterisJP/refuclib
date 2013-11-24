/* Code common for string manipulation sources */

#ifndef RF_STRING_MANIPULATION_PH
#define RF_STRING_MANIPULATION_PH

//for inline
    #include <Definitions/inline.h>
//for the sanity macros
    #include <Utils/sanity.h>
//for UINT_MAX
    #include <limits.h>
//for rfStringX_FromString_IN()
    #include <String/corex.h>
//for stringx realloc
    #include "common.ph"

#define ADD_TYPE_TO_STRING(str_, type_, typemax_, typefs_, rc_, val_)   \
    rc_ = snprintf(rfString_Data(str_) + rfString_ByteLength(str_),     \
                   typemax_,                                            \
                   typefs_, val_);                                      \
    if(rc_ < 0 || rc_ >= typemax_)                                      \
    {                                                                   \
        RF_ERROR("During appending "type_" to a string snprintf failed"); \
        return false;                                                   \
    }                                                                   \
    rfString_ByteLength(str_) += rc_;                                   \


i_INLINE_DECL bool rfStringGEN_Prepend(void* thisstr, const char* other,
                                       unsigned int orig_size,
                                       unsigned int other_size)
{
    int i;
    //move the old string to the end of the buffer
    for(i = orig_size; i >=0 ; i--)
    {
        rfString_Data(thisstr)[i + other_size] = rfString_Data(thisstr)[i];
    }
    //add the string to the start
    memcpy(
        rfString_Data(thisstr),
        other,
        other_size
   );
   rfString_ByteLength(thisstr) += other_size;
   return true;
}

/* checks if adding a string will need buffer reallocation */
i_INLINE_DECL bool rfStringXGEN_Append(RF_StringX* s,
                                       const char* other,
                                       unsigned int bytes_to_copy)
{
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(s,
                       rfString_ByteLength(s) + bytes_to_copy,
                       false);
    rfStringGEN_Append(s, other, bytes_to_copy);
    return true;
}

i_INLINE_DECL bool replace_intro(void* s, uint32_t* number, 
                                 const void* sstr, const char options)
{
    //TODO: Change this with a thread local buffer or thread local string
    RF_StringX temp;//just a temporary string for finding the occurences
    //will keep the number of found instances of the substring
    //if the substring string is not even found return false
    uint32_t foundN = 0;
    bool ret = true;
    if(rfString_FindBytePos(s, sstr, options) == RF_FAILURE)
    {
        return false;
    }

    //if the given num is 0 just make sure we replace all
    if(*number == 0)
    {
        *number = UINT_MAX;
    }
    //find how many occurences exist
    if(!rfStringX_FromString_IN(&temp, s))
    {
        return false;
    }
    while((rfBuffer_Ptr_u32(TSBUFFA, foundN) = rfString_FindBytePos(
               &temp, sstr, options))  != RF_FAILURE)
    {
        int32_t move = (rfBuffer_Ptr_u32(TSBUFFA, foundN) + 
                        rfString_ByteLength(sstr)
        );
        rfBuffer_Ptr_u32(TSBUFFA, foundN) = (
            rfBuffer_Ptr_u32(TSBUFFA, foundN) + temp.bIndex
        );
        temp.bIndex += move;
        rfString_Data(&temp) += move;
        rfString_ByteLength(&temp) -= move;
        foundN++;
        //if buffer is in danger of overflow realloc it
        if(foundN >= rfBuffer_Size_u32(TSBUFFA))
        {
            if(rfBuffer_Increase_u32(TSBUFFA, rfBuffer_Size_u32(TSBUFFA) * 2))
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
    rfStringX_Deinit(&temp);
    return ret;
}

i_INLINE_DECL void replace_greater(void* s, uint32_t number,
                                   const void* sstr, const void* rstr)
{
    unsigned int i, j;
    unsigned int diff = rfString_ByteLength(rstr) - rfString_ByteLength(sstr);
    //now replace all the substrings one by one
    for(i = 0; i < number; i ++)
    {
        memmove(
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i) + rfString_ByteLength(sstr)+ diff,
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i) + rfString_ByteLength(sstr),
            rfString_ByteLength(s) - (rfBuffer_Ptr_u32(TSBUFFA, i) + rfString_ByteLength(sstr))
        );
        //copy in the replacement
        memcpy(
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i),
            rfString_Data(rstr),
            rfString_ByteLength(rstr)
        );
        //also increase the original size
        rfString_ByteLength(s) += diff;
        //also increase all the subsequent found byte positions
        for(j = i + 1; j < number; j ++)
        {
            rfBuffer_Ptr_u32(TSBUFFA, j) = rfBuffer_Ptr_u32(TSBUFFA, j) + diff;
        }
    }
}


i_INLINE_DECL void replace_lesser(void* s, uint32_t number,
                                  const void* sstr, const void* rstr)
{
    unsigned int i, j;
    unsigned int diff = rfString_ByteLength(sstr) - rfString_ByteLength(rstr);

    //now replace all the substrings one by one
    for(i = 0; i < number; i ++)
    {
        //copy in the replacement
        memcpy(
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i),
            rfString_Data(rstr),
            rfString_ByteLength(rstr)
        );
        //move all of the contents of the string to fit the replacement
        memmove(
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i) + rfString_ByteLength(rstr),
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i) + rfString_ByteLength(sstr),
            rfString_ByteLength(s) - (rfBuffer_Ptr_u32(TSBUFFA, i) + rfString_ByteLength(sstr))
        );
        //reduce bytelength
        rfString_ByteLength(s) -= diff;
        //also decrease all the subsequent found byte positions
        for(j = i + 1; j < number; j ++)
        {
            rfBuffer_Ptr_u32(TSBUFFA, j) = rfBuffer_Ptr_u32(TSBUFFA, j) - diff;
        }
    }
}

i_INLINE_DECL void replace_equal(void* s, uint32_t number, const void* rstr)
{
    unsigned int i;
    for(i = 0; i < number; i ++)
    {
        memcpy(
            rfString_Data(s) + rfBuffer_Ptr_u32(TSBUFFA, i),
            rfString_Data(rstr),
            rfString_ByteLength(rstr)
        );
    }
}

#endif
