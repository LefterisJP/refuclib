#include "example.h"

//! [Function Definition]
// This is how we will define a function with default arguments in a source file
// Remember that we declared it as a function with 2 default values so the
// ints and the float need to get their default values in order
RF_DEFINE_DFUNC3(float,test_func,int,firstNum,int,secondNum,float,thirdNum,100,0.5)
{
	float ret;
	//let's add the second number to the first
	ret = firstNum+secondNum;
	ret = ret*thirdNum;
	return ret;
}
//! [Function Definition]


//! [Function Calling]
//Define a macro that will correctly choose the appropriate default arguments function.
#define test_func(...) RF_SELECT_FUNC(test_func,__VA_ARGS__)
//let's test the default arguments function calling
int main()
{
	float a = test_func(100);//200*0.5 = 100
	float b = test_func(100,1);//101*0.5 = 50.5
	float c = test_func(100,2,2.5);//102*2.5 = 255
	return 0;
}
//! [Function Calling]