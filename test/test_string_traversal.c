#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <rflib/refu.h>
#include <rflib/string/traversalx.h>
#include <rflib/string/core.h>
#include <rflib/string/corex.h>

/* --- Stringx Traversal Tests --- START --- */

START_TEST(test_stringx_move_after) {
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT("に");
    static const struct RFstring sub2 = RF_STRING_STATIC_INIT("　");
    struct RFstringx s;
    struct RFstringx str_buff;
    struct RFstring dependent_s;
    ck_assert(rf_stringx_init_buff(&str_buff, 1024, ""));
    ck_assert(
        rf_stringx_init(
            &s,
            "中国共産党総書記に習近平氏　新指導部の７人発表")
    );
    ck_assert(RF_FAILURE != rf_stringx_move_after(&s, &sub1, &str_buff,
                                                  RF_STRINGX_ARGUMENT));

    ck_assert_rf_str_eq_cstr(&s, "習近平氏　新指導部の７人発表");
    ck_assert_rf_str_eq_cstr(&str_buff, "中国共産党総書記");

    /* dependent string */
    rf_stringx_reset(&s);
    ck_assert(RF_FAILURE != rf_stringx_move_after(&s, &sub1, &dependent_s,
                                                  RF_STRING_DEPENDENT));
    ck_assert_rf_str_eq_cstr(&s, "習近平氏　新指導部の７人発表");
    ck_assert_rf_str_eq_cstr(&dependent_s, "中国共産党総書記");

    /* no result string given */
    ck_assert(RF_FAILURE != rf_stringx_move_after(&s, &sub2, NULL,
                                                  RF_STRINGX_ARGUMENT));
    ck_assert_rf_str_eq_cstr(&s, "新指導部の７人発表");

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&str_buff);
}END_TEST

