#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    RF_String s1,s2;
    uint32_t i=0,c;
    DEFAULT_LIB_INIT();
	
	//testing normal string iteration
    EXPECT(true,
           rfString_Init(&s1,
                         "Testing to see if RF_String can correctly iterate"
                         " the characters of a string"));
	rfString_Iterate_Start(&s1, i, c)
      EXPECTGE(printf("[%u]:%c\n", i, c), 0);
	rfString_Iterate_End(i)
	
	//testing string iteration from a specific index
	EXPECTGE(printf("\n\n"), 0);
	i=18;
	rfString_Iterate_Start(&s1, i, c)
      EXPECTGE(printf("[%u]:%c\n", i, c), 0);
	rfString_Iterate_End(i)
	
	//testing backwards string iteration
	EXPECTGE(printf("\n\n"), 0);
	EXPECT(true,
         rfString_Init(
             &s2,
             "Can the backwards iteration work? Well all we gotta do is "
             "test it"));
	i = rfString_Length(&s2)-1;
	rfString_IterateB_Start(&s2, i, c)
      EXPECTGE(printf("[%u]:%c\n", i, c), 0);
	rfString_IterateB_End(i)
		
	
	return 0;
}
