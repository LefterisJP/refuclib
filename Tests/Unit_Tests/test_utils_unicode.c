#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"


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
    ck_assert_nnt_str_eq_cstr(utf8, "また米兵が沖縄の人");
    ck_assert_uint_eq(byte_length, 27);

    /* empy codepoints */
    ck_assert(rf_utf8_encode(codepoints2, 0, &byte_length, utf8, 50));
    ck_assert_nnt_str_eq_cstr(utf8, "また米兵が沖縄の人"); /* no change */
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
    /* Greek 'φ' */
    ck_assert_int_eq(2, (rf_utf8_encode_single(0x3c6, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "φ");
    /* CJK '要' */
    ck_assert_int_eq(3, (rf_utf8_encode_single(0x8981, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "要");
    /* Character '𐌈' from the SMP (Supplementrary Miltilingual Plane) */
    ck_assert_int_eq(4, (rf_utf8_encode_single(0x10308, utf8)));
    ck_assert_nnt_str_eq_cstr(utf8, "𐌈");


    /* no invalid input tests, user should make sure
     * these functions are not
     * called with null pointers
     */
}END_TEST

START_TEST(test_utf8_decode) {
    static const char utf8[] = "また米兵が沖縄の人";
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
    static const char utf8_str_1[] = "また米兵が沖縄の人";
    static const char utf8_str_2[] = "Ascii characters";
    static const char utf8_str_3[] = "Η χώρα κρέμεται πάλι από μια κλωστή.";
    uint32_t byte_length;

    ck_assert(rf_utf8_verify_cstr(utf8_str_1, &byte_length));
    ck_assert_uint_eq(byte_length, 27);

    ck_assert(rf_utf8_verify_cstr(utf8_str_2, &byte_length));
    ck_assert_uint_eq(byte_length, 16);

    ck_assert(rf_utf8_verify_cstr(utf8_str_3, &byte_length));
    ck_assert_uint_eq(byte_length, 65);

    /* invalid utf8 sequences:
     * http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
     */


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
    tcase_add_test(unicode_utf8, test_utf8_encode);
    tcase_add_test(unicode_utf8, test_utf8_encode_single);
    tcase_add_test(unicode_utf8, test_utf8_decode);
    tcase_add_test(unicode_utf8, test_utf8_verify_cstr);



    TCase *unicode_utf16 = tcase_create("UTF16 encoding");
    tcase_add_test(unicode_utf16, test_utf16_decode);
    tcase_add_test(unicode_utf16, test_utf16_encode);

    suite_add_tcase(s, unicode_utf8);
    suite_add_tcase(s, unicode_utf16);
    return s;
}
