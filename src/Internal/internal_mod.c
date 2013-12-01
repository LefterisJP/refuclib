#include "internal_mod.ph"

#include <Utils/buffer.h>


i_THREAD__ RFbuffer _tsbuffa;

bool module_internal_init_ts()
{
    if(!rf_buffer_init(&_tsbuffa, 512))
    {
        return false;
    }
    return true;
}
bool module_internal_init()
{
    return module_internal_init_ts();
}

