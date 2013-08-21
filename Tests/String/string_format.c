#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    RF_StringX s;
    RF_String string,result;
	
    EXPECT(rfInit(), true);
	
	
    EXPECT(true, rfStringX_Init_buff(&s, 4096, ""));
    EXPECT(true,rfString_Init(&string,"これはストリングです。"));
	/* - Testing currently supported features of Refu string formatting - */
	
/**
 ** [DECIMAL]
 **/
    EXPECTGE(rfPrintf("Decimal %+8d\n", 1337), 0);
    EXPECTGE(rfPrintf("Left-justify %-8uMore stuff follow\n", 2012), 0);
    EXPECTGE(rfPrintf("Left-justify %0-8uThis time the padding is zeroes\n", 2012), 0);
    EXPECTGE(rfPrintf("Right justify with zeros for padding %08d\n", -1986), 0);
    EXPECTGE(rfPrintf("Testing %d %d\n", -1, 4), 0);
/**
 ** [DECIMAL]
 **/

/**
 ** [HEXADECIMAL]
 **/
    EXPECTGE(rfPrintf("1337 in hex is %x\n", 1337), 0);
    EXPECTGE(rfPrintf("15 in hex is %#x\n", 15), 0);
    EXPECTGE(rfPrintf("Hex with upper flag and padding:%#15X\n", 3735928559), 0);
    EXPECTGE(rfPrintf("Left justify hex:%-#15xand stuff follow\n", 3131961357), 0);
/**
 ** [HEXADECIMAL]
 **/

/**
 ** [FLOATINGPOINT]
 **/
    EXPECTGE(rfPrintf("Printing a double %f\n", 1.618033), 0);
/**
 ** [STRINGXFORMAT]
 **/
    EXPECT(true,
           rfStringX_Format(&s,
                            "Printing a double with flags %15.9f",
                            0.12345678911));
    rfString_PruneEnd(&s, 1);//since the low order digit is rounded in an
  //implementation defined manner we just drop it for the test's sake
    EXPECT(true,
           rfString_Equal(&s,
                          RFS_("Printing a double with flags     0.12345678")));
/**
 ** [STRINGXFORMAT]
 **/
    EXPECTGE(rfPrintf("Negative double: %f\n", -0.456), 0);
    EXPECTGE(rfPrintf("Special double: %f\n", 1.0f / 0.0f), 0);
    EXPECTGE(rfPrintf("Special double: %F\n", 1.0f / 0.0f), 0);
    EXPECTGE(rfPrintf("Special double: %f\n", 0.0f / 0.0f), 0);
    EXPECTGE(rfPrintf("Special double: %F\n", 0.0f / 0.0f), 0);
    EXPECTGE(rfPrintf("No digits after radix case 1: %f\n", 163.0f), 0);
    EXPECTGE(rfPrintf("No digits after radix case 2: %#f\n", 0.0f), 0);
/**
 ** [FLOATINGPOINT]
 **/
	
/**
 ** [SCIENTIFIC]
 **/
    EXPECTGE(rfPrintf("Printing in scientific notation %e\n", 134.5430), 0);
    EXPECTGE(rfPrintf("%e\n", 42.0f), 0);
    EXPECTGE(rfPrintf("Force radix printing %#E\n", 42.0f), 0);
	//since how this number can be printed can differ let's just test the power
    EXPECT(true,
           rfStringX_Format(&s, "Small num %.12E \n", 0.000003234));
    EXPECT(10, rfString_Find(&s, RFS_("3.23")));
    EXPECTNOT(RF_FAILURE,
              rfStringX_MoveAfter(&s, RFS_("-"), &result, 0)); //the E should be minus
    EXPECT(true, rfString_Equal(&s, RFS_("6 \n"))); //the E should be -6
    EXPECT(true,
           rfStringX_Format(&s,"Small num %.12E \n",0.000003234));
    EXPECTGE(rfPrintf("Negative %.2e\n", -567.2), 0);
/**
 ** [SCIENTIFIC]
 **/
	
/**
 ** [UTF8CHAR]
 **/
#pragma GCC diagnostic ignored "-Wmultichar"
	//utf8 characters
	//note that putting a unicode character inside single quotes like we do
  //is in most cases an error except for uses like an rfStringX_Format
  
  //GCC will give you a -Wmultichar warning in such a situation which
  //we ignore with the above #pragma 
    EXPECT(true,
           rfStringX_Format(
               &s,"You can safely input both ascii chars like '"
               "%c' and utf8 chars like '%c' or '%c'",
               '3','Θ','忍'));
    EXPECT(true,
           rfString_Equal(
               &s,
               RFS_("You can safely input both ascii chars like "
                    "'3' and utf8 chars like 'Θ' or '忍'")));
/**
 ** [UTF8CHAR]
 **/

/**
 ** [CSTRING]
 **/
    EXPECT(true,
           rfStringX_Format(&s, "C strings like \"%s\" are also supported",
                            "Hello World"));
    EXPECT(true,
           rfString_Equal(
               &s,
               RFS_("C strings like \"Hello World\" are also supported")));
/**
 ** [CSTRING]
 **/

/**
 ** [REFUSTRING]
 **/
	//Refu strings
    EXPECT(true,
           rfStringX_Format(
               &s,
               "An addition over usual printf is %%S for Refu "
               "Strings like \"%S\"", &string));
	//note that in the case below since RFS_() takes no extra arguments there
  //is no need to escape %S. In fact all 
	//the cases where RFS_() takes no extra arguments don't even attempt to
  //evaluate it as a formatted string
    EXPECT(true,
           rfString_Equal(
               &s,
               RFS_("An addition over usual printf is %S for Refu"
                    " Strings like \"これはストリングです。\"")));
/**
 ** [REFUSTRING]
 **/
	return 0;
}
