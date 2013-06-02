#include <RFstring.h>
#include <refu.h>

int main()
{
    RF_String s1,*s2,s3;
	//Initialize the library
	rfInit();
    rfString_Init(&s1,"Hello World");
/**
 ** [COPY_STACK]
 **
 **/
    //copy a string on the stack
    rfString_Copy_IN(&s3,&s1);
/**
 ** [COPY_STACK]
 ** [COPY_HEAP]
 **
 **/
    //copy a string on the heap
    s2 = rfString_Copy_OUT(&s1);
/**
 ** [COPY_HEAP]
 **
 **/
    return 0;
}