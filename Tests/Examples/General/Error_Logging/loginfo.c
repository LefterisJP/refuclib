#include <rf_setup.h>
//just two functions
void foo1()
{
	LOG_INFO("We got in foo1");
}
void foo2(int var)
{
   LOG_INFO("We got in foo2 with value %d",var);
}
//the main function
int main()
{
	//initialize the library and call the two functions
	rfInit();
	foo1();
	foo2(42);
	//finally log info in detail just to showcase it
	DLOG_INFO("Main is about to exit");
	return 0;
}
