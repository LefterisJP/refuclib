#include <RFsystem.h>
#include <stdio.h>
#include <refu.h>
#include <RFstring.h>
#include "../refu_tests.h"

int main()
{
    FILE* f;
    DEFAULT_LIB_INIT();

    EXPECTNOT(f = fopen("afile.txt", "w"), NULL);
    fputc('!', f);
    fclose(f);

    EXPECT(rfFileExists(RFS_("afile.txt")), true);
    EXPECT(rfRenameFile(RFS_("afile.txt"), RFS_("newfile.txt")), true);
    EXPECT(rfFileExists(RFS_("newfile.txt")), true);
    EXPECT(rfDeleteFile(RFS_("newfile.txt")), true);
    EXPECT(rfFileExists(RFS_("newfile.txt")), false);

    return 0;
}
