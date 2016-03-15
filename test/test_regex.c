#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <rflib/string/rf_str_core.h>
#include <rflib/string/regex.h>

START_TEST(test_re_match_single1) {
    struct RFstring pattern = RF_STRING_STATIC_INIT("[a-z]+");
    struct RFstring subject = RF_STRING_STATIC_INIT("WE should 123");
    struct RFre *re = rfre_compile(&pattern);
    ck_assert(re);
    struct RFre_match mdata;
    ck_assert(rfre_match_single(re, &subject, &mdata));
    ck_assert(mdata.captures_num == 0);
    ck_assert(darray_size(mdata.matches) == 1);
    ck_assert_rf_str_eq_cstr(&darray_item(mdata.matches, 0), "should");

    rfre_match_deinit(&mdata);
    rfre_destroy(re);
} END_TEST

START_TEST(test_re_match_single2) {
    struct RFstring pattern = RF_STRING_STATIC_INIT("([a-z]+).*?([1-9]+)");
    struct RFstring subject = RF_STRING_STATIC_INIT("foobar otherstuff 4235 and more stuff");
    struct RFre *re = rfre_compile(&pattern);
    ck_assert(re);
    struct RFre_match mdata;
    ck_assert(rfre_match_single(re, &subject, &mdata));
    ck_assert(mdata.captures_num == 2);
    ck_assert(darray_size(mdata.matches) == 3);
    ck_assert_rf_str_eq_cstr(&darray_item(mdata.matches, 0), "foobar otherstuff 4235");
    ck_assert_rf_str_eq_cstr(&darray_item(mdata.matches, 1), "foobar");
    ck_assert_rf_str_eq_cstr(&darray_item(mdata.matches, 2), "4235");

    rfre_match_deinit(&mdata);
    rfre_destroy(re);
} END_TEST


Suite *regex_suite_create(void)
{
    Suite *s = suite_create("Regular Expressions");

    TCase *tc1 = tcase_create("simple_match");
    tcase_add_checked_fixture(tc1,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(tc1, test_re_match_single1);
    tcase_add_test(tc1, test_re_match_single2);

    suite_add_tcase(s, tc1);

    return s;
}
