#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    struct RFstringx sx1,sx2,sx3,resx1;
    struct RFstring res1,res2,res3;
	
    DEFAULT_LIB_INIT();
	
    EXPECT(true,
           rf_stringx_init(
               &sx1,"中国共産党総書記に習近平氏　新指導部の７人発表"));
    EXPECT_NOT(RF_FAILURE,rf_stringx_move_after(&sx1,RFS_("に"),&res1,0));
    EXPECT(true,rf_string_equal(&sx1,RFS_("習近平氏　新指導部の７人発表")));
    EXPECT(true,rf_string_equal(&res1,RFS_("中国共産党総書記")));
	
    EXPECT(true,
           rf_stringx_init(
               &sx2,
               "Με την κεντρική εκδήλωση, μετά τις 2 το μεσημέρι, και την "
               "πορεία προς την αμερικανική πρεσβεία, περίπου μία ώρα "
               "αργότερα, κορυφώνονται το Σάββατο οι εκδηλώσεις για την "
               "39η επέτειο από την Εξέγερση του Πολυτεχνείου. Λόγω των "
               "εκδηλώσεων, θα ισχύσουν έκτακτες κυκλοφοριακές ρυθμίσεις "
               "έως και το πρωί της Κυριακής."));
    rf_stringx_move_forward(&sx2,96);
    EXPECT(true,
           rf_string_equal(
               &sx2,
               RFS_("περίπου μία ώρα αργότερα, κορυφώνονται το Σάββατο οι "
                    "εκδηλώσεις για την 39η επέτειο από την Εξέγερση του "
                    "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
                    "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
                    "της Κυριακής.")));
    rf_stringx_move_back(&sx2,46);
    EXPECT(true,
           rf_string_equal(
               &sx2,
               RFS_("και την πορεία προς την αμερικανική πρεσβεία, περίπου"
                    " μία ώρα αργότερα, κορυφώνονται το Σάββατο οι "
                    "εκδηλώσεις για την 39η επέτειο από την Εξέγερση του "
                    "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
                    "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
                    "της Κυριακής.")));
	
    rf_stringx_reset(&sx1);
    EXPECT(true,
           rf_string_equal(
               &sx1,
               RFS_("中国共産党総書記に習近平氏　新指導部の７人発表")));
    EXPECT_NOT(RF_FAILURE,
              rf_stringx_move_after(&sx1,RFS_("　"),
                                  &resx1,
                                  RF_STRINGX_ARGUMENT));
    rf_stringx_move_forward(&resx1,3);
    EXPECT(true,rf_string_equal(&resx1,RFS_("産党総書記に習近平氏")));
    EXPECT(true,rf_string_equal(&sx1,RFS_("新指導部の７人発表")));
	
    EXPECT(true,rf_stringx_move_afterv(&sx2,&res2,0,2,RFS_(","),RFS_("39")));
    EXPECT(true,
           rf_string_equal(
               &sx2,
               RFS_(" περίπου μία ώρα αργότερα, κορυφώνονται το Σάββατο οι"
                    " εκδηλώσεις για την 39η επέτειο από την Εξέγερση του "
                    "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
                    "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί της"
                    " Κυριακής.")));
    EXPECT(true,
           rf_string_equal(
               &res2,
               RFS_("και την πορεία προς την αμερικανική πρεσβεία")));
	
    EXPECT(true,
           rf_stringx_init(
               &sx3,
               "これがMoveAfterPairのテストですね！「Δεν θα επιστραφω εγω"
               "」　Γεμιζουμε το στρινγκ με διαφορα 「ブラケットの中のテ"
               "キストは結果になる」Let's see if the function will work "
               "as expected."))	;
    EXPECT(true,
           rf_stringx_move_after_pair(&sx3,RFS_("「"),RFS_("」"),&res3,0,2));
    EXPECT(true,
           rf_string_equal(
               &sx3,
               RFS_("Let's see if the function will work as expected.")));
    EXPECT(true,
           rf_string_equal(
               &res3,
               RFS_("ブラケットの中のテキストは結果になる")));
	
	return 0;
}
