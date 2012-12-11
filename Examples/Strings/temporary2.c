#include <RFstring.h>
#include <refu.h>

int main()
{
	RF_String s
	//initialize the library
	rfInit("errorLog","infoLog");
	//The example string
	rfString_Init(&s,"Beethoven");
	//by using the local memory stack we can avoid  temporary strings, as we use the macro to create them on the local stack
	//which means they cease to exist with no memory leaks after the function completes its operation
	rfString_Append(&s,RFS_(" is one of the great classic composers"));
	//do stuff with the example string ...
	return 0;
}