/**
** Just a simple header file to conveniently define anything that
** would be commonly needed in all tests
**/


//prevent stdout buffering. Must-have when debugging a test that crushes
#define printf(...)	{\
	printf(__VA_ARGS__);\
	fflush(stdout);}


#define SIGNAL_POSITION() printf("passed >> line [%d] of test file \"%s\"\n",__LINE__,__FILE__)

//statement func is expected to return value. If it does not we print the location where it happened 
#define EXPECT(value,func)	\
	if(value != func)\
		printf("*ERROR*: A function or statement did not act as it was expected to ==> at line [%d] of test file \"%s\"\n",__LINE__,__FILE__)\
	else\
		SIGNAL_POSITION()
		
//statement func is expected to return anything BUT value. If it does we print the location where it happened 
#define EXPECTNOT(value,func)	\
	if(value == func)\
		printf("*ERROR*: A function or statement did not act as it was expected to ==> at line [%d] of test file \"%s\"\n",__LINE__,__FILE__)\
	else\
		SIGNAL_POSITION()

//statement func is expected to return something greater or equalto value. If it does not we print the location where it happened 
#define EXPECTGE(value,func)	\
	if(func < value)\
		printf("*ERROR*: A function or statement did not act as it was expected to ==> at line [%d] of test file \"%s\"\n",__LINE__,__FILE__)\
	else\
		SIGNAL_POSITION()
			
		
//statement func is expected to return value. If it does not we print the location where it happened along with a message	
#define EXPECT_MSG(value,func,msg)	\
	if(value != func)\
		printf("*ERROR*: "msg" ==> at line [%d] of test file \"%s\"\n",__LINE__,__FILE__)\
	else\
		SIGNAL_POSITION()
		
//statement func is expected to return anything BUT value. If it does we print the location where it happened along with a message	
#define EXPECTNOT_MSG(value,func,msg)	\
	if(value == func)\
		printf("*ERROR*: "msg" ==> at line [%d] of test file \"%s\"\n",__LINE__,__FILE__)\
	else\
		SIGNAL_POSITION()
	
//statement func is expected to return something greater or equal to value. If it does not we print the location where it happened along with a message	
#define EXPECTGE_MSG(value,func,msg)	\
	if(func < value)\
		printf("*ERROR*: "msg" ==> at line [%d] of test file \"%s\"\n",__LINE__,__FILE__)\
	else\
		SIGNAL_POSITION()
	
//For when we want to remove the whole checking and printing
//#define EXPECT(a,b) b;
//#define EXPECT_MSG(a,b,c) b;