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

// TODO: Work on replacing all these defines with the same
// from test_io_helpers.h    ... there are some minor differences
// that can be addressed

#define PATH_TO "test/"
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
    "UTF-8 File",
    "Little Endian UTF-16 file",
    "Big Endian UTF-16 file",
    /* It's actually opposite, file has wrong string inside  */
    "Little Endian UTF-32 string file",
    /* It's actually opposite, file has wrong string inside  */
    "Big Endian UTF-32 string file"
};

static const char *line_scenario2[] = {
    "UTF-8 File\n"
    SECOND_LINE_UTF8,

    "Little Endian UTF-16 file\n"
    SECOND_LINE_UTF16,

    "Big Endian UTF-16 file\n"
    SECOND_LINE_UTF16,

    /* It's actually opposite, file has wrong string inside  */
    "Little Endian UTF-32 string file\n"
    SECOND_LINE_UTF32_BE,
    /* It's actually opposite, file has wrong string inside  */
    "Big Endian UTF-32 string file\n"
    SECOND_LINE_UTF32_LE
};

static const char *line_scenario3[] = {
    "UTF-8 File"
    SECOND_LINE_UTF8
    THIRD_LINE_UTF8,

    "Little Endian UTF-16 file"
    SECOND_LINE_UTF16
    THIRD_LINE_UTF16,

    "Big Endian UTF-16 file"
    SECOND_LINE_UTF16
    THIRD_LINE_UTF16,

    /* It's actually opposite, file has wrong string inside  */
    "Little Endian UTF-32 string file"
    SECOND_LINE_UTF32_BE
    THIRD_LINE_UTF32_BE,

    /* It's actually opposite, file has wrong string inside  */
    "Big Endian UTF-32 string file"
    SECOND_LINE_UTF32_LE
    THIRD_LINE_UTF32_LE,
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

static void test_rf_string_init_generic(const char* filename, int encoding,
                                         int endianess)
{
    FILE *f;
    struct RFstring s, s2;
    char eof;

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(
        rf_string_from_file_init(
            &s, f, &eof, RF_EOL_LF, encoding, endianess, NULL)
    );


    /* get rid of the BOM and check for the right string */
    ck_assert(rf_string_prune_start(&s, 1, NULL));
    ck_assert_rf_str_eq_cstr(&s, get_line(encoding,
                                                endianess, line_scenario1));

    /* invalid input */
    ck_assert(
        !rf_string_from_file_init(
            &s2, f, NULL, RF_EOL_LF, encoding, endianess, NULL)
    );
    ck_assert(
        !rf_string_from_file_init(
            &s2, f, &eof, RF_EOL_LF, 999, endianess, NULL)
    );

    rf_string_deinit(&s);
    fclose(f);
}

static void test_rf_string_assign_generic(const char* filename, int encoding,
                                         int endianess)
{
    FILE *f;
    struct RFstring s, s2;
    char eof;

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(rf_string_init(&s, "a"));
    ck_assert(
        rf_string_from_file_assign(
            &s, f, &eof, RF_EOL_LF, encoding, endianess)
    );


    /* get rid of the BOM and check for the right string */
    ck_assert(rf_string_prune_start(&s, 1, NULL));
    ck_assert_rf_str_eq_cstr(&s, get_line(encoding,
                                                endianess, line_scenario1));

    /* invalid input */
    ck_assert(
        !rf_string_from_file_assign(
            &s2, f, NULL, RF_EOL_LF, encoding, endianess)
    );
    ck_assert(
        !rf_string_from_file_assign(
            &s2, f, &eof, RF_EOL_LF, 999, endianess)
    );

    rf_string_deinit(&s);
    fclose(f);
}
static void test_rf_stringx_assign_generic(const char* filename, int encoding,
                                           int endianess)
{
    FILE *f;
    struct RFstringx s, s2;
    char eof;

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(rf_stringx_init(&s, "a"));
    ck_assert(
        rf_stringx_from_file_assign(
            &s, f, &eof, RF_EOL_LF, encoding, endianess)
    );


    /* get rid of the BOM and check for the right string */
    ck_assert(rf_string_prune_start(&s, 1, NULL));
    ck_assert_rf_strx_eq_cstr(&s, get_line(encoding,
                                           endianess, line_scenario1));

    /* invalid input */
    ck_assert(
        !rf_stringx_from_file_assign(
            &s2, f, NULL, RF_EOL_LF, encoding, endianess)
    );
    ck_assert(
        !rf_stringx_from_file_assign(
            &s2, f, &eof, RF_EOL_LF, 999, endianess)
    );

    rf_stringx_deinit(&s);
    fclose(f);
}

static void test_rf_string_append_generic(const char* filename, int encoding,
                                          int endianess)
{
    FILE *f;
    struct RFstring s, s2;
    char eof;

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(rf_string_init(&s, "a"));
    ck_assert(
        rf_string_from_file_assign(
            &s, f, &eof, RF_EOL_LF, encoding, endianess)
    );


    /* get rid of the BOM and check for the right string */
    ck_assert(rf_string_prune_start(&s, 1, NULL));
    ck_assert_rf_str_eq_cstr(&s, get_line(encoding,
                                                endianess, line_scenario1));

    ck_assert(rf_string_from_file_append(&s, f, &eof, RF_EOL_LF,
                                         encoding, endianess));

    ck_assert_rf_str_eq_cstr(&s, get_line
                             (encoding,
                              endianess, line_scenario2)
    );

    /* invalid input */
    ck_assert(
        !rf_string_from_file_append(
            &s2, f, NULL, RF_EOL_LF, encoding, endianess)
    );
    ck_assert(
        !rf_string_from_file_append(
            &s2, f, &eof, RF_EOL_LF, 999, endianess)
    );

    rf_string_deinit(&s);
    fclose(f);
}
static void test_rf_stringx_append_generic(const char* filename, int encoding,
                                          int endianess)
{
    FILE *f;
    struct RFstringx s, s2;
    char eof;

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(rf_stringx_init(&s, "a"));
    ck_assert(
        rf_stringx_from_file_assign(
            &s, f, &eof, RF_EOL_LF, encoding, endianess)
    );


    /* get rid of the BOM and check for the right string */
    ck_assert(rf_string_prune_start(&s, 1, NULL));
    ck_assert_rf_strx_eq_cstr(&s, get_line(encoding,
                                           endianess, line_scenario1));

    ck_assert(rf_stringx_from_file_append(&s, f, &eof, RF_EOL_LF,
                                          encoding, endianess));

    ck_assert_rf_strx_eq_cstr(&s, get_line
                              (encoding,
                              endianess, line_scenario2)
    );

    /* invalid input */
    ck_assert(
        !rf_stringx_from_file_append(
            &s2, f, NULL, RF_EOL_LF, encoding, endianess)
    );
    ck_assert(
        !rf_stringx_from_file_append(
            &s2, f, &eof, RF_EOL_LF, 999, endianess)
    );

    rf_stringx_deinit(&s);
    fclose(f);
}

/* this one also checks that EOF is set fine */
static void test_rf_string_fwrite_generic(const char* filename, int encoding,
                                          int endianess)
{
    FILE *f;
    FILE *out;
    struct RFstring s, s2;
    char eof;
    static struct RFstring out_name = RF_STRING_STATIC_INIT(PATH_TO"outputfile");
    static struct RFstring nl = RF_STRING_STATIC_INIT("\n");

    ck_assert((f = fopen(filename, "rb")) != NULL);
    ck_assert(
        rf_string_from_file_init(
            &s, f, &eof, RF_EOL_LF, encoding, endianess, NULL)
    );
    /* get rid of the BOM and read in the rest of the file */
    ck_assert(rf_string_prune_start(&s, 1, NULL));

    ck_assert(
        rf_string_from_file_append(
            &s, f, &eof, RF_EOL_LF, encoding, endianess)
    );
    ck_assert(
        rf_string_from_file_append(
            &s, f, &eof, RF_EOL_LF, encoding, endianess)
    );
    /* end of file should have been reached */
    ck_assert(eof == true);

    /* remove all newlines since we want to read it one-off */
    ck_assert(rf_string_remove(&s, &nl, 0, 0));
    /* open an output file and write the string there */
    ck_assert((out = fopen(PATH_TO"outputfile","w+b")) != NULL);
    ck_assert(rf_string_fwrite(&s, out, encoding, endianess));
    
    /* Read the string from the same file and compare*/
    ck_assert_int_eq(0, fseek(out, 0, SEEK_SET));

    ck_assert(rf_string_from_file_init(&s2, out, &eof, RF_EOL_LF,
                                       encoding, endianess, NULL));


    ck_assert_rf_str_eq_cstr(&s, get_line
                             (encoding,
                              endianess, line_scenario3)
    );    

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    fclose(f);
    fclose(out);

    /* invalid encoding input */
    ck_assert(!rf_string_fwrite(&s, out, 999, endianess));

    /* also delete the outfile */
    ck_assert(rf_system_delete_file(&out_name));
}







/* String init from file tests -- START */
START_TEST(test_string_from_file_init_utf8) {
    test_rf_string_init_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_string_from_file_init_utf16_le) {
    test_rf_string_init_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_init_utf16_be) {
    test_rf_string_init_generic(PATH_TO"utf16bestringfile",
                                 RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_init_utf32_le) {
    test_rf_string_init_generic(PATH_TO"utf32lestringfile",
                                 RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_init_utf32_be) {
    test_rf_string_init_generic(PATH_TO"utf32bestringfile",
                                 RF_UTF32, RF_BIG_ENDIAN);
}END_TEST



/* String assign from file tests -- START */

START_TEST(test_string_from_file_assign_utf8) {
    test_rf_string_assign_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_string_from_file_assign_utf16_le) {
    test_rf_string_assign_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_assign_utf16_be) {
    test_rf_string_assign_generic(PATH_TO"utf16bestringfile",
                                RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_assign_utf32_le) {
    test_rf_string_assign_generic(PATH_TO"utf32lestringfile",
                                RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_assign_utf32_be) {
    test_rf_string_assign_generic(PATH_TO"utf32bestringfile",
                                RF_UTF32, RF_BIG_ENDIAN);
}END_TEST


/* String append from file tests -- START */

START_TEST(test_string_from_file_append_utf8) {
    test_rf_string_append_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_string_from_file_append_utf16_le) {
    test_rf_string_append_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_append_utf16_be) {
    test_rf_string_append_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_append_utf32_le) {
    test_rf_string_append_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_from_file_append_utf32_be) {
    test_rf_string_append_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST


/* String fwrite tests -- START -- they also check for proper EOF setting */
START_TEST(test_string_fwrite_utf8) {
    test_rf_string_fwrite_generic(PATH_TO"utf8stringfile",
                                  RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_string_fwrite_utf16_le) {
    test_rf_string_fwrite_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_fwrite_utf16_be) {
    test_rf_string_fwrite_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_string_fwrite_utf32_le) {
    test_rf_string_fwrite_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_string_fwrite_utf32_be) {
    test_rf_string_fwrite_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST


/* Stringx assign from file tests -- START */

START_TEST(test_stringx_from_file_assign_utf8) {
    test_rf_stringx_assign_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_stringx_from_file_assign_utf16_le) {
    test_rf_stringx_assign_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_stringx_from_file_assign_utf16_be) {
    test_rf_stringx_assign_generic(PATH_TO"utf16bestringfile",
                                RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_stringx_from_file_assign_utf32_le) {
    test_rf_stringx_assign_generic(PATH_TO"utf32lestringfile",
                                RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_stringx_from_file_assign_utf32_be) {
    test_rf_stringx_assign_generic(PATH_TO"utf32bestringfile",
                                RF_UTF32, RF_BIG_ENDIAN);
}END_TEST


/* Stringx append from file tests -- START */

START_TEST(test_stringx_from_file_append_utf8) {
    test_rf_stringx_append_generic(PATH_TO"utf8stringfile",
                                RF_UTF8, RF_ENDIANESS_UNKNOWN);
}END_TEST

START_TEST(test_stringx_from_file_append_utf16_le) {
    test_rf_stringx_append_generic(PATH_TO"utf16lestringfile",
                                RF_UTF16, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_stringx_from_file_append_utf16_be) {
    test_rf_stringx_append_generic(PATH_TO"utf16bestringfile",
                                  RF_UTF16, RF_BIG_ENDIAN);
}END_TEST

START_TEST(test_stringx_from_file_append_utf32_le) {
    test_rf_stringx_append_generic(PATH_TO"utf32lestringfile",
                                  RF_UTF32, RF_LITTLE_ENDIAN);
}END_TEST

START_TEST(test_stringx_from_file_append_utf32_be) {
    test_rf_stringx_append_generic(PATH_TO"utf32bestringfile",
                                  RF_UTF32, RF_BIG_ENDIAN);
}END_TEST



Suite *string_files_suite_create(void)
{
    Suite *s = suite_create("String Files I/O");

    TCase *string_files_init = tcase_create("String Files init");
    tcase_add_checked_fixture(string_files_init,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_files_init, test_string_from_file_init_utf8);
    tcase_add_test(string_files_init, test_string_from_file_init_utf16_le);
    tcase_add_test(string_files_init, test_string_from_file_init_utf16_be);
    tcase_add_test(string_files_init, test_string_from_file_init_utf32_le);
    tcase_add_test(string_files_init, test_string_from_file_init_utf32_be);


    TCase *string_files_assign = tcase_create("String Files Assign");
    tcase_add_checked_fixture(string_files_assign,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_files_assign, test_string_from_file_assign_utf8);
    tcase_add_test(string_files_assign, test_string_from_file_assign_utf16_le);
    tcase_add_test(string_files_assign, test_string_from_file_assign_utf16_be);
    tcase_add_test(string_files_assign, test_string_from_file_assign_utf32_le);
    tcase_add_test(string_files_assign, test_string_from_file_assign_utf32_be);

    TCase *string_files_append = tcase_create("String Files Append");
    tcase_add_checked_fixture(string_files_append,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_files_append, test_string_from_file_append_utf8);
    tcase_add_test(string_files_append, test_string_from_file_append_utf16_le);
    tcase_add_test(string_files_append, test_string_from_file_append_utf16_be);
    tcase_add_test(string_files_append, test_string_from_file_append_utf32_le);
    tcase_add_test(string_files_append, test_string_from_file_append_utf32_be);

    TCase *string_files_fwrite = tcase_create("String Files fwrite");
    tcase_add_checked_fixture(string_files_fwrite,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_files_fwrite, test_string_fwrite_utf8);
    tcase_add_test(string_files_fwrite, test_string_fwrite_utf16_le);
    tcase_add_test(string_files_fwrite, test_string_fwrite_utf16_be);
    tcase_add_test(string_files_fwrite, test_string_fwrite_utf32_le);
    tcase_add_test(string_files_fwrite, test_string_fwrite_utf32_be);


    /* RFstringx is almost idential so just check its assign and append */
    TCase *stringx_files_assign = tcase_create("Stringx Files Assign");
    tcase_add_checked_fixture(stringx_files_assign,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(stringx_files_assign, test_stringx_from_file_assign_utf8);
    tcase_add_test(stringx_files_assign, test_stringx_from_file_assign_utf16_le);
    tcase_add_test(stringx_files_assign, test_stringx_from_file_assign_utf16_be);
    tcase_add_test(stringx_files_assign, test_stringx_from_file_assign_utf32_le);
    tcase_add_test(stringx_files_assign, test_stringx_from_file_assign_utf32_be);

    TCase *stringx_files_append = tcase_create("Stringx Files Append");
    tcase_add_checked_fixture(stringx_files_append,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(stringx_files_append, test_stringx_from_file_append_utf8);
    tcase_add_test(stringx_files_append, test_stringx_from_file_append_utf16_le);
    tcase_add_test(stringx_files_append, test_stringx_from_file_append_utf16_be);
    tcase_add_test(stringx_files_append, test_stringx_from_file_append_utf32_le);
    tcase_add_test(stringx_files_append, test_stringx_from_file_append_utf32_be);


    suite_add_tcase(s, string_files_init);
    suite_add_tcase(s, string_files_assign);
    suite_add_tcase(s, string_files_append);
    suite_add_tcase(s, string_files_fwrite);

    suite_add_tcase(s, stringx_files_assign);
    suite_add_tcase(s, stringx_files_append);
    return s;
}
