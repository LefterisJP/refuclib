#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    RF_StringX sx1,sx2,sx3,resx1;
    RF_String res1,res2,res3;
	
    EXPECT(rfInit(), true);
	
    EXPECT(true,
           rfStringX_Init(
               &sx1,"中国共産党総書記に習近平氏　新指導部の７人発表"));
    EXPECTNOT(RF_FAILURE,rfStringX_MoveAfter(&sx1,RFS_("に"),&res1,0));
    EXPECT(true,rfString_Equal(&sx1,RFS_("習近平氏　新指導部の７人発表")));
    EXPECT(true,rfString_Equal(&res1,RFS_("中国共産党総書記")));
	
    EXPECT(true,
           rfStringX_Init(
               &sx2,
               "Με την κεντρική εκδήλωση, μετά τις 2 το μεσημέρι, και την "
               "πορεία προς την αμερικανική πρεσβεία, περίπου μία ώρα "
               "αργότερα, κορυφώνονται το Σάββατο οι εκδηλώσεις για την "
               "39η επέτειο από την Εξέγερση του Πολυτεχνείου. Λόγω των "
               "εκδηλώσεων, θα ισχύσουν έκτακτες κυκλοφοριακές ρυθμίσεις "
               "έως και το πρωί της Κυριακής."));
    rfStringX_MoveForward(&sx2,96);
    EXPECT(true,
           rfString_Equal(
               &sx2,
               RFS_("περίπου μία ώρα αργότερα, κορυφώνονται το Σάββατο οι "
                    "εκδηλώσεις για την 39η επέτειο από την Εξέγερση του "
                    "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
                    "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
                    "της Κυριακής.")));
    rfStringX_MoveBack(&sx2,46);
    EXPECT(true,
           rfString_Equal(
               &sx2,
               RFS_("και την πορεία προς την αμερικανική πρεσβεία, περίπου"
                    " μία ώρα αργότερα, κορυφώνονται το Σάββατο οι "
                    "εκδηλώσεις για την 39η επέτειο από την Εξέγερση του "
                    "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
                    "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί "
                    "της Κυριακής.")));
	
    rfStringX_Reset(&sx1);
    EXPECT(true,
           rfString_Equal(
               &sx1,
               RFS_("中国共産党総書記に習近平氏　新指導部の７人発表")));
    EXPECTNOT(RF_FAILURE,
              rfStringX_MoveAfter(&sx1,RFS_("　"),
                                  &resx1,
                                  RF_STRINGX_ARGUMENT));
    rfStringX_MoveForward(&resx1,3);
    EXPECT(true,rfString_Equal(&resx1,RFS_("産党総書記に習近平氏")));
    EXPECT(true,rfString_Equal(&sx1,RFS_("新指導部の７人発表")));
	
    EXPECT(true,rfStringX_MoveAfterv(&sx2,&res2,0,2,RFS_(","),RFS_("39")));
    EXPECT(true,
           rfString_Equal(
               &sx2,
               RFS_(" περίπου μία ώρα αργότερα, κορυφώνονται το Σάββατο οι"
                    " εκδηλώσεις για την 39η επέτειο από την Εξέγερση του "
                    "Πολυτεχνείου. Λόγω των εκδηλώσεων, θα ισχύσουν "
                    "έκτακτες κυκλοφοριακές ρυθμίσεις έως και το πρωί της"
                    " Κυριακής.")));
    EXPECT(true,
           rfString_Equal(
               &res2,
               RFS_("και την πορεία προς την αμερικανική πρεσβεία")));
	
    EXPECT(true,
           rfStringX_Init(
               &sx3,
               "これがMoveAfterPairのテストですね！「Δεν θα επιστραφω εγω"
               "」　Γεμιζουμε το στρινγκ με διαφορα 「ブラケットの中のテ"
               "キストは結果になる」Let's see if the function will work "
               "as expected."))	;
    EXPECT(true,
           rfStringX_MoveAfterPair(&sx3,RFS_("「"),RFS_("」"),&res3,0,2));
    EXPECT(true,
           rfString_Equal(
               &sx3,
               RFS_("Let's see if the function will work as expected.")));
    EXPECT(true,
           rfString_Equal(
               &res3,
               RFS_("ブラケットの中のテキストは結果になる")));
	
	return 0;
}
