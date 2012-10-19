#include <Data_Structures/rfc_list.h>

//! [PRINT_LISTS]
//prints a ListP
void printListP(RF_ListP* list)
{
    uint32_t i;
    printf("\n");
    for(i =0;i<list->size; i ++)
        printf("ListP[%d]=\"%s\"\n",i,rfString_ToCstr((RF_String*)rfListP_Get(list,i)));

}
//prints a ListV
void printListV(RF_ListV* list)
{
    uint32_t i;
    RF_String s;
    printf("\n");
    for(i =0;i<list->size; i ++)
    {
        rfListV_Get(list,i,&s);
        printf("ListV[%d]=\"%s\"\n",i,rfString_ToCstr(&s));
    }
}

//! [PRINT_LISTS]
int main()
{
    rfInit("errorLog","infoLog");
    RF_ListP listP;
    RF_ListV listV;
    RF_String s1,s2,s3,s4,s5,s6,set;
    //Initialize the strings
    rfString_Init(&s1,"First string on the stack");
    rfString_Init(&s2,"Second string on the stack");
    rfString_Init(&s3,"Third string on the stack");
    rfString_Init(&s4,"Fourth string on the stack");
    rfString_Init(&s5,"I am an addition");
    rfString_Init(&s6,"I am another addition");
    rfString_Init(&set,"I was inserted by Set");
//! [POPULATE_LISTP]
    //Initialize a list of 4 pointers to string and add 4 of them
    rfListP_Init(&listP,RF_String,4);
    rfListP_Add(&listP,rfString_Create("First string on the heap"));
    rfListP_Add(&listP,rfString_Create("Second string on the heap"));
    rfListP_Add(&listP,rfString_Create("Third string on the heap"));
    rfListP_Add(&listP,rfString_Create("Fourth string on the heap"));
//! [POPULATE_LISTP]

//! [POPULATE_LISTV]
    //Initialize a list of 4 strings and add the ones we had before
    rfListV_Init(&listV,RF_String,4);
    rfListV_Add(&listV,&s1);
    rfListV_Add(&listV,&s2);
    rfListV_Add(&listV,&s3);
    rfListV_Add(&listV,&s4);
//! [POPULATE_LISTV]

//! [DO_PRINT1]
    //print the 2 lists
    printListP(&listP);
    printListV(&listV);
//! [DO_PRINT1]

//! [ADD_REMOVE]
    //make a copy of the values list
    RF_ListV listV2;
    rfListV_Copy_IN(&listV2,&listV);
    //and now remove 2 items from the original values list
    //note that removing means also calling the object's destructor
    //in this case rfString_Deinit
    rfListV_Remove(&listV,1);
    //notice that everything moves back when you remove something from a list
    //so we are removing a different item here
    rfListV_Remove(&listV,1);
    //also let's add some string to the copied values list
    rfListV_Add(&listV2,&s5);
    rfListV_Add(&listV2,&s6);
    rfListV_Set(&listV2,0,&set,true);

    //print the 2 values list now
    printListV(&listV);
    printListV(&listV2);
//! [ADD_REMOVE]

//! [CLEANUP]
	//cleanup of the lists
	rfListV_Deinit(&listV);
	rfListV_Deinit(&listV2);
	rfListP_Deinit(&listP);
//! [CLEANUP]
    return 0;
}
