#include <RFstring.h>
#include <rf_stdio.h>
//! [STRINGS]
//These are some strings, used for the purpose of the example. Suppose they came by reading an external file
static const char* strings[] =
{
    "One string",
    "Another String",
    "Yet another string",
    "One final string"
}
//! [STRINGS]

//! [PARSE_LINE]
//This is a function that parses the lines of the file and
//returns them with the line number appended.
char parseLine(RF_StringX* str,uint32_t line)
{
    //read in the string from the "file" and put that along with its line into the given StringX
    rfStringX_Assign(str,RFS_("\"%s\" line %u", strings[line],line));
	//print the parsed string
	rfPrintf("%S\n",str);
}
//! [PARSE_LINE]
//! [PARSE_LINE_BAD]
char parseLine(RF_String* str,uint32_t line)
{
    //read in the string from the "file" and initialize a new String with its content and the added line
	rfString_Init(str,"\"%s\" line %u", strings[line],line);
	//print the parsed string
	rfPrintf("%S\n",str);
	//gotta deinit or else memory leak will happen
	rfString_Deinit(str);//deallocation
}
//! [PARSE_LINE_BAD]

int main()
{
    int i;
	rfInit();//init the library
//! [INIT_STRINGX]
    //Remember: StringX is a buffer
    RF_StringX buff;
    rfStringX_Init_buff(&buff,256,"");
//! [INIT_STRINGX]
    //Parse all 4 lines of the "file"
    for(i=0; i < 4; i++)
        parseLine(&buff,i);
//! [FREE]
    //finally free the buffer StringX
    rfStringX_Deinit(&buff);
//! [FREE]
    return 0;
}
