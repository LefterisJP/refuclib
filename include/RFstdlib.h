/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STDLIB_H
#define RF_STDLIB_H

#include <stdint.h>
struct RFstring;

void rf_stdlib_print_int64(int64_t n);
void rf_stdlib_print_uint64(uint64_t n);
void rf_stdlib_print_double(double n);
void rf_stdlib_print_string(const struct RFstring *s);

#endif
