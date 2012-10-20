#include <rf_xml.h>

int main()
{
    RF_XML xml;
    RF_XMLTag* tag,*tag2;// tag pointers to be used in the example
    //init the library
    rfInit("errorLog","infoLog");
 //! [INIT]
    //initialize the "xmltest" file in memory
    if(rfXML_Init(&xml,RFS_("newxmlfile"),XML_NEW_IN_MEMORY|XML_SPACES,RF_UTF8)!=RF_SUCCESS)
       return -1;
//! [INIT]
//! [TAG_INSERTION]
//! [ADD_ROOT]
    //create the root
    rfXML_AddRoot(&xml,RFXML_(0,RFS_("music")));
//! [ADD_ROOT]
//! [ADD_ROOT_CHILD]
    //init a tag as child of the root
    tag = rfXMLTag_Create(rfXML_GetRoot(&xml),RFS_("song"),0,1,RFS_("title"),RFS_("Time Is Running Out"));
    //add tags as children of the root
//! [TEMP_TAGS_EXAMPLE]
    rfXMLTag_AddTag(tag,RFXML_(0,RFS_("artist"),RFS_("Muse")));
    rfXMLTag_AddTag(tag,RFXML_(0,RFS_("genre"),RFS_("Alternative Rock")));
    rfXMLTag_AddTag(tag,RFXML_(0,RFS_("album"),RFS_("Absolution")));
//! [TEMP_TAGS_EXAMPLE]
//! [ADD_ROOT_CHILD]
    //init another tag as child of the root and add children to it
    tag = rfXMLTag_Create(rfXML_GetRoot(&xml),RFS_("song"),0,1,RFS_("title"),RFS_("The Unforgiven II"));
    rfXMLTag_AddTag(tag,RFXML_(0,RFS_("artist"),RFS_("Metallica")));
//! [SPECIFIC_TAG]
    tag2 = rfXMLTag_Create(tag,RFS_("genre"),RFS_("Metal"));//not ice that we keep a pointr to this tag
    rfXMLTag_AddTag(tag,RFXML_(0,RFS_("album"),RFS_("Reload")));
    //add another tag as a child of a specific tag
    rfXMLTag_AddTag(tag2,RFXML_(0,RFS_("subgenre"),0,1,RFS_("name"),RFS_("Melodic Metal")));
//! [SPECIFIC_TAG]
//! [TAG_INSERTION]
//! [TAG_OUT]
    //print the tag to the file an dcleanup the xml handler
    rfXML_ToFile(&xml);
    rfXML_Deinit(&xml);
//! [TAG_OUT]

    return 0;
}