#include <RFstdlib.h>

#include <stdio.h>
#include <String/rf_str_common.h>

void rf_stdlib_print_int(int64_t n)
{
    printf("%"PRId64, n);
    fflush(stdout);
}
void rf_stdlib_print_string(const struct RFstring *s)
{
    printf(RF_STR_PF_FMT, RF_STR_PF_ARG(s));
    fflush(stdout);
}
