#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <rflib/refu.h>
#include <rflib/utils/array.h>

static const uint32_t arr2[] = { 15, 25, 35, 45, 32, 65, 74, 12, 32, 45 };


static bool arr_populate_and_check(struct RFarray *arr)
{
    uint32_t v;
    rf_array_at_unsafe(arr, 0, uint32_t) = 13;
    rf_array_at_unsafe(arr, 1, uint32_t) = 37;
    rf_array_at_unsafe(arr, 2, uint32_t) = 42;


    rf_array_get(arr, 0, uint32_t, v, ck_abort_msg("rf_array_get() failed"));
    ck_assert_int_eq(v, 13);
    rf_array_get(arr, 1, uint32_t, v, ck_abort_msg("rf_array_get() failed"));
    ck_assert_int_eq(v, 37);
    rf_array_get(arr, 2, uint32_t, v, ck_abort_msg("rf_array_get() failed"));
    ck_assert_int_eq(v, 42);

    rf_array_get(arr, 3, uint32_t, v, return true);
    return false;
}

static void arr_set_realloc_check(struct RFarray *arr)
{
    unsigned int i;
    uint32_t v;

    for (i = 0; i < 10; i++) {
        rf_array_set(arr, i, uint32_t, arr2[i],
                     ck_abort_msg("rf_array_set() failed"));
    }

    for (i = 0; i < 10; i++) {
        rf_array_get(arr, i, uint32_t, v,
                     ck_abort_msg("rf_array_get() failed"));
        ck_assert_uint_eq(v, arr2[i]);
    }
}

START_TEST (test_array_temp_init) {
    struct RFarray arr;
    RF_ARRAY_TEMP_INIT(&arr, uint32_t, 3);

    ck_assert_msg(arr_populate_and_check(&arr),
                  "Temporary initialized array failed at checking");

    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_shallow_init) {
    uint32_t temp_arr[3];
    struct RFarray arr = RF_ARRAY_SHALLOW_INIT(temp_arr);

    ck_assert_msg(arr_populate_and_check(&arr),
                  "Shallow initialized array failed at checking");
    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_init) {
    struct RFarray arr;
    ck_assert(rf_array_init(&arr, 3, uint32_t));
    ck_assert_msg(arr_populate_and_check(&arr),
                  "Heap initialized array failed at checking");
    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_get) {
    struct RFarray arr;
    unsigned int i;
    uint32_t v;
    ck_assert(rf_array_init(&arr, 5, uint32_t));

    for (i = 0; i < 5; i++) {
        rf_array_set(&arr, i, uint32_t, arr2[i],
                     ck_abort_msg("rf_array_set() failed"));
    }

    for (i = 0; i < 5; i++) {
        rf_array_get(&arr, i, uint32_t, v,
                     ck_abort_msg("rf_array_get() failed"));
        ck_assert_uint_eq(v, arr2[i]);
    }

    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_at_unsafe) {
    struct RFarray arr;
    unsigned int i;
    ck_assert(rf_array_init(&arr, 5, uint32_t));

    for (i = 0; i < 5; i++) {
        rf_array_set(&arr, i, uint32_t, arr2[i],
                     ck_abort_msg("rf_array_set() failed"));
    }

    for (i = 0; i < 5; i++) {
        ck_assert_uint_eq(arr2[i], rf_array_at_unsafe(&arr, i, uint32_t));
    }

    rf_array_deinit(&arr);
} END_TEST


START_TEST (test_array_set_realloc_normal) {
    struct RFarray arr;

    ck_assert(rf_array_init(&arr, 5, uint32_t));
    arr_set_realloc_check(&arr);
    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_set_realloc_with_fixed) {
    uint32_t temp_arr[5];
    struct RFarray arr = RF_ARRAY_SHALLOW_INIT(temp_arr);

    arr_set_realloc_check(&arr);
    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_set_realloc_with_temp) {
    struct RFarray arr;

    RF_ARRAY_TEMP_INIT(&arr, uint32_t, 5);
    arr_set_realloc_check(&arr);
    rf_array_deinit(&arr);
} END_TEST

START_TEST (test_array_set_norealloc) {
    struct RFarray arr;
    unsigned int i;
    bool set_failed = false;
    ck_assert(rf_array_init(&arr, 5, uint32_t));

    for (i = 0; i < 5; i++) {
        rf_array_set_norealloc(&arr, i, uint32_t, arr2[i],
                     ck_abort_msg("rf_array_set() failed"));
    }

    // the last index that would need reallocation should fail
    rf_array_set_norealloc(&arr, 5, uint32_t, 643, set_failed = true);
    ck_assert_msg(set_failed, "Setting out of bound index did not fail");

    // finally also check that setting worked
    for (i = 0; i < 5; i++) {
        ck_assert_uint_eq(arr2[i], rf_array_at_unsafe(&arr, i, uint32_t));
    }

    rf_array_deinit(&arr);
} END_TEST

Suite *utils_array_suite_create(void)
{
    Suite *s = suite_create("Utils array");

    TCase *utils_array_initialization = tcase_create(
        "Utils array initialization"
    );
    tcase_add_test(utils_array_initialization, test_array_temp_init);
    tcase_add_test(utils_array_initialization, test_array_shallow_init);
    tcase_add_test(utils_array_initialization, test_array_init);

    TCase *utils_array_getters = tcase_create(
        "Utils array getters"
    );
    tcase_add_test(utils_array_getters, test_array_get);
    tcase_add_test(utils_array_getters, test_array_at_unsafe);

    TCase *utils_array_setters = tcase_create(
        "Utils array setters"
    );
    tcase_add_test(utils_array_setters, test_array_set_realloc_normal);
    tcase_add_test(utils_array_setters, test_array_set_realloc_with_fixed);
    tcase_add_test(utils_array_setters, test_array_set_realloc_with_temp);
    tcase_add_test(utils_array_setters, test_array_set_norealloc);


    suite_add_tcase(s, utils_array_initialization);
    suite_add_tcase(s, utils_array_getters);
    suite_add_tcase(s, utils_array_setters);

    return s;
}
