#include <String/regex.h>

// some useful PCRE2 links:
// http://www.regular-expressions.info/pcre2.html
// http://www.pcre.org/current/doc/html/pcre2api.html
// http://www.pcre.org/current/doc/html/pcre2demo.html
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <Utils/sanity.h>
#include <Utils/memory.h>
#include <String/rf_str_core.h>


struct RFre {
    pcre2_code *re;
};

#define PCRE_BUFF_SIZE 256
#define RF_PCRE_ERROR_OFF(msg_, err_, off_, buff_, bufflen_)            \
    do {                                                                \
        int i_msglen_ = pcre2_get_error_message(err_,buff_, bufflen_);  \
        if (i_msglen_ < 0) {                                            \
            RF_ERROR("pcre2_get_error_message() failed. Provide bigger buffer"); \
        } else {                                                        \
            RF_ERROR(msg_ " at offset %d with error:\n %.*s.", off_, buff_, i_msglen_); \
        }                                                               \
    } while(0)


#define RF_PCRE_ERROR(msg_, err_, buff_, bufflen_)                      \
    do {                                                                \
        int i_msglen_ = pcre2_get_error_message(err_,buff_, bufflen_);  \
        if (i_msglen_ < 0) {                                            \
            RF_ERROR("pcre2_get_error_message() failed. Provide bigger buffer"); \
        } else {                                                        \
            RF_ERROR(msg_ " with error:\n %.*s.", buff_, i_msglen_);    \
        }                                                               \
    } while(0)

struct RFre *rfre_compile(const struct RFstring *pattern)
{
    struct RFre *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);

    int error_num;
    PCRE2_SIZE error_offset;
    uint8_t buff[PCRE_BUFF_SIZE];
    ret->re = pcre2_compile(
        (PCRE2_SPTR8)rf_string_data(pattern),
        rf_string_length_bytes(pattern),
        PCRE2_NO_UTF_CHECK,
        &error_num,
        &error_offset,
        NULL);

    if (!ret->re) {
        RF_PCRE_ERROR_OFF("pcre2_compile() failed", error_num, error_offset, buff, PCRE_BUFF_SIZE);
        free(ret);
        return NULL;
    }
    return ret;
}

bool rfre_match(struct RFre *re, const struct RFstring *subject, struct string_arr *matches)
{
    pcre2_match_data *match_data;
    int rc;
    uint8_t buff[PCRE_BUFF_SIZE];
    bool ret = false;

    if (!(match_data = pcre2_match_data_create_from_pattern(re->re, NULL))) {
        RF_ERROR("pcre2_match_data_create_from_pattern() failed");
        return false;
    }
    rc = pcre2_match(
        re->re,
        (PCRE2_SPTR8)rf_string_data(subject),
        rf_string_length_bytes(subject),
        0,                    /* start at offset 0 in the subject */
        PCRE2_NO_UTF_CHECK | PCRE2_UTF,
        match_data,
        NULL);

    if (rc < 0) {
        if (rc == PCRE2_ERROR_NOMATCH) {
            RF_PCRE_ERROR("pcre2_match() failed", rc, buff, PCRE_BUFF_SIZE);
        }
        goto free_match_data;
    }
    if (rc == 0) {
        // should not happen here since we used pcre2_match_data_create_from_pattern()
        RF_ERROR("pcre2_match() failed due to ovector not being big enough.");
        goto free_match_data;
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
    darray_init(*matches);
    int i;
    for (i = 0; i < rc; ++i) {
        darray_resize(*matches, matches->size + 1);
        RF_STRING_SHALLOW_INIT(
            &darray_top(*matches),
            (char*)ovector[2 * i],
            ovector[2 * i + 1] - ovector[2 * i]);
    }

    // success
    ret = true;

free_match_data:
    pcre2_match_data_free(match_data);
    return ret;
}
