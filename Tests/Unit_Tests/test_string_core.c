#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"

#include <refu.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
void setup()
{
    rf_init("refuclib.log", 0, LOG_DEBUG);
}

void teardown()
{
    rf_deinit();
}


/* --- String Initialization Tests --- START --- */

START_TEST(test_string_init) {
    struct RFstring s;
    ck_assert(rf_string_init(&s, "This is the test of String initialization"));
    ck_assert_rf_str_eq_cstr(&s, "This is the test of String initialization");

    ck_assert(!rf_string_init(&s, NULL));
    ck_assert(!rf_string_init(NULL, NULL));

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_initv) {
    struct RFstring s;
    ck_assert(
        rf_string_initv(&s,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    ck_assert_rf_str_eq_cstr(&s,
                             "Printf style initialization 1337 3.142 912341");
    ck_assert(!rf_string_initv(&s, NULL));
    ck_assert(!rf_string_initv(NULL, NULL));

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_init_cp) {
    struct RFstring s;
    ck_assert(rf_string_init_cp(&s, 0x2708));
    ck_assert_rf_str_eq_cstr(&s, "✈");
    ck_assert(!rf_string_initv(NULL, 0));

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

    ck_assert(!rf_string_initv(NULL, 0));

    rf_string_deinit(&s3);
    rf_string_deinit(&s2);
    rf_string_deinit(&s1);
}END_TEST

START_TEST(test_string_init_double) {
    struct RFstring s1, s2, s3;

    ck_assert(rf_string_init_double(&s1, 0));
    ck_assert_rf_str_eq_cstr(&s1, "0.0");

    ck_assert(rf_string_init_double(&s2, 54.12452));
    ck_assert_rf_str_eq_cstr(&s2, "54.12452");

    ck_assert(rf_string_init_double(&s3, -0.00192));
    ck_assert_rf_str_eq_cstr(&s3, "-0.00192");

    ck_assert(!rf_string_initv(NULL, 0));

    rf_string_deinit(&s3);
    rf_string_deinit(&s2);
    rf_string_deinit(&s1);
}END_TEST


START_TEST(test_string_init_utf16) {
    struct RFstring s1, s2;

    const uint16_t utf16_buffer[] = {0xD834, 0xDD1E, 0x6771};
    unsigned int utf16b_len = 3 * sizeof(uint16_t);

    /* G-clef music symbol + Higashi*/
    ck_assert(rf_string_init_utf16(&s1, utf16_buffer, utf16b_len));
    ck_assert_rf_str_eq_cstr(&s1, "\xf0\x9d\x84\x9e東");

    ck_assert(!rf_string_init_utf16(NULL, 0, 0));
    ck_assert(!rf_string_init_utf16(&s2, 0, 0));
    
    rf_string_deinit(&s1);
}END_TEST

START_TEST(test_string_init_utf32) {
    struct RFstring s1, s2;

    const uint32_t utf32_buffer[] = {0x6771,0x4EAC};
    unsigned int utf32b_len = 2;

    ck_assert(rf_string_init_utf32(&s1, utf32_buffer, utf32b_len));
    ck_assert_rf_str_eq_cstr(&s1, "東京");

    ck_assert(!rf_string_init_utf32(NULL, 0, 0));
    ck_assert(!rf_string_init_utf32(&s2, 0, 0));
    
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
/* --- String Initialization Tests --- END --- */


/* --- Stringx Initialization Tests --- START --- */
START_TEST(test_stringx_init) {
    struct RFstringx sx;

    //expect all StringX initializations be succesfull
    ck_assert(rf_stringx_init(&sx, "Initializing a StringX"));
    ck_assert_rf_strx_eq_cstr(&sx, "Initializing a StringX");

    ck_assert(!rf_stringx_init(&sx, NULL));
    ck_assert(!rf_stringx_init(&sx, NULL));

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

    ck_assert(!rf_stringx_initv(&sx, NULL));
    ck_assert(!rf_stringx_initv(NULL, NULL));

    rf_stringx_deinit(&sx);
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
/* --- Stringx Initialization Tests --- END --- */


Suite *string_core_suite_create(void)
{
    Suite *s = suite_create("String Core");

    TCase *string_init = tcase_create("String Initialization");
    tcase_add_checked_fixture(string_init, setup, teardown);
    tcase_add_test(string_init, test_string_init);
    tcase_add_test(string_init, test_string_initv);
    tcase_add_test(string_init, test_string_init_cp);
    tcase_add_test(string_init, test_string_init_int);
    tcase_add_test(string_init, test_string_init_double);
    tcase_add_test(string_init, test_string_init_utf16);
    tcase_add_test(string_init, test_string_init_utf32);
    tcase_add_test(string_init, test_string_init_unsafe_nnt);

    TCase *stringx_init = tcase_create("Stringx basic");
    tcase_add_checked_fixture(stringx_init, setup, teardown);
    tcase_add_test(stringx_init, test_stringx_init);    
    tcase_add_test(stringx_init, test_stringx_initv);
    tcase_add_test(stringx_init, test_stringx_init_unsafe_nnt);
    tcase_add_test(stringx_init, test_stringx_init_unsafe_bnnt);


    suite_add_tcase(s, string_init);
    suite_add_tcase(s, stringx_init);
    return s;
}
