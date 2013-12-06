#include "rf_internal_mod.ph"

#include <Utils/buffer.h>


i_THREAD__ struct RFbuffer _tsbuffa;

bool rf_internal_activate_ts()
{
    if(!rf_buffer_init(&_tsbuffa, 512))
    {
        return false;
    }
    return true;
}
bool rf_internal_activate()
{
    return rf_internal_activate_ts();
}

void rf_internal_deactivate_ts()
{
    rf_buffer_deinit(&_tsbuffa);
}
void rf_internal_deactivate()
{
    rf_internal_deactivate_ts();
}

