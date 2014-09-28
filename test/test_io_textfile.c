#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "test_io_helpers.h"
#include "utilities_for_testing.h"


#include <refu.h>
#include <String/rf_str_files.h>
#include <String/rf_str_filesx.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
#include <String/rf_str_manipulation.h>
#include <String/rf_str_traversalx.h>
#include <System/rf_system.h>
#include <IO/rf_textfile.h>
#include <Utils/array.h>


#define PARTIAL_SECOND_LINE_UTF8                    \
    "２日の東京外国為替市場では、主要通貨に対して"

#define PARTIAL_SECOND_LINE_UTF16               \
    "Κανένα περιθώριο για δ"

#define PARTIAL_SECOND_LINE_UTF32_BE                \
    "また米兵が沖縄の人の暮らしを脅かした。２日未"

#define PARTIAL_SECOND_LINE_UTF32_LE            \
    "Η χώρα κρέμεται πάλι α"

static const char *partial_line_scenario2[] = {
    PARTIAL_SECOND_LINE_UTF8"\n",
    PARTIAL_SECOND_LINE_UTF16"\n",
    PARTIAL_SECOND_LINE_UTF16"\n",
    /* It's actually opposite"\n", file has wrong string inside  */
    PARTIAL_SECOND_LINE_UTF32_BE"\n",
    /* It's actually opposite, file has wrong string inside  */
    PARTIAL_SECOND_LINE_UTF32_LE"\n",


    PARTIAL_SECOND_LINE_UTF8,
    PARTIAL_SECOND_LINE_UTF16,
    PARTIAL_SECOND_LINE_UTF16,
    /* It's actually opposite, file has wrong string inside  */
    PARTIAL_SECOND_LINE_UTF32_BE,
    /* It's actually opposite, file has wrong string inside  */
    PARTIAL_SECOND_LINE_UTF32_LE
};

struct RFstringx g_buff;
struct RFstringx g_fname;
void setup_textfile_tests()
{
    rf_init("refuclib.log", 0, LOG_DEBUG);
    ck_assert(rf_stringx_init_buff(&g_buff, 512, ""));
    ck_assert(rf_stringx_init_buff(&g_fname, 64, ""));
}
void teardown_textfile_tests()
{
    rf_stringx_deinit(&g_buff);
    rf_stringx_deinit(&g_fname);
    rf_deinit();
}

static void test_textfile_read_line_generic(const char *filename,
                                            enum RFtext_encoding encoding,
                                            enum RFendianess endianess)
{
    struct RFtextfile f;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, filename, strlen(filename)));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_READ, endianess,
                               encoding, RF_EOL_LF));

    /* 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(encoding, endianess, false, line_scenario1));
    /* 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(encoding, endianess, false, line_scenario2));
    /* 3rd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(encoding, endianess, false, line_scenario3));
    /* EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&f, &g_buff));

    rf_textfile_deinit(&f);
}

static void test_textfile_read_line_chars_generic(const char *filename,
                                                  enum RFtext_encoding encoding,
                                                  enum RFendianess endianess)
{
    struct RFtextfile f;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, filename, strlen(filename)));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_READ, endianess,
                               encoding, RF_EOL_LF));

    /* 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(encoding, endianess, false, line_scenario1));
    /* 2nd line partial */
    ck_assert(RF_SUCCESS == rf_textfile_read_line_chars(&f, &g_buff, 22));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(encoding, endianess, false, partial_line_scenario2));

    rf_textfile_deinit(&f);
}

/* Textfile control tests -- START */
START_TEST(test_textfile_init) {
    struct RFtextfile f;

    /* open existing file for reading */
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"utf8stringfile",
                  strlen(CLIB_TESTS_PATH"utf8stringfile")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_READ, RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));
    rf_textfile_deinit(&f);

    /* open new file for writting */
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));
    ck_assert(rf_system_delete_file(&g_fname));
    rf_textfile_deinit(&f);

    /* illegal input */
    ck_assert(!rf_textfile_init(&f, &g_fname, 999,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));
    ck_assert(!rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, 9999));
    rf_textfile_deinit(&f);

    ck_assert(rf_system_delete_file(&g_fname));
}END_TEST

