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

static enum RFS_rc test_rf_strings_buffer_fillfmt_detect_realloc(const char *fmt,
                                                                 unsigned int *size,
                                                                 char **buffPtr,
                                                                 ...)
{
    enum RFS_rc rc;
    va_list args;
    va_start(args, buffPtr);
    rc = rf_strings_buffer_fillfmt_detect_realloc(fmt, size, buffPtr, args);
    va_end(args);
    return rc;
}

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

START_TEST (test_RFS_same_ptr) {
    struct RFstring *s1;
    RFS_push();
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS(&s1, RF_STR_PF_FMT "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
        RF_STR_PF_ARG(s1));
    ck_assert_rf_str_eq_cstr(s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
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
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%s",
                  &size,
                  &buff2,
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"));
    ck_assert_uint_eq(size, 104);
    ck_assert_nnt_str_eq_cstr(
        buff2,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_string_buffer_fillfmt_detect_realloc_at_no_realloc) {
    unsigned int size;
    RFS_push();
    char *buff1;
    char *buff2;
    ck_assert(RFS_SUCCESS == test_rf_strings_buffer_fillfmt_detect_realloc(
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz%d%s",
                  &size,
                  &buff1,
                  42, "ABCD"));
    ck_assert_uint_eq(size, 58);
    ck_assert_nnt_str_eq_cstr(
        buff1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz42ABCD");
    RFS_push();
    ck_assert(RFS_SUCCESS == test_rf_strings_buffer_fillfmt_detect_realloc(
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%s",
                  &size,
                  &buff2,
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"));
    ck_assert_uint_eq(size, 104);
    ck_assert_nnt_str_eq_cstr(
        buff2,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_pop();
    RFS_pop();
} END_TEST

void setup_realloc_tests()
{
    rf_init(LOG_TARGET_STDOUT, NULL, LOG_DEBUG, 128, RF_DEFAULT_TS_WORKBUFF_SIZE);
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

START_TEST (test_string_buffer_fillfmt_detect_realloc_with_realloc) {
    unsigned int size;
    RFS_push();
    char *buff1;
    char *buff2;
    ck_assert(RFS_SUCCESS == test_rf_strings_buffer_fillfmt_detect_realloc(
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz%d%s",
                  &size,
                  &buff1,
                  42, "ABCD"));
    ck_assert_uint_eq(size, 58);
    ck_assert_nnt_str_eq_cstr(
        buff1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz42ABCD");
    RFS_push();
    ck_assert(RFS_REALLOC == test_rf_strings_buffer_fillfmt_detect_realloc(
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%d%s",
                  &size,
                  &buff2,
                  1337,
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
              ));
    ck_assert(RFS_SUCCESS == test_rf_strings_buffer_fillfmt_detect_realloc(
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%d%s",
                  &size,
                  &buff2,
                  1337,
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
              ));
    ck_assert_uint_eq(size, 108);
    ck_assert_nnt_str_eq_cstr(
        buff2,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ1337"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_pop();
    RFS_pop();
} END_TEST

START_TEST (test_RFS_same_ptr_realloc) {
    struct RFstring *s1;
    RFS_push();
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS(&s1,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(
        s1,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    RFS_pop();
} END_TEST

START_TEST (test_RFS_same_ptr_realloc_vararg) {
    struct RFstring *s1;
    RFS_push();
    RFS(&s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS(&s1, RF_STR_PF_FMT "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
        RF_STR_PF_ARG(s1));
    ck_assert_rf_str_eq_cstr(s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    RFS_pop();
} END_TEST

static struct RFstring *get_str(struct RFstring **ret, int choice)
{
    if (choice == 1) {
        RFS(ret, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    } else {
        RFS(ret, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%d",
            1337);
    }
    return *ret;
}

static struct RFstring *get_str_with_RFC_recursive(struct RFstring **ret, struct RFstring *s)
{
    if (s == *ret) {
    RFS(ret, RF_STR_PF_FMT"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
        RF_STR_PF_ARG(*ret));
    } else {
        RFS(ret, RF_STR_PF_FMT"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
            RF_STR_PF_ARG(s));
    }
    return *ret;
}

START_TEST (test_RFS_return_from_function_with_realloc) {
    RFS_push();
    struct RFstring *s1;
    struct RFstring *s2;
    s1 = get_str(&s1, 1);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    s2 = get_str(&s2, 2);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2,
                             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ1337");
    RFS_pop();
} END_TEST

// note: This test shows one major limitation of RFS().
// If you try to use it inside another function and call it recursively
// with the same string being assigned to and also read from in the varargs
// you need to explicitly check for that and call the macro accordingly as seen
// at get_str_with_RFC_recursive()
START_TEST (test_RFS_return_from_function_with_realloc_same_ptr) {
    RFS_push();
    struct RFstring *s1;
    s1 = get_str(&s1, 1);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    s1 = get_str_with_RFC_recursive(&s1, s1);
    ck_assert_rf_str_eq_cstr(s1,
                             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    RFS_pop();
} END_TEST

static struct RFstring *get_str_rec(struct RFstring **ret, int num, bool first)
{
    if (first) {
        RFS(ret,
            "%dabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", num);
    } else {
        RFS(ret,
            RF_STR_PF_FMT "%dabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
            RF_STR_PF_ARG(*ret), num);
    }
    if (num > 0) {
        get_str_rec(ret, num - 1, false);
    }

    return *ret;
}

// test RFS calls in recursive functions that add local RFS initialized strings
// to the returned string
static struct RFstring *get_str_rec_with_local(struct RFstring **ret, int num)
{
    if (num > 0) {
        get_str_rec_with_local(ret, num - 1);
    }
    struct RFstring *s;
    RFS(&s, "pre%d", num);
    if (num == 0) {
        RFS(ret,
            RF_STR_PF_FMT"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
            RF_STR_PF_ARG(s));
    } else {
        RFS(ret,
            RF_STR_PF_FMT RF_STR_PF_FMT "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
            RF_STR_PF_ARG(*ret), RF_STR_PF_ARG(s));
    }

    return *ret;
}

static void test_RFS_in_recursive_functions_with_local()
{
    RFS_push();
    struct RFstring *s1;
    s1 = get_str_rec_with_local(&s1, 0);
    ck_assert_rf_str_eq_cstr(
        s1,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_pop();
    RFS_push();
    struct RFstring *s2;
    s2 = get_str_rec_with_local(&s2, 1);
    ck_assert_rf_str_eq_cstr(
        s2,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    RFS_pop();
    RFS_push();
    struct RFstring *s3;
    s3 = get_str_rec_with_local(&s3, 2);
    ck_assert_rf_str_eq_cstr(
        s3,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre2abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    RFS_pop();
    RFS_push();
    struct RFstring *s4;
    s4 = get_str_rec_with_local(&s4, 3);
    ck_assert_rf_str_eq_cstr(
        s4,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre2abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre3abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    RFS_pop();
}

static void test_RFS_in_recursive_functions()
{
    RFS_push();
    struct RFstring *s1;
    s1 = get_str_rec(&s1, 0, true);
    ck_assert_rf_str_eq_cstr(
        s1,
        "0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_pop();
    RFS_push();
    struct RFstring *s2;
    s2 = get_str_rec(&s2, 1, true);
    ck_assert_rf_str_eq_cstr(
        s2,
        "1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_pop();
}

START_TEST (test_RFS_in_recursive_functions_no_realloc) {
    test_RFS_in_recursive_functions();
} END_TEST

START_TEST (test_RFS_in_recursive_functions_with_realloc) {
    test_RFS_in_recursive_functions();
} END_TEST

START_TEST (test_RFS_in_recursive_functions_with_local_no_realloc) {
    test_RFS_in_recursive_functions_with_local();
} END_TEST

START_TEST (test_RFS_in_recursive_functions_with_local_with_realloc) {
    test_RFS_in_recursive_functions_with_local();
} END_TEST



Suite *string_buffers_suite_create(void)
{
    Suite *s = suite_create("string_buffers");

    TCase *tc1 = tcase_create("string_buffers_simple");
    tcase_add_checked_fixture(tc1,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(tc1, test_RFS);
    tcase_add_test(tc1, test_RFS_same_ptr);
    tcase_add_test(tc1, test_string_buffer_fillfmt);
    tcase_add_test(tc1, test_string_buffer_fillfmt_detect_realloc_at_no_realloc);

    TCase *tc2 = tcase_create("string_buffers_realloc");
    tcase_add_checked_fixture(tc2,
                              setup_realloc_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc2, test_RFS_realloc_at_second_use);
    tcase_add_test(tc2, test_RFS_realloc_at_first_use);
    tcase_add_test(tc2, test_RFS_realloc_vararg_at_second_use);
    tcase_add_test(tc2, test_RFS_realloc_vararg_at_first_use);
    tcase_add_test(tc2, test_string_buffer_fillfmt_realloc);
    tcase_add_test(tc2, test_RFS_same_ptr_realloc);
    tcase_add_test(tc2, test_RFS_same_ptr_realloc_vararg);
    tcase_add_test(tc2, test_string_buffer_fillfmt_detect_realloc_with_realloc);

    TCase *tc3 = tcase_create("string_buffers_misc_at_realloc");
    tcase_add_checked_fixture(tc3,
                              setup_realloc_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc3, test_RFS_return_from_function_with_realloc);
    tcase_add_test(tc3, test_RFS_return_from_function_with_realloc_same_ptr);
    tcase_add_test(tc3, test_RFS_in_recursive_functions_with_realloc);
    tcase_add_test(tc3, test_RFS_in_recursive_functions_with_local_with_realloc);

    TCase *tc4 = tcase_create("string_buffers_misc_no_realloc");
    tcase_add_checked_fixture(tc4,
                              setup_generic_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc4, test_RFS_in_recursive_functions_no_realloc);
    tcase_add_test(tc4, test_RFS_in_recursive_functions_with_local_no_realloc);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    suite_add_tcase(s, tc3);
    suite_add_tcase(s, tc4);
    return s;
}
