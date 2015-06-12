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

#include <Utils/array.h>

/* --- String Acessors Tests --- START --- */
START_TEST(test_string_length) {
    struct RFstringx sx;
    struct RFstring s, s2, s3;

    ck_assert(rf_stringx_init(&sx, "Δοκιμαζουμε την αναθεση"));
    ck_assert(rf_string_init(
                  &s,
                  "The president said he had set down a \"15-minute rule\""
                  "with his team for response to governors and mayors seeking "
                  "federal assistance."));
    ck_assert(rf_string_init(&s2, ""));
    ck_assert(rf_string_init(&s3,
                             "【北京＝峯村健司】中国共産党の第１７期中央委員会"
                             "第７回全体会議（７中全会）が１日、北京市内で開会"
                             "した模様だ。８日からの第１８回党大会の準備作業を"
                             "する。党指導部の世代交代が進むとみられる５年に１"
                             "度の党大会前後の一連の政治的な行事が本格的に始ま"
                             "った。"));


    ck_assert_uint_eq(rf_string_length(RF_STRX2STR(&sx)), 23);
    ck_assert_uint_eq(rf_string_length(&s), 131);
    ck_assert_uint_eq(rf_string_length(&s2), 0);
    ck_assert_uint_eq(rf_string_length(&s3), 123);

    rf_stringx_deinit(&sx);
    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

START_TEST(test_string_length_bytes) {
    struct RFstringx sx;
    struct RFstring s, s2;

    ck_assert(rf_stringx_init(&sx, "Δοκιμαζουμε την αναθεση"));
    ck_assert(rf_string_init(&s, "This is the test for String retrievals."));
    ck_assert(rf_string_init(&s2, ""));

    ck_assert_uint_eq(rf_string_length_bytes(&sx), 44);
    ck_assert_uint_eq(rf_string_length_bytes(&s),
              strlen("This is the test for String retrievals."));

    ck_assert_uint_eq(rf_string_length_bytes(&s2), 0);

    rf_stringx_deinit(&sx);
    rf_string_deinit(&s);
    rf_string_deinit(&s2);
}END_TEST

START_TEST(test_string_data) {
    struct RFstring s;
    struct RFstringx sx;

    ck_assert(rf_string_init(&s, "A simple string"));
    ck_assert_int_eq(rf_string_data(&s)[2], 's');

    ck_assert(rf_stringx_init(&sx, "An extended string this is"));
    ck_assert_int_eq(rf_string_data(&sx)[13], 't');

    rf_string_deinit(&s);
    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_string_get_char) {
    struct RFstring s, s2, s3;
    uint32_t cp;
    ck_assert(rf_string_init(
                  &s,
                  "The president said he had set down a \"15-minute rule\""
                  "with his team for response to governors and mayors seeking "
                  "federal assistance."));
    ck_assert(rf_string_init(&s2, ""));
    ck_assert(rf_string_init(&s3,
                             "【北京＝峯村健司】中国共産党の第１７期中央委員会"
                             "第７回全体会議（７中全会）が１日、北京市内で開会"
                             "した模様だ。８日からの第１８回党大会の準備作業を"
                             "する。党指導部の世代交代が進むとみられる５年に１"
                             "度の党大会前後の一連の政治的な行事が本格的に始ま"
                             "った。"));

    /* expect 84: (decimal value of 'T' */
    ck_assert(rf_string_get_char(&s, 0, &cp));
    ck_assert_uint_eq(cp, 84);

    /* expect 29987: (decimal value of '産') */
    ck_assert(rf_string_get_char(&s3, 12, &cp));
    ck_assert_uint_eq(cp, 29987);

    ck_assert(!rf_string_get_char(&s2, 0, &cp));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);
}END_TEST

/* --- String Retrieval Tests --- START --- */
START_TEST(test_string_substr) {
    struct RFstring s;
    struct RFstring ret;

    ck_assert(rf_string_init(
                  &s,
                  "【ワシントン＝行方史郎】ハリケーン「サンディ」の影響で"
                  "、米ニューヨーク州とニュージャージー州で運転中だった原"
                  "発３基が２９日から３０日にかけて停止した。米原子力規制"
                  "委員会（ＮＲＣ）が発表した。いずれも原子炉の冷却機能は"
                  "確保されており、いまのところ安全上の問題はないという。"
                  "ニューヨーク州のナインマイルポイント原発１号機とインデ"
                  "ィアナポイント原発３号機では原子炉が自動的に緊急停止。"
                  "いずれも送電網の障害が原因とみられる。ニュージャージー"
                  "州のセーラム原発１号機は、川から水を取る循環水ポンプが"
                  "故障し、手動で原子炉を止めた。　また、燃料交換のため停"
                  "止中だったニュージャージー州オイスタークリーク原発では"
                  "、取水口の水位が一時、警戒レベルを超えた。電力会社は、"
                  "ＮＲＣが定めた４段階の緊急態勢のうち下から２番目の「警"
                  "戒」を宣言して事態を注視している。")
    );
    /* get substring at startpos 308 and 1 positions on */
    ck_assert(rf_string_substr(&s, 308, 10, &ret));
    ck_assert_rf_str_eq_cstr(&ret, "警戒レベルを超えた。");

    /* edge case. substr index out of bounds */
    ck_assert(!rf_string_substr(&s, 1022, 22, &ret));

    rf_string_deinit(&s);
    rf_string_deinit(&ret);
}END_TEST

START_TEST(test_string_find) {
    struct RFstring s, s2, s3;
    struct RFstring f1, f2, f3, f4;
    ck_assert(rf_string_init(
                  &s,
                  "This is the test for String retrievals. We are testing "
                  "retrieval in various situations"
              )
    );
    ck_assert(rf_string_init(&s2,"ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    ck_assert(rf_string_init(&s3,"abcdefghijklmnopqrstuvwxyz"));

    ck_assert(rf_string_init(&f1, "We are"));
    ck_assert(rf_string_init(&f2, "I am not in the string"));
    ck_assert(rf_string_init(&f3, "ijklmnopqrstuv"));
    ck_assert(rf_string_init(&f4, "DEFGHIJKL"));

    ck_assert_int_eq(rf_string_find(&s, &f1, 0), 40);
    ck_assert_int_eq(rf_string_find(&s, &f2, 0), RF_FAILURE);

    ck_assert_int_eq(rf_string_find(&s2, &f3, RF_CASE_IGNORE), 8);
    ck_assert_int_eq(rf_string_find(&s3, &f4, RF_CASE_IGNORE), 3);
    ck_assert_int_eq(rf_string_find(&s3, &f4, RF_CASE_IGNORE|RF_MATCH_WORD),
                     RF_FAILURE);

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&s3);

    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
    rf_string_deinit(&f3);
    rf_string_deinit(&f4);
}END_TEST

START_TEST(test_string_find_i) {
    struct RFstring s, s2;
    struct RFstring f1, f2;

    ck_assert(rf_string_init(
                  &s,
                  "A record 4m (13ft) tidal surge sent seawater cascading "
                  "into large parts of New York City's subway system"
              )
    );
    ck_assert(rf_string_init(
                  &s2,
                  "東京電力福島第一原発の被曝（ひばく）隠し問題で、厚生労働"
                  "省は３０日、データの誤入力や線量計の未装着など１９件の不"
                  "適切な事例が新たに見つかったと発表した")
    );

    ck_assert(rf_string_init(&f1, "New York"));
    ck_assert(rf_string_init(&f2, "不適切な事例"));

    ck_assert_int_eq(RF_FAILURE,
                     rf_string_find_i(&s, &f1, 2, 30, 0));
    ck_assert_int_eq(55, rf_string_find_i(&s2, &f2, 34, 120, 0));

    /* edge cases */
    ck_assert_int_eq(RF_FAILURE,
                     rf_string_find_i(&s, &f1, 1024, 30, 0));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);

    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
}END_TEST

START_TEST(test_string_begins_with) {
    struct RFstring s;
    struct RFstring f1, f2;
    ck_assert(rf_string_init(
                  &s,
                  "Στον πυρετό της έγκρισης των μέτρων και των "
                  "προαπαιτούμενων ενεργειών μπαίνει από την Τετάρτη η Αθήνα")
    );

    ck_assert(rf_string_init(&f1, "Στον πυρετό"));
    ck_assert(rf_string_init(&f2, "Στο πυρετό"));

    ck_assert(rf_string_begins_with(&s, &f1, 0));
    ck_assert(!rf_string_begins_with(&s, &f2, 0));

    rf_string_deinit(&s);

    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
}END_TEST

START_TEST(test_string_begins_with_any) {
    const struct RFstring s = RF_STRING_STATIC_INIT(" \t    \t eleos");
    const struct RFstring s2 = RF_STRING_STATIC_INIT(" 影 π π eleos2");
    const struct RFstring s3 = RF_STRING_STATIC_INIT("not starting with stuff");
    static const struct RFstring chars1 = RF_STRING_STATIC_INIT(" \t");
    static const struct RFstring chars2 = RF_STRING_STATIC_INIT(" π影");
    unsigned int bytes;

    ck_assert_int_eq(8, rf_string_begins_with_any(&s, &chars1, 0, 0));
    ck_assert_int_eq(8, rf_string_begins_with_any(&s, &chars1, 0, &bytes));
    ck_assert_int_eq(8, bytes);

    ck_assert_int_eq(7, rf_string_begins_with_any(&s2, &chars2, 0, &bytes));
    ck_assert_int_eq(11, bytes);

    ck_assert_int_eq(3,
                     rf_string_begins_with_any(&s2,
                                               &chars2,
                                               rf_string_data(&s2) + 4,
                                               &bytes));
    ck_assert_int_eq(5, bytes);

    bytes = 0;
    ck_assert_int_eq(0,
                     rf_string_begins_with_any(&s3, &chars1, 0, &bytes));
    ck_assert_int_eq(0, bytes);
}END_TEST

START_TEST(test_string_ends_with) {
    struct RFstring s;
    struct RFstring f1, f2;
    ck_assert(rf_string_init(
                  &s,
                  "Στον πυρετό της έγκρισης των μέτρων και των "
                  "προαπαιτούμενων ενεργειών μπαίνει από την Τετάρτη η Αθήνα")
    );

    ck_assert(rf_string_init(&f1, "Αθήνα"));
    ck_assert(rf_string_init(&f2, "Δεν ειμαι στο τελος"));

    ck_assert(rf_string_ends_with(&s, &f1, 0));
    ck_assert(!rf_string_ends_with(&s, &f2, 0));

    rf_string_deinit(&s);

    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
}END_TEST

START_TEST(test_string_count) {
    struct RFstring s, s2;
    struct RFstring f1, f2, f3;

    const struct RFstring f4 = RF_STRING_STATIC_INIT("i");
    ck_assert(rf_string_init(
                  &s,
                  "【ワシントン＝行方史郎】ハリケーン「サンディ」の影響で"
                  "、米ニューヨーク州とニュージャージー州で運転中だった原"
                  "発３基が２９日から３０日にかけて停止した。米原子力規制"
                  "委員会（ＮＲＣ）が発表した。いずれも原子炉の冷却機能は"
                  "確保されており、いまのところ安全上の問題はないという。"
                  "ニューヨーク州のナインマイルポイント原発１号機とインデ"
                  "ィアナポイント原発３号機では原子炉が自動的に緊急停止。"
                  "いずれも送電網の障害が原因とみられる。ニュージャージー"
                  "州のセーラム原発１号機は、川から水を取る循環水ポンプが"
                  "故障し、手動で原子炉を止めた。　また、燃料交換のため停"
                  "止中だったニュージャージー州オイスタークリーク原発では"
                  "、取水口の水位が一時、警戒レベルを超えた。電力会社は、"
                  "ＮＲＣが定めた４段階の緊急態勢のうち下から２番目の「警"
                  "戒」を宣言して事態を注視している。")
    );
    ck_assert(rf_string_init(&s2, "Simple ascii count of 'i'"));

    ck_assert(rf_string_init(&f1, "ニューヨーク"));
    ck_assert(rf_string_init(&f2, "州"));
    ck_assert(rf_string_init(&f3, "東京"));

    ck_assert_int_eq(4, rf_string_count(&s2, &f4, 0, 0, 0));

    ck_assert_int_eq(2, rf_string_count(&s, &f1, 0, 0, 0));
    ck_assert_int_eq(5, rf_string_count(&s, &f2, 0, 0, 0));
    ck_assert_int_eq(0 ,rf_string_count(&s, &f3, 0, 0, 0));

    /* search specific bytelength */
    ck_assert_int_eq(2, rf_string_count(&s2, &f4, 11, 0, 0));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);

    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
    rf_string_deinit(&f3);
}END_TEST

START_TEST(test_string_count_positions) {
    static const struct RFstring s = RF_STRING_STATIC_INIT(
        "This is a test sentence"
    );
    static const struct RFstring space = RF_STRING_STATIC_INIT(" ");
    static const struct RFstring s2 = RF_STRING_STATIC_INIT(
        "repeat is nice when you repeat and repeat and repeat"
    );
    static const struct RFstring rep = RF_STRING_STATIC_INIT("repeat");
    static const struct RFstring s3 = RF_STRING_STATIC_INIT("end in newline\n");
    static const struct RFstring nl = RF_STRING_STATIC_INIT("\n");
    struct RFarray arr1;
    RF_ARRAY_TEMP_INIT(&arr1, uint32_t, 4);
    uint32_t arr2_buff[2];
    struct RFarray arr2 = RF_ARRAY_SHALLOW_INIT(arr2_buff);

    /* test counting positions with array of enough space */
    ck_assert_int_eq(4, rf_string_count(&s, &space, 0, &arr1, 0));
    ck_assert_uint_eq(4, rf_array_at_unsafe(&arr1, 0, uint32_t));
    ck_assert_uint_eq(7, rf_array_at_unsafe(&arr1, 1, uint32_t));
    ck_assert_uint_eq(9, rf_array_at_unsafe(&arr1, 2, uint32_t));
    ck_assert_uint_eq(14, rf_array_at_unsafe(&arr1, 3, uint32_t));
    /* test for newlines at end of string */
    ck_assert_uint_eq(1, rf_string_count(&s3, &nl, 0, &arr1, 0));
    ck_assert_uint_eq(14, rf_array_at_unsafe(&arr1, 0, uint32_t));

    /* test counting positions with smaller array */
    ck_assert_int_eq(4, rf_string_count(&s2, &rep, 0, &arr2, RF_CASE_IGNORE));
    ck_assert_uint_eq(0, rf_array_at_unsafe(&arr2, 0, uint32_t));
    ck_assert_uint_eq(24, rf_array_at_unsafe(&arr2, 1, uint32_t));
    ck_assert_uint_eq(35, rf_array_at_unsafe(&arr2, 2, uint32_t));
    ck_assert_uint_eq(46, rf_array_at_unsafe(&arr2, 3, uint32_t));

    rf_array_deinit(&arr1);
    rf_array_deinit(&arr2);
}END_TEST


/* --- String Positional Retrieval Tests --- START --- */
START_TEST(test_string_scanf_after) {
    struct RFstring s;
    struct RFstring f1;
    int year;
    ck_assert(
        rf_string_init(
            &s,
            "Υπερψηφίστηκε με σημαντικές απώλειες στους κόλπους της "
            "συγκυβέρνησης το σχέδιο νόμου του υπουργείου Οικονομικών "
            "που αφορά στη μείωση του ποσοστού του Δημοσίου σε ΔΕΚΟ. "
            "Αντίθετα, απορρίφθηκε η επίμαχη τροπολογία για την ένταξη "
            "του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ. Νωρίτερα ο υπουργός Οικονομικών "
            "Γιάννης Στουρνάρας κατέθεσε στο προεδρείο τον προϋπολογισμό"
            " του 2013. Απορρίφθηκε επίσης οριακά το άρθρο 4 που αφορά "
            "τις εισφορές του ΤΕΕ")
    );
    ck_assert(rf_string_init(&f1, "τον προϋπολογισμό του "));

    ck_assert(rf_string_scanf_after(&s, &f1, "%d", &year));
    ck_assert_int_eq(2013,year);

    rf_string_deinit(&s);
    rf_string_deinit(&f1);
}END_TEST

START_TEST(test_string_between) {
    static const struct RFstring p1l = RF_STRING_STATIC_INIT("Αντίθετα, ");
    static const struct RFstring p1r = RF_STRING_STATIC_INIT("του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ.");
    static const struct RFstring f1 = RF_STRING_STATIC_INIT("I am not there");
    static const struct RFstring p2l = RF_STRING_STATIC_INIT("I am a string with ");
    static const struct RFstring p2r = RF_STRING_STATIC_INIT(" Letters");

    struct RFstring s, s2;
    struct RFstring ret;
    struct RFstringx retx;

    ck_assert(
        rf_string_init(
            &s,
            "Υπερψηφίστηκε με σημαντικές απώλειες στους κόλπους της "
            "συγκυβέρνησης το σχέδιο νόμου του υπουργείου Οικονομικών "
            "που αφορά στη μείωση του ποσοστού του Δημοσίου σε ΔΕΚΟ. "
            "Αντίθετα, απορρίφθηκε η επίμαχη τροπολογία για την ένταξη "
            "του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ. Νωρίτερα ο υπουργός Οικονομικών "
            "Γιάννης Στουρνάρας κατέθεσε στο προεδρείο τον προϋπολογισμό"
            " του 2013. Απορρίφθηκε επίσης οριακά το άρθρο 4 που αφορά "
            "τις εισφορές του ΤΕΕ")
    );
    ck_assert(
           rf_string_init(&s2,"I Am A StRinG wItH PecULIAR LeTteRs caSE")
    );

    ck_assert(rf_string_init(&ret, ""));
    ck_assert(rf_stringx_init_buff(&retx, 512, ""));

    ck_assert(rf_string_between(&s, &p1l, &p1r, &ret, 0));
    ck_assert_rf_str_eq_cstr(
        &ret,
        "απορρίφθηκε η επίμαχη τροπολογία για την ένταξη ");

    /* expect (true), and "PecULIAR" which is the substring */
    ck_assert(rf_string_between(&s2, &p2l, &p2r, &retx,
                                RF_CASE_IGNORE|RF_STRINGX_ARGUMENT));
    ck_assert_rf_strx_eq_cstr(&retx, "PecULIAR");

    /* not found cases */
    ck_assert(!rf_string_between(&s, &p1l, &f1, &ret, 0));
    ck_assert(!rf_string_between(&s, &f1, &f1, &ret, 0));

    rf_string_deinit(&s);
    rf_string_deinit(&s2);
    rf_string_deinit(&ret);
    rf_stringx_deinit(&retx);
}END_TEST

START_TEST(test_string_beforev) {
    struct RFstring s;
    struct RFstring f1, f2, f3, f4;
    struct RFstring ret;

    ck_assert(rf_string_init(
                  &s,
                  "<meta name=\"application-name\" content=\"BBC News\" />")
    );

    ck_assert(rf_string_init(&f1, "/>"));
    ck_assert(rf_string_init(&f2, "\" "));
    ck_assert(rf_string_init(&f3, "not in there"));
    ck_assert(rf_string_init(&f4, "not in there either"));

    ck_assert(rf_string_beforev(&s, &ret, 0, 2, &f1, &f2));
    ck_assert_rf_str_eq_cstr(&ret, "<meta name=\"application-name");

    /* not in the string */
    ck_assert(!rf_string_beforev(&s, &ret, 0, 2, &f3, &f4));

    rf_string_deinit(&s);
    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
    rf_string_deinit(&f3);
    rf_string_deinit(&f4);
    rf_string_deinit(&ret);

}END_TEST

START_TEST(test_string_before) {
    struct RFstring s;
    struct RFstring f1, f2;
    struct RFstring ret;
    ck_assert(
        rf_string_init(
            &s,
            "神奈川県の黒岩知事は３１日の定例記者会見で、次期衆院選をに"
            "らみ、都知事を辞職した石原慎太郎氏が新党を作り、日本維新"
            "の会などと民主、自民両党に対抗する「第３極」の結集を図ろ"
            "うとしていることについて、「政策なんかどうでもいいという、"
            "ある種の暴論だ」と痛烈に批判した")
    );

    ck_assert(rf_string_init(&f1, "は"));
    ck_assert(rf_string_init(&f2, "東京"));

    ck_assert(rf_string_before(&s, &f1 ,&ret, 0));
    ck_assert_rf_str_eq_cstr(&ret, "神奈川県の黒岩知事");
    /* f2 is not a substring */
    ck_assert(!rf_string_before(&s, &f2 ,&ret, 0));

    rf_string_deinit(&s);
    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
    rf_string_deinit(&ret);
}END_TEST

START_TEST(test_string_after) {
    struct RFstring s;
    struct RFstring f1, f2;
    struct RFstring ret;
    ck_assert(
        rf_string_init(
            &s,
            "神奈川県の黒岩知事は３１日の定例記者会見で、次期衆院選をに"
            "らみ、都知事を辞職した石原慎太郎氏が新党を作り、日本維新"
            "の会などと民主、自民両党に対抗する「第３極」の結集を図ろ"
            "うとしていることについて、「政策なんかどうでもいいという、"
            "ある種の暴論だ」と痛烈に批判した")
    );

    ck_assert(rf_string_init(&f1, "「第３極」の"));
    ck_assert(rf_string_init(&f2, "not in there"));

    ck_assert(rf_string_after(&s ,&f1, &ret, 0));
    ck_assert_rf_str_eq_cstr(
        &ret,
        "結集を図ろうとしていることについて、「政策なんかどう"
        "でもいいという、ある種の暴論だ」と痛烈に批判した"
    );

    /* no spaces so RF_MATCH_WORD leads to failure */
    ck_assert(!rf_string_after(&s ,&f1, &ret, RF_MATCH_WORD));
    /* substring not in main string */
    ck_assert(!rf_string_after(&s, &f2, &ret, 0));

    rf_string_deinit(&s);
    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
    rf_string_deinit(&ret);
}END_TEST

START_TEST(test_string_afterv) {
    struct RFstring s;
    struct RFstring f1, f2, f3;
    struct RFstringx ret;
    struct RFstring ret2;
    ck_assert(rf_string_init(&s, "神奈川県,福島県,和歌山県"));

    ck_assert(rf_string_init(&f1, " "));
    ck_assert(rf_string_init(&f2, ","));
    ck_assert(rf_string_init(&f3, "eleos"));

    ck_assert(
        rf_string_afterv(
            &s, &ret, RF_STRINGX_ARGUMENT, 2, &f1, &f2)
    );
    ck_assert_rf_str_eq_cstr(&ret, "福島県,和歌山県");
    ck_assert(rf_string_before(RF_STRX2STR(&ret), &f2, &ret2, 0));
    ck_assert_rf_str_eq_cstr(&ret2, "福島県");

    /* substring not in main string */
    ck_assert(!rf_string_afterv(&s, &ret, RF_STRINGX_ARGUMENT, 1, &f3));

    rf_string_deinit(&s);
    rf_string_deinit(&f1);
    rf_string_deinit(&f2);
    rf_string_deinit(&f3);
    rf_stringx_deinit(&ret);
    rf_string_deinit(&ret2);

}END_TEST

START_TEST (test_invalid_string_retrieval) {
    struct RFstring s;
    struct RFstring f1;
    struct RFstringx f3;
    struct RFstring ret;
    int year;
    static const struct RFstring p1l = RF_STRING_STATIC_INIT("Αντίθετα, ");

    ck_assert(rf_string_init(
                  &s,
                  "The president said he had set down a \"15-minute rule\""
                  "with his team for response to governors and mayors seeking "
                  "federal assistance."));
    ck_assert(!rf_string_get_char(&s, 0, NULL));
    ck_assert_int_eq(rf_string_find(&s, NULL, 0), RF_FAILURE);
    ck_assert_int_eq(RF_FAILURE,
                     rf_string_find_i(&s, NULL, 2, 30, 0));
    ck_assert_int_eq(-1 ,rf_string_count(&s, NULL, 0, 0, 0));
    ck_assert(!rf_string_substr(&s, 21, 22, NULL));
    ck_assert(!rf_string_scanf_after(&s, NULL, "%d", &year));
    ck_assert(!rf_string_scanf_after(&s, &f1, NULL, &year));
    ck_assert(!rf_string_scanf_after(&s, &f1, "%d", NULL));
    ck_assert(!rf_string_between(&s, NULL, &f1, &ret, 0));
    ck_assert(!rf_string_between(&s, &p1l, NULL, &ret, 0));
    ck_assert(!rf_string_between(&s, &p1l, &f1, NULL, 0));
    ck_assert(!rf_string_beforev(&s, NULL, 0, 2, &f1, &p1l));
    ck_assert(!rf_string_beforev(&s, &ret, 0, 2, NULL, NULL));
    ck_assert(!rf_string_before(&s, NULL, &ret, 0));
    ck_assert(!rf_string_before(&s, &f1, NULL, 0));
    ck_assert(!rf_string_after(&s, NULL, &ret, 0));
    ck_assert(!rf_string_after(&s, &f1, NULL, 0));
    ck_assert(!rf_string_afterv(&s, NULL, RF_STRINGX_ARGUMENT, 1, &f3));
    ck_assert(!rf_string_afterv(&s, &ret, 0, 1, NULL));

    rf_string_deinit(&s);
} END_TEST

Suite *string_retrieval_suite_create(void)
{
    Suite *s = suite_create("String Retrieval");

    TCase *string_accessors = tcase_create(
        "String Accessors"
    );
    tcase_add_checked_fixture(string_accessors,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_accessors, test_string_length);
    tcase_add_test(string_accessors, test_string_length_bytes);
    tcase_add_test(string_accessors, test_string_data);
    tcase_add_test(string_accessors, test_string_get_char);

    TCase *string_retrieval = tcase_create("String Retrieval");
    tcase_add_checked_fixture(string_retrieval,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_retrieval, test_string_substr);
    tcase_add_test(string_retrieval, test_string_find);
    tcase_add_test(string_retrieval, test_string_find_i);
    tcase_add_test(string_retrieval, test_string_begins_with);
    tcase_add_test(string_retrieval, test_string_begins_with_any);
    tcase_add_test(string_retrieval, test_string_ends_with);
    tcase_add_test(string_retrieval, test_string_count);
    tcase_add_test(string_retrieval, test_string_count_positions);

    TCase *string_positional_retrieval = tcase_create(
        "String Positional Retrieval"
    );
    tcase_add_checked_fixture(string_positional_retrieval,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(string_positional_retrieval, test_string_scanf_after);
    tcase_add_test(string_positional_retrieval, test_string_between);
    tcase_add_test(string_positional_retrieval, test_string_beforev);
    tcase_add_test(string_positional_retrieval, test_string_before);
    tcase_add_test(string_positional_retrieval, test_string_after);
    tcase_add_test(string_positional_retrieval, test_string_afterv);

    TCase *string_invalid_retrieval = tcase_create(
        "String Invalid Retrieval"
    );
    tcase_add_checked_fixture(string_invalid_retrieval,
                              setup_invalid_args_tests,
                              teardown_invalid_args_tests);
    tcase_add_test(string_invalid_retrieval, test_invalid_string_retrieval);

    suite_add_tcase(s, string_accessors);
    suite_add_tcase(s, string_retrieval);
    suite_add_tcase(s, string_positional_retrieval);
    suite_add_tcase(s, string_invalid_retrieval);
    return s;
}
