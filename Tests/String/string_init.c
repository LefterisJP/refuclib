#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
	RF_String s1,s2,s3,s4,s5,s6,s7;
	RF_StringX* sx1,*sx2,sx3;
	RF_StringX sx4,sx5,sx6,sx7,sx8,*sx9;
	RF_String* sp1;
	const uint16_t utf16Buffer[] = {0xD834,0xDD1E,0x6771,0x0};
	const uint32_t utf32Buffer[] = {0x6771,0x4EAC,0x0};
	rfInit();
	//initialize RF_Strings
	EXPECT(true,rfString_Init(&s1, "This is the test of String initialization"))
	EXPECT(true,rfString_Init(&s2,"%s %d %.3f %u","Printf style initialization",1337,3.141592,912341))
	EXPECT(true,rfString_Init_f(&s3,1.6180));
	EXPECT(true,rfString_Init_i(&s4,1024));
	EXPECT(true,rfString_Init_cp(&s5,0x2708))
	//expect all StringX initializations be succesfull
	EXPECT(true,rfStringX_Init(&sx4,"Initializing a StringX"))
	EXPECT(true,rfStringX_Init_f(&sx5,3.141592))
	EXPECT(true,rfStringX_Init_i(&sx6,4096))
	EXPECTNOT(0,(sx9=rfStringX_Create_cp(0x2708)))

	//simply print the strings to see if they initialized correctly
	rfPrintf("%S\n",&s1);
	rfPrintf("%S\n",&s2);
	rfPrintf("%S\n",&s3);
	rfPrintf("%S\n",&s4);
	rfPrintf("%S\n",&sx4);
	rfPrintf("%S\n",&sx5);
	rfPrintf("%S\n",&sx6);
	//testing conversions from String to StringX (this atm can never be zero but since it's a test anyway ...)
	EXPECTNOT(0,(sx1 = rfStringX_FromString_OUT(&s1)))
	EXPECTNOT(0,(sx2 = rfStringX_Create_buff(4096,"This is a StringX initialiation with a buffer")))
	rfStringX_FromString_IN(&sx3,&s2);
	//simply print the stringXs to see if the conversion happened correctly
	rfPrintf("%S\n",sx1);
	rfPrintf("%S\n",sx2);
	rfStringX_MoveForward(&sx3,7);
	rfPrintf("%S\n",&sx3);


	//check if we get the expected strings after initializations below
	EXPECT_MSG(true,rfString_Equal(&s5,RFS_("✈")),"Initialization of string from codepoint failed")
	EXPECT_MSG(true,rfString_Equal(sx9,RFS_("✈")),"Initialization of stringX from codepoint failed")
	
	//checking if the initialization of a string from utf-32 works
	EXPECT_MSG(true,rfString_Init_UTF32(&s6,utf32Buffer),"Initializing a String from a utf32 buffer failed")
	EXPECT_MSG(true,rfString_Equal(&s6,RFS_("東京")),"The result of the string initialization from a utf32 buffer was not what was expected")

	//checking if the initialization of a stringX from utf-32 works
	EXPECT(true,rfStringX_Init_UTF32(&sx8,utf32Buffer))
	EXPECT(true,rfString_Equal(&sx8,RFS_("東京")))
	
    //checking if the initialization of a string from trans-BMP utf-16 works (G-clef music symbol)+東
    EXPECT(true,rfString_Init_UTF16(&s7,utf16Buffer))
    EXPECTNOT(0,(sp1 = rfString_Create_cp(0x1D11E)))
    EXPECT(true,rfString_Equal(&s7,RFS_("%S東",sp1)))
	//checking if StringX initialization from utf-16 works
	EXPECT(true,rfStringX_Init_UTF16(&sx7,utf16Buffer));
	EXPECT(true,rfString_Equal(&sx7,RFS_("%S東",sp1)))
	
	/*Testing assignments also since there are only 2 functions*/
	
	//expect succesfull assignments
	rfString_Assign(&s1,RFS_("Having removed himself from the election campaign to concentrate on the storm, President Obama will now see at first hand just how destructive Hurricane Sandy has been. He'll travel to Atlantic City where the Republican governor, Chris Christie - normally a fierce critic - will show him scenes of widespread destruction along the Jersey Shore. They'll meet some of those who have lost homes, as well as the emergency teams who have been working around the clock since the weekend."));
	EXPECT(true,rfString_Equal(&s1,RFS_("Having removed himself from the election campaign to concentrate on the storm, President Obama will now see at first hand just how destructive Hurricane Sandy has been. He'll travel to Atlantic City where the Republican governor, Chris Christie - normally a fierce critic - will show him scenes of widespread destruction along the Jersey Shore. They'll meet some of those who have lost homes, as well as the emergency teams who have been working around the clock since the weekend.")))
	rfStringX_Assign(sx2, RFS_("Δοκιμαζουμε την αναθεση για StringX"));
	EXPECT(true,rfString_Equal(sx2,RFS_("Δοκιμαζουμε την αναθεση για StringX")))
	//expect true in all: testing assigning a unicode codepoint to a string (the +/-) sign
	EXPECT(true,rfStringX_Assign_char(sx2,0xB1))
	EXPECT(true,rfString_Equal(sx2,RFS_("±")));
	return 0;
}
