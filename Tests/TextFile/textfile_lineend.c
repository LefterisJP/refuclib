#include <RFtextfile.h>
#include <RFstring.h>
#include <refu.h>
#include <RFprintf.h>
#include "../refu_tests.h"

int main()
{
    RF_TextFile f;

    EXPECT(rfInit(), true);
	//attempt auto detection of line ending for an LF unix style UTF-8 text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf8_LF.forcebin"),
               RF_FILE_READ,
               RF_ENDIANESS_UNKNOWN,
               RF_UTF8,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_LF);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a CR style UTF-8 text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf8_CR.forcebin"),
               RF_FILE_READ,
               RF_ENDIANESS_UNKNOWN,
               RF_UTF8,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_CR);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a windows style CRLF UTF-8 text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf8_CRLF.forcebin"),
               RF_FILE_READ,
               RF_ENDIANESS_UNKNOWN,
               RF_UTF8,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_CRLF);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a CR Macintosh style UTF-16 big endian text file
    EXPECT(true,
           rfTextFile_Init(&f,
                           RFS_("utf16le_CR.forcebin"),
                           RF_FILE_READ,
                           RF_LITTLE_ENDIAN,
                           RF_UTF16,
                           RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_CR);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a CRLF Windows style UTF-16 little endian text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf16be_CRLF.forcebin"),
               RF_FILE_READ,
               RF_BIG_ENDIAN,
               RF_UTF16,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_CRLF);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a CR Macintosh style UTF-16 big endian text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf16le_LF.forcebin"),
               RF_FILE_READ,
               RF_LITTLE_ENDIAN,
               RF_UTF16,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_LF);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a LFstyle UTF-32 Little Endian text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf32be_LF.forcebin"),
               RF_FILE_READ,
               RF_BIG_ENDIAN,
               RF_UTF32,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_LF);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a CR-LF windows style UTF-32 Big Endian text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf32le_CRLF.forcebin"),
               RF_FILE_READ,
               RF_LITTLE_ENDIAN,
               RF_UTF32,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_CRLF);
    rfTextFile_Deinit(&f);
	//attempt auto detection of line ending for a CR style UTF-32 Little Endian text file
    EXPECT(true,
           rfTextFile_Init(
               &f,
               RFS_("utf32be_CR.forcebin"),
               RF_FILE_READ,
               RF_BIG_ENDIAN,
               RF_UTF32,
               RF_EOL_AUTO));
    EXPECT(f.eol,RF_EOL_CR);
	rfTextFile_Deinit(&f);

	return 0;
}
