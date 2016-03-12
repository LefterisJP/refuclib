#include <RFstdlib.h>

#include <stdio.h>
#include <String/rf_str_common.h>

void rf_stdlib_print_int64(int64_t n)
{
    printf("%"PRId64, n);
    fflush(stdout);
}

void rf_stdlib_print_uint64(uint64_t n)
{
    printf("%"PRIu64, n);
    fflush(stdout);
}

void rf_stdlib_print_double(double n)
{
    printf("%f", n);
    fflush(stdout);
}

void rf_stdlib_print_string(const struct RFstring *s)
{
    printf(RFS_PF, RFS_PA(s));
    fflush(stdout);
}
