#include <RFstring.h>
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
    RFstring s1,s2,s3,s4,s5,s6,s7,s8;
    RFstring* words;
    uint32_t length,wordsN,i;
    int num;
    double dbl;

    DEFAULT_LIB_INIT();

    //testing to lowercase and to uppercase
    EXPECT(true,rf_string_init(&s1,"ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    rf_string_to_lower(&s1);
    EXPECT(true,rf_string_init(&s2,"abcdefghijklmnopqrstuvwxyz"));
    rf_string_to_upper(&s2);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s1)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s2)), 0);


    //first create the string to test. Japanese(Adachiku) + MusicalSymbol(G clef)
    EXPECT(true,rf_string_init_cp(&s4,0x1D11E));
    EXPECT(true,rf_string_init(&s3,"足立区"));
    rf_string_append(&s3,&s4);
    //testing conversion to UTF16 and to UTF32
    EXPECT(1,checkUTF32(rf_string_to_utf32(&s3,&length)));
    EXPECT(1,checkUTF16(rf_string_to_utf16(&s3,&length)));

    //expect 0(false) and 1(true) and 15042
    EXPECT(rf_string_init(&s5,"15042"), true);
    EXPECT(false, rf_string_to_int(&s3,&num));
    EXPECT(true,rf_string_to_int(&s5,&num));
    EXPECTGE(printf("%d\n",num), 0);

    //expect RE_STRING_TOFLOAT and RF_SUCCESS and 3.141592
    EXPECT(true,rf_string_init(&s6,"3.141592"));
    EXPECT(false ,rf_string_to_double(&s3,&dbl));
    EXPECT(true, rf_string_to_double(&s6,&dbl));
    EXPECTGE(printf("%f\n",dbl), 0);

    //expect the sentence separated in words
    EXPECT(true,
           rf_string_init(
               &s7,
               "Drug companies are facing mounting pressure to investigate"
               " reports that new medicines are being tested on some of "
               "the poorest people in India without their knowledge"));
    EXPECT(true,rf_string_tokenize(&s7,RFS_(" "),&wordsN,&words));
    for(i=0;i<wordsN;i++)
    {
        EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&words[i])), 0);
    }
    //free stuff
    for(i = 0; i < wordsN; i++)
    {
        rf_string_deinit(&words[i]);
    }
    free(words);

    //expect false, no commas in there
    EXPECT(false,rf_string_tokenize(&s7,RFS_(","),&wordsN,&words));

    //expect 10 (number of prefectures) and all true string comparisons marking the correct separation of Japanese prefectures by the prefecture kanji '県'
    //notice the last prefecture is missing its '県' kanji to not get an extra empty string token
    EXPECT(true,
           rf_string_init(&s8,
                         "新潟県富山県石川県福井県山梨県長野県岐阜県静岡"
                         "県愛知県青森"));
    EXPECT(true,rf_string_tokenize(&s8,RFS_("県"),&wordsN,&words));
    EXPECT_MSG(10,
               wordsN,
               "Tokenization of some Japanese provinces did not give the "
               "expected tokens number");
    EXPECT(true,rf_string_equal(RFS_("新潟"),&words[0]));//Niigata
    EXPECT(true,rf_string_equal(RFS_("富山"),&words[1]));//Toyama
    EXPECT(true,rf_string_equal(RFS_("石川"),&words[2]));//Ishikawa
    EXPECT(true,rf_string_equal(RFS_("福井"),&words[3]));//Fukui
    EXPECT(true,rf_string_equal(RFS_("山梨"),&words[4]));//Yamanashi
    EXPECT(true,rf_string_equal(RFS_("長野"),&words[5]));//Nagoya
    EXPECT(true,rf_string_equal(RFS_("岐阜"),&words[6]));//Gifu
    EXPECT(true,rf_string_equal(RFS_("静岡"),&words[7]));//Shizuoka
    EXPECT(true,rf_string_equal(RFS_("愛知"),&words[8]));//Aichi
    EXPECT(true,rf_string_equal(RFS_("青森"),&words[9]));//Aomori

	return 0;
}
