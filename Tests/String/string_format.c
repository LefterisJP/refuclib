#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
	RF_StringX s;
	RF_String string,result;
	rfInit();
	
	
	EXPECT(true,rfStringX_Init_buff(&s,4096,""))
	EXPECT(true,rfString_Init(&string,"これはストリングです。"))
	/* - Testing currently supported features of Refu string formatting - */
	
//! [DECIMAL]
	//decimals
	rfPrintf("Decimal %+8d\n",1337);
	rfPrintf("Left-justify %-8uMore stuff follow\n",2012);
	rfPrintf("Left-justify %0-8uThis time the padding is zeroes\n",2012);
	rfPrintf("Right justify with zeros for padding %08d\n",-1986);
//! [DECIMAL]

//! [HEXADECIMAL]
	//hexadecimals
	rfPrintf("1337 in hex is %x\n",1337);
	rfPrintf("15 in hex is %#x\n",15);
	rfPrintf("Hex with upper flag and padding:%#15X\n",3735928559);
	rfPrintf("Left justify hex:%-#15xand stuff follow\n",3131961357);
//! [HEXADECIMAL]

//! [FLOATINGPOINT]
	//floating point
	rfPrintf("Printing a double %f\n",1.618033);
//! [STRINGXFORMAT]
	EXPECT(RF_SUCCESS,rfStringX_Format(&s,"Printing a double with flags %15.9f",0.12345678911))
	rfString_PruneEnd(&s,1);//since the low order digit is rounded in an implementation defined manner we just drop it for the test's sake
	EXPECT(true,rfString_Equal(&s,RFS_("Printing a double with flags     0.12345678")))
//! [STRINGXFORMAT]
	rfPrintf("Negative double: %f\n",-0.456);
	rfPrintf("Special double: %f\n",1.0f/0.0f);
	rfPrintf("Special double: %F\n",1.0f/0.0f);
	rfPrintf("Special double: %f\n",0.0f/0.0f);
	rfPrintf("Special double: %F\n",0.0f/0.0f);
	rfPrintf("No digits after radix case 1: %f\n",163.0f);
	rfPrintf("No digits after radix case 2: %#f\n",0.0f);
//! [FLOATINGPOINT]
	
//! [SCIENTIFIC]
	//Scientific notation floats
	rfPrintf("Printing in scientific notation %e\n",134.5430);
	rfPrintf("%e\n",42.0f);
	rfPrintf("Force radix printing %#E\n",42.0f);
	//since how this number can be printed can differ let's just test the power
	EXPECT(RF_SUCCESS,rfStringX_Format(&s,"Small num %.12E \n",0.000003234))
	EXPECT(10,rfString_Find(&s,RFS_("3.23")))
	EXPECTNOT(RF_FAILURE,rfStringX_MoveAfter(&s,RFS_("-"),&result,0)) //the E should be minus
	EXPECT(true,rfString_Equal(&s,RFS_("6 \n"))) //the E should be -6
	EXPECT(RF_SUCCESS,rfStringX_Format(&s,"Small num %.12E \n",0.000003234))
	rfPrintf("Negative %.2e\n",-567.2);
//! [SCIENTIFIC]
	
//! [UTF8CHAR]
	//utf8 characters
	//note that the first call will likely give a multi-character constant warning, but in this case it is safe to use (as long as you have utf8-encoded source files)
	EXPECT(RF_SUCCESS,rfStringX_Format(&s,"You can safely input utf8 chars like '%c' or '%c'",'Θ','忍'))
	EXPECT(true,rfString_Equal(&s,RFS_("You can safely input utf8 chars like 'Θ' or '忍'")))
//! [UTF8CHAR]

//! [CSTRING]
	//cstrings
	EXPECT(RF_SUCCESS,rfStringX_Format(&s,"C strings like \"%s\" are also supported","Hello World"))
	EXPECT(true,rfString_Equal(&s,RFS_("C strings like \"Hello World\" are also supported")))
//! [CSTRING]

//! [REFUSTRING]
	//Refu strings
	EXPECT(RF_SUCCESS,rfStringX_Format(&s,"An addition over usual printf is %%S for Refu Strings like \"%S\"",&string))
	//note that in the case below since RFS_() takes no extra arguments there is no need to escape %S. In fact all 
	//the cases where RFS_() takes no extra arguments don't even attempt to evaluate it as a formatted string
	EXPECT(true,rfString_Equal(&s,RFS_("An addition over usual printf is %S for Refu Strings like \"これはストリングです。\"")))
//! [REFUSTRING]
	return 0;
}
