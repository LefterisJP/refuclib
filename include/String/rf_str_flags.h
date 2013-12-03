/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

#ifndef RF_STRING_FLAGS_H
#define RF_STRING_FLAGS_H

/**
 ** Bitflags options for various RFstring 
 ** matching functions. They can be combined depending
 ** on the specific function. Check each respective function's
 ** documentation for details
 */
enum RFstring_matching_options {
    RF_CASE_IGNORE = 0x1, /*!< The case should not be matched
                            in the string operation. For now this only
                            works for english language characters */
    RF_MATCH_WORD = 0x2, /*!< Denotes that the exact word should be
                           matche and that it should be separated by 
                           whitespace characters */
    RF_STRINGX_ARGUMENT = 0x4, /*!< At least one of the function's arguments
                                 are RFstringx instead of a simple RFstring */
};



//flags for string formatting
#define RF_FORMATFLAG_LJUSTIFY   0x1
#define RF_FORMATFLAG_SIGN       0x2
#define RF_FORMATFLAG_NOSIGN     0x4
#define RF_FORMATFLAG_SHARP      0x8
#define RF_FORMATFLAG_PADZEROES  0x10
#define RF_FORMATFLAG_UPPER      0x20

#endif//include guards end
