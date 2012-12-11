#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>
#include <stdlib.h>


#include "../refu_tests.h"
//the check for UTF16 conversion of the Japanese(Adachiku) + MusicalSymbol(G clef) string
int checkUTF16(uint16_t* buff)
{
    if(buff[0] == 0x8DB3 &&
       buff[1] == 0x7ACB &&
       buff[2] == 0x533A &&
       buff[3] == 0xD834 &&
       buff[4] == 0xDD1E &&
       buff[5] == 0x0)
        return 1;
    else
        return 0;
}
//the check for UTF32 conversion of the Japanese(Adachiku) + MusicalSymbol(G clef) string
int checkUTF32(uint32_t* buff)
{
    if(buff[0] == 0x8DB3 &&
       buff[1] == 0x7ACB &&
       buff[2] == 0x533A &&
       buff[3] == 0x1D11E &&
       buff[4] == 0x0)
        return 1;
    else
        return 0;
}

int main()
{
	RF_String s1,s2,s3,s4,s5,s6,s7,s8;
	RF_String* words;
	uint32_t length,wordsN,i;
	int num;
	double dbl;

	rfInit();

    //testing to lowercase and to uppercase
	EXPECT(true,rfString_Init(&s1,"ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	rfString_ToLower(&s1);
	EXPECT(true,rfString_Init(&s2,"abcdefghijklmnopqrstuvwxyz"));
    rfString_ToUpper(&s2);
    rfPrintf("%S\n",&s1);
    rfPrintf("%S\n",&s2);


    //first create the string to test. Japanese(Adachiku) + MusicalSymbol(G clef)
    EXPECT(true,rfString_Init_cp(&s4,0x1D11E));
    EXPECT(true,rfString_Init(&s3,"足立区"));
    rfString_Append(&s3,&s4);
    //testing conversion to UTF16 and to UTF32
    EXPECT(1,checkUTF32(rfString_ToUTF32(&s3,&length)));
    EXPECT(1,checkUTF16(rfString_ToUTF16(&s3,&length)));

    //expect 0(false) and 1(true) and 15042
    rfString_Init(&s5,"15042");
    EXPECT(false,rfString_ToInt(&s3,&num));
    EXPECT(true,rfString_ToInt(&s5,&num));
    rfPrintf("%d\n",num);

    //expect RE_STRING_TOFLOAT and RF_SUCCESS and 3.141592
    EXPECT(true,rfString_Init(&s6,"3.141592"));
    EXPECT(RE_STRING_TOFLOAT,rfString_ToDouble(&s3,&dbl));
    EXPECT(RF_SUCCESS,rfString_ToDouble(&s6,&dbl));
    rfPrintf("%f\n",dbl);

    //expect the sentence separated in words
    EXPECT(true,rfString_Init(&s7,"Drug companies are facing mounting pressure to investigate reports that new medicines are being tested on some of the poorest people in India without their knowledge"));
    EXPECT(true,rfString_Tokenize(&s7,RFS_(" "),&wordsN,&words));
    for(i=0;i<wordsN;i++)
    {
        rfPrintf("%S\n",&words[i]);
    }
    //free stuff
    for(i = 0; i < wordsN; i++)
    {
        rfString_Deinit(&words[i]);
    }
    free(words);

    //expect false, no commas in there
    EXPECT(false,rfString_Tokenize(&s7,RFS_(","),&wordsN,&words));

    //expect 10 (number of prefectures) and all true string comparisons marking the correct separation of Japanese prefectures by the prefecture kanji '県'
    //notice the last prefecture is missing its '県' kanji to not get an extra empty string token
    EXPECT(true,rfString_Init(&s8,"新潟県富山県石川県福井県山梨県長野県岐阜県静岡県愛知県青森"));
    EXPECT(true,rfString_Tokenize(&s8,RFS_("県"),&wordsN,&words));
    EXPECT_MSG(10,wordsN,"Tokenization of some Japanese provinces did not give the expected tokens number");
    EXPECT(true,rfString_Equal(RFS_("新潟"),&words[0]));//Niigata
    EXPECT(true,rfString_Equal(RFS_("富山"),&words[1]));//Toyama
    EXPECT(true,rfString_Equal(RFS_("石川"),&words[2]));//Ishikawa
    EXPECT(true,rfString_Equal(RFS_("福井"),&words[3]));//Fukui
    EXPECT(true,rfString_Equal(RFS_("山梨"),&words[4]));//Yamanashi
    EXPECT(true,rfString_Equal(RFS_("長野"),&words[5]));//Nagoya
    EXPECT(true,rfString_Equal(RFS_("岐阜"),&words[6]));//Gifu
    EXPECT(true,rfString_Equal(RFS_("静岡"),&words[7]));//Shizuoka
    EXPECT(true,rfString_Equal(RFS_("愛知"),&words[8]));//Aichi
    EXPECT(true,rfString_Equal(RFS_("青森"),&words[9]));//Aomori






	return 0;
}
