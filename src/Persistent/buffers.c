#include <Persistent/buffers.h>


i_THREAD__ struct RFbuffer _ts_workbuf;

bool rf_persistent_buffers_activate_ts(size_t ts_buffer_size)
{
    if (!rf_buffer_init(&_ts_workbuf, ts_buffer_size, NULL)) {
        return false;
    }
    return true;
}

bool rf_persistent_buffers_activate(size_t ts_buffer_size)
{
    return rf_persistent_buffers_activate_ts(ts_buffer_size);
}

void rf_persistent_buffers_deactivate_ts()
{
    rf_buffer_deinit(&_ts_workbuf);
}

void rf_persistent_buffers_deactivate()
{
    rf_persistent_buffers_deactivate_ts();
}
