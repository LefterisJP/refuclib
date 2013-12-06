#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <refu.h>
#include <String/rf_str_core.h>
#include <String/rf_str_corex.h>
void setup()
{
    rf_init("refuclib.log", 0, LOG_DEBUG);
}

void teardown()
{
    rf_deinit();
}


START_TEST(test_string_init) {
    struct RFstring s;
    ck_assert(rf_string_init(&s, "This is the test of String initialization"));
    const char* cstr = "This is the test of String initialization";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s), cstr, strlen(cstr)));
    
    ck_assert(!rf_string_init(&s, NULL));
    ck_assert(!rf_string_init(NULL, NULL));

    rf_string_deinit(&s);
}END_TEST

START_TEST(test_stringx_init) {
    struct RFstringx sx;

    //expect all StringX initializations be succesfull
    ck_assert(rf_stringx_init(&sx, "Initializing a StringX"));
    const char* cstr = "Initializing a StringX";
    ck_assert_int_eq(0, memcmp(rf_string_data(&sx), cstr, strlen(cstr)));
    
    ck_assert(!rf_stringx_init(&sx, NULL));
    ck_assert(!rf_stringx_init(&sx, NULL));

    rf_stringx_deinit(&sx);
}END_TEST

START_TEST(test_string_initv) {
    struct RFstring s;
    ck_assert(
        rf_string_initv(&s,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    const char* cstr = "Printf style initialization 1337 3.142 912341";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s), cstr, strlen(cstr)));

    ck_assert(!rf_string_initv(&s, NULL));
    ck_assert(!rf_string_initv(NULL, NULL));

    rf_string_deinit(&s);    
}END_TEST

START_TEST(test_stringx_initv) {
    struct RFstringx sx;
    ck_assert(
        rf_stringx_initv(&sx,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    const char* cstr = "Printf style initialization 1337 3.142 912341";
    ck_assert_int_eq(0, memcmp(rf_string_data(&sx), cstr, strlen(cstr)));

    ck_assert(!rf_stringx_initv(&sx, NULL));
    ck_assert(!rf_stringx_initv(NULL, NULL));

    rf_stringx_deinit(&sx);
}END_TEST



#if 0
START_TEST(test_init_string_simple) {
    struct RFstring s1,s2,s3,s4,s5,s6,s7;
    ck_assert(rf_string_init(&s1, "This is the test of String initialization"));
    ck_assert(
        rf_string_initv(&s2,
                        "%s %d %.3f %u",
                        "Printf style initialization",
                        1337, 3.141592, 912341)
    );
    ck_assert(rf_string_init_double(&s3, 1.6180));
    ck_assert(rf_string_init_int(&s4, 1024));
    ck_assert(rf_string_init_cp(&s5, 0x2708));

    /* verify strings initialized correctly */
    const char* cstr_s1 = "This is the test of String initialization";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s1), cstr_s1, strlen(cstr_s1)));
    const char* cstr_s2 = "Printf style initialization 1337 3.142 912341";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s2), cstr_s2, strlen(cstr_s1)));
    const char* cstr_s3 = "1.618000";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s3), cstr_s3, strlen(cstr_s3)));
    const char* cstr_s4 = "1024";
    ck_assert_int_eq(0, memcmp(rf_string_data(&s4), cstr_s4, strlen(cstr_s4)));

    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s1)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s2)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s3)), 0);
    EXPECTGE(printf(RF_STR_PF_FMT"\n", RF_STR_PF_ARG(&s4)), 0);
}END_TEST
#endif


Suite *string_initialization_suite_create(void)
{
    Suite *s = suite_create("String Initialization");

    TCase *tc_string = tcase_create("String basic");
    tcase_add_checked_fixture(tc_string, setup, teardown);
    tcase_add_test(tc_string, test_string_init);
    tcase_add_test(tc_string, test_stringx_init);
    tcase_add_test(tc_string, test_string_initv);
    tcase_add_test(tc_string, test_stringx_initv);
    suite_add_tcase(s, tc_string);
    return s;
}
