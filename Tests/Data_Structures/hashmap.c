#include <RFstring.h>
#include <RFhashmap.h>
#include <RFmemory.h>
#include <refu.h>
#include <stdio.h>

#include "../refu_tests.h"


/* a generic object, to try the generic hashmap */
typedef struct an_object
{
    int num;
    float f;
    struct RFstring s;
}an_object;

static an_object* an_object_create(int n, float f, struct RFstring* s)
{
    an_object* ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    ret->num = n;
    ret->f = f;
    rf_string_copy_in(&ret->s, s);
    return ret;
}

static void an_object_destroy(an_object* a)
{
    rf_string_deinit(&a->s);
    free(a);
}

bool iterate_destroy(void* value, void* data)
{
    an_object_destroy(value);
    return true;
}

static void test_generic_map(RFhashmap* m)
{
    int i;
    an_object* obj;
    bool exists;
    //populate the hashmap
    for(i = 0; i < 20; i ++)
    {
        obj = an_object_create(i, (float)i, RFS_("%d", i));
        EXPECT(true, rf_hashmap_insert(m, RFS_("%d",i), obj, &exists));
        EXPECT(false, exists);
    }

    //test it
    for(i = 0; i < 20; i++)
    {
        EXPECT_NOT((obj = rf_hashmap_get(m, RFS_("%d",i), &exists)), NULL);
        EXPECT(obj->num == i, true);
        EXPECT(obj->f == (float)i, true);
        EXPECT(rf_string_equal(&obj->s, RFS_("%d", i)), true );
    }

}

int main()
{
    RFhashmap m;
    DEFAULT_LIB_INIT();

    
    // test non library object hashmap
    EXPECT(rf_hashmap_init(&m, 20), true);
    test_generic_map(&m);
    //free all object using hashmap iterate
    EXPECT(rf_hashmap_iterate(&m, iterate_destroy, NULL), true);
    rf_hashmap_deinit(&m);

    return 0;
}
