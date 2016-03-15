/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/system/rf_system_info_linux.h>

//system info global definition
struct RFsystem_info g_sys_info;

i_INLINE_INS enum RFendianess rf_system_get_endianess();
i_INLINE_INS enum RFendianess rf_system_get_other_endianess();
i_INLINE_INS bool rf_system_has_high_res_timer();
