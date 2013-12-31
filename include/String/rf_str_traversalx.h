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
#ifndef RF_STRING_TRAVERSALX_H
#define RF_STRING_TRAVERSALX_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstringX
#include <String/rf_str_flags.h> //for RFstring_matching_options
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
#include <Definitions/inline.h> //for inline declarations
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name Traversing an RFstringx
//! @{

/**
 ** @brief Moves the internal pointer right after the the first occurence
 ** of substring
 **
 ** Searches the StringX for the @c sub substring and moves the internal
 ** pointer right after it. Optionally the string between the beginning and
 ** the end of the moving is returned in @c result parameter
 ** @lmsFunction
 ** @param thisstr     The extended string to work on
 ** @param sub         The substring after which to move inside the current
 **                    String. @inhtype{String,StringX} @tmpSTR
 ** @param result      Pass a pointer to a @c String type to be
 **                    assigned to with the substring between the start of
 **                    @c thisstr and the end of the moving. If the passed
 **                    pointer is of RFstringx type also pass the 
 **                    @c RF_STRINGX_ARGUMENT bitflag argument in the 
 **                    @c options argument. The caller is responsible for
 **                    initializing the string but the function will assign to it
 **                    and also reallocate its buffer if needed
 **                    If 0 nothing is assigned @inhtype{String,StringX}
 ** @param options     @see rf_string_remove() for details of the
 **                    possible options. An additional option that can be given:
 **                    + @c RF_STRINGX_ARGUMENT: Pass this bitflag option if the
 **                    pointer you gave for initialization at @c result is
 **                    of RFstringx type
 ** @return            Returns the number of positions (bytes) moved
 **                    or RF_FAILURE if the substring was not found in the String
 ** @see rf_stringx_move_afterv()
 ** @see rf_stringx_move_after_pair()
 ** @see rf_stringx_move_forward()
 ** @see rf_stringx_move_back()
 ** @see rf_stringx_reset()
 **/
i_DECLIMEX_ int32_t rf_stringx_move_after(
    struct RFstringx* thisstr,
    const void* sub,
    void* result,
    enum RFstring_matching_options options
);

/**
 ** @brief Moves the internal pointer n characters back
 **
 ** It never goes before the original start of the buffer so there is
 ** no worry of memory corruption.
 ** @param thisstr     The extended string to work on
 ** @param n           The number of characters to move the internal pointer
 ** @see rf_stringx_move_forward()
 ** @see rf_stringx_move_after()
 ** @see rf_stringx_reset()
 **/
i_DECLIMEX_ void rf_stringx_move_back(struct RFstringx* thisstr, uint32_t n);
/**
 ** @brief Moves the internal pointer n characters forward
 **
 ** It never goes beyond the buffer's length so there is no fear of memory
 ** corruption. But since many times the buffer can be bigger than the string
 ** we can have it go to non-string buffer data so the user has to be careful.
 ** @param thisstr The extended string to work on
 ** @param n The number of character to move the internal pointer
 ** @see rf_stringx_move_back()
 ** @see rf_stringx_move_after()
 ** @see rf_stringx_reset()
 **/
i_DECLIMEX_ void rf_stringx_move_forward(struct RFstringx* thisstr, uint32_t n);

/**
 ** @brief Moves the internal pointer to the end of the string
 ** @param s The string to move
 **/
i_INLINE_DECL void rf_stringx_move_end(struct RFstringx* s)
{
    s->INH_String.data += s->INH_String.length;
    s->bIndex += s->INH_String.length;
    s->INH_String.length = 0;
}

/**
 ** @brief Resets the internal pointer of the StringX
 **
 ** @param thisstr The stringX whose internal pointer to reset
 ** @see rf_stringx_move_forward()
 ** @see rf_stringx_move_back()
 ** @see rf_stringx_move_after()
 **/
i_DECLIMEX_ void rf_stringx_reset(struct RFstringx* thisstr);

/**
 ** @brief Moves the internal pointer after the first occurence of any
 ** of the given substrings
 **
 ** Searches for any of the given Substrings inside @c thisstr. Moves the
 ** internal pointer right after the first occuring substring. Optionally and
 ** if the value given at @c result is not 0 the string between the start and
 ** the end of the moving is returned. The parameters accepted can be of any
 ** string type, even temporary strings initialized with the RFS_() macro
 ** @lmsFunction
 ** @param thisstr      The extended string to work on
 ** @param result       For details @see rf_stringx_move_after()
 ** @param options      For details @see rf_stringx_move_after()
 ** @param parN         The number of parameters
 ** @extraVarArgLim
 ** @param ...          The strings to search for.
 **                     @inhtype{String,StringX} @tmpSTR
 ** @return             true if the substring got initialized and false if
 **                     none of the parameters are found or due to error
 ** @see rf_stringx_move_after()
 ** @see rf_stringx_move_after_pair()
 ** @see rf_stringx_move_forward()
 ** @see rf_stringx_move_back()
 ** @see rf_stringx_reset()
 **/
i_DECLIMEX_ bool rf_stringx_move_afterv(struct RFstringx* thisstr, void* result,
                                        enum RFstring_matching_options options,
                                        const unsigned char parN, ...);

/**
 ** @brief Moves the internal string pointer after the substring formed
 ** by the @c left and @c right substrings
 **
 ** Searches @c thisstr for the occurence of both the @c left and @c right
 ** substrings and if found moves the internal pointer right after them.
 ** If the number given at @c occurence does not exist in the string the
 ** function returns @c false. Finally if @c result is not zero then what
 ** is between the @c left and @c right substrings is returned inside @c result.
 ** @lmsFunction
 ** @param thisstr       The extended string to work on
 ** @param left          The left substring that will define the new substring.
 **                      @inhtype{String,StringX} @tmpSTR
 ** @param right         The right substring that will define the new substring.
 **                      @inhtype{String,StringX} @tmpSTR
 ** @param result        For details @see rf_stringx_move_after()
 ** @param options       For details @see rf_stringx_move_after()
 ** @param occurence     If this is not 0 then the function
 **                      will search for the number of occurence given in this
 **                      parameter. If it is 0 it will search for the first
 **                      occurence. If it is not found then the function shall
 **                      return false
 ** @return              Returns true if the substring is found and false if
 **                      not and in the case of an error
 ** @see rf_stringx_move_after()
 ** @see rf_stringx_move_afterv()
 ** @see rf_stringx_move_forward()
 ** @see rf_stringx_move_back()
 ** @see rf_stringx_reset()
 **/
i_DECLIMEX_ bool rf_stringx_move_after_pair(
    struct RFstringx* thisstr,
    const void* left,
    const void* right, void* result, 
    enum RFstring_matching_options options,
    uint32_t occurence
);

//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
