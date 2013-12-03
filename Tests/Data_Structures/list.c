//we have to include the extra object explicitly
#include "../ExtraObjects/test.h" 
#include <RFstring.h>
#include <RFlist.h>
#include <refu.h>

#include "../refu_tests.h"

int ints[10] = {20, 30, 14, 1, 5, 4, 23, 221, 2314, 2105};
int ints_adel[7] = {20, 14, 5, 4, 23, 221, 2314};

//a function to iterate through a linked list checking its int data
void iterate_ll_ints(RFlist_i* l, int* arr)
{
    int d;
    int i = 0;
    rf_list_i_iterate(l, d)
    {
        EXPECT(arr[i], d);
        i++;
    }
}


//a function to check the string data
void iterate_ll_strings(RFlist_string* l)
{
    struct RFstring* s;
    int i = 0;
    rf_list_string_iterate(l, s)
    {
        if(i == 0)
        {
            EXPECT(rf_string_equal(s, RFS_("String 1")), true);
        }
        else if(i == 1)
        {
            EXPECT(rf_string_equal(s, RFS_("String 2")), true);
        }
        else
        {
            EXPECT(rf_string_equal(s, RFS_("String 3")), true);
        }         
        i++;
    }
} 

void iterate_ll_stringsdel(RFlist_string* l)
{
    struct RFstring* s;
    int i = 0;
    rf_list_string_iterate(l, s)
    {
        if(i == 0)
        {
            EXPECT(rf_string_equal(s, RFS_("String 2")), true);
        }
        else
        {
            EXPECT(rf_string_equal(s, RFS_("String 3")), true);
        }
        i++;
    }
} 

int main()
{
    RFlist_i l1;
    RFlist_string *l2, l3;
    RFlist_test_object t1;
    struct RFstring s;
    int i;
    DEFAULT_LIB_INIT();

    //simple test of adding 10 ints to a linked list and then iterating
    //through them
    EXPECT(rf_list_i_init(&l1), true);
    for(i = 0; i < 10; i++)
        EXPECT(rf_list_i_add(&l1, ints[i]), true);
    iterate_ll_ints(&l1, ints);
    
    EXPECT(rf_list_i_has(&l1, 1), true);
    //now let's delete 3 ints
    EXPECT(rf_list_i_delete(&l1, 1), true);
    EXPECT(rf_list_i_delete(&l1, 2105), true);
    EXPECT(rf_list_i_delete(&l1, 30), true);

    //1 should not be there now
    EXPECT(rf_list_i_has(&l1, 1), false);

    //try to delete an int that is not in the linked list
    EXPECT(rf_list_i_delete(&l1, 543), false);

    //check to see if we have what we expect after deletion
    iterate_ll_ints(&l1, ints_adel);

    //let's make a linked list of strings, destruction and construction
    //function should be automatically figured out

    l2 = rf_list_string_create();

    EXPECT(rf_string_init(&s,"String 2"), true);

    EXPECT(rf_list_string_add(l2, RFS_("String 1")), true);
    EXPECT(rf_list_string_add(l2, &s), true);
    EXPECT(rf_list_string_add(l2, RFS_("String 3")), true);

    //iterate and check if it's okay
    iterate_ll_strings(l2);
    

    //make a copy
    rf_list_string_copy_in(&l3, l2);
    iterate_ll_strings(&l3);

    //delete 1st string from the copy
    EXPECT(rf_list_string_delete(&l3, RFS_("String 1")), true);
    
    //check that the copy has only 2 strings but the original still has all
    iterate_ll_stringsdel(&l3);
    iterate_ll_strings(l2);

    //finally let's make a list of object which are outside the library
    //TODO: work on this part of the test a bit more, don't only make
    // sure the build system can compile extra objects
    rf_list_test_object_init(&t1);
    
    return 0;
}
