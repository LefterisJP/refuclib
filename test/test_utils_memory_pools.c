#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <refu.h>
#include <Utils/log.h>
#include <Utils/fixed_memory_pool.h>
#include <System/rf_system.h>

#include <String/rf_str_common.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
#include <String/rf_str_retrieval.h>
#include <IO/rf_textfile.h>

struct foo {
    int x;
    int y;
    int z;
    struct RFstring s;
};

bool foo_init(struct foo *f, int x, int y, int z, const char *s)
{
    f->x = x;
    f->y = y;
    f->z = z;
    if (!rf_string_init(&f->s, s)) {
        return false;
    }

    return true;
}

struct foo *foo_create(struct rf_fixed_memorypool *pool,
                       int x, int y, int z, const char *s)
{
    struct foo *f = rf_fixed_memorypool_alloc_element(pool);
    if (!foo_init(f, x, y, z, s)) {
        return NULL;
    }
    return f;
}

void foo_deinit(struct foo *f)
{
    rf_string_deinit(&f->s);
}

void foo_destroy(struct foo *f, struct rf_fixed_memorypool *pool)
{
    foo_deinit(f);
    rf_fixed_memorypool_free_element(pool, f);
}

bool foo_equals(struct foo *f, int x, int y, int z, char *s)
{
    struct RFstring str;
    RF_STRING_SHALLOW_INIT(&str, s, strlen(s));
    if (f->x != x) {
        return false;
    }

    if (f->y != y) {
        return false;
    }

    if (f->z != z) {
        return false;
    }

    if (!rf_string_equal(&f->s, &str)) {
        return false;
    }
    return true;
}

#define ck_assert_create_foo(foo_, pool_, x_, y_, z_, cstr_)  \
    do {                                                      \
        foo_ = foo_create(pool_, x_, y_, z_, cstr_);          \
        ck_assert(foo_);                                      \
        ck_assert(foo_equals(foo_, x_, y_, z_, cstr_));       \
    } while(0)

START_TEST(test_allocatation) {
    struct rf_fixed_memorypool pool;
    struct foo *f;
    ck_assert(rf_fixed_memorypool_init(&pool, sizeof(struct foo), 256));

    ck_assert_create_foo(f, &pool, 55, 21, 12, "eleos re paidia");
    foo_destroy(f, &pool);

    rf_fixed_memorypool_deinit(&pool);
}END_TEST

START_TEST(test_alloc_free_realloc) {
    struct rf_fixed_memorypool pool;
    struct foo *f;
    struct foo *f2;
    ck_assert(rf_fixed_memorypool_init(&pool, sizeof(struct foo), 256));

    ck_assert_create_foo(f, &pool, 55, 21, 12, "eleos re paidia");
    foo_destroy(f, &pool);

    ck_assert_create_foo(f, &pool, 13, 14, 12, "testity");
    f2 = f;
    ck_assert_create_foo(f, &pool, 1, 2, 3, "pirates are awesome");
    foo_destroy(f2, &pool);
    f2 = f;

    ck_assert_create_foo(f, &pool, 42, 13, 66, "ninjas too");
    foo_destroy(f, &pool);
    foo_destroy(f2, &pool);


    rf_fixed_memorypool_deinit(&pool);
}END_TEST

START_TEST(test_alloc_more_than_capacity) {
    #define ALLOC_ELEMENTS 512
    unsigned int i;
    struct rf_fixed_memorypool pool;
    struct foo *arr[ALLOC_ELEMENTS];
    struct foo *f;
    char buff[10];

    ck_assert(rf_fixed_memorypool_init(&pool,
                                       sizeof(struct foo),
                                       ALLOC_ELEMENTS/2));

    for (i = 0; i < ALLOC_ELEMENTS; i ++) {
        sprintf(buff, "%d", i);
        ck_assert_create_foo(f, &pool, i, i + 1, i + 3, buff);
        arr[i] = f;
    }

    for (i = 0; i < ALLOC_ELEMENTS; i ++) {
        foo_destroy(arr[i], &pool);
    }

    rf_fixed_memorypool_deinit(&pool);
}END_TEST


void int_arr_shuffle(int *array, size_t n)
{
    if (n > 1)  {
        size_t i;
        for (i = 0; i < n - 1; i++)  {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

START_TEST(test_alloc_multiple_chunks) {
    #define ALLOC_ELEMENTS 512
    unsigned int i;
    unsigned int j;
    struct rf_fixed_memorypool pool;
    struct foo *arr[ALLOC_ELEMENTS];
    int indices[ALLOC_ELEMENTS];
    struct foo *f;
    char buff[10];

    ck_assert(rf_fixed_memorypool_init(&pool,
                                       sizeof(struct foo),
                                       ALLOC_ELEMENTS/10));

    for (i = 0; i < ALLOC_ELEMENTS; i ++) {
        indices[i] = i;
    }
    int_arr_shuffle(indices, ALLOC_ELEMENTS);

    // create in random order
    for (i = 0, j = indices[i]; i < ALLOC_ELEMENTS; i ++) {
        sprintf(buff, "%d", j);
        ck_assert_create_foo(f, &pool, j, j + 1, j + 1, buff);
        arr[j] = f;
    }

    int_arr_shuffle(indices, ALLOC_ELEMENTS);
    // destroy in random order
    for (i = 0, j = indices[i]; i < ALLOC_ELEMENTS; i ++) {
        foo_destroy(arr[j], &pool);
    }

    rf_fixed_memorypool_deinit(&pool);
}END_TEST

Suite *utils_memory_pools_suite_create(void)
{
    Suite *s = suite_create("Memory_pools");

    TCase *fixed_size = tcase_create("memory_pools_fixed_size");
    tcase_add_checked_fixture(fixed_size,
                              setup_generic_tests,
                              teardown_generic_tests);
    tcase_add_test(fixed_size, test_allocatation);
    tcase_add_test(fixed_size, test_alloc_free_realloc);
    tcase_add_test(fixed_size, test_alloc_more_than_capacity);
    tcase_add_test(fixed_size, test_alloc_multiple_chunks);

    suite_add_tcase(s, fixed_size);
    return s;
}
