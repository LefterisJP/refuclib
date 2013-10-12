typedef struct test_obj
{
    int a;	
    int b;
}test_obj;

int test_init(test_obj* t);
int test_copy(test_obj* dst, test_obj* src);
char test_equal(test_obj* a, test_obj* b);
void test_destroy(test_obj* a);

