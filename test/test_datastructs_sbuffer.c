#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <rflib/datastructs/sbuffer.h>

struct foo {
    uint64_t x;
    double y;
};

static void foo_init(struct foo *f, uint64_t x, double y)
{
    f->x = x;
    f->y = y;
}

static bool foo_init_check(struct foo *f, uint64_t x, double y)
{
    if (!f) {
        return false;
    }
    foo_init(f, x, y);
    return x == f->x && DBLCMP_EQ(y, f->y);
}

static bool realloc_occured = false;
static void reset_realloc()
{
    realloc_occured = false;
}
static void setup_sbuffer_tests()
{
    reset_realloc();
}
static void teardown_sbuffer_tests()
{
}

static bool realloc_cb(struct RFsbuffer *b)
{
    (void)b;
    realloc_occured = true;
    return true;
}

START_TEST (test_sbuffer_simple_alloc) {
    struct RFsbuffer b;
    struct foo *f1;
    struct foo *f2;
    struct foo *f3;
    ck_assert(rf_sbuffer_init(&b, 1024, NULL));
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    f3 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    rf_sbuffer_deinit(&b);
} END_TEST

START_TEST (test_sbuffer_simple_alloc_with_stack_use) {
    struct RFsbuffer b;
    struct foo *f1;
    struct foo *f2;
    struct foo *f3;
    ck_assert(rf_sbuffer_init(&b, 1024, NULL));

    rf_sbuffer_push(&b);
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    f3 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    rf_sbuffer_pop(&b);
    rf_sbuffer_push(&b);
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    f3 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    rf_sbuffer_pop(&b);

    rf_sbuffer_deinit(&b);
} END_TEST

START_TEST (test_sbuffer_simple_alloc_perfect_fit) {
    struct RFsbuffer b;
    struct foo *f1;
    ck_assert(rf_sbuffer_init(&b, 16, realloc_cb));
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    ck_assert(realloc_occured == false);
    rf_sbuffer_deinit(&b);
} END_TEST

START_TEST (test_sbuffer_first_alloc_not_fitting_block) {
    struct RFsbuffer b;
    struct foo *f1;
    ck_assert(rf_sbuffer_init(&b, 12, realloc_cb));
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    ck_assert(realloc_occured);
    rf_sbuffer_deinit(&b);
} END_TEST

START_TEST (test_sbuffer_with_realloc_needed) {
    struct RFsbuffer b;
    struct foo *f1;
    struct foo *f2;
    ck_assert(rf_sbuffer_init(&b, 24, realloc_cb));
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    ck_assert(realloc_occured);
    rf_sbuffer_deinit(&b);
} END_TEST

struct boo {
    uint64_t x;
    double y;
    char padding[128];
};

static void boo_init(struct boo *f, uint64_t x, double y)
{
    f->x = x;
    f->y = y;
}

static bool boo_init_check(struct boo *f, uint64_t x, double y)
{
    if (!f) {
        return false;
    }
    boo_init(f, x, y);
    return x == f->x && DBLCMP_EQ(y, f->y);
}

START_TEST (test_sbuffer_alloc_with_next_realloc_more_than_double_size_of_previous) {
    struct RFsbuffer b;
    struct foo *f1;
    struct boo *b1;
    ck_assert(rf_sbuffer_init(&b, 24, realloc_cb));
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    b1 = rf_sbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b1, 4, 4.14));
    ck_assert(realloc_occured);
    rf_sbuffer_deinit(&b);
} END_TEST

START_TEST (test_sbuffer_alloc_multiple_reallocs_with_push_pop) {
    struct RFsbuffer b;
    struct foo *f1;
    struct foo *f2;
    struct foo *f3;
    struct boo *b1;
    struct boo *b2;
    struct boo *b3;

    // the block has size 24 and fits f1 which has 16 bytes size
    ck_assert(rf_sbuffer_init(&b, 24, realloc_cb));
    f1 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    ck_assert(!realloc_occured);

    rf_sbuffer_push(&b);

    // we allocate a boo which needs 144 bytes and as such we get a reallocation
    b1 = rf_sbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b1, 2, 2.12));
    ck_assert(realloc_occured);
    reset_realloc();

    rf_sbuffer_pop(&b);

    // going back to only f1
    rf_sbuffer_push(&b);

    // allocate some foo structs to where the boo struct was, no more blocks needed
    f2 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    ck_assert(!realloc_occured);
    f3 = rf_sbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    ck_assert(!realloc_occured);

    rf_sbuffer_pop(&b);

    // going back again to only f1
    rf_sbuffer_push(&b);

    // now allocate two boo structs which would totally fill up the remaining
    // size and  right afterwards request a foo struct which would end up with us
    // requesting yet another realloc
    b2 = rf_sbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b2, 6, 6.16));
    ck_assert(!realloc_occured);
    b3 = rf_sbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b3, 7, 7.17));
    ck_assert(realloc_occured);
    rf_sbuffer_pop(&b);

    rf_sbuffer_deinit(&b);
} END_TEST

Suite *datastructs_sbuffer_suite_create(void)
{
    Suite *s = suite_create("data_structures_sbuffer");

    TCase *tc1 = tcase_create("sbuffer_simple");
    tcase_add_checked_fixture(tc1, setup_sbuffer_tests, teardown_sbuffer_tests);
    tcase_add_test(tc1, test_sbuffer_simple_alloc);
    tcase_add_test(tc1, test_sbuffer_simple_alloc_with_stack_use);
    tcase_add_test(tc1, test_sbuffer_simple_alloc_perfect_fit);

    TCase *tc2 = tcase_create("sbuffer_with_reallocs");
    tcase_add_checked_fixture(tc2, setup_sbuffer_tests, teardown_sbuffer_tests);
    tcase_add_test(tc2, test_sbuffer_first_alloc_not_fitting_block);
    tcase_add_test(tc2, test_sbuffer_with_realloc_needed);
    tcase_add_test(tc2, test_sbuffer_alloc_with_next_realloc_more_than_double_size_of_previous);
    tcase_add_test(tc2, test_sbuffer_alloc_multiple_reallocs_with_push_pop);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);

    return s;
}
