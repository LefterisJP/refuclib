/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_REGEX_H
#define RF_STRING_REGEX_H

#include <Data_Structures/darray.h>
#include <String/rf_str_decl.h>
#include <stdbool.h>

struct RFre;

struct string_arr {darray(struct RFstring);};

struct RFre *rfre_compile(const struct RFstring *pattern);
bool rfre_match(struct RFre *re,
                const struct RFstring *subject,
                struct string_arr *matches);

#endif
