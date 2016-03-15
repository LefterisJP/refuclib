/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/string/rf_str_traversalx.h>

#include <rflib/string/rf_str_retrieval.h>
#include <rflib/string/rf_str_core.h>
#include <rflib/string/rf_str_corex.h>
#include "rf_str_common.ph"

#include <rflib/defs/retcodes.h>
#include <rflib/utils/rf_unicode.h>
#include <rflib/utils/memory.h>
#include <rflib/utils/sanity.h>

#include <stdarg.h>
#include <limits.h>

static inline bool move_internal_ptr(struct RFstringx* s, int32_t move,
                                     void *resultP, uint32_t len, char options)
{
    s->bIndex += move;
    rf_string_data(s) += move;
    //also reduce the byte length of this string
    rf_string_length_bytes(s) -= move;

    //also if we want the string returned
    if (resultP != 0)
    {
        if (options & RF_STRINGX_ARGUMENT) {
            struct RFstringx* result = (struct RFstringx*) resultP;
            rf_stringx_reset(result);
            RF_STRINGX_REALLOC(result, move - len, return false);
            rf_string_length_bytes(result) = move - len;
            memcpy(
                rf_string_data(result),
                rf_string_data(s) - move,
                rf_string_length_bytes(result)
            );
        } else if (options & RF_STRING_DEPENDENT) {
            struct RFstring *result = (struct RFstring*) resultP;
            rf_string_length_bytes(result) = move - len;
            rf_string_data(result) = rf_string_data(s) - move;
        } else {
            struct RFstring* result = (struct RFstring*) resultP;
            if (rf_string_length_bytes(result) > move - len) {
                RF_REALLOC(rf_string_data(result), char,
                          rf_string_length_bytes(result), return false);
            }
            rf_string_length_bytes(result) = move - len;
            memcpy(
                rf_string_data(result),
                rf_string_data(s) - move,
                rf_string_length_bytes(result)
            );
        }
    }
    return true;
}

int32_t rf_stringx_move_after(struct RFstringx* thisstr,
                              const struct RFstring *sub,
                              void *result,
                              enum RFstring_matching_options options)
{
    int32_t move;
    /* rf_string_find_byte_pos takes care of invalid input checking */

    //check for substring existence and return failure if not found
    if((move = rf_string_find_byte_pos(RF_STRX2STR(thisstr), sub, options)) == RF_FAILURE) {
        return RF_FAILURE;
    }
    //if found, move the internal pointer
    move += rf_string_length_bytes(sub);

    if(!move_internal_ptr(thisstr, move, result,
                          rf_string_length_bytes(sub), options)) {
        move = RF_FAILURE;
    }

    return move;
}

void rf_stringx_move_back(struct RFstringx* thisstr, uint32_t n)
{
    uint32_t length;
    RF_ASSERT(thisstr, "got null string in function");
    length = 0;

    while(thisstr->bIndex >0) {
        if(!rf_utf8_is_continuation_byte(rf_string_data(thisstr)[0])) {
            if(n == length) {
                break;
            }
            length ++;
        }
        //so keep changing the internal index and the bytes pointer
        rf_string_data(thisstr)--;
        thisstr->bIndex--;
        //and also keep adding to the bytelength
        rf_string_length_bytes(thisstr) ++;
    }
}

i_INLINE_INS void rf_stringx_move_to_index(struct RFstringx* thisstr,
                                           uint32_t n);

void rf_stringx_move_forward(struct RFstringx* thisstr, uint32_t n)
{
    uint32_t length;
    RF_ASSERT(thisstr, "got null string in function");
    length = 0;

    while(rf_string_length_bytes(thisstr) != 0)
    {
        if(!rf_utf8_is_continuation_byte(rf_string_data(thisstr)[0]))
        {
            if(n == length)
            {
                break;
            }
            length ++;
        }
        //so keep changing the internal index and the bytes pointer
        rf_string_data(thisstr)++;
        thisstr->bIndex++;
        //and reducing from the byte length
        rf_string_length_bytes(thisstr)--;
    }
}

i_INLINE_INS void rf_stringx_move_end(struct RFstringx* s);
i_INLINE_INS void rf_stringx_move_bytes(struct RFstringx* s,
                                        int bytes_num);

void rf_stringx_reset(struct RFstringx* thisstr)
{
    rf_string_data(thisstr) -= thisstr->bIndex;
    rf_string_length_bytes(thisstr) += thisstr->bIndex;
    thisstr->bIndex = 0;
}

