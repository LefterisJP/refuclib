#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"
#include <refu.h>
#include <String/rf_str_core.h>
#include <String/rf_str_common.h>

static bool test_rf_strings_buffer_fillfmt(const char *fmt,
                                           unsigned int *size,
                                           char **buffPtr,
                                           ...)
{
    bool rc;
    va_list args;
    va_start(args, buffPtr);
    rc = rf_strings_buffer_fillfmt(fmt, size, buffPtr, args);
    va_end(args);
    return rc;
}

START_TEST (test_RFS_unsafe) {
    RFS_push();
    struct RFstring *s1 = RFS_UNSAFE("am a simple string");
    ck_assert_rf_str_eq_cstr(s1, "am a simple string");
    RFS_push();
    struct RFstring *s2 = RFS_UNSAFE("foo "RF_STR_PF_FMT" %d", RF_STR_PF_ARG(s1), 55);
    ck_assert_rf_str_eq_cstr(s1, "am a simple string");
    ck_assert_rf_str_eq_cstr(s2, "foo am a simple string 55");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_RFS) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_push();
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_push();
    RFS(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_string_buffer_fillfmt) {
    unsigned int size;
    RFS_push();
    char *buff1;
    char *buff2;
    ck_assert(test_rf_strings_buffer_fillfmt(
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz%d%s",
                  &size,
                  &buff1,
                  42, "ABCD"));
    ck_assert_uint_eq(size, 58);
    ck_assert_nnt_str_eq_cstr(
        buff1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz42ABCD");
    RFS_push();
    ck_assert(test_rf_strings_buffer_fillfmt(
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
                  &size,
                  &buff2));
    ck_assert_uint_eq(size, 52);
    ck_assert_nnt_str_eq_cstr(
        buff2,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
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
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_push();
    RFS(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_RFS_realloc_at_first_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_push();
    RFS(&s1,
               "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
               "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_push();
    RFS(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_RFS_realloc_vararg_at_second_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_push();
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz%d%d", 23, 24);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz2324");
    RFS_push();
    RFS(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%s%d", "eleos", 124);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz2324");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZeleos124");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_RFS_realloc_vararg_at_first_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_push();
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy%d%d", 23, 24);
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy2324");
    RFS_push();
    RFS(&s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%s%d", "eleos", 124);
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy2324");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZeleos124");

    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_string_buffer_fillfmt_realloc) {
    unsigned int size;
    RFS_push();
    char *buff1;
    char *buff2;
    ck_assert(test_rf_strings_buffer_fillfmt(
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz%d%s",
                  &size,
                  &buff1,
                  42, "ABCD"));
    ck_assert_uint_eq(size, 58);
    ck_assert_nnt_str_eq_cstr(
        buff1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz42ABCD");
    RFS_push();
    ck_assert(test_rf_strings_buffer_fillfmt(
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz%d%s",
                  &size,
                  &buff2,
                  1337, "CELINA"));
    ck_assert_uint_eq(size, 88);
    ck_assert_nnt_str_eq_cstr(
        buff2,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz1337CELINA");
    ck_assert_nnt_str_eq_cstr(
        buff1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz42ABCD");
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
    tcase_add_test(tc1, test_RFS_unsafe);
    tcase_add_test(tc1, test_RFS);
    tcase_add_test(tc1, test_string_buffer_fillfmt);

    TCase *tc2 = tcase_create("string_buffers_realloc");
    tcase_add_checked_fixture(tc2,
                              setup_realloc_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc2, test_RFS_realloc_at_second_use);
    tcase_add_test(tc2, test_RFS_realloc_at_first_use);
    tcase_add_test(tc2, test_RFS_realloc_vararg_at_second_use);
    tcase_add_test(tc2, test_RFS_realloc_vararg_at_first_use);
    tcase_add_test(tc2, test_string_buffer_fillfmt_realloc);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    return s;
}
