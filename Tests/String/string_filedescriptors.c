#include <RFstring.h>
#include <rf_system.h>
#include <stdio.h>


#include "../refu_tests.h"
int main()
{
	RF_String s1,s2,s3,s4,s5,s6,s7,s8,s9,s10;
	RF_StringX sx1,sx2,sx3,sx4,sx5,sx6,sx7,sx8,sx9,sx10;
    FILE* futf8,*futf16,*futf32,*f;
    char eof;
	rfInit();
	
/// --> UTF8

    //this is not the normal way to open a unicode file but since
    //the unicode file descriptor functions need to be tested here separately from the TextFile module
    //we are not using RF_TextFile. Normally you would use that to handle
    //things like BOM in the start of the file
	EXPECTNOT(0,(futf8=fopen("utf8stringfile","rb")))
    //expect to see succesfull initialization
	EXPECTGE(0,rfString_Init_fUTF8(&s1,futf8,&eof,RF_EOL_LF))
	
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&s1,1))
    printf("%s",rfString_Cstr(&s1));//ending newline is included since we don't use RF_TextFile
    
    //assign the next line to the same string
    EXPECTGE(0,rfString_Assign_fUTF8(&s1,futf8,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfString_Append_fUTF8(&s1,futf8,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true")
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&s1,RFS_("\n"),0,0))
    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&s1,f,RF_UTF8))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfString_Init_fUTF8(&s2,f,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&s1,&s2))
	
	///let's do the same for RF_StringX
	fclose(f);
	EXPECT(0,fseek(futf8,0,SEEK_SET))
	
	EXPECTGE(0,rfStringX_Init_fUTF8(&sx1,futf8,&eof,RF_EOL_LF))
	EXPECT(true,rfString_PruneStart(&sx1,1))
	printf("%s",rfString_Cstr(&sx1));//ending newline is included since we don't use RF_TextFile
	EXPECTGE(0,rfStringX_Assign_fUTF8(&sx1,futf8,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfStringX_Append_fUTF8(&sx1,futf8,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true")
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&sx1,RFS_("\n"),0,0))
    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&sx1,f,RF_UTF8))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfStringX_Init_fUTF8(&sx2,f,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&sx1,&sx2))
	
    fclose(futf8);
    fclose(f);
