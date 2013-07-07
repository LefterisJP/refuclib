#include <RFstring.h>
#include <RFdynamicarray.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"

int ints[10] = {20, 30, 14, 1, 5, 4, 23, 221, 2314, 2105};
int ints_adel[7] = {20, 14, 5, 4, 23, 221, 2314};
int ints_aset[7] = {20, 25, 5, 55, 23, 221, 143};

int main()
{
    RF_DynamicArray_I a1;
    RF_DynamicArray_String a2, *a3;
    RF_String s, *sp;
    int i, v;
    EXPECT(rfInit(), true)

    //Create a dynamic array of 5 ints capacity and add 10 ints to it
    EXPECT(rfDynamicArray_I_Init(&a1, 5), true)
    for(i = 0; i < 10; i++)
    {
        EXPECT(rfDynamicArray_I_Add(&a1, ints[i]), true)
    }
    //check that the 9th element is as expected
    EXPECT(rfDynamicArray_I_Get_IN(&a1, 9, &i), RF_SUCCESS)
    EXPECT(i == 2105, true)

    //now delete some items
    EXPECT(rfDynamicArray_I_Remove(&a1, 9), RF_SUCCESS)
    EXPECT(rfDynamicArray_I_Remove(&a1, 3), RF_SUCCESS)
    EXPECT(rfDynamicArray_I_Remove(&a1, 1), RF_SUCCESS)

    //now iterate it and check with the get function if we have as many as
    //we should
    for(i = 0; i< 7; i++)
    {
        EXPECT(rfDynamicArray_I_Get_OUT(&a1, i, &v), ints_adel[i])
    }
    
    //now attempt to set an element out of range
    EXPECT(rfDynamicArray_I_Set(&a1, 12, 1337), false) 

    //now set a few elements
    EXPECT(rfDynamicArray_I_Set(&a1, 1, 25), true)
    EXPECT(rfDynamicArray_I_Set(&a1, 3, 55), true)
    EXPECT(rfDynamicArray_I_Set(&a1, 6, 143), true)
    //and check they are indeed as expected
    for(i = 0; i< 7; i++)
    {
        EXPECT(rfDynamicArray_I_Get_OUT(&a1, i, &v), ints_aset[i])
    }

    // now let's go for an array of 2 strings 
    EXPECT( rfDynamicArray_String_Init(&a2, 2), true)
    // add 100 strings to it (testing reallocation)
    for(i = 0; i < 100; i ++)
    {
        EXPECT(rfDynamicArray_String_Add(&a2, RFS_("String %d",i)),
               true)
    }
    // testing Get_IN
    EXPECT(rfDynamicArray_String_Get_IN(&a2, 55, &s), RF_SUCCESS)
    EXPECT(rfString_Equal(&s, RFS_("String 55")), true)

    // testing Get_OUT
    sp = rfDynamicArray_String_Get_OUT(&a2, 86, &i);
    EXPECT(i, RF_SUCCESS)
    EXPECT(rfString_Equal(sp, RFS_("String 86")), true)

    // making a copy
    a3 = rfDynamicArray_String_Copy_OUT(&a2);

    // let's remove the first 50 strings
    for(i=0 ; i < 50; i++)
    {
        EXPECT(rfDynamicArray_String_Remove(&a2, 0), RF_SUCCESS)
    }

    // the copy should have them all correctly
    for(i = 0; i < 100; i++)
    {
        EXPECT(rfDynamicArray_String_Get_IN(a3, i, &s), RF_SUCCESS)
        EXPECT(rfString_Equal(&s, RFS_("String %d", i)), true)
    }

    // the original should start from 51 and have nothing after index 51
    for(i = 0; i < 50; i++)
    {
        EXPECT(rfDynamicArray_String_Get_IN(&a2, i, &s), RF_SUCCESS)
        EXPECT(rfString_Equal(&s, RFS_("String %d", i + 50 )), true)
    }
    EXPECTNOT(rfDynamicArray_String_Get_IN(&a2, 51, &s), RF_SUCCESS)

    return 0;
}
