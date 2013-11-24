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
typedef struct RF_Buffer
{
    char* buff;
    size_t size;
    unsigned int index;
}RF_Buffer;

i_INLINE_DECL bool rfBuffer_Init(RF_Buffer* b, size_t size)
{
    b->size = size;
    b->index = 0;
    RF_MALLOC(b->buff, size, false);
    return true;
}

i_INLINE_DECL char* rfBuffer_Ptr(RF_Buffer* b)
{
    return b->buff + b->index;
}

i_INLINE_DECL bool rfBuffer_Increase(RF_Buffer* b, size_t size)
{
    if(size <= b->index + b->size)
    {
        return true;
    }

    RF_REALLOC(b->buff, char, size, false);
    b->size = size;
    return true;
}

i_INLINE_DECL bool rfBuffer_Copy(RF_Buffer* b, const char* src, size_t len)
{
    if(!rfBuffer_Increase(b, len))
    {
        return false;
    }

    memcpy(rfBuffer_Ptr(b), src, len);
    return true;
}

i_INLINE_DECL size_t rfBuffer_Size(RF_Buffer* b)
{
    return b->size - b->index;
}


i_INLINE_DECL void rfBuffer_SetIndex(RF_Buffer* b, unsigned int index)
{
    b->index = index;
}

i_INLINE_DECL unsigned int rfBuffer_Index(RF_Buffer* b)
{
    return b->index;
}

#define rfBuffer_Size_u32(i_BUFF_) \
    (rfBuffer_Size(i_BUFF_) / sizeof(uint32_t))

#define rfBuffer_Ptr_u32(i_BUFF_, i_IND_)       \
    ((uint32_t*)rfBuffer_Ptr(i_BUFF_))[i_IND_]

#define rfBuffer_Increase_u32(i_BUFF_, i_SIZE_) \
    rfBuffer_Increase(i_BUFF_, (i_SIZE_) * sizeof(uint32_t))


#endif
