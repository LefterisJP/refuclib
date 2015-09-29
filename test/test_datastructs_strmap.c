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
        ck_assert(strmap_add(&map, &stringobjs[i].str, &stringobjs[i].obj));
    }

    for (i = 0; i < ARR_SIZE; i++) {
        struct object *obj = strmap_get(&map, &stringobjs[i].str);
        ck_assert_int_eq(obj->val, stringobjs[i].obj.val);
    }

    strmap_clear(&map);

} END_TEST

START_TEST (test_strmap_add_sameval_string) {
    // this test also catches an invalid read of size 1 valgrind memory error
    // that was occuring when attemping to add a string of the same value to a map
    unsigned int i;
    struct obj_strmap map;
    strmap_init(&map);
    for (i = 0; i < ARR_SIZE; i++) {
        ck_assert(strmap_add(&map, &stringobjs[i].str, &stringobjs[i].obj));
    }

    for (i = 0; i < ARR_SIZE; i++) {
        struct object *obj = strmap_get(&map, &stringobjs[i].str);
        ck_assert_int_eq(obj->val, stringobjs[i].obj.val);
    }

    // it's important to test a different string pointer
    struct RFstring *other = rf_string_create("String1");
    ck_assert(!strmap_add(&map, other, &stringobjs[0].obj));
    ck_assert_int_eq(errno, EEXIST); // make sure errno is set correctly

    strmap_clear(&map);
    rf_string_destroy(other);

} END_TEST

START_TEST (test_strmap_del) {
    unsigned int i;
    struct obj_strmap map;
    strmap_init(&map);
    for (i = 0; i < ARR_SIZE; i++) {
        ck_assert(strmap_add(&map, &stringobjs[i].str, &stringobjs[i].obj));
    }

    for (i = 0; i < ARR_SIZE; i++) {
        struct object *obj = strmap_get(&map, &stringobjs[i].str);
        ck_assert_int_eq(obj->val, stringobjs[i].obj.val);
    }

    // it's important to test a different string pointer
    struct RFstring *other = rf_string_create("String1");
    struct RFstring *s = strmap_del(&map, other, NULL);
    ck_assert(rf_string_equal(other, s));
    s = strmap_del(&map, &stringobjs[1].str, NULL);
    ck_assert(rf_string_equal(&stringobjs[1].str, s));

    // try to get the string objects, to make sure they are deleted
    ck_assert(!strmap_get(&map, &stringobjs[0].str));
    ck_assert(!strmap_get(&map, &stringobjs[1].str));

    strmap_clear(&map);
    rf_string_destroy(other);
} END_TEST

bool iterate_cb(const struct RFstring *member, struct object *obj, void *userdata)
{
    (void)member;
    bool *foundarr = userdata;
    // let's consider obj->val - 1 as the index to the found array
    unsigned idx = obj->val - 1;
    ck_assert_msg(!foundarr[idx], "The iterated index was already set");
    foundarr[idx] = true;
    return true;
}

START_TEST (test_strmap_iterate) {
    bool found[ARR_SIZE] = {0};
    unsigned int i;
    struct obj_strmap map;
    strmap_init(&map);
    for (i = 0; i < ARR_SIZE; i++) {
        ck_assert(strmap_add(&map, &stringobjs[i].str, &stringobjs[i].obj));
    }

    strmap_iterate(&map, iterate_cb, &found);

    // check that the iterate callback is filled the found array
    for (i = 0; i < ARR_SIZE; i++) {
        ck_assert(found[i]);
    }

    strmap_clear(&map);
} END_TEST

Suite *datastructs_strmap_suite_create(void)
{
    Suite *s = suite_create("data_structures_strmap");

    TCase *tc1 = tcase_create("strmap_add");
    tcase_add_test(tc1, test_strmap_add_different_strings);
    tcase_add_test(tc1, test_strmap_add_sameval_string);

    TCase *tc2 = tcase_create("strmap_del");
    tcase_add_test(tc2, test_strmap_del);

    TCase *tc3 = tcase_create("strmap_iterate");
    tcase_add_test(tc3, test_strmap_iterate);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    suite_add_tcase(s, tc3);
    return s;
}
