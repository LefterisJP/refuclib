#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    struct RFstring s1,*s2,s3,s4,s5;
    struct RFstringx sx1,*sx2,sx3,sx4,*sx5,sx6;
    DEFAULT_LIB_INIT();
	
    //testing RFstring copying functions
    EXPECT(true,
           rf_string_init(
               &s1,
               "Millions of Americans are voting on whether to re-elect "
               "President Barack Obama or choose Mitt Romney with polls "
               "predicting a tight race."));
    EXPECT_NOT(0, s2=rf_string_copy_out(&s1));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(s2)), 0);
    EXPECT(true,rf_string_prune_end(s2,14));
    EXPECT(rf_string_copy_in(&s3,s2), true);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s3)), 0);
    EXPECT(rf_string_copy_chars(&s4,&s3,21), true);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s4)), 0);
    EXPECT(true,
           rf_string_init(
               &s5,
               "Ολι Ρεν: Αισιόδοξος για την εκταμίευση της δόσης στο "
               "Eurogroup Νοεμβρίου"));
 
	//testing RFstringx copying functions from String
    EXPECT(rf_stringx_from_string_in(&sx1,&s1), true);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&sx1)), 0);
    EXPECT_NOT(0, sx2=rf_stringx_from_string_out(&s5));
    EXPECT(true,
           rf_string_equal(
               sx2,
               RFS_("Ολι Ρεν: Αισιόδοξος για την εκταμίευση της δόσης στο "
                    "Eurogroup Νοεμβρίου")));
	
	//testing RFstringx copying functions from StringX
    EXPECT(true,
           rf_stringx_init(
               &sx3,
             "日本野球機構は６日、１６日（福岡）、１８日（札幌）に開催され"
               "る、野球日本代表とキューバ代表による国際試合「侍ジャパンマ"
               "ッチ２０１２」の出場２９選手を、発表した"));
    rf_stringx_move_forward(&sx3,10);
    EXPECT(rf_stringx_copy_in(&sx4,&sx3), true);
    EXPECT_NOT(0, sx5=rf_stringx_copy_out(&sx3));
    EXPECT(true,
           rf_string_equal(
               &sx4,
               RFS_("１６日（福岡）、１８日（札幌）に開催される、野球日本代表と"
                    "キューバ代表による国際試合「侍ジャパンマッチ２０１２」の出"
                    "場２９選手を、発表した")));
    EXPECT(true,
           rf_string_equal(
               sx5,
               RFS_("１６日（福岡）、１８日（札幌）に開催される、野球日本"
                    "代表とキューバ代表による国際試合「侍ジャパンマッチ２"
                    "０１２」の出場２９選手を、発表した")));
	
	  //testing moving a stringx's internal pointer and then copying certain chars from it
    rf_stringx_move_forward(sx5,15);
    EXPECT(rf_stringx_copy_chars(&sx6,sx5,27), true);
    EXPECT(true,
           rf_string_equal(&sx6,
                          RFS_("に開催される、野球日本代表とキューバ代表に"
                               "よる国際試合")));

	return 0;
}
