#include <RFstring.h>
#include <RFhashmap.h>
#include <refu.h>
#include <stdio.h>

#include "../refu_tests.h"

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
    "Warsaw",
    "Zurich",
    "Budapest",
    "Bucharest"
};

#define m3_size (sizeof(m3_values)/sizeof(char*))

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


int main()
{
    RF_Hashmap_String* m3;
    int i;
    RF_String str, *strp;
    bool exists;
    DEFAULT_LIB_INIT();

    m3 = rfHashmap_String_Create(10);
    EXPECT_NOT(m3, NULL);
    for(i = 0; i < m3_size; i ++)
    {
        EXPECT(true, rfHashmap_String_Insert(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   RFS_("%s", m3_values[i]),
                   &exists));
        EXPECT(false, exists);
    }
    for(i = 0; i < m3_size; i ++)
    {
        /* test the Get_IN function which copies the get value */
        EXPECT(true, rfHashmap_String_Get_IN(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   &str));
        EXPECT(true, rfString_Equal(&str, RFS_("%s", m3_values[i])));
        rfString_Deinit(&str);
    }
    for(i = 0; i < m3_size; i ++)
    {
        /* test the Get_OUT function */
        strp = rfHashmap_String_Get_OUT(m3, RFS_("%s", m3_keys[i]));
        EXPECT_NOT(strp, NULL);
        EXPECT(true, rfString_Equal(strp, RFS_("%s", m3_values[i])));
    }
    //also in this hashmap test the iteration function
    EXPECT(true, rfHashmap_String_Iterate(m3, string_on_iterate, NULL));
    rfHashmap_String_Destroy(m3);

    return 0;
}
