#include "rf_internal_mod.ph"

#include <Utils/buffer.h>


i_THREAD__ RFbuffer _tsbuffa;

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

