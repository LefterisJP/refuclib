/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */

#include <rflib/string/rf_str_conversion.h>

#include <rflib/string/rf_str_retrieval.h>
#include <rflib/string/rf_str_common.h>
#include "rf_str_common.ph"
#include "rf_str_defines.ph"

#include <rflib/defs/retcodes.h>
#include <rflib/utils/log.h>
#include <rflib/utils/rf_unicode.h>
#include <rflib/utils/memory.h>
#include <rflib/math/math.h>
#include <rflib/utils/sanity.h>
#include <rflib/persistent/buffers.h>

#include <errno.h>
#include <math.h>

uint16_t *rf_string_to_utf16(const struct RFstring *s, uint32_t *length)
{
    uint32_t* codepoints,charsN;
    uint16_t* utf16;
    RF_ASSERT(s, "got null string in function");
    if (length == NULL) {
        RF_WARNING("Did not provide a length argument");
        return NULL;
    }
    RF_MALLOC(codepoints, rf_string_length_bytes(s) * 4, return NULL);
    //get the unicode codepoints
    if(!rf_utf8_decode(rf_string_data(s), rf_string_length_bytes(s), &charsN,
                      codepoints, rf_string_length_bytes(s) * 4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        free(codepoints);
        return NULL;
    }
    //encode them in UTF-16, no check here since it comes from an RFstring
    // which is always guaranteed to have valid UTF-8 and as such valid codepoints
    RF_MALLOC(utf16, rf_string_length_bytes(s) * 4, return NULL);
    if(!rf_utf16_encode(codepoints, charsN, length,
                       utf16, rf_string_length_bytes(s) * 4))
    {
        RF_ERROR("Error at encoding a buffer in UTF-16");
        free(utf16);
        free(codepoints);
        utf16 = NULL;
    }
    free(codepoints);
    return utf16;
}

uint32_t *rf_string_to_utf32(const struct RFstring *s, uint32_t *length)
{
    uint32_t* cp;
    RF_ASSERT(s, "got null string in function");
    if (length == NULL) {
        RF_WARNING("Did not provide a length argument");
        return NULL;
    }
    RF_MALLOC(cp, rf_string_length_bytes(s) * 4, return NULL);
    //get the unicode codepoints
    if(!rf_utf8_decode(rf_string_data(s), rf_string_length_bytes(s), length,
                      cp, rf_string_length_bytes(s) * 4))
    {
        RF_ERROR("Error during decoding a UTF-8 byte stream");
        cp = NULL;
    }
    return cp;
}

char *rf_string_cstr(const struct RFstring *str)
{
    char* ret;
    RF_ASSERT(str, "got null string in function");
    RF_MALLOC(ret, rf_string_length_bytes(str) + 1, return NULL);
    memcpy(ret, rf_string_data(str), rf_string_length_bytes(str));
    ret[rf_string_length_bytes(str)] = '\0';
    return ret;
}

bool rf_string_to_int(const struct RFstring *str, int64_t *v, size_t *off)
{
    char *cstr;
    char *end;
    size_t length;
    bool ret = true;
    RF_ASSERT(str, "got null string in function");
    if (!v) {
        RF_WARNING("Provided null pointer for the returned int");
        return false;
    }

    RFS_PUSH();
    cstr = rf_string_cstr_from_buff_or_die(str);
    errno = 0;
    *v = strtoull (cstr, &end, 10);
    length = end - cstr;
    if (length == 0 || errno) {
        ret = false;
    }

    if (off) {
        *off = length - 1;
    }

    RFS_POP();
    return ret;
}

i_DECLIMEX_ bool rf_string_to_uint(const struct RFstring *str,
                                   size_t start_off,
                                   uint64_t *v,
                                   size_t *off,
                                   int base)
{
    char *cstr;
    char *end;
    bool ret = true;
    size_t length;
    RF_ASSERT(str, "got null string in function");
    if (!v) {
        RF_WARNING("Provided null pointer for the returned uint");
        return false;
    }

    RFS_PUSH();
    cstr = rf_string_cstr_from_buff_or_die(str);
    errno = 0;
    *v = strtoull (cstr + start_off, &end, base);
    length = end - cstr;
    if(length - start_off == 0 || errno) {
        ret = false;
    } else {
        if (off) {
            *off = length - 1;
        }
    }

    RFS_POP();
    return ret;
}

i_INLINE_INS bool rf_string_to_uint_dec(const struct RFstring *thisstr,
                                        uint64_t *v,
                                        size_t *off);
i_INLINE_INS bool rf_string_to_uint_hex(const struct RFstring *thisstr,
                                        uint64_t *v,
                                        size_t *off);
i_INLINE_INS bool rf_string_to_uint_bin(const struct RFstring *thisstr,
                                        uint64_t *v,
                                        size_t *off);
i_INLINE_INS bool rf_string_to_uint_oct(const struct RFstring *thisstr,
                                        uint64_t *v,
                                        size_t *off);

bool rf_string_to_double(const struct RFstring *str, double *f, size_t *off)
{
    char *cstr;
    char *end;
    bool ret = true;
    size_t length;
    RF_ASSERT(str, "got null string in function");
    if (!f) {
        RF_WARNING("Provided null pointer for the returned double");
        return false;
    }

    RFS_PUSH();
    cstr = rf_string_cstr_from_buff_or_die(str);
    errno = 0;
    *f = strtod (cstr, &end);
    length = end - cstr;
    if (length == 0 || errno) {
        ret = false;
    }

    if (off) {
        *off = length - 1;
    }

    RFS_POP();
    return ret;
}

void rf_string_to_lower(struct RFstring *s)
{
    uint32_t charI,byteI;
    RF_ASSERT(s, "got null string in function");

    RF_STRING_ITERATE_START(s, charI, byteI)
        //if the character is lowercase
        if(rf_string_data(s)[byteI] >= 65 && rf_string_data(s)[byteI] <= 90)
        {
            //turn it into uppercase
            rf_string_data(s)[byteI] += 32;
        }
    RF_STRING_ITERATE_END(charI, byteI)
}

void rf_string_to_upper(struct RFstring *s)
{
    uint32_t charI, byteI;
    RF_STRING_ITERATE_START(s, charI, byteI)
        //if the character is lowercase
        if(rf_string_data(s)[byteI] >= 97 && rf_string_data(s)[byteI] <= 122)
        {
            //turn it into uppercase
            rf_string_data(s)[byteI] -= 32;
        }
    RF_STRING_ITERATE_END(charI, byteI)
}

bool rf_string_tokenize(const struct RFstring *str,
                        const struct RFstring *sep,
                        uint32_t *ret_tokens_num,
                        struct RFstring **tokens)
{
    uint32_t i, sepLen;
    char *s;
    char *e;
    int32_t tokens_num;
    RF_ASSERT(str, "got null string in function");

    if (!sep) {
        RF_WARNING("Did not provide a separator string");
        return false;
    }

    if (!ret_tokens_num || !tokens) {
        RF_WARNING("Null pointers detected for the output data");
        return false;
    }

    //first find the occurences of the separator, and then the number of tokens
    tokens_num = rf_string_count(str, sep, 0, 0, 0) + 1;
    //error checking
    if (tokens_num <= 1) {
        return false;
    }
    //allocate the tokens
    RF_MALLOC(*tokens, sizeof(struct RFstring) * (tokens_num),
              return false);
    //find the length of the separator
    sepLen = rf_string_length_bytes(sep);

    s = rf_string_data(str);
    e = rf_string_data(str) + rf_string_length_bytes(str);
    // the cast is safe here due to the if check above
    for (i = 0; i < (uint32_t)tokens_num - 1; i ++) {
        //find each substring
        e = strstr_nnt(s, e - s,
                       rf_string_data(sep), rf_string_length_bytes(sep));
        rf_string_length_bytes(&(*tokens)[i]) = e - s;
        RF_MALLOC(rf_string_data(&(*tokens)[i]),
                  rf_string_length_bytes(&(*tokens)[i]),
                      return false
        );
        //put in the data
        memcpy(rf_string_data(&(*tokens)[i]),
               s,
               rf_string_length_bytes(&(*tokens)[i])
        );
        //prepare for next sub-string
        s = e + sepLen;

    }
    ///make sure that if it's the last substring we change strategy
    rf_string_length_bytes(&(*tokens)[i]) = (
        rf_string_length_bytes(str) - (s - rf_string_data(str))
    );
    RF_MALLOC(rf_string_data(&(*tokens)[i]),
              rf_string_length_bytes(&(*tokens)[i]),
              return false
    );
    //put in the data
    memcpy(rf_string_data(&(*tokens)[i]),
           s,
           rf_string_length_bytes(&(*tokens)[i])
    );

    *ret_tokens_num = tokens_num;

    return true;
}

const struct RFstring *rf_string_ordinal(unsigned int num)
{
    if (num <= 4 || num >= 21) { // these rules don't apply for 10-20
        switch (num % 10) {
        case 1:
            return RFS("%ust", num);
        case 2:
            return RFS("%und", num);
        case 3:
            return RFS("%urd", num);
        }
    }
    // else
    return RFS("%uth", num);
}
