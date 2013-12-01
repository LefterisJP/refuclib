#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    RFstring s1,s2,s3,s4,s5;
    RFstring ret,ret2;
    RFstringx retx1;
    int year;
    DEFAULT_LIB_INIT();

    EXPECT(true,
           rf_string_init(
               &s1,
               "Υπερψηφίστηκε με σημαντικές απώλειες στους κόλπους της "
               "συγκυβέρνησης το σχέδιο νόμου του υπουργείου Οικονομικών "
               "που αφορά στη μείωση του ποσοστού του Δημοσίου σε ΔΕΚΟ. "
               "Αντίθετα, απορρίφθηκε η επίμαχη τροπολογία για την ένταξη "
               "του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ. Νωρίτερα ο υπουργός Οικονομικών "
               "Γιάννης Στουρνάρας κατέθεσε στο προεδρείο τον προϋπολογισμό"
               " του 2013. Απορρίφθηκε επίσης οριακά το άρθρο 4 που αφορά "
               "τις εισφορές του ΤΕΕ"));

    //expect true and 2013: (the number coming after the given substring)
    EXPECT(true,
           rf_string_scanf_after(
               &s1,RFS_("τον προϋπολογισμό του "),"%d",&year));
    EXPECT(2013,year);
    //expect (true) and (true) that this is actually the found substring
    EXPECT(true,
           rf_string_between(
               &s1, RFS_("Αντίθετα, "),
               RFS_("του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ."),&ret,0));
    EXPECT(true,
           rf_string_equal(
               &ret,
               RFS_("απορρίφθηκε η επίμαχη τροπολογία για την ένταξη ")));
    //expect (false), so no substring was found
    EXPECT(false,
           rf_string_between(
               &s1,
               RFS_("Αντίθετα, "),
               RFS_("I am not in there"),&ret,0));
    //expect (false), so no substring was found
    EXPECT(false,
           rf_string_between(
               &s1,
               RFS_("I am not in there either"),
               RFS_("I am not in there"),&ret,0));

    rf_string_deinit(&ret);

    EXPECT(true,
           rf_string_init(&s2,"I Am A StRinG wItH PecULIAR LeTteRs caSE"));
    //expect (true), and "PecULIAR" which is the substring
    EXPECT(true,
           rf_string_between(
               &s2,
               RFS_("I am a string with "),
               RFS_(" Letters"),
               &ret,
               RF_CASE_IGNORE));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&ret)), 0);
    rf_string_deinit(&ret);

    //expect true from both and <meta name="application-name
    EXPECT(true,
           rf_string_init(
               &s3,
               "<meta name=\"application-name\" content=\"BBC News\" />"));
    EXPECT(true,rf_string_beforev(&s3,&ret,0,2,RFS_("/>"),RFS_("\" ")));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&ret)), 0);
    //expect false
    EXPECT(false,
           rf_string_beforev(&s3, &ret, 0, 4, RFS_("56"),
                            RFS_("@"), RFS_("ένταξη"),
                            RFS_("Not in there")));
    rf_string_deinit(&ret);

    //expect true from all: meaning we did indeed get the substring we were expecting
    EXPECT(true,
           rf_string_init(
               &s4,
               "神奈川県の黒岩知事は３１日の定例記者会見で、次期衆院選をに"
               "らみ、都知事を辞職した石原慎太郎氏が新党を作り、日本維新"
               "の会などと民主、自民両党に対抗する「第３極」の結集を図ろ"
               "うとしていることについて、「政策なんかどうでもいいという、"
               "ある種の暴論だ」と痛烈に批判した"));
    EXPECT(true,rf_string_before(&s4,RFS_("は"),&ret,0));
    EXPECT(true,rf_string_equal(&ret,RFS_("神奈川県の黒岩知事")));
    //expect (false)
    EXPECT(false,rf_string_before(&s4,RFS_("東京"),&ret,0));
    rf_string_deinit(&ret);

    //expect false due to the RF_MATCH_WORD flag .. which btw should never be used in Japanese text since words are not separated by spaces
    EXPECT(false,
           rf_string_after(&s4,RFS_("「第３極」の"),&ret,RF_MATCH_WORD));
    //expect (true) and then (true): meaning we did indeed find the expected substring
    EXPECT(true,rf_string_after(&s4,RFS_("「第３極」の"),&ret,0));
    EXPECT(true,
           rf_string_equal(
               &ret,
               RFS_(
                   "結集を図ろうとしていることについて、「政策なんかどう"
                   "でもいいという、ある種の暴論だ」と痛烈に批判した")));
    rf_string_deinit(&ret);

    EXPECT(true,rf_string_init(&s5,"神奈川県,福島県,和歌山県"));
    EXPECT(true,
           rf_string_afterv(
               &s5, &retx1, RF_STRINGX_ARGUMENT, 2, RFS_(" "),RFS_(",")));
    EXPECT(true,rf_string_before(&retx1,RFS_(","),&ret2,0));
    EXPECT(true,rf_string_equal(&ret2,RFS_("福島県")));
    rf_stringx_move_forward(&retx1,4);;
    EXPECT(true,rf_string_equal(&retx1,RFS_("和歌山県")));

    return 0;
}
