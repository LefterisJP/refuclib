#include <RFsystem.h>
#include <stdio.h>
#include <refu.h>
#include <RFstring.h>
#include "../refu_tests.h"

int main()
{
    FILE* f;
    DEFAULT_LIB_INIT();

    EXPECT_NOT(f = fopen("afile.txt", "w"), NULL);
    fputc('!', f);
    fclose(f);

    EXPECT(rf_system_file_exists(RFS_("afile.txt")), true);
    EXPECT(rf_system_rename_file(RFS_("afile.txt"), RFS_("newfile.txt")), true);
    EXPECT(rf_system_file_exists(RFS_("newfile.txt")), true);
    EXPECT(rf_system_delete_file(RFS_("newfile.txt")), true);
    EXPECT(rf_system_file_exists(RFS_("newfile.txt")), false);

    return 0;
}
