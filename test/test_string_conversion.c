#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


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

    free(buff);
    rf_string_deinit(&s);
}END_TEST

START_TEST(test_invalid_string_to_utf) {
    struct RFstring s;
    uint32_t length;
    uint16_t *buff;
    //first create the string to test. Japanese(Adachiku) + MusicalSymbol(G clef)
    ck_assert(rf_string_init(&s,"足立区\xf0\x9d\x84\x9e"));
    buff = rf_string_to_utf16(&s, &length);
    ck_assert_not_nullptr(buff);

    ck_assert(!rf_string_to_utf16(&s, NULL));
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
    int64_t num;
    size_t off;
    ck_assert(rf_stringx_init(&sx, "45687"));
    ck_assert(rf_string_init(&s2, "0"));
    ck_assert(rf_string_init(&s, "-123442"));

    ck_assert(rf_string_to_int(RF_STRX2STR(&sx), &num, NULL));
    ck_assert_int_eq(num, 45687);

    ck_assert(rf_string_to_int(&s, &num, &off));
    ck_assert_int_eq(num, -123442);
    ck_assert_uint_eq(off, 6);

    ck_assert(rf_string_to_int(&s2, &num, &off));
    ck_assert_int_eq(num, 0);
    ck_assert_uint_eq(off, 0);

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_invalid_string_to_int) {
    struct RFstring s;
    struct RFstring ns, ns2;
    int64_t num;

    ck_assert(rf_string_init(&s, "0"));
    ck_assert(!rf_string_to_int(&s, NULL, NULL));

    ck_assert(rf_string_init(&ns, "Not a number"));
    ck_assert(!rf_string_to_int(&ns, &num, NULL));

    ck_assert(rf_string_init(&ns2, ""));
    ck_assert(!rf_string_to_int(&ns2, &num, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&ns);
    rf_string_deinit(&ns2);
}END_TEST

START_TEST(test_string_to_uint) {
    uint64_t num;
    size_t off;

    static struct RFstring s1 = RF_STRING_STATIC_INIT("45632");
    static struct RFstring s2 = RF_STRING_STATIC_INIT("0xFEAC23");
    static struct RFstring s3 = RF_STRING_STATIC_INIT("074572");
    static struct RFstring s4 = RF_STRING_STATIC_INIT("0b110101");

    static struct RFstring sf2 = RF_STRING_STATIC_INIT("0xGFAC2Y");
    static struct RFstring sf3 = RF_STRING_STATIC_INIT("09289");
    static struct RFstring sf4 = RF_STRING_STATIC_INIT("0b3012310");

    ck_assert(rf_string_to_uint_dec(&s1, &num, NULL));
    ck_assert_uint_eq(45632, num);

    ck_assert(rf_string_to_uint_hex(&s2, &num, &off));
    ck_assert_uint_eq(16690211, num);
    ck_assert_uint_eq(7, off);
    ck_assert(!rf_string_to_uint_hex(&sf2, &num, &off));

    ck_assert(rf_string_to_uint_oct(&s3, &num, &off));
    ck_assert_uint_eq(31098, num);
    ck_assert_uint_eq(5, off);
    ck_assert(!rf_string_to_uint_oct(&sf3, &num, &off));

    ck_assert(rf_string_to_uint_bin(&s4, &num, &off));
    ck_assert_uint_eq(53, num);
    ck_assert_uint_eq(7, off);
    ck_assert(!rf_string_to_uint_bin(&sf4, &num, NULL));
}END_TEST

START_TEST(test_string_to_double) {
    struct RFstringx sx;
    struct RFstring s, s2;
    struct RFstring ns, ns2;
    double num;
    size_t off;
    ck_assert(rf_stringx_init(&sx, "245.9310"));
    ck_assert(rf_string_init(&s, "-0.02341"));
    ck_assert(rf_string_init(&s2, "0.0"));

    ck_assert(rf_string_to_double(RF_STRX2STR(&sx), &num, NULL));
    ck_assert_double_eq(num, 245.9310);

    ck_assert(rf_string_to_double(&s, &num, &off));
    ck_assert_double_eq(num, -0.02341);
    ck_assert_uint_eq(off, 7);

    ck_assert(rf_string_to_double(&s2, &num, &off));
    ck_assert_double_eq(num, 0.0f);
    ck_assert_uint_eq(off, 2);

    ck_assert(rf_string_init(&ns, "Not a number"));
    ck_assert(!rf_string_to_double(&ns, &num, NULL));

    ck_assert(rf_string_init(&ns2, ""));
    ck_assert(!rf_string_to_double(&ns2, &num, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&ns);
    rf_string_deinit(&ns2);
    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_invalid_string_to_double) {
    struct RFstring s;
    ck_assert(rf_string_init(&s, "-0.02341"));

    ck_assert(!rf_string_to_double(&s, NULL, NULL));

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_to_lower) {
    struct RFstringx sx;
    struct RFstring s;
    ck_assert(rf_stringx_init(&sx, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    rf_string_to_lower(RF_STRX2STR(&sx));
    ck_assert_rf_str_eq_cstr(&sx, "abcdefghijklmnopqrstuvwxyz");

    ck_assert(rf_string_init(&s, ""));
    rf_string_to_lower(&s); /* make sure nothing bad happens in this case */

    rf_stringx_deinit(&sx);
    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_to_upper) {
    struct RFstringx sx;
    struct RFstring s;
    ck_assert(rf_stringx_init(&sx, "abcdefghijklmnopqrstuvwxyz"));
    rf_string_to_upper(RF_STRX2STR(&sx));
    ck_assert_rf_str_eq_cstr(&sx, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    ck_assert(rf_string_init(&s, ""));
    rf_string_to_upper(&s); /* make sure nothing bad happens in this case */

    rf_stringx_deinit(&sx);
    rf_string_deinit(&s);
}END_TEST

START_TEST(test_string_tokenize) {
    struct RFstringx sx;
    struct RFstring tok_space, tok_comma;
    struct RFstring* words;
    uint32_t words_num, i;
    static const char * words_arr[] = {
        "Drug", "companies", "are", "facing", "mounting", "pressure",
        "to", "investigate", "reports", "that", "new", "medicines", "are",
        "being", "tested", "on", "some", "of", "the", "poorest", "people",
        "in", "India", "without", "their", "knowledge"
    };
    //expect the sentence separated in words
    ck_assert(
        rf_stringx_init(
               &sx,
               "Drug companies are facing mounting pressure to investigate"
               " reports that new medicines are being tested on some of "
               "the poorest people in India without their knowledge")
    );
    ck_assert(rf_string_init(&tok_space, " "));
    ck_assert(rf_string_init(&tok_comma, ","));

    ck_assert(rf_string_tokenize(RF_STRX2STR(&sx), &tok_space, &words_num, &words));
    ck_assert_int_eq(words_num, sizeof(words_arr)/sizeof(char*));
    for(i = 0; i < words_num; i++) {
        ck_assert_rf_str_eq_cstr(&words[i], words_arr[i]);
    }
    //free stuff
    for(i = 0; i < words_num; i++) {
        rf_string_deinit(&words[i]);
    }
    free(words);

    rf_stringx_deinit(&sx);
    rf_string_deinit(&tok_space);
    rf_string_deinit(&tok_comma);
}END_TEST

START_TEST(test_invalid_string_tokenize) {
    struct RFstringx sx;
    struct RFstring tok_space, tok_comma;
    struct RFstring* words;
    uint32_t words_num;
    //expect the sentence separated in words
    ck_assert(
        rf_stringx_init(
               &sx,
               "Drug companies are facing mounting pressure to investigate"
               " reports that new medicines are being tested on some of "
               "the poorest people in India without their knowledge")
    );
    ck_assert(rf_string_init(&tok_space, " "));
    ck_assert(rf_string_init(&tok_comma, ","));

    /* no commas in the string */
    ck_assert(!rf_string_tokenize(RF_STRX2STR(&sx), &tok_comma ,&words_num, &words));
    /* no separator string */
    ck_assert(!rf_string_tokenize(RF_STRX2STR(&sx), NULL ,&words_num, &words));
    /* no output data */
    ck_assert(!rf_string_tokenize(RF_STRX2STR(&sx), &tok_space, NULL, NULL));

    rf_stringx_deinit(&sx);
    rf_string_deinit(&tok_space);
    rf_string_deinit(&tok_comma);
}END_TEST

START_TEST(test_string_tokenize_unicode) {
    struct RFstring s;
    struct RFstring tok;
    struct RFstring* words;
    uint32_t words_num, i;
    static const char * words_arr[] = {
        "新潟", /* Niigata */
        "富山", /* Toyama */
        "石川", /* Ishikawa */
        "福井", /* Fukui */
        "山梨", /* Yamanashi */
        "長野", /* Nagoya */
        "岐阜", /* Gifu */
        "静岡", /* Shizuoka */
        "愛知", /* Aichi */
        "青森" /* Aomori */
    };

    /*
     * expect 10 (number of prefectures) and all true string comparisons
     * marking the correct separation of Japanese prefectures by the prefecture
     * kanji '県'. Notice the last prefecture is missing its '県' kanji to not
     * get an extra empty string token
     */
    ck_assert(
        rf_string_init(
            &s,
            "新潟県富山県石川県福井県山梨県長野県岐阜県静岡"
            "県愛知県青森")
    );
    ck_assert(rf_string_init(&tok, "県"));

    ck_assert(rf_string_tokenize(&s, &tok, &words_num, &words));
    ck_assert_int_eq(words_num, sizeof(words_arr)/sizeof(char*));
    for(i = 0; i < words_num; i++) {
        ck_assert_rf_str_eq_cstr(&words[i], words_arr[i]);
    }
    //free stuff
    for(i = 0; i < words_num; i++) {
        rf_string_deinit(&words[i]);
    }
    free(words);


    rf_string_deinit(&s);
    rf_string_deinit(&tok);
}END_TEST

/* --- String Other Conversion Tests --- END --- */

Suite *string_conversion_suite_create(void)
{
    Suite *s = suite_create("String Conversion");

    TCase *string_encodings_conversions = tcase_create(
        "String Encodings Conversion"
    );
    tcase_add_checked_fixture(string_encodings_conversions,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_encodings_conversions, test_string_to_utf16);
    tcase_add_test(string_encodings_conversions, test_string_to_utf32);
    tcase_add_test(string_encodings_conversions, test_string_to_cstr);


    TCase *string_other_conversions = tcase_create("String Other Conversions");
    tcase_add_checked_fixture(string_other_conversions,
                                  setup_generic_tests,
                                  teardown_generic_tests);
    tcase_add_test(string_other_conversions, test_string_to_int);
    tcase_add_test(string_other_conversions, test_string_to_uint);
    tcase_add_test(string_other_conversions, test_string_to_double);
    tcase_add_test(string_other_conversions, test_string_to_lower);
    tcase_add_test(string_other_conversions, test_string_to_upper);
    tcase_add_test(string_other_conversions, test_string_tokenize);
    tcase_add_test(string_other_conversions, test_string_tokenize_unicode);

    TCase *string_invalid_conversions = tcase_create("String Invalid Arguments Conversion");
    tcase_add_checked_fixture(string_invalid_conversions,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(string_invalid_conversions, test_invalid_string_to_utf);
    tcase_add_test(string_invalid_conversions, test_invalid_string_to_int);
    tcase_add_test(string_invalid_conversions, test_invalid_string_to_double);
    tcase_add_test(string_invalid_conversions, test_invalid_string_tokenize);


    suite_add_tcase(s, string_encodings_conversions);
    suite_add_tcase(s, string_other_conversions);
    suite_add_tcase(s, string_invalid_conversions);
    return s;
}
