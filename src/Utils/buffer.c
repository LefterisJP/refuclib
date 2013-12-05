#include <Utils/buffer.h>

i_INLINE_INS bool rf_buffer_init(RFbuffer* b, size_t size);
i_INLINE_INS void rf_buffer_deinit(RFbuffer* b);
i_INLINE_INS bool rf_buffer_increase(RFbuffer* b, size_t size);
i_INLINE_INS bool rf_buffer_copy(RFbuffer* b, const char* src, size_t len);
i_INLINE_INS char* rf_buffer_ptr(RFbuffer* b);
i_INLINE_INS size_t rf_buffer_size(RFbuffer* b);
i_INLINE_INS void rf_buffer_set_index(RFbuffer* b, unsigned int index);
i_INLINE_INS unsigned int rf_buffer_index(RFbuffer* b);
