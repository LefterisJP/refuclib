#ifndef RF_PERSISTENT_BUFFERS_H
#define RF_PERSISTENT_BUFFERS_H

/**
 * Some persisten buffers that get activated at library initialization
 */

//for bool
    #include <Definitions/retcodes.h>
//for RFbuffer
    #include <Utils/buffer.h>
//for thread specific attribute
    #include <Definitions/threadspecific.h>

extern i_THREAD__ struct RFbuffer _tsbuffa;
#define TSBUFFA (&_tsbuffa)

bool rf_persistent_buffers_activate_ts();
bool rf_persistent_buffers_activate();
void rf_persistent_buffers_deactivate_ts();
void rf_persistent_buffers_deactivate();

/**
 * Create an RFstring from a buffer.
 *
 * The user should return the buffer to the state before the call of
 * this function as soon as possible to free the string
 *
 * @param b        The buffer from which to create the string
 * @param other    The string from which to create this str
 */
const struct RFstring *rf_persistent_buffer_create_str_from_str(
    struct RFbuffer *b,
    const struct RFstring *other);

#endif
