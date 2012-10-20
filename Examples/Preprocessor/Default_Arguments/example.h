//Here we want to declare a function that takes 3 arguments
// and the last 2 of them can have default values
RF_DECLARE_DFUNC3(float, //function will return float
				  test_func,//function name
				  2,//2 out of 3 arguments will have default values
				  int,//first argument type
				  int,//second argument type 
				  float)//fourth argument type 