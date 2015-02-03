#include <Persistent/buffers.h>


i_THREAD__ struct RFbuffer _tsbuffa;

bool rf_persistent_buffers_activate_ts()
{
    if(!rf_buffer_init(&_tsbuffa, 2048))
    {
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


const struct RFstring *rf_persistent_buffer_create_str_from_str(
    struct RFbuffer *b,
    const struct RFstring *other)
{
    struct RFstring *ret;
    uint32_t other_length = rf_string_length_bytes(other);
    size_t required_size = sizeof(struct RFstring) + other_length;
    rf_buffer_try_increase_size(b, required_size);

    // point the string itself to the buffer
    ret = rf_buffer_current_ptr(b, struct RFstring);
    rf_buffer_move_index(b, 1, struct RFstring);

    // point the string's buffer to the buffer
    rf_string_data(ret) = rf_buffer_current_ptr(b, char);
    rf_string_length_bytes(ret) = other_length;
    memcpy(rf_string_data(ret), rf_string_data(other), other_length);
    rf_buffer_move_index(b, other_length, char);

    return ret;
}
