#include "internal_mod.ph"

#include <Utils/buffer.h>


i_THREAD__ RF_Buffer _tsbuffa;

bool module_internal_init_ts()
{
    if(!rfBuffer_Init(&_tsbuffa, 512))
    {
        return false;
    }
    return true;
}
bool module_internal_init()
{
    return module_internal_init_ts();
}

