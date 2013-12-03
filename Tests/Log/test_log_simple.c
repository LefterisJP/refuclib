#include <refu.h>
#include <RFtextfile.h>
#include <RFstring.h>
#include "../refu_tests.h"





int main()
{
    RFtextfile log_file;
    struct RFstringx buff;
    DEFAULT_LIB_INIT();

    RF_ERROR("This is an error");
    RF_INFO("This is an info");
    RF_WARNING("This is a warning");
    RF_DEBUG("This is a debug message");
    EXPECT_TRUE(rf_LogFlush());

    EXPECT_TRUE(rf_textfile_init(&log_file, RFS_("refulib.log"), RF_FILE_READ,
                                RF_LITTLE_ENDIAN, RF_UTF8, RF_EOL_LF));
    EXPECT_TRUE(rf_stringx_init_buff(&buff, 1024, ""));
    
    EXPECT(rf_textfile_read_line(&log_file, &buff), RF_SUCCESS);
    EXPECT_NOT(RF_FAILURE, rf_string_find(&buff, RFS_("This is an error"), 0));
    
    EXPECT(rf_textfile_read_line(&log_file, &buff), RF_SUCCESS);
    EXPECT_NOT(RF_FAILURE, rf_string_find(&buff, RFS_("This is an info"), 0));

    EXPECT(rf_textfile_read_line(&log_file, &buff), RF_SUCCESS);
    EXPECT_NOT(RF_FAILURE, rf_string_find(&buff, RFS_("This is a warning"), 0));


    rf_stringx_deinit(&buff);
    rf_textfile_deinit(&log_file);
    return 0;
}
