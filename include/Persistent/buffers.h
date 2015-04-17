#ifndef RF_PERSISTENT_BUFFERS_H
#define RF_PERSISTENT_BUFFERS_H

/**
 * Some persistent buffers that get activated at library initialization
 */

//for bool
    #include <Definitions/retcodes.h>
//for RFbuffer
    #include <Data_Structures/buffer.h>
//for thread specific attribute
    #include <Definitions/threadspecific.h>

extern i_THREAD__ struct RFbuffer _ts_workbuf;
#define RF_TSBUFF (&_ts_workbuf)

/**
 * Activate thread-specific persistent buffers
 *
 * The thread specific work buffer is an RFbuffer used for temporary storage
 * of various kinds. Size must be checked and realloc handled manually in all
 * cases. @ref RF_TSBUFF will give you a pointer to this buffer.
 *
 * For temporary strings we use the rf_strings_buffer_ctx_XXX() family
 * of functions
 *
 * @param ts_buffer_size        The initial size of the thread specific
 *                              work buffer in bytes
 * @return                      true for succesful activation and false otherwise
 */
bool rf_persistent_buffers_activate_ts(size_t ts_buffer_size);
/**
 * Activate all persistent buffers
 *
 * @see rf_persistent_buffers_activate_ts()
 */
bool rf_persistent_buffers_activate(size_t ts_buffer_size);
/**
 * Dactivate thread-specific persistent buffers
 */
void rf_persistent_buffers_deactivate_ts();
/**
 * Dactivate all persistent buffers
 */
void rf_persistent_buffers_deactivate();
#endif
