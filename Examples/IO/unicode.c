#include <String/rf_unicode.h>
#include <string.h>
#include <stdlib.h>
#include <refu.h>

int main()
{
	uint32_t* codepoints,charsN,utf16Length;
	uint16_t* utf16;
	rfInit();//initialize refu library
	const char utf8[] = "I am a utf8 buffer";
/**
 ** [UTF8_DECODE]
 **
 **/
	//decode the utf8 buffer into unicode codepoints
	codepoints = rfUTF8_Decode(utf8,strlen(utf8),&charsN);
/**
 ** [UTF8_DECODE]
 ** [UTF16_ENCODE]
 **
 **/
	//now encode them in UTF-16
	utf16 = rfUTF16_Encode(codepoints,charsN,&utf16Length);
/**
 ** [UTF16_ENCODE]
 ** [OUTPUT]
 **
 **/
	//output the utf-16 encoded buffer into a new file
	FILE* f = fopen("testfile","w");
	fwrite(utf16,2,utf16Length,f);
	fclose(f);
	//finally cleanup the two buffers
	free(codepoints);
	free(utf16);
	return 0;
/**
 ** [OUTPUT]
 **
 **/
}