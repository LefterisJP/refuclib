#include <Persistent/buffers.h>


i_THREAD__ struct RFbuffer _tsbuffa;

bool rf_persistent_buffers_activate_ts()
{
    // TSBUFFA is used for various string operations but not for temporary strings
    // would be good if it was slowly removed in favour of the much more safe
    // rf_strings_buffer_ctx
    if (!rf_buffer_init(&_tsbuffa, 1024, NULL)) {
        return false;
    }
    return true;
}

bool rf_persistent_buffers_activate()
{
    return rf_persistent_buffers_activate_ts();
}

void rf_persistent_buffers_deactivate_ts()
{
    rf_buffer_deinit(&_tsbuffa);
}

void rf_persistent_buffers_deactivate()
{
    rf_persistent_buffers_deactivate_ts();
}
