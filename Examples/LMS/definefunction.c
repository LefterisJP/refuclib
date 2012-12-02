#include <rf_string.h>
#include <rf_localmem.h> //must include this for the local scope macros


//an example of a function that will not work well with temporary strings
int myFunction_bad(RF_String* s)
{
	return rfString_Length(s);
}
//an example of a function that will properly work with temporary strings
int myFunction_good(RF_String* s)
{
	int ret=0;
	RF_ENTER_LOCAL_SCOPE()//this macro has to be called at the start of the function
	ret = rfString_Length(s);
	RF_EXIT_LOCAL_SCOPE()//this macro has to be called at the end of the function
	return ret;
}

int main()
{
	int result;
	//initialize the library. 
	rfInit();
	
	//never use the temporary object macros like RFS_() with a function that has not been properly set to accept them
	result=myFunction_bad(RFS_("I am a bad string"));
	//this call will work as intended
	result=myFunction_good(RFS_("I am going into a good function example"));
	return 0;
}