START_TEST(test_stringx_move_end) {
    struct RFstringx s;
    ck_assert(
        rf_stringx_init(
            &s,
            "中国共産党総書記に習近平氏　新指導部の７人発表")
    );
    rf_stringx_move_end(&s);

    ck_assert_rf_str_eq_cstr(&s, "");
    ck_assert(s.bIndex == strlen(
                  "中国共産党総書記に習近平氏　新指導部の７人発表")
    );

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_move_back) {
    struct RFstringx s;

    ck_assert(
        rf_stringx_init(
            &s,
            "Με την κεντρική εκδήλωση, μετά τις 2 το "
            "μεσημέρι, και την "
            "πορεία προς την αμερικανική πρεσβεία, "
            "περίπου μία ώρα "
            "αργότερα, κορυφώνονται το Σάββατο οι "
            "εκδηλώσεις για την "
            "39η επέτειο από την Εξέγερση του "
            "Πολυτεχνείου. Λόγω των "
            "εκδηλώσεων, θα ισχύσουν έκτακτες "
            "κυκλοφοριακές ρυθμίσεις "
            "έως και το πρωί της Κυριακής."
        )
    );

    rf_stringx_move_forward(&s, 96);
    ck_assert_rf_str_eq_cstr(
        &s,
        "περίπου μία ώρα αργότερα, κορυφώνονται το "
        "Σάββατο οι "
        "εκδηλώσεις για την 39η επέτειο από την Εξέγερση "
        "του Πολυτεχνείου. Λόγω των εκδηλώσεων, θα "
        "ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
        "της Κυριακής."
    );
    rf_stringx_move_back(&s, 46);
    ck_assert_rf_str_eq_cstr(
        &s,
        "και την πορεία προς την αμερικανική πρεσβεία, "
        "περίπου"
        " μία ώρα αργότερα, κορυφώνονται το Σάββατο οι "
        "εκδηλώσεις για την 39η επέτειο από την Εξέγερση "
        "του Πολυτεχνείου. Λόγω των εκδηλώσεων, θα "
        "ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και "
        "το πρωί "
        "της Κυριακής."
    );


    /* go backward more than the beginning */
    rf_stringx_move_back(&s, 999);
    ck_assert_rf_str_eq_cstr(
        &s,
        "Με την κεντρική εκδήλωση, μετά τις 2 το "
        "μεσημέρι, και την "
        "πορεία προς την αμερικανική πρεσβεία, "
        "περίπου μία ώρα "
        "αργότερα, κορυφώνονται το Σάββατο οι "
        "εκδηλώσεις για την "
        "39η επέτειο από την Εξέγερση του "
        "Πολυτεχνείου. Λόγω των "
        "εκδηλώσεων, θα ισχύσουν έκτακτες "
        "κυκλοφοριακές ρυθμίσεις "
        "έως και το πρωί της Κυριακής.");

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_move_to_index) {
    struct RFstringx s;
    unsigned int pr_index;

    ck_assert(
        rf_stringx_init(
            &s,
            "Με την κεντρική εκδήλωση, μετά τις 2 το "
            "μεσημέρι, και την "
            "πορεία προς την αμερικανική πρεσβεία, "
            "περίπου μία ώρα "
            "αργότερα, κορυφώνονται το Σάββατο οι "
            "εκδηλώσεις για την "
            "39η επέτειο από την Εξέγερση του "
            "Πολυτεχνείου. Λόγω των "
            "εκδηλώσεων, θα ισχύσουν έκτακτες "
            "κυκλοφοριακές ρυθμίσεις "
            "έως και το πρωί της Κυριακής."
        )
    );
    pr_index = s.bIndex;


    rf_stringx_move_forward(&s, 96);
    ck_assert_rf_str_eq_cstr(
        &s,
        "περίπου μία ώρα αργότερα, κορυφώνονται το "
        "Σάββατο οι "
        "εκδηλώσεις για την 39η επέτειο από την Εξέγερση "
        "του Πολυτεχνείου. Λόγω των εκδηλώσεων, θα "
        "ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
        "της Κυριακής."
    );
    rf_stringx_move_to_index(&s, pr_index);
    ck_assert_rf_str_eq_cstr(
        &s,
        "Με την κεντρική εκδήλωση, μετά τις 2 το "
        "μεσημέρι, και την "
        "πορεία προς την αμερικανική πρεσβεία, "
        "περίπου μία ώρα "
        "αργότερα, κορυφώνονται το Σάββατο οι "
        "εκδηλώσεις για την "
        "39η επέτειο από την Εξέγερση του "
        "Πολυτεχνείου. Λόγω των "
        "εκδηλώσεων, θα ισχύσουν έκτακτες "
        "κυκλοφοριακές ρυθμίσεις "
        "έως και το πρωί της Κυριακής."
    );
    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_move_forward) {
    struct RFstringx s;
    struct RFstringx s2;

    ck_assert(
        rf_stringx_init(
            &s,
            "Με την κεντρική εκδήλωση, μετά τις 2 το "
            "μεσημέρι, και την "
            "πορεία προς την αμερικανική πρεσβεία, "
            "περίπου μία ώρα "
            "αργότερα, κορυφώνονται το Σάββατο οι "
            "εκδηλώσεις για την "
            "39η επέτειο από την Εξέγερση του "
            "Πολυτεχνείου. Λόγω των "
            "εκδηλώσεων, θα ισχύσουν έκτακτες "
            "κυκλοφοριακές ρυθμίσεις "
            "έως και το πρωί της Κυριακής."
        )
    );

    rf_stringx_move_forward(&s, 96);
    ck_assert_rf_str_eq_cstr(
        &s,
        "περίπου μία ώρα αργότερα, κορυφώνονται το "
        "Σάββατο οι "
        "εκδηλώσεις για την 39η επέτειο από την Εξέγερση "
        "του Πολυτεχνείου. Λόγω των εκδηλώσεων, θα "
        "ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
        "της Κυριακής."
    );

    ck_assert(
        rf_stringx_init(&s2, "中国共産党総書記に習近平氏　新指導部の７人発表")
    );
    rf_stringx_move_forward(&s2, 3);
    ck_assert_rf_str_eq_cstr(&s2, "産党総書記に習近平氏　新指導部の７人発表");

    /* go forward more than the length */
    rf_stringx_move_forward(&s2, 999);
    ck_assert_rf_str_eq_cstr(&s2, "");

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_stringx_reset) {
    struct RFstringx s;
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT("に");
    ck_assert(
        rf_stringx_init(
            &s,
            "中国共産党総書記に習近平氏　新指導部の７人発表"
        )
    );

    /* resetting an unmoved string has no effect */
    rf_stringx_reset(&s);
    ck_assert_rf_str_eq_cstr(
        &s,
        "中国共産党総書記に習近平氏　新指導部の７人発表"
    );

    /* move and reset */
    ck_assert(RF_FAILURE != rf_stringx_move_after(&s, &sub1, NULL, 0));
    ck_assert_rf_str_eq_cstr(&s, "習近平氏　新指導部の７人発表");
    rf_stringx_reset(&s);
    ck_assert_rf_str_eq_cstr(
        &s,
        "中国共産党総書記に習近平氏　新指導部の７人発表"
    );

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_move_afterv) {
    unsigned int saved_index;
    struct RFstringx s;
    struct RFstringx str_buff;
    struct RFstring dependent_s;
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT(",");
    static const struct RFstring sub2 = RF_STRING_STATIC_INIT("39");
    ck_assert(rf_stringx_init_buff(&str_buff, 1024, ""));
    ck_assert(
        rf_stringx_init(
            &s,
            "Με την κεντρική εκδήλωση, μετά τις 2 το "
            "μεσημέρι, και την "
            "πορεία προς την αμερικανική πρεσβεία, "
            "περίπου μία ώρα "
            "αργότερα, κορυφώνονται το Σάββατο οι "
            "εκδηλώσεις για την "
            "39η επέτειο από την Εξέγερση του "
            "Πολυτεχνείου. Λόγω των "
            "εκδηλώσεων, θα ισχύσουν έκτακτες "
            "κυκλοφοριακές ρυθμίσεις "
            "έως και το πρωί της Κυριακής."
        )
    );

    rf_stringx_move_forward(&s, 96);
    ck_assert_rf_str_eq_cstr(
        &s,
        "περίπου μία ώρα αργότερα, κορυφώνονται το "
        "Σάββατο οι "
        "εκδηλώσεις για την 39η επέτειο από την Εξέγερση "
        "του Πολυτεχνείου. Λόγω των εκδηλώσεων, θα "
        "ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
        "της Κυριακής."
    );
    rf_stringx_move_back(&s, 46);
    ck_assert_rf_str_eq_cstr(
        &s,
        "και την πορεία προς την αμερικανική πρεσβεία, "
        "περίπου"
        " μία ώρα αργότερα, κορυφώνονται το Σάββατο οι "
        "εκδηλώσεις για την 39η επέτειο από την Εξέγερση "
        "του Πολυτεχνείου. Λόγω των εκδηλώσεων, θα "
        "ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και "
        "το πρωί "
        "της Κυριακής."
    );
    saved_index = s.bIndex;
    ck_assert(rf_stringx_move_afterv(&s, &str_buff, 0, 2, &sub1, &sub2));

    ck_assert_rf_str_eq_cstr(
        &s,
        " περίπου μία ώρα αργότερα, κορυφώνονται "
        "το Σάββατο οι"
        " εκδηλώσεις για την 39η επέτειο από "
        "την Εξέγερση του "
        "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και "
        "το πρωί της Κυριακής."
    );
    ck_assert_rf_str_eq_cstr(
        &str_buff,
        "και την πορεία προς την αμερικανική πρεσβεία"
    );

    /* dependent string */
    rf_stringx_move_to_index(&s, saved_index);
    ck_assert(rf_stringx_move_afterv(&s, &dependent_s,
                                     RF_STRING_DEPENDENT, 2, &sub1, &sub2));

    ck_assert_rf_str_eq_cstr(
        &s,
        " περίπου μία ώρα αργότερα, κορυφώνονται "
        "το Σάββατο οι"
        " εκδηλώσεις για την 39η επέτειο από "
        "την Εξέγερση του "
        "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
        "έκτακτες κυκλοφοριακές ρυθμίσεις έως και "
        "το πρωί της Κυριακής."
    );
    ck_assert_rf_str_eq_cstr(
        &dependent_s,
        "και την πορεία προς την αμερικανική πρεσβεία"
    );

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&str_buff);
}END_TEST

