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
static bool string_on_iterate(struct RFstring* s, void* user_data)
{
    int i;
    bool success = false;
    static bool found[m3_size] = {false, false, false, false, false, false,
                                  false, false, false, false, false, false
    };
    for(i = 0; i < m3_size; i++)
    {
        if(rf_string_equal(s, RFS_(m3_values[i])) && !found[i])
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
    RFhashmap_string* m3;
    int i;
    struct RFstring str, *strp;
    bool exists;
    DEFAULT_LIB_INIT();

    m3 = rf_hashmap_string_create(10);
    EXPECT_NOT(m3, NULL);
    for(i = 0; i < m3_size; i ++)
    {
        EXPECT(true, rf_hashmap_string_insert(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   RFS_("%s", m3_values[i]),
                   &exists));
        EXPECT(false, exists);
    }
    for(i = 0; i < m3_size; i ++)
    {
        /* test the Get_IN function which copies the get value */
        EXPECT(true, rf_hashmap_string_get_in(
                   m3,
                   RFS_("%s", m3_keys[i]),
                   &str));
        EXPECT(true, rf_string_equal(&str, RFS_("%s", m3_values[i])));
        rf_string_deinit(&str);
    }
    for(i = 0; i < m3_size; i ++)
    {
        /* test the Get_OUT function */
        strp = rf_hashmap_string_get_out(m3, RFS_("%s", m3_keys[i]));
        EXPECT_NOT(strp, NULL);
        EXPECT(true, rf_string_equal(strp, RFS_("%s", m3_values[i])));
    }
    //also in this hashmap test the iteration function
    EXPECT(true, rf_hashmap_string_iterate(m3, string_on_iterate, NULL));
    rf_hashmap_string_destroy(m3);

    return 0;
}
