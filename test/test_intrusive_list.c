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

const static int check_arr[] = { 0, 1, 1, 2, 3, 5};

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

Suite *intrusive_list_suite_create(void)
{
    Suite *s = suite_create("Intrusive_List");

    TCase *tc1 = tcase_create("Intrusive_List_Copy");
    tcase_add_test(tc1, test_intrusive_list_copy);
    tcase_add_test(tc1, test_intrusive_list_copy_empty);

    suite_add_tcase(s, tc1);

    return s;
}
