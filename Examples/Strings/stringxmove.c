#include <RFstring.h>
#include <rf_stdio.h>

int main()
{	
   	RF_StringX s,s2;
	//initialize the refu library
	rfInit("errorLog","infoLog");
	//initialize an extended string
	rfStringX_Init(&s,"Name:%s Age:%d Height:%.2f","John",25,1.75);
	//attempt to move 5 characters in front and let's see the string at that state
	rfStringX_MoveForward(&s,5);
	rfPrintf("String after 5 characters: \"%S\"\n",&s);
	//reset the internal pointer to the beginning
	rfStringX_Reset(&s);
	//move the internal pointer after the first space character and let's see the string at that state
	rfStringX_MoveAfter(&s,RFS_(" "),&s2,RF_STRINGX_ARGUMENT); //since s2 is a StringX let the function know that we need a StringX result
	rfPrintf("String after first space: \"%S\"\n",&s);
	//let's also see what was passed during moving
    rfPrintf("From start until end of moving: \"%S\"\n",&s2);
	return 0;
}