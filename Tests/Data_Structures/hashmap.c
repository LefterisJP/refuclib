#include <RFstring.h>
#include <RFhashmap.h>
#include <RFprintf.h>
#include <refu.h>
#include <stdio.h>

#include "../refu_tests.h"

int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
              11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

char* m3_keys[] = { 
    "Greece",
    "Germany",
    "Japan",
    "USA",
    "Italy",
    "Great Britain",
    "Spain",
    "France",
    "Russia",
    "Poland",
    "Switzerland",
    "Hungary",
    "Romania"
};

char* m3_values[] = { 
    "Athens",
    "Berlin",
    "Tokyo",
    "Washington",
    "Rome",
    "London",
    "Madrid",
    "Paris",
    "Moscow",
    "Warsae",
    "Zurich",
    "Budapest",
    "Bucharest"
};

#define m3_size (sizeof(m3_values)/sizeof(char*))

/* a generic object, to try the generic hashmap */
typedef struct an_object
{
    int num;
    float f;
    RF_String s;
}an_object;

static char an_object_copy(an_object* a, an_object* b)
{
    a->num = b->num;
    a->f = b->f;
    return rfString_Copy_IN(&a->s, &b->s);
}

static void an_object_destroy(an_object* a)
{
    rfString_Deinit(&a->s);
}

/* function used to test hashmap iteration for String:String hashmap */
static bool string_on_iterate(RF_String* s, void* user_data)
{
    int i;
    bool success = false;
    static bool found[m3_size] = {false, false, false, false, false, false,
                                  false, false, false, false, false, false
    };
    for(i = 0; i < m3_size; i++)
    {
        if(rfString_Equal(s, RFS_(m3_values[i])) && !found[i])
        {
            found[i] = true;
            success = true;
            break;
        }
    }
    EXPECT(success, true);
    return true;
}

static void test_generic_map(RF_Hashmap* m)
{
    int i;
    an_object obj;
    bool exists;
    //populate the hashmap
    for(i = 0; i < 20; i ++)
    {
        obj.num = i;
        obj.f = (float) i;
        rfString_Init(&obj.s, "%d", i);
        EXPECT(true, rfHashmap_Insert_Copy(m, RFS_("%d",i), &obj, &exists));
        EXPECT(false, exists);
    }

    //test it
    for(i = 0; i < 20; i++)
    {
        EXPECT(rfHashmap_Get_Copy(m, RFS_("%d",i), &obj), true);
        EXPECT(obj.num == i, true);
        EXPECT(obj.f == (float)i, true);
        EXPECT(rfString_Equal(&obj.s, RFS_("%d", i)), true );
    }

}

int main()
{
    RF_Hashmap_I m1, m2;
    RF_Hashmap_String* m3;
    RF_Hashmap m;
    int i, v;
    RF_String str;
    bool exists;
    EXPECT(rfInit(), true);
    // test a hashmap with some unicode keys
    EXPECT(true, rfHashmap_I_Init(&m1, 20));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("ενα"), 1, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("δυο"), 2, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("τρια"), 3, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("τεσσερα"), 4, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("πεντε"), 5, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("εξι"), 6, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("εφτα"), 7, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("οκτω"), 8, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("εννια"), 9, &exists));
    EXPECT(false, exists);
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("δεκα"), 10, &exists));
    EXPECT(false, exists);

    EXPECT(true, rfHashmap_I_Get(&m1, RFS_("οκτω"), &i));
    EXPECT(i == 8, true);
    EXPECT(true, rfHashmap_I_Get(&m1, RFS_("τρια"), &i));
    EXPECT(i == 3, true);
    
    rfHashmap_I_Deinit(&m1);
    // test a hashmap that will need to rehash
    EXPECT(true, rfHashmap_I_Init(&m2, 10));
    for(i = 0; i < 20; i++)
    {
        EXPECT(true, rfHashmap_I_Insert(
                   &m2,
                   RFS_("%d", nums[i]),
                   nums[i],
                   &exists));
        EXPECT(false, exists);
    }
    for(i = 0; i < 20; i++)
    {
        EXPECT(true, rfHashmap_I_Get(
                   &m2,
                   RFS_("%d", nums[i]),
                   &v));
        EXPECT(v == nums[i], true);
    }
    /* also test the out function */
    v = rfHashmap_I_Get_OUT(&m2, RFS_("4"));
    EXPECT(v, 4);
    rfHashmap_I_Deinit(&m2);

    // test a hashmap of string keys and values
    m3 = rfHashmap_String_Create(10);
    EXPECTNOT(m3, NULL);
    for(i = 0; i < m3_size; i ++)
    {
        EXPECT(true, rfHashmap_String_Insert_Copy(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   RFS_("%s", m3_values[i]),
                   &exists));
        EXPECT(false, exists);
    }
    for(i = 0; i < m3_size; i ++)
    {
        EXPECT(true, rfHashmap_String_Get_Copy(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   &str));
        EXPECT(true, rfString_Equal(&str, RFS_("%s", m3_values[i])));
    }
    //also in this hashmap test the iteration function
    EXPECT(true, rfHashmap_String_Iterate(m3, string_on_iterate, NULL));
    rfHashmap_String_Destroy(m3);
    
    // test non library object hashmap
    EXPECT(rfHashmap_Init(&m, 20, sizeof(an_object),
                          (char(*)(void*, void*))an_object_copy,
                          (void(*)(void*))an_object_destroy), true);
    test_generic_map(&m);
    rfHashmap_Deinit(&m);

    return 0;
}
