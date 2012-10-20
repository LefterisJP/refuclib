#include <rf_string.h>

int main()
{	
   	RF_StringX s,s2;
	//initialize the refu library
	rfInit("errorLog","infoLog");
	//initialize an extended string
	rfStringX_Init(&s,"Name:%s Age:%d Height:%.2f","John",25,1.75);
	//attempt to move 5 characters in front and let's see the string at that state
	rfStringX_MoveForward(&s,5);
	printf("String after 5 characters: \"%s\"\n",rfString_ToCstr(&s));
	//reset the internal pointer to the beginning
	rfStringX_Reset(&s);
	//move the internal pointer after the first space character and let's see the string at that state
	rfStringX_MoveAfter(&s,RFS_(" "),&s2);
	printf("String after first space: \"%s\"\n",rfString_ToCstr(&s));
	//let's also see what was passed during moving
    printf("From start until end of moving: \"%s\"\n",rfString_ToCstr(&s2));
	return 0;
}