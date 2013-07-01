#include "test.h"



int test_init(test_obj* t)
{
    t->a = 1;
    t->b = 2;
    return 1;
}

int test_copy(test_obj* dst, test_obj* src)
{
    dst->a = src->a;
    dst->b = src->b;
    return 1;
}
char test_equal(test_obj* a, test_obj* b)
{
    if(a->a != b->a && a->b != b->b)
        return 0;
    return 1;
}

void test_destroy(test_obj* a)
{
}

