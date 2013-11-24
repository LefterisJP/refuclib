#ifndef RF_INTERNAL_PH
#define RF_INTERNAL_PH

//for bool
    #include <Definitions/retcodes.h>
//for RF_Buffer
    #include <Utils/buffer.h>
//for thread specific attribute
    #include <Definitions/threadspecific.h>

extern i_THREAD__ RF_Buffer _tsbuffa;
#define TSBUFFA (&_tsbuffa)

bool module_internal_init_ts();
bool module_internal_init();

#endif
