#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
	  RFstring s1,s2,s3,s4;
	  RFstringx sx1,sx2, sx3;

    DEFAULT_LIB_INIT();

    EXPECT(true, rf_string_init(&s1,"The meaning of life"));
    EXPECT(rf_string_append(&s1, RFS_(" is doing what you love")), true);
    EXPECT(true,rf_string_init(&s2, " and 42"));
    EXPECT(rf_string_append(&s1,&s2), true);

    //expect the addition of the above sentence in stdout
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s1)), 0);

    //expect the additions of the sentences below in stdout
    EXPECT(true,rf_string_init(&s3,"A very interesting"));
    EXPECT(rf_string_append(
               &s3,
               RFS_(" %s is %s and its value is %.4f","number",
                    "the golden mean",1.6180339)), true);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s3)), 0);

    //expect true if the additions of these are correct
    EXPECT(true,rf_string_init(&s4,"東京"));
    EXPECT(rf_string_prepend(&s4,RFS_("日本の首都は")), true);
    EXPECT(rf_string_append(&s4,RFS_("だと思います。")), true);
    EXPECT(rf_string_append(&s4,RFS_("ちなみに今日は%d日です。",26)), true);
    EXPECT_MSG(true,
               rf_string_equal(
                   &s4,
                   RFS_("日本の首都は東京だと思います。ちなみに今日は26日"
                        "です。")),
               "The Japanese strings did not get added"
               " correctly");

    ///RF_STringX testing
    EXPECT(true,
           rf_stringx_init(
               &sx1,
               "I am on the train from Kita Senju to Kashiwa station."));
    EXPECT(
        true,
        rf_stringx_init(&sx2,
                       "Barack Obama has won a second term as US president "
                       "after winning the crucial battleground of Ohio, "
                       "taking him past the 270 margin."));
    rf_stringx_move_forward(&sx1, 8);
    EXPECT(rf_stringx_append(
               &sx1,
               RFS_("研究室に行ってきます。")),
           true);
    EXPECT(true,
           rf_string_equal(
               &sx1,
               RFS_("the train from Kita Senju to Kashiwa station."
                    "研究室に行ってきます。")));
    EXPECT(rf_stringx_prepend(
               &sx2,
               RFS_("オバマ氏再選　ロムニー氏を振り切る　米大統領選 || ")),
           true);
    EXPECT(true,
           rf_string_equal(
               &sx2,
               RFS_("オバマ氏再選　ロムニー氏を振り切る　米大統領選 || Barack"
                    " Obama has won a second term as US president after "
                    "winning the crucial battleground of Ohio, taking him "
                    "past the 270 margin.")));
    rf_stringx_move_forward(&sx2,7);

    EXPECT(rf_stringx_insert(
               &sx2,
               17,
               RFS_("|| Δεύτερη θητεία στην προεδρία των ΗΠΑ κατέκτησε ο "
                    "Μπαράκ Ομπάμα ")),
           true);
    EXPECT(true,
           rf_string_equal(
               &sx2,
               RFS_("ロムニー氏を振り切る　米大統領選 || Δεύτερη θητεία "
                    "στην προεδρία των ΗΠΑ κατέκτησε ο Μπαράκ Ομπάμα || "
                    "Barack Obama has won a second term as US president "
                    "after winning the crucial battleground of Ohio, "
                    "taking him past the 270 margin.")));


    EXPECT(rf_stringx_init(&sx3, "Testing appending a cha"),
           true);
    EXPECT(rf_stringx_append_char(&sx3, (uint32_t)'r'), true);
    EXPECT(rf_string_equal(
               &sx3, RFS_("Testing appending a char")), true);



	return 0;
}
