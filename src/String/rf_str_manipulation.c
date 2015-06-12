/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
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
#include <Utils/sanity.h> //for sanity macros
#include <Persistent/buffers.h> //for the internal persistent buffer
/*------------- libc inclusion --------------*/
#include <limits.h> //for UINT_MAX e.t.c.
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/

// Appends the parameter String to this one
bool rf_string_append(struct RFstring* thisstr, const struct RFstring *other)
{
    unsigned int newLen;
    RF_ASSERT(thisstr, "got null string in function");
    if (!other) {
        RF_WARNING("Provided null append string");
        return false;
    }

    newLen = rf_string_length_bytes(thisstr) + rf_string_length_bytes(other);
    //reallocate this string to fit the new addition
    RF_REALLOC(rf_string_data(thisstr), char, newLen , return false);
    rf_string_generic_append(thisstr,
                             rf_string_data(other),
                             rf_string_length_bytes(other));

    return true;
}

bool rf_string_append_int(struct RFstring* thisstr, const int32_t i)
{
    int rc;
    RF_ASSERT(thisstr, "got null string in function");
    //reallocate this string to fit the new addition
    RF_REALLOC(rf_string_data(thisstr), char,
               rf_string_length_bytes(thisstr) + MAX_UINT32_STRING_CHAR_SIZE ,
               return false
    );
    ADD_TYPE_TO_STRING(thisstr, "int", MAX_UINT32_STRING_CHAR_SIZE,
                       "%i", rc, i);
    return true;
}

bool rf_string_append_double(struct RFstring* thisstr,
                             double d,
                             unsigned int precision)
{
    int rc;
    RF_ASSERT(thisstr, "got null string in function");
    RF_REALLOC(rf_string_data(thisstr), char,
               rf_string_length_bytes(thisstr) + MAX_DOUBLE_STRING_CHAR_SIZE ,
               return false
    );
    ADD_TYPE_TO_STRING(thisstr, "double", MAX_DOUBLE_STRING_CHAR_SIZE,
                       "%.*f", rc, precision, d);
    return true;
}

bool rf_string_prepend(struct RFstring* thisstr, const struct RFstring *other)
{
    RF_ASSERT(thisstr, "got null string in function");
    if (!other) {
        RF_WARNING("Provided null string for prepending");
        return false;
    }

    //reallocate this string to fit the new addition
    RF_REALLOC(rf_string_data(thisstr), char,
               rf_string_length_bytes(thisstr) + rf_string_length_bytes(other),
               return false);

    if (!rf_string_generic_prepend(thisstr,
                            rf_string_data(other),
                            rf_string_length_bytes(thisstr),
                                  rf_string_length_bytes(other))) {
        
        return false;
    }

    return true;
}

bool rf_string_remove(struct RFstring *thisstr,
                      const struct RFstring *rstr,
                      uint32_t number,
                      enum RFstring_matching_options options)
{
    uint32_t i, count, occurences=0;
    int32_t bytePos;
    char found = false;
    RF_ASSERT(thisstr, "got null string in function");
    if (!rstr) {
        RF_WARNING("Gave NULL substring to remove");
        return false;
    }

    //as long as we keep finding rstr in the string keep removing it
    do {
        bytePos = rf_string_find_byte_pos(thisstr, rstr, options);
        //if the substring is not found
        if (bytePos == RF_FAILURE) {
            //if we have not even found it once , we fail
            if (!found) {
                return false;
            } else {
                //else we are done
                break;
            }
        }
        //substring found
        found = true;
        //move all of the string after the found position back
        count = 0;
        for (i = bytePos;
             i < rf_string_length_bytes(thisstr) - rf_string_length_bytes(rstr);
             i ++) {
            
            rf_string_data(thisstr)[i] = (
                rf_string_data(thisstr)[i + rf_string_length_bytes(rstr)]
            );
            count++;
        }
        //now change the byte length
        rf_string_length_bytes(thisstr) -= rf_string_length_bytes(rstr);
        //count number of occurences, if we reached the required, stop
        occurences++;
        if (occurences == number) {
            break;
        }
    } while(bytePos != RF_FAILURE);

    return true;
}

//Removes all of the characters of the string except those specified
bool rf_string_keep_only(struct RFstring *thisstr,
                         const struct RFstring *keepstr,
                         int *removals)
{
    uint32_t charValue;
    uint32_t i;
    char unused[MAX_UTF8C_BYTES];
    int character_byte_length;
    size_t keep_length;
    int j;
    bool exists;
    bool ret = false;
    uint32_t *buffer;
    RF_ASSERT(thisstr, "got null string in function");

    if (!keepstr) {
        RF_WARNING("Provided null string");
        return false;
    }

    if (removals) {
        *removals = 0;
    }

    //get all characters of the string in the array
    rf_mbuffer_push(RF_TSBUFFM);
    buffer = rf_string_fill_codepoints(keepstr, &keep_length);
    if (!buffer) {
        goto end;
    }

    //now iterate every character of this string
    i=0;
    rf_string_iterate_start(thisstr, i, charValue)
        //for every character check if it exists in the keep str
        exists = false;
    for (j = 0; j < keep_length; ++j) {
        if (buffer[j] == charValue) {
            exists = true;
        }
    }

    if (!exists) {
            /* move the string back to cover and delete the character */
            character_byte_length = rf_utf8_encode_single(charValue, unused);
            if(character_byte_length < 0) {
                RF_ERROR("Could not decode a codepoint into UTF-8");
                goto end;
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
    ret = true;

end:
    rf_mbuffer_pop(RF_TSBUFFM);
    return ret;
}

bool rf_string_prune_start(struct RFstring *thisstr, uint32_t n, unsigned int *removals)
{
    //iterate the characters of the string
    uint32_t i;
    uint32_t length = 0;
    unsigned new_byte_pos = 0;
    char found = false;
    RF_ASSERT(thisstr, "got null string in function");

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

bool rf_string_prune_end(struct RFstring *thisstr, uint32_t n, unsigned int *removals)
{
    //start the iteration of the characters from the end of the string
    int32_t new_byte_pos = -1;
    uint32_t character_position;
    uint32_t byte_position;
    RF_ASSERT(thisstr, "got null string in function");

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

bool rf_string_prune_middle_b(struct RFstring *thisstr, uint32_t p,
                              uint32_t n, unsigned int *removals)
{
    uint32_t byte_position;
    uint32_t character_position;
    int previous_byte_pos = -1;
    int new_byte_pos = 0;
    RF_ASSERT(thisstr, "got null string in function");

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

bool rf_string_prune_middle_f(struct RFstring *thisstr, uint32_t p,
                              uint32_t n, unsigned int *removals)
{
    uint32_t j, i;
    uint32_t byte_position;
    uint32_t character_position;
    int previous_byte_pos = -1;
    int new_byte_pos = -1;
    bool character_reached = false;
    RF_ASSERT(thisstr, "got null string in function");

    if (removals) {
        *removals = 0;
    }
    if (!n) {
        return true;
    }

    RF_STRING_ITERATE_START(thisstr, character_position, byte_position)
        //if we reach the number of characters passed as a parameter, note it
        if (character_position == p) {
            previous_byte_pos = byte_position;
            character_reached = true;
        }
    
        if (character_reached && removals) {
            *removals = *removals + 1;
        }

        if (character_position == p + n) {
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
    if (new_byte_pos == -1) {
        rf_string_length_bytes(thisstr) -= (
            rf_string_length_bytes(thisstr) - previous_byte_pos
        );
        return true;
    }

    //move the bytes in the buffer to remove the requested characters
    for(i = previous_byte_pos, j = 0;
        j < rf_string_length_bytes(thisstr) - new_byte_pos;
        i ++, j++)
    {
        rf_string_data(thisstr)[i] = rf_string_data(thisstr)[new_byte_pos + j];
    }
    //find the new byte length
    rf_string_length_bytes(thisstr) -= (new_byte_pos - previous_byte_pos);
    return true;
}

bool rf_string_trim_start(struct RFstring *thisstr,
                          const struct RFstring *sub,
                          unsigned int *removals)
{
    uint32_t byte_position;
    unsigned int i;
    unsigned int matching_chars = 0;
    RF_ASSERT(thisstr, "got null string in function");
    
    if (!sub) {
        RF_WARNING("Provided null pointer for substring");
        return false;
    }

    matching_chars = rf_string_begins_with_any(thisstr, sub, 0, &byte_position);


    if (removals) {
        *removals = matching_chars;
    }

    //if we had any match
    if(matching_chars) {
        //remove the characters
        for (i =0; i < rf_string_length_bytes(thisstr) - byte_position; i++ ) {
            rf_string_data(thisstr)[i] = rf_string_data(thisstr)[i + byte_position];
        }
        //also change bytelength
        rf_string_length_bytes(thisstr) -= byte_position;
    }

    return true;
}

bool rf_string_trim_end(struct RFstring *thisstr,
                        const struct RFstring *sub,
                        unsigned int *rem)
{
    bool iteration_match;
    uint32_t i;
    uint32_t byte_position;
    uint32_t last_byte_position;
    size_t sub_length;
    int j;
    uint32_t *buffer;
    unsigned int removals = 0;
    bool ret = false;
    RF_ASSERT(thisstr, "got null string in function");
    
    if (!sub) {
        RF_WARNING("Provided null substring for trimming");
        return false;
    }
    
    last_byte_position = 0;

    //get all the codepoints of the string
    rf_mbuffer_push(RF_TSBUFFM);
    buffer = rf_string_fill_codepoints(sub, &sub_length);
    if (!buffer) {
        goto end;
    }

    //iterate thisstring from the end
    i = 0;
    RF_STRING_ITERATEB_START(thisstr, i, byte_position)
        iteration_match = true;
        //for every substring character
        for (j = 0; j < sub_length; ++j) {
            //if we got a match
            if (rf_string_bytepos_to_codepoint(thisstr, byte_position) == buffer[j]) {
                removals += 1;
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
    if (removals != 0) {
        rf_string_length_bytes(thisstr) -= (
            rf_string_length_bytes(thisstr) - last_byte_position
        );
    }

    if (rem) {
        *rem = removals;
    }
    ret = true;

end:
    rf_mbuffer_pop(RF_TSBUFFM);
    return ret;
}

bool rf_string_trim(struct RFstring *thisstrP,
                    const struct RFstring *subP,
                    unsigned int *removals)
{
    unsigned int local_removals = 0;
    bool res1, res2;
    if (removals) {
        res1 = rf_string_trim_start(thisstrP, subP, &local_removals);
        *removals = local_removals;
        res2 = rf_string_trim_end(thisstrP, subP, &local_removals);
        *removals = *removals + local_removals;
    } else {
        res1 = rf_string_trim_start(thisstrP, subP, NULL);
        res2 = rf_string_trim_end(thisstrP, subP, NULL);        
    }
    return res1|res2;
}


bool rf_string_replace(struct RFstring* thisstr,
                       const struct RFstring *sstr,
                       const struct RFstring *rstr,
                       const uint32_t num,
                       enum RFstring_matching_options options)
{
    uint32_t number = num;
    uint32_t *buff;
    bool ret = false;
    RF_ASSERT(thisstr, "got NULL string in function");

    /* sstr existence is checked for inside replace_intro() function */
    if (!rstr) {
        RF_WARNING("Gave null pointer for the substring to replace");
        return false;
    }

    if (!(buff = replace_intro(thisstr, &number, sstr, options))) {
        return false;
    }

    //act depending on the size difference of rstr and sstr
    if (rf_string_length_bytes(rstr)> rf_string_length_bytes(sstr)) {
        RF_REALLOC(
            rf_string_data(thisstr), char,
            rf_string_length_bytes(thisstr) + number * (
                rf_string_length_bytes(rstr) - rf_string_length_bytes(sstr)
            ),
            goto end
        );
        replace_greater(thisstr, buff, number, sstr, rstr);
    } else if (rf_string_length_bytes(rstr) < rf_string_length_bytes(sstr)) {
        replace_lesser(thisstr, buff, number, sstr, rstr);
    } else { //replace and remove strings are equal
        replace_equal(thisstr, buff, number, rstr);
    }
    ret = true;

end:
    return ret;
}


/* module related inline object insertion*/

i_INLINE_INS bool rf_string_generic_prepend(struct RFstring *thisstr,
                                            const char* other,
                                            unsigned int orig_size,
                                            unsigned int other_size);

uint32_t *replace_intro(struct RFstring *s,
                        uint32_t *number,
                        const struct RFstring *sstr,
                        enum RFstring_matching_options options)
{
    //TODO: Change this with a thread local buffer or thread local string
    struct RFstringx temp;//just a temporary string for finding the occurences
    //will keep the number of found instances of the substring
    //if the substring string is not even found return false
    uint32_t foundN = 0;
    int found_pos;
    size_t buff_size;
    uint32_t *buff;
    if (rf_string_find_byte_pos(s, sstr, options) == RF_FAILURE) {
        return NULL;
    }

    //if the given num is 0 just make sure we replace all
    if (*number == 0) {
        *number = UINT_MAX;
        // 24, initial size of buffer since we don't know how many occurences there will be
        buff_size = 24 * sizeof(uint32_t);
    } else {
        buff_size = (*number) * sizeof(uint32_t);
    }
    
    //find how many occurences exist
    if (!rf_stringx_from_string_in(&temp, s)) {
        return NULL;
    }

    rf_sbuffer_push(RF_TSBUFFS);
    buff = rf_sbuffer_alloc(RF_TSBUFFS, buff_size);
    if (!buff) {
        goto end;
    }

    found_pos = rf_string_find_byte_pos(RF_STRX2STR(&temp), sstr, options);
    buff[0] = found_pos;
    while (found_pos != RF_FAILURE) {
        int32_t move = buff[foundN] + rf_string_length_bytes(sstr);
        buff[foundN]= buff[foundN] + temp.bIndex;
        temp.bIndex += move;
        rf_string_data(&temp) += move;
        rf_string_length_bytes(&temp) -= move;
        foundN++;
        //if buffer is in danger of overflow realloc it
        if (foundN >= buff_size) {
            buff_size *= 2;
            buff = rf_sbuffer_extend(RF_TSBUFFS, buff_size);
            if (!buff) {
                RF_ERROR("Not enough memory to increase internal buffer");
                goto end;
            }
        }
        //if we found the required number of occurences break;
        if (foundN >= *number) {
            break;
        }
        found_pos = rf_string_find_byte_pos(RF_STRX2STR(&temp), sstr, options);
        buff[foundN] = found_pos;
    }

    //don't replace more than possible
    if (*number > foundN) {
        *number = foundN;
    }

  end:
    rf_stringx_deinit(&temp);
    return buff;
}
i_INLINE_INS void replace_greater(struct RFstring *s,
                                  uint32_t *buff,
                                  uint32_t number,
                                  const struct RFstring *sstr,
                                  const struct RFstring *rstr);
i_INLINE_INS void replace_lesser(struct RFstring *s,
                                 uint32_t *buff,
                                 uint32_t number,
                                 const struct RFstring *sstr,
                                 const struct RFstring *rstr);
i_INLINE_INS void replace_equal(struct RFstring *s,
                                const uint32_t *buff,
                                uint32_t number,
                                const struct RFstring *rstr);
