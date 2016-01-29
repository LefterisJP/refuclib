/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <String/rf_str_retrieval.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h> //for rf_stringx_init_unsafe()
#include <String/rf_str_conversion.h>
#include "rf_str_common.ph" //for required string private macros and functions
/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for bool
#include <Utils/rf_unicode.h> //rf_utf8_is_continuation_byte()
#include <Utils/bits.h>  //for RF_BITFLAG_ON
#include <Utils/memory.h> //for refu memory allocation
#include <Utils/sanity.h> //for sanity macros
#include <Utils/array.h> //for RFarray
/*------------- libc inclusion --------------*/
#include <stdarg.h>
#include <limits.h> //for INT_MAX e.t.c.
#include <stdio.h> // for sscanf
/*------------- End of includes -------------*/

// Finds the length of the string in characters
uint32_t rf_string_length(const struct RFstring *str)
{
    uint32_t length,i;
    RF_ASSERT(str, "got null string in function");
    RF_STRING_ITERATE_START(str, length, i)
    RF_STRING_ITERATE_END(length, i);
    return length;
}

bool rf_string_get_char(const struct RFstring *str, uint32_t c, uint32_t *cp)
{
    uint32_t length, i;
    RF_ASSERT(str, "got null string in function");
    if (!cp) {
        RF_WARNING("provided null pointer for the returned codepoint");
        return false;
    }
    RF_STRING_ITERATE_START(str, length, i)
        if(length == c)
        {
            *cp = rf_string_bytepos_to_codepoint(str, i);
            return true;
        }
    RF_STRING_ITERATE_END(length, i)

    return false;
}

const struct RFstring *rf_string_or_empty(const struct RFstring *s)
{
    return s ? s : rf_string_empty_get();
}

struct RFstring *rf_string_substr(
    const struct RFstring *s,
    uint32_t start_pos,
    uint32_t chars_num,
    int options,
    struct RFstring *ret
)
{
    uint32_t charI;
    uint32_t byteI;
    uint32_t startI;
    uint32_t endI;
    bool started = false;
    bool ended = false;
    startI = endI = 0;
    RF_ASSERT(s, "got null string in function");

    if (!RF_BITFLAG_ON(options, RF_SOPT_ASCII)) {
        if (!ret) {
            RF_WARNING("provided null pointer for the return string");
            return false;
        }

        RF_STRING_ITERATE_START(s, charI, byteI)
            if (charI == start_pos) {
                startI = byteI;
                started = true;
            }
            if (charI == start_pos + chars_num) {
                endI = byteI;
                ended = true;
                break;
            }
        RF_STRING_ITERATE_END(charI, byteI)
    }

    if (RF_BITFLAG_ON(options, RF_SOPT_TMP)) {
        if (start_pos >= rf_string_length_bytes(s)) {
            return false;
        }
        if (chars_num + start_pos >= rf_string_length_bytes(s)) {
            return false;
        }
        return RFS(
            "%.*s",
            chars_num,
            rf_string_data(s) + start_pos
        );
    } else {
        if (!ended) {
            // if the start was found return from there until the end of the string
            if (started) {
                endI = byteI;
            } else { // else the substring was not found
                return NULL;
            }
        }
        if (!rf_string_init_unsafe_nnt(ret, rf_string_data(s) + startI, endI - startI)) {
            return NULL;
        }
        return ret;
    }
}


int32_t rf_string_find(const struct RFstring *tstr,
                       const struct RFstring *sstr,
                       enum RFstring_matching_options options)
{
    /* sanity checks are performed inside rf_string_find_byte_pos() */
    int32_t ret;
    if ((ret = rf_string_find_byte_pos(tstr, sstr, options)) == RF_FAILURE) {
        return RF_FAILURE;
    }

    return rf_string_bytepos_to_charpos(tstr, ret, false);
}


int32_t rf_string_find_i(const struct RFstring *thisstr,
                         const struct RFstring *sstr,
                         uint32_t start_pos,
                         uint32_t length,
                         enum RFstring_matching_options options)
{
    struct RFstring sub;
    int32_t ret = RF_FAILURE;//the return value
    /* sanity checks are performed inside rf_string_substr() */
    if (!rf_string_substr(thisstr, start_pos, length, RF_SOPT_DEFAULT, &sub)) {
        return RF_FAILURE;
    }

    //now search for the sstr substring inside the sub substring defined by length
    if ((ret = rf_string_find(&sub, sstr, options)) != RF_FAILURE) {
        ret += start_pos;//return the proper position in the original string
    }
    rf_string_deinit(&sub);//free the sub substring and return

    return ret;
}

i_INLINE_INS bool rf_string_ends_with(const struct RFstring *thisstr,
                                       const struct RFstring *endstr,
                                       enum RFstring_matching_options options);

unsigned int rf_string_begins_with_any(const struct RFstring *thisstr,
                                       const struct RFstring *chars,
                                       const char *limit,
                                       unsigned int *bytes)
{
    bool iteration_match;
    size_t sub_length;
    uint32_t byte_position;
    unsigned int matching_chars = 0;
    unsigned i = 0;
    unsigned j = 0;
    uint32_t *buffer;

    rf_mbuffer_push(RF_TSBUFFM);
    //get all the codepoints of the string
    buffer = rf_string_fill_codepoints(chars, &sub_length);
    if (!buffer) {
        goto cleanup;
    }

    //iterate this string from the beginning
    i = 0;
    RF_STRING_ITERATE_START(thisstr, i, byte_position)
        iteration_match = true;
        //for every substring character
        for(j = 0; j < sub_length; j++) {
            //if we got a match
            if (rf_string_bytepos_to_codepoint(thisstr, byte_position) == buffer[j]) {
                iteration_match = false;
                matching_chars += 1;
                break;
            }
        }
        if (limit &&
            (limit - (rf_string_data(thisstr) + byte_position)) <= 0) {
            byte_position ++; /* gotta remember it's the next byte */
            break;
        }
        if (iteration_match) {
            break;
        }

    RF_STRING_ITERATE_END(i, byte_position)

cleanup:
    rf_mbuffer_pop(RF_TSBUFFM);
    if (bytes) {
        *bytes = byte_position;
    }
    return matching_chars;
}

int rf_string_count(const struct RFstring *tstr,
                    const struct RFstring *sstr,
                    unsigned int bytes,
                    struct RFarray *positions,
                    enum RFstring_matching_options options)
{
    (void)options; //reserved for future use
    unsigned int n;
    char *s;
    char *prs;
    unsigned int len;

    n = 0;
    if (!sstr) {
        RF_WARNING("Provided null search string");
        return -1;
    }
    s = rf_string_data(tstr);

    len = rf_string_length_bytes(tstr);
    if (bytes != 0) {
        len = bytes;
    }

    //as long as the substring is found in the string
    prs = s;
    s = strstr_nnt(s, len, rf_string_data(sstr), rf_string_length_bytes(sstr));
    while(s) {
        if (positions) {
            rf_array_set(positions, n, uint32_t,
                         s - rf_string_data(tstr), return -1);
        }
        n ++;
        len -= (s - prs) + rf_string_length_bytes(sstr);
        s += rf_string_length_bytes(sstr);
        prs = s;
        s = strstr_nnt(s, len,
                       rf_string_data(sstr), rf_string_length_bytes(sstr));
    }


    return n;
}


