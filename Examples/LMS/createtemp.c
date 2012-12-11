#include <RFlocalmem.h> //must include this for the local scope macros
#include <refu.h>

//We shall allow temporary objects of type 'foo' to be created
typedef struct foo
{
	int id;
	char* name;
}foo;

//! [TEMP_INIT_FUNC]
//This will be the function that can initialize temporary foo objects. Exactly like RFS_() is in normal library code
foo* FOO_(int id,char* name)
{
	foo* ret;//this will point to the temporary foo's location in the local stack
	//need to call this at the beginning of the function to try and remember the local stack 
	//pointer before the function to which FOO_() has been passed as an argument was called
	rfLMS_ArgsEval();
	
	//push the local stack to allocate an object of type 'foo'
	ret = rfLMS_Push(sizeof(foo));
	//push the local stack enough for foo's name too
	ret->name = rfLMS_Push(strlen(name)+1);
	
	//and now copy the data into foo
	ret->id = id;
	strcpy(ret-name,name)
	
	//finally return the temporary foo
	return ret;
}
//! [TEMP_INIT_FUNC]

//! [TEMP_FOO]
void dosth_with_foo(foo* f)
{//must be in the beginning of any function that may accept any sort of temporary objects residing in the local stack
	RF_ENTER_LOCAL_SCOPE()
	printf("Name is %s and ID is %d\n",f->name,f->id);
	//must be in the end of any function that may accept any sort of temporary objects residing in the local stack
	RF_EXIT_LOCAL_SCOPE()
}
//! [TEMP_FOO]

int main()
{
	int result;
	//initialize the library. 
	rfInit();
	
	//! [USE_TEMP]
	//initialize a temporary foo to use with the function
	dosth_with_foo(FOO_(1337,"I am a leet foo"));
	//after the function has finished the temporary foo no longer exists
	return 0;
	//! [USE_TEMP]
}
