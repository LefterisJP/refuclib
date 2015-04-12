#ifndef RF_PERSISTENT_BUFFERS_H
#define RF_PERSISTENT_BUFFERS_H

/**
 * Some persistent buffers that get activated at library initialization
 */

//for bool
    #include <Definitions/retcodes.h>
//for RFbuffer
    #include <Utils/buffer.h>
//for thread specific attribute
    #include <Definitions/threadspecific.h>

extern i_THREAD__ struct RFbuffer _tsbuffa;
#define TSBUFFA (&_tsbuffa)

/**
 * Activate thread-specific persistent buffers
 */
bool rf_persistent_buffers_activate_ts();
/**
 * Activate all persistent buffers
 */
bool rf_persistent_buffers_activate();
/**
 * Dactivate thread-specific persistent buffers
 */
void rf_persistent_buffers_deactivate_ts();
/**
 * Dactivate all persistent buffers
 */
void rf_persistent_buffers_deactivate();
#endif
