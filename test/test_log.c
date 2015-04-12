#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <refu.h>
#include <Utils/log.h>
#include <System/rf_system.h>

#include <String/rf_str_common.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
#include <String/rf_str_retrieval.h>
#include <IO/rf_textfile.h>

void setup_log_tests()
{
    rf_init(LOG_TARGET_FILE, "refuclib.log", LOG_DEBUG, RF_DEFAULT_STRING_BUFFERS_SIZE);
}

void teardown_log_tests()
{
    rf_deinit();
}

/* --- Simple Log Tests --- START --- */
START_TEST(test_log_flush_and_check) {
    static const struct RFstring log_name = RF_STRING_STATIC_INIT("refuclib.log");
    struct RFtextfile log_file;
    struct RFstringx buff;
    static const struct RFstring s1 = RF_STRING_STATIC_INIT("Error1");
    static const struct RFstring s2 = RF_STRING_STATIC_INIT("Error2");
    static const struct RFstring s3 = RF_STRING_STATIC_INIT("Error3");
    static const struct RFstring s4 = RF_STRING_STATIC_INIT("Error4");
    static const struct RFstring s5 = RF_STRING_STATIC_INIT("Error5");
    static const struct RFstring s6 = RF_STRING_STATIC_INIT("Error6");
    static const struct RFstring s7 = RF_STRING_STATIC_INIT("Error7");
    static const struct RFstring s8 = RF_STRING_STATIC_INIT("Error8");
    static const struct RFstring s9 = RF_STRING_STATIC_INIT("Error9");
    static const struct RFstring s10 = RF_STRING_STATIC_INIT("Error10");


    RF_ERROR("Error1");
    RF_ERROR("Error2");
    RF_ERROR("Error3");
    RF_ERROR("Error4");
    RF_ERROR("Error5");
    RF_ERROR("Error6");
    RF_ERROR("Error7");
    RF_ERROR("Error8");
    RF_ERROR("Error9");
    RF_ERROR("Error10");

    ck_assert(RF_LOG_FLUSH());

    ck_assert(rf_textfile_init(&log_file,
                               &log_name, RF_FILE_READ,
                               RF_LITTLE_ENDIAN, RF_UTF8, RF_EOL_LF));
    ck_assert(rf_stringx_init_buff(&buff, 1024, ""));

    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s1, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s2, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s3, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s4, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s5, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s6, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s7, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s8, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s9, 0));
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    ck_assert(RF_FAILURE != rf_string_find(&buff, &s10, 0));

    rf_stringx_deinit(&buff);
    rf_textfile_deinit(&log_file);
    rf_system_delete_file(&log_name);
}END_TEST

START_TEST(test_log_a_lot) {
    unsigned i;
    struct RFtextfile log_file;
    struct RFstringx buff;
    static const struct RFstring log_name = RF_STRING_STATIC_INIT("refuclib.log");

    for (i = 0; i < 1000; i ++) {
        RF_INFO("This information message will be brought to you again and "
                "again and again and again and again and again!");
    }
    
    ck_assert(RF_LOG_FLUSH());

    ck_assert(rf_textfile_init(&log_file,
                               &log_name, RF_FILE_READ,
                               RF_LITTLE_ENDIAN, RF_UTF8, RF_EOL_LF));
    ck_assert(rf_stringx_init_buff(&buff, 1024, ""));
    
    for (i = 0; i < 1000; i ++) {
        ck_assert(RF_SUCCESS == rf_textfile_read_line(&log_file, &buff));
    }

    rf_stringx_deinit(&buff);
    rf_textfile_deinit(&log_file);
    rf_system_delete_file(&log_name);
}END_TEST

Suite *log_suite_create(void)
{
    Suite *s = suite_create("Log");

    TCase *simple_log = tcase_create("Simple Log");
    tcase_add_checked_fixture(simple_log,
                              setup_log_tests,
                              teardown_log_tests);
    tcase_add_test(simple_log, test_log_flush_and_check);
    tcase_add_test(simple_log, test_log_a_lot);




    suite_add_tcase(s, simple_log);

    return s;
}