bool rf_string_scanf_after(const struct RFstring *str,
                           const struct RFstring *astr,
                           const char* format,
                           void* var)
{
    char *s;
    char *cstr;
    const char *found;
    unsigned int sub_length;
    bool ret = false;

    RF_ASSERT(str, "got null string in function");
    if (!astr) {
        RF_WARNING("Provided NULL \'after\' string");
        return false;
    }
    if (!format) {
        RF_WARNING("Provided NULL \'format\' c string");
        return false;
    }
    if (!var) {
        RF_WARNING("Provided NULL pointer for the return value");
        return false;
    }

    found = strstr_nnt(rf_string_data(str), rf_string_length_bytes(str),
                      rf_string_data(astr), rf_string_length_bytes(astr));
    if (!found) {
        return false;
    }
    //get a pointer to the start of the position where sscanf will be used
    s = rf_string_data(str) + (
        (found - rf_string_data(str)) + rf_string_length_bytes(astr)
    );


    // copy it into a temporary buffer to null terminate it for sscanf
    struct RFstring temps;
    sub_length = (rf_string_data(str) + rf_string_length_bytes(str)) - s + 1;
    RF_STRING_SHALLOW_INIT(&temps, s, sub_length - 1);
    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(&temps))) {
        goto end;
    }
    //use sscanf (!!!) strings are not null terminated
    if (sscanf(cstr, format, var) <= 0) {
        goto end;
    }
    ret = true;

end:
    //success
    RFS_POP();
    return ret;
}


bool rf_string_between(const struct RFstring *tstr,
                       const struct RFstring *lstr,
                       const struct RFstring *rstr,
                       void *result,
                       enum RFstring_matching_options options)
{
    int start, end;
    struct RFstring temp;
    bool ret = false;
    enum RFstring_matching_options new_options = options & RF_CASE_IGNORE
                                               ? RF_CASE_IGNORE : 0;
    RF_ASSERT(tstr, "got null string in function");
    /* null pointer check for lstr and rstr is in rf_string_find_byte_pos() */

    if (!result) {
        RF_WARNING("Provided null pointer for the result string");
        return false;
    }

    //find the left substring
    if ((start = rf_string_find_byte_pos(tstr, lstr, new_options)) == RF_FAILURE) {
        return false;
    }
    //get what is after it, no check since we already found it
    rf_string_after(tstr, lstr, &temp, new_options);
    //find the right substring in the remaining part
    if((end = rf_string_find_byte_pos(&temp, rstr, new_options)) == RF_FAILURE) {
        goto cleanup_temp;
    }

    //initialize the string to return
    if (options & RF_STRINGX_ARGUMENT) {
        if (!rf_stringx_assign_unsafe_nnt(
               result,
               rf_string_data(tstr) + start + rf_string_length_bytes(lstr),
               end)) {

            goto cleanup_temp;
        }
    } else {
        if (!rf_string_assign_unsafe_nnt(
                result,
                rf_string_data(tstr) + start + rf_string_length_bytes(lstr),
                end)) {
            goto cleanup_temp;
        }
    }
    //success
    ret = true;

cleanup_temp:
    //free temp string
    rf_string_deinit(&temp);
    return ret;
}

bool rf_string_beforev(const struct RFstring *thisstr,
                       void *result,
                       enum RFstring_matching_options options,
                       const unsigned char parN,  ...)
{
    const struct RFstring* s;
    int32_t i,minPos,thisPos;
    va_list argList;
    RF_ASSERT(thisstr, "got null string in function");

    if (!result) {
        RF_WARNING("Provided null string for the result");
        return false;
    }

    //get the parameter characters
    va_start(argList, parN);
    minPos = INT_MAX;
    for (i = 0; i < parN; i++) {
        s = (const struct RFstring*) va_arg(argList, struct RFstring*);
        /* null pointer check is in rf_string_find_byte_pos() */
        if ((thisPos= rf_string_find_byte_pos(thisstr, s, options))!= RF_FAILURE) {
            if (thisPos < minPos) {
               minPos = thisPos;
            }
        }
    }
    va_end(argList);

    //if it is not found
    if (minPos == INT_MAX) {
        return false;
    }
    //if it is found initialize the substring for return
    if (options & RF_STRINGX_ARGUMENT) {
        if (!rf_stringx_init_unsafe_nnt(result, rf_string_data(thisstr), minPos)) {
            return false;
        }
    } else {
        if (!rf_string_init_unsafe_nnt(result, rf_string_data(thisstr), minPos)) {
            return false;
        }
    }

    return true;
}

