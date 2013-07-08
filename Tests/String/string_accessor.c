#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>


#include "../refu_tests.h"
char utf8CString[] = {"【北京＝峯村健司】中国共産党の第１７期中央委員会第７回全体会議（７中全会）が１日、北京市内で開会した模様だ。８日からの第１８回党大会\
の準備作業をする。党指導部の世代交代が進むとみられる５年に１度の党大会前後の一連の政治的な行事が本格的に始まった。"};
//checks if 2 utf8 buffers are equal
int checkEqual(char* b1,char* b2)
{
    uint32_t i=0;
    while(b1[i] != 0)
    {
        if(b1[i] != b2[i])
            return 0;
        i++;
    }
    return 1;
}

int main()
{
    RF_String s1,s2;
    
    EXPECT(rfInit(), true);

    //expect 131 : This string's length
    EXPECT(true,
           rfString_Init(
               &s1,
               "The president said he had set down a \"15-minute rule\""
               "with his team for response to governors and mayors seeking "
               "federal assistance."));
    EXPECT_MSG(131,rfString_Length(&s1),"String had unexpected length");

    //expect 123: This string's length
    EXPECT(true,
           rfString_Init(&s2,
                         "【北京＝峯村健司】中国共産党の第１７期中央委員会"
                         "第７回全体会議（７中全会）が１日、北京市内で開会"
                         "した模様だ。８日からの第１８回党大会の準備作業を"
                         "する。党指導部の世代交代が進むとみられる５年に１"
                         "度の党大会前後の一連の政治的な行事が本格的に始ま"
                         "った。"));
    EXPECT_MSG(123,rfString_Length(&s2),"String had unexpected length");
	
    //expect 1
    EXPECT(true, checkEqual(utf8CString,rfString_Cstr(&s2)));
    //expect 84: (decimal value of 'T'
    EXPECT(84,rfString_GetChar(&s1,0));
    //expect 29987: (decimal value of '産')
    EXPECT(29987,rfString_GetChar(&s2,12));
    //expect 20581: (decimal value of the character at position 18 '健')
    //this is an internal function and will only work for non-continuation byte positions
    EXPECT(20581,rfString_BytePosToCodePoint(&s2,18));
    //expect 9: (character position of byte position 27 '中')
    //also an internal function
    EXPECT(9,rfString_BytePosToCharPos(&s2,27,false));
    //expect 9: (character position of byte position 29 and if it is a continuation byte take the previous char which is '中')
    EXPECT(9,rfString_BytePosToCharPos(&s2,29,true));
    //expect 10: (character position of byte position 28 and if it is a continuation byte take the next char which is '国')
    EXPECT(10,rfString_BytePosToCharPos(&s2,28,false));


	return 0;
}
