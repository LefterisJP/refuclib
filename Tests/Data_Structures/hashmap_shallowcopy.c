#include <RFstring.h>
#include <RFhashmap.h>
#include <refu.h>
#include <stdio.h>

#include "../refu_tests.h"

int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
              11, 12, 13, 14, 15, 16, 17, 18, 19, 20};


int main()
{
    RFhashmap_i m1, m2;
    int i;
    bool exists, found;
    DEFAULT_LIB_INIT();

    // test a hashmap with some unicode keys
    EXPECT(true, rf_hashmap_i_init(&m1, 20));
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("ενα"), 1, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("δυο"), 2, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("τρια"), 3, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("τεσσερα"), 4, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("πεντε"), 5, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("εξι"), 6, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("εφτα"), 7, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("οκτω"), 8, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("εννια"), 9, &exists));
    EXPECT(false, exists);
    EXPECT(true, rf_hashmap_i_insert(&m1, RFS_("δεκα"), 10, &exists));
    EXPECT(false, exists);

    EXPECT(8, rf_hashmap_i_get(&m1, RFS_("οκτω"), &found));
    EXPECT(found, true);
    EXPECT(3, rf_hashmap_i_get(&m1, RFS_("τρια"), &found));
    EXPECT(found, true);
    
    rf_hashmap_i_deinit(&m1);
    // test a hashmap that will need to rehash
    EXPECT(true, rf_hashmap_i_init(&m2, 10));
    for(i = 0; i < 20; i++)
    {
        EXPECT(true, rf_hashmap_i_insert(
                   &m2,
                   RFS_("%d", nums[i]),
                   nums[i],
                   &exists));
        EXPECT(false, exists);
    }
    for(i = 0; i < 20; i++)
    {
        EXPECT(nums[i], rf_hashmap_i_get(
                   &m2,
                   RFS_("%d", nums[i]),
                   &found));
        EXPECT(found, true);
    }
    rf_hashmap_i_deinit(&m2);

    return 0;
}
