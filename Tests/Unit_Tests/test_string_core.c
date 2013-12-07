#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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


START_TEST(test_string_init) {
    struct RFstring s;
    ck_assert(rf_string_init(&s, "This is the test of String initialization"));
    const char* cstr = "This is the test of String initialization";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s), cstr, strlen(cstr)));

    ck_assert(!rf_string_init(&s, NULL));
    ck_assert(!rf_string_init(NULL, NULL));

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_stringx_init) {
    struct RFstringx sx;

    //expect all StringX initializations be succesfull
    ck_assert(rf_stringx_init(&sx, "Initializing a StringX"));
    const char* cstr = "Initializing a StringX";
    ck_assert_int_eq(0, memcmp(rf_string_data(&sx), cstr, strlen(cstr)));

    ck_assert(!rf_stringx_init(&sx, NULL));
    ck_assert(!rf_stringx_init(&sx, NULL));

    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_string_initv) {
    struct RFstring s;
    ck_assert(
        rf_string_initv(&s,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    const char* cstr = "Printf style initialization 1337 3.142 912341";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s), cstr, strlen(cstr)));

    ck_assert(!rf_string_initv(&s, NULL));
    ck_assert(!rf_string_initv(NULL, NULL));

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_stringx_initv) {
    struct RFstringx sx;
    ck_assert(
        rf_stringx_initv(&sx,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    const char* cstr = "Printf style initialization 1337 3.142 912341";
    ck_assert_int_eq(0, memcmp(rf_string_data(&sx), cstr, strlen(cstr)));

    ck_assert(!rf_stringx_initv(&sx, NULL));
    ck_assert(!rf_stringx_initv(NULL, NULL));

    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_string_init_cp) {
    struct RFstring s;
    ck_assert(rf_string_init_cp(&s, 0x2708));
    const char* cstr = "✈";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s), cstr, strlen(cstr)));

    ck_assert(!rf_string_initv(NULL, 0));

    rf_string_deinit(&s);
}END_TEST


START_TEST(test_string_init_int) {
    struct RFstring s1, s2, s3;

    ck_assert(rf_string_init_int(&s1, 0));
    const char* cstr_s1 = "0";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s1), cstr_s1, strlen(cstr_s1)));

    ck_assert(rf_string_init_int(&s2, 1495872));
    const char* cstr_s2 = "1495872";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s2), cstr_s2, strlen(cstr_s2)));

    ck_assert(rf_string_init_int(&s3, -909283));
    const char* cstr_s3 = "-909283";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s3), cstr_s3, strlen(cstr_s3)));

    ck_assert(!rf_string_initv(NULL, 0));

    rf_string_deinit(&s3);
    rf_string_deinit(&s2);
    rf_string_deinit(&s1);
}END_TEST

START_TEST(test_string_init_double) {
    struct RFstring s1, s2, s3;

    ck_assert(rf_string_init_double(&s1, 0));
    const char* cstr_s1 = "0.0";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s1), cstr_s1, strlen(cstr_s1)));

    ck_assert(rf_string_init_double(&s2, 54.12452));
    const char* cstr_s2 = "54.12452";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s2), cstr_s2, strlen(cstr_s2)));

    ck_assert(rf_string_init_double(&s3, -0.00192));
    const char* cstr_s3 = "-0.00192";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s3), cstr_s3, strlen(cstr_s3)));

    ck_assert(!rf_string_initv(NULL, 0));

    rf_string_deinit(&s3);
    rf_string_deinit(&s2);
    rf_string_deinit(&s1);
}END_TEST


START_TEST(test_string_init_utf16) {
    struct RFstring s1, s2;

    const uint16_t utf16_buffer[] = {0xD834, 0xDD1E, 0x6771};
    unsigned int utf16b_len = 3 * sizeof(uint16_t);

    ck_assert(rf_string_init_utf16(&s1, utf16_buffer, utf16b_len));
    /* G-clef music symbol + Higashi*/
    const char* cstr_s1 = "\xf0\x9d\x84\x9e東"; 
    ck_assert_int_eq(0, memcmp(rf_string_data(&s1), cstr_s1, strlen(cstr_s1)));

    ck_assert(!rf_string_init_utf16(NULL, 0, 0));
    ck_assert(!rf_string_init_utf16(&s2, 0, 0));
    
    rf_string_deinit(&s1);
}END_TEST


Suite *string_core_suite_create(void)
{
    Suite *s = suite_create("String Core");

    TCase *tc_string = tcase_create("String basic");
    tcase_add_checked_fixture(tc_string, setup, teardown);
    tcase_add_test(tc_string, test_string_init);
    tcase_add_test(tc_string, test_stringx_init);
    tcase_add_test(tc_string, test_string_initv);
    tcase_add_test(tc_string, test_stringx_initv);
    tcase_add_test(tc_string, test_string_init_cp);
    tcase_add_test(tc_string, test_string_init_int);
    tcase_add_test(tc_string, test_string_init_double);
    tcase_add_test(tc_string, test_string_init_utf16);
    suite_add_tcase(s, tc_string);
    return s;
}
