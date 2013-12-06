#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Suite *string_initialization_suite_create(void);

static const char *SILENT = "CK_SILENT";
static const char *MINIMAL = "CK_MINIMAL";
static const char *NORMAL = "CK_NORMAL";
static const char *VERBOSE = "CK_VERBOSE";


static enum print_output choose_print_output(char* arg)
{
    if (strcmp(arg, SILENT) == 0) {
        return CK_SILENT;
    } else if (strcmp(arg, MINIMAL) == 0) {
        return CK_MINIMAL;
    } else if (strcmp(arg, NORMAL) == 0) {
        return CK_NORMAL;
    } else if (strcmp(arg, VERBOSE) == 0) {
        return CK_VERBOSE;
    } else {
        //should never happen but let's just go verbose
        return CK_VERBOSE;
    }
}


int main(int argc, char **argv)
{
    int number_failed;
    enum print_output print_type;
    print_type = CK_VERBOSE;
    if (argc >= 2) {
        print_type = choose_print_output(argv[1]);
    }
    printf("\n\n=== Running Refu C library Unit Tests ===\n");
    SRunner *sr = srunner_create(string_initialization_suite_create());
    srunner_run_all(sr, print_type);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
