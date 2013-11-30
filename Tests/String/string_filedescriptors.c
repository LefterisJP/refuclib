#include <RFstring.h>
#include <RFsystem.h>
#include <refu.h>

#include "../refu_tests.h"

static void test_file_encoding(const char* filename, int encoding,
                       int endianess)
{
    FILE* inF, *f;
    RF_String s1,s2;
    RF_StringX sx1,sx2;
    char eof;

    //things like BOM in the start of the file
    EXPECT_NOT(0, inF=fopen(filename, "rb"));
    //expect to see succesfull initialization
    EXPECTGE(rfString_FInit(&s1, inF, &eof, RF_EOL_LF,
                            encoding, endianess, NULL),
             0);
	
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&s1,1));
    EXPECTGE(printf(RF_STR_PF_FMT, RF_STR_PF_ARG(&s1)), 0);//ending newline is included since we don't use RF_TextFile, RF_SUCCESS)
    //assign the next line to the same string
    EXPECTGE(rfString_FAssign(&s1, inF, &eof, RF_EOL_LF,
                              encoding, endianess),
             0);
    //append the next line to the same string
    EXPECTGE(rfString_FAppend(&s1, inF, &eof, RF_EOL_LF,
                              encoding, endianess),
             0);
    EXPECT_MSG(true,
               eof,
               "The end of file should have been reached and this flag "
               "should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true, rfString_Remove(&s1,RFS_("\n"),0,0));
  //open an output file and write the string there
    EXPECT_NOT(0, f = fopen("outputfile","w+b"));
    EXPECT(true,rfString_Fwrite(&s1, f, encoding, endianess));
    EXPECT(0,fseek(f,0,SEEK_SET));

  //read from that same file and compare
    EXPECTGE(rfString_FInit(&s2, f, &eof, RF_EOL_LF,
                            encoding, endianess, NULL),
             0);
    EXPECT(true,rfString_Equal(&s1,&s2));
	
	///let's do the same for RF_StringX
    fclose(f);
    EXPECT(0, fseek(inF, 0, SEEK_SET));
	
    EXPECTGE(rfStringX_FInit(&sx1, inF, &eof, RF_EOL_LF,
                             encoding, endianess),
             0);
    EXPECT(true,rfString_PruneStart(&sx1,1));
    EXPECTGE(printf(RF_STR_PF_FMT, RF_STR_PF_ARG(&sx1)), 0);//ending newline is included since we don't use RF_TextFile, RF_SUCCESS)
    EXPECTGE(rfStringX_FAssign(&sx1, inF, &eof, RF_EOL_LF,
                               encoding, endianess),
             0);
    //append the next line to the same string
    EXPECTGE(rfStringX_FAppend(&sx1, inF, &eof, RF_EOL_LF,
                               encoding, endianess),
             0);
    EXPECT_MSG(true,
               eof,
               "The end of file should have been reached and this flag "
               "should have been true");
	  //remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&sx1, RFS_("\n"), 0, 0));
    //open an output file and write the string there
    EXPECT_NOT(0,(f = fopen("outputfile","w+b")));
    EXPECT(true,rfString_Fwrite(&sx1, f, encoding, endianess));
    EXPECT(0, fseek(f,0,SEEK_SET));

    //read from that same file and compare
    EXPECTGE(rfStringX_FInit(&sx2, f, &eof, RF_EOL_LF,
                             encoding, endianess),
             0);
    EXPECT(true,rfString_Equal(&sx1, &sx2));
	
    fclose(inF);
    fclose(f);

}


int main()
{
    DEFAULT_LIB_INIT();

    //this is not the normal way to open a unicode file but since
    //the unicode file descriptor functions need to be tested here separately from the TextFile module
    //we are not using RF_TextFile. Normally you would use that to handle	

    test_file_encoding("utf8stringfile", RF_UTF8, RF_ENDIANESS_UNKNOWN);
    test_file_encoding("utf16lestringfile", RF_UTF16, RF_LITTLE_ENDIAN);
    test_file_encoding("utf16bestringfile", RF_UTF16, RF_BIG_ENDIAN);
    test_file_encoding("utf32bestringfile", RF_UTF32, RF_BIG_ENDIAN);
    test_file_encoding("utf32lestringfile", RF_UTF32, RF_LITTLE_ENDIAN);

    
    EXPECT(true, rfDeleteFile(RFS_("outputfile")));

	return 0;
}
