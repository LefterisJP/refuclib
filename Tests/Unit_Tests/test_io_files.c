#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <refu.h>
#include <String/rf_str_files.h>
#include <String/rf_str_filesx.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
#include <String/rf_str_manipulation.h>
#include <System/rf_system.h>
#include <IO/rf_file.h>

#define PATH_TO "Tests/Unit_Tests/"
#define SECOND_LINE_UTF8 \
    "２日の東京外国為替市場では、主要通貨に対して"                      \
    "円が売られている。午前１０時現在、ドルに対する円"                  \
    "相場は前日午後５時時点に比べ２９銭円安ドル高の"                    \
    "１ドル＝８０円２６～２７銭。ユーロに対しては同２８銭円"            \
    "安ユーロ高の１ユーロ＝１０３円７６～８０銭。"

#define SECOND_LINE_UTF16 \
    "Κανένα περιθώριο για διαπραγμάτευση δεν αφήνει το ΔΝΤ"         \
    " το οποίο ξεκαθαρίζει πως οι αλλαγές στα εργασιακά αποτελούν " \
    "αναπόσπαστο κομμάτι της συμφωνίας Ελλάδας και τρόικας."        \

#define SECOND_LINE_UTF32_BE                                         \
    "また米兵が沖縄の人の暮らしを脅かした。２日未明、酔った米"       \
    "兵が民家に押し入り、中学生を殴ったとされる事件。半月前"         \
    "の米兵による集団強姦（ごうかん）事件を受けて在日米軍に出た"     \
    "外出禁止令を破る振る舞いに、「米兵には綱紀も何もない」"         \
    "と憤りの声が上がった。"

#define SECOND_LINE_UTF32_LE                                              \
    "Η χώρα κρέμεται πάλι από μια κλωστή. Η σημερινή ψηφοφορία"         \
    " στην Βουλή δείχνει πως το πολιτικό σύστημα επιμένει στην αυτοκτονία," \
    " τη δική του και της χώρας."


#define THIRD_LINE_UTF8                                                 \
    "Αντισυνταγματικές κρίθηκαν σχεδόν ομόφωνα από την Ολομέλεια "      \
    "του Ελεγκτικού Συνεδρίου, σύμφωνα με πληροφορίες, οι διατάξεις "   \
    "του υπουργείου Οικονομικών με τις οποίες επέρχονται νέες μειώσεις " \
    "στις συντάξεις, της τάξεως του 5% έως 10%, ενώ με την είσοδο του " \
    "νέου έτους αυξάνεται το όριο συνταξιοδότησης κατά δύο έτη στα 67 " \
    "και καταργούνται για τους συνταξιούχους τα επιδόματα (δώρα) "      \
    "Χριστουγέννων και Πάσχα, όπως και το επίδομα θερινής άδειας."

#define THIRD_LINE_UTF16                            \
    "米国の経済指標が市場予想を上回ったことから、"  \
    "米景気が緩やかに改善するとの期待が広がり、"    \
    "ドルが買われ円が売られた。"

#define THIRD_LINE_UTF32_LE                                             \
    "中塚一宏金融相は２日の閣議後の記者会見で、インターネットバンキング" \
    "で偽画面を使った不正送金が相次いでいることを受け、すべての銀行に対し" \
    "、同じような被害があるかどうかを報告するよう求めたことを明らかにした。"

#define THIRD_LINE_UTF32_BE                                             \
    "Για την επίπτωση της διαπραγμάτευσης κυβέρνησης - τρόικας για τα " \
    "εργασιακά στον κυβερνητικό συνασπισμό είπε πως αυτό αφορά την "   \
    "ίδια την κυβέρνηση."

static const char *line_scenario1[] = {
    "UTF-8 File\n",
    "Little Endian UTF-16 file\n",
    "Big Endian UTF-16 file\n",
    /* It's actually opposite, file has wrong string inside  */
    "Little Endian UTF-32 string file\n",
    /* It's actually opposite, file has wrong string inside  */
    "Big Endian UTF-32 string file\n"
};

static const char *line_scenario2[] = {
    SECOND_LINE_UTF8,

    SECOND_LINE_UTF16,

    SECOND_LINE_UTF16,

    /* It's actually opposite, file has wrong string inside  */
    SECOND_LINE_UTF32_BE,
    /* It's actually opposite, file has wrong string inside  */
    SECOND_LINE_UTF32_LE
};

static const char *line_scenario3[] = {
    THIRD_LINE_UTF8,

    THIRD_LINE_UTF16,

    THIRD_LINE_UTF16,

    /* It's actually opposite, file has wrong string inside  */
    THIRD_LINE_UTF32_BE,

    /* It's actually opposite, file has wrong string inside  */
    THIRD_LINE_UTF32_LE,
};


static const uint32_t line_scenario4_bytes[] = {18, 12, 12, 24, 20};
/* each line is encoded in its own encoding */
static const char *line_scenario4[] = {
    "２日の東京外",

    "\x9A\x3\xB1\x3\xBD\x3\xAD\x3\xBD\x3\xB1\x3",
    "\x9A\x3\xB1\x3\xBD\x3\xAD\x3\xBD\x3\xB1\x3",

    /* It's actually opposite, file has wrong string inside  */
    "\x7E\x30\0\0\5F\x30\0\0\x73\x7c\0\0\x75\x51\0\0\x4c\x30\0\0\x96\x6C\0\0",
    /* It's actually opposite, file has wrong string inside  */
    "\x97\x3\0\0\xC7\x3\0\0\xCE\x3\0\0\xC1\x3\0\0\xB1\x3\0\0"
};

/* one character but in original encodring */
static const uint32_t char_scenario1[] = {
    0x92BCEF,
    0x39A,
    0x39A,
    0x307E,
    0x397
};

/* one character but in unicode codepoints */
static const uint32_t char_scenario2[] = {
    0x65E5,
    0x3B1,
    0x3B1,
    0x305F,
    0x20
};


static const char *get_line(int encoding, int endianess, const char* scenario[])
{
    if (encoding == RF_UTF8) {
        return scenario[0];
    } else if (encoding == RF_UTF16 && endianess == RF_LITTLE_ENDIAN) {
        return scenario[1];
    } else if (encoding == RF_UTF16 && endianess == RF_BIG_ENDIAN) {
        return scenario[2];
    } else if (encoding == RF_UTF32 && endianess == RF_LITTLE_ENDIAN) {
        return scenario[4];
    }

    return scenario[3];
}

static const uint32_t get_uint32_array(int encoding, int endianess,
                                       const uint32_t* arr)
{
    if (encoding == RF_UTF8) {
        return arr[0];
    } else if (encoding == RF_UTF16 && endianess == RF_LITTLE_ENDIAN) {
        return arr[1];
    } else if (encoding == RF_UTF16 && endianess == RF_BIG_ENDIAN) {
        return arr[2];
    } else if (encoding == RF_UTF32 && endianess == RF_LITTLE_ENDIAN) {
        return arr[4];
    }
    return arr[3];
}

static bool choose_read_line(int encoding,
                             FILE *f,
                             enum RFeol_mark eol,
                             char** utf8,
                             uint32_t* byte_length,
                             uint32_t* buffer_size,
                             char* eof,
                             enum RFendianess endianess)
{
    /* this also tests that NULL can be provided to bytes_read argument */
    switch(encoding) {
    case RF_UTF8:
        return rf_file_read_line_utf8(f, eol, utf8, byte_length, buffer_size, eof);
        break;
    case RF_UTF16:
        return rf_file_read_line_utf16(f, eol, utf8, byte_length,
                                       eof, NULL, endianess);
        break;
    case RF_UTF32:
        return rf_file_read_line_utf32(f, eol, utf8, byte_length,
                                       eof, NULL, endianess);
        break;
    }
    return false;
}

static void test_file_read_line_generic(const char* filename, int encoding,
                                        int endianess)
{
    FILE *f;
    char eof;
    char *utf8;
    uint32_t byte_length;
    uint32_t buffer_size;

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(
        choose_read_line(
            encoding, f, RF_EOL_LF, &utf8, &byte_length, &buffer_size, &eof, endianess
        )
    );


    ck_assert_nnt_str_eq_cstr(
        utf8 + 3, /* skip the bom */
        get_line(encoding, endianess, line_scenario1)
    );
    free(utf8);

    ck_assert(
        choose_read_line(
            encoding, f, RF_EOL_LF, &utf8, &byte_length, &buffer_size, &eof, endianess
        )
    );
    ck_assert_nnt_str_eq_cstr(
        utf8,
        get_line(encoding, endianess, line_scenario2)
    );
    free(utf8);

    ck_assert(
        choose_read_line(
            encoding, f, RF_EOL_LF, &utf8, &byte_length, &buffer_size, &eof, endianess
        )
    );
    ck_assert_nnt_str_eq_cstr(
        utf8,
        get_line(encoding, endianess, line_scenario3)
    );

    /* invalid input */
    ck_assert(
        !choose_read_line(
            encoding, f, RF_EOL_LF, NULL, &byte_length, &buffer_size, &eof, endianess
        )
    );


    free(utf8);
    fclose(f);
}

static bool choose_read_bytes(int encoding,
                             FILE *f,
                             enum RFeol_mark eol,
                             char* buff,
                             char* eof,
                             enum RFendianess endianess)
{
    int cnum = get_uint32_array(encoding, endianess, line_scenario4_bytes);
    /* this also tests that NULL can be provided to bytes_read argument */
    switch(encoding) {
    case RF_UTF8:
        return rf_file_read_bytes_utf8(buff, cnum, f, eof, eol, NULL);
        break;
    case RF_UTF16:
        return rf_file_read_bytes_utf16(buff, cnum, f, eof, eol, NULL, endianess);
        break;
    case RF_UTF32:
        return rf_file_read_bytes_utf32(buff, cnum, f, eof, eol, NULL, endianess);
        break;
    }
    return false;
}
static void test_file_read_bytes_generic(const char* filename, int encoding,
                                         int endianess)
{
    FILE *f;
    char eof;
    char buff[4096];
    char *utf8;
    uint32_t byte_length;
    uint32_t buffer_size;

    ck_assert((f = fopen(filename, "rb")) != NULL);

    /* read first line */
    ck_assert(
        choose_read_line(
            encoding, f, RF_EOL_LF, &utf8, &byte_length, &buffer_size, &eof, endianess
        )
    );
    free(utf8);
    /* and then read 6 next characters */
    ck_assert(
        choose_read_bytes(
            encoding, f, RF_EOL_LF, buff, &eof, endianess
        )
    );

    ck_assert_nnt_str_eq_cstr(buff, get_line(encoding, endianess, line_scenario4));

    /* invalid input */
    ck_assert(
        !choose_read_line(
            encoding, f, RF_EOL_LF, NULL, &byte_length, &buffer_size, &eof, endianess
        )
    );

    fclose(f);
}


static bool choose_read_char(int encoding,
                             FILE *f,
                             uint32_t *ret_value,
                             bool cp,
                             char* eof,
                             enum RFendianess endianess)
{
    switch(encoding) {
    case RF_UTF8:
        return rf_file_read_char_utf8(f, ret_value, cp, eof);
        break;
    case RF_UTF16:
        return rf_file_read_char_utf16(f, ret_value, cp, endianess, eof);
        break;
    case RF_UTF32:
        return rf_file_read_char_utf32(f, ret_value, endianess, eof);
        break;
    }
    return false;
}
static void test_file_read_char_generic(const char* filename, int encoding,
                                        int endianess)
{
    FILE *f;
    char eof;
    char *utf8;
    uint32_t byte_length;
    uint32_t buffer_size;
    uint32_t ret_value;

    ck_assert((f = fopen(filename, "rb")) != NULL);

    /* read first line */
    ck_assert(
        choose_read_line(
            encoding, f, RF_EOL_LF, &utf8, &byte_length, &buffer_size, &eof, endianess
        )
    );
    free(utf8);
    /* and then read 1 character without decoding to codepoint */
    ck_assert(
        choose_read_char(
            encoding, f, &ret_value, false, &eof, endianess
        )
    );
    ck_assert_uint_eq(ret_value,
                      get_uint32_array(encoding, endianess, char_scenario1));
    /* then read 1 more character and decode to codepoint */
    ck_assert(
        choose_read_char(
            encoding, f, &ret_value, true, &eof, endianess
        )
    );
    ck_assert_uint_eq(ret_value,
                      get_uint32_array(encoding, endianess, char_scenario2));


    fclose(f);
}


static bool choose_move_back_char(int encoding,
                             FILE *f,
                             uint32_t *ret_value,
                             enum RFendianess endianess)
{
    switch(encoding) {
    case RF_UTF8:
        return rf_file_move_back_char_utf8(f, ret_value);
        break;
    case RF_UTF16:
        return rf_file_move_back_char_utf16(f, ret_value, endianess);
        break;
    case RF_UTF32:
        return rf_file_move_back_char_utf32(f, ret_value, endianess);
        break;
    }
    return false;
}
static void test_file_move_back_char_generic(const char* filename, int encoding,
                                             int endianess)
{
    FILE *f;
    char eof;
    char *utf8;
    uint32_t byte_length;
    uint32_t buffer_size;
    uint32_t ret_value;

    ck_assert((f = fopen(filename, "rb")) != NULL);

    /* read first line */
    ck_assert(
        choose_read_line(
            encoding, f, RF_EOL_LF, &utf8, &byte_length, &buffer_size, &eof, endianess
        )
    );
    free(utf8);
    /* and then read 1 character forward */
    ck_assert(
        choose_read_char(
            encoding, f, &ret_value, false, &eof, endianess
        )
    );
    ck_assert_uint_eq(ret_value,
                      get_uint32_array(encoding, endianess, char_scenario1));
    /* then read 1 character forward decoding the codepont */
    ck_assert(
        choose_read_char(
            encoding, f, &ret_value, true, &eof, endianess
        )
    );
    ck_assert_uint_eq(ret_value,
                      get_uint32_array(encoding, endianess, char_scenario2));
    /* then read 1 character backwards (should yield same result - codepoints are decoded) */
    ck_assert(
        choose_move_back_char(
            encoding, f, &ret_value, endianess
        )
    );
    ck_assert_uint_eq(ret_value,
                      get_uint32_array(encoding, endianess, char_scenario2));


    fclose(f);
}

/* IO Read Line tests -- START */

START_TEST(test_file_read_line_utf8) {
    test_file_read_line_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_file_read_line_utf16_le) {
    test_file_read_line_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_read_line_utf16_be) {
    test_file_read_line_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_file_read_line_utf32_le) {
    test_file_read_line_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_read_line_utf32_be) {
    test_file_read_line_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST

/* IO Read bytes tests -- START */

START_TEST(test_file_read_bytes_utf8) {
    test_file_read_bytes_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_file_read_bytes_utf16_le) {
    test_file_read_bytes_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_read_bytes_utf16_be) {
    test_file_read_bytes_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_file_read_bytes_utf32_le) {
    test_file_read_bytes_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_read_bytes_utf32_be) {
    test_file_read_bytes_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST

/* IO Read char tests -- START */

START_TEST(test_file_read_char_utf8) {
    test_file_read_char_generic(PATH_TO"utf8stringfile", RF_UTF8,
                                RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_file_read_char_utf16_le) {
    test_file_read_char_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_read_char_utf16_be) {
    test_file_read_char_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_file_read_char_utf32_le) {
    test_file_read_char_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_read_char_utf32_be) {
    test_file_read_char_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST


/* IO move back char tests -- START */

START_TEST(test_file_move_back_char_utf8) {
    test_file_move_back_char_generic(PATH_TO"utf8stringfile", RF_UTF8,
                                RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_file_move_back_char_utf16_le) {
    test_file_move_back_char_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_move_back_char_utf16_be) {
    test_file_move_back_char_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_file_move_back_char_utf32_le) {
    test_file_move_back_char_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_file_move_back_char_utf32_be) {
    test_file_move_back_char_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST

Suite *io_files_suite_create(void)
{
    Suite *s = suite_create("Files I/O");

    TCase *io_read_line = tcase_create("Read Line");
    tcase_add_checked_fixture(io_read_line,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(io_read_line, test_file_read_line_utf8);
    tcase_add_test(io_read_line, test_file_read_line_utf16_le);
    tcase_add_test(io_read_line, test_file_read_line_utf16_be);
    tcase_add_test(io_read_line, test_file_read_line_utf32_le);
    tcase_add_test(io_read_line, test_file_read_line_utf32_be);

    TCase *io_read_bytes = tcase_create("Read Bytes");
    tcase_add_checked_fixture(io_read_bytes,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(io_read_bytes, test_file_read_bytes_utf8);
    tcase_add_test(io_read_bytes, test_file_read_bytes_utf16_le);
    tcase_add_test(io_read_bytes, test_file_read_bytes_utf16_be);
    tcase_add_test(io_read_bytes, test_file_read_bytes_utf32_le);
    tcase_add_test(io_read_bytes, test_file_read_bytes_utf32_be);

    TCase *io_read_char = tcase_create("Read Character");
    tcase_add_checked_fixture(io_read_char,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(io_read_char, test_file_read_char_utf8);
    tcase_add_test(io_read_char, test_file_read_char_utf16_le);
    tcase_add_test(io_read_char, test_file_read_char_utf16_be);
    tcase_add_test(io_read_char, test_file_read_char_utf32_le);
    tcase_add_test(io_read_char, test_file_read_char_utf32_be);

    TCase *io_move_back_char = tcase_create("Read Character");
    tcase_add_checked_fixture(io_move_back_char,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(io_move_back_char, test_file_move_back_char_utf8);
    tcase_add_test(io_move_back_char, test_file_move_back_char_utf16_le);
    tcase_add_test(io_move_back_char, test_file_move_back_char_utf16_be);
    tcase_add_test(io_move_back_char, test_file_move_back_char_utf32_le);
    tcase_add_test(io_move_back_char, test_file_move_back_char_utf32_be);


    suite_add_tcase(s, io_read_line);
    suite_add_tcase(s, io_read_bytes);
    suite_add_tcase(s, io_read_char);
    suite_add_tcase(s, io_move_back_char);

    return s;
}