/// --> UTF16LE

    EXPECTNOT(0,(futf16 = fopen("utf16lestringfile","rb")))

    //expect to see succesfull initialization
	EXPECTGE(0,rfString_Init_fUTF16(&s3,futf16,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&s3,1));
    printf("%s",rfString_Cstr(&s3));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfString_Assign_fUTF16(&s3,futf16,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string and check for EOF
    EXPECTGE(0,rfString_Append_fUTF16(&s3,futf16,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&s3,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&s3,f,RF_UTF16_LE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfString_Init_fUTF16(&s4,f,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&s3,&s4));
	
	///do the same for RF_StringX
	EXPECT(0,fseek(futf16,0,SEEK_SET))
	fclose(f);
	//expect to see succesfull initialization
	EXPECTGE(0,rfStringX_Init_fUTF16(&sx3,futf16,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&sx3,1));
    printf("%s",rfString_Cstr(&sx3));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfStringX_Assign_fUTF16(&sx3,futf16,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string and check for EOF
    EXPECTGE(0,rfStringX_Append_fUTF16(&sx3,futf16,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&sx3,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&sx3,f,RF_UTF16_LE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfStringX_Init_fUTF16(&sx4,f,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&sx3,&sx4));
	
    fclose(futf16);
    fclose(f);

/// --> UTF16BE
    EXPECTNOT(0,(futf16 = fopen("utf16bestringfile","rb")))

    //expect to see succesfull initialization
	EXPECTGE(0,rfString_Init_fUTF16(&s5,futf16,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&s5,1))
    printf("%s",rfString_Cstr(&s5));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfString_Assign_fUTF16(&s5,futf16,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfString_Append_fUTF16(&s5,futf16,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&s5,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&s5,f,RF_UTF16_BE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfString_Init_fUTF16(&s6,f,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&s5,&s6))
	
	///do the same for RF_StringX
	fclose(f);
	EXPECT(0,fseek(futf16,0,SEEK_SET))
	//expect to see succesfull initialization
	EXPECTGE(0,rfStringX_Init_fUTF16(&sx5,futf16,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&sx5,1))
    printf("%s",rfString_Cstr(&sx5));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfStringX_Assign_fUTF16(&sx5,futf16,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfStringX_Append_fUTF16(&sx5,futf16,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&sx5,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&s5,f,RF_UTF16_BE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfStringX_Init_fUTF16(&sx6,f,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&sx5,&sx6))
	
    fclose(futf16);
    fclose(f);

/// --> UTF32LE

    EXPECTNOT(0,(futf32 = fopen("utf32lestringfile","rb")))

    //expect to see succesfull initialization
	EXPECTGE(0,rfString_Init_fUTF32(&s7,futf32,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&s7,1))
    printf("%s",rfString_Cstr(&s7));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfString_Assign_fUTF32(&s7,futf32,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfString_Append_fUTF32(&s7,futf32,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&s7,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&s7,f,RF_UTF32_LE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfString_Init_fUTF32(&s8,f,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&s7,&s8))
	
	///for RF_StringX
    EXPECT(0,fseek(futf32,0,SEEK_SET))
	fclose(f);
	//expect to see succesfull initialization
	EXPECTGE(0,rfStringX_Init_fUTF32(&sx7,futf32,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&sx7,1))
    printf("%s",rfString_Cstr(&sx7));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfStringX_Assign_fUTF32(&sx7,futf32,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfStringX_Append_fUTF32(&sx7,futf32,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&sx7,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&sx7,f,RF_UTF32_LE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfStringX_Init_fUTF32(&sx8,f,RF_LITTLE_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&sx7,&sx8))
	
    fclose(futf32);
    fclose(f);

/// --> UTF32BE

    EXPECTNOT(0,(futf32 = fopen("utf32bestringfile","rb")))

    //expect to see succesfull initialization
	EXPECTGE(0,rfString_Init_fUTF32(&s9,futf32,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&s9,1))
    printf("%s",rfString_Cstr(&s9));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfString_Assign_fUTF32(&s9,futf32,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfString_Append_fUTF32(&s9,futf32,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&s9,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&s9,f,RF_UTF32_BE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfString_Init_fUTF32(&s10,f,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&s9,&s10))
    
	
	///for RF_StringX
	EXPECT(0,fseek(futf32,0,SEEK_SET))
	fclose(f);
    //expect to see succesfull initialization
	EXPECTGE(0,rfStringX_Init_fUTF32(&sx9,futf32,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //expect to see the first line of the file (as mentioned in the first comment let's just get rid of the bom here)
    EXPECT(true,rfString_PruneStart(&sx9,1))
    printf("%s",rfString_Cstr(&sx9));//ending newline is included since we don't use RF_TextFile
    //assign the next line to the same string
    EXPECTGE(0,rfStringX_Assign_fUTF32(&sx9,futf32,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    //append the next line to the same string
    EXPECTGE(0,rfStringX_Append_fUTF32(&sx9,futf32,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
	EXPECT_MSG(true,eof,"The end of file should have been reached and this flag should have been true");
	//remove the appended newlines since we want to read it one-off from the other file
    EXPECT(true,rfString_Remove(&sx9,RFS_("\n"),0,0))

    //open an output file and write the string there
    EXPECTNOT(0,(f = fopen("outputfile","w+b")))
    EXPECT(RF_SUCCESS,rfString_Fwrite(&sx9,f,RF_UTF32_BE))
    EXPECT(0,fseek(f,0,SEEK_SET))

    //read from that same file and compare
    EXPECTGE(0,rfStringX_Init_fUTF32(&sx10,f,RF_BIG_ENDIAN,&eof,RF_EOL_LF))
    EXPECT(true,rfString_Equal(&sx9,&sx10))
	
	fclose(futf32);
    fclose(f);

    rfDeleteFile(RFS_("outputfile"));

	return 0;
}
