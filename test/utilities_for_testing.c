#include <refu.h>
#include "utilities_for_testing.h"

void setup_generic_tests()
{
    rf_init(LOG_TARGET_STDOUT, NULL, LOG_DEBUG);
}

void teardown_generic_tests()
{
    rf_deinit();
}

void setup_invalid_args_tests()
{
    rf_init(LOG_TARGET_FILE, "refuclib.log", LOG_DEBUG);
}

void teardown_invalid_args_tests()
{
    rf_deinit();
}
