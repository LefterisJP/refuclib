#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "utilities_for_testing.h"
#include <rflib/string/rf_str_core.h>
#include <rflib/datastructs/htable.h>
#include <rflib/utils/hash.h>

struct object {
    struct RFstring name;
    unsigned int val;
};

struct object *object_create(const char *str, unsigned int val)
{
    struct object *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_string_init(&ret->name, str)) {
        free(ret);
        return NULL;
    }
    ret->val = val;
    return ret;
}

void object_destroy(struct object *obj)
{
    rf_string_deinit(&obj->name);
    free(obj);
}

static size_t rehash_fn(const void *e, void *user_arg)
{
    struct object *obj = (struct object*)e;
    (void)user_arg;
    return rf_hash_str_stable(&obj->name, 0);
}

static bool cmp_fn(const void *e, void *str)
{
    struct object *obj = (struct object*)e;
    return rf_string_equal(&obj->name, str);
}

START_TEST (test_ht_get_simple) {
    struct htable ht;
    const struct RFstring s = RF_STRING_STATIC_INIT("Celina");
    htable_init(&ht, rehash_fn, NULL);
    struct object *obj = object_create("Celina", 55);
    ck_assert(obj);
    ck_assert(htable_add(&ht, rf_hash_str_stable(&obj->name, 0), obj));
    ck_assert(obj == htable_get(&ht, rf_hash_str_stable(&s, 0), cmp_fn, &s));

    htable_clear(&ht);
    object_destroy(obj);
} END_TEST

START_TEST (test_ht_get_empty) {
    struct htable ht;
    const struct RFstring s = RF_STRING_STATIC_INIT("Celina");
    htable_init(&ht, rehash_fn, NULL);
    ck_assert(!htable_get(&ht, rf_hash_str_stable(&s, 0), cmp_fn, &s));
    htable_clear(&ht);
} END_TEST


Suite *datastructs_htable_suite_create(void)
{
    Suite *s = suite_create("data_structures_htable");

    TCase *tc1 = tcase_create("htable_get");
    tcase_add_test(tc1, test_ht_get_simple);
    tcase_add_test(tc1, test_ht_get_empty);

    suite_add_tcase(s, tc1);
    return s;
}
