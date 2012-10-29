#include <rf_string.h>
#include <stdio.h>
int main()
{
	RF_String s1,s2,s3,s4;
	RF_StringX* sx1,*sx2,sx3;
	
	rfInit();
	
	rfString_Init(&s1, "This is the test of String initialization");
	rfString_Init(&s2,"%s %d %.3f %u","Printf style initialization",1337,3.141592,912341);
	rfString_Init_f(&s3,1.6180);
	rfString_Init_i(&s4,1024);
	printf("%s\n",rfString_Cstr(&s1));
	printf("%s\n",rfString_Cstr(&s2));
	printf("%s\n",rfString_Cstr(&s3));
	printf("%s\n",rfString_Cstr(&s4));
	
	sx1 = rfStringX_FromString_OUT(&s1);
	sx2 = rfStringX_Create_buff(4096,"This is a StringX initialiation with a buffer");
	rfStringX_FromString_IN(&sx3,&s2);
	printf("%s\n",rfString_Cstr(sx1));
	printf("%s\n",rfString_Cstr(sx2));
	printf("%s\n",rfString_Cstr(&sx3));
	
	return 0;
}