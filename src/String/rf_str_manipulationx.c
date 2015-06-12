/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
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
#include <Utils/sanity.h> //for sanity macros
/*------------- End of includes -------------*/

bool rf_stringx_append(struct RFstringx* thisstr, const struct RFstring *other)
{
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!other) {
        RF_WARNING("Provided null string for appending");
        return false;
    }
    
    return rf_stringx_generic_append(thisstr, rf_string_data(other),
                                     rf_string_length_bytes(other));
}

bool rf_stringx_append_chars(struct RFstringx *thisstr,
                             const struct RFstring *other,
                             unsigned int chars)
{
    uint32_t i;
    uint32_t length = 0;
    unsigned end = 0;
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!other) {
        RF_WARNING("Provided null string for appending");
        return false;
    }

    //find out the bytelength of the requested char position
    RF_STRING_ITERATE_START(other, length, i);
        if (length == chars) {
            end = i;
            break;
        }
    RF_STRING_ITERATE_END(length, i)
    //if the end is not found
    if (!end) {
        end = rf_string_length_bytes(other);
    }
    return rf_stringx_generic_append(thisstr, rf_string_data(other), end);
}

bool rf_stringx_append_char(struct RFstringx* thisstr, uint32_t unichar)
{
    char utf8[MAX_UTF8C_BYTES];
    int length;
    RF_ASSERT(thisstr, "got NULL string in function");
    if((length = rf_utf8_encode_single(unichar,utf8)) <= 0)
    {
        RF_ERROR("Encoding the given unicode codepoint to UTF8 failed");
        return false;
    }

    return rf_stringx_generic_append(thisstr, utf8, length);
}

bool rf_stringx_prepend(struct RFstringx* thisstr, const struct RFstring *other)
{
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!other) {
        RF_WARNING("Provided null string for prepending");
        return false;
    }

    //if the new string does not fit inside the buffer reallocate it
    RF_STRINGX_REALLOC(
        thisstr,
        rf_string_length_bytes(thisstr) + rf_string_length_bytes(other),
        return false
    );

    if (!rf_string_generic_prepend(RF_STRX2STR(thisstr),
                                   rf_string_data(other),
                                   rf_string_length_bytes(thisstr),
                                   rf_string_length_bytes(other))) {
        return false;
    }

    return true;
}

bool rf_stringx_insert(struct RFstringx* thisstr,
                       uint32_t pos,
                       const struct RFstring *other)
{
    uint32_t length;
    uint32_t bytePos;
    uint32_t size;
    uint32_t i;
    bool found = false;
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!other) {
        RF_WARNING("Provided null string for inserting");
        return false;
    }

    //iterate this string to find the byte position of the character position
    RF_STRING_ITERATE_START(thisstr, length, i)
        if (length == pos) {
            //found the position. Is saved in bytePos
            bytePos = i;
            found = true;
            break;
        }
    RF_STRING_ITERATE_END(length, i);
    if (!found) {
        return false;
    }


    //keep the original byte length here
    size = rf_string_length_bytes(thisstr);
    //get the new byte length
    rf_string_length_bytes(thisstr) += rf_string_length_bytes(other);
    //check if the new string fits in the buffer and if not reallocate it
    RF_STRINGX_REALLOC(thisstr, rf_string_length_bytes(thisstr), return false);

    //move the string's contents to make room for the extra string insertion
    memmove(
        rf_string_data(thisstr) + rf_string_length_bytes(other) + bytePos,
        rf_string_data(thisstr) + bytePos, size - bytePos);
    //now insert the new string
    memcpy(rf_string_data(thisstr) + bytePos,
           rf_string_data(other),
           rf_string_length_bytes(other));

    return true;
}

/*--- RFstringx unsafe appending functions---*/

bool rf_stringx_append_bytes(struct RFstringx* thisstr,
                             const struct RFstring *other,
                             const unsigned int bytes)
{
    unsigned int add_bytes;
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!other) {
        RF_WARNING("Provided null string for byte appending");
        return false;
    }

    add_bytes = rf_string_length_bytes(other);
    if (bytes < add_bytes) {
        add_bytes = bytes;
    }
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(
        thisstr,
        rf_string_length_bytes(thisstr) + add_bytes,
        return false);
    
    return rf_stringx_generic_append(thisstr, rf_string_data(other), add_bytes);
}

bool rf_stringx_append_cstr(struct RFstringx* thisstr, const char* cstr)
{
    size_t len;
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!cstr) {
        RF_WARNING("Provided null c string for appending");
        return false;
    }
    len = strlen(cstr);
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(
        thisstr,
        rf_string_length_bytes(thisstr) + len,
        return false);

    return rf_stringx_generic_append(thisstr, cstr, len);
}

/*--- RFstringx replacing functions---*/

bool rf_stringx_replace(struct RFstringx *thisstr,
                        const struct RFstring *sstr,
                        const struct RFstring *rstr,
                        uint32_t num,
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
    if (!(buff = replace_intro(RF_STRX2STR(thisstr), &number, sstr, options))) {
        return false;
    }

    //act depending on the size difference of rstr and sstr
    if (rf_string_length_bytes(rstr) > rf_string_length_bytes(sstr)) {
        //reallocate the string if needed
        RF_STRINGX_REALLOC(
            thisstr,
            rf_string_length_bytes(thisstr) + number * (
                rf_string_length_bytes(rstr) - rf_string_length_bytes(sstr)
            ),
            goto end
        );
        replace_greater(RF_STRX2STR(thisstr), buff, number, sstr, rstr);
    } else if (rf_string_length_bytes(rstr) < rf_string_length_bytes(sstr)) {
        replace_lesser(RF_STRX2STR(thisstr), buff, number, sstr, rstr);
    } else { //replace and remove strings are equal
        replace_equal(RF_STRX2STR(thisstr), buff, number, rstr);
    }
    ret = true;

end:
    rf_sbuffer_pop(RF_TSBUFFS);
    return ret;
}

//TODO: I really don't like this temporary string_buff. Try to get rid of it
bool rf_stringx_replace_between(struct RFstringx *thisstr,
                                const struct RFstring *left,
                                const struct RFstring *right,
                                const struct RFstring *rstr,
                                enum RFstring_matching_options options,
                                uint32_t i)
{
    uint32_t j,move,start = thisstr->bIndex;
    bool found = false;
    bool ret = false;
    bool same_separators = false;
    struct RFstringx string_buff;
    RF_ASSERT(thisstr, "got NULL string in function");

    if (!left) {
        RF_WARNING("Provided null pointer for the left string");
        return false;
    }
    if (!right) {
        RF_WARNING("Provided null pointer for the right string");
        return false;
    }
    if (!rstr) {
        RF_WARNING("Provided null pointer for the replace string");
        return false;
    }

    if (rf_string_equal(left, right)) {
        same_separators = true;
    }

    if (!rf_stringx_init_buff(&string_buff, 128, "")) {
        RF_ERROR("No memory");
        return false;
    }

    if (i == 0) { //if we want all occurences replaced
        
        //while we have occurences of the pair
        while (!rf_string_between(RF_STRX2STR(thisstr), left, right, &string_buff, options)) {
            found = true;
            /*
             * move the internal pointer right after the left part of the pair
             * and replace what was found there. None of the functions
             * below fail since the while condition is true
             */
            rf_stringx_move_after(thisstr, left, 0, options);
            if (rf_stringx_replace(thisstr, RF_STRX2STR(&string_buff), rstr, 1, options)) {
                goto cleanup2;
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

        //if we replaced at least 1 occurence
        if (found) {
            move = thisstr->bIndex - start;
            rf_string_data(thisstr) -= move;
            thisstr->bIndex = start;
            rf_string_length_bytes(thisstr) += move;
            //success
            ret = true;
        }
        goto cleanup2;
    }

    ///if we check for a specific instance
    for (j = 1; j < i; j++) {
        //move after the pair of the 'j' inbetween substrings
        if (!rf_stringx_move_after_pair(thisstr, left, right, 0, options, 0)) {
            //and if the occurence does not exist
            move = thisstr->bIndex - start;
            rf_string_data(thisstr) -= move;
            thisstr->bIndex = start;
            //and increase its bytelength
            rf_string_length_bytes(thisstr) += move;
            goto cleanup2;//falure
        }
    }
    //move after the pair of the inbetween substrings we actually want
    if (!rf_string_between(RF_STRX2STR(thisstr), left, right, &string_buff, options)) {
        //and if the occurence does not exist
        move = thisstr->bIndex-start;
        rf_string_data(thisstr) -= move;
        thisstr->bIndex = start;
        //and increase its bytelength
        rf_string_length_bytes(thisstr) += move;
        goto cleanup2;//falure
    }
    ///success
    //move after the left part of the pair
    rf_stringx_move_after(thisstr, left, 0, options);
    //and then replace the occurence
    if (!rf_stringx_replace(thisstr, RF_STRX2STR(&string_buff), rstr, 1, options)) {
        //failure
        goto cleanup2;
    }
    //now we are done and should go back
    move = thisstr->bIndex - start;
    rf_string_data(thisstr) -= move;
    thisstr->bIndex = start;
    rf_string_length_bytes(thisstr) += move;

    //if we get here success
    ret = true;
cleanup2:
    rf_stringx_deinit(&string_buff);
    return ret;
}




i_INLINE_INS bool rf_stringx_generic_append(struct RFstringx* s,
                                            const char* other,
                                            unsigned int bytes_to_copy);
