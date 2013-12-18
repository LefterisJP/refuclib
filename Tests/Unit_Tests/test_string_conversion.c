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
    
    ck_assert(!rf_string_to_utf16(&s, NULL));

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
    struct RFstring ns, ns2;
    int32_t num;
    ck_assert(rf_stringx_init(&sx, "45687"));
    ck_assert(rf_string_init(&s, "-123442"));
    ck_assert(rf_string_init(&s2, "0"));

    ck_assert(rf_string_to_int(&sx, &num));
    ck_assert_int_eq(num, 45687);

    ck_assert(rf_string_to_int(&s, &num));
    ck_assert_int_eq(num, -123442);

    ck_assert(rf_string_to_int(&s2, &num));
    ck_assert_int_eq(num, 0);

    ck_assert(!rf_string_to_int(&s2, NULL));

    ck_assert(rf_string_init(&ns, "Not a number"));
    ck_assert(!rf_string_to_int(&ns, &num));

    ck_assert(rf_string_init(&ns2, ""));
    ck_assert(!rf_string_to_int(&ns2, &num));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&ns);
    rf_string_deinit(&ns2);
    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_string_to_double) {
    struct RFstringx sx;
    struct RFstring s, s2;
    struct RFstring ns, ns2;
    double num;
    ck_assert(rf_stringx_init(&sx, "245.9310"));
    ck_assert(rf_string_init(&s, "-0.02341"));
    ck_assert(rf_string_init(&s2, "0.0"));

    ck_assert(rf_string_to_double(&sx, &num));
    ck_assert_double_eq(num, 245.9310);

    ck_assert(rf_string_to_double(&s, &num));
    ck_assert_double_eq(num, -0.02341);

    ck_assert(rf_string_to_double(&s2, &num));
    ck_assert_double_eq(num, 0.0f);

    ck_assert(!rf_string_to_int(&s2, NULL));

    ck_assert(rf_string_init(&ns, "Not a number"));
    ck_assert(!rf_string_to_double(&ns, &num));

    ck_assert(rf_string_init(&ns2, ""));
    ck_assert(!rf_string_to_double(&ns2, &num));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&ns);
    rf_string_deinit(&ns2);
    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_string_to_lower) {
    struct RFstringx sx;
    struct RFstring s;
    ck_assert(rf_stringx_init(&sx, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    rf_string_to_lower(&sx);
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
    rf_string_to_upper(&sx);
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
    
    ck_assert(rf_string_tokenize(&sx, &tok_space, &words_num, &words));
    ck_assert_int_eq(words_num, sizeof(words_arr)/sizeof(char*));
    for(i = 0; i < words_num; i++) {
        ck_assert_rf_str_eq_cstr(&words[i], words_arr[i]);
    }
    //free stuff
    for(i = 0; i < words_num; i++) {
        rf_string_deinit(&words[i]);
    }
    free(words);

    /* no commas in the string */
    ck_assert(!rf_string_tokenize(&sx, &tok_comma ,&words_num, &words));
    /* no separator string */
    ck_assert(!rf_string_tokenize(&sx, NULL ,&words_num, &words));
    /* no output data */
    ck_assert(!rf_string_tokenize(&sx, &tok_space, NULL, NULL));

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
                                  setup_string_tests,
                                  teardown_string_tests);
    tcase_add_test(string_encodings_conversions, test_string_to_utf16);
    tcase_add_test(string_encodings_conversions, test_string_to_utf32);
    tcase_add_test(string_encodings_conversions, test_string_to_cstr);


    TCase *string_other_conversions = tcase_create("String Other Conversions");
    tcase_add_checked_fixture(string_other_conversions,
                                  setup_string_tests,
                                  teardown_string_tests);
    tcase_add_test(string_other_conversions, test_string_to_int);
    tcase_add_test(string_other_conversions, test_string_to_double);
    tcase_add_test(string_other_conversions, test_string_to_lower);
    tcase_add_test(string_other_conversions, test_string_to_upper);
    tcase_add_test(string_other_conversions, test_string_tokenize);
    tcase_add_test(string_other_conversions, test_string_tokenize_unicode);


    suite_add_tcase(s, string_encodings_conversions);
    suite_add_tcase(s, string_other_conversions);
    return s;
}
