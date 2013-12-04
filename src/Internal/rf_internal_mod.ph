#ifndef RF_INTERNAL_PH
#define RF_INTERNAL_PH

//for bool
    #include <Definitions/retcodes.h>
//for RFbuffer
    #include <Utils/buffer.h>
//for thread specific attribute
    #include <Definitions/threadspecific.h>

extern i_THREAD__ RFbuffer _tsbuffa;
#define TSBUFFA (&_tsbuffa)

bool rf_internal_activate_ts();
bool rf_internal_activate();

#endif