START_TEST(test_textfile_copy) {
    struct RFtextfile f;
    struct RFtextfile copy_f;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"utf8stringfile",
                  strlen(CLIB_TESTS_PATH"utf8stringfile")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_READ,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* make a copy */
    ck_assert(rf_textfile_copy_in(&copy_f, &f));
    /* 2nd line from copy */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&copy_f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2));

    /* illegal input */
    ck_assert(!rf_textfile_copy_in(&copy_f, NULL));

    rf_textfile_deinit(&f);
    rf_textfile_deinit(&copy_f);
}END_TEST

START_TEST(test_textfile_set_mode) {
    struct RFtextfile f;
    static const char *s;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* write line 1 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario1);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 2 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario2);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 3 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario3);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* go to the starting position and switch to reading */
    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));
    ck_assert(rf_textfile_set_mode(&f, RF_FILE_READ));

    /* read 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* read 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2));

    /* read 3rd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario3));

    /* check for EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&f, &g_buff));

    ck_assert(rf_system_delete_file(&g_fname));

    rf_textfile_deinit(&f);
}END_TEST

/* Textfile Read Line tests -- START */
START_TEST(test_textfile_read_line_utf8) {
    test_textfile_read_line_generic(CLIB_TESTS_PATH"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_textfile_read_line_utf16_le) {
    test_textfile_read_line_generic(CLIB_TESTS_PATH"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_utf16_be) {
    test_textfile_read_line_generic(CLIB_TESTS_PATH"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_utf32_le) {
    test_textfile_read_line_generic(CLIB_TESTS_PATH"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_utf32_be) {
    test_textfile_read_line_generic(CLIB_TESTS_PATH"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_chars_utf8) {
    test_textfile_read_line_chars_generic(CLIB_TESTS_PATH"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_textfile_read_line_chars_utf16_le) {
    test_textfile_read_line_chars_generic(CLIB_TESTS_PATH"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_chars_utf16_be) {
    test_textfile_read_line_chars_generic(CLIB_TESTS_PATH"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_chars_utf32_le) {
    test_textfile_read_line_chars_generic(CLIB_TESTS_PATH"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_line_chars_utf32_be) {
    test_textfile_read_line_chars_generic(CLIB_TESTS_PATH"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_textfile_read_lines) {
    struct RFtextfile f;
    static const struct RFstring fname = RF_STRING_STATIC_INIT(
        CLIB_TESTS_PATH"utf8stringfile"
    );
    uint32_t buff_arr[64];
    struct RFarray line_arr = RF_ARRAY_SHALLOW_INIT(buff_arr);
    ck_assert(rf_textfile_init(&f, &fname, RF_FILE_READ,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    rf_stringx_reset(&g_buff);
    /* read all lines (3) */
    ck_assert(3 == rf_textfile_read_lines(&f, 0, &g_buff, &line_arr));
    ck_assert_rf_str_eq_cstr(&g_buff,
                             FIRST_LINE_UTF8"\n"
                             SECOND_LINE_UTF8"\n"
                             THIRD_LINE_UTF8"\n"
    );

    ck_assert_int_eq(rf_array_at_unsafe(&line_arr, 0, uint32_t), 0);
    ck_assert_int_eq(rf_array_at_unsafe(&line_arr, 1, uint32_t), 11);
    ck_assert_int_eq(rf_array_at_unsafe(&line_arr, 2, uint32_t), 366);
    ck_assert_int_eq(rf_array_at_unsafe(&line_arr, 3, uint32_t), 1166);

    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));

    /* read 3 lines */
    ck_assert(3 == rf_textfile_read_lines(&f, 3, &g_buff, 0));
    ck_assert_rf_str_eq_cstr(&g_buff,
                             FIRST_LINE_UTF8"\n"
                             SECOND_LINE_UTF8"\n"
                             THIRD_LINE_UTF8"\n"
    );

    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));

    /* read 2 lines */
    ck_assert(2 == rf_textfile_read_lines(&f, 2, &g_buff, 0));
    ck_assert_rf_str_eq_cstr(&g_buff,
                             FIRST_LINE_UTF8"\n"
                             SECOND_LINE_UTF8"\n"
    );

    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));

    /* read 1 line */
    ck_assert(1 == rf_textfile_read_lines(&f, 1, &g_buff, 0));
    ck_assert_rf_str_eq_cstr(&g_buff, FIRST_LINE_UTF8"\n");

    rf_textfile_deinit(&f);
}END_TEST
/* Textfile Writting tests -- START */

START_TEST(test_textfile_write) {
    struct RFtextfile f;
    struct RFtextfile rf;

    static const char *s;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* write line 1 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario1);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 2 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario2);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 3 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario3);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* illegal input */
    ck_assert(!rf_textfile_write(&f, NULL));

    rf_textfile_deinit(&f);

    /* reading */
    ck_assert(rf_textfile_init(&rf, &g_fname, RF_FILE_READ,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));
    /* read 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&rf, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* read 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&rf, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2));

    /* read 3rd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&rf, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario3));

    /* check for EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&rf, &g_buff));

    ck_assert(rf_system_delete_file(&g_fname));

    rf_textfile_deinit(&rf);
}END_TEST

START_TEST(test_textfile_insert_after) {
    struct RFtextfile f;
    static const char *s;
    static const struct RFstring str = RF_STRING_STATIC_INIT("placeholder");
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* write line 1 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario1);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 3 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario3);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* now insert line in between */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_insert(&f, 1, &g_buff, true));

    /* go to the starting position and switch to reading */
    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));
    ck_assert(rf_textfile_set_mode(&f, RF_FILE_READ));

    /* read 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* read 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2));

    /* read 3rd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario3));
    /* EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&f, &g_buff));

    /* illegal input */
    ck_assert(!rf_textfile_insert(&f, 0, &str, true));
    ck_assert(!rf_textfile_insert(&f, 9999, &str, true));
    ck_assert(!rf_textfile_insert(&f, 1, NULL, true));


    ck_assert(rf_system_delete_file(&g_fname));
    rf_textfile_deinit(&f);
}END_TEST

START_TEST(test_textfile_insert_before) {
    struct RFtextfile f;
    static const char *s;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* write line 1 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario1);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 3 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario3);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* now insert line before 1st */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_insert(&f, 1, &g_buff, false));

    /* go to the starting position and switch to reading */
    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));
    ck_assert(rf_textfile_set_mode(&f, RF_FILE_READ));

    /* read 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario2));

    /* read 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* read 3rd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario3));
    /* EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&f, &g_buff));

    rf_textfile_deinit(&f);
    ck_assert(rf_system_delete_file(&g_fname));
}END_TEST

START_TEST(test_textfile_remove) {
    struct RFtextfile f;
    static const char *s;
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* write line 1 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario1);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 2 */
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, SECOND_LINE_UTF8"\n",
                  strlen(SECOND_LINE_UTF8"\n")));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 3 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario3);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, THIRD_LINE_UTF8"\n",
                  strlen(THIRD_LINE_UTF8"\n")));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* remove the 2nd line */
    ck_assert(rf_textfile_remove(&f, 2));

    /* go to the starting position and switch to reading */
    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));
    ck_assert(rf_textfile_set_mode(&f, RF_FILE_READ));

    /* read 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* read 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario3));

    /* EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&f, &g_buff));

    /* illegal input */
    ck_assert(!rf_textfile_remove(&f, 0));
    ck_assert(!rf_textfile_remove(&f, 9999));

    rf_textfile_deinit(&f);
    ck_assert(rf_system_delete_file(&g_fname));
}END_TEST

START_TEST(test_textfile_replace) {
    struct RFtextfile f;
    static const char *s;
    static struct RFstring str = RF_STRING_STATIC_INIT("Line Replacement");
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_fname, CLIB_TESTS_PATH"temp_file",
                  strlen(CLIB_TESTS_PATH"temp_file")));
    ck_assert(rf_textfile_init(&f, &g_fname, RF_FILE_NEW,
                               RF_ENDIANESS_UNKNOWN,
                               RF_UTF8, RF_EOL_LF));

    /* write line 1 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario1);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, s,
                  strlen(s)));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 2 */
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, SECOND_LINE_UTF8"\n",
                  strlen(SECOND_LINE_UTF8"\n")));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* write line 3 */
    s = get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, true, line_scenario3);
    ck_assert(rf_stringx_assign_unsafe_nnt(
                  &g_buff, THIRD_LINE_UTF8"\n",
                  strlen(THIRD_LINE_UTF8"\n")));
    ck_assert(rf_textfile_write(&f, &g_buff));

    /* replace the 2nd line */
    ck_assert(rf_textfile_replace(&f, 2, &str));

    /* go to the starting position and switch to reading */
    ck_assert(RF_SUCCESS == rf_textfile_go_to_line(&f, 1));
    ck_assert(rf_textfile_set_mode(&f, RF_FILE_READ));

    /* read 1st line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(
        &g_buff,
        get_line(RF_UTF8, RF_ENDIANESS_UNKNOWN, false, line_scenario1));

    /* read 2nd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert(rf_string_equal(&str, &g_buff));

    /* read 3rd line */
    ck_assert(RF_SUCCESS == rf_textfile_read_line(&f, &g_buff));
    ck_assert_rf_str_eq_cstr(&g_buff, THIRD_LINE_UTF8);

    /* EOF */
    ck_assert(RE_FILE_EOF == rf_textfile_read_line(&f, &g_buff));

    /* illegal input */
    ck_assert(!rf_textfile_replace(&f, 0, &str));
    ck_assert(!rf_textfile_replace(&f, 9999, &str));
    ck_assert(!rf_textfile_replace(&f, 2, NULL));

    ck_assert(rf_system_delete_file(&g_fname));
    rf_textfile_deinit(&f);
}END_TEST


Suite *io_textfile_suite_create(void)
{
    Suite *s = suite_create("Textfile");

    TCase *textfile_control = tcase_create("Textfile Control");
    tcase_add_checked_fixture(textfile_control,
                              setup_textfile_tests,
                              teardown_textfile_tests);
    tcase_add_test(textfile_control, test_textfile_init);
    tcase_add_test(textfile_control, test_textfile_copy);
    tcase_add_test(textfile_control, test_textfile_set_mode);

    TCase *textfile_read_lines = tcase_create("Textfile Reading");
    tcase_add_checked_fixture(textfile_read_lines,
                              setup_textfile_tests,
                              teardown_textfile_tests);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_utf8);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_utf16_le);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_utf16_be);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_utf32_le);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_utf32_be);

    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_chars_utf8);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_chars_utf16_le);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_chars_utf16_be);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_chars_utf32_le);
    tcase_add_test(textfile_read_lines,
                   test_textfile_read_line_chars_utf32_be);
    tcase_add_test(textfile_read_lines, test_textfile_read_lines);

    TCase *textfile_writting = tcase_create("Textfile Writting");
    tcase_add_checked_fixture(textfile_writting,
                              setup_textfile_tests,
                              teardown_textfile_tests);
    tcase_add_test(textfile_writting, test_textfile_write);
    tcase_add_test(textfile_writting, test_textfile_insert_after);
    tcase_add_test(textfile_writting, test_textfile_insert_before);
    tcase_add_test(textfile_writting, test_textfile_remove);
    tcase_add_test(textfile_writting, test_textfile_replace);

    suite_add_tcase(s, textfile_control);
    suite_add_tcase(s, textfile_read_lines);
    suite_add_tcase(s, textfile_writting);

    return s;
}
