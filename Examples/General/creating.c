#include <rf_string.h>
#include <Data_Structures/rfc_list.h>
//! [LIB_INIT]
int main()
{
	//Initialize the library
	rfInit("refuErrorLog","refuInfoLog",1000);
//! [LIB_INIT]

//! [Init]
	RF_String s1;//this is a string on the stack
	RF_ListP list1;//this is a list on the stack
	//Initializing a string on the stack
	rfString_Init(&s1,"Lorem ipsum on the stack");
	//Initializing a list on the stack, that will initially have room for 10 Strings
	rfListP_Init(&list1,RF_String,10);
//! [Init]

//! [Create]
	RF_String* s2;//this is a pointer to a string
	RF_ListP* list2;//this is a pointer to a list
	//Create a string on the heap
	s2 = rfString_Create("Lorem ipsum on the heap");
	//Create a list on the heap, that will initially have room for 10 Strings
	list2 = rfListP_Create(RF_String,10);
//! [Create]

//! [Deinit]
	//Free the memory of objects on the stack created with init
	rfString_Deinit(&s1);
	rfListP_Deinit(&list1);
	//Beware! If you get out of scope and have not deinitialized them you have a memory leak
//! [Deinit]

//! [Destroy]
	//Destroy objects on the heap
	rfString_Destroy(s2);
	rfListP_Destroy(list2);
//! [Destroy]
    return 0;
}
