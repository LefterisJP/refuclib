#include <RFstring.h>
#include <RFhashmap.h>
#include <RFprintf.h>
#include <RFmemory.h>
#include <refu.h>
#include <stdio.h>

#include "../refu_tests.h"


/* a generic object, to try the generic hashmap */
typedef struct an_object
{
    int num;
    float f;
    RF_String s;
}an_object;

static an_object* an_object_create(int n, float f, RF_String* s)
{
    an_object* ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    ret->num = n;
    ret->f = f;
    rfString_Copy_IN(&ret->s, s);
    return ret;
}

static void an_object_destroy(an_object* a)
{
    rfString_Deinit(&a->s);
    free(a);
}

bool iterate_destroy(void* value, void* data)
{
    an_object_destroy(value);
    return true;
}

static void test_generic_map(RF_Hashmap* m)
{
    int i;
    an_object* obj;
    bool exists;
    //populate the hashmap
    for(i = 0; i < 20; i ++)
    {
        obj = an_object_create(i, (float)i, RFS_("%d", i));
        EXPECT(true, rfHashmap_Insert(m, RFS_("%d",i), obj, &exists));
        EXPECT(false, exists);
    }

    //test it
    for(i = 0; i < 20; i++)
    {
        EXPECTNOT((obj = rfHashmap_Get(m, RFS_("%d",i))), NULL);
        EXPECT(obj->num == i, true);
        EXPECT(obj->f == (float)i, true);
        EXPECT(rfString_Equal(&obj->s, RFS_("%d", i)), true );
    }

}

int main()
{
    RF_Hashmap m;
    EXPECT(rfInit(), true);

    
    // test non library object hashmap
    EXPECT(rfHashmap_Init(&m, 20), true);
    test_generic_map(&m);
    //free all object using hashmap iterate
    EXPECT(rfHashmap_Iterate(&m, iterate_destroy, NULL), true);
    rfHashmap_Deinit(&m);

    return 0;
}
