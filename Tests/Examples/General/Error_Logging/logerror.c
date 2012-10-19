#include <IO/rf_unicode.h>
#include <string.h>

//! [ERRORDEF]
#define NO_MEANING      RF_HIGHEST_ERROR-1
#define ILLEGAL_INPUT   RF_HIGHEST_ERROR-2
//! [ERRORDEF]
//! [ENCODE]
//a function to encode a utf8 buffer into a given encoding
char* encode(const char* utf8,uint32_t length,char encoding)
{
    uint32_t charsN,*codepoints,utf16Length;
    uint16_t* utf16;
	switch(encoding)
	{
	    case RF_UTF8:
            LOG_ERROR("There is no meaning to encoding a utf8 buffer again in utf8",NO_MEANING);
            return 0;
	    break;
	    case RF_UTF16://encode it into utf-16
            codepoints = rfUTF8_Decode(utf8,length,&charsN);
            utf16 = rfUTF16_Encode(codepoints,charsN,&utf16Length);
            free(codepoints);
            return (char*)utf16;
	    break;
	    case RF_UTF32:
            codepoints = rfUTF8_Decode(utf8,length,&charsN);
            return (char*)codepoints;
	    break;
	    default:
            LOG_ERROR("Illegal input value %d for the encoding argument",ILLEGAL_INPUT,encoding);
            return 0;
	    break;
	}
	return 0;
}
//! [ENCODE]

int main()
{
    char* buffer;
	rfInit();//initialize refu library
	const char utf8[] = "A buffer";
//! [CALL]
	//call the function with a no meaning value
    buffer = encode(utf8,strlen(utf8),RF_UTF8);
    //call the function with an illegal value
    buffer = encode(utf8,strlen(utf8),46);
//! [CALL]

//! [LAST_ERROR]
    //just showcasing the RF_GETLASTERROR() macro
    if(RF_GETLASTERROR()!=0)
        printf("The last error value is: [%d]",RF_GETLASTERROR());
//! [LAST_ERROR]
	return 0;
}

