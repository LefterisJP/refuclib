#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
    struct RFstring s1,s2;
    uint32_t i=0,c;
    DEFAULT_LIB_INIT();
	
	//testing normal string iteration
    EXPECT(true,
           rf_string_init(&s1,
                         "Testing to see if RF_string can correctly iterate"
                         " the characters of a string"));
	rf_string_iterate_start(&s1, i, c)
      EXPECTGE(printf("[%u]:%c\n", i, c), 0);
	rf_string_iterate_end(i)
	
	//testing string iteration from a specific index
	EXPECTGE(printf("\n\n"), 0);
	i=18;
	rf_string_iterate_start(&s1, i, c)
      EXPECTGE(printf("[%u]:%c\n", i, c), 0);
	rf_string_iterate_end(i)
	
	//testing backwards string iteration
	EXPECTGE(printf("\n\n"), 0);
	EXPECT(true,
         rf_string_init(
             &s2,
             "Can the backwards iteration work? Well all we gotta do is "
             "test it"));
	i = rf_string_length(&s2)-1;
	rf_string_iterate_b_start(&s2, i, c)
      EXPECTGE(printf("[%u]:%c\n", i, c), 0);
	rf_string_iterate_b_end(i)
		
	
	return 0;
}
