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

int main()
{
    RF_Hashmap_I m1, m2;
    RF_Hashmap_String* m3;
    int i, v;
    RF_String str;
    EXPECT(rfInit(), true);
    // test a hashmap with some unicode keys
    EXPECT(true, rfHashmap_I_Init(&m1, 20));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("ενα"), 1));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("δυο"), 2));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("τρια"), 3));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("τεσσερα"), 4));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("πεντε"), 5));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("εξι"), 6));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("εφτα"), 7));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("οκτω"), 8));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("εννια"), 9));
    EXPECT(true, rfHashmap_I_Insert(&m1, RFS_("δεκα"), 10));
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
                   nums[i]));
    }
    for(i = 0; i < 20; i++)
    {
        EXPECT(true, rfHashmap_I_Get(
                   &m2,
                   RFS_("%d", nums[i]),
                   &v));
        EXPECT(v == nums[i], true);
    }
    rfHashmap_I_Deinit(&m2);

    // test a hashmap of string keys and values
    m3 = rfHashmap_String_Create(10);
    EXPECTNOT(m3, NULL);
    for(i = 0; i < 13; i ++)
    {
        EXPECT(true, rfHashmap_String_Insert(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   RFS_("%s", m3_values[i])));
    }
    for(i = 0; i < 13; i ++)
    {
        EXPECT(true, rfHashmap_String_Get(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   &str));
        EXPECT(true, rfString_Equal(&str, RFS_("%s", m3_values[i])));
    }
    rfHashmap_String_Destroy(m3);

    return 0;
}
