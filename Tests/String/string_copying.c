#include <RFstring.h>
#include <stdio.h>

#include "../refu_tests.h"
int main()
{
	RF_String s1,*s2,s3,s4,s5;
	RF_StringX sx1,*sx2,sx3,sx4,*sx5,sx6;
	
	rfInit();
	
	//testing RF_String copying functions
	EXPECT(true,rfString_Init(&s1,"Millions of Americans are voting on whether to re-elect President Barack Obama or choose Mitt Romney with polls predicting a tight race."))
	EXPECTNOT(0,(s2=rfString_Copy_OUT(&s1)))
	printf("%s\n",rfString_Cstr(s2))
	EXPECT(true,rfString_PruneEnd(s2,14))
	rfString_Copy_IN(&s3,s2);
	printf("%s\n",rfString_Cstr(&s3))
	rfString_Copy_chars(&s4,&s3,21);
	printf("%s\n",rfString_Cstr(&s4))
	EXPECT(true,rfString_Init(&s5,"Ολι Ρεν: Αισιόδοξος για την εκταμίευση της δόσης στο Eurogroup Νοεμβρίου"))
 
	//testing RF_StringX copying functions from String
	rfStringX_FromString_IN(&sx1,&s1);
	printf("%s\n",rfString_Cstr(&sx1))
	EXPECTNOT(0,(sx2=rfStringX_FromString_OUT(&s5)))
	EXPECT(true,rfString_Equal(sx2,RFS_("Ολι Ρεν: Αισιόδοξος για την εκταμίευση της δόσης στο Eurogroup Νοεμβρίου")))
	
	//testing RF_StringX copying functions from StringX
	EXPECT(true,rfStringX_Init(&sx3,"日本野球機構は６日、１６日（福岡）、１８日（札幌）に開催される、野球日本代表とキューバ代表による国際試合「侍ジャパンマッチ２０１２」の出場２９選手を、発表した"))
	rfStringX_MoveForward(&sx3,10);
	rfStringX_Copy_IN(&sx4,&sx3);
	EXPECTNOT(0,(sx5=rfStringX_Copy_OUT(&sx3)))
	EXPECT(true,rfString_Equal(&sx4,RFS_("１６日（福岡）、１８日（札幌）に開催される、野球日本代表とキューバ代表による国際試合「侍ジャパンマッチ２０１２」の出場２９選手を、発表した")))
	EXPECT(true,rfString_Equal(sx5,RFS_("１６日（福岡）、１８日（札幌）に開催される、野球日本代表とキューバ代表による国際試合「侍ジャパンマッチ２０１２」の出場２９選手を、発表した")))
	
	//testing moving a stringx's internal pointer and then copying certain chars from it
	rfStringX_MoveForward(sx5,15);
	rfStringX_Copy_chars(&sx6,sx5,27);
	EXPECT(true,rfString_Equal(&sx6,RFS_("に開催される、野球日本代表とキューバ代表による国際試合")))
	return 0;
}