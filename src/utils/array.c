/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/utils/array.h>

i_INLINE_INS bool rf_array_init_(struct RFarray *a,
                                 size_t element_size,
                                 size_t size);
i_INLINE_INS void rf_array_deinit(struct RFarray *a);
i_INLINE_INS void *rf_array_at_(struct RFarray *a, size_t i, size_t typesize);
i_INLINE_INS size_t rf_array_size_(struct RFarray *a);
i_INLINE_INS void *rf_array_ptrto_index_(struct RFarray *a,
                                         size_t i,
                                         size_t typesize);
