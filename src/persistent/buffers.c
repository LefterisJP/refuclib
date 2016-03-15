#include <rflib/persistent/buffers.h>

i_THREAD__ struct RFmbuffer i_ts_mbuf;
i_THREAD__ struct RFsbuffer i_ts_sbuf;

bool rf_persistent_buffers_activate_ts(size_t ts_mbuffer_size, size_t ts_sbuffer_size)
{
    if (!rf_mbuffer_init(&i_ts_mbuf, ts_mbuffer_size)) {
        return false;
    }
    if (!rf_sbuffer_init(&i_ts_sbuf, ts_sbuffer_size, NULL)) {
        return false;
    }
    return true;
}

bool rf_persistent_buffers_activate(size_t ts_mbuffer_size, size_t ts_sbuffer_size)
{
    return rf_persistent_buffers_activate_ts(ts_mbuffer_size, ts_sbuffer_size);
}

void rf_persistent_buffers_deactivate_ts()
{
    rf_sbuffer_deinit(&i_ts_sbuf);
    rf_mbuffer_deinit(&i_ts_mbuf);
}

void rf_persistent_buffers_deactivate()
{
    rf_persistent_buffers_deactivate_ts();
}
