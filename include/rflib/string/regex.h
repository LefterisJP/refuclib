/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_REGEX_H
#define RF_STRING_REGEX_H

#include <rflib/datastructs/darray.h>
#include <rflib/string/decl.h>
#include <stdbool.h>

struct RFre;

struct string_arr {darray(struct RFstring);};

/**
 * Holds the results of a match on an regular expression
 */
struct RFre_match {
    //! An array of matched strings.
    //!
    //! If used with rfre_match_single() then its first entry will be the entire
    //! matched string and any subsequent entry will be for each captured group
    //! in the given pattern.
    //!
    //! If used with rfre_match_all() it's the same only repeated by the number
    //! of matches.
    struct string_arr matches;
    //! The number of capture groups the regular expression pattern had.
    uint32_t captures_num;
};

void rfre_match_deinit(struct RFre_match *mdata);

struct RFre *rfre_compile(const struct RFstring *pattern);
void rfre_destroy(struct RFre *re);

bool rfre_match_single(struct RFre *re,
                       const struct RFstring *subject,
                       struct RFre_match *mdata);
bool rfre_match_all(struct RFre *re,
                    const struct RFstring *subject,
                    struct RFre_match *mdata);



#endif
