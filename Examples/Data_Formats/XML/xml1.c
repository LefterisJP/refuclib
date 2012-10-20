#include <rf_xml.h>

int main()
{
    RF_XML xml;
    RF_XMLTag temp;
    //init the library
    rfInit("errorLog","infoLog");

//! [PARSE]
    //initialize and parse the file "xmltest"
    if(rfXML_Init(&xml,RFS_("xmltest"),XML_OPEN_IN_MEMORY,RF_UTF8)!=RF_SUCCESS)
       return -1;
//! [PARSE]
//! [TAG_SEARCH]
    //let's get a pointer to the tag with name book and "bk106" id attribute
    RF_XMLTag* t1 = rfXML_GetTag(&xml,RFS_("book"),0,1,RFS_("id"),RFS_("bk106"));
    if(t1 == 0)
        return -2;
    RF_XMLTag* t2 = rfXML_GetTag(&xml,RFS_("author"));
    if(t2 == 0)
        return -2;
//! [TAG_SEARCH]
//! [TAG_ADD]
    //add a new tag to the xml
    //method 1
    rfXMLTag_AddTag(t1,RFXML_(0,RFS_("volume"),RFS_("Special Edition"),2,
                                           RFS_("Edition"),RFS_("1"),
                                           RFS_("Revision date"),RFS_("23/10/2008")));
    //method 2
    rfXMLTag_Init(&temp,t2,RFS_("from"),0,1,RFS_("country"),RFS_("Greece"));
    //note that with method 2, temp should not be freed since it has been put inside the xml file in memory
//! [TAG_ADD]

//! [XML_OUT]
    if(rfXML_ToFile(&xml,RFS_("testfile")) != RF_SUCCESS)
        return -3;
//! [XML_OUT]
//! [CLEANUP]
    //recursively destroy the handler and all its tags
    rfXML_Deinit(&xml);
//! [CLEANUP]

	return 0;
}