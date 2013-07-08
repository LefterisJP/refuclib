#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
	  RF_String s1,s2,s3,s4;
	  RF_StringX sx1,sx2;

	  EXPECT(rfInit(), true);

    EXPECT(true, rfString_Init(&s1,"The meaning of life"));
    EXPECT(rfString_Append(&s1, RFS_(" is doing what you love")), true);
    EXPECT(true,rfString_Init(&s2, " and 42"));
    EXPECT(rfString_Append(&s1,&s2), true);

    //expect the addition of the above sentence in stdout
    EXPECTGE(rfPrintf("%S\n",&s1), 0);

    //expect the additions of the sentences below in stdout
    EXPECT(true,rfString_Init(&s3,"A very interesting"));
    EXPECT(rfString_Append(
               &s3,
               RFS_(" %s is %s and its value is %.4f","number",
                    "the golden mean",1.6180339)), true);
    EXPECTGE(rfPrintf("%S\n",&s3), 0);

    //expect true if the additions of these are correct
    EXPECT(true,rfString_Init(&s4,"東京"));
    EXPECT(rfString_Prepend(&s4,RFS_("日本の首都は")), true);
    EXPECT(rfString_Append(&s4,RFS_("だと思います。")), true);
    EXPECT(rfString_Append(&s4,RFS_("ちなみに今日は%d日です。",26)), true);
    EXPECT_MSG(true,rfString_Equal(
                   &s4,
                   RFS_("日本の首都は東京だと思います。ちなみに今日は26日"
                        "です。")),"The Japanese strings did not get added"
               " correctly");

    ///RF_STringX testing
    EXPECT(true,
           rfStringX_Init(
               &sx1,
               "I am on the train from Kita Senju to Kashiwa station."));
    EXPECT(
        true,
        rfStringX_Init(&sx2,
                       "Barack Obama has won a second term as US president "
                       "after winning the crucial battleground of Ohio, "
                       "taking him past the 270 margin."));
    rfStringX_MoveForward(&sx1,8);
    EXPECT(rfStringX_Append(
               &sx1,
               RFS_("研究室に行ってきます。")),
           true);
    EXPECT(true,
           rfString_Equal(
               &sx1,
               RFS_("the train from Kita Senju to Kashiwa station."
                    "研究室に行ってきます。")));
    EXPECT(rfStringX_Prepend(
               &sx2,
               RFS_("オバマ氏再選　ロムニー氏を振り切る　米大統領選 || ")),
           true);
    EXPECT(true,
           rfString_Equal(
               &sx2,
               RFS_("オバマ氏再選　ロムニー氏を振り切る　米大統領選 || Barack"
                    " Obama has won a second term as US president after "
                    "winning the crucial battleground of Ohio, taking him "
                    "past the 270 margin.")));
    rfStringX_MoveForward(&sx2,7);

    EXPECT(rfStringX_Insert(
               &sx2,
               17,
               RFS_("|| Δεύτερη θητεία στην προεδρία των ΗΠΑ κατέκτησε ο "
                    "Μπαράκ Ομπάμα ")),
           true);
    EXPECT(true,
           rfString_Equal(
               &sx2,
               RFS_("ロムニー氏を振り切る　米大統領選 || Δεύτερη θητεία "
                    "στην προεδρία των ΗΠΑ κατέκτησε ο Μπαράκ Ομπάμα || "
                    "Barack Obama has won a second term as US president "
                    "after winning the crucial battleground of Ohio, "
                    "taking him past the 270 margin.")));
	return 0;
}
