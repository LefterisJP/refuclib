#include <refu.h>
#include <RFtextfile.h>
#include <RFstring.h>
#include "../refu_tests.h"





int main()
{
    RF_TextFile log_file;
    RF_StringX buff;
    DEFAULT_LIB_INIT();

    RF_ERROR("This is an error");
    RF_INFO("This is an info");
    RF_WARNING("This is a warning");
    RF_DEBUG("This is a debug message");
    EXPECT_TRUE(rf_LogFlush());

    EXPECT_TRUE(rfTextFile_Init(&log_file, RFS_("refulib.log"), RF_FILE_READ,
                                RF_LITTLE_ENDIAN, RF_UTF8, RF_EOL_LF));
    EXPECT_TRUE(rfStringX_Init_buff(&buff, 1024, ""));
    
    EXPECT(rfTextFile_ReadLine(&log_file, &buff), RF_SUCCESS);
    EXPECT_NOT(RF_FAILURE, rfString_Find(&buff, RFS_("This is an error"), 0));
    
    EXPECT(rfTextFile_ReadLine(&log_file, &buff), RF_SUCCESS);
    EXPECT_NOT(RF_FAILURE, rfString_Find(&buff, RFS_("This is an info"), 0));

    EXPECT(rfTextFile_ReadLine(&log_file, &buff), RF_SUCCESS);
    EXPECT_NOT(RF_FAILURE, rfString_Find(&buff, RFS_("This is a warning"), 0));


    rfStringX_Deinit(&buff);
    rfTextFile_Deinit(&log_file);
    return 0;
}
