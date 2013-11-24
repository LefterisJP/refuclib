#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    RF_String s1,s2,s3;
    RF_StringX sx1;


    EXPECT(rfInit(), true);


    //expect the fully formed compile command "gcc -c -I../../includes -Wall -O2 tocompile.c -o ./obj/tocompile.o"
    EXPECT(true,
           rfString_Init(
               &s1,
               "gcc -c -I$INCLUDES $FLAGS $FILE.c -o ./$OBJDIRNAME/$FILE.o"));
    EXPECT(true,
           rfString_Replace(
               &s1,RFS_("$INCLUDES"),RFS_("../../includes"),0,0));
    EXPECT(true,rfString_Replace(&s1,RFS_("$FLAGS"),RFS_("-Wall -O2"),0,0));
    EXPECT(true,rfString_Replace(&s1,RFS_("$FILE"),RFS_("tocompile"),0,0));
    EXPECT(true,rfString_Replace(&s1,RFS_("$OBJDIRNAME"),RFS_("obj"),0,0));
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s1)), 0);

    //expect the same string with some kanji replaced by their English equivalent
    EXPECT(true,
           rfString_Init(
               &s2,
               "中国メディアはＪ３１について、９月に就役した初の空母「遼"
               "寧」号などに搭載する新型艦載機や輸出用戦闘機になるとの見"
               "方を伝えている。"));
    EXPECT(true,
           rfString_Replace(
               &s2,RFS_("中国メディア"),RFS_("Chinese media"),0,0));
    EXPECT(true,rfString_Replace(&s2,RFS_("９月"),RFS_("September"),0,0));
    EXPECT(true,
           rfString_Replace(&s2,RFS_("就役した"),RFS_("commisioned"),0,0));
    EXPECT(true,
           rfString_Replace(&s2,RFS_("初の空母"),RFS_(" new Carrier"),0,0));
    EXPECT(true,
           rfString_Replace(&s2,RFS_("搭載する"),RFS_("load"),0,0));
    EXPECT(true,
           rfString_Replace(&s2,
                            RFS_("輸出用戦闘機"),
                            RFS_("Fighter used for exporting"),0,0));
    EXPECT(true,
           rfString_Equal(
               &s2,
               RFS_("Chinese mediaはＪ３１について、Septemberにcommisioned"
                    " new Carrier「遼寧」号などにload新型艦載機やFighter "
                    "used for exportingになるとの見方を伝えている。")));

    //expect the same String only with many english words replaced with Greek equivalents and check for equality
    EXPECT(true,
           rfString_Init(
               &s3,
               "Journalist Costas Vaxevanis has gone on trial in Athens "
               "for breach of privacy after publishing the names of 2,000 "
               "Greeks with Swiss bank accounts"));
    EXPECT(true,
           rfString_Replace(
               &s3,RFS_("Journalist"),RFS_("Ο Δημοσιογραφος"),0,0));
    EXPECT(true,
           rfString_Replace(&s3,
                            RFS_("for breach of privacy"),
                            RFS_("για καταπατηση προσωπικων δικαιωματων"),
                            0,0));
    EXPECT(true,
           rfString_Replace(
               &s3,
               RFS_("after publishing"),
               RFS_("αφου δημοσιευσε"),0,0));
    EXPECT(true,
           rfString_Replace(&s3,RFS_("the names"),RFS_("τα ονοματα"),0,0));
    EXPECT(true,
           rfString_Replace(
               &s3,
               RFS_("with Swiss bank accounts"),
               RFS_("με λογαριασμους σε Ελβετικες τραπεζες"),0,0));
    EXPECT(true,
           rfString_Equal(
               &s3,
               RFS_("Ο Δημοσιογραφος Costas Vaxevanis has gone on trial in"
                    " Athens για καταπατηση προσωπικων δικαιωματων αφου "
                    "δημοσιευσε τα ονοματα of 2,000 Greeks με "
                    "λογαριασμους σε Ελβετικες τραπεζες")));


	//Stringx replacing
    EXPECT(true,rfStringX_Init(&sx1,"News from all over the world."));
    EXPECTNOT(RF_FAILURE,rfStringX_MoveAfter(&sx1,RFS_("News ")));
    EXPECT(true,
           rfStringX_Replace(
               &sx1,
               RFS_("."),
               RFS_(" || 投稿動画を参考に捜査…タクシー運転手殴った疑いで"
                    "逮捕 ||"),0,0));
	EXPECT(true,
         rfString_Equal(
             &sx1,
             RFS_("from all over the world || 投稿動画を参考に捜査…タク"
                  "シー運転手殴った疑いで逮捕 ||")));
	
	EXPECT(true,
         rfStringX_ReplaceBetween(
             &sx1,
             RFS_("||"),
             RFS_(" "),
             RFS_("Φοβερη ειδηση που μολις βγηκε απο το το γραφειο ειδησεων. Η Μαρικα παντρευεται. Ναι δεν κανουμε πλακα, η Μαρικα σοβαρα "
                  "παντρευεται με τον Τοτο. Μιλαμε για νεα βιβλικων "
                  "διαστασεων φιλε μου ||"),0,1));
	EXPECT(true,
         rfString_Equal(
             &sx1,
             RFS_("from all over the world ||Φοβερη ειδηση που μολις "
                  "βγηκε απο το το γραφειο ειδησεων. Η Μαρικα παντρευεται"
                  ". Ναι δεν κανουμε πλακα, η Μαρικα σοβαρα παντρευεται "
                  "με τον Τοτο. Μιλαμε για νεα βιβλικων διαστασεων φιλε "
                  "μου || 投稿動画を参考に捜査…タクシー運転手殴った"
                  "疑いで逮捕 ||")));
	
	return 0;
}