START_TEST(test_stringx_skip_chars) {
    struct RFstringx s;
    unsigned int bytes;
    unsigned int line_count;
    static const struct RFstring s1 = RF_STRING_STATIC_INIT("   \t  something");
    static const struct RFstring chars1 = RF_STRING_STATIC_INIT(" \t");
    static const struct RFstring s2 = RF_STRING_STATIC_INIT(
        " ブ ブ ラ something_else");
    static const struct RFstring chars2 = RF_STRING_STATIC_INIT(" ブラ");
    static const struct RFstring s3 = RF_STRING_STATIC_INIT(
        " \n  \n \t foo");
    static const struct RFstring chars3 = RF_STRING_STATIC_INIT(" \t\n");
    static const struct RFstring s4 = RF_STRING_STATIC_INIT(
        "not skipping \t \n \n");


    ck_assert(rf_stringx_init_buff(&s, 1024, ""));
    ck_assert(rf_stringx_assign(&s, &s1));

    ck_assert_int_eq(6, rf_stringx_skip_chars(&s, &chars1, 0, &bytes, 0));
    ck_assert_int_eq(6, bytes);
    ck_assert_rf_str_eq_cstr(&s, "something");

    ck_assert(rf_stringx_assign(&s, &s2));
    ck_assert_int_eq(7, rf_stringx_skip_chars(&s, &chars2, 0, &bytes, 0));
    ck_assert_int_eq(13, bytes);
    ck_assert_rf_str_eq_cstr(&s, "something_else");

    ck_assert(rf_stringx_assign(&s, &s3));
    ck_assert_int_eq(8, rf_stringx_skip_chars(&s, &chars3, 0,
                                              &bytes, &line_count));
    ck_assert_int_eq(8, bytes);
    ck_assert_int_eq(2, line_count);
    ck_assert_rf_str_eq_cstr(&s, "foo");

    ck_assert(rf_stringx_assign(&s, &s3));
    ck_assert_int_eq(3, rf_stringx_skip_chars(&s, &chars3,
                                              rf_string_data(&s) + 2,
                                              &bytes, &line_count));
    ck_assert_int_eq(3, bytes);
    ck_assert_int_eq(1, line_count);
    ck_assert_rf_str_eq_cstr(&s," \n \t foo");

    ck_assert(rf_stringx_assign(&s, &s3));
    ck_assert_int_eq(1, rf_stringx_skip_chars(&s, &chars3,
                                              rf_string_data(&s),
                                              &bytes, &line_count));
    ck_assert_int_eq(1, bytes);
    ck_assert_int_eq(0, line_count);
    ck_assert_rf_str_eq_cstr(&s,"\n  \n \t foo");

    ck_assert(rf_stringx_assign(&s, &s4));
    ck_assert_int_eq(0, rf_stringx_skip_chars(&s, &chars3,
                                              0, &bytes, &line_count));
    ck_assert_int_eq(0, bytes);
    ck_assert_int_eq(0, line_count);

    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_move_after_pair) {
    struct RFstringx s;
    struct RFstring res_str_good;
    struct RFstring res_str_bad;
    struct RFstring dependent_s;
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT("「");
    static const struct RFstring sub2 = RF_STRING_STATIC_INIT("」");
    static const struct RFstring sub3 = RF_STRING_STATIC_INIT("eleos");
    static const struct RFstring sub4 = RF_STRING_STATIC_INIT("notthere");

    ck_assert(
        rf_stringx_init(
            &s,
            "これがMoveAfterPairのテストですね！「Δεν θα επιστραφω εγω"
            "」　Γεμιζουμε το στρινγκ "
            "με διαφορα 「ブラケットの中のテ"
            "キストは結果になる」Let's see if the function will work "
            "as expected."
        )
    );


    ck_assert(
        rf_stringx_move_after_pair(&s, &sub1, &sub2, &res_str_good, 0, 2)
    );
    ck_assert_rf_str_eq_cstr(
        &s, "Let's see if the function will work as expected."
    );
    ck_assert_rf_str_eq_cstr(
        &res_str_good, "ブラケットの中のテキストは結果になる"
    );

    /* dependent string */
    rf_stringx_reset(&s);
    ck_assert(
        rf_stringx_move_after_pair(&s, &sub1, &sub2,
                                   &dependent_s, RF_STRING_DEPENDENT, 2)
    );
    ck_assert_rf_str_eq_cstr(
        &s, "Let's see if the function will work as expected."
    );
    ck_assert_rf_str_eq_cstr(
        &dependent_s, "ブラケットの中のテキストは結果になる"
    );

    /* non existing substrings */
    ck_assert(!rf_stringx_move_after_pair(&s, &sub3, &sub4, &res_str_bad, 0, 2));

    rf_stringx_deinit(&s);
    rf_string_deinit(&res_str_good);
    rf_string_deinit(&dependent_s);
}END_TEST

