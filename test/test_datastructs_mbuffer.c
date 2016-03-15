#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <rflib/datastructs/mbuffer.h>

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

START_TEST (test_mbuffer_simple_alloc) {
    struct RFmbuffer b;
    struct foo *f1;
    struct foo *f2;
    struct foo *f3;
    ck_assert(rf_mbuffer_init(&b, 1024));
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    f3 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    rf_mbuffer_deinit(&b);
} END_TEST

START_TEST (test_mbuffer_simple_alloc_with_stack_use) {
    struct RFmbuffer b;
    struct foo *f1;
    struct foo *f2;
    struct foo *f3;
    ck_assert(rf_mbuffer_init(&b, 1024));

    rf_mbuffer_push(&b);
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    f3 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    rf_mbuffer_pop(&b);
    rf_mbuffer_push(&b);
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    f3 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    rf_mbuffer_pop(&b);

    rf_mbuffer_deinit(&b);
} END_TEST

START_TEST (test_mbuffer_simple_alloc_perfect_fit) {
    struct RFmbuffer b;
    struct foo *f1;
    ck_assert(rf_mbuffer_init(&b, 16));
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    ck_assert_uint_eq(b.blocks_num, 1);
    rf_mbuffer_deinit(&b);
} END_TEST

START_TEST (test_mbuffer_alloc_not_fitting_first_block) {
    struct RFmbuffer b;
    struct foo *f1;
    ck_assert(rf_mbuffer_init(&b, 12));
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    ck_assert_uint_eq(b.blocks_num, 2);
    rf_mbuffer_deinit(&b);
} END_TEST

START_TEST (test_mbuffer_alloc_with_new_block_needed) {
    struct RFmbuffer b;
    struct foo *f1;
    struct foo *f2;
    ck_assert(rf_mbuffer_init(&b, 24));
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    f2 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    ck_assert_uint_eq(b.blocks_num, 2);
    rf_mbuffer_deinit(&b);
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

START_TEST (test_mbuffer_alloc_with_next_block_more_than_double_size_of_previous) {
    struct RFmbuffer b;
    struct foo *f1;
    struct boo *b1;
    ck_assert(rf_mbuffer_init(&b, 24));
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    b1 = rf_mbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b1, 4, 4.14));
    ck_assert_uint_eq(b.blocks_num, 2);
    rf_mbuffer_deinit(&b);
} END_TEST

START_TEST (test_mbuffer_alloc_multiple_blocks_with_push_pop) {
    struct RFmbuffer b;
    struct foo *f1;
    struct foo *f2;
    struct foo *f3;
    struct foo *f4;
    struct boo *b1;
    struct boo *b2;
    struct boo *b3;

    // first block has size 24 and fits f1 which has 16 bytes size
    ck_assert(rf_mbuffer_init(&b, 24));
    f1 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f1, 3, 3.13));
    ck_assert_uint_eq(b.blocks_num, 1);
    ck_assert_uint_eq(b.curr_block_idx, 0);

    rf_mbuffer_push(&b);

    // we allocate a boo which needs 144 bytes and as such second block is asked for
    // we are now at second block
    b1 = rf_mbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b1, 2, 2.12));
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 1);

    rf_mbuffer_pop(&b);

    // going back to only f1
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 0);

    rf_mbuffer_push(&b);

    // allocate some foo structs to where the boo struct was, no more blocks needed
    f2 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f2, 4, 4.14));
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 1);
    f3 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f3, 5, 5.15));
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 1);

    rf_mbuffer_pop(&b);

    // going back again to only f1
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 0);

    rf_mbuffer_push(&b);

    // now allocate two boo structs which would totally fill up the 2nd block
    // right afterwards request a foo struct which would end up with us requesting
    // a 3rd block
    b2 = rf_mbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b2, 6, 6.16));
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 1);
    b3 = rf_mbuffer_alloc(&b, sizeof(struct boo));
    ck_assert(boo_init_check(b3, 7, 7.17));
    ck_assert_uint_eq(b.blocks_num, 2);
    ck_assert_uint_eq(b.curr_block_idx, 1);
    f4 = rf_mbuffer_alloc(&b, sizeof(struct foo));
    ck_assert(foo_init_check(f4, 8, 8.18));
    ck_assert_uint_eq(b.blocks_num, 3);
    ck_assert_uint_eq(b.curr_block_idx, 2);
    rf_mbuffer_pop(&b);

    // go back again to only f1
    ck_assert_uint_eq(b.blocks_num, 3);
    ck_assert_uint_eq(b.curr_block_idx, 0);

    rf_mbuffer_deinit(&b);
} END_TEST

Suite *datastructs_mbuffer_suite_create(void)
{
    Suite *s = suite_create("data_structures_mbuffer");

    TCase *tc1 = tcase_create("mbuffer_simple");
    tcase_add_test(tc1, test_mbuffer_simple_alloc);
    tcase_add_test(tc1, test_mbuffer_simple_alloc_with_stack_use);
    tcase_add_test(tc1, test_mbuffer_simple_alloc_perfect_fit);

    TCase *tc2 = tcase_create("mbuffer_with_reallocs");
    tcase_add_test(tc2, test_mbuffer_alloc_not_fitting_first_block);
    tcase_add_test(tc2, test_mbuffer_alloc_with_new_block_needed);
    tcase_add_test(tc2, test_mbuffer_alloc_with_next_block_more_than_double_size_of_previous);
    tcase_add_test(tc2, test_mbuffer_alloc_multiple_blocks_with_push_pop);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);

    return s;
}
