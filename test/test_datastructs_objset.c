#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <String/rf_str_core.h>
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

/* -- Tests for objest with specific functions given -- */

struct person {
    struct RFstring name;
    int age;
};

static inline const void *custom_objset_key(const struct person *p)
{
	return &p->name;
}
static inline size_t custom_objset_hashfn(const struct RFstring *elem)
{
	return rf_hash_str_stable(elem, 0);
}
static inline bool custom_objset_eqfn(const struct person *p1,
                                      const struct person *p2)
{
	return rf_string_equal(&p1->name, &p2->name);
}

OBJSET_DEFINE_TYPE(person,
                   struct person,
                   custom_objset_key,
                   custom_objset_hashfn,
                   custom_objset_eqfn)

static const struct person test_person_arr[] = {
    {.age = 0, .name = RF_STRING_STATIC_INIT("Joe")},
    {.age = 1, .name = RF_STRING_STATIC_INIT("Celina")},
    {.age = 2, .name = RF_STRING_STATIC_INIT("Florian")},
    {.age = 3, .name = RF_STRING_STATIC_INIT("Marek")},
    {.age = 4, .name = RF_STRING_STATIC_INIT("Gavin")},
    {.age = 5, .name = RF_STRING_STATIC_INIT("Katerina")},
    {.age = 6, .name = RF_STRING_STATIC_INIT("Eugenia")},
    {.age = 7, .name = RF_STRING_STATIC_INIT("Chris")},
};
#define TEST_PARR_SIZE (sizeof(test_person_arr) / sizeof(struct person))

static const struct person test_person_arr2[] = {
    {.age = 0, .name = RF_STRING_STATIC_INIT("Joe")},
    {.age = 1, .name = RF_STRING_STATIC_INIT("Celina")},
    {.age = 2, .name = RF_STRING_STATIC_INIT("Nobu")},
    {.age = 3, .name = RF_STRING_STATIC_INIT("Florian")},
    {.age = 4, .name = RF_STRING_STATIC_INIT("Atsushi")},
};
#define TEST_PARR2_SIZE (sizeof(test_person_arr2) / sizeof(struct person))

static const struct person test_person_arr3[] = {
    {.age = 0, .name = RF_STRING_STATIC_INIT("Joe")},
    {.age = 1, .name = RF_STRING_STATIC_INIT("Celina")},
    {.age = 2, .name = RF_STRING_STATIC_INIT("Florian")},
    {.age = 3, .name = RF_STRING_STATIC_INIT("Marek")},
    {.age = 4, .name = RF_STRING_STATIC_INIT("Gavin")},
    {.age = 5, .name = RF_STRING_STATIC_INIT("Katerina")},
    {.age = 6, .name = RF_STRING_STATIC_INIT("Eugenia")},
    {.age = 7, .name = RF_STRING_STATIC_INIT("Chris")},
    {.age = 2, .name = RF_STRING_STATIC_INIT("Nobu")},
    {.age = 4, .name = RF_STRING_STATIC_INIT("Atsushi")},
};
#define TEST_PARR3_SIZE (sizeof(test_person_arr3) / sizeof(struct person))

// exact duplicate of person_arr. Checking that for same key values but different
// pointers sets are gonna be equal
static const struct person test_person_arr_duplicate[] = {
    {.age = 0, .name = RF_STRING_STATIC_INIT("Joe")},
    {.age = 1, .name = RF_STRING_STATIC_INIT("Celina")},
    {.age = 2, .name = RF_STRING_STATIC_INIT("Florian")},
    {.age = 3, .name = RF_STRING_STATIC_INIT("Marek")},
    {.age = 4, .name = RF_STRING_STATIC_INIT("Gavin")},
    {.age = 5, .name = RF_STRING_STATIC_INIT("Katerina")},
    {.age = 6, .name = RF_STRING_STATIC_INIT("Eugenia")},
    {.age = 7, .name = RF_STRING_STATIC_INIT("Chris")},
};
#define TEST_PARRD_SIZE (sizeof(test_person_arr_duplicate) / sizeof(struct person))

START_TEST (test_init) {
    struct rf_objset_person set;
    rf_objset_init(&set, person);
    ck_assert(rf_objset_empty(&set));
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_add_get) {
    struct rf_objset_person set;
    unsigned int i;
    struct person *v;
    rf_objset_init(&set, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person, &test_person_arr[i]));
    }
    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        v = rf_objset_get(&set, person,  &test_person_arr[i]);
        ck_assert(v);
        // simple pointer comparison here since it's the same array
        ck_assert(v == test_person_arr + i);
    }
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_add_duplicates) {
    struct rf_objset_person set;
    unsigned int i;
    struct person *v;
    rf_objset_init(&set, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person,  &test_person_arr[i]));
    }
    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person,  &test_person_arr[i]));
    }
    ck_assert_uint_eq(8, rf_objset_size(&set));

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        v = rf_objset_get(&set, person, &test_person_arr[i]);
        ck_assert(v);
        // simple pointer comparison here since it's the same array
        ck_assert(v == test_person_arr + i);
    }
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_add_duplicates_other_pointers_same_keys) {
    struct rf_objset_person set;
    unsigned int i;
    struct person *v;
    rf_objset_init(&set, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person,  &test_person_arr[i]));
    }
    for (i = 0; i < TEST_PARR2_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person,  &test_person_arr2[i]));
    }
    ck_assert_uint_eq(10, rf_objset_size(&set));

    for (i = 0; i < TEST_PARR3_SIZE; ++i) {
        v = rf_objset_get(&set, person, &test_person_arr3[i]);
        ck_assert(v);
        ck_assert(rf_string_equal(&v->name, &test_person_arr3[i].name));
    }
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_del) {
    struct rf_objset_person set;
    unsigned int i;
    struct person *v;
    rf_objset_init(&set, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person, &test_person_arr[i]));
    }
    ck_assert(rf_objset_del(&set, person, &test_person_arr[7]));
    ck_assert(rf_objset_del(&set, person, &test_person_arr[6]));
    ck_assert(!rf_objset_del(&set, person, &test_person_arr[6]));
    ck_assert_uint_eq(6, rf_objset_size(&set));

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        v = rf_objset_get(&set, person, &test_person_arr[i]);
        if (i <= 5) {
        ck_assert(v);
        // simple pointer comparison here since it's the same array
        ck_assert(v == test_person_arr + i);
        } else {
            ck_assert(!v);
        }
    }
    rf_objset_clear(&set);
} END_TEST

START_TEST (test_foreach_other_pointers) {
    struct rf_objset_person set;
    unsigned int i;
    struct person *v;
    struct rf_objset_iter it;
    rf_objset_init(&set, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person,  &test_person_arr[i]));
    }
    for (i = 0; i < TEST_PARR2_SIZE; ++i) {
        ck_assert(rf_objset_add(&set, person,  &test_person_arr2[i]));
    }
    ck_assert_uint_eq(10, rf_objset_size(&set));

    for (i = 0; i < TEST_PARR3_SIZE; ++i) {
        v = rf_objset_get(&set, person, &test_person_arr3[i]);
        ck_assert(v);
        ck_assert(rf_string_equal(&v->name, &test_person_arr3[i].name));
    }

    // check that each element is found only once during iteration. Just add indices
    // up to get triangular number and then for each index subtract. Should get 0
    int triang = triangular_number(TEST_PARR3_SIZE);
    rf_objset_foreach(&set, &it, v) {
        for (i = 0; i < TEST_PARR3_SIZE; ++i) {
            if (rf_string_equal(&v->name, &test_person_arr3[i].name)) {
                triang -= i + 1;
            }
        }
    }
    ck_assert_int_eq(triang, 0);

    rf_objset_clear(&set);
} END_TEST

START_TEST (test_subset) {
    struct rf_objset_person set1;
    struct rf_objset_person set2;
    unsigned int i;
    rf_objset_init(&set1, person);
    rf_objset_init(&set2, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set1, person, &test_person_arr[i]));
    }
    for (i = 0; i < TEST_PARR3_SIZE; ++i) {
        ck_assert(rf_objset_add(&set2, person, &test_person_arr3[i]));
    }

    ck_assert(rf_objset_subset(&set1, &set2, person));
    ck_assert(!rf_objset_subset(&set2, &set1, person));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
} END_TEST

START_TEST (test_subset_empty) {
    struct rf_objset_person set1;
    struct rf_objset_person set2;
    struct rf_objset_person set3;
    unsigned int i;
    rf_objset_init(&set1, person);
    rf_objset_init(&set2, person);
    rf_objset_init(&set3, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set3, person, &test_person_arr[i]));
    }

    ck_assert(rf_objset_subset(&set1, &set2, person));
    ck_assert(rf_objset_subset(&set2, &set3, person));
    ck_assert(!rf_objset_subset(&set3, &set2, person));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
    rf_objset_clear(&set3);
} END_TEST

START_TEST (test_equal) {
    struct rf_objset_person set1;
    struct rf_objset_person set2;
    unsigned int i;
    rf_objset_init(&set1, person);
    rf_objset_init(&set2, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set1, person, &test_person_arr[i]));
    }
    for (i = 0; i < TEST_PARRD_SIZE; ++i) {
        ck_assert(rf_objset_add(&set2, person, &test_person_arr_duplicate[i]));
    }

    ck_assert(rf_objset_equal(&set1, &set2, person));
    ck_assert(rf_objset_equal(&set2, &set1, person));
    rf_objset_clear(&set1);
    rf_objset_clear(&set2);
} END_TEST

START_TEST (test_equal_empty) {
    struct rf_objset_person set1;
    struct rf_objset_person set2;
    struct rf_objset_person set3;
    unsigned int i;
    rf_objset_init(&set1, person);
    rf_objset_init(&set2, person);
    rf_objset_init(&set3, person);

    for (i = 0; i < TEST_PARR_SIZE; ++i) {
        ck_assert(rf_objset_add(&set3, person, &test_person_arr[i]));
    }

    ck_assert(rf_objset_equal(&set1, &set2, person));
    ck_assert(rf_objset_equal(&set2, &set1, person));
    ck_assert(!rf_objset_equal(&set3, &set2, person));
    ck_assert(!rf_objset_equal(&set2, &set3, person));
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

    TCase *tc2 = tcase_create("objset_with_functions_provided");
    tcase_add_test(tc2, test_init);
    tcase_add_test(tc2, test_add_get);
    tcase_add_test(tc2, test_add_duplicates);
    tcase_add_test(tc2, test_add_duplicates_other_pointers_same_keys);
    tcase_add_test(tc2, test_del);
    tcase_add_test(tc2, test_foreach_other_pointers);
    tcase_add_test(tc2, test_subset);
    tcase_add_test(tc2, test_subset_empty);
    tcase_add_test(tc2, test_equal);
    tcase_add_test(tc2, test_equal_empty);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);

    return s;
}
