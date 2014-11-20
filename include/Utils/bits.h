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
#ifndef RF_UTILS_BITS_H
#define RF_UTILS_BITS_H


/**
 ** Checks if the bit @c BIT_ is set in the variable @c VAR_
 **
 **/
#define RF_BIT_ON(VAR_,BIT_)    ((VAR_) & (1<<(BIT_)))
/**
 ** Checks if the bitflag @c FLAG_ is set in the variable @c VAR_
 **
 **/
#define RF_BITFLAG_ON(VAR_,FLAG_)    (((VAR_) & (FLAG_)) != 0)
/**
 ** Sets the bitflag @c FLAG_ in the variable @c VAR_
 **
 **/
#define RF_BITFLAG_SET(VAR_,FLAG_)  (VAR_) |= (FLAG_)
/**
 ** Unsets the bitflag @c FLAG_ in the variable @c VAR_
 **
 **/
#define RF_BITFLAG_UNSET(VAR_,FLAG_)  (VAR_) &= (~(FLAG_))


//this is not used anywhere yet (why is it here?)
/**
 ** Gets the value (0) or (1) of the __n__ bit of __byte__
 **
 **/
#define RF_GET_BIT(__byte__,__n__)  ((__byte__& (0x01<<__n__) )>>__n__ )


#endif//include guards end
