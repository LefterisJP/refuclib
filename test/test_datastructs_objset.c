#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <Data_Structures/objset.h>

static const int test_int_arr[] = { 0, 1, 2, 3, 4, 5 };
#define TEST_ARR_SIZE (sizeof(test_int_arr) / sizeof(int))

struct objset_int { OBJSET_MEMBERS(int *); };

static unsigned int triangular_number(unsigned int inp)
{
    unsigned int ret = 0;
    while (inp > 0) {
        ret += inp;
        --inp;
    }
    return ret;
}

START_TEST (test_init_default) {
    struct objset_int set;
    rf_objset_init_default(&set);
    ck_assert(rf_objset_empty(&set));
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_add_get_default) {
    struct objset_int set;
    unsigned int i;
    int *v;
    rf_objset_init_default(&set);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set, &test_int_arr[i]));
    }
    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        v = rf_objset_get_default(&set, &test_int_arr[i]);
        ck_assert(v);
        ck_assert_int_eq(*v, test_int_arr[i]);
    }    
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_add_duplicates_default) {
    struct objset_int set;
    unsigned int i;
    int *v;
    rf_objset_init_default(&set);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set, &test_int_arr[i]));
    }
    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set, &test_int_arr[i]));
    }
    ck_assert_uint_eq(6, rf_objset_size(&set));
    
    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        v = rf_objset_get_default(&set, &test_int_arr[i]);
        ck_assert(v);
        ck_assert_int_eq(*v, test_int_arr[i]);
    }    
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_del_default) {
    struct objset_int set;
    unsigned int i;
    int *v;
    rf_objset_init_default(&set);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set, &test_int_arr[i]));
    }
    ck_assert(rf_objset_del_default(&set, &test_int_arr[5]));
    ck_assert(rf_objset_del_default(&set, &test_int_arr[4]));
    ck_assert(!rf_objset_del_default(&set, &test_int_arr[4]));
    ck_assert_uint_eq(4, rf_objset_size(&set));
    
    for (i = 0; i < TEST_ARR_SIZE - 2; ++i) {
        v = rf_objset_get_default(&set, &test_int_arr[i]);
        ck_assert(v);
        ck_assert_int_eq(*v, test_int_arr[i]);
    }    
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_foreach) {
    struct objset_int set;
    unsigned int i;
    struct rf_objset_iter it;
    int *v;
    rf_objset_init_default(&set);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set, &test_int_arr[i]));
    }

    // check that each element is found only once during iteration. Just add indices
    // up to get triangular number and then for each index subtract. Should get 0
    int triang = triangular_number(TEST_ARR_SIZE);
    rf_objset_foreach(&set, &it, v) {
        for (i = 0; i < TEST_ARR_SIZE; ++i) {
            if (test_int_arr[i] == *v) {
                triang -= i + 1;
            }
        }
    }
    ck_assert_int_eq(triang, 0);

    rf_objset_clear(&set);
} END_TEST

START_TEST (test_subset_default) {
    struct objset_int set1;
    struct objset_int set2;
    unsigned int i;
    rf_objset_init_default(&set1);
    rf_objset_init_default(&set2);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set1, &test_int_arr[i]));
    }
    for (i = 0; i < 3; ++i) {
        ck_assert(rf_objset_add_default(&set2, &test_int_arr[i]));
    }

    ck_assert(!rf_objset_subset_default(&set1, &set2));
    ck_assert(rf_objset_subset_default(&set2, &set1));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
} END_TEST

START_TEST (test_subset_empty_default) {
    struct objset_int set1;
    struct objset_int set2;
    struct objset_int set3;
    unsigned int i;
    rf_objset_init_default(&set1);
    rf_objset_init_default(&set2);
    rf_objset_init_default(&set3);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set3, &test_int_arr[i]));
    }

    ck_assert(rf_objset_subset_default(&set1, &set2));
    ck_assert(rf_objset_subset_default(&set2, &set3));
    ck_assert(!rf_objset_subset_default(&set3, &set2));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
    rf_objset_clear(&set3);
} END_TEST

START_TEST (test_equal_default) {
    struct objset_int set1;
    struct objset_int set2;
    unsigned int i;
    rf_objset_init_default(&set1);
    rf_objset_init_default(&set2);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set1, &test_int_arr[i]));
    }
    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set2, &test_int_arr[i]));
    }

    ck_assert(rf_objset_equal_default(&set1, &set2));
    ck_assert(rf_objset_equal_default(&set2, &set1));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
} END_TEST

START_TEST (test_equal_empty_default) {
    struct objset_int set1;
    struct objset_int set2;
    struct objset_int set3;
    unsigned int i;
    rf_objset_init_default(&set1);
    rf_objset_init_default(&set2);
    rf_objset_init_default(&set3);

    for (i = 0; i < TEST_ARR_SIZE; ++i) {
        ck_assert(rf_objset_add_default(&set3, &test_int_arr[i]));
    }

    ck_assert(rf_objset_equal_default(&set1, &set2));
    ck_assert(rf_objset_equal_default(&set2, &set1));
    ck_assert(!rf_objset_equal_default(&set3, &set2));
    ck_assert(!rf_objset_equal_default(&set2, &set3));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
    rf_objset_clear(&set3);
} END_TEST

Suite *datastructs_objset_suite_create(void)
{
    Suite *s = suite_create("data_structures_objset");

    TCase *tc1 = tcase_create("objset_defaults");
    tcase_add_test(tc1, test_init_default);
    tcase_add_test(tc1, test_add_get_default);
    tcase_add_test(tc1, test_add_duplicates_default);
    tcase_add_test(tc1, test_del_default);
    tcase_add_test(tc1, test_foreach);
    tcase_add_test(tc1, test_subset_default);
    tcase_add_test(tc1, test_subset_empty_default);
    tcase_add_test(tc1, test_equal_default);
    tcase_add_test(tc1, test_equal_empty_default);


    suite_add_tcase(s, tc1);

    return s;
}
