#include <refu.h>

void setup_string_tests()
{
    rf_init("refuclib.log", 0, LOG_DEBUG);
}

void teardown_string_tests()
{
    rf_deinit();
}
