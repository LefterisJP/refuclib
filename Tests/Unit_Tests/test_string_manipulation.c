#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <refu.h>
#include <String/rf_str_manipulation.h>
#include <String/rf_str_manipulationx.h>
#include <String/rf_str_traversalx.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>


/* --- String Appending Tests --- START --- */
START_TEST(test_string_append) {
    struct RFstring s, s2, s3;
    struct RFstring a1 = RF_STRING_STATIC_INIT(" is doing what you love");
    struct RFstring a2 = RF_STRING_STATIC_INIT(". Unicode string "
                                               "研究室に行ってきます。");

    ck_assert(rf_string_init(&s, "The meaning of life"));
    ck_assert(rf_string_append(&s, &a1));
    ck_assert_rf_str_eq_cstr(&s, "The meaning of life is doing what you love");

    ck_assert(rf_string_init(&s2, "A very interesting"));
    ck_assert(rf_string_initv(&s3, " %s is %s and its value is %.4f",
                             "number",
                             "the golden mean",
                             1.6180339)
    );
    ck_assert(rf_string_append(&s2, &s3));
    ck_assert(rf_string_append(&s2, &a2));
    ck_assert_rf_str_eq_cstr(&s2, "A very interesting number is the golden mean "
                             "and its value is 1.6180. Unicode string "
                             "研究室に行ってきます。");

    /* invalid input */
    ck_assert(!rf_string_append(&s, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_string_append_int) {
    struct RFstring s, s2, s3;

    ck_assert(rf_string_init(&s, "My favorite number is "));
    ck_assert(rf_string_init(&s2, "My favorite number is "));
    ck_assert(rf_string_init(&s3, "My favorite number is "));

    ck_assert(rf_string_append_int(&s, 347));
    ck_assert_rf_str_eq_cstr(&s, "My favorite number is 347");

    ck_assert(rf_string_append_int(&s2, -1456));
    ck_assert_rf_str_eq_cstr(&s2, "My favorite number is -1456");

    ck_assert(rf_string_append_int(&s3, 0));
    ck_assert_rf_str_eq_cstr(&s3, "My favorite number is 0");

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_string_append_double) {
    struct RFstring s, s2, s3;

    ck_assert(rf_string_init(&s, "My favorite floating point number is "));
    ck_assert(rf_string_init(&s2, "My favorite floating point number is "));
    ck_assert(rf_string_init(&s3, "My favorite floating point number is "));

    ck_assert(rf_string_append_double(&s, 0.00341));
    ck_assert_rf_str_eq_cstr(&s, "My favorite floating point number is 0.00341");

    ck_assert(rf_string_append_double(&s2, -123.193));
    ck_assert_rf_str_eq_cstr(&s2, "My favorite floating point number is -123.193");

    ck_assert(rf_string_append_double(&s3, 0.0));
    ck_assert_rf_str_eq_cstr(&s3, "My favorite floating point number is 0.0");

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_string_prepend) {
    struct RFstring s, s2;
    struct RFstring p1 = RF_STRING_STATIC_INIT("日本の首都は");
    struct RFstring p2 = RF_STRING_STATIC_INIT("Δεύτερη θητεία");
    struct RFstring a1 = RF_STRING_STATIC_INIT("だと思います。");

    ck_assert(rf_string_init(&s, "東京"));
    ck_assert(rf_string_initv(&s2, "ちなみに今日は%d日です。",26));

    ck_assert(rf_string_prepend(&s, &p1));
    ck_assert(rf_string_append(&s, &a1));
    ck_assert(rf_string_append(&s, &s2));
    ck_assert(rf_string_prepend(&s, &p2));

    ck_assert_rf_str_eq_cstr(
        &s,
        "Δεύτερη θητεία日本の首都は東京だと思います。"
        "ちなみに今日は26日です。"
    );

    /* invalid input */
    ck_assert(!rf_string_prepend(&s, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST


/* --- String Removing Tests --- START --- */
START_TEST(test_string_remove) {
    struct RFstring s, s2, s3;
    struct RFstring r1 = RF_STRING_STATIC_INIT("\r\n");
    struct RFstring r2 = RF_STRING_STATIC_INIT(
        "歌舞伎俳優の中村橋吾さん（３３）が都内の路上で見知ら"
        "ぬ男に突然顔を殴られる被害に遭っていたことが、警視庁"
        "への取材でわかった。"
    );
    struct RFstring r3 = RF_STRING_STATIC_INIT("歌舞伎");
    struct RFstring r4 = RF_STRING_STATIC_INIT("abc");

    ck_assert(
        rf_string_init(
            &s,
            "If remove\r\ndoes not work correctly here\r\nthe test will"
            " surely\r\nfail!\r\n")
    );
    ck_assert(rf_string_remove(&s, &r1, 0, 0));
    ck_assert_rf_str_eq_cstr(&s, "If removedoes not work correctly herethe test "
                             "will surelyfail!");


    ck_assert(
           rf_string_init(
               &s2,
               "歌舞伎俳優の中村橋吾さん（３３）が都内の路上で見知らぬ男"
               "に突然顔を殴られる被害に遭っていたことが、警視庁への取材"
               "でわかった。同庁は暴行事件として調べている。")
    );
    ck_assert(rf_string_remove(&s2, &r2, 0, 0));
    ck_assert_rf_str_eq_cstr(&s2, "同庁は暴行事件として調べている。");

    /* Remove only specific number substrings */
    ck_assert(rf_string_init(
                  &s3,
                  "This Abc ABC is aBC the remaining abc sentence AbC")
    );
    ck_assert(rf_string_remove(&s3, &r4, 4, RF_CASE_IGNORE));
    ck_assert_rf_str_eq_cstr(&s3, "This   is  the remaining  sentence AbC");


    /* non existing substring */
    ck_assert(!rf_string_remove(&s2, &r3, 0, 0 ));
    /* invalid input */
    ck_assert(!rf_string_remove(&s2, NULL, 0, 0 ));


    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_string_keep_only) {
    int removals;
    struct RFstring s, s2, s3;
    struct RFstring k1 = RF_STRING_STATIC_INIT(
        "あいうえおかきくけこがぎぐげごさしすせそざじずぜぞらりるれ"
        "ろたちつてとだでどまみむめもなにぬねのはひふへほぱぴぷぺぽ"
        "をわっ1234567890。"
    );
    struct RFstring k2 = RF_STRING_STATIC_INIT("abcdefghijklmnopqrstuvwxyz 'I");
    struct RFstring k3 = RF_STRING_STATIC_INIT("zxy");

    ck_assert(rf_string_init(
                  &s,
                  "その中の1人に突然顔を殴られた。けがはなかった。")
    );
    ck_assert(rf_string_keep_only(&s, &k1, &removals));
    ck_assert_rf_str_eq_cstr(&s, "そのの1にをられた。けがはなかった。");
    ck_assert_int_eq(removals, 6);

    /* no characters in the original string */
    ck_assert(rf_string_init(&s2, "I don't have any japanese characters"));
    ck_assert(rf_string_keep_only(&s2, &k2, &removals));
    ck_assert_rf_str_eq_cstr(&s2, "I don't have any japanese characters");
    ck_assert_int_eq(removals, 0);

    /* Remove all(Keep nothing) and no removal reporting */
    ck_assert(rf_string_init(&s3, "Eleos re paidia"));
    ck_assert(rf_string_keep_only(&s3, &k3, 0));
    ck_assert_rf_str_eq_cstr(&s3, "");

    /* invalid input*/
    ck_assert(!rf_string_keep_only(&s3, NULL, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_string_prune_start) {
    struct RFstring s, s2;
    unsigned int removals;

    ck_assert(
        rf_string_init(
            &s,
            "Βουτιά σημειώνει το Χρηματιστήριο την "
            "Πέμπτη, καθώς οι επενδυτές προχωρούν σε "
            "ρευστοποιήσεις σε όλο το φάσμα της "
            "αγοράς και κυρίως στις τράπεζες.")
    );

    ck_assert(rf_string_prune_start(&s, 7, NULL));
    ck_assert_rf_str_eq_cstr(
        &s,
        "σημειώνει το Χρηματιστήριο την "
        "Πέμπτη, καθώς οι επενδυτές προχωρούν σε "
        "ρευστοποιήσεις σε όλο το φάσμα της "
        "αγοράς και κυρίως στις τράπεζες."
    );

    /* prune all remaining */
    ck_assert(rf_string_prune_start(&s, 9999, &removals));
    ck_assert_rf_str_eq_cstr(&s, "");
    ck_assert_uint_eq(removals, 138);

    /* prune nothing */
    ck_assert(rf_string_init(&s2, "Testity string"));
    ck_assert(rf_string_prune_start(&s2, 0, &removals));
    ck_assert_rf_str_eq_cstr(&s2, "Testity string");
    ck_assert_uint_eq(removals, 0);

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_prune_end) {
    struct RFstring s, s2;
    unsigned int removals;
    ck_assert(
        rf_string_init(
            &s,
            "Βουτιά σημειώνει το Χρηματιστήριο την "
            "Πέμπτη, καθώς οι επενδυτές προχωρούν σε "
            "ρευστοποιήσεις σε όλο το φάσμα της "
            "αγοράς και κυρίως στις τράπεζες.")
    );

    ck_assert(rf_string_prune_end(&s, 10, &removals));
    ck_assert_rf_str_eq_cstr(
        &s,
        "Βουτιά σημειώνει το Χρηματιστήριο την "
        "Πέμπτη, καθώς οι επενδυτές προχωρούν σε "
        "ρευστοποιήσεις σε όλο το φάσμα της "
        "αγοράς και κυρίως στις"
    );
    ck_assert_uint_eq(removals, 10);

    /* prune all remaining */
    ck_assert(rf_string_prune_end(&s, 9999, &removals));
    ck_assert_rf_str_eq_cstr(&s, "");
    ck_assert_uint_eq(removals, 135);

    /* prune nothing */
    ck_assert(rf_string_init(&s2, "Testity string"));
    ck_assert(rf_string_prune_end(&s2, 0, &removals));
    ck_assert_rf_str_eq_cstr(&s2, "Testity string");
    ck_assert_uint_eq(removals, 0);

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_prune_middle_b) {
    struct RFstring s, s2;
    unsigned int removals;

    ck_assert(
        rf_string_init(
            &s,
            "Βουτιά σημειώνει το Χρηματιστήριο την "
            "Πέμπτη, καθώς οι επενδυτές προχωρούν σε "
            "ρευστοποιήσεις σε όλο το φάσμα της "
            "αγοράς και κυρίως στις τράπεζες.")
    );

    ck_assert(rf_string_prune_middle_b(&s, 43, 37, NULL));
    ck_assert_rf_str_eq_cstr(
        &s,
        "Βουτιά , καθώς οι επενδυτές προχωρούν σε "
        "ρευστοποιήσεις σε όλο το φάσμα της "
        "αγοράς και κυρίως στις τράπεζες."
    );

    /* prune all remaining */
    ck_assert(rf_string_prune_middle_b(&s, 8, 9999, &removals));
    ck_assert_rf_str_eq_cstr(
        &s,
        "καθώς οι επενδυτές προχωρούν σε "
        "ρευστοποιήσεις σε όλο το φάσμα της "
        "αγοράς και κυρίως στις τράπεζες."
    );
    ck_assert_uint_eq(removals, 8);

    /* prune nothing */
    ck_assert(rf_string_init(&s2, "Testity string"));
    ck_assert(rf_string_prune_middle_b(&s2, 3, 0, &removals));
    ck_assert_rf_str_eq_cstr(&s2, "Testity string");
    ck_assert_uint_eq(removals, 0);

    /* prune backwards from beyond the length of the string */
    ck_assert(!rf_string_prune_middle_b(&s2, 256, 240, &removals));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_prune_middle_f) {
    struct RFstring s, s2;
    unsigned int removals;

    ck_assert(
        rf_string_init(
            &s,
            "Βουτιά σημειώνει το Χρηματιστήριο την "
            "Πέμπτη, καθώς οι επενδυτές προχωρούν σε "
            "ρευστοποιήσεις σε όλο το φάσμα της "
            "αγοράς και κυρίως στις τράπεζες.")
    );

    ck_assert(rf_string_prune_middle_f(&s, 44, 75, NULL));
    ck_assert_rf_str_eq_cstr(
        &s,
        "Βουτιά σημειώνει το Χρηματιστήριο την "
        "Πέμπτη και κυρίως στις τράπεζες."
    );

    /* prune all remaining */
    ck_assert(rf_string_prune_middle_f(&s, 44, 9999, &removals));
    ck_assert_rf_str_eq_cstr(
        &s,
        "Βουτιά σημειώνει το Χρηματιστήριο την Πέμπτη"
    );
    ck_assert_uint_eq(removals, 26);

    /* prune nothing */
    ck_assert(rf_string_init(&s2, "Testity string"));
    ck_assert(rf_string_prune_middle_f(&s2, 3, 0, &removals));
    ck_assert_rf_str_eq_cstr(&s2, "Testity string");
    ck_assert_uint_eq(removals, 0);

    /* prune forwards from beyond the length of the string */
    ck_assert(!rf_string_prune_middle_f(&s2, 256, 240, &removals));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_trim_start) {
    unsigned int removals;
    struct RFstring s, s2;
    struct RFstring sub1 = RF_STRING_STATIC_INIT(" \t123456789)");
    struct RFstring sub2 = RF_STRING_STATIC_INIT("\t\n\r");
    struct RFstring sub3 = RF_STRING_STATIC_INIT(" あいうえおか");

    ck_assert(
        rf_string_init(
            &s,
            "  \t\t  3)This is the 3rd bullet point of something")
    );
    ck_assert(rf_string_trim_start(&s, &sub1, &removals));
    ck_assert_rf_str_eq_cstr(&s, "This is the 3rd bullet point of something");
    ck_assert_uint_eq(removals, 8);

    /* no removals given */
    ck_assert(
        rf_string_init(
            &s2,
            "  あ  あ あ いうえその中の1人")
    );
    ck_assert(rf_string_trim_start(&s2, &sub3, NULL));
    ck_assert_rf_str_eq_cstr(&s2, "その中の1人");

    /* substring that contains no characters of the string  */
    ck_assert(rf_string_trim_start(&s, &sub2, &removals));
    ck_assert_rf_str_eq_cstr(&s, "This is the 3rd bullet point of something");
    ck_assert_uint_eq(removals, 0);

    /* invalid input */
    ck_assert(!rf_string_trim_start(&s, NULL, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_trim_end) {
    unsigned int removals;
    struct RFstring s, s2;
    struct RFstring sub1 = RF_STRING_STATIC_INIT(
        "あいうえおかきくけこがぎぐげごさしすせそざじずぜぞら"
        "りるれろたちつてとだでどまみむめもなにぬねのはひふへ"
        "ほぱぴぷぺぽをわっ。"
    );
    struct RFstring sub2 = RF_STRING_STATIC_INIT("\t\n\r");

    ck_assert(rf_string_init(&s, "警視庁への取材でわかった。"));
    ck_assert(rf_string_trim_end(&s, &sub1, &removals));
    ck_assert_rf_str_eq_cstr(&s, "警視庁への取材");
    ck_assert_uint_eq(removals, 6);

    /* no removals given */
    ck_assert(
        rf_string_init(
            &s2,
            "その中の1人  \t\t\n\r  \r")
    );
    ck_assert(rf_string_trim_end(&s2, &sub2, NULL));
    ck_assert_rf_str_eq_cstr(&s2, "その中の1人");

    /* substring that contains no characters of the string  */
    ck_assert(rf_string_trim_end(&s, &sub2, &removals));
    ck_assert_rf_str_eq_cstr(&s, "警視庁への取材");
    ck_assert_uint_eq(removals, 0);

    /* invalid input */
    ck_assert(!rf_string_trim_start(&s, NULL, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_trim) {
    unsigned int removals;
    struct RFstring s, s2;
    struct RFstring sub1 = RF_STRING_STATIC_INIT(
        "あいうえおかきくけこがぎぐげごさしすせそざじずぜぞら"
        "りるれろたちつてとだでどまみむめもなにぬねのはひふへ"
        "ほぱぴぷぺぽをわっ。"
    );
    struct RFstring sub2 = RF_STRING_STATIC_INIT(" \t\n\r");

    ck_assert(rf_string_init(
                  &s,
                  "   \t\t\t  This is a nice string but has lots of tabs, "
                  "spaces and newlines on either end \n\n\n")
    );
    ck_assert(rf_string_trim(&s, &sub2, &removals));
    ck_assert_rf_str_eq_cstr(
        &s,
        "This is a nice string but has lots of tabs, "
        "spaces and newlines on either end"
    );
    ck_assert_uint_eq(removals, 12);

    /* no removals given */
    ck_assert(
        rf_string_init(
            &s2,
            "その中の1人に突然顔を殴られた。けがはなかった。")
    );
    ck_assert(rf_string_trim(&s2, &sub1, NULL));
    ck_assert_rf_str_eq_cstr(&s2, "中の1人に突然顔を殴");

    /* substring that contains no characters of the string  */
    ck_assert(rf_string_trim_end(&s2, &sub2, &removals));
    ck_assert_rf_str_eq_cstr(&s2, "中の1人に突然顔を殴");
    ck_assert_uint_eq(removals, 0);

    /* invalid input */
    ck_assert(!rf_string_trim(&s, NULL, NULL));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST


/* --- String Replacing Tests --- START --- */
START_TEST(test_string_replace) {
    struct RFstring s, s2;

    struct RFstring sa1 = RF_STRING_STATIC_INIT("$INCLUDES");
    struct RFstring ra1 = RF_STRING_STATIC_INIT("../../includes");
    struct RFstring sa2 = RF_STRING_STATIC_INIT("$FLAGS");
    struct RFstring ra2 = RF_STRING_STATIC_INIT("-Wall -O2");
    struct RFstring sa3 = RF_STRING_STATIC_INIT("$FILE");
    struct RFstring ra3 = RF_STRING_STATIC_INIT("tocompile");
    struct RFstring sa4 = RF_STRING_STATIC_INIT("$OBJDIRNAME");
    struct RFstring ra4 = RF_STRING_STATIC_INIT("obj");

    struct RFstring sb1 = RF_STRING_STATIC_INIT("中国メディア");
    struct RFstring rb1 = RF_STRING_STATIC_INIT("Chinese media");
    struct RFstring sb2 = RF_STRING_STATIC_INIT("就役した");
    struct RFstring rb2 = RF_STRING_STATIC_INIT("commisioned");
    struct RFstring sb3 = RF_STRING_STATIC_INIT("初の空母");
    struct RFstring rb3 = RF_STRING_STATIC_INIT((" new Carrier"));
    struct RFstring sb4 = RF_STRING_STATIC_INIT("搭載する");
    struct RFstring rb4 = RF_STRING_STATIC_INIT(("load"));
    struct RFstring sb5 = RF_STRING_STATIC_INIT("輸出用戦闘機");
    struct RFstring rb5 = RF_STRING_STATIC_INIT(("Fighter used for exporting"));
    struct RFstring sb6 = RF_STRING_STATIC_INIT("９月");
    struct RFstring rb6 = RF_STRING_STATIC_INIT(("September"));

    /* replace some flags in a compile command */
    ck_assert(
        rf_string_init(
            &s,
            "gcc -c -I$INCLUDES $FLAGS $FILE.c -o ./$OBJDIRNAME/$FILE.o"
        )
    );
    ck_assert(rf_string_replace(&s, &sa1, &ra1, 0, 0));
    ck_assert(rf_string_replace(&s, &sa2, &ra2, 0, 0));
    ck_assert(rf_string_replace(&s, &sa3, &ra3, 0, 0));
    ck_assert(rf_string_replace(&s, &sa4, &ra4, 0, 0));
    ck_assert_rf_str_eq_cstr(
        &s,
        "gcc -c -I../../includes -Wall -O2 tocompile.c -o ./obj/tocompile.o"
    );

    /* replace some Japanese words with the english equivalent */
    ck_assert(
        rf_string_init(
            &s2,
            "中国メディアはＪ３１について、９月に就役した初の空母「遼"
            "寧」号などに搭載する新型艦載機や輸出用戦闘機になるとの見"
            "方を伝えている。"
        )
    );
    ck_assert(rf_string_replace(&s2, &sb1, &rb1, 0, 0));
    ck_assert(rf_string_replace(&s2, &sb2, &rb2, 0, 0));
    ck_assert(rf_string_replace(&s2, &sb3, &rb3, 0, 0));
    ck_assert(rf_string_replace(&s2, &sb4, &rb4, 0, 0));
    ck_assert(rf_string_replace(&s2, &sb5, &rb5, 0, 0));
    ck_assert(rf_string_replace(&s2, &sb6, &rb6, 0, 0));
    ck_assert_rf_str_eq_cstr(
        &s2,
        "Chinese mediaはＪ３１について、Septemberにcommisioned"
        " new Carrier「遼寧」号などにload新型艦載機やFighter "
        "used for exportingになるとの見方を伝えている。"
    );


    /* invalid input */
    ck_assert(!rf_string_replace(&s, NULL, &rb1, 0, 0));
    ck_assert(!rf_string_replace(&s, &sa1, NULL, 0, 0));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST




/* --- Stringx appending Tests --- START --- */
START_TEST(test_stringx_append) {
    struct RFstringx s;
    struct RFstring add1 = RF_STRING_STATIC_INIT("研究室に行ってきます。");

    ck_assert(rf_stringx_init(
                  &s, "I am on the train from Kita Senju to Kashiwa station."));
    rf_stringx_move_forward(&s, 8);
    ck_assert(rf_stringx_append(&s, &add1));
    ck_assert_rf_str_eq_cstr(
        &s,
        "the train from Kita Senju to Kashiwa station."
        "研究室に行ってきます。"
    );

    /* invalid input */
    ck_assert(!rf_stringx_append(&s, NULL));
    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_append_chars) {
    struct RFstringx s, s2;
    struct RFstring add1 = RF_STRING_STATIC_INIT("研究室に行ってきます。");
    struct RFstring add2 = RF_STRING_STATIC_INIT("Δεύτερη θητεία");

    ck_assert(rf_stringx_init(
                  &s, "I am on the train from Kita Senju to Kashiwa station."));
    rf_stringx_move_forward(&s, 8);
    ck_assert(rf_stringx_append_chars(&s, &add1, 3));
    ck_assert_rf_str_eq_cstr(
        &s,
        "the train from Kita Senju to Kashiwa station."
        "研究室"
    );

    /* no chars */
    ck_assert(rf_stringx_append_chars(&s, &add1, 0));
    ck_assert_rf_str_eq_cstr(
        &s,
        "the train from Kita Senju to Kashiwa station."
        "研究室"
    );

    /* more chars than the to add string */
    ck_assert(rf_stringx_init(&s2, "S"));
    ck_assert(rf_stringx_append_chars(&s2, &add2, 999));
    ck_assert_rf_str_eq_cstr(&s2, "SΔεύτερη θητεία");
        

    /* invalid input */
    ck_assert(!rf_stringx_append_chars(&s, NULL, 0));

    rf_stringx_deinit(&s);        
    rf_stringx_deinit(&s2);        
}END_TEST

START_TEST(test_stringx_append_char) {
    struct RFstringx s;
    
    ck_assert(rf_stringx_init(&s, "Testing appending a cha"));
    ck_assert(rf_stringx_append_char(&s, (uint32_t)'r'));
    ck_assert_rf_str_eq_cstr(&s, "Testing appending a char");

    rf_stringx_deinit(&s);        
}END_TEST

START_TEST(test_stringx_prepend) {
    struct RFstringx s, s2, s3;
    struct RFstring p1 = RF_STRING_STATIC_INIT("日本の首都は");
    struct RFstring p2 = RF_STRING_STATIC_INIT("Δεύτερη θητεία");
    struct RFstring p3 = RF_STRING_STATIC_INIT(
        "オバマ氏再選　ロムニー氏を振り切る　米大統領選 || "
    );
    struct RFstring a1 = RF_STRING_STATIC_INIT("だと思います。");


    ck_assert(rf_stringx_init(&s, "東京"));
    ck_assert(rf_stringx_initv(&s2, "ちなみに今日は%d日です。",26));

    ck_assert(rf_stringx_prepend(&s, &p1));
    ck_assert(rf_stringx_append(&s, &a1));
    ck_assert(rf_stringx_append(&s, &s2));
    ck_assert(rf_stringx_prepend(&s, &p2));

    ck_assert_rf_str_eq_cstr(
        &s,
        "Δεύτερη θητεία日本の首都は東京だと思います。"
        "ちなみに今日は26日です。"
    );

    ck_assert(rf_stringx_init(
                  &s3,
                  "Barack Obama has won a second term as US president "
                  "after winning the crucial battleground of Ohio, "
                  "taking him past the 270 margin."
              )
    );
    ck_assert(rf_stringx_prepend(&s3, &p3));
    ck_assert_rf_strx_eq_cstr(
        &s3,
        "オバマ氏再選　ロムニー氏を振り切る　米大統領選 || Barack"
        " Obama has won a second term as US president after "
        "winning the crucial battleground of Ohio, taking him "
        "past the 270 margin."
    );
    

    /* invalid input */
    ck_assert(!rf_stringx_prepend(&s2, NULL));

    rf_stringx_deinit(&s);        
    rf_stringx_deinit(&s2);            
    rf_stringx_deinit(&s3);            
}END_TEST

START_TEST(test_stringx_insert) {
    struct RFstringx s, s2;
    struct RFstring p1 = RF_STRING_STATIC_INIT(
        "オバマ氏再選　ロムニー氏を振り切る　米大統領選 || "
    );
    struct RFstring ins1 = RF_STRING_STATIC_INIT(
        "|| Δεύτερη θητεία στην προεδρία των ΗΠΑ "
        "κατέκτησε ο Μπαράκ Ομπάμα "
    );
    struct RFstring ins2 = RF_STRING_STATIC_INIT("123 - ");

    ck_assert(rf_stringx_init(
                  &s,
                  "Barack Obama has won a second term as US president "
                  "after winning the crucial battleground of Ohio, "
                  "taking him past the 270 margin."
              )
    );
    ck_assert(rf_stringx_prepend(&s, &p1));
    ck_assert_rf_strx_eq_cstr(
        &s,
        "オバマ氏再選　ロムニー氏を振り切る　米大統領選 || Barack"
        " Obama has won a second term as US president after "
        "winning the crucial battleground of Ohio, taking him "
        "past the 270 margin."
    );
    rf_stringx_move_forward(&s, 7);

    ck_assert(rf_stringx_insert(&s, 17, &ins1));
    ck_assert_rf_str_eq_cstr(
        &s,
        "ロムニー氏を振り切る　米大統領選 || Δεύτερη θητεία "
        "στην προεδρία των ΗΠΑ κατέκτησε ο Μπαράκ Ομπάμα || "
        "Barack Obama has won a second term as US president "
        "after winning the crucial battleground of Ohio, "
        "taking him past the 270 margin."
    );


    /* insert after string end */
    ck_assert(rf_stringx_init(&s2, "I am a small string"));
    ck_assert(!rf_stringx_insert(&s2, 999, &ins2));
    ck_assert_rf_strx_eq_cstr(&s2, "I am a small string");

    /* insert at beginning */
    ck_assert(rf_stringx_insert(&s2, 0, &ins2));
    ck_assert_rf_strx_eq_cstr(&s2, "123 - I am a small string");

    /* invalid input */
    ck_assert(!rf_stringx_insert(&s2, 0, NULL));

    rf_stringx_deinit(&s);        
    rf_stringx_deinit(&s2);            
}END_TEST

/* --- Stringx Unsafe Appending Tests --- START --- */
START_TEST(test_stringx_append_bytes) {
    struct RFstringx s;
    struct RFstring add1 = RF_STRING_STATIC_INIT("unsafe operations ");
    struct RFstring add2 = RF_STRING_STATIC_INIT("στην προεδρία");

    /* bytes of a normal ascii string (more than possible) */
    ck_assert(rf_stringx_init(&s, "I know how to use "));
    ck_assert(rf_stringx_append_bytes(&s, &add1, 999));
    ck_assert_rf_strx_eq_cstr(&s, "I know how to use unsafe operations ");
    
    /* bytes of a string with UTF-8 characters*/
    ck_assert(rf_stringx_append_bytes(&s, &add2, 8));
    ck_assert_rf_strx_eq_cstr(
        &s,
        "I know how to use unsafe operations στην"
    );

    /* no bytes */
    ck_assert(rf_stringx_append_bytes(&s, &add2, 0));
    ck_assert_rf_strx_eq_cstr(
        &s,
        "I know how to use unsafe operations στην"
    );    

    /* invalid input */
    ck_assert(!rf_stringx_append_bytes(&s, NULL, 8));
    
    rf_stringx_deinit(&s);
}END_TEST

START_TEST(test_stringx_append_cstr) {
    struct RFstringx s;
    static const char cstr[] = "unsafe operations";

    ck_assert(rf_stringx_init(&s, "I know how to use "));
    ck_assert(rf_stringx_append_cstr(&s, cstr));
    ck_assert_rf_strx_eq_cstr(&s, "I know how to use unsafe operations");

    /* invalid input */
    ck_assert(!rf_stringx_append_cstr(&s, NULL));

    rf_stringx_deinit(&s);
}END_TEST

/* --- Stringx Replacing Tests --- START --- */
START_TEST(test_stringx_replace) {
    struct RFstringx s, s2;

    struct RFstring sub1 = RF_STRING_STATIC_INIT("News ");

    struct RFstring sa1 = RF_STRING_STATIC_INIT("Journalist");
    struct RFstring ra1 = RF_STRING_STATIC_INIT("Ο Δημοσιογραφος");
    struct RFstring sa2 = RF_STRING_STATIC_INIT("for breach of privacy");
    struct RFstring ra2 = RF_STRING_STATIC_INIT(
        "για καταπατηση προσωπικων δικαιωματων"
    );
    struct RFstring sa3 = RF_STRING_STATIC_INIT("after publishing");
    struct RFstring ra3 = RF_STRING_STATIC_INIT("αφου δημοσιευσε");
    struct RFstring sa4 = RF_STRING_STATIC_INIT("the names");
    struct RFstring ra4 = RF_STRING_STATIC_INIT("τα ονοματα");
    struct RFstring sa5 = RF_STRING_STATIC_INIT("with Swiss bank accounts");
    struct RFstring ra5 = RF_STRING_STATIC_INIT(
        "με λογαριασμους σε Ελβετικες τραπεζες"
    );

    struct RFstring sb1 = RF_STRING_STATIC_INIT(".");
    struct RFstring rb1 = RF_STRING_STATIC_INIT(
        " || 投稿動画を参考に捜査…タクシー運転手殴った疑いで"
        "逮捕 ||"
    );

    /* replacing english words with greek ones */
    ck_assert(
        rf_stringx_init(
            &s,
            "Journalist Costas Vaxevanis has gone on trial in Athens "
            "for breach of privacy after publishing the names of 2,000 "
            "Greeks with Swiss bank accounts"
        )
    );
    ck_assert(rf_stringx_replace(&s, &sa1, &ra1, 0, 0));
    ck_assert(rf_stringx_replace(&s, &sa2, &ra2, 0, 0));
    ck_assert(rf_stringx_replace(&s, &sa3, &ra3, 0, 0));
    ck_assert(rf_stringx_replace(&s, &sa4, &ra4, 0, 0));
    ck_assert(rf_stringx_replace(&s, &sa5, &ra5, 0, 0));
    ck_assert_rf_strx_eq_cstr(
        &s,
        "Ο Δημοσιογραφος Costas Vaxevanis has gone on trial in"
        " Athens για καταπατηση προσωπικων δικαιωματων αφου "
        "δημοσιευσε τα ονοματα of 2,000 Greeks με "
        "λογαριασμους σε Ελβετικες τραπεζες"
    );

    /* moved Stringx replacing */
    ck_assert(rf_stringx_init(&s2, "News from all over the world."));
    ck_assert(rf_stringx_move_after(&s2, &sub1, 0, 0) != RF_FAILURE);
    ck_assert(rf_stringx_replace(&s2, &sb1, &rb1, 0, 0));
    ck_assert_rf_str_eq_cstr(
        &s2,
        "from all over the world || 投稿動画を参考に捜査…タク"
        "シー運転手殴った疑いで逮捕 ||"
    );

    /* invalid input */
    ck_assert(!rf_stringx_replace(&s, NULL, &rb1, 0, 0));
    ck_assert(!rf_stringx_replace(&s, &sa1, NULL, 0, 0));

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

START_TEST(test_stringx_replace_between) {
    struct RFstringx s, s2;

    struct RFstring left = RF_STRING_STATIC_INIT("||");
    struct RFstring right = RF_STRING_STATIC_INIT(" ");
    struct RFstring rep1 = RF_STRING_STATIC_INIT(
        "Φοβερη ειδηση που μολις βγηκε απο το το "
        "γραφειο ειδησεων. Η Μαρικα παντρευεται. "
        "Ναι δεν κανουμε πλακα, η Μαρικα σοβαρα "
        "παντρευεται με τον Τοτο. Μιλαμε για νεα "
        "βιβλικων διαστασεων φιλε μου ||"
    );
    struct RFstring rep2 = RF_STRING_STATIC_INIT("tokenized");

    /* a normal case */
    ck_assert(rf_stringx_init(
                  &s,
                  "from all over the world || 投稿動画を参考に捜査…タク"
                  "シー運転手殴った疑いで逮捕 ||")
    );
    ck_assert(rf_stringx_replace_between(&s, &left, &right, &rep1, 0, 1));
    ck_assert_rf_strx_eq_cstr(
        &s,
        "from all over the world ||Φοβερη ειδηση που μολις "
        "βγηκε απο το το γραφειο ειδησεων. "
        "Η Μαρικα παντρευεται"
        ". Ναι δεν κανουμε πλακα, η Μαρικα "
        "σοβαρα παντρευεται "
        "με τον Τοτο. Μιλαμε για νεα "
        "βιβλικων διαστασεων φιλε "
        "μου || 投稿動画を参考に捜査…タクシー運転手殴った"
        "疑いで逮捕 ||"
    );

    /* specific instance */
    ck_assert(rf_stringx_init(&s2, "These are simply space separated words"));
    ck_assert(rf_stringx_replace_between(&s2, &right, &right, &rep2, 0, 4));
    ck_assert_rf_strx_eq_cstr(&s2, "These are simply space tokenized words");

    
    /* invalid input */
    ck_assert(!rf_stringx_replace_between(&s, NULL, &right, &rep1, 0, 1));
    ck_assert(!rf_stringx_replace_between(&s, &left, NULL, &rep1, 0, 1));
    ck_assert(!rf_stringx_replace_between(&s, &left, &right, NULL, 0, 1));

    rf_stringx_deinit(&s);
    rf_stringx_deinit(&s2);
}END_TEST

Suite *string_manipulation_suite_create(void)
{
    Suite *s = suite_create("String Manipulation");

    TCase *string_appending = tcase_create("String Appending");
    tcase_add_checked_fixture(string_appending,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_appending, test_string_append);
    tcase_add_test(string_appending, test_string_append_int);
    tcase_add_test(string_appending, test_string_append_double);
    tcase_add_test(string_appending, test_string_prepend);

    TCase *string_removing = tcase_create("String Removing");
    tcase_add_checked_fixture(string_removing,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_removing, test_string_remove);
    tcase_add_test(string_removing, test_string_keep_only);
    tcase_add_test(string_removing, test_string_prune_start);
    tcase_add_test(string_removing, test_string_prune_end);
    tcase_add_test(string_removing, test_string_prune_middle_b);
    tcase_add_test(string_removing, test_string_prune_middle_f);
    tcase_add_test(string_removing, test_string_trim_start);
    tcase_add_test(string_removing, test_string_trim_end);
    tcase_add_test(string_removing, test_string_trim);

    TCase *string_replacing = tcase_create("String Replacing");
    tcase_add_checked_fixture(string_replacing,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(string_replacing, test_string_replace);



    TCase *stringx_appending = tcase_create("Stringx Appending");
    tcase_add_checked_fixture(stringx_appending,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(stringx_appending, test_stringx_append);
    tcase_add_test(stringx_appending, test_stringx_append_char);
    tcase_add_test(stringx_appending, test_stringx_append_chars);
    tcase_add_test(stringx_appending, test_stringx_prepend);
    tcase_add_test(stringx_appending, test_stringx_insert);

    TCase *stringx_unsafe_appending = tcase_create("Stringx Unsafe Appending");
    tcase_add_checked_fixture(stringx_unsafe_appending,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(stringx_unsafe_appending, test_stringx_append_bytes);
    tcase_add_test(stringx_unsafe_appending, test_stringx_append_cstr);

    TCase *stringx_replacing = tcase_create("Stringx Replacing");
    tcase_add_checked_fixture(stringx_replacing,
                              setup_string_tests,
                              teardown_string_tests);
    tcase_add_test(stringx_replacing, test_stringx_replace);
    tcase_add_test(stringx_replacing, test_stringx_replace_between);


    suite_add_tcase(s, string_appending);
    suite_add_tcase(s, string_removing);
    suite_add_tcase(s, string_replacing);

    suite_add_tcase(s, stringx_appending);
    suite_add_tcase(s, stringx_unsafe_appending);
    suite_add_tcase(s, stringx_replacing);
    return s;
}
