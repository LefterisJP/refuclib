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

/*------------- Corrensponding Header inclusion -------------*/
#include <String/rf_str_manipulationx.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_core.h> //for rf_string_deinit
#include <String/rf_str_retrieval.h> //for rf_string_between and others
#include <String/rf_str_traversalx.h>
#include "rf_str_common.ph" //for private string functions and macros
#include "rf_str_manipulation.ph" //for manipulation only stuff
/*------------- Outside Module inclusion -------------*/
#include <Utils/rf_unicode.h> //for unicode functions
#include <Utils/log.h>
#include <Utils/memory.h> //for refu memory allocation
#include <Utils/localscope.h>//for local scope macros
#include <Utils/sanity.h> //for sanity macros
/*------------- End of includes -------------*/

bool rf_stringx_append(struct RFstringx* thisstr, const void* other)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    
    if (!other) {
        RF_WARNING("Provided null string for appending");
        ret = false;
        goto cleanup;
    }
    
    ret = rf_stringx_generic_append(thisstr, rf_string_data(other),
                                    rf_string_length_bytes(other));


  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_append_chars(struct RFstringx* thisstr,
                             const void* other,
                             unsigned int chars)
{
    uint32_t i;
    uint32_t length = 0;
    unsigned end = 0;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    
    if (!other) {
        RF_WARNING("Provided null string for appending");
        ret = false;
        goto cleanup;
    }

    //find out the bytelength of the requested char position
    RF_STRING_ITERATE_START(other, length, i);
        if(length == chars)
        {
            end = i;
            break;
        }
    RF_STRING_ITERATE_END(length, i)
    //if the end is not found
    if(!end)
    {
        end = rf_string_length_bytes(other);
    }
    ret = rf_stringx_generic_append(thisstr, rf_string_data(other), end);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_append_char(struct RFstringx* thisstr, uint32_t unichar)
{
    char utf8[MAX_UTF8C_BYTES];
    int length;
    RF_ASSERT(thisstr);
    if((length = rf_utf8_encode_single(unichar,utf8)) <= 0)
    {
        RF_ERROR("Encoding the given unicode codepoint to UTF8 failed");
        return false;
    }

    return rf_stringx_generic_append(thisstr, utf8, length);
}

bool rf_stringx_prepend(struct RFstringx* thisstr, const void* other)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);

    if (!other) {
        RF_WARNING("Provided null string for prepending");
        ret = false;
        goto cleanup;
    }

    //if the new string does not fit inside the buffer reallocate it
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        rf_string_length_bytes(thisstr) + rf_string_length_bytes(other),
        ret = false,
        cleanup
    );

    if(!rf_string_generic_prepend(thisstr,
                            rf_string_data(other),
                            rf_string_length_bytes(thisstr),
                            rf_string_length_bytes(other)))
    {
        ret = false;
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_insert(struct RFstringx* thisstr, uint32_t pos,
                      const void* other)
{
    uint32_t length, bytePos, size, i;
    bool found = false, ret = false;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);

    if (!other) {
        RF_WARNING("Provided null string for inserting");
        ret = false;
        goto cleanup;
    }
    
    //keep the original byte length here
    size = rf_string_length_bytes(thisstr);
    //get the new byte length
    rf_string_length_bytes(thisstr) += rf_string_length_bytes(other);
    //check if the new string fits in the buffer and if not reallocate it
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        rf_string_length_bytes(thisstr),
        ;,
        cleanup);
    //iterate this string to find the byte position of the character position
    RF_STRING_ITERATE_START(thisstr, length, i)
        if(length == pos)
        {
            //found the position. Is saved in bytePos
            bytePos = i;
            found = true;
            break;
        }
    RF_STRING_ITERATE_END(length, i);
    //if the position is found in the string then insert
    if(found)
    {
        ret = true;
        //move the string's contents to make room for the extra string insertion
        memmove(
            rf_string_data(thisstr) + rf_string_length_bytes(other) + bytePos,
            rf_string_data(thisstr) + bytePos, size - bytePos);
        //now insert the new string
        memcpy(rf_string_data(thisstr) + bytePos,
               rf_string_data(other),
               rf_string_length_bytes(other));
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

/*--- RFstringx unsafe appending functions---*/

bool rf_stringx_append_bytes(struct RFstringx* thisstr, const void* other,
                             const unsigned int bytes)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);

    if (!other) {
        RF_WARNING("Provided null string for byte appending");
        ret = false;
        goto cleanup;
    }

    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        rf_string_length_bytes(thisstr) + bytes,
        ret = false,
        cleanup);
    ret = rf_stringx_generic_append(thisstr, rf_string_data(other), bytes);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_append_cstr(struct RFstringx* thisstr, const char* cstr)
{
    size_t len;
    RF_ASSERT(thisstr);

    if (!cstr) {
        RF_WARNING("Provided null c string for appending");
        return false;
    }
    len = strlen(cstr);
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(
        thisstr,
        rf_string_length_bytes(thisstr) + len,
        false);
    
    return rf_stringx_generic_append(thisstr, cstr, len);
}

/*--- RFstringx replacing functions---*/

bool rf_stringx_replace(struct RFstringx* thisstr, const void* sstr,
                        const void* rstr, uint32_t num,
                        enum RFstring_matching_options options)
{
    //will keep the number of given instances to find
    uint32_t number = num;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);

    /* sstr existence is checked for inside replace_intro() function */
    if (!rstr) {
        RF_WARNING("Gave null pointer for the substring to replace");
        ret = false;
        goto cleanup;
    }
    if(!replace_intro(thisstr, &number, sstr, options))
    {
        ret = false;
        goto cleanup;
    }

    //act depending on the size difference of rstr and sstr
    if(rf_string_length_bytes(rstr)> rf_string_length_bytes(sstr))
    {
        //reallocate the string if needed
        RF_STRINGX_REALLOC_JMP(
            thisstr,
            rf_string_length_bytes(thisstr) + number * (
                rf_string_length_bytes(rstr) - rf_string_length_bytes(sstr)
            ),
            ret = false, cleanup
        );
        replace_greater(thisstr, number, sstr, rstr);
    }
    else if(rf_string_length_bytes(rstr) < rf_string_length_bytes(sstr))
    {
        replace_lesser(thisstr, number, sstr, rstr);
    }
    else //replace and remove strings are equal
    {
        replace_equal(thisstr, number, rstr);
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_stringx_replace_between(struct RFstringx* thisstr, const void* left,
                                const void* right, const void* rstr,
                                enum RFstring_matching_options options,
                                uint32_t i)
{
    uint32_t j,move,start = thisstr->bIndex;
    bool found = false;
    bool ret=false;
    bool same_separators = false;
    struct RFstringx string_buff;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);

    if (!left) {
        RF_WARNING("Provided null pointer for the left string");
        ret = false;
        goto cleanup;
    }
    if (!right) {
        RF_WARNING("Provided null pointer for the right string");
        ret = false;
        goto cleanup;
    }
    if (!rstr) {
        RF_WARNING("Provided null pointer for the replace string");
        ret = false;
        goto cleanup;
    }

    if (rf_string_equal(left, right)) {
        same_separators = true;
    }
    
    if (!rf_stringx_init_buff(&string_buff, 128, "")) {
        ret = false;
        RF_ERROR("No memory");
        goto cleanup;
    }
    
    if(i == 0)//if we want all occurences replaced
    {
        //while we have occurences of the pair
        while(!rf_string_between(thisstr, left, right, &string_buff, options))
        {
            found = true;
            /*
             * move the internal pointer right after the left part of the pair
             * and replace what was found there. None of the functions
             * below fail since the while condition is true
             */
            rf_stringx_move_after(thisstr, left, 0, options);
            if(rf_stringx_replace(thisstr, &string_buff, rstr, 1, options))
            {
                goto cleanup;
            }
            
            /* if left == right don't go over the right separator when searching */
            if (!same_separators) {
                move = rf_string_length_bytes(rstr) + rf_string_length_bytes(right);
            } else {
                move = rf_string_length_bytes(rstr);
            }
            thisstr->bIndex += move;
            rf_string_data(thisstr) += move;
            rf_string_length_bytes(thisstr) -= move;
        }

        if(found)//if we replaced at least 1 occurence
        {
            move = thisstr->bIndex - start;
            rf_string_data(thisstr) -= move;
            thisstr->bIndex = start;
            rf_string_length_bytes(thisstr) += move;
            //success
            ret = true;
        }
        goto cleanup;
    }

    ///if we check for a specific instance
    for(j = 1; j < i; j++)
    {
        //move after the pair of the 'j' inbetween substrings
        if(!rf_stringx_move_after_pair(thisstr, left, right,
                                       &string_buff, options, 0))
        {
            //and if the occurence does not exist
            move = thisstr->bIndex - start;
            rf_string_data(thisstr) -= move;
            thisstr->bIndex = start;
            //and increase its bytelength
            rf_string_length_bytes(thisstr) += move;
            goto cleanup;//falure
        }
    }
    //move after the pair of the inbetween substrings we actually want
    if(!rf_string_between(thisstr, left, right, &string_buff, options))
    {
        //and if the occurence does not exist
        move = thisstr->bIndex-start;
        rf_string_data(thisstr) -= move;
        thisstr->bIndex = start;
        //and increase its bytelength
        rf_string_length_bytes(thisstr) += move;
        goto cleanup;//falure
    }
    ///success
    //move after the left part of the pair
    rf_stringx_move_after(thisstr, left, 0, options);
    //and then replace the occurence
    if(!rf_stringx_replace(thisstr, &string_buff, rstr, 1, options))
    {
        //failure
        goto cleanup;
    }
    //now we are done and should go back
    rf_stringx_deinit(&string_buff);
    move = thisstr->bIndex - start;
    rf_string_data(thisstr) -= move;
    thisstr->bIndex = start;
    rf_string_length_bytes(thisstr) += move;

    //if we get here success
    ret = true;
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}




i_INLINE_INS bool rf_stringx_generic_append(struct RFstringx* s,
                                      const char* other,
                                      unsigned int bytes_to_copy);
