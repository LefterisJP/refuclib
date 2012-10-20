#include <rf_xml.h>

int main()
{
    RF_XML xml;
    //init the library
    rfInit("errorLog","infoLog");
 //! [INIT]
    //initialize the "xmltest" file in the disk
    if(rfXML_Init(&xml,RFS_("newxmlfile"),XML_NEW_IN_DISK,RF_UTF8)!=RF_SUCCESS)
       return -1;
//! [INIT]
//! [TAG_INSERTION]
    //insert one tag and its subtags
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("music")),0);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("song"),0,1,RFS_("title"),RFS_("Time Is Running Out")),XML_TAG_END|XML_GO);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("artist"),RFS_("Muse")),XML_TAG_END);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("genre"),RFS_("Alternative Rock")),XML_TAG_END);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("album"),RFS_("Absolution")),XML_TAG_END);
//! [GOTO_ROOT]
	//go back to root
    if(rfXML_GoToTag(&xml,RFS_("music")) != RF_SUCCESS)
        return -1; //failure to find the root
//! [GOTO_ROOT]
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("song"),0,1,RFS_("title"),RFS_("The Unforgiven II")),XML_TAG_END|XML_GO);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("artist"),RFS_("Metallica")),XML_TAG_END);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("genre"),RFS_("Metal")),XML_TAG_END);
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("album"),RFS_("Reload")),XML_TAG_END);
//! [GOTO_TAG]
    if(rfXML_GoToTag(&xml,RFS_("genre"),RFS_("Metal"))!= RF_SUCCESS)
        return -2;
    rfXML_InsertTag(&xml,RFXML_(0,RFS_("subgenre"),0,1,RFS_("name"),RFS_("Melodic Metal")),XML_TAG_END);
//! [GOTO_TAG]
//! [TAG_INSERTION]
//! [CLEANUP]
    //cleanup the XML handler
    rfXML_Deinit(&xml);
//! [CLEANUP]

    return 0;
}