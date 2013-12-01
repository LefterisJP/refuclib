#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    RFstring s1,s2,s3,s4,s5,s6,s7;
    DEFAULT_LIB_INIT();


    //expect true and then the string without the \r
    EXPECT(true,
           rf_string_init(
               &s1,
               "If remove\r\ndoes not work correctly here\r\nthe test will"
               " surely\r\nfail!\r\n"));
    EXPECT(true,rf_string_remove(&s1,RFS_("\r"), 0, 0));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s1)), 0);

    //expect true for all confirming that by removing we got left only with what we should be
    EXPECT(true,
           rf_string_init(
               &s2,
               "歌舞伎俳優の中村橋吾さん（３３）が都内の路上で見知らぬ男"
               "に突然顔を殴られる被害に遭っていたことが、警視庁への取材"
               "でわかった。同庁は暴行事件として調べている。"));
    EXPECT(true,
           rf_string_remove(
               &s2,
               RFS_("歌舞伎俳優の中村橋吾さん（３３）が都内の路上で見知ら"
                    "ぬ男に突然顔を殴られる被害に遭っていたことが、警視庁"
                    "への取材でわかった。"),
               0, 0));
    EXPECT(true,
           rf_string_equal(&s2,RFS_("同庁は暴行事件として調べている。")));
    //expect false, can't remove that from the string since it is already removed
    EXPECT(false,rf_string_remove(&s2,RFS_("歌舞伎"), 0, 0 ));

    //expect true: keep only the hiragana(it's not the list of all the hiragana, must have forgotten something),the numbers and the full stop from this sentence
    EXPECT(true,
           rf_string_init(
               &s3,"その中の1人に突然顔を殴られた。けがはなかった。"));
    rf_string_keep_only(
        &s3,
        RFS_("あいうえおかきくけこがぎぐげごさしすせそざじずぜぞらりるれ"
             "ろたちつてとだでどまみむめもなにぬねのはひふへほぱぴぷぺぽ"
             "をわっ1234567890。"));
    EXPECT(true,
           rf_string_equal(&s3,RFS_("そのの1にをられた。けがはなかった。")));
    //expect true from all(all prunings should be succesfull)
    EXPECT(true,
           rf_string_init(
               &s4,
               "Βουτιά σημειώνει το Χρηματιστήριο την Πέμπτη, καθώς οι επε"
               "νδυτές προχωρούν σε ρευστοποιήσεις σε όλο το φάσμα της "
               "αγοράς και κυρίως στις τράπεζες."));
    EXPECT(true,rf_string_prune_start(&s4,7));
    EXPECT(true,rf_string_prune_end(&s4,10));
    EXPECT(true,rf_string_prune_middle_f(&s4,38,19));
    EXPECT(true,rf_string_prune_middle_b(&s4,65,28));
    //expect true: means we got the string we expected to get
    EXPECT(true,
           rf_string_equal(
               &s4,
               RFS_("σημειώνει το Χρηματιστήριο την Πέμπτη, σε όλο το "
                    "φάσμα της αγοράς και κυρίως στις")));

    //expect the trimmed string: "This is a nice string but has lots of tabs, spaces and newlines on either end"
    EXPECT(true,
           rf_string_init(
               &s5,
               "   \t\t\t  This is a nice string but has lots of tabs, "
               "spaces and newlines on either end \n\n\n"));
    EXPECT(true,rf_string_trim(&s5,RFS_(" \n\t")));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s5)), 0);

    //Trim all the hiragana and full width stop from the end of the string and check the string afterwards
    EXPECT(true,rf_string_init(&s6,"警視庁への取材でわかった。"));
    EXPECT(true,
           rf_string_trim_end(
               &s6,
               RFS_("あいうえおかきくけこがぎぐげごさしすせそざじずぜぞら"
                    "りるれろたちつてとだでどまみむめもなにぬねのはひふへ"
                    "ほぱぴぷぺぽをわっ。")));
    EXPECT(true,rf_string_equal(&s6,RFS_("警視庁への取材")));
    //expect the string trimmed of all spaces,tabs and number-bullet points in the beginning
    EXPECT(true,
           rf_string_init(
               &s7,
               "  \t\t  3)This is the 3rd bullet point of something"));
    EXPECT(true,rf_string_trim_start(&s7,RFS_(" \t123456789)")));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s7)), 0);

	return 0;
}
