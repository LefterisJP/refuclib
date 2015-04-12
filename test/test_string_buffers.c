#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"
#include <refu.h>
#include <String/rf_str_core.h>
#include <String/rf_str_common.h>

START_TEST (test_RFS) {
    RFS_push();
    struct RFstring *s1 = RFS_("am a simple string");
    ck_assert_rf_str_eq_cstr(s1, "am a simple string");
    RFS_push();
    struct RFstring *s2 = RFS_("foo "RF_STR_PF_FMT" %d", RF_STR_PF_ARG(s1), 55);
    ck_assert_rf_str_eq_cstr(s1, "am a simple string");
    ck_assert_rf_str_eq_cstr(s2, "foo am a simple string 55");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_RFS_assign) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_push();
    RFS_assign(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_push();
    RFS_assign(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");

    RFS_pop();
    RFS_pop();
} END_TEST

void setup_realloc_tests()
{
    rf_init(LOG_TARGET_STDOUT, NULL, LOG_DEBUG, 128);
}

void teardown_realloc_tests()
{
    rf_deinit();
}

START_TEST (test_RFS_realloc_at_second_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_push();
    RFS_assign(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_push();
    RFS_assign(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");

    RFS_pop();
    RFS_pop();
} END_TEST

Suite *string_buffers_suite_create(void)
{
    Suite *s = suite_create("string_buffers");

    TCase *tc1 = tcase_create("string_buffers_simple");
    tcase_add_checked_fixture(tc1,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(tc1, test_RFS);
    tcase_add_test(tc1, test_RFS_assign);

    TCase *tc2 = tcase_create("string_buffers_realloc");
    tcase_add_checked_fixture(tc2,
                              setup_realloc_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc2, test_RFS_realloc_at_second_use);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    return s;
}
