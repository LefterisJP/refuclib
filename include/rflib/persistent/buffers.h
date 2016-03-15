#ifndef RF_PERSISTENT_BUFFERS_H
#define RF_PERSISTENT_BUFFERS_H

#include <rflib/defs/retcodes.h>
#include <rflib/datastructs/mbuffer.h>
#include <rflib/datastructs/sbuffer.h>
#include <rflib/defs/threadspecific.h>

/**
 * Some persistent buffers that get activated at library initialization
 */
extern i_THREAD__ struct RFmbuffer i_ts_mbuf;
#define RF_TSBUFFM (&i_ts_mbuf)
extern i_THREAD__ struct RFsbuffer i_ts_sbuf;
#define RF_TSBUFFS (&i_ts_sbuf)

/**
 * Activate thread-specific persistent buffers
 *
 * The thread specific work buffer is an RFmbuffer used for temporary storage
 * of various different things. @ref RF_TSBUFF will give you a pointer to this buffer.
 *
 *
 * @param ts_mbuffer_size       The initial size of the thread specific
 *                              multiblock work buffer in bytes
 * @param ts_mbuffer_size       The initial size of the thread specific
 *                              single block work buffer in bytes
 * @return                      true for succesful activation and false otherwise
 */
bool rf_persistent_buffers_activate_ts(size_t ts_mbuffer_size, size_t ts_sbuffer_size);

/**
 * Activate all persistent buffers
 *
 * @see rf_persistent_buffers_activate_ts()
 */
bool rf_persistent_buffers_activate(size_t ts_mbuffer_size, size_t ts_sbuffer_size);

/**
 * Dactivate thread-specific persistent buffers
 */
void rf_persistent_buffers_deactivate_ts();

/**
 * Dactivate all persistent buffers
 */
void rf_persistent_buffers_deactivate();
#endif
