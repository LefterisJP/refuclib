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

static bool foo_init(struct foo *f, int x, int y, int z, const char *s)
{
    f->x = x;
    f->y = y;
    f->z = z;
    if (!rf_string_init(&f->s, s)) {
        return false;
    }

    return true;
}

static struct foo *foo_create(struct rf_fixed_memorypool *pool,
                       int x, int y, int z, const char *s)
{
    struct foo *f = rf_fixed_memorypool_alloc_element(pool);
    if (!foo_init(f, x, y, z, s)) {
        return NULL;
    }
    return f;
}

static void foo_deinit(struct foo *f)
{
    rf_string_deinit(&f->s);
}

static bool foo_destroy(struct foo *f, struct rf_fixed_memorypool *pool)
{
    foo_deinit(f);
    return rf_fixed_memorypool_free_element(pool, f);
}

static bool foo_equals(struct foo *f, int x, int y, int z, char *s)
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
        ck_assert(foo_ != NULL);                              \
        ck_assert(foo_equals(foo_, x_, y_, z_, cstr_));       \
    } while(0)

START_TEST(test_allocatation) {
    struct rf_fixed_memorypool pool;
    struct foo *f;
    ck_assert(rf_fixed_memorypool_init(&pool, sizeof(struct foo), 256));

    ck_assert_create_foo(f, &pool, 55, 21, 12, "eleos re paidia");
    ck_assert(foo_destroy(f, &pool));

    rf_fixed_memorypool_deinit(&pool);
}END_TEST

START_TEST(test_alloc_free_realloc) {
    struct rf_fixed_memorypool pool;
    struct foo *f;
    struct foo *f2;
    ck_assert(rf_fixed_memorypool_init(&pool, sizeof(struct foo), 256));

    ck_assert_create_foo(f, &pool, 55, 21, 12, "eleos re paidia");
    ck_assert(foo_destroy(f, &pool));

    ck_assert_create_foo(f, &pool, 13, 14, 12, "testity");
    f2 = f;
    ck_assert_create_foo(f, &pool, 1, 2, 3, "pirates are awesome");
    ck_assert(foo_destroy(f2, &pool));
    f2 = f;

    ck_assert_create_foo(f, &pool, 42, 13, 66, "ninjas too");
    ck_assert(foo_destroy(f, &pool));
    ck_assert(foo_destroy(f2, &pool));


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
        ck_assert(foo_destroy(arr[i], &pool));
    }

    rf_fixed_memorypool_deinit(&pool);
}END_TEST


static void int_arr_shuffle(int *array, size_t n)
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

START_TEST(test_alloc_multiple_small_chunks) {
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
    for (i = 0; i < ALLOC_ELEMENTS; i ++) {
        j = indices[i];
        sprintf(buff, "%d", j);
        ck_assert_create_foo(f, &pool, j, j + 1, j + 2, buff);
        arr[j] = f;
    }


    int_arr_shuffle(indices, ALLOC_ELEMENTS);
    // destroy in random order
    for (i = 0; i < ALLOC_ELEMENTS; i ++) {
        j = indices[i];
        ck_assert(foo_destroy(arr[j], &pool));
    }

    rf_fixed_memorypool_deinit(&pool);
}END_TEST

static inline bool should_alloc(int percentage)
{
    return rand() % 100 >= percentage;
}

static void foo_arr_shuffle(struct foo **array, size_t n)
{
    if (n > 1)  {
        size_t i;
        for (i = 0; i < n - 1; i++)  {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          struct foo *t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

static void do_test_multiple_alloc_dealloc(uint32_t pool_chunk_size,
                                           int alloc_percentage,
                                           unsigned int test_actions)
{
    unsigned int i;
    unsigned int allocated_elements = 0;
    unsigned int actions = 0;
    struct rf_fixed_memorypool pool;
    struct foo **arr = malloc(sizeof(struct foo*)*test_actions);
    struct foo *f;
    char buff[10];

    memset(arr, 0, sizeof(struct foo*) * test_actions);
    ck_assert(rf_fixed_memorypool_init(&pool,
                                       sizeof(struct foo),
                                       pool_chunk_size));

    while (actions < test_actions) {
        if (allocated_elements == 0 || should_alloc(alloc_percentage)) {
            /* let's allocate an element */
            sprintf(buff, "%d", actions);
            ck_assert_create_foo(f, &pool, actions, actions + 1,
                                 actions + 2, buff);
            arr[allocated_elements] = f;
            allocated_elements++;
        } else {
            /* let's randomly free an element. Shuffle, pop and destroy last */
            foo_arr_shuffle(arr, allocated_elements);
            ck_assert(foo_destroy(arr[allocated_elements - 1], &pool));
            arr[allocated_elements - 1] = NULL;
            allocated_elements--;
        }
        actions ++;
    }

    //destroy the rest in whatever order they ended up
    for (i = 0; i < allocated_elements; i ++) {
        ck_assert(foo_destroy(arr[i], &pool));
    }

    rf_fixed_memorypool_deinit(&pool);
    free(arr);
}

START_TEST(test_alloc_dealloc_randomly_equal) {
    do_test_multiple_alloc_dealloc(1024, 50, 4096);
}END_TEST

START_TEST(test_alloc_dealloc_randomly_more_alloc) {
    do_test_multiple_alloc_dealloc(1024, 80, 4096);
}END_TEST

#if 0
START_TEST(test_alloc_dealloc_randomly) {
    /* test ACTIONS_TO_TEST allocations and deallocations. Randomly either
     * allocate from the pool or free an allocated element.
     */
    #define ACTIONS_TO_TEST 4096
    unsigned int i;
    unsigned int allocated_elements = 0;
    unsigned int actions = 0;
    struct rf_fixed_memorypool pool;
    struct foo *arr[ACTIONS_TO_TEST];
    int indices[ALLOC_ELEMENTS];
    struct foo *f;
    char buff[10];

    memset(arr, 0, sizeof(struct foo*) * ACTIONS_TO_TEST);
    ck_assert(rf_fixed_memorypool_init(&pool,
                                       sizeof(struct foo),
                                       1024));

    while (actions < ACTIONS_TO_TEST) {
        if (allocated_elements == 0 || should_alloc(50)) {
            /* let's allocate an element */
            sprintf(buff, "%d", actions);
            ck_assert_create_foo(f, &pool, actions, actions + 1,
                                 actions + 2, buff);
            arr[allocated_elements] = f;
            allocated_elements++;
        } else {
            /* let's randomly free an element. Shuffle, pop and destroy last */
            foo_arr_shuffle(arr, allocated_elements);
            ck_assert(foo_destroy(arr[allocated_elements - 1], &pool));
            arr[allocated_elements - 1] = NULL;
            allocated_elements--;
        }
        actions ++;
    }

    //destroy the rest in whatever order they ended up
    for (i = 0; i < allocated_elements; i ++) {
        ck_assert(foo_destroy(arr[i], &pool));
    }

    rf_fixed_memorypool_deinit(&pool);
}END_TEST
#endif

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
    tcase_add_test(fixed_size, test_alloc_multiple_small_chunks);
    tcase_add_test(fixed_size, test_alloc_dealloc_randomly_equal);
    tcase_add_test(fixed_size, test_alloc_dealloc_randomly_more_alloc);

    suite_add_tcase(s, fixed_size);
    return s;
}
