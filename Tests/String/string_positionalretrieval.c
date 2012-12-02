#include <RFstring.h>
#include <stdio.h>

#include "../refu_tests.h"
int main()
{
	RF_String s1,s2,s3,s4,s5;
	RF_String ret,ret2;
	RF_StringX retx1;
    int year;


	rfInit();

    EXPECT(true,rfString_Init(&s1,"Υπερψηφίστηκε με σημαντικές απώλειες στους κόλπους της συγκυβέρνησης το σχέδιο νόμου του υπουργείου Οικονομικών που αφορά στη μείωση του ποσοστού του Δημοσίου σε ΔΕΚΟ. \
Αντίθετα, απορρίφθηκε η επίμαχη τροπολογία για την ένταξη του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ. Νωρίτερα ο υπουργός Οικονομικών Γιάννης Στουρνάρας κατέθεσε στο προεδρείο τον προϋπολογισμό του 2013.\
Απορρίφθηκε επίσης οριακά το άρθρο 4 που αφορά τις εισφορές του ΤΕΕ"))

    //expect true and 2013: (the number coming after the given substring)
    EXPECT(true,rfString_ScanfAfter(&s1,RFS_("τον προϋπολογισμό του "),"%d",&year))
    EXPECT(2013,year)
    //expect (true) and (true) that this is actually the found substring
    EXPECT(true,rfString_Between(&s1,RFS_("Αντίθετα, "),RFS_("του ΕΤΑΠ-ΜΜΕ στον ΕΟΠΥΥ."),&ret,0))
    EXPECT(true,rfString_Equal(&ret,RFS_("απορρίφθηκε η επίμαχη τροπολογία για την ένταξη ")))
    //expect (false), so no substring was found
    EXPECT(false,rfString_Between(&s1,RFS_("Αντίθετα, "),RFS_("I am not in there"),&ret,0))
    //expect (false), so no substring was found
    EXPECT(false,rfString_Between(&s1,RFS_("I am not in there either"),RFS_("I am not in there"),&ret,0))

    rfString_Deinit(&ret);

    EXPECT(true,rfString_Init(&s2,"I Am A StRinG wItH PecULIAR LeTteRs caSE"))
    //expect (true), and "PecULIAR" which is the substring
    EXPECT(true,rfString_Between(&s2,RFS_("I am a string with "),RFS_(" Letters"),&ret,RF_CASE_IGNORE))
    printf("%s\n",rfString_Cstr(&ret));
    rfString_Deinit(&ret);

    //expect true from both and <meta name="application-name
    EXPECT(true,rfString_Init(&s3,"<meta name=\"application-name\" content=\"BBC News\" />"))
    EXPECT(true,rfString_Beforev(&s3,&ret,0,2,RFS_("/>"),RFS_("\" ")))
    printf("%s\n",rfString_Cstr(&ret));
    //expect false
    EXPECT(false,rfString_Beforev(&s3,&ret,0,4,RFS_("56"),RFS_("@"),RFS_("ένταξη"),RFS_("Not in there")))
    rfString_Deinit(&ret);

    //expect true from all: meaning we did indeed get the substring we were expecting
    EXPECT(true,rfString_Init(&s4,"神奈川県の黒岩知事は３１日の定例記者会見で、次期衆院選をにらみ、都知事を辞職した石原慎太郎氏が新党を作り、\
日本維新の会などと民主、自民両党に対抗する「第３極」の結集を図ろうとしていることについて、「政策なんかどうでもいいという、ある種の暴論だ」と痛烈に批判した"))
    EXPECT(true,rfString_Before(&s4,RFS_("は"),&ret,0))
    EXPECT(true,rfString_Equal(&ret,RFS_("神奈川県の黒岩知事")))
    //expect (false)
    EXPECT(false,rfString_Before(&s4,RFS_("東京"),&ret,0))
    rfString_Deinit(&ret);

    //expect false due to the RF_MATCH_WORD flag .. which btw should never be used in Japanese text since words are not separated by spaces
    EXPECT(false,rfString_After(&s4,RFS_("「第３極」の"),&ret,RF_MATCH_WORD))
    //expect (true) and then (true): meaning we did indeed find the expected substring
    EXPECT(true,rfString_After(&s4,RFS_("「第３極」の"),&ret,0))
    EXPECT(true,rfString_Equal(&ret,RFS_("結集を図ろうとしていることについて、「政策なんかどうでもいいという、ある種の暴論だ」と痛烈に批判した")))
    rfString_Deinit(&ret);

    EXPECT(true,rfString_Init(&s5,"神奈川県,福島県,和歌山県"))
    EXPECT(true,rfString_Afterv(&s5,&retx1,RF_STRINGX_ARGUMENT,2,RFS_(" "),RFS_(",")))
    EXPECT(true,rfString_Before(&retx1,RFS_(","),&ret2,0))
    EXPECT(true,rfString_Equal(&ret2,RFS_("福島県")))
	rfStringX_MoveForward(&retx1,4);
	EXPECT(true,rfString_Equal(&retx1,RFS_("和歌山県")))

	return 0;
}
