#ifndef RF_BUFFER_H
#define RF_BUFFER_H

//for bool
    #include <Definitions/retcodes.h>
//for inline
    #include <Definitions/inline.h> 
//for alloc realloc
    #include <Utils/memory.h> 
//for memcpy
    #include <string.h>
typedef struct RFbuffer
{
    char* buff;
    size_t size;
    unsigned int index;
}RFbuffer;

i_INLINE_DECL bool rf_buffer_init(RFbuffer* b, size_t size)
{
    b->size = size;
    b->index = 0;
    RF_MALLOC(b->buff, size, false);
    return true;
}

i_INLINE_DECL void rf_buffer_deinit(RFbuffer* b)
{
    free(b->buff);
}

i_INLINE_DECL char* rf_buffer_ptr(RFbuffer* b)
{
    return b->buff + b->index;
}

i_INLINE_DECL bool rf_buffer_increase(RFbuffer* b, size_t size)
{
    if(size <= b->index + b->size)
    {
        return true;
    }

    RF_REALLOC(b->buff, char, size, false);
    b->size = size;
    return true;
}

i_INLINE_DECL bool rf_buffer_copy(RFbuffer* b, const char* src, size_t len)
{
    if(!rf_buffer_increase(b, len))
    {
        return false;
    }

    memcpy(rf_buffer_ptr(b), src, len);
    return true;
}

i_INLINE_DECL size_t rf_buffer_size(RFbuffer* b)
{
    return b->size - b->index;
}


i_INLINE_DECL void rf_buffer_set_index(RFbuffer* b, unsigned int index)
{
    b->index = index;
}

i_INLINE_DECL unsigned int rf_buffer_index(RFbuffer* b)
{
    return b->index;
}

#define rf_buffer_size_u32(i_BUFF_) \
    (rf_buffer_size(i_BUFF_) / sizeof(uint32_t))

#define rf_buffer_ptr_u32(i_BUFF_, i_IND_)       \
    ((uint32_t*)rf_buffer_ptr(i_BUFF_))[i_IND_]

#define rf_buffer_increase_u32(i_BUFF_, i_SIZE_) \
    rf_buffer_increase(i_BUFF_, (i_SIZE_) * sizeof(uint32_t))


#endif
