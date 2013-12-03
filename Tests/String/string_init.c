#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    struct RFstring s1,s2,s3,s4,s5,s6,s7;
    struct RFstringx* sx1,*sx2,sx3;
    struct RFstringx sx4,sx5,sx6,sx7,sx8,*sx9;
    struct RFstring* sp1;
    const uint16_t utf16Buffer[] = {0xD834,0xDD1E,0x6771};
    unsigned int utf16b_len = 3 * sizeof(uint16_t);
    const uint32_t utf32Buffer[] = {0x6771,0x4EAC};
    unsigned int utf32b_len = 2;
    DEFAULT_LIB_INIT();
    //initialize RFstrings
    EXPECT_TRUE(rf_string_init(&s1, "This is the test of String initialization"));
    EXPECT_TRUE(
        rf_string_initv(&s2,
                      "%s %d %.3f %u",
                      "Printf style initialization",
                      1337, 3.141592, 912341)
    );
    EXPECT_TRUE(rf_string_init_double(&s3, 1.6180));
    EXPECT_TRUE(rf_string_init_int(&s4, 1024));
    EXPECT_TRUE(rf_string_init_cp(&s5, 0x2708));
    //expect all StringX initializations be succesfull
    EXPECT_TRUE(rf_stringx_init(&sx4,"Initializing a StringX"));
    EXPECT_TRUE(rf_stringx_init_double(&sx5, 3.141592));
    EXPECT_TRUE(rf_stringx_init_int(&sx6, 4096));
    EXPECT_NOT(0, sx9=rf_stringx_create_cp(0x2708));

    //simply print the strings to see if they initialized correctly
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s1)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s2)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s3)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s4)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&sx4)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&sx5)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&sx6)), 0);
    //testing conversions from String to StringX (this atm can never be zero but since it's a test anyway ...)
    EXPECT_NOT(0,sx1 = rf_stringx_from_string_out(&s1));
    EXPECT_NOT(0,
              sx2 = rf_stringx_create_buff(4096,"This is a StringX initialiation with a buffer"));
    EXPECT(rf_stringx_from_string_in(&sx3,&s2), true);
    //simply print the stringXs to see if the conversion happened correctly
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(sx1)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(sx2)), 0);
    rf_stringx_move_forward(&sx3,7);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&sx3)), 0);


	//check if we get the expected strings after initializations below
    EXPECT_MSG(true,
               rf_string_equal(&s5,RFS_("✈")),
               "Initialization of string from codepoint failed");
    EXPECT_MSG(true,
               rf_string_equal(sx9,RFS_("✈")),
               "Initialization of stringX from codepoint failed");
	
	//checking if the initialization of a string from utf-32 works
    EXPECT_MSG(true,
               rf_string_init_utf32(&s6,utf32Buffer, utf32b_len),
               "Initializing a String from a utf32 buffer failed");
    EXPECT_MSG(true,
               rf_string_equal(&s6,RFS_("東京")),
               "The result of the string initialization from a utf32 buffer"
               " was not what was expected");

	//checking if the initialization of a stringX from utf-32 works
    EXPECT_TRUE(rf_stringx_init_utf32(&sx8, utf32Buffer, utf32b_len));
    EXPECT_TRUE(rf_string_equal(&sx8,RFS_("東京")));
	
    //checking if the initialization of a string from trans-BMP utf-16 works (G-clef music symbol)+東
    EXPECT_TRUE(rf_string_init_utf16(&s7, utf16Buffer, utf16b_len));
    EXPECT_NOT(0, sp1 = rf_string_create_cp(0x1D11E));
    EXPECT_TRUE(
        rf_string_equal(&s7, RFS_(RF_STR_PF_FMT"東", RF_STR_PF_ARG(sp1)))
    );
    //checking if StringX initialization from utf-16 works
    EXPECT_TRUE(rf_stringx_init_utf16(&sx7,utf16Buffer, utf16b_len));
    EXPECT_TRUE(rf_string_equal(&sx7,RFS_(RF_STR_PF_FMT"東",RF_STR_PF_ARG(sp1))));
	
	/*Testing assignments also since there are only 2 functions*/
	
	//expect succesfull assignments
    EXPECT(
        rf_string_assign(
            &s1,
            RFS_(
                "Having removed himself from the election campaign to "
                "concentrate on the storm, President Obama will now see at "
                "first hand just how destructive Hurricane Sandy has been. "
                "He'll travel to Atlantic City where the Republican "
                "governor, Chris Christie - normally a fierce critic - will"
                " show him scenes of widespread destruction along the "
                "Jersey Shore. They'll meet some of those who have lost "
                "homes, as well as the emergency teams who have been "
                "working around the clock since the weekend.")),
        true);
    EXPECT(true,
           rf_string_equal(
               &s1,
               RFS_(
                   "Having removed himself from the election campaign to "
                   "concentrate on the storm, President Obama will now "
                   "see at first hand just how destructive Hurricane Sandy"
                   " has been. He'll travel to Atlantic City where the "
                   "Republican governor, Chris Christie - normally a "
                   "fierce critic - will show him scenes of widespread "
                   "destruction along the Jersey Shore. They'll meet some"
                   " of those who have lost homes, as well as the "
                   "emergency teams who have been working around the "
                   "clock since the weekend.")));
    EXPECT(rf_stringx_assign(sx2, RFS_("Δοκιμαζουμε την αναθεση για StringX")),
           true);
    EXPECT(true,
           rf_string_equal(sx2,RFS_("Δοκιμαζουμε την αναθεση για StringX")));
	//expect true in all: testing assigning a unicode codepoint to a string (the +/-) sign
    EXPECT(true,rf_stringx_assign_char(sx2,0xB1));
    EXPECT(true,rf_string_equal(sx2,RFS_("±")));;
	return 0;
}
