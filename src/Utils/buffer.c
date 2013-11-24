#include <Utils/buffer.h>

i_INLINE_INS bool rfBuffer_Init(RF_Buffer* b, size_t size);
i_INLINE_INS bool rfBuffer_Increase(RF_Buffer* b, size_t size);
i_INLINE_INS bool rfBuffer_Copy(RF_Buffer* b, const char* src, size_t len);
i_INLINE_INS char* rfBuffer_Ptr(RF_Buffer* b);
i_INLINE_INS size_t rfBuffer_Size(RF_Buffer* b);
i_INLINE_INS void rfBuffer_SetIndex(RF_Buffer* b, unsigned int index);
i_INLINE_INS unsigned int rfBuffer_Index(RF_Buffer* b);