bool rf_string_before(const struct RFstring *thisstr,
                      const struct RFstring *sstr,
                      void *result,
                      enum RFstring_matching_options options)
{
    int32_t rv;
    RF_ASSERT(thisstr, "got null string in function");

    /* null pointer check for sstr is in rf_string_find_byte_pos() */
    if (!result) {
        RF_WARNING("Provided NULL pointer for the string to return");
        return false;
    }

    //find the substring
    if ((rv = rf_string_find_byte_pos(thisstr, sstr, options)) == RF_FAILURE) {
        return false;
    }
    //if it is found initialize the substring for return
    if (options & RF_STRINGX_ARGUMENT) {
        if (!rf_stringx_init_unsafe_nnt(result, rf_string_data(thisstr), rv)) {
            return false;
        }

    } else{
        if (!rf_string_init_unsafe_nnt(result, rf_string_data(thisstr), rv)) {
            return false;
        }

    }

    return true;
}

bool rf_string_after(const struct RFstring *thisstr,
                     const struct RFstring *after,
                     void *result,
                     enum RFstring_matching_options options)
{
    int32_t bytePos;
    RF_ASSERT(thisstr, "got null string in function");

    if (!result) {
        RF_WARNING("Null pointer given for the result string ");
        return false;
    }
    /* sanity check for after string is in rf_string_find_byte_pos() */
    //check for substring existence
    if ((bytePos = rf_string_find_byte_pos(thisstr, after, options)) == RF_FAILURE) {
        return false;
    }
    bytePos += rf_string_length_bytes(after);
    if (options & RF_STRINGX_ARGUMENT) {
        if(!rf_stringx_init_unsafe_nnt(
               result,
               rf_string_data(thisstr) + bytePos,
               rf_string_length_bytes(thisstr) - bytePos)) {

            return false;
        }
    } else {
        if(!rf_string_init_unsafe_nnt(
               result,
               rf_string_data(thisstr) + bytePos,
               rf_string_length_bytes(thisstr) - bytePos)) {

            return false;
        }
    }

    return true;
}


bool rf_string_afterv(const struct RFstring *thisstr,
                      void *result,
                      enum RFstring_matching_options options,
                      const unsigned char parN, ...)
{
    const struct RFstring* s;
    int32_t i, minPos, thisPos;
    uint32_t minPosLength=0;
    //will keep the argument list
    va_list argList;
    RF_ASSERT(thisstr, "got null string in function");

    if (!result) {
        RF_WARNING("Null pointer given for the result string ");
        return false;
    }

    //get the parameter characters
    va_start(argList,parN);
    minPos = INT_MAX;
    for (i = 0; i < parN; i++) {
        s = (const struct RFstring*) va_arg(argList, struct RFstring*);
        /* null pointer check is in rf_string_find_byte_pos() */
        if( (thisPos= rf_string_find_byte_pos(thisstr, s, options))!= RF_FAILURE) {
            if (thisPos < minPos) {
                minPos = thisPos;
                minPosLength = rf_string_length_bytes(s);
            }
        }
    }
    va_end(argList);
    //if it is not found
    if (minPos == INT_MAX) {
        return false;
    }
    //if it is found initialize the substring
    minPos += minPosLength;//go after the found substring
    if (options & RF_STRINGX_ARGUMENT) {
        if (!rf_stringx_init_unsafe_nnt(
                result,
                rf_string_data(thisstr) + minPos,
                rf_string_length_bytes(thisstr) - minPos)) {

            return false;
        }
    } else {
        if (!rf_string_init_unsafe_nnt(
                result,
                rf_string_data(thisstr) + minPos,
                rf_string_length_bytes(thisstr) - minPos)) {

            return false;
        }
    }

    return true;
}
