#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <refu.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
#include <String/rf_str_traversalx.h>


static bool test_accept_vargs(struct RFstring *s,
                              struct RFstring **alloc_s,
                              const char *fmt, ...)
{
    bool ret;
    va_list args;
    va_list copy_list;

    va_start(args, fmt);
    va_copy(copy_list, args);
    *alloc_s = rf_string_createvl(fmt, copy_list);
    va_end(copy_list);
    ret = rf_string_initvl(s, fmt, args);
    va_end(args);

    return ret;
}

static bool test_acceptx_vargs(struct RFstringx *s,
                               struct RFstringx **alloc_s,
                               const char *fmt, ...)
{
    bool ret;
    va_list args;
    va_list copy_list;

    va_start(args, fmt);
    va_copy(copy_list, args);
    *alloc_s = rf_stringx_createvl(fmt, copy_list);
    va_end(copy_list);
    ret = rf_stringx_initvl(s, fmt, args);
    va_end(args);

    return ret;
}

/* --- String Initialization Tests --- START --- */
START_TEST(test_string_init) {
    struct RFstring s;
    ck_assert(rf_string_init(&s, "This is the test of String initialization"));
    ck_assert_rf_str_eq_cstr(&s, "This is the test of String initialization");

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_initv) {
    struct RFstring s, s2;
    ck_assert(
        rf_string_initv(&s,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    ck_assert_rf_str_eq_cstr(&s,
                             "Printf style initialization 1337 3.142 912341");

    RF_STRING_SHALLOW_INIT(&s2, rf_string_data(&s), rf_string_length(&s));
    ck_assert_rf_str_eq_cstr(&s2,
                             "Printf style initialization 1337 3.142 912341");

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_initvl) {
    struct RFstring s, s2;
    struct RFstring *s1;
    ck_assert(
        test_accept_vargs(&s,
                          &s1,
                          "%s %d %.3f %u",
                          "Printf style initialization",
                          1337, 3.141592, 912341)
    );
    ck_assert_rf_str_eq_cstr(&s,
                             "Printf style initialization 1337 3.142 912341");
    ck_assert_rf_str_eq_cstr(s1,
                             "Printf style initialization 1337 3.142 912341");

    RF_STRING_SHALLOW_INIT(&s2, rf_string_data(&s), rf_string_length(&s));
    ck_assert_rf_str_eq_cstr(&s2,
                             "Printf style initialization 1337 3.142 912341");

    rf_string_deinit(&s);
    rf_string_destroy(s1);
}END_TEST

START_TEST(test_string_init_cp) {
    struct RFstring s;
    ck_assert(rf_string_init_cp(&s, 0x2708));
    ck_assert_rf_str_eq_cstr(&s, "✈");

    rf_string_deinit(&s);
}END_TEST


START_TEST(test_string_init_int) {
    struct RFstring s1, s2, s3;

    ck_assert(rf_string_init_int(&s1, 0));
    ck_assert_rf_str_eq_cstr(&s1, "0");

    ck_assert(rf_string_init_int(&s2, 1495872));
    ck_assert_rf_str_eq_cstr(&s2, "1495872");

    ck_assert(rf_string_init_int(&s3, -909283));
    ck_assert_rf_str_eq_cstr(&s3, "-909283");

    rf_string_deinit(&s3);
    rf_string_deinit(&s2);
    rf_string_deinit(&s1);
}END_TEST

START_TEST(test_string_init_double) {
    struct RFstring s1, s2, s3;

    ck_assert(rf_string_init_double(&s1, 0, 1));
    ck_assert_rf_str_eq_cstr(&s1, "0.0");

    ck_assert(rf_string_init_double(&s2, 54.12452, 6));
    ck_assert_rf_str_eq_cstr(&s2, "54.124520");

    ck_assert(rf_string_init_double(&s3, -0.00192, 5));
    ck_assert_rf_str_eq_cstr(&s3, "-0.00192");

    rf_string_deinit(&s3);
    rf_string_deinit(&s2);
    rf_string_deinit(&s1);
}END_TEST


START_TEST(test_string_init_utf16) {
    struct RFstring s1;

    const uint16_t utf16_buffer[] = {0xD834, 0xDD1E, 0x6771};
    unsigned int utf16b_len = 3 * sizeof(uint16_t);

    /* G-clef music symbol + Higashi*/
    ck_assert(rf_string_init_utf16(&s1, utf16_buffer, utf16b_len));
    ck_assert_rf_str_eq_cstr(&s1, "\xf0\x9d\x84\x9e東");

    rf_string_deinit(&s1);
}END_TEST

START_TEST(test_string_init_utf32) {
    struct RFstring s1;

    const uint32_t utf32_buffer[] = {0x6771,0x4EAC};
    unsigned int utf32b_len = 2;

    ck_assert(rf_string_init_utf32(&s1, utf32_buffer, utf32b_len));
    ck_assert_rf_str_eq_cstr(&s1, "東京");

    rf_string_deinit(&s1);
}END_TEST

START_TEST(test_string_init_unsafe_nnt) {
    struct RFstring s;


    ck_assert(
        rf_string_init_unsafe_nnt(&s,
                                  "A big string with unicode "
                                  "characters चुनाव included",
                                  52)
    );
    ck_assert_rf_str_eq_cstr(&s, "A big string with unicode characters चुनाव");

    rf_string_deinit(&s);
}END_TEST

START_TEST (test_invalid_string_init) {
    struct RFstring s;
    ck_assert(!rf_string_init(&s, NULL));
    ck_assert(!rf_string_initv(&s, NULL));
    ck_assert(!rf_string_init_utf16(&s, 0, 0));
    ck_assert(!rf_string_init_utf32(&s, 0, 0));
} END_TEST
/* --- String Initialization Tests --- END --- */


/* --- String Assignment Tests --- START --- */
START_TEST(test_string_assign) {
    struct RFstring s, s2;

    ck_assert(rf_string_init(&s, "Nana this is an apple"));
    ck_assert(rf_string_init(&s2,  "Nana this is a tasty orange"));

    ck_assert(rf_string_assign(&s, &s2));
    ck_assert_rf_str_eq_cstr(&s, "Nana this is a tasty orange");

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_assignv) {
    struct RFstring s, s2;
    static const struct RFstring big = RF_STRING_STATIC_INIT(
        "this is a big string, oder?");
    ck_assert(rf_string_init(&s, "Nana this is an apple"));
    ck_assert(rf_string_init(&s2,  "small"));

    ck_assert(rf_string_assignv(&s, "number: %d", 1337));
    ck_assert_rf_str_eq_cstr(&s, "number: 1337");

    // see if reallocation happens correctly
    ck_assert(rf_string_assignv(&s2, "number: %d and string: \""
                                RFS_PF"\"", 1337, RFS_PA(&big)));
    ck_assert_rf_str_eq_cstr(&s2, "number: 1337 and string: \"this is a big "
                             "string, oder?\"");
    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_assign_char) {
    struct RFstring s, s2;

    ck_assert(rf_string_init(&s, "Nana this is an apple"));
    ck_assert(rf_string_assign_char(&s, 0xB1));
    ck_assert_rf_str_eq_cstr(&s, "±");

    ck_assert(rf_string_init(&s2, "a"));
    ck_assert(rf_string_assign_char(&s2, 0xB1));
    ck_assert_rf_str_eq_cstr(&s2, "±");

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_assign_unsafe_nnt) {
    struct RFstring s;
    const char cstr[] = "Assigned string, bigger than the assignee";

    ck_assert(rf_string_init(&s, "Small string"));
    ck_assert(rf_string_assign_unsafe_nnt(&s, cstr, strlen(cstr)));
    ck_assert_rf_str_eq_cstr(&s, "Assigned string, bigger than the assignee");

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_invalid_string_assign) {
    struct RFstring s;

    ck_assert(!rf_string_assign(&s, NULL));
    ck_assert(!rf_string_assign_unsafe_nnt(&s, NULL, 1));

}END_TEST
/* --- String Assignment Tests --- END --- */

/* --- String Copying Tests --- START --- */

START_TEST(test_string_copy_in) {
    struct RFstring s, s2, s3;

    ck_assert(rf_string_init(
                  &s,
                  "Robot rock, Time of your life, Human after all")
    );

    ck_assert(rf_string_copy_in(&s2, &s));
    ck_assert_rf_str_eq_cstr(
        &s2, "Robot rock, Time of your life, Human after all");
    RF_STRING_SHALLOW_COPY(&s3, &s);
    ck_assert_rf_str_eq_cstr(
        &s3, "Robot rock, Time of your life, Human after all");

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_copy_chars) {
    struct RFstring s, s2, s3;

    ck_assert(rf_string_init(
                  &s,
                  "Robot rock, Time of your life, Human after all")
    );

    ck_assert(rf_string_copy_chars(&s2, &s, 10));
    ck_assert_rf_str_eq_cstr(&s2, "Robot rock");

    ck_assert(rf_string_copy_chars(&s3, &s, 0));
    ck_assert_rf_str_eq_cstr(&s3, "");

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_invalid_string_copy) {
    struct RFstring s;

    ck_assert(!rf_string_copy_in(&s, NULL));
    ck_assert(!rf_string_copy_chars(&s, NULL, 0));
}END_TEST
/* --- String Copying Tests --- END --- */

/* --- String Misc Tests --- START --- */
START_TEST(test_string_equal) {
    struct RFstringx sx;
    struct RFstring s;

    ck_assert(rf_stringx_init(
                  &sx,
                  "Δοκιμαζουμε την αναθεση για StringX"
              )
    );
    ck_assert(rf_string_init(
                  &s,
                  "Δοκιμαζουμε την αναθεση για StringX"
              )
    );
    ck_assert(rf_string_equal(&s, RF_STRX2STR(&sx)));

    rf_stringx_deinit(&sx);
    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_bytepos_to_codepoint) {
    struct RFstring s;
    ck_assert(
           rf_string_init(
               &s,
               "【北京＝峯村健司】中国共産党の第１７期中央委員会"
               "第７回全体会議（７中全会）が１日、北京市内で開会"
               "した模様だ。８日からの第１８回党大会の準備作業を"
               "する。党指導部の世代交代が進むとみられる５年に１"
               "度の党大会前後の一連の政治的な行事が本格的に始ま"
               "った。")
    );

    /* expecting 20581, decimal value of the the character at position 18 '健' */
    ck_assert_int_eq(20581,
                     rf_string_bytepos_to_codepoint(&s, 18)
    );

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_bytepos_to_charpos) {
    struct RFstring s;
    ck_assert(
           rf_string_init(
               &s,
               "【北京＝峯村健司】中国共産党の第１７期中央委員会"
               "第７回全体会議（７中全会）が１日、北京市内で開会"
               "した模様だ。８日からの第１８回党大会の準備作業を"
               "する。党指導部の世代交代が進むとみられる５年に１"
               "度の党大会前後の一連の政治的な行事が本格的に始ま"
               "った。")
    );

    /* expecting 9, (character position of byte position 27 '中') */
    ck_assert_int_eq(9,
                     rf_string_bytepos_to_charpos(&s, 27, false)
    );
    /* expecting 9: (character position of byte position 29 and if it
       is a continuation byte take the previous char which is '中') */
    ck_assert_int_eq(9,
                     rf_string_bytepos_to_charpos(&s, 27, true)
    );
    /* expecting 10: (character position of byte position 28 and if it
       is a continuation byte take the next char which is '国') */
    ck_assert_int_eq(10,
                     rf_string_bytepos_to_charpos(&s, 28, false)
    );

    rf_string_deinit(&s);
}END_TEST

#define START_OF_UNICODE_INDEX 99
START_TEST(test_string_iterate) {
    struct RFstring s;
    uint32_t i=0, c;

    static const char *cstr = (
        "Testing to see if RFstring can correctly iterate"
        " the characters of a string even if it has unicode "
        "よる国際試合"
    );
    ck_assert(rf_string_init(&s, cstr));

    rf_string_iterate_start(&s, i, c)
    if( i >= START_OF_UNICODE_INDEX) {
        switch(i) {
        case START_OF_UNICODE_INDEX: /* unicode value of よ */
            ck_assert_int_eq(c, 12424);
            break;
        case START_OF_UNICODE_INDEX + 1: /* unicode value of る */
            ck_assert_int_eq(c, 12427);
            break;
        case START_OF_UNICODE_INDEX + 2: /* unicode value of 国 */
            ck_assert_int_eq(c, 22269);
            break;
        case START_OF_UNICODE_INDEX + 3: /* unicode value of 際 */
            ck_assert_int_eq(c, 38555);
            break;
        case START_OF_UNICODE_INDEX + 4: /* unicode value of 試 */
            ck_assert_int_eq(c, 35430);
            break;
        case START_OF_UNICODE_INDEX + 5: /* unicode value of 合 */
            ck_assert_int_eq(c, 21512);
            break;
        }
    } else {
        ck_assert_int_eq(c, cstr[i]);
    }
    rf_string_iterate_end(i)

    rf_string_deinit(&s);
}END_TEST

 /* Some times fails ... TODO: figure out why */
START_TEST(test_string_iterate_backwards) {
    struct RFstring s;
    uint32_t i=0, c;

    static const char *cstr = (
        "Testing to see if RFstring can correctly iterate"
        " the characters of a string even if it has unicode "
        "よる国際試合"
    );
    ck_assert(rf_string_init(&s, cstr));


    i = rf_string_length(&s) - 1; /* start from last character */
    rf_string_iterate_b_start(&s, i, c)
    if (i >= START_OF_UNICODE_INDEX) {
        switch(i) {
        case START_OF_UNICODE_INDEX: /* unicode value of よ */
            ck_assert_int_eq(c, 12424);
            break;
        case START_OF_UNICODE_INDEX + 1: /* unicode value of る */
            ck_assert_int_eq(c, 12427);
            break;
        case START_OF_UNICODE_INDEX + 2: /* unicode value of 国 */
            ck_assert_int_eq(c, 22269);
            break;
        case START_OF_UNICODE_INDEX + 3: /* unicode value of 際 */
            ck_assert_int_eq(c, 38555);
            break;
        case START_OF_UNICODE_INDEX + 4: /* unicode value of 試 */
            ck_assert_int_eq(c, 35430);
            break;
        case START_OF_UNICODE_INDEX + 5: /* unicode value of 合 */
            ck_assert_int_eq(c, 21512);
            break;
        }
    } else {
        ck_assert_int_eq(c, cstr[i]);
    }
    rf_string_iterate_b_end(i)

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_iterator) {
    uint32_t value;
    static const struct RFstring s = RF_STRING_STATIC_INIT(
        "Testing to see if RFstring can correctly iterate"
        " the characters of a string even if it has unicode "
        "よる国際試合"
    );
    struct RFstring_iterator it;

    rf_string_get_iter(&s, &it);

    while (rf_string_iterator_next(&it, &value)) {
        if (it.character_pos >= START_OF_UNICODE_INDEX) {
            switch(it.character_pos) {
            case START_OF_UNICODE_INDEX: /* unicode value of よ */
                ck_assert_int_eq(value, 12424);
                break;
            case START_OF_UNICODE_INDEX + 1: /* unicode value of る */
                ck_assert_int_eq(value, 12427);
                break;
            case START_OF_UNICODE_INDEX + 2: /* unicode value of 国 */
                ck_assert_int_eq(value, 22269);
                break;
            case START_OF_UNICODE_INDEX + 3: /* unicode value of 際 */
                ck_assert_int_eq(value, 38555);
                break;
            case START_OF_UNICODE_INDEX + 4: /* unicode value of 試 */
                ck_assert_int_eq(value, 35430);
                break;
            case START_OF_UNICODE_INDEX + 5: /* unicode value of 合 */
                ck_assert_int_eq(value, 21512);
                break;
            }
        } else {
            ck_assert_int_eq(value, rf_string_data(&s)[it.character_pos]);
        }
    }
}END_TEST

#undef START_OF_UNICODE_INDEX
/* --- String Misc Tests --- END --- */

/* --- Stringx Initialization Tests --- START --- */
START_TEST(test_stringx_init) {
    struct RFstringx sx;

    //expect all StringX initializations be succesfull
    ck_assert(rf_stringx_init(&sx, "Initializing a StringX"));
    ck_assert_rf_strx_eq_cstr(&sx, "Initializing a StringX");

    rf_stringx_deinit(&sx);
}END_TEST



START_TEST(test_stringx_initv) {
    struct RFstringx sx;
    ck_assert(
        rf_stringx_initv(&sx,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    ck_assert_rf_strx_eq_cstr(&sx,
                              "Printf style initialization 1337 3.142 912341");

    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_stringx_initvl) {
    struct RFstringx sx;
    struct RFstringx *sx_alloc;
    ck_assert(
        test_acceptx_vargs(&sx,
                           &sx_alloc,
                           "%s %d %.3f %u",
                           "Printf style initialization",
                           1337, 3.141592, 912341)
    );
    ck_assert_rf_strx_eq_cstr(&sx,
                              "Printf style initialization 1337 3.142 912341");
    ck_assert_rf_strx_eq_cstr(sx_alloc,
                              "Printf style initialization 1337 3.142 912341");


    rf_stringx_deinit(&sx);
    rf_stringx_destroy(sx_alloc);
}END_TEST

START_TEST(test_stringx_init_unsafe_nnt) {
    struct RFstringx s;


    ck_assert(
        rf_stringx_init_unsafe_nnt(&s,
                                   "A big string with unicode "
                                   "characters चुनाव included",
                                   52)
    );
    ck_assert_rf_strx_eq_cstr(&s, "A big string with unicode characters चुनाव");

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_init_unsafe_bnnt) {
    struct RFstringx s;


    ck_assert(
        rf_stringx_init_unsafe_bnnt(&s,
                                    "A big string with unicode "
                                    "characters चुनाव included",
                                    52,
                                    200)
    );
    ck_assert_rf_strx_eq_cstr(&s, "A big string with unicode characters चुनाव");
    ck_assert_int_eq(s.bSize, 200);

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_create_buffv) {
    struct RFstringx *s;
    struct RFstringx *s2;

    s = rf_stringx_create_buffv(
        1,
        "Having removed himself from the election campaign to "
        "concentrate on the storm, President Obama will now "
        "see at first hand just how destructive Hurricane Sandy"
        " has been. He'll travel to Atlantic City where the "
        "Republican governor, Chris Christie - normally a "
        "fierce critic - will show him scenes of widespread "
        "destruction along the Jersey Shore. They'll meet some"
        " of those who have lost homes, as well as the "
        "emergency teams who have been working around the "
        "clock since the weekend."
    );
    ck_assert_not_nullptr(s);
    ck_assert_rf_strx_eq_cstr(
        s,
        "Having removed himself from the election campaign to "
        "concentrate on the storm, President Obama will now "
        "see at first hand just how destructive Hurricane Sandy"
        " has been. He'll travel to Atlantic City where the "
        "Republican governor, Chris Christie - normally a "
        "fierce critic - will show him scenes of widespread "
        "destruction along the Jersey Shore. They'll meet some"
        " of those who have lost homes, as well as the "
        "emergency teams who have been working around the "
        "clock since the weekend."
    );

    s2 = rf_stringx_create_buffv(256, "There are %d people here", 45678);
    ck_assert_rf_strx_eq_cstr(s2, "There are 45678 people here");
    ck_assert_int_eq(256, s2->bSize);

    rf_stringx_destroy(s);
    rf_stringx_destroy(s2);
}END_TEST

START_TEST(test_stringx_init_buffv) {
    struct RFstringx s;
    struct RFstringx s2;

    ck_assert(rf_stringx_init_buffv(
                  &s,
                  1,
                  "Having removed himself from the election campaign to "
                  "concentrate on the storm, President Obama will now "
                  "see at first hand just how destructive Hurricane Sandy"
                  " has been. He'll travel to Atlantic City where the "
                  "Republican governor, Chris Christie - normally a "
                  "fierce critic - will show him scenes of widespread "
                  "destruction along the Jersey Shore. They'll meet some"
                  " of those who have lost homes, as well as the "
                  "emergency teams who have been working around the "
                  "clock since the weekend."
              )
    );
    ck_assert_rf_strx_eq_cstr(
        &s,
        "Having removed himself from the election campaign to "
        "concentrate on the storm, President Obama will now "
        "see at first hand just how destructive Hurricane Sandy"
        " has been. He'll travel to Atlantic City where the "
        "Republican governor, Chris Christie - normally a "
        "fierce critic - will show him scenes of widespread "
        "destruction along the Jersey Shore. They'll meet some"
        " of those who have lost homes, as well as the "
        "emergency teams who have been working around the "
        "clock since the weekend."
    );

    ck_assert(rf_stringx_init_buffv(
                  &s2, 256, "There are %d people here", 45678)
    );
    ck_assert_rf_strx_eq_cstr(&s2, "There are 45678 people here");
    ck_assert_int_eq(256, s2.bSize);

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_stringx_init_buff) {
    struct RFstringx s;
    struct RFstringx s2;

    ck_assert(rf_stringx_init_buff(
                  &s,
                  1,
                  "Having removed himself from the election campaign to "
                  "concentrate on the storm, President Obama will now "
                  "see at first hand just how destructive Hurricane Sandy"
                  " has been. He'll travel to Atlantic City where the "
                  "Republican governor, Chris Christie - normally a "
                  "fierce critic - will show him scenes of widespread "
                  "destruction along the Jersey Shore. They'll meet some"
                  " of those who have lost homes, as well as the "
                  "emergency teams who have been working around the "
                  "clock since the weekend."
              )
    );
    ck_assert_rf_strx_eq_cstr(
        &s,
        "Having removed himself from the election campaign to "
        "concentrate on the storm, President Obama will now "
        "see at first hand just how destructive Hurricane Sandy"
        " has been. He'll travel to Atlantic City where the "
        "Republican governor, Chris Christie - normally a "
        "fierce critic - will show him scenes of widespread "
        "destruction along the Jersey Shore. They'll meet some"
        " of those who have lost homes, as well as the "
        "emergency teams who have been working around the "
        "clock since the weekend."
    );

    ck_assert(rf_stringx_init_buff(
                  &s2, 256, "There are 3 people here")
    );
    ck_assert_rf_strx_eq_cstr(&s2, "There are 3 people here");
    ck_assert_int_eq(256, s2.bSize);

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_invalid_stringx_init) {
    struct RFstringx sx;

    ck_assert(!rf_stringx_init(&sx, NULL));
    ck_assert(!rf_stringx_initv(&sx, NULL));
    ck_assert(!rf_stringx_init_buffv(&sx, 222, NULL));
    ck_assert(!rf_stringx_create_buffv(222, NULL));

}END_TEST
/* --- Stringx Initialization Tests --- END --- */


/* --- Stringx Assignment Tests --- START --- */
START_TEST(test_stringx_assign) {
    struct RFstringx s, s2;

    ck_assert(rf_stringx_init(&s, "Nana this is an apple"));
    ck_assert(rf_stringx_init(&s2,  "Nana this is a tasty orange"));

    ck_assert(rf_stringx_assign(&s, RF_STRX2STR(&s2)));
    ck_assert_rf_strx_eq_cstr(&s, "Nana this is a tasty orange");

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_stringx_assignv) {
    struct RFstringx s1, s2, s3;
    static const struct RFstring ss = RF_STRING_STATIC_INIT("other");

    ck_assert(rf_stringx_init(&s1, "Simple rf_stringx assignment"));
    ck_assert(rf_stringx_assignv(&s1, "number: %u", 42));
    ck_assert_rf_str_eq_cstr(&s1, "number: 42");

    // test assigning to a moved string
    ck_assert(rf_stringx_init(&s2, "Moved rf_stringx assignment"));
    rf_stringx_move_forward(&s2, 6);
    ck_assert(rf_stringx_assignv(&s2, "number: %u str: \""RFS_PF"\"",
                                 42, RFS_PA(&ss)));
    ck_assert_rf_str_eq_cstr(&s2, "number: 42 str: \"other\"");
    rf_stringx_reset(&s2);
    ck_assert_rf_str_eq_cstr(&s2, "Moved number: 42 str: \"other\"");

    // test reallocation
    ck_assert(rf_stringx_init_buff(&s3, 2, ""));
    ck_assert(rf_stringx_assignv(&s3, "bigger string than original buffer"));
    ck_assert_rf_str_eq_cstr(&s3, "bigger string than original buffer");

    rf_stringx_deinit(&s1);
    rf_stringx_deinit(&s2);
    rf_stringx_deinit(&s3);
}END_TEST

START_TEST(test_stringx_assign_char) {
    struct RFstringx s, s2;

    ck_assert(rf_stringx_init(&s, "Nana this is an apple"));
    ck_assert(rf_stringx_assign_char(&s, 0xB1));
    ck_assert_rf_strx_eq_cstr(&s, "±");

    ck_assert(rf_stringx_init(&s2, "a"));
    ck_assert(rf_stringx_assign_char(&s2, 0xB1));
    ck_assert_rf_strx_eq_cstr(&s2, "±");

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_stringx_assign_unsafe_nnt) {
    struct RFstringx s;
    const char cstr[] = "Assigned string, bigger than the assignee";

    ck_assert(rf_stringx_init(&s, "Small string"));
    ck_assert(rf_stringx_assign_unsafe_nnt(&s, cstr, strlen(cstr)));
    ck_assert_rf_strx_eq_cstr(&s, "Assigned string, bigger than the assignee");

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_from_string_in) {
    struct RFstring s;
    struct RFstringx sx;

    ck_assert(
        rf_string_init(
            &s,
            "A big string with unicode "
            "characters चुनाव included"
        )
    );

    ck_assert(rf_stringx_from_string_in(&sx, &s));
    ck_assert_rf_strx_eq_cstr(
        &sx,
        "A big string with unicode "
        "characters चुनाव included"
    );

    rf_string_deinit(&s);
    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_invalid_stringx_assign) {
    struct RFstringx s;

    ck_assert(!rf_stringx_assign(&s, NULL));
    ck_assert(!rf_stringx_assign_unsafe_nnt(&s, NULL, 1));
    ck_assert(!rf_stringx_from_string_in(&s, NULL));
}END_TEST
/* --- Stringx Assignment Tests --- END --- */

/* --- Stringx Copying Tests --- START --- */

START_TEST(test_stringx_copy_in) {
    struct RFstringx s, s2;

    ck_assert(rf_stringx_init(
                  &s,
                  "Robot rock, Time of your life, Human after all")
    );

    ck_assert(rf_stringx_copy_in(&s2, &s));
    ck_assert_rf_strx_eq_cstr(
        &s2, "Robot rock, Time of your life, Human after all");

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_stringx_copy_chars) {
    struct RFstringx s, s2, s3;

    ck_assert(rf_stringx_init(
                  &s,
                  "Robot rock, Time of your life, Human after all")
    );

    ck_assert(rf_stringx_copy_chars(&s2, &s, 10));
    ck_assert_rf_strx_eq_cstr(&s2, "Robot rock");

    ck_assert(rf_stringx_copy_chars(&s3, &s, 0));
    ck_assert_rf_str_empty(&s3);

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
    rf_stringx_deinit(&s3);
}END_TEST

START_TEST(test_stringx_shallow_copies) {
    struct RFstringx s, s2;

    ck_assert(rf_stringx_init(
                  &s,
                  "Robot rock, Time of your life, Human after all")
    );

    RF_STRINGX_SHALLOW_COPY(&s2, &s);
    ck_assert_rf_str_eq_cstr(&s2, "Robot rock, Time of your life, Human after all");

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_invalid_stringx_copy) {
    struct RFstringx s;

    ck_assert(!rf_stringx_copy_in(&s, NULL));
    ck_assert(!rf_stringx_copy_chars(&s, NULL, 0));
}END_TEST
/* --- Stringx Copying Tests --- END --- */

Suite *string_core_suite_create(void)
{
    Suite *s = suite_create("String Core");

    TCase *string_init = tcase_create("String Initialization");
    tcase_add_checked_fixture(string_init,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_init, test_string_init);
    tcase_add_test(string_init, test_string_initv);
    tcase_add_test(string_init, test_string_initvl);
    tcase_add_test(string_init, test_string_init_cp);
    tcase_add_test(string_init, test_string_init_int);
    tcase_add_test(string_init, test_string_init_double);
    tcase_add_test(string_init, test_string_init_utf16);
    tcase_add_test(string_init, test_string_init_utf32);
    tcase_add_test(string_init, test_string_init_unsafe_nnt);

    TCase *string_assign = tcase_create("String Assignment");
    tcase_add_checked_fixture(string_assign,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_assign, test_string_assign);
    tcase_add_test(string_assign, test_string_assignv);
    tcase_add_test(string_assign, test_string_assign_char);
    tcase_add_test(string_assign, test_string_assign_unsafe_nnt);

    TCase *string_copy = tcase_create("String Copying");
    tcase_add_checked_fixture(string_copy,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_copy, test_string_copy_in);
    tcase_add_test(string_copy, test_string_copy_chars);

    TCase *string_misc = tcase_create("String Miscellaneous");
    tcase_add_checked_fixture(string_misc,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_misc, test_string_equal);
    tcase_add_test(string_misc, test_string_bytepos_to_codepoint);
    tcase_add_test(string_misc, test_string_bytepos_to_charpos);
    tcase_add_test(string_misc, test_string_iterate);
    tcase_add_test(string_misc, test_string_iterate_backwards);
    tcase_add_test(string_misc, test_string_iterator);




    TCase *stringx_init = tcase_create("Stringx Initialization");
    tcase_add_checked_fixture(stringx_init,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(stringx_init, test_stringx_init);
    tcase_add_test(stringx_init, test_stringx_initv);
    tcase_add_test(stringx_init, test_stringx_initvl);
    tcase_add_test(stringx_init, test_stringx_init_unsafe_nnt);
    tcase_add_test(stringx_init, test_stringx_init_unsafe_bnnt);
    tcase_add_test(stringx_init, test_stringx_create_buffv);
    tcase_add_test(stringx_init, test_stringx_init_buffv);
    tcase_add_test(stringx_init, test_stringx_init_buff);

    TCase *stringx_assign = tcase_create("Stringx Assignment");
    tcase_add_checked_fixture(stringx_assign,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(stringx_assign, test_stringx_assign);
    tcase_add_test(stringx_assign, test_stringx_assignv);
    tcase_add_test(stringx_assign, test_stringx_assign_char);
    tcase_add_test(stringx_assign, test_stringx_assign_unsafe_nnt);
    tcase_add_test(stringx_assign, test_stringx_from_string_in);

    TCase *stringx_copy = tcase_create("Stringx Copying");
    tcase_add_checked_fixture(stringx_copy,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(stringx_copy, test_stringx_copy_in);
    tcase_add_test(stringx_copy, test_stringx_copy_chars);
    tcase_add_test(stringx_copy, test_stringx_shallow_copies);


    TCase *string_invalid_args = tcase_create("string_core_invalid_args");
    tcase_add_checked_fixture(string_invalid_args,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(string_invalid_args, test_invalid_string_init);
    tcase_add_test(string_invalid_args, test_invalid_string_assign);
    tcase_add_test(string_invalid_args, test_invalid_string_copy);
    tcase_add_test(string_invalid_args, test_invalid_stringx_init);
    tcase_add_test(string_invalid_args, test_invalid_stringx_assign);
    tcase_add_test(string_invalid_args, test_invalid_stringx_copy);


    suite_add_tcase(s, string_init);
    suite_add_tcase(s, string_assign);
    suite_add_tcase(s, string_copy);
    suite_add_tcase(s, string_misc);
    suite_add_tcase(s, stringx_init);
    suite_add_tcase(s, stringx_assign);
    suite_add_tcase(s, stringx_copy);
    suite_add_tcase(s, string_invalid_args);
    return s;
}
