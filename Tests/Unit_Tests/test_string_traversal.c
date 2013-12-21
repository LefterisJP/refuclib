#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <refu.h>
#include <String/rf_str_traversalx.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>


/* --- Stringx Traversal Tests --- START --- */

START_TEST(test_stringx_move_after) {
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT("に");
    static const struct RFstring sub2 = RF_STRING_STATIC_INIT("　");
    static const struct RFstring sub3 = RF_STRING_STATIC_INIT("ain't there");
    struct RFstringx s;
    struct RFstringx str_buff;
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

    /* no result string given */
    ck_assert(RF_FAILURE != rf_stringx_move_after(&s, &sub2, NULL,
                                                  RF_STRINGX_ARGUMENT));
    ck_assert_rf_str_eq_cstr(&s, "新指導部の７人発表");
    /* invalid input */
    ck_assert(RF_FAILURE == rf_stringx_move_after(&s, NULL, &str_buff,
                                                  RF_STRINGX_ARGUMENT));
    ck_assert(RF_FAILURE == rf_stringx_move_after(&s, &sub3, &str_buff,
                                                  RF_STRINGX_ARGUMENT));

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&str_buff);
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
    struct RFstringx s;
    struct RFstringx str_buff;
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT(",");
    static const struct RFstring sub2 = RF_STRING_STATIC_INIT("39");
    static const struct RFstring sub3 = RF_STRING_STATIC_INIT("eleos");
    static const struct RFstring sub4 = RF_STRING_STATIC_INIT("notthere");
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

    /* non existing substrings and null return */
    ck_assert(!rf_stringx_move_afterv(&s, NULL, 0, 2, &sub3, &sub4));

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&str_buff);
}END_TEST

START_TEST(test_stringx_move_after_pair) {
    struct RFstringx s;
    struct RFstringx str_buff;
    static const struct RFstring sub1 = RF_STRING_STATIC_INIT("「");
    static const struct RFstring sub2 = RF_STRING_STATIC_INIT("」");
    static const struct RFstring sub3 = RF_STRING_STATIC_INIT("eleos");
    static const struct RFstring sub4 = RF_STRING_STATIC_INIT("notthere");

    ck_assert(rf_stringx_init_buff(&str_buff, 1024, ""));
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
        rf_stringx_move_after_pair(&s, &sub1, &sub2, &str_buff, 0, 2)
    );
    ck_assert_rf_str_eq_cstr(
        &s, "Let's see if the function will work as expected."
    );
    ck_assert_rf_str_eq_cstr(
        &str_buff, "ブラケットの中のテキストは結果になる"
    );

    /* non existing substrings */
    ck_assert(!rf_stringx_move_after_pair(&s, &sub3, &sub4, &str_buff, 0, 2));
    /* invalid input */
    ck_assert(!rf_stringx_move_after_pair(&s, NULL, &sub2, &str_buff, 0, 2));
    ck_assert(!rf_stringx_move_after_pair(&s, &sub1, NULL, &str_buff, 0, 2));

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
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(stringx_traversal, test_stringx_move_after);
    tcase_add_test(stringx_traversal, test_stringx_move_back);
    tcase_add_test(stringx_traversal, test_stringx_move_forward);
    tcase_add_test(stringx_traversal, test_stringx_reset);
    tcase_add_test(stringx_traversal, test_stringx_move_afterv);
    tcase_add_test(stringx_traversal, test_stringx_move_after_pair);



    suite_add_tcase(s, stringx_traversal);
    return s;
}
