#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"

#include <refu.h>
#include <Data_Structures/intrusive_list.h>

struct foo_elem {
    struct RFilist_node ln;
    int value;
};

struct foo_elem *foo_elem_create(int val)
{
    struct foo_elem *elem;
    RF_MALLOC(elem, sizeof(*elem), return NULL);
    elem->value = val;
    return elem;
}

void foo_elem_destroy(struct foo_elem *f)
{
    free(f);
}

struct foo {
    struct RFilist_head lh;
    int elems;
};

struct foo *foo_create()
{
    struct foo *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    rf_ilist_head_init(&ret->lh);
    ret->elems = 0;
    return ret;
}

// free foo without freeing the list
void foo_destroy(struct foo *f)
{
    free(f);
}

bool list_add(struct RFilist_head *list, int val)
{
    struct foo_elem *elem = foo_elem_create(val);
    if (!elem) {
        return false;
    }
    rf_ilist_add_tail(list, &elem->ln);
    return true;
}

bool foo_add(struct foo *f, int val)
{
    if (!list_add(&f->lh, val)) {
        return false;
    }
    f->elems ++;
    return true;
}

static const int check_arr[] = { 0, 1, 1, 2, 3, 5};

START_TEST (test_intrusive_list_copy) {
    struct foo *f1;
    struct foo *f2;
    unsigned int i;
    struct foo_elem *elem;
    struct foo_elem *tmp;

    f1 = foo_create();
    ck_assert(f1);
    for (i = 0; i < sizeof(check_arr) / sizeof(int); i++) {
        ck_assert(foo_add(f1, check_arr[i]));
    }

    f2 = foo_create();
    ck_assert(f2);
    rf_ilist_copy(&f1->lh, &f2->lh);
    foo_destroy(f1);

    i = 0;
    rf_ilist_for_each(&f2->lh, elem, ln) {
        ck_assert_int_eq(elem->value, check_arr[i]);
        i++;
    }

    // test done, free stuff
    rf_ilist_for_each_safe(&f2->lh, elem, tmp, ln) {
        foo_elem_destroy(elem);
    }
    foo_destroy(f2);
} END_TEST

START_TEST (test_intrusive_list_copy_empty) {
    struct RFilist_head h1;
    struct RFilist_head h2;
    struct foo_elem *elem;
    struct foo_elem *tmp;
    unsigned int i;

    rf_ilist_head_init(&h1);
    rf_ilist_copy(&h1, &h2);
    for (i = 0; i < sizeof(check_arr) / sizeof(int); i++) {
        list_add(&h2, check_arr[i]);
    }

    i = 0;
    rf_ilist_for_each(&h2, elem, ln) {
        ck_assert_int_eq(elem->value, check_arr[i]);
        i++;
    }

    // test done, free stuff
    rf_ilist_for_each_safe(&h2, elem, tmp, ln) {
        foo_elem_destroy(elem);
    }
} END_TEST

START_TEST (test_intrusive_list_pop) {
    struct RFilist_head h1;
    struct foo_elem *used1 = foo_elem_create(1);
    struct foo_elem *used2 = foo_elem_create(2);
    struct foo_elem *used3 = foo_elem_create(3);
    rf_ilist_head_init(&h1);
    rf_ilist_add_tail(&h1, &used1->ln);
    rf_ilist_add_tail(&h1, &used2->ln);
    rf_ilist_add_tail(&h1, &used3->ln);

    struct foo_elem *p = rf_ilist_pop(&h1, struct foo_elem, ln);
    ck_assert_int_eq(p->value, 1);
    ck_assert_int_eq(rf_ilist_size(&h1), 2);

    foo_elem_destroy(used1);
    foo_elem_destroy(used2);
    foo_elem_destroy(used3);
} END_TEST

START_TEST (test_intrusive_list_pop_back) {
    struct RFilist_head h1;
    struct foo_elem *used1 = foo_elem_create(1);
    struct foo_elem *used2 = foo_elem_create(2);
    struct foo_elem *used3 = foo_elem_create(3);
    rf_ilist_head_init(&h1);
    rf_ilist_add_tail(&h1, &used1->ln);
    rf_ilist_add_tail(&h1, &used2->ln);
    rf_ilist_add_tail(&h1, &used3->ln);

    struct foo_elem *p = rf_ilist_pop_back(&h1, struct foo_elem, ln);
    ck_assert_int_eq(p->value, 3);
    ck_assert_int_eq(rf_ilist_size(&h1), 2);

    foo_elem_destroy(used1);
    foo_elem_destroy(used2);
    foo_elem_destroy(used3);
} END_TEST

START_TEST (test_intrusive_list_pop_until) {
    static const int carr[] = { 4, 5 };
    struct RFilist_head h1;
    struct foo_elem *used1 = foo_elem_create(1);
    struct foo_elem *used2 = foo_elem_create(2);
    struct foo_elem *used3 = foo_elem_create(3);
    struct foo_elem *used4 = foo_elem_create(4);
    struct foo_elem *used5 = foo_elem_create(5);
    rf_ilist_head_init(&h1);
    rf_ilist_add_tail(&h1, &used1->ln);
    rf_ilist_add_tail(&h1, &used2->ln);
    rf_ilist_add_tail(&h1, &used3->ln);
    rf_ilist_add_tail(&h1, &used4->ln);
    rf_ilist_add_tail(&h1, &used5->ln);

    rf_ilist_pop_until(&h1, &used4->ln);
    unsigned int idx = 0;
    struct foo_elem *elem;
    rf_ilist_for_each(&h1, elem, ln) {
        ck_assert_int_eq(elem->value, carr[idx]);
        ++idx;
    }

    foo_elem_destroy(used1);
    foo_elem_destroy(used2);
    foo_elem_destroy(used3);
    foo_elem_destroy(used4);
    foo_elem_destroy(used5);
} END_TEST

START_TEST (test_intrusive_list_pop_back_until) {
    static const int carr[] = { 1, 2, 3};
    struct RFilist_head h1;
    struct foo_elem *used1 = foo_elem_create(1);
    struct foo_elem *used2 = foo_elem_create(2);
    struct foo_elem *used3 = foo_elem_create(3);
    struct foo_elem *used4 = foo_elem_create(4);
    struct foo_elem *used5 = foo_elem_create(5);
    rf_ilist_head_init(&h1);
    rf_ilist_add_tail(&h1, &used1->ln);
    rf_ilist_add_tail(&h1, &used2->ln);
    rf_ilist_add_tail(&h1, &used3->ln);
    rf_ilist_add_tail(&h1, &used4->ln);
    rf_ilist_add_tail(&h1, &used5->ln);

    rf_ilist_pop_back_until(&h1, &used3->ln);
    unsigned int idx = 0;
    struct foo_elem *elem;
    rf_ilist_for_each(&h1, elem, ln) {
        ck_assert_int_eq(elem->value, carr[idx]);
        ++idx;
    }

    foo_elem_destroy(used1);
    foo_elem_destroy(used2);
    foo_elem_destroy(used3);
    foo_elem_destroy(used4);
    foo_elem_destroy(used5);
} END_TEST

START_TEST (test_intrusive_list_has) {
    struct RFilist_head h1;
    struct foo_elem *used1 = foo_elem_create(1);
    struct foo_elem *used2 = foo_elem_create(2);
    struct foo_elem *used3 = foo_elem_create(3);
    rf_ilist_head_init(&h1);
    rf_ilist_add_tail(&h1, &used1->ln);
    rf_ilist_add_tail(&h1, &used2->ln);
    rf_ilist_add_tail(&h1, &used3->ln);

    // check that all of the added list objects return true;
    ck_assert(rf_ilist_has(&h1, &used1->ln));
    ck_assert(rf_ilist_has(&h1, &used2->ln));
    ck_assert(rf_ilist_has(&h1, &used3->ln));

    //create 2 other objects, don't add them and check if they are there
    struct foo_elem *unknown1 = foo_elem_create(13234);
    struct foo_elem *unknown2 = foo_elem_create(918034);
    ck_assert(!rf_ilist_has(&h1, &unknown1->ln));
    ck_assert(!rf_ilist_has(&h1, &unknown2->ln));

    // free stuff
    foo_elem_destroy(used1);
    foo_elem_destroy(used2);
    foo_elem_destroy(used3);
    foo_elem_destroy(unknown1);
    foo_elem_destroy(unknown2);
} END_TEST

START_TEST (test_intrusive_list_size) {
    struct foo *f1;
    unsigned int i;
    struct foo_elem *elem;
    struct foo_elem *tmp;

    f1 = foo_create();
    ck_assert(f1);
    for (i = 0; i < sizeof(check_arr) / sizeof(int); i++) {
        ck_assert(foo_add(f1, check_arr[i]));
    }
    ck_assert_uint_eq(sizeof(check_arr) / sizeof(int), rf_ilist_size(&f1->lh));

    // test done, free stuff
    rf_ilist_for_each_safe(&f1->lh, elem, tmp, ln) {
        foo_elem_destroy(elem);
    }
    foo_destroy(f1);
} END_TEST

struct babushka {
    struct foo_elem e;
};

struct babushka *babushka_create(int val)
{
    struct babushka *b;
    RF_MALLOC(b, sizeof(*b), return NULL);
    b->e.value = val;
    return b;
}

void babushka_destroy(struct babushka *b)
{
    free(b);
}

START_TEST (test_intrusive_list_nested) {
    // test for working with intrusive lists if handles are nested within structures
    struct RFilist_head list;
    rf_ilist_head_init(&list);
    struct babushka *b = babushka_create(3);
    rf_ilist_add_tail(&list, &b->e.ln);
    b = babushka_create(5);
    rf_ilist_add_tail(&list, &b->e.ln);
    b = babushka_create(42);
    rf_ilist_add_tail(&list, &b->e.ln);

    b = rf_ilist_pop(&list, struct babushka, e.ln);
    ck_assert_int_eq(b->e.value, 3);
    babushka_destroy(b);
    b = rf_ilist_pop(&list, struct babushka, e.ln);
    ck_assert_int_eq(b->e.value, 5);
    babushka_destroy(b);
    b = rf_ilist_pop(&list, struct babushka, e.ln);
    ck_assert_int_eq(b->e.value, 42);
    babushka_destroy(b);
    ck_assert(!rf_ilist_pop(&list, struct babushka, e.ln));
} END_TEST

Suite *intrusive_list_suite_create(void)
{
    Suite *s = suite_create("Intrusive_List");

    TCase *tc1 = tcase_create("Intrusive_List_Copy");
    tcase_add_test(tc1, test_intrusive_list_copy);
    tcase_add_test(tc1, test_intrusive_list_copy_empty);

    TCase *tc2 = tcase_create("Intrusive_List_Removals");
    tcase_add_test(tc2, test_intrusive_list_pop);
    tcase_add_test(tc2, test_intrusive_list_pop_back);
    tcase_add_test(tc2, test_intrusive_list_pop_until);
    tcase_add_test(tc2, test_intrusive_list_pop_back_until);

    TCase *tc3 = tcase_create("Intrusive_List_Misc");
    tcase_add_test(tc3, test_intrusive_list_has);
    tcase_add_test(tc3, test_intrusive_list_size);
    tcase_add_test(tc3, test_intrusive_list_nested);

    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    suite_add_tcase(s, tc3);
    return s;
}