START_TEST(test_invalid_stringx_traversal) {
    static const struct RFstring sub3 = RF_STRING_STATIC_INIT("ain't there");
    static const struct RFstring sub4 = RF_STRING_STATIC_INIT("notthere");
    struct RFstringx s;
    struct RFstringx str_buff;
    struct RFstring res_str_bad;
    ck_assert(rf_stringx_init_buff(&str_buff, 1024, ""));
    ck_assert(
        rf_stringx_init(
            &s,
            "中国共産党総書記に習近平氏　新指導部の７人発表")
    );

    ck_assert(RF_FAILURE == rf_stringx_move_after(&s, NULL, &str_buff,
                                                  RF_STRINGX_ARGUMENT));
    ck_assert(RF_FAILURE == rf_stringx_move_after(&s, &sub3, &str_buff,
                                                  RF_STRINGX_ARGUMENT));
    ck_assert(!rf_stringx_move_afterv(&s, NULL, 0, 2, &sub3, &sub4));
    ck_assert(!rf_stringx_move_after_pair(&s, NULL, &sub3, &res_str_bad, 0, 2));
    ck_assert(!rf_stringx_move_after_pair(&s, &sub4, NULL, &res_str_bad, 0, 2));

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&str_buff);
}END_TEST

Suite *string_traversal_suite_create(void)
{
    Suite *s = suite_create("String Traversal");

    TCase *stringx_traversal = tcase_create(
        "String Traversal Tests"
    );
    tcase_add_checked_fixture(stringx_traversal,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(stringx_traversal, test_stringx_move_after);
    tcase_add_test(stringx_traversal, test_stringx_move_end);
    tcase_add_test(stringx_traversal, test_stringx_move_back);
    tcase_add_test(stringx_traversal, test_stringx_move_to_index);
    tcase_add_test(stringx_traversal, test_stringx_move_forward);
    tcase_add_test(stringx_traversal, test_stringx_reset);
    tcase_add_test(stringx_traversal, test_stringx_move_afterv);
    tcase_add_test(stringx_traversal, test_stringx_skip_chars);
    tcase_add_test(stringx_traversal, test_stringx_move_after_pair);

    TCase *invalid_stringx_traversal = tcase_create(
        "Invalid Argument String Traversal Tests"
    );
    tcase_add_checked_fixture(invalid_stringx_traversal,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(invalid_stringx_traversal, test_invalid_stringx_traversal);

    suite_add_tcase(s, stringx_traversal);
    suite_add_tcase(s, invalid_stringx_traversal);
    return s;
}
