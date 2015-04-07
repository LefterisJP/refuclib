#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"


#include <Data_Structures/objset.h>



START_TEST (test_init_default) {
    struct objset_int { OBJSET_MEMBERS(int *); } foo;
    rf_objset_init_default(&foo);
    rf_objset_clear(&foo);
} END_TEST


Suite *datastructs_objset_suite_create(void)
{
    Suite *s = suite_create("data_structures_objset");

    TCase *tc1 = tcase_create("simple");
    tcase_add_test(tc1, test_init_default);


    suite_add_tcase(s, tc1);

    return s;
}
