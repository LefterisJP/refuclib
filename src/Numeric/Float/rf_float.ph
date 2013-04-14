/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
**
**
**
** -- Float/rf_float.ph
** This file contains float related functions that allow
** us to transform a float into ascii. In the beginning I attempted
** my own implementation of converting a double to a string but finally went with David M. Gay.'s
** dtoa() algorithm.  You can get your own copy and/or more information
** here: http://www.netlib.org/fp/
**
** It's used by the String/format.h functions.
** It's a private header since there is no need to expose this functionality to the user.
** If more functions are added this may change
**
---------------------For internal library include make sure to have----------------------------

---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_FLOAT_PH
#define RF_FLOAT_PH

//I am just writting the documentation of dtoa to match the rest of the Refu library format
// and as a way to better udnerstand its inner workings

//! @brief Turns a double into a string
//! @author David M. Gay.
//! @copyright The author of this software is David M. Gay.
//!
//! Copyright (c) 1991, 2000, 2001 by Lucent Technologies.
//!
//! Permission to use, copy, modify, and distribute this software for any
//! purpose without fee is hereby granted, provided that this entire notice
//! is included in all copies of any software which is or includes a copy
//! or modification of this software and in all copies of the supporting
//! documentation for such software.
//!
//! THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
//! WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
//! REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
//! OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
//!
//! @details
//! Trailing zeroes are supressed from the returned string. If d is +-Infinity or NaN,
//! then *decpt is set to 9999.
//! @param mode Possible values are:
//! + @c 0: Will return the shortest string that yields d when read in and rounded to nearest.
//! + @c 1: Like 0, but with Steele & White stopping rule;e.g. with IEEE P754 arithmetic , mode 0 gives
//! 1e23 whereas mode 1 gives 9.999999999999999e22.
//! + @c 2: Will return a string with @c ndigits maximum number of significand digits (max(1,ndigits)).
//!			Trailing zeroes are suppressed.
//! + @c 3: Will return a string with @c ndigits @b after the decimal point.
//!         Trailing zeros are suppressed, and @c ndigits can be negative.
//! + @c 4,5: similar to 2 and 3, respectively, but (in
//!			round-nearest mode) with the tests of mode 0 to
//!			possibly return a shorter string that rounds to d.
//!			With IEEE arithmetic and compilation with
//!			-DHonor_FLT_ROUNDS, modes 4 and 5 behave the same
//!			as modes 2 and 3 when FLT_ROUNDS != 1.
//! + @c 6-9: Debugging modes similar to mode - 4:  don't try
//!			fast floating-point estimate (if applicable).
//!
//! Values of mode other than 0-9 are treated as mode 0.
//! Sufficient space is allocated to the return value
//! to hold the suppressed trailing zeros.
//! @param ndigits The number of digits after the decimal point
//! @param decpt The position of the radix character relative to the beginning of the string shall be stored
//! in the integer pointed to by decpt (negative means to the left of the returned digits).If value is zero,
//! it is unspecified whether the integer pointed to by decpt would be 0 or 1. The radix character shall not be included in the returned string.
//! @param sign If the sign of the result is negative, the integer pointed to by sign shall be non-zero; otherwise, it shall be 0.
//! @param rve If not null, *rve is set to point to the end of the return value
//! @return A null terminated string containing the digits of @c d. No memory is allocated, unless it is a pathological case.
char* doubleToStr(double d, int mode, int ndigits,int *decpt, int *sign, char **rve);



#endif//include guards end
