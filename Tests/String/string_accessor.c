#include <RFstring.h>
#include <stdlib.h>
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
    struct RFstring s1,s2;
    struct RFstringx sx1;
    uint32_t cp;
    char *cs;
    DEFAULT_LIB_INIT();

    //expect 131 : This string's length
    EXPECT(true,
           rf_string_init(
               &s1,
               "The president said he had set down a \"15-minute rule\""
               "with his team for response to governors and mayors seeking "
               "federal assistance."));
    EXPECT_MSG(131,rf_string_length(&s1),"String had unexpected length");

    //expect 123: This string's length
    EXPECT(true,
           rf_string_init(&s2,
                         "【北京＝峯村健司】中国共産党の第１７期中央委員会"
                         "第７回全体会議（７中全会）が１日、北京市内で開会"
                         "した模様だ。８日からの第１８回党大会の準備作業を"
                         "する。党指導部の世代交代が進むとみられる５年に１"
                         "度の党大会前後の一連の政治的な行事が本格的に始ま"
                         "った。"));
    EXPECT_MSG(123, rf_string_length(&s2), "String had unexpected length");
	
    //expect 1
    cs = rf_string_cstr(&s2);
    EXPECT_TRUE(checkEqual(utf8CString, cs));
    free(cs);
    //expect 84: (decimal value of 'T'
    EXPECT_TRUE(rf_string_get_char(&s1, 0, &cp));
    EXPECT_TRUE(cp == 84);
    //expect 29987: (decimal value of '産')
    EXPECT_TRUE(rf_string_get_char(&s2, 12, &cp));
    EXPECT_TRUE(cp == 29987);
    //expect 20581: (decimal value of the character at position 18 '健')
    //this is an internal function and will only work for non-continuation byte positions
    EXPECT(20581, rf_string_bytepos_to_codepoint(&s2,18));
    //expect 9: (character position of byte position 27 '中')
    //also an internal function
    EXPECT(9, rf_string_bytepos_to_charpos(&s2,27,false));
    //expect 9: (character position of byte position 29 and if it is a continuation byte take the previous char which is '中')
    EXPECT(9, rf_string_bytepos_to_charpos(&s2,29,true));
    //expect 10: (character position of byte position 28 and if it is a continuation byte take the next char which is '国')
    EXPECT(10, rf_string_bytepos_to_charpos(&s2,28,false));

    //testing null strings
    rf_stringx_null(&sx1);
    EXPECT_TRUE(rf_stringx_is_null(&sx1));
    rf_string_null(&s2);
    EXPECT_TRUE(rf_string_is_null(&s2));

    //testing empty string
    EXPECT_TRUE(rf_string_init(&s2, ""));
    EXPECT_TRUE(rf_string_is_empty(&s2));
    EXPECT_TRUE(rf_stringx_init(&sx1, ""));
    EXPECT_TRUE(rf_string_is_empty(&sx1));
	return 0;
}
