//we have to include the extra object explicitly
#include "../ExtraObjects/test.h" 
#include <RFstring.h>
#include <RFlist.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"

int ints[10] = {20, 30, 14, 1, 5, 4, 23, 221, 2314, 2105};
int ints_adel[7] = {20, 14, 5, 4, 23, 221, 2314};

//a function to iterate through a linked list checking its int data
void iterate_ll_ints(RF_List_I* l, int* arr)
{
    int d;
    int i = 0;
    rfList_I_Iterate(l, d)
    {
        EXPECT(arr[i], d)
        i++;
    }
}


//a function to check the string data
void iterate_ll_strings(RF_List_String* l)
{
    RF_String* s;
    int i = 0;
    rfList_String_Iterate(l, s)
    {
        if(i == 0)
        {
            EXPECT(rfString_Equal(s, RFS_("String 1")), true)
        }
        else if(i == 1)
        {
            EXPECT(rfString_Equal(s, RFS_("String 2")), true)
        }
        else
        {
            EXPECT(rfString_Equal(s, RFS_("String 3")), true)
        }         
        i++;
    }
} 

void iterate_ll_stringsdel(RF_List_String* l)
{
    RF_String* s;
    int i = 0;
    rfList_String_Iterate(l, s)
    {
        if(i == 0)
        {
            EXPECT(rfString_Equal(s, RFS_("String 2")), true)
        }
        else
        {
            EXPECT(rfString_Equal(s, RFS_("String 3")), true)
        }
        i++;
    }
} 

int main()
{
    RF_List_I l1;
    RF_List_String *l2, l3;
    RF_List_test_object t1;
    RF_String s;
    int i;
    rfInit();

    //simple test of adding 10 ints to a linked list and then iterating
    //through them
    EXPECT(rfList_I_Init(&l1), true)
    for(i = 0; i < 10; i++)
        EXPECT(rfList_I_Add(&l1, ints[i]), true)
    iterate_ll_ints(&l1, ints);
    
    EXPECT(rfList_I_Has(&l1, 1), true)
    //now let's delete 3 ints
    EXPECT(rfList_I_Delete(&l1, 1), true)
    EXPECT(rfList_I_Delete(&l1, 2105), true)
    EXPECT(rfList_I_Delete(&l1, 30), true)

    //1 should not be there now
    EXPECT(rfList_I_Has(&l1, 1), false)

    //try to delete an int that is not in the linked list
    EXPECT(rfList_I_Delete(&l1, 543), false)

    //check to see if we have what we expect after deletion
    iterate_ll_ints(&l1, ints_adel);

    //let's make a linked list of strings, destruction and construction
    //function should be automatically figured out

    l2 = rfList_String_Create();

    EXPECT(rfString_Init(&s,"String 2"), true)

    EXPECT(rfList_String_Add(l2, RFS_("String 1")), true)
    EXPECT(rfList_String_Add(l2, &s), true)
    EXPECT(rfList_String_Add(l2, RFS_("String 3")), true)

    //iterate and check if it's okay
    iterate_ll_strings(l2);
    

    //make a copy
    rfList_String_Copy_IN(&l3, l2);
    iterate_ll_strings(&l3);

    //delete 1st string from the copy
    EXPECT(rfList_String_Delete(&l3, RFS_("String 1")), true)
    
    //check that the copy has only 2 strings but the original still has all
    iterate_ll_stringsdel(&l3);
    iterate_ll_strings(l2);

    //finally let's make a list of object which are outside the library
    //TODO: work on this part of the test a bit more, don't only make
    // sure the build system can compile extra objects
    rfList_test_object_Init(&t1);
    
    return 0;
}
