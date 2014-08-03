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
#include <String/rf_str_manipulation.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_corex.h>
#include <String/rf_str_core.h> //for string iterations
#include <String/rf_str_retrieval.h> //for rf_string_length()
#include "rf_str_common.ph" //for required string private macros and functions
#include "rf_str_manipulation.ph" //for manipulation specific common macros
#include "rf_str_mod.ph" //for the submod initialization
#include "rf_str_defines.ph" //for some defines
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h>
#include <Utils/rf_unicode.h> //for unicode functions
#include <Utils/memory.h> //for refu memory allocation
#include <Utils/localscope.h> //for local scope macros
#include <Utils/sanity.h> //for sanity macros
#include "../Internal/rf_internal_mod.ph" //for internal thread specific buffer
/*------------- libc inclusion --------------*/
#include <limits.h> //for UINT_MAX e.t.c.
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/

// Appends the parameter String to this one
bool rf_string_append(struct RFstring* thisstr, const void* other)
{
    unsigned int newLen;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    if (!other) {
        RF_WARNING("Provided null append string");
        ret = false;
        goto cleanup;
    }

    newLen = rf_string_length_bytes(thisstr) + rf_string_length_bytes(other);
    //reallocate this string to fit the new addition
    RF_REALLOC_JMP(rf_string_data(thisstr), char, newLen ,
                   ret = false, cleanup);
    rf_string_generic_append(thisstr,
                             rf_string_data(other),
                             rf_string_length_bytes(other));
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_append_int(struct RFstring* thisstr, const int32_t i)
{
    int rc;
    RF_ASSERT(thisstr);
    //reallocate this string to fit the new addition
    RF_REALLOC(rf_string_data(thisstr), char,
               rf_string_length_bytes(thisstr) + MAX_UINT32_STRING_CHAR_SIZE ,
               false
    );
    ADD_TYPE_TO_STRING(thisstr, "int", MAX_UINT32_STRING_CHAR_SIZE,
                       "%i", rc, i);
    return true;
}

bool rf_string_append_double(struct RFstring* thisstr, const double d)
{
    int rc;
    RF_ASSERT(thisstr);
    RF_REALLOC(rf_string_data(thisstr), char,
                   rf_string_length_bytes(thisstr) + MAX_DOUBLE_STRING_CHAR_SIZE ,
                   false
    );
    ADD_TYPE_TO_STRING(thisstr, "double", MAX_DOUBLE_STRING_CHAR_SIZE,
                       "%f", rc, d);
    return true;
}

bool rf_string_prepend(struct RFstring* thisstr, const void* other)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    if (!other) {
        RF_WARNING("Provided null string for prepending");
        ret = false;
        goto cleanup;
    }

    //reallocate this string to fit the new addition
    RF_REALLOC_JMP(rf_string_data(thisstr), char,
                   rf_string_length_bytes(thisstr) + rf_string_length_bytes(other),
                   ret = false, cleanup);

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

bool rf_string_remove(void* thisstr, const void* rstr, uint32_t number,
                      enum RFstring_matching_options options)
{
    uint32_t i,count,occurences=0;
    int32_t bytePos;
    char found = false, ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    if (!rstr) {
        RF_WARNING("Gave NULL substring to remove");
        ret = false;
        goto cleanup;
    }

    //as long as we keep finding rstr in the string keep removing it
    do
    {
        bytePos = rf_string_find_byte_pos(thisstr, rstr, options);
        //if the substring is not found
        if(bytePos == RF_FAILURE)
        {
            //if we have not even found it once , we fail
            if(!found)
            {
                ret = false;
                goto cleanup;
            }
            else
            {
                //else we are done
                break;
            }
        }
        //substring found
        found = true;
        //move all of the string a position back
        count = 0;
        for(i = bytePos; i <= rf_string_length_bytes(thisstr); i ++)
        {
            rf_string_data(thisstr)[i] = (
                rf_string_data(thisstr)[i + rf_string_length_bytes(rstr)]
            );
            count++;
        }
        //now change the byte length
        rf_string_length_bytes(thisstr) -= rf_string_length_bytes(rstr);
        //count number of occurences, if we reached the required, stop
        occurences++;
        if(occurences == number)
        {
            break;
        }
    }while(bytePos != RF_FAILURE);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all of the characters of the string except those specified
bool rf_string_keep_only(void* thisstr, const void* keepstr, int *removals)
{
    uint32_t charValue;
    uint32_t i;
    char unused[MAX_UTF8C_BYTES];
    int character_byte_length;
    int keepLength;
    int j;
    bool exists;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);

    if (!keepstr) {
        RF_WARNING("Provided null string");
        ret = false;
        goto cleanup;
    }

    if (removals) {
        *removals = 0;
    }

    //get all characters of the string in the array
    keepLength = rf_string_fill_codepoints(keepstr);
    if (keepLength < 0 ) {
        return false;
    }

    //now iterate every character of this string
    i=0;
    rf_string_iterate_start(thisstr, i, charValue)
        //for every character check if it exists in the keep str
        exists = false;
    for (j = 0; j < keepLength; j++) {
            if (rf_buffer_from_current_at(TSBUFFA, j, uint32_t) == charValue)
            {
                exists = true;
            }
        }

        if(!exists)
        {
            /* move the string back to cover and delete the character */
            character_byte_length = rf_utf8_encode_single(charValue, unused);
            if(character_byte_length < 0) {
                RF_ERROR("Could not decode a codepoint into UTF-8");
                ret = false;
                goto cleanup;
            }

            if (removals) {
                *removals = *removals + 1;
            }

            /*
             *this is kind of a dirty way to do it. the rf_string_iterate_start
             *macro internally uses a byteIndex_ variable. We use that here to
             * determine the current byteIndex_ of the string in the iteration
             *and move the string backwards
             */

            /* special case: last character */
            if (byteIndex_ + character_byte_length != rf_string_length_bytes(thisstr)) {
                memmove(
                    rf_string_data(thisstr) + byteIndex_,
                    rf_string_data(thisstr) + byteIndex_ + character_byte_length,
                    rf_string_length_bytes(thisstr) - byteIndex_ - character_byte_length
                );
            }
            rf_string_length_bytes(thisstr) -= character_byte_length;
            continue;//by contiuing here we make sure that the current string position won't be moved to assure that we also check the newly moved characters
        }
    rf_string_iterate_end(i)

cleanup:

    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_prune_start(void* thisstr, uint32_t n, unsigned int *removals)
{
    //iterate the characters of the string
    uint32_t i;
    uint32_t length = 0;
    unsigned new_byte_pos = 0;
    char found = false;
    RF_ASSERT(thisstr);

    if (removals) {
        *removals = 0;
    }

    RF_STRING_ITERATE_START(thisstr, length, i);
        //if we reach the number of characters passed as a parameter, note it
        if(length == n)
        {
            //remember that now i is the byte position we need
            new_byte_pos = i;
            found = true;
            break;
        }
        if (removals) {
             *removals = *removals + 1;
        }
    RF_STRING_ITERATE_END(length, i)

    //if the string does not have n chars to remove it becomes an empty string
    if(!found)
    {
        rf_string_length_bytes(thisstr) = 0;
        return true;
    }

    //move the string back to cover the empty places
    for(i = 0; i < rf_string_length_bytes(thisstr) - new_byte_pos; i++)
    {
        rf_string_data(thisstr)[i] = rf_string_data(thisstr)[i + new_byte_pos];
    }
    //get the new bytelength
    rf_string_length_bytes(thisstr) -= new_byte_pos;

    return true;
}

bool rf_string_prune_end(void* thisstr, uint32_t n, unsigned int *removals)
{
    //start the iteration of the characters from the end of the string
    int32_t new_byte_pos = -1;
    uint32_t character_position;
    uint32_t byte_position;
    RF_ASSERT(thisstr);

    if (removals) {
        *removals = 0;
    }
    if (!n) {
        return true;
    }

    RF_STRING_ITERATEB_START(thisstr, character_position, byte_position)
        if (removals) {
            *removals = *removals + 1;
        }
        //if we found the requested number of characters from the end
        if (character_position == n) {
            new_byte_pos = byte_position;
            break;
        }
    RF_STRING_ITERATEB_END(character_position, byte_position)

    //if the string does not have n chars to remove, fail
    if (new_byte_pos == -1) {
        rf_string_length_bytes(thisstr) = 0;
        return true;
    }

    //juts set the new byte length
    rf_string_length_bytes(thisstr) -= (
        (rf_string_length_bytes(thisstr) - new_byte_pos)
    );
    return true;
}

bool rf_string_prune_middle_b(void* thisstr, uint32_t p,
                              uint32_t n, unsigned int *removals)
{
    uint32_t byte_position;
    uint32_t character_position;
    int previous_byte_pos = -1;
    int new_byte_pos = 0;
    RF_ASSERT(thisstr);

    if (removals) {
        *removals = 0;
    }
    if (!n) {
        return true;
    }
    
    RF_STRING_ITERATE_START(thisstr, character_position, byte_position)
        /* +1 is to make sure that indexing works from 0 */
        if (character_position == p - n + 1) {
             new_byte_pos = byte_position;
        }
        //if we reach the number of characters passed as a parameter, note it
        if (character_position == p + 1) {
            //we search for p+1  because we want to include all of the p character
            previous_byte_pos = byte_position;
            //also break since we don't care after position p
            break;
        }
    RF_STRING_ITERATE_END(character_position, byte_position)


    //if the position was not found in the string do nothing
    if (previous_byte_pos == -1) {
        if (removals) {
            *removals = 0;
        }
        return false;
    }
    if (removals) {
        *removals = p;
        if (new_byte_pos != 0) {
            *removals = p - n;
        }
    }
    /*
     *move the contents of the string to cover the removed
     *characters and change its length
     */
    memmove(
        rf_string_data(thisstr) + new_byte_pos,
        rf_string_data(thisstr) + previous_byte_pos,
        rf_string_length_bytes(thisstr) - previous_byte_pos);
    rf_string_length_bytes(thisstr) -= (previous_byte_pos - new_byte_pos);
    return true;
}

bool rf_string_prune_middle_f(void* thisstr, uint32_t p,
                              uint32_t n, unsigned int *removals)
{
    uint32_t j, i;
    uint32_t byte_position;
    uint32_t character_position;
    int previous_byte_pos = -1;
    int new_byte_pos = 0;
    bool character_reached = false;
    RF_ASSERT(thisstr);

    if (removals) {
        *removals = 0;
    }
    if (!n) {
        return true;
    }

    RF_STRING_ITERATE_START(thisstr, character_position, byte_position)
        //if we reach the number of characters passed as a parameter, note it
        if(character_position == p)
        {
            previous_byte_pos = byte_position;
            character_reached = true;
        }
    
        if (character_reached && removals) {
            *removals = *removals + 1;
        }

        if(character_position == p + n)
        {
            new_byte_pos = byte_position;
            break;//since we got all the data we needed
        }
    RF_STRING_ITERATE_END(character_position, byte_position)

    //if the position was not found in the string do nothing
    if (previous_byte_pos == -1) {
        if (removals) {
            *removals = 0;
        }
        return false;
    }

    /*
     * if we did not find the byte position of p+n then we remove
     * everything from previous_byte_pos until the end of the string
     */
    if(new_byte_pos == -1)
    {
        rf_string_length_bytes(thisstr) -= (
            rf_string_length_bytes(thisstr) - previous_byte_pos
        );
        return true;
    }

    //move the bytes in the buffer to remove the requested characters
    for(i=previous_byte_pos, j=0;
        j<= rf_string_length_bytes(thisstr) - new_byte_pos;
        i ++,j++)
    {
        rf_string_data(thisstr)[i] = rf_string_data(thisstr)[new_byte_pos + j];
    }
    //find the new byte length
    rf_string_length_bytes(thisstr) -= (new_byte_pos - previous_byte_pos);
    return true;
}

bool rf_string_trim_start(void* thisstr, const void* sub, unsigned int *removals)
{
    bool ret = true;
    uint32_t byte_position;
    unsigned int i;
    unsigned int matching_chars = 0;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    
    if (!sub) {
        RF_WARNING("Provided null pointer for substring");
        ret = false;
        goto cleanup;
    }

    matching_chars = rf_string_begins_with_any(thisstr, sub, &byte_position);


    if (removals) {
        *removals = matching_chars;
    }


    //if we had any match
    if(matching_chars) {
        //remove the characters
        for(i =0; i < rf_string_length_bytes(thisstr) - byte_position; i++ )
        {
            rf_string_data(thisstr)[i] = rf_string_data(thisstr)[i + byte_position];
        }
        //also change bytelength
        rf_string_length_bytes(thisstr) -= byte_position;
    }


  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_trim_end(void* thisstr, const void* sub, unsigned int *removals)
{
    bool ret;
    bool iteration_match;
    uint32_t i;
    uint32_t byte_position, last_byte_position;
    int subLength;
    int j;
    RF_ENTER_LOCAL_SCOPE();
    RF_ASSERT(thisstr);
    
    if (!sub) {
        RF_WARNING("Provided null substring for trimming");
        ret = false;
        goto cleanup;
    }
    
    ret = true;
    last_byte_position = 0;
    if (removals) {
        *removals = 0;
    }

    //get all the codepoints of the string
    subLength = rf_string_fill_codepoints(sub);
    if (subLength < 0) {
        return false;
    }

    //iterate thisstring from the end
    i = 0;
    RF_STRING_ITERATEB_START(thisstr, i, byte_position)
        iteration_match = true;
        //for every substring character
        for(j = 0; j < subLength; j++)
        {
            //if we got a match
            if(rf_string_bytepos_to_codepoint(thisstr, byte_position) ==
               rf_buffer_from_current_at(TSBUFFA, j, uint32_t))
            {
                if (removals) {
                    *removals = *removals + 1;
                }
                iteration_match = false;
                last_byte_position = byte_position;
                break;
            }
        }
        if (iteration_match) {
            break;
        }
    RF_STRING_ITERATEB_END(i, byte_position)

    //if we had any match
    if(ret)
    {
        rf_string_length_bytes(thisstr) -= (
            rf_string_length_bytes(thisstr) - last_byte_position
        );
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_trim(void* thisstrP, const void* subP, unsigned int *removals)
{
    unsigned int local_removals = 0;
    bool res1, res2;
    RF_ENTER_LOCAL_SCOPE();
    if (removals) {
        res1 = rf_string_trim_start(thisstrP, subP, &local_removals);
        *removals = local_removals;
        res2 = rf_string_trim_end(thisstrP, subP, &local_removals);
        *removals = *removals + local_removals;
    } else {
        res1 = rf_string_trim_start(thisstrP, subP, NULL);
        res2 = rf_string_trim_end(thisstrP, subP, NULL);        
    }
    RF_EXIT_LOCAL_SCOPE();
    return res1|res2;
}


bool rf_string_replace(struct RFstring* thisstr, const void* sstr,
                       const void* rstr, const uint32_t num,
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

    if(!replace_intro(thisstr, &number, sstr, options)) {
        ret = false;
        goto cleanup;
    }

    //act depending on the size difference of rstr and sstr
    if(rf_string_length_bytes(rstr)> rf_string_length_bytes(sstr))
    {
        RF_REALLOC_JMP(
            rf_string_data(thisstr), char,
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


/* module related inline object insertion*/

i_INLINE_INS bool rf_string_generic_prepend(void* thisstr, const char* other,
                                      unsigned int orig_size,
                                      unsigned int other_size);
i_INLINE_INS bool replace_intro(void* s, uint32_t* number, const void* sstr,
                                enum RFstring_matching_options options);
i_INLINE_INS void replace_greater(void* s, uint32_t number,
                                  const void* sstr, const void* rstr);
i_INLINE_INS void replace_lesser(void* s, uint32_t number,
                                 const void* sstr, const void* rstr);
i_INLINE_INS void replace_equal(void* s, uint32_t number, const void* rstr);
