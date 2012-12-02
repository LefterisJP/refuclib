#include <RFstring.h>
#include <stdio.h>

#include "../refu_tests.h"
int main()
{
	RF_String s1,s2,s3,s4,s5,s6,s7;


	rfInit();


    //expect true and then the string without the \r
    EXPECT(true,rfString_Init(&s1,"If remove\r\ndoes not work correctly here\r\nthe test will surely\r\nfail!\r\n"))
    EXPECT(true,rfString_Remove(&s1,RFS_("\r")))
    printf("%s\n",rfString_Cstr(&s1));

    //expect true for all confirming that by removing we got left only with what we should be
    EXPECT(true,rfString_Init(&s2,"歌舞伎俳優の中村橋吾さん（３３）が都内の路上で見知らぬ男に突然顔を殴られる被害に遭っていたことが、警視庁への取材でわかった。同庁は暴行事件として調べている。"))
    EXPECT(true,rfString_Remove(&s2,RFS_("歌舞伎俳優の中村橋吾さん（３３）が都内の路上で見知らぬ男に突然顔を殴られる被害に遭っていたことが、警視庁への取材でわかった。")))
    EXPECT(true,rfString_Equal(&s2,RFS_("同庁は暴行事件として調べている。")))
    //expect false, can't remove that from the string since it is already removed
    EXPECT(false,rfString_Remove(&s2,RFS_("歌舞伎")))

    //expect true: keep only the hiragana(it's not the list of all the hiragana, must have forgotten something),the numbers and the full stop from this sentence
    EXPECT(true,rfString_Init(&s3,"その中の1人に突然顔を殴られた。けがはなかった。"))
    rfString_KeepOnly(&s3,RFS_("あいうえおかきくけこがぎぐげごさしすせそざじずぜぞらりるれろたちつてとだでどまみむめもなにぬねのはひふへほぱぴぷぺぽをわっ1234567890。"));
    EXPECT(true,rfString_Equal(&s3,RFS_("そのの1にをられた。けがはなかった。")))
    //expect true from all(all prunings should be succesfull)
    EXPECT(true,rfString_Init(&s4,"Βουτιά σημειώνει το Χρηματιστήριο την Πέμπτη, καθώς οι επενδυτές προχωρούν σε ρευστοποιήσεις σε όλο το φάσμα της αγοράς και κυρίως στις τράπεζες."))
    EXPECT(true,rfString_PruneStart(&s4,7))
    EXPECT(true,rfString_PruneEnd(&s4,10))
    EXPECT(true,rfString_PruneMiddleF(&s4,38,19))
    EXPECT(true,rfString_PruneMiddleB(&s4,65,28))
    //expect true: means we got the string we expected to get
    EXPECT(true,rfString_Equal(&s4,RFS_("σημειώνει το Χρηματιστήριο την Πέμπτη, σε όλο το φάσμα της αγοράς και κυρίως στις")))

    //expect the trimmed string: "This is a nice string but has lots of tabs, spaces and newlines on either end"
    EXPECT(true,rfString_Init(&s5,"   \t\t\t  This is a nice string but has lots of tabs, spaces and newlines on either end \n\n\n"))
    EXPECT(true,rfString_Trim(&s5,RFS_(" \n\t")))
    printf("%s\n",rfString_Cstr(&s5));

    //Trim all the hiragana and full width stop from the end of the string and check the string afterwards
    EXPECT(true,rfString_Init(&s6,"警視庁への取材でわかった。"))
    EXPECT(true,rfString_TrimEnd(&s6,RFS_("あいうえおかきくけこがぎぐげごさしすせそざじずぜぞらりるれろたちつてとだでどまみむめもなにぬねのはひふへほぱぴぷぺぽをわっ。")))
    EXPECT(true,rfString_Equal(&s6,RFS_("警視庁への取材")))
    //expect the string trimmed of all spaces,tabs and number-bullet points in the beginning
    EXPECT(true,rfString_Init(&s7,"  \t\t  3)This is the 3rd bullet point of something"))
    EXPECT(true,rfString_TrimStart(&s7,RFS_(" \t123456789)")))
    printf("%s\n",rfString_Cstr(&s7));

	return 0;
}
