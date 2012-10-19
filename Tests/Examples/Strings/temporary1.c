#include <rf_string.h>

int main()
{
	RF_String s,temp1,temp2;
	//initialize the library
	rfInit("errorLog","infoLog");
	//The example string
	rfString_Init(&s,"Beethoven");
	//in such a situation we would need to initialize two more temporary strings to be appended
	rfString_Init(&temp1," is one of");
	rfString_Init(&temp2," the great classic composers");
	//append the temporary strings
	rfString_Append(&s,&temp1);
	rfString_Append(&s,&temp
	//finally we also need to deinitialize the temporary strings
	rfString_Deinit(&temp1);
	rfString_Deinit(&temp2);
	//do stuff with the example string ...
	return 0;
}