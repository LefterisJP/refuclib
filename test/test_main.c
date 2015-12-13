#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Suite *string_core_suite_create(void);
Suite *string_conversion_suite_create(void);
Suite *string_retrieval_suite_create(void);
Suite *string_files_suite_create(void);
Suite *string_manipulation_suite_create(void);
Suite *string_traversal_suite_create(void);
Suite *string_buffers_suite_create(void);

Suite *regex_suite_create(void);

Suite *utils_unicode_suite_create(void);
Suite *utils_array_suite_create(void);
Suite *utils_memory_pools_suite_create(void);
Suite *datastructs_objset_suite_create(void);
Suite *datastructs_sbuffer_suite_create(void);
Suite *datastructs_mbuffer_suite_create(void);
Suite *datastructs_darray_suite_create(void);
Suite *datastructs_strmap_suite_create(void);
Suite *datastructs_htable_suite_create(void);

Suite *intrusive_list_suite_create(void);

Suite *io_files_suite_create(void);
Suite *io_textfile_suite_create(void);

Suite *log_suite_create(void);

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

static enum fork_status choose_fork_status(char* arg)
{
    enum fork_status type = CK_FORK;
    if (strcmp(arg, "False") == 0) {
        type = CK_NOFORK;
    }
    return type;
}

int main(int argc, char **argv)
{
    int number_failed;
    enum print_output print_type;
    enum fork_status fork_type;
    /* default values */
    print_type = CK_VERBOSE;
    fork_type = CK_FORK;

    if (argc >= 2) {
        print_type = choose_print_output(argv[1]);
    }
    if (argc >= 3) {
        fork_type = choose_fork_status(argv[2]);
    }

    printf("\n\n=== Running Refu C library Unit Tests ===\n");
    SRunner *sr = srunner_create(log_suite_create());

    srunner_add_suite(sr, string_core_suite_create());
    srunner_add_suite(sr, string_conversion_suite_create());
    srunner_add_suite(sr, string_retrieval_suite_create());
    srunner_add_suite(sr, string_files_suite_create());
    srunner_add_suite(sr, string_manipulation_suite_create());
    srunner_add_suite(sr, string_traversal_suite_create());
    srunner_add_suite(sr, string_buffers_suite_create());
    srunner_add_suite(sr, regex_suite_create());

    srunner_add_suite(sr, utils_unicode_suite_create());
    srunner_add_suite(sr, utils_array_suite_create());
    srunner_add_suite(sr, utils_memory_pools_suite_create());
    srunner_add_suite(sr, datastructs_objset_suite_create());
    srunner_add_suite(sr, datastructs_sbuffer_suite_create());
    srunner_add_suite(sr, datastructs_mbuffer_suite_create());
    srunner_add_suite(sr, datastructs_darray_suite_create());
    srunner_add_suite(sr, datastructs_strmap_suite_create());
    srunner_add_suite(sr, datastructs_htable_suite_create());

    srunner_add_suite(sr, intrusive_list_suite_create());

    srunner_add_suite(sr, io_files_suite_create());
    srunner_add_suite(sr, io_textfile_suite_create());

    srunner_set_fork_status (sr, fork_type);
    srunner_run_all(sr, print_type);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    /* close standard streams to silence valgrind */
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
