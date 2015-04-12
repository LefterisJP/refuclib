#include "rf_str_mod.ph"
#include <String/rf_str_common.h>

//just leaving it here in case I need it later
bool rf_string_activate(size_t string_buffer_size)
{
    return rf_strings_buffer_ctx_init(string_buffer_size);
}

void rf_string_deactivate()
{
    rf_strings_buffer_ctx_deinit();
}
