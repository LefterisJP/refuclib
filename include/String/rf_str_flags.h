/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_FLAGS_H
#define RF_STRING_FLAGS_H

/**
 * Bitflags options for various RFstring
 * matching functions. They can be combined depending
 * on the specific function. Check each respective function's
 * documentation for details
 */
enum RFstring_matching_options {
    RF_CASE_IGNORE = 0x1, /*!< The case should not be matched
                            in the string operation. For now this only
                            works for english language characters */
    RF_MATCH_WORD = 0x2, /*!< Denotes that the exact word should be
                           matche and that it should be separated by
                           whitespace characters */
    RF_STRINGX_ARGUMENT = 0x4, /*!< At least one of the function's arguments
                                 is an RFstringx instead of a simple RFstring */
    RF_STRING_DEPENDENT = 0x8, /*!< At least one of the function's arguments
                                 is an RFstring without an initialized buffer.
                                 The function should make the passed string
                                 point to another string and depend on it.
                                 The function's documentation should clearly
                                 state what string would that be */

};



//flags for string formatting
#define RF_FORMATFLAG_LJUSTIFY   0x1
#define RF_FORMATFLAG_SIGN       0x2
#define RF_FORMATFLAG_NOSIGN     0x4
#define RF_FORMATFLAG_SHARP      0x8
#define RF_FORMATFLAG_PADZEROES  0x10
#define RF_FORMATFLAG_UPPER      0x20

#endif//include guards end
