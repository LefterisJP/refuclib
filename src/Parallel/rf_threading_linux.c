/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <Parallel/rf_threading.h>

int rf_thread_get_id()
{
    return pthread_self();
}

i_INLINE_INS bool rf_mutex_init(struct RFmutex *mutex);
i_INLINE_INS bool rf_mutex_lock(struct RFmutex *mutex);
i_INLINE_INS bool rf_mutex_unlock(struct RFmutex *mutex);
i_INLINE_INS bool rf_mutex_deinit(struct RFmutex *mutex);
