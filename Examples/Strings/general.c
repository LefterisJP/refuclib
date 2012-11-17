#include <rf_string.h>

int main()
{	
	RF_String s1,s2;
	//initialize the refu library
	rfInit("errorLog","infoLog");
	//initialize two strings
	rfString_Init(&s1,"I am the first String ");
	//string initialization portraying usage of printf-style formatted string literal
	rfString_Init(&s2,"and I am the String number %d " ,2);
	//appending a String to another String
	rfString_Append(&s1,&s2);
	//appending a string literal to another string
	rfString_Append(&s1,RFS_("and finally I am the %drd",3));
	//printf it to the stdout
	printf("%s",rfString_Cstr(&s1));
	return 0;
}