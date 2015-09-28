#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"
#include <String/rf_str_core.h>
#include <Data_Structures/strmap.h>

struct object {
    unsigned int val;
};

struct pair {
    struct RFstring str;
    struct object obj;
};

static const struct pair stringobjs[] = {
    { RF_STRING_STATIC_INIT("String1"), {.val = 1} },
    { RF_STRING_STATIC_INIT("String2"), {.val = 2} },
    { RF_STRING_STATIC_INIT("String3"), {.val = 3} },
    { RF_STRING_STATIC_INIT("String4"), {.val = 4} },
    { RF_STRING_STATIC_INIT("String5"), {.val = 5} }
};

struct obj_strmap {
    STRMAP_MEMBERS(struct object*);
};

#define ARR_SIZE (sizeof(stringobjs) / sizeof(struct pair))

START_TEST (test_strmap_add_different_strings) {
    unsigned int i;
    struct obj_strmap map;
    strmap_init(&map);
    for (i = 0; i < ARR_SIZE; i++) {
        strmap_add(&map, &stringobjs[i].str, &stringobjs[i].obj);
    }

    for (i = 0; i < ARR_SIZE; i++) {
        struct object *obj = strmap_get(&map, &stringobjs[i].str);
        ck_assert_int_eq(obj->val, stringobjs[i].obj.val);
    }

    strmap_clear(&map);

} END_TEST

Suite *datastructs_strmap_suite_create(void)
{
    Suite *s = suite_create("data_structures_strmap");

    TCase *tc1 = tcase_create("strmap_add");
    tcase_add_test(tc1, test_strmap_add_different_strings);

    suite_add_tcase(s, tc1);
    return s;
}