void rf_stringx_clear(struct RFstringx *thisstr)
{
    rf_stringx_reset(thisstr);
    rf_string_length_bytes(thisstr) = 0;
}

bool rf_stringx_move_afterv(struct RFstringx* thisstr,
                            void* result,
                            enum RFstring_matching_options options,
                            const unsigned char parN, ...)
{
    uint32_t i,paramLen;
    int32_t minPos;
    int32_t thisPos;
    //will keep the argument list
    va_list argList;
    /* validity of input is checked for in the following functions */

    // will keep the winning parameter length
    paramLen = 0;
    //get the parameter characters
    va_start(argList,parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        //get the param
        struct RFstring* s = va_arg(argList, struct RFstring*);
        //if the parameter got found in the string see if it's the closest
        if((thisPos = rf_string_find_byte_pos(RF_STRX2STR(thisstr), s, options)) != RF_FAILURE)
        {
            if(thisPos < minPos)
            {
                minPos = thisPos;
                //keep the length of the winnning parameter
                paramLen = rf_string_length_bytes(s);
            }
        }
    }
    va_end(argList);

    //if it is not found
    if (minPos == INT_MAX) {
        return false;
    }
    //move the internal pointer after the substring
    return move_internal_ptr(thisstr, minPos + paramLen, result, paramLen, options);
}

unsigned int rf_stringx_skip_chars(struct RFstringx* thisstr,
                                   const struct RFstring *chars,
                                   const char *limit,
                                   unsigned int *bytes,
                                   unsigned int *line_count)
{
    uint32_t bytes_to_move;
    unsigned int chars_to_skip;
    static const struct RFstring nl = RF_STRING_STATIC_INIT("\n");

    chars_to_skip = rf_string_begins_with_any(RF_STRX2STR(thisstr),
                                              chars, limit,
                                              &bytes_to_move);

    if (chars_to_skip && line_count) {
        *line_count = rf_string_count(RF_STRX2STR(thisstr), &nl, bytes_to_move, 0, 0);
    }

    if (chars_to_skip) {
        rf_stringx_move_bytes(thisstr, bytes_to_move);
    }

    if (bytes) {
        *bytes = bytes_to_move;
    }
    return chars_to_skip;
}

//TODO: replace this ugly string_buff with something else
bool rf_stringx_move_after_pair(struct RFstringx* thisstr,
                                const struct RFstring *left,
                                const struct RFstring *right,
                                void *resultP,
                                enum RFstring_matching_options options,
                                uint32_t occurence)
{
    uint32_t i,move,start = thisstr->bIndex;
    bool found = false;
    bool ret = true;
    bool same_separators = false;
    struct RFstringx string_buff;
    if (!left || !right) {
        RF_WARNING("Provided NULL substring for left or right pair");
        return false;
    }
    //check the occurence parameter
    if (occurence == 0) {
        occurence = 1;
    }

    if (!rf_stringx_init_buff(&string_buff, 128, "")) {
        return false;
    }

    if (rf_string_equal(left, right)) {
        same_separators = true;
    }

    //get the in between string and if it is null return false
    for (i = 1; i <= occurence; i ++) {

        //attempt to get the in between string
        if(!rf_string_between(RF_STRX2STR(thisstr), left, right,
                              &string_buff, options|RF_STRINGX_ARGUMENT))
        {
            ret = false;
            goto cleanup_str_buff;
        }

        //move after this occurence of the pair
        rf_stringx_move_after(thisstr, RF_STRX2STR(&string_buff), 0, options);

        /* if left == right don't go over the right separator when searching */
        if (!same_separators) {
            rf_stringx_move_after(thisstr, right, 0, options);
        }

        //if we found it
        if (i == occurence) {
            found = true;
            break;
        }
    }
    //if we get here and the result is not found return failure
    if (!found) {
        //get the pointer back
        move = thisstr->bIndex - start;
        rf_string_data(thisstr) -= move;
        rf_string_length_bytes(thisstr) += move;
        thisstr->bIndex = start;
        ret = false;
        goto cleanup_str_buff;
    }
    //if we don't want to keep the result free it
    if (resultP) {
        if (options & RF_STRINGX_ARGUMENT) {
            rf_stringx_copy_in(resultP, &string_buff);
        } else {
            rf_string_copy_in(resultP, RF_STRX2STR(&string_buff));
        }
    }

cleanup_str_buff:
    rf_stringx_deinit(&string_buff);
    return ret;
}
