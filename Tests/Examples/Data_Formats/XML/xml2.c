#include <rf_xml.h>

int main()
{
    RF_XML xml;
    RF_StringX s;
    //init the library
    rfInit("errorLog","infoLog");

//! [INIT]
    //initialize the "xmltest" file in the disk
    if(rfXML_Init(&xml,RFS_("xmltest"),XML_OPEN_IN_DISK,RF_UTF8)!=RF_SUCCESS)
       return -1;
//! [INIT]

//! [TAG_SEARCH]
//search for a specific tag in the file
RF_XMLTag* t1 = rfXML_GetTag(&xml,RFS_("genre"),RFS_("Fantasy"));
//! [TAG_SEARCH]
//! [TAG_PRINT]
if(t1 == 0)
    return -2;
//turn the Tag t1 into a string and print it
rfXMLTag_ToStr(t1,&s);
printf("%s",s.bytes);
//! [TAG_PRINT]
//! [TAG_ADD]
//insert a new tag into the file as a child of the last tag we got (t1)
rfXML_InsertTag(&xml,RFXML_(0,RFS_("subgenre"),RFS_("A Fantasy subgenre"),1,RFS_("name"),RFS_("steampunk")),XML_TAG_END);
//! [TAG_ADD]
//! [CLEANUP]
//finally clean up the XML handler and the temporary string
rfXML_Deinit(&xml);
rfStringX_Deinit(&s);
//! [CLEANUP]

    return 0;
}