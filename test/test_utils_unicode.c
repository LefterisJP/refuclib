#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <refu.h>
#include <Utils/rf_unicode.h>

/* --- UTF8 encoding Tests --- START --- */
START_TEST(test_utf8_encode) {
    uint32_t byte_length;
    uint32_t codepoints[] = {0x307E, 0x305F, 0x7C73, 0x5175, 0x304C,
                             0x6C96, 0x7E04, 0x306E, 0x4EBA
    };
    uint32_t codepoints2[] = {};
    char utf8[50];


    ck_assert(rf_utf8_encode(codepoints, 9, &byte_length, utf8, 50));
    ck_assert_nnt_str_eq_cstr(utf8, "ã¾ãŸç±³å…µãŒæ²–ç¸„ã®äºº");
    ck_assert_uint_eq(byte_length, 27);

    /* empy codepoints */
    ck_assert(rf_utf8_encode(codepoints2, 0, &byte_length, utf8, 50));
    ck_assert_nnt_str_eq_cstr(utf8, "ã¾ãŸç±³å…µãŒæ²–ç¸„ã®äºº"); /* no change */
    ck_assert_uint_eq(byte_length, 0);

    /* no invalid input tests, user should make sure
     * these functions are not
     * called with null pointers
     */

}END_TEST

START_TEST(test_utf8_encode_single) {
    char utf8[50];

    /* ASCII 'z' */
    ck_assert_int_eq(1, (rf_utf8_encode_single(0x7A, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "z");
    /* Greek 'Ï†' */
    ck_assert_int_eq(2, (rf_utf8_encode_single(0x3c6, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "Ï†");
    /* CJK 'è¦' */
    ck_assert_int_eq(3, (rf_utf8_encode_single(0x8981, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "è¦");
    /* Character 'ğŒˆ' from the SMP (Supplementrary Miltilingual Plane) */
    ck_assert_int_eq(4, (rf_utf8_encode_single(0x10308, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "ğŒˆ");


    /* no invalid input tests, user should make sure
     * these functions are not
     * called with null pointers
     */
}END_TEST

START_TEST(test_utf8_decode) {
    static const char utf8[] = "ã¾ãŸç±³å…µãŒæ²–ç¸„ã®äºº";
    uint32_t expected_codepoints[] = {0x307E, 0x305F, 0x7C73, 0x5175, 0x304C,
                                      0x6C96, 0x7E04, 0x306E, 0x4EBA
    };
    uint32_t codepoints_number;
    uint32_t codepoints_buff[9];
    unsigned int i;

    ck_assert(rf_utf8_decode(utf8, 27, &codepoints_number,
                             codepoints_buff, 9*sizeof(uint32_t)));
    ck_assert_uint_eq(codepoints_number, 9);
    for (i = 0; i < 9; i ++) {
        ck_assert_uint_eq(codepoints_buff[i], expected_codepoints[i]);
    }


    /* no invalid input tests, user should make sure
     * these functions are not
     * called with null pointers
     */
}END_TEST

START_TEST(test_utf8_verify_cstr) {
    /* some valid utf8 sequences*/
    static const char utf8_str_1[] = "ã¾ãŸç±³å…µãŒæ²–ç¸„ã®äºº";
    static const char utf8_str_2[] = "Ascii characters";
    static const char utf8_str_3[] = "Î— Ï‡ÏÏÎ± ÎºÏÎ­Î¼ÎµÏ„Î±Î¹ Ï€Î¬Î»Î¹ Î±Ï€ÏŒ Î¼Î¹Î± ÎºÎ»Ï‰ÏƒÏ„Î®.";
    uint32_t byte_length;

    ck_assert(rf_utf8_verify(utf8_str_1, &byte_length, 0));
    ck_assert_uint_eq(byte_length, 27);

    ck_assert(rf_utf8_verify(utf8_str_2, &byte_length, 0));
    ck_assert_uint_eq(byte_length, 16);

    ck_assert(rf_utf8_verify(utf8_str_3, &byte_length, 0));
    ck_assert_uint_eq(byte_length, 65);

}END_TEST


 /*
  * UTF8 decoding stress TESTS - START
  * http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
  */
#define ck_assert_valid_utf8(i_stream_)                               \
    ck_assert(rf_utf8_verify(i_stream_, NULL, sizeof(i_stream_) - 1))

#define ck_assert_invalid_utf8(i_stream_)                               \
    ck_assert(!rf_utf8_verify(i_stream_, NULL, sizeof(i_stream_) - 1))

 /* UTF8 boundary byte streams */
START_TEST(test_boundary_utf8_first_possible_sequence_of_certain_length) {
    static const char b1[] = {"\0"};
    static const char b2[] = {"Â€"};
    static const char b3[] = {"à €"};
    static const char b4[] = {"ğ€€"};
    static const char b5[] = {"øˆ€€€"};
    static const char b6[] = {"ü„€€€€"};

    ck_assert_valid_utf8(b1);
    ck_assert_valid_utf8(b2);
    ck_assert_valid_utf8(b3);
    ck_assert_valid_utf8(b4);
    /* 5 and 6 byte encodings are not valid according to RFC 3629 */
    ck_assert_invalid_utf8(b5);
    ck_assert_invalid_utf8(b6);
}END_TEST

START_TEST(test_boundary_utf8_last_possible_sequence_of_certain_length) {
    static const char b1[] = {""};
    static const char b2[] = {"ß¿"};
    static const char b3[] = {"ï¿¿"};
    static const char b4[] = {"÷¿¿¿"};
    static const char b5[] = {"û¿¿¿¿"};
    static const char b6[] = {"ı¿¿¿¿¿"};

    ck_assert_valid_utf8(b1);
    ck_assert_valid_utf8(b2);
    /* valid but resulting codepoint invalid*/
    ck_assert_invalid_utf8(b3);
    /* all the rest could be validly encoded but are greater than the maximum */
    ck_assert_invalid_utf8(b4);
    ck_assert_invalid_utf8(b5);
    ck_assert_invalid_utf8(b6);
}END_TEST

START_TEST(test_boundary_utf8_other) {
    static const char b1[] = {"íŸ¿"};
    static const char b2[] = {"î€€"};
    static const char b3[] = {"ï¿½"};
    static const char b4[] = {"ô¿¿"};
    static const char b5[] = {"ô€€"};

    ck_assert_valid_utf8(b1);
    ck_assert_valid_utf8(b2);
    ck_assert_valid_utf8(b3);
    ck_assert_valid_utf8(b4);
    /* validly encoded but greater than the maximum */
    ck_assert_invalid_utf8(b5);
}END_TEST

/* UTF8 malformed sequences */

START_TEST(test_malformed_utf8_unexpected_continuation_bytes){
    static const char first_cb[] = {"€"};
    static const char last_cb [] = {"¿"};
    static const char cb_2[] = {"€¿"};
    static const char cb_3[] = {"€¿€"};
    static const char cb_4[] = {"€¿€¿"};
    static const char cb_5[] = {"€¿€¿€"};
    static const char cb_6[] = {"€¿€¿€¿"};
    static const char cb_7[] = {"€¿€¿€¿€"};
    static const char cb_all[] = {
        "€‚ƒ„…†‡ˆ‰Š‹Œ"
        "‘’“”•–—˜™š›œŸ"
        " ¡¢£¤¥¦§¨©ª«¬­®¯"
        "°±²³´µ¶·¸¹º»¼½¾¿"
    };

   ck_assert_invalid_utf8(first_cb);
   ck_assert_invalid_utf8(last_cb);
   ck_assert_invalid_utf8(cb_2);
   ck_assert_invalid_utf8(cb_3);
   ck_assert_invalid_utf8(cb_4);
   ck_assert_invalid_utf8(cb_5);
   ck_assert_invalid_utf8(cb_6);
   ck_assert_invalid_utf8(cb_7);
   ck_assert_invalid_utf8(cb_all);
}END_TEST

START_TEST(test_malformed_utf8_lonely_start){
    /* 
     * All 32 first bytes of 2-byte sequences (0xc0-0xdf),
     * each followed by a space character
    */
    static const char case1[] = {
        "À Á Â Ã Ä Å Æ Ç È É Ê Ë Ì Í "
        "Î Ï Ğ Ñ Ò Ó Ô Õ Ö × Ø Ù Ú Û "
        "Ü İ Ş ß "
    };
    /*
     * All 16 first bytes of 3-byte sequences (0xe0-0xef),
     * each followed by a space character
     */
    static const char case2[] = {
        "à á â ã ä å æ ç è é ê ë "
        "ì í î ï "
    };
    /*
     * All 8 first bytes of 4-byte sequences (0xf0-0xf7),
     * each followed by a space character
     */
    static const char case3[] = {
        "ğ ñ ò ó ô õ ö ÷ "
    };
    /*
     * All 4 first bytes of 5-byte sequences (0xf8-0xfb),
     * each followed by a space character
     */
    static const char case4[] = {"ø ù ú û "};
    /*
     * All 2 first bytes of 6-byte sequences (0xfc-0xfd),
     * each followed by a space character
     */
    static const char case5[] = {"ü ı "};

   ck_assert_invalid_utf8(case1);
   ck_assert_invalid_utf8(case2);
   ck_assert_invalid_utf8(case3);
   ck_assert_invalid_utf8(case4);
   ck_assert_invalid_utf8(case5);
}END_TEST

START_TEST(test_malformed_utf8_last_continuation_byte_missing){
    /* 2-byte sequence with last byte missing (U+0000) */
    static const char case1[] = {"À"};
    /* 3-byte sequence with last byte missing (U+0000) */
    static const char case2[] = {"à€"};
    /* 4-byte sequence with last byte missing (U+0000) */
    static const char case3[] = {"ğ€€"};
    /* 5-byte sequence with last byte missing (U+0000) */
    static const char case4[] = {"ø€€€"};
    /* 6-byte sequence with last byte missing (U+0000) */
    static const char case5[] = {"ü€€€€"};
    /* 2-byte sequence with last byte missing (U-000007FF) */
    static const char case6[] = {"ß"};
    /* 3-byte sequence with last byte missing (U-0000FFFF) */
    static const char case7[] = {"ï¿"};
    /* 4-byte sequence with last byte missing (U-001FFFFF) */
    static const char case8[] = {"÷¿¿"};
    /* 5-byte sequence with last byte missing (U-03FFFFFF) */
    static const char case9[] = {"û¿¿¿"};
    /* 6-byte sequence with last byte missing (U-7FFFFFFF) */
    static const char case10[] = {"ı¿¿¿¿"};


   ck_assert_invalid_utf8(case1);
   ck_assert_invalid_utf8(case2);
   ck_assert_invalid_utf8(case3);
   ck_assert_invalid_utf8(case4);
   ck_assert_invalid_utf8(case5);
   ck_assert_invalid_utf8(case6);
   ck_assert_invalid_utf8(case7);
   ck_assert_invalid_utf8(case8);
   ck_assert_invalid_utf8(case9);
   ck_assert_invalid_utf8(case10);
}END_TEST

START_TEST(test_malformed_utf8_incomplete_sequence_concatenation){
    /* 
     * All the 10 sequences of 3.3 concatenated, you should see 10 malformed
     * sequences being signalled
     */
    static const char case1[] = {
        "Àà€ğ€€ø€€€ü€€€€"
        "ßï¿÷¿¿û¿¿¿ı¿¿¿¿"
    };

   ck_assert_invalid_utf8(case1);
}END_TEST

START_TEST(test_malformed_utf8_impossible_bytes){
    /* The following 2 bytes can't appear in a UTF-8 string (reserved for BOM) */
    static const char fe[] = {"ş"};
    static const char ff[] = {"ÿ"};
    static const char fefe_ffff[] = {"şşÿÿ"};


   ck_assert_invalid_utf8(fe);
   ck_assert_invalid_utf8(ff);
   ck_assert_invalid_utf8(fefe_ffff);
}END_TEST


 /* --- UTF8 overlong sequences Tests --- START --- */
START_TEST(test_overlong_utf8_ascii) {
    static const char case1[] = {"À¯"};
    static const char case2[] = {"à€¯"};
    static const char case3[] = {"ğ€€¯"};
    static const char case4[] = {"ø€€€¯"};
    static const char case5[] = {"ü€€€€¯"};

    ck_assert_invalid_utf8(case1);
    ck_assert_invalid_utf8(case2);
    ck_assert_invalid_utf8(case3);
    ck_assert_invalid_utf8(case4);
    ck_assert_invalid_utf8(case5);
}END_TEST

START_TEST(test_maximum_overlong_utf8_sequence) {
    static const char case1[] = {"Á¿"};
    static const char case2[] = {"àŸ¿"};
    static const char case3[] = {"ğ¿¿"};
    static const char case4[] = {"ø‡¿¿¿"};
    static const char case5[] = {"üƒ¿¿¿¿"};

    ck_assert_invalid_utf8(case1);
    ck_assert_invalid_utf8(case2);
    ck_assert_invalid_utf8(case3);
    ck_assert_invalid_utf8(case4);
    ck_assert_invalid_utf8(case5);
}END_TEST

START_TEST(test_utf8_overlong_null) {
    static const char case1[] = {"À€"};
    static const char case2[] = {"à€€"};
    static const char case3[] = {"ğ€€€"};
    static const char case4[] = {"ø€€€€"};
    static const char case5[] = {"ü€€€€€"};

    ck_assert_invalid_utf8(case1);
    ck_assert_invalid_utf8(case2);
    ck_assert_invalid_utf8(case3);
    ck_assert_invalid_utf8(case4);
    ck_assert_invalid_utf8(case5);
}END_TEST


/* --- UTF8 illegal code positions Tests --- START --- */
/*
 * The following UTF-8 sequences should be rejected like malformed
 * sequences, because they never represent valid ISO 10646 characters and
 * a UTF-8 decoder that accepts them might introduce security problems
 * comparable to overlong UTF-8 sequences.
 */
START_TEST(test_utf8_illegal_single_utf16_surrogate) {
    static const char case1[] = {"í €"};
    static const char case2[] = {"í­¿"};
    static const char case3[] = {"í®€"};
    static const char case4[] = {"í¯¿"};
    static const char case5[] = {"í°€"};
    static const char case6[] = {"í¾€"};
    static const char case7[] = {"í¿¿"};


    ck_assert_invalid_utf8(case1);
    ck_assert_invalid_utf8(case2);
    ck_assert_invalid_utf8(case3);
    ck_assert_invalid_utf8(case4);
    ck_assert_invalid_utf8(case5);
    ck_assert_invalid_utf8(case6);
    ck_assert_invalid_utf8(case7);
}END_TEST

START_TEST(test_utf8_illegal_paired_utf16_surrogate) {
    static const char case1[] = {"í €í°€"};
    static const char case2[] = {"í €í¿¿"};
    static const char case3[] = {"í­¿í°€"};
    static const char case4[] = {"í­¿í¿¿"};
    static const char case5[] = {"í®€í°€"};
    static const char case6[] = {"í®€í¿¿"};
    static const char case7[] = {"í¯¿í°€"};
    static const char case8[] = {"í¯¿í¿¿"};

    ck_assert_invalid_utf8(case1);
    ck_assert_invalid_utf8(case2);
    ck_assert_invalid_utf8(case3);
    ck_assert_invalid_utf8(case4);
    ck_assert_invalid_utf8(case5);
    ck_assert_invalid_utf8(case6);
    ck_assert_invalid_utf8(case7);
    ck_assert_invalid_utf8(case8);
}END_TEST

START_TEST(test_utf8_illegal_other) {
    /* U+FFFE */
    static const char case1[] = {"ï¿¾"};
    /* U+FFFF */
    static const char case2[] = {"ï¿¿"};
    /* code_point greater than the max codepoint which is: 0x10ffff */
    static const char case3[] = {"\xf4\x9f\xbf\xbf"};
    /* codepoint in illegal range 0xfdd0-0xfdef */
    static const char case4[] = {"\xEF\xB7\x90"};
    static const char case5[] = {"\xEF\xB7\xAF"};
    /* codepoint in illegal range 0xfffe-0xffff */
    static const char case6[] = {"\xEF\xBF\xBE"};
    static const char case7[] = {"\xEF\xBF\xBF"};

    ck_assert_invalid_utf8(case1);
    ck_assert_invalid_utf8(case2);
    ck_assert_invalid_utf8(case3);
    ck_assert_invalid_utf8(case4);
    ck_assert_invalid_utf8(case5);
    ck_assert_invalid_utf8(case6);
    ck_assert_invalid_utf8(case7);
}END_TEST

/* --- UTF16 encoding Tests --- START --- */
START_TEST(test_utf16_decode) {
    /* Japanese(Adachiku) + MusicalSymbol(G clef) */
    uint16_t utf16[] = {0x8DB3, 0x7ACB, 0x533A, 0xD834, 0xDD1E};
    uint32_t codepoints_number;
    uint32_t codepoints[20];
    uint32_t expected_codepoints[] = {0x8DB3, 0x7ACB, 0x533A, 0x1D11E};
    int i;

    ck_assert(rf_utf16_decode((char*)utf16, sizeof(uint16_t) * 5,
                              &codepoints_number, codepoints, 50));
    ck_assert_uint_eq(codepoints_number, 4);
    for (i = 0; i < 4; i++) {
        ck_assert_uint_eq(codepoints[i], expected_codepoints[i]);
    }

    /* no invalid input tests, user should make sure
     * these functions are not
     * called with null pointers
     */
}END_TEST

START_TEST(test_utf16_encode) {
    uint32_t codepoints[] = {0x8DB3, 0x7ACB, 0x533A, 0x1D11E};
    uint32_t utf16_length;
    uint16_t utf16[20];
    uint16_t expected_utf16[] = {0x8DB3, 0x7ACB, 0x533A, 0xD834, 0xDD1E};
    int i;

    ck_assert(rf_utf16_encode(codepoints, 4, &utf16_length, utf16, 20));
    ck_assert_uint_eq(utf16_length, 5);
    for (i = 0; i < 5; i++) {
        ck_assert_uint_eq(utf16[i], expected_utf16[i]);
    }

    /* no invalid input tests, user should make sure
     * these functions are not
     * called with null pointers
     */
}END_TEST


Suite *utils_unicode_suite_create(void)
{
    Suite *s = suite_create("Unicode encodings");

    TCase *unicode_utf8 = tcase_create("UTF8 encoding");
    tcase_add_checked_fixture(unicode_utf8,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(unicode_utf8, test_utf8_encode);
    tcase_add_test(unicode_utf8, test_utf8_encode_single);
    tcase_add_test(unicode_utf8, test_utf8_decode);
    tcase_add_test(unicode_utf8, test_utf8_verify_cstr);

    TCase *boundary_utf8_encoding = tcase_create("UTF8 encoding "
                                                 "boundary conditions");
    tcase_add_checked_fixture(boundary_utf8_encoding,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(
        boundary_utf8_encoding,
        test_boundary_utf8_first_possible_sequence_of_certain_length);
    tcase_add_test(
        boundary_utf8_encoding,
        test_boundary_utf8_last_possible_sequence_of_certain_length);
    tcase_add_test(boundary_utf8_encoding, test_boundary_utf8_other);

    TCase *malformed_utf8_encoding = tcase_create("UTF8 encoding "
                                                  "malformed streams");
    tcase_add_checked_fixture(malformed_utf8_encoding,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(
        malformed_utf8_encoding,
        test_malformed_utf8_unexpected_continuation_bytes
    );
    tcase_add_test(
        malformed_utf8_encoding,
        test_malformed_utf8_lonely_start
    );
    tcase_add_test(
        malformed_utf8_encoding,
        test_malformed_utf8_last_continuation_byte_missing
    );
    tcase_add_test(
        malformed_utf8_encoding,
        test_malformed_utf8_incomplete_sequence_concatenation
    );
    tcase_add_test(
        malformed_utf8_encoding,
        test_malformed_utf8_impossible_bytes
    );

    TCase *overlong_utf8_encoding = tcase_create("UTF8 encoding "
                                                  "overlong sequences");
    tcase_add_checked_fixture(overlong_utf8_encoding,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(overlong_utf8_encoding, test_overlong_utf8_ascii);
    tcase_add_test(overlong_utf8_encoding, test_maximum_overlong_utf8_sequence);
    tcase_add_test(overlong_utf8_encoding, test_utf8_overlong_null);

    TCase *illegal_code_position_utf8_encoding = tcase_create(
        "UTF8 encoding "
        "illegal code positions");
    tcase_add_checked_fixture(illegal_code_position_utf8_encoding,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(illegal_code_position_utf8_encoding,
                   test_utf8_illegal_single_utf16_surrogate);
    tcase_add_test(illegal_code_position_utf8_encoding,
                   test_utf8_illegal_paired_utf16_surrogate);
    tcase_add_test(illegal_code_position_utf8_encoding,
                   test_utf8_illegal_other);

    TCase *unicode_utf16 = tcase_create("UTF16 encoding");
    tcase_add_checked_fixture(unicode_utf16,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(unicode_utf16, test_utf16_decode);
    tcase_add_test(unicode_utf16, test_utf16_encode);


    suite_add_tcase(s, unicode_utf8);
    suite_add_tcase(s, boundary_utf8_encoding);
    suite_add_tcase(s, malformed_utf8_encoding);
    suite_add_tcase(s, overlong_utf8_encoding);
    suite_add_tcase(s, illegal_code_position_utf8_encoding);

    suite_add_tcase(s, unicode_utf16);
    return s;
}
