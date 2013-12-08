#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "test_string_helpers.h"


#include <refu.h>
#include <String/rf_str_conversion.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>


/* --- String Encoding Tests --- START --- */

START_TEST(test_string_to_utf16) {
    struct RFstring s;
    uint32_t length;
    uint16_t *buff;
    //first create the string to test. Japanese(Adachiku) + MusicalSymbol(G clef)
    ck_assert(rf_string_init(&s,"足立区\xf0\x9d\x84\x9e"));
    buff = rf_string_to_utf16(&s, &length);
    ck_assert_not_nullptr(buff);
    
    ck_assert_int_eq(length, 5);
    ck_assert_msg(buff[0] == 0x8DB3, "First converted word does not match");
    ck_assert_msg(buff[1] == 0x7ACB, "Second converted word does not match");
    ck_assert_msg(buff[2] == 0x533A, "Third converted word does not match");
    ck_assert_msg(buff[3] == 0xD834, "Fourth converted word does not match");
    ck_assert_msg(buff[4] == 0xDD1E, "Fifth converted word does not match");
    
    ck_assert(!rf_string_to_utf16(&s, NULL));

    free(buff);
    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_to_utf32) {
    struct RFstring s;
    uint32_t length;
    uint32_t *buff;
    //first create the string to test. Japanese(Adachiku) + MusicalSymbol(G clef)
    ck_assert(rf_string_init(&s,"足立区\xf0\x9d\x84\x9e"));
    buff = rf_string_to_utf32(&s, &length);
    ck_assert_not_nullptr(buff);
    
    ck_assert_int_eq(length, 4);
    ck_assert_msg(buff[0] == 0x8DB3, "First converted word does not match");
    ck_assert_msg(buff[1] == 0x7ACB, "Second converted word does not match");
    ck_assert_msg(buff[2] == 0x533A, "Third converted word does not match");
    ck_assert_msg(buff[3] == 0x1D11E, "Fourth converted word does not match");
    
    ck_assert(!rf_string_to_utf32(&s, NULL));

    free(buff);
    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_to_cstr) {
    struct RFstring s;
    char *cstr;

    ck_assert(rf_string_init(&s, "This is a normal RFstring"));
    cstr = rf_string_cstr(&s);
    ck_assert_not_nullptr(cstr);
    ck_assert_str_eq(cstr, "This is a normal RFstring");

    free(cstr);
    rf_string_deinit(&s);
}END_TEST

/* --- String Encoding Tests --- END --- */

/* --- String Other Conversion Tests --- START --- */
START_TEST(test_string_to_int) {
    struct RFstringx sx;
    struct RFstring s, s2;
    struct RFstring ns, ns2;
    int32_t num;
    ck_assert(rf_stringx_init(&sx, "45687"));
    ck_assert(rf_string_init(&s, "-123442"));
    ck_assert(rf_string_init(&s2, "0"));

    ck_assert(rf_string_to_int(&sx, &num));
    ck_assert_int_eq(num, 45687);

    ck_assert(rf_string_to_int(&s, &num));
    ck_assert_int_eq(num, -123442);

    ck_assert(rf_string_to_int(&s2, &num));
    ck_assert_int_eq(num, 0);

    ck_assert(!rf_string_to_int(&s2, NULL));

    ck_assert(rf_string_init(&ns, "Not a number"));
    ck_assert(!rf_string_to_int(&ns, &num));

    ck_assert(rf_string_init(&ns2, ""));
    ck_assert(!rf_string_to_int(&ns2, &num));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&ns);
    rf_string_deinit(&ns2);
    rf_stringx_deinit(&sx);
}END_TEST
/* --- String Other Conversion Tests --- END --- */

Suite *string_conversion_suite_create(void)
{
    Suite *s = suite_create("String Conversion");

    TCase *string_encodings_conversions = tcase_create(
        "String Encodings Conversion"
    );
    tcase_add_checked_fixture(string_encodings_conversions,
                                  setup_string_tests,
                                  teardown_string_tests);
    tcase_add_test(string_encodings_conversions, test_string_to_utf16);
    tcase_add_test(string_encodings_conversions, test_string_to_utf32);
    tcase_add_test(string_encodings_conversions, test_string_to_cstr);


    TCase *string_other_conversions = tcase_create("String Other Conversions");
    tcase_add_checked_fixture(string_other_conversions,
                                  setup_string_tests,
                                  teardown_string_tests);
    tcase_add_test(string_other_conversions, test_string_to_int);


    suite_add_tcase(s, string_encodings_conversions);
    suite_add_tcase(s, string_other_conversions);
    return s;
}
