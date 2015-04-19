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

START_TEST (test_RFS) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_PUSH();
    s1 = RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_PUSH();
    s2 = RFS("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");

    RFS_POP();
    RFS_POP();
} END_TEST

START_TEST (test_RFS_same_ptr) {
    struct RFstring *s1;
    RFS_PUSH();
    s1 = RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    s1 = RFS(RF_STR_PF_FMT "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
             RF_STR_PF_ARG(s1));
    ck_assert_rf_str_eq_cstr(s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    RFS_POP();
} END_TEST

START_TEST (test_string_buffer_fillfmt) {
    unsigned int size;
    RFS_PUSH();
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
    RFS_PUSH();
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
    RFS_POP();
    RFS_POP();
} END_TEST

void setup_realloc_tests()
{
    rf_init(LOG_TARGET_STDOUT, NULL, LOG_DEBUG,
            128,
            RF_DEFAULT_TS_SBUFF_INITIAL_SIZE);
}

void teardown_realloc_tests()
{
    rf_deinit();
}

START_TEST (test_RFS_realloc_at_second_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_PUSH();
    s1 = RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_PUSH();
    s2 = RFS("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    RFS_POP();
    RFS_POP();
} END_TEST

START_TEST (test_RFS_realloc_at_first_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_PUSH();
    s1 = RFS(
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_PUSH();
    s2 = RFS("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    RFS_POP();
    RFS_POP();
} END_TEST

START_TEST (test_RFS_realloc_vararg_at_second_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_PUSH();
    s1 = RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz%d%d", 23, 24);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz2324");
    RFS_PUSH();
    s2 = RFS("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%s%d", "eleos", 124);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz2324");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZeleos124");
    RFS_POP();
    RFS_POP();
} END_TEST

START_TEST (test_RFS_realloc_vararg_at_first_use) {
    struct RFstring *s1;
    struct RFstring *s2;
    RFS_PUSH();
    s1 = RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy%d%d", 23, 24);
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy2324");
    RFS_PUSH();
    s2 = RFS("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%s%d", "eleos", 124);
    ck_assert_rf_str_eq_cstr(
        s1,
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy2324");
    ck_assert_rf_str_eq_cstr(s2, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZeleos124");
    RFS_POP();
    RFS_POP();
} END_TEST

START_TEST (test_string_buffer_fillfmt_realloc) {
    unsigned int size;
    RFS_PUSH();
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
    RFS_PUSH();
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
    RFS_POP();
    RFS_POP();
} END_TEST

static struct RFstring *get_str(int choice)
{
    if (choice == 1) {
        return RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    } else {
        return RFS("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ%d",
                   1337);
    }
}

START_TEST (test_RFS_return_from_function_with_realloc) {
    RFS_PUSH();
    struct RFstring *s1;
    struct RFstring *s2;
    s1 = get_str(1);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    s2 = get_str(2);
    ck_assert_rf_str_eq_cstr(s1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    ck_assert_rf_str_eq_cstr(s2,
                             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ1337");
    RFS_POP();
} END_TEST

static struct RFstring *get_str_rec(struct RFstring *s, int num, bool first)
{
    if (first) {
        s = RFS("%dabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", num);
    } else {
        s = RFS(RF_STR_PF_FMT "%dabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
                RF_STR_PF_ARG(s), num);
    }
    if (num > 0) {
        s = get_str_rec(s, num - 1, false);
    }

    return s;
}

// test RFS calls in recursive functions that add local RFS initialized strings
// to the returned string
static struct RFstring *get_str_rec_with_local(struct RFstring *ret, int num)
{
    if (num > 0) {
        ret = get_str_rec_with_local(ret, num - 1);
    }
    struct RFstring *s;
    s = RFS("pre%d", num);
    if (num == 0) {
        ret = RFS(RF_STR_PF_FMT"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
                  RF_STR_PF_ARG(s));
    } else {
        ret = RFS(RF_STR_PF_FMT
                  RF_STR_PF_FMT
                  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
                  RF_STR_PF_ARG(ret), RF_STR_PF_ARG(s));
    }

    return ret;
}

static void test_RFS_in_recursive_functions_with_local()
{
    RFS_PUSH();
    struct RFstring *s1;
    s1 = get_str_rec_with_local(NULL, 0);
    ck_assert_rf_str_eq_cstr(
        s1,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_POP();
    RFS_PUSH();
    struct RFstring *s2;
    s2 = get_str_rec_with_local(NULL, 1);
    ck_assert_rf_str_eq_cstr(
        s2,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    RFS_POP();
    RFS_PUSH();
    struct RFstring *s3;
    s3 = get_str_rec_with_local(NULL, 2);
    ck_assert_rf_str_eq_cstr(
        s3,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre2abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    RFS_POP();
    RFS_PUSH();
    struct RFstring *s4;
    s4 = get_str_rec_with_local(NULL, 3);
    ck_assert_rf_str_eq_cstr(
        s4,
        "pre0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre2abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "pre3abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    RFS_POP();
}

static void test_RFS_in_recursive_functions()
{
    RFS_PUSH();
    struct RFstring *s1;
    s1 = get_str_rec(NULL, 0, true);
    ck_assert_rf_str_eq_cstr(
        s1,
        "0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_POP();
    RFS_PUSH();
    struct RFstring *s2;
    s2 = get_str_rec(NULL, 1, true);
    ck_assert_rf_str_eq_cstr(
        s2,
        "1abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "0abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    RFS_POP();
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

START_TEST (test_RFS_NT) {
    struct RFstring *s1 = RFS_NT("a string");
    struct RFstring *s2 = RFS_NT("a string %s %d", "with", 123);
    ck_assert_rf_str_eq_cstr(s1, "a string");
    ck_assert(rf_string_data(s1)[rf_string_length_bytes(s1)] == '\0');
    ck_assert_rf_str_eq_cstr(s2, "a string with 123");
    ck_assert(rf_string_data(s2)[rf_string_length_bytes(s2)] == '\0');
    struct RFstring *s3 = RFS_NT_OR_DIE("a string");
    struct RFstring *s4 = RFS_NT("a string %s %d", "with", 123);
    ck_assert_rf_str_eq_cstr(s3, "a string");
    ck_assert(rf_string_data(s3)[rf_string_length_bytes(s3)] == '\0');
    ck_assert_rf_str_eq_cstr(s4, "a string with 123");
    ck_assert(rf_string_data(s4)[rf_string_length_bytes(s4)] == '\0');

    // test that subsequent allocations don't overwite the null terminating
    // character
    ck_assert(rf_string_data(s1)[rf_string_length_bytes(s1)] == '\0');
    ck_assert(rf_string_data(s2)[rf_string_length_bytes(s2)] == '\0');
    ck_assert(rf_string_data(s3)[rf_string_length_bytes(s3)] == '\0');
    ck_assert(rf_string_data(s4)[rf_string_length_bytes(s4)] == '\0');
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

    TCase *tc2 = tcase_create("string_buffers_realloc");
    tcase_add_checked_fixture(tc2,
                              setup_realloc_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc2, test_RFS_realloc_at_second_use);
    tcase_add_test(tc2, test_RFS_realloc_at_first_use);
    tcase_add_test(tc2, test_RFS_realloc_vararg_at_second_use);
    tcase_add_test(tc2, test_RFS_realloc_vararg_at_first_use);
    tcase_add_test(tc2, test_string_buffer_fillfmt_realloc);

    TCase *tc3 = tcase_create("string_buffers_misc_at_realloc");
    tcase_add_checked_fixture(tc3,
                              setup_realloc_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc3, test_RFS_return_from_function_with_realloc);
    tcase_add_test(tc3, test_RFS_in_recursive_functions_with_realloc);
    tcase_add_test(tc3, test_RFS_in_recursive_functions_with_local_with_realloc);

    TCase *tc4 = tcase_create("string_buffers_misc_no_realloc");
    tcase_add_checked_fixture(tc4,
                              setup_generic_tests,
                              teardown_realloc_tests);
    tcase_add_test(tc4, test_RFS_in_recursive_functions_no_realloc);
    tcase_add_test(tc4, test_RFS_in_recursive_functions_with_local_no_realloc);
    tcase_add_test(tc4, test_RFS_NT);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    suite_add_tcase(s, tc3);
    suite_add_tcase(s, tc4);
    return s;
}
