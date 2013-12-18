#include <refu.h>
#include "utilities_for_testing.h"

void setup_string_tests()
{
    rf_init("refuclib.log", 0, LOG_DEBUG);
}

void teardown_string_tests()
{
    rf_deinit();
}


void setup_generic_tests()
{
    rf_init("refuclib.log", 0, LOG_DEBUG);
}

void teardown_generic_tests()
{
    rf_deinit();
}
