#include <RFstring.h>
#include <RFlinkedlist.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"

int ints[10] = {20, 30, 14, 1, 5, 4, 23, 221, 2314, 2105};
int ints_adel[7] = {20, 14, 5, 4, 23, 221, 2314};

//a function to iterate through a linked list checking its int data
void iterate_ll_ints(RF_LinkedListI* l, int* arr)
{
    int d;
    int i = 0;
    RF_LINKEDLISTI_ITERATE_START(l, d)
    EXPECT(arr[i], d)
    i++;
    RF_LINKEDLISTI_ITERATE_END()
}


//a function to check the string data
void iterate_ll_strings(RF_LinkedList* l)
{
    RF_String* s;
    int i = 0;
    RF_LINKEDLIST_ITERATE_START(l, s)
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
    RF_LINKEDLIST_ITERATE_END()    
} 

void iterate_ll_stringsdel(RF_LinkedList* l)
{
    RF_String* s;
    int i = 0;
    RF_LINKEDLIST_ITERATE_START(l, s)
    if(i == 0)
    {
        EXPECT(rfString_Equal(s, RFS_("String 2")), true)
    }
    else
    {
        EXPECT(rfString_Equal(s, RFS_("String 3")), true)
    }
    i++;
    RF_LINKEDLIST_ITERATE_END()    
} 

int main()
{
    RF_LinkedListI l1;
    RF_LinkedList *l2, l3;
    RF_String s;
    int i;
    rfInit();

    //simple test of adding 10 ints to a linked list and then iterating
    //through them
    EXPECT(rfLinkedListI_Init(&l1), true)
    for(i = 0; i < 10; i++)
        EXPECT(rfLinkedListI_Add(&l1, ints[i]), true)
    iterate_ll_ints(&l1, ints);
    
    EXPECT(rfLinkedListI_Has(&l1, 1), true)
    //now let's delete 3 ints
    EXPECT(rfLinkedListI_Delete(&l1, 1), true)
    EXPECT(rfLinkedListI_Delete(&l1, 2105), true)
    EXPECT(rfLinkedListI_Delete(&l1, 30), true)

    //1 should not be there now
    EXPECT(rfLinkedListI_Has(&l1, 1), false)

    //try to delete an int that is not in the linked list
    EXPECT(rfLinkedListI_Delete(&l1, 543), false)

    //check to see if we have what we expect after deletion
    iterate_ll_ints(&l1, ints_adel);

    //let's make a linked list of strings, destruction and construction
    //function should be automatically figured out

    l2 = rfLinkedList_Create(RF_String,
                             rfString_Deinit,
                             rfString_Copy_IN,
                             rfString_Equal);

    rfString_Init(&s,"String 2");

    EXPECT(rfLinkedList_Add(l2, RFS_("String 1")), true)
    EXPECT(rfLinkedList_Add(l2, &s), true)
    EXPECT(rfLinkedList_Add(l2, RFS_("String 3")), true)

    //iterate and check if it's okay
    iterate_ll_strings(l2);
    

    //make a copy
    rfLinkedList_Copy_IN(&l3, l2);
    iterate_ll_strings(&l3);

    //delete 1st string from the copy
    EXPECT(rfLinkedList_Delete(&l3, RFS_("String 1")), true)
    
    //check that the copy has only 2 strings but the original still has all
    iterate_ll_stringsdel(&l3);
    iterate_ll_strings(l2);
    return 0;
}
