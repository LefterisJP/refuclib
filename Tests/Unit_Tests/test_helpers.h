/* just some additional macros to help with unit testing*/
#ifndef RF_UNIT_TESTS_HELPERS
#define RF_UNIT_TESTS_HELPERS

#include <Utils/constcmp.h>

/* RFstring/x related macros */
#define ck_assert_rf_str_eq_cstr(rfstr_, cstr_) \
    ck_assert_int_eq(0, memcmp(rf_string_data(rfstr_), cstr_, strlen(cstr_)))

#define ck_assert_rf_strx_eq_cstr(str_, lit_)                          \
    do{                                                                 \
        ck_assert_uint_ge(((struct RFstringx*)str_)->bSize, strlen(lit_)); \
        ck_assert_int_eq(0, memcmp(rf_string_data(str_), lit_, strlen(lit_))); \
        ck_assert_int_eq(rf_string_data(str_)[((struct RFstringx*)str_)->bIndex], \
                         lit_[0]);                                      \
    }while(0)

#define ck_assert_rf_str_empty(str_) \
    ck_assert_int_eq(0, rf_string_length_bytes(str_))

/* pointer related */
#define ck_assert_not_nullptr(ptr_) \
    ck_assert(ptr_ != NULL)



/* for doubles */
#define ck_assert_double_eq(val1_, val2_)             \
    ck_assert_msg(DBLCMP_EQ(val1_, val2_),            \
                  "double %lf != %lf", val1_, val2_)


#endif
