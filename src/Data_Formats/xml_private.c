/**
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** In this source file the private xml functionality is implemented
**/

#include "xml_private.h"

#include <rf_xml.h>
#include <rf_string.h>
#include <IO/rfc_textfile.h>
#include <rf_utils.h>


//! Runs through the xml tree searching for a tag recursively by name and/or contents
//! @param x The XML file handler
//! @param t The currently searching tag
//! @param tName The name of the tag we are searching for
//! @param contents The contents of the tag we are searching for
//! @return Returns the tag if it was found or NULL if not
RF_XMLTag* rfXML_SearchSmall(RF_XML* x,RF_XMLTag* t,void* tName,void* contents)
{
    uint32_t i;
	RF_XMLTag* tt;
    //check if this is the tag we are searching for
    if(rfString_Equal(tName,&t->name)==true)
    {
        char isOkay = true;
        if(contents != 0)
        {
           if(rfString_Equal(contents,&t->contents)!=true)
           {
                isOkay = false;
           }
        }
        if(isOkay)
        {
            //then this is the tag end the search and save how many root children we already searched
            RF_BITFLAG_SET(x->flags,XML_SEARCH_DONE);
            return t;
        }
    }
    //if this is the root make sure we don't search already searched for tags

    //if not check all of the tag's children
    for(i = 0; i< t->children.size; i++)
    {
        if(RF_BITFLAG_ON(x->flags,XML_SEARCH_DONE)==true)
            break;
       if( (  tt = rfXML_SearchSmall(x, rfXMLTag_GetChild(t,i)  ,tName,contents))!= 0 )
        return tt;
    }
    return 0;
}


//! Runs through the xml tree searching for a tag recursively by name ,attributes and contents
//! @param t The currently searching tag
//! @param tName The name of the tag we are searching for
//! @param contents The contents we want the tag to have
//! @param attrN The number of attributes
//! @return Returns the tag if it was found or NULL if not
RF_XMLTag* rfXML_SearchFull(RF_XML* x,RF_XMLTag* t,void* tNameP,void* contentsP,uint32_t attrN,RF_String** sAttribs,RF_String** sAttribValues)
{
    uint32_t i;
    RF_String* tName,*contents;
    tName = (RF_String*)tNameP;
    contents = (RF_String*)contentsP;
    //remember the tag
    x->currentTag = t;

    char match = true;
    if( rfString_Equal(&t->name,tName)==true) //name matches
    {
        //if we have any attributes to match
        for(i = 0; i < attrN; i ++)
        {

            if( rfString_Equal(  ((RF_String*) (rfListP_Get(&t->attributes,i))),sAttribs[i])      == false ||  //attribute name does not match
                rfString_Equal( ((RF_String*)(rfListP_Get(&t->attribValues,i))),sAttribValues[i]) == false  )  //attribute value does not match
            {
                match = false;
                break;
            }
        }
        //if we also need to match contents
        if(contents != 0)
        {
            //for now only search the first line of content, and if it does not match note it
            if(rfString_Equal(&t->contents,contents)==true)
                match = false;
        }
        //if we get here without breaking, we found the tag
        if(match)
        {
            //if this is the tag end the search and save how many root children we already searched
            RF_BITFLAG_SET(x->flags,XML_SEARCH_DONE);
            return t;
        }
    }//end of if name matches
    RF_XMLTag* tt;
    //if not found yet check all of the tag's children
    for(i = 0; i< t->children.size; i++)
    {
        if(RF_BITFLAG_ON(x->flags,XML_SEARCH_DONE)==true)
            break;

        if( (  tt = rfXML_SearchFull(x,rfXMLTag_GetChild(t,i), tName ,contents,attrN,sAttribs,sAttribValues))!= NULL )
            return tt;
    }
    return 0;
}

//! Runs through the tree searching for a tag tName with a child childName somewhere in its children
//! @param x The XML handler
//! @param t The currently searching tag
//! @param tName The name of the tag we are searching for
//! @param childName The name of the child tag we want the tag we search for to have
//! @param direct Boolean flag denoting whether childName neeeds to be direct child (=true) or anywhere in its childre(=false)
//! @param rParent Boolean flag denoting whether we want to return the parent or the child.
//! @return Returns the tag if it was found or NULL if not
RF_XMLTag* i_rfXML_SearchChild(RF_XML* x,RF_XMLTag* t,void* tNameP,void* childNameP,char direct,char rParent)
{
    uint32_t i;
    RF_String* tName,*childName;
    tName = (RF_String*)tNameP;
    childName = (RF_String*)childNameP;
    //remember the tag
    x->currentTag = t;

    //check if this has the name of the tag we search for
    if(rfString_Equal(tName,&t->name) == true)
    {
        RF_XMLTag* tt;
        //recursively check its children to see if childName exists somewhere in there
        if(direct == false)
        {
            if((tt=rfXML_SearchSmall(x,t,childName,0)) != 0)
            {
                //if the child exists end the search
                RF_BITFLAG_SET(x->flags,XML_SEARCH_DONE);
                if(rParent==true)
                    return t;
                else
                    return tt;
            }
            else//child does not exist, continue search
                RF_BITFLAG_UNSET(x->flags,XML_SEARCH_DONE);
        }
        else//just check its direct children to see if childName exists in them
        {
            for(i = 0; i< t->children.size; i++)
            {
                tt = rfXMLTag_GetChild(t,i);
                //if child is found then search is over
                if( rfString_Equal( &tt->name ,childName) ==true )
                {
                    //if the child exists end the search
                    RF_BITFLAG_SET(x->flags,XML_SEARCH_DONE);
                    //return either parent of child depending on the flag
                    if(rParent==true)
                        return t;
                    else
                        return tt;
                }
            }
        }
    }//end of if this is tName

    RF_XMLTag* tt;
    //if not check all of the tag's children to see if tName is there
    for(i = 0; i< t->children.size; i++)
    {
        if(RF_BITFLAG_ON(x->flags,XML_SEARCH_DONE)==true)
            break;

       if( (  tt = i_rfXML_SearchChild(x,rfXMLTag_GetChild(t,i),tName,childName,direct,rParent))!= NULL )
            return tt;

    }
    return 0;
}

//! Runs through the tree recursively printing the tags
//! @param t The XML tag that we recursively print
//! @param f The textfile in which we print
//! @param strBuff An already initialize RF_StringX buffer
//! @param level The level of the tag in the xml tree. Is used for the /t tabs
//! @return Returns true for succesful writting of the @c t XML tag and false otherwise
char i_rfXMLTag_PrintToFile(RF_XMLTag* t,RF_TextFile* f,RF_StringX* strBuff,uint32_t level)
{
    uint32_t i;
    //prepend the required number of tabs
    for(i = 0; i < level; i ++)
        rfTextFile_Write(f,RFS_("\t"));
    //create the tag string
    rfStringX_Assign(strBuff,RFS_("<%s",t->name.bytes));
    for(i = 0; i < t->attributes.size; i ++)
    {
        rfStringX_Append(strBuff,RFS_(" %s=\"%s\"",((RF_String*)rfListP_Get(&t->attributes,i))->bytes,((RF_String*)rfListP_Get(&t->attribValues,i))->bytes));
    }
    rfStringX_Append(strBuff,RFS_(">\n"));
    //print it
    if(rfTextFile_Write(f,strBuff) != RF_SUCCESS)
    {
        LOG_ERROR("Failed to write tag \"<%s>\" to the XML output file",RE_XML_WRITE,t->name.bytes);
        return false;
    }
    //also print the tag's contents(if any)
    if(rfString_Equal(&t->contents,RFS_(""))!= true)
    {
        for(i= 0; i< level; i ++)
            rfTextFile_Write(f,RFS_("\t"));
        rfTextFile_Write(f,RFS_("%s\n",rfString_Cstr(&t->contents)));
    }
    //recursively call this function for all the children of this tag
    for(i=0;i < t->children.size; i++)
    {
        if(i_rfXMLTag_PrintToFile((RF_XMLTag*)rfListP_Get(&t->children,i),f,strBuff,level+1) == false)
        {
            LOG_ERROR("Failed to write tag's \"<%s>\" No. %d child",RE_XML_WRITE,t->name.bytes,i+1)
            return false;
        }
    }
    //when done write the closing of the tag
    //prepend the required number of tabs
    for(i = 0; i < level; i ++)
        rfTextFile_Write(f,RFS_("\t"));
    if(rfTextFile_Write(f,RFS_("</%s>\n",t->name.bytes)) != RF_SUCCESS)
    {
        LOG_ERROR("Failed to write tag's \"<%s>\" closing to the XML output file",RE_XML_WRITE,t->name.bytes);
        return false;
    }
    //success
    return true;
}

//internal full file parsing function, intended to be used only from the start of the file to parse the entirety of the file
int32_t i_rfXML_Parse(RF_XML* x,RF_XMLTag* currentTag)
{
    int32_t parserState,error;
    RF_StringX contents,check;
    ///start parsing the bulk of the xml file
    parserState = TAG_TITLE;
    while(1)
    {
        RF_XMLTag* nTag;
        int32_t opF,clF;
        switch(parserState)
        {
            ///parser is expecting a tag's opening
            case TAG_TITLE:
                //get the title and if no tag name is found in this line
                if(rfStringX_MoveAfterPair(&x->s,RFS_("<"),RFS_(">"),&check,0,0) == false)
                {
                    //if there is nothing else in this line go to the next
                    if(rfString_Equal(&x->s,RFS_(""))== true)
                    {
                        RF_XML_READLINE(x,"Failed to find a tag's opening",&x->s);
                        //back to the start of the loop
                        continue;
                    }
                }//end of if no tag was found in the line

                //if a space is found in the tag name it means that this tag has parameters
                if(rfString_Find(&check,RFS_(" "),0) != RF_FAILURE)
                {
                    char* until = "=\"";
                    RF_StringX attr,attrV;
                    //read the name. Whatever String is contained before space. No need for a failure check here since we know it does have " "
                    rfStringX_MoveAfter(&check,RFS_(" "),&contents,0);
                    //create the new tag with the name
                    nTag = rfXMLTag_Create(currentTag,&contents);
                    //get the attributes as long as we have any
                    while(rfStringX_MoveAfter(&check,RFS_(until),&attr,0) != RF_FAILURE)
                    {
                        until = "\"";
                        //read the values and add them to the tag
                        rfStringX_MoveAfter(&check,RFS_("\""),&attrV,0);
                        rfXMLTag_AddAttribute(nTag,&attr,&attrV);
                        //free the strings
                        rfStringX_Deinit(&attr);rfStringX_Deinit(&attrV);
                    }
                    //free string
                    rfStringX_Deinit(&contents);
                }
                else//just create the new tag
                    nTag = rfXMLTag_Create(currentTag,&check);
                //free one more string
                rfStringX_Deinit(&check);
                //make the new tag current and change state
                currentTag = nTag;
                parserState = TAG_CONTENTS;
            break;
            ///parser is expecting tag contents
            case TAG_CONTENTS:
                //if there is nothing else in this line go to the next
                if(rfString_Equal(&x->s,RFS_(""))==true )
                {
                    RF_XML_READLINE_NO_EOFCHECK(x,"Parsing failed while expecting tag contents",&x->s);
                    //here do a separate eof check
                    if(error == RE_FILE_EOF)
                    {
                        LOG_ERROR("While the parser was parsing the contents of <%s> tag the end of file was found unexpectedly. The expected </%s> closing tag was not found",RE_XML_UNEXPECTED_EOF,
                                 currentTag->name.bytes,currentTag->name.bytes);
                        error = RE_XML_UNEXPECTED_EOF;
                        goto cleanup1;
                    }
                }
                //trim space and tabs from the beginning and find if and where a tag opens and closes
                rfString_TrimStart(&x->s,RFS_(" \t"));
                opF = rfString_Find(&x->s,RFS_("<"),0);
                clF = rfString_Find(&x->s,RFS_("</"),0);
                //if a new tag is not opening right there
                if( opF != 0 && clF != 0)
                {
                    //take everything before the tag opening or new line as content
                    if(rfStringX_MoveAfter(&x->s,RFS_("<"),&check,0) != RF_FAILURE)
                    {
                        rfXMLTag_AddContent(currentTag,&check);
                        //here need to go back by 1 since we need to find '<' in the string in subsequent run
                        rfStringX_MoveBack(&x->s,1);
                        rfStringX_Deinit(&check);
                    }
                    else
                    {
                        rfXMLTag_AddContent(currentTag,&x->s);
                        rfStringX_Assign(&x->s,RFS_(""));
                    }
                }
                else//else depending on the '<' '</' found change the state
                {
                    if(clF != RF_FAILURE && clF <= opF && opF != RF_FAILURE)
                        parserState = TAG_CLOSING;
                    else
                        parserState = TAG_TITLE;
                }
            break;
            ///parser is expecting a tag to close
            case TAG_CLOSING:
                if(rfStringX_MoveAfterPair(&x->s,RFS_("</"),RFS_(">"),&check,0,0) != false)
                {
                    ///success -- if the root tag closed we are done
                    if(currentTag == &x->root)
                    {
                        rfStringX_Deinit(&check);
                        //denote that the whole file is now in memory
                        RF_BITFLAG_SET(x->flags,XML_IN_MEMORY);
                        return RF_SUCCESS;
                    }
                    //if we find a closing tag we don't expect
                    if( rfString_Equal(&currentTag->name,&check) == false)
                    {
                        LOG_ERROR("Expected to find </%s> tag but found </%s>. Aborting XML file parsing.",RE_XML_PARSE_FAILURE,currentTag->name.bytes,rfString_Cstr(&check));
                        rfStringX_Deinit(&check);
                        rfXML_Deinit(x);
                        return RE_XML_PARSE_FAILURE;
                    }
                    //go one tag up
                    currentTag = currentTag->parent;
                    //and go to the start of the loop again
                    parserState = TAG_CONTENTS;
                    rfStringX_Deinit(&check);
                    continue;
                }
                else
                {
                    LOG_ERROR("Expected to find </%s> but did not encounter it. Aborting XML file parsing.",RE_XML_PARSE_FAILURE,currentTag->name.bytes);
                    rfStringX_Deinit(&check);
                    rfXML_Deinit(x);
                    return RE_XML_PARSE_FAILURE;
                }
            break;
        }//parserstate switch closes
    }//end of main parsing loop
    ///if we get here then that is an error
    LOG_ERROR("Corrupt XML file, caused getting out of the parsing loop. XML Parsing failed.",RE_XML_PARSE_FAILURE);
    error = RE_XML_PARSE_FAILURE;
cleanup1:
    //free up memory
    rfXML_Deinit(x);
    return error;;
}

int32_t rfXML_GoNext_dsk(RF_XML* x,RF_XMLTag* t)
{
    int32_t parserState,error,level;
    foff_rft prPos;
    RF_StringX contents,check,copyS;
    RF_XMLTag* startTag = x->currentTag;
    rfTextFile_GetOffset(&x->f,&prPos); /// cleanup1 -- go back to the file offset at function call
    level = 1;
    //make a copy of the buffer string in case we need to restore it
    rfStringX_Copy_IN(&copyS,&x->s); /// cleanup2 -- to reset the string back
    //any call of this function resets the no more children flag
    RF_BITFLAG_UNSET(x->flags,XML_NOCHILDREN);
    ///start parsing
    parserState = TAG_CONTENTS;
    while(1)
    {
        RF_XMLTag* nTag;
        int32_t opF,clF;
        switch(parserState)
        {
            ///parser is expecting a tag's opening
            case TAG_TITLE:
                //get the title and if no tag name is found in this line
                if(rfStringX_MoveAfterPair(&x->s,RFS_("<"),RFS_(">"),&check,0,0) == false)
                {
                    //if there is nothing else in this line go to the next
                    if(rfString_Equal(&x->s,RFS_(""))== true)
                    {
                        RF_XML_READLINE_GOTO(x,"Failed to find a tag's opening",&x->s,error,cleanup2);
                        //back to the start of the loop
                        continue;
                    }
                }//end of if no tag was found in the line
                //tag is found so let's go deeper in the level
                level++;
                //if a space is found in the tag name it means that this tag has parameters
                if(rfString_Find(&check,RFS_(" "),0) != RF_FAILURE)
                {
                    char* until = "=\"";
                    RF_StringX attr,attrV;
                    //read the name. Whatever String is contained before space. No need for a failure check here since we know it does have " "
                    rfStringX_MoveAfter(&check,RFS_(" "),&contents,0);
                    //create the new tag with the name
                    nTag = rfXMLTag_Create(x->currentTag,&contents);

                    //get the attributes as long as we have any
                    while(rfStringX_MoveAfter(&check,RFS_(until),&attr,0) != RF_FAILURE)
                    {
                        until = "\"";
                        //read the values and add them to the tag
                        rfStringX_MoveAfter(&check,RFS_("\""),&attrV,0);
                        rfXMLTag_AddAttribute(nTag,&attr,&attrV);
                        //free the strings
                        rfStringX_Deinit(&attr);rfStringX_Deinit(&attrV);
                    }
                    //free string
                    rfStringX_Deinit(&contents);
                }
                else//just create the new tag
                {
                    nTag = rfXMLTag_Create(x->currentTag,&check);
                }
                //free one more string
                rfStringX_Deinit(&check);
                //make the new tag current and change state
                x->currentTag = nTag;
                parserState = TAG_CONTENTS;
            break;
            ///parser is expecting tag contents
            case TAG_CONTENTS:
                //if there is nothing else in this line go to the next
                if(rfString_Equal(&x->s,RFS_(""))==true )
                {
                    RF_XML_READLINE_GOTO(x,"Parsing failed while expecting tag contents",&x->s,error,cleanup2);
                }
                //trim space and tabs from the beginning and find if and where a tag opens and closes
                rfString_TrimStart(&x->s,RFS_(" \t"));
                opF = rfString_Find(&x->s,RFS_("<"),0);
                clF = rfString_Find(&x->s,RFS_("</"),0);
                //if a new tag is not opening right there
                if( opF != 0 && clF != 0)
                {
                    //take everything before the tag opening or new line as content
                    if(rfStringX_MoveAfter(&x->s,RFS_("<"),&check,0) != RF_FAILURE)
                    {
                        rfXMLTag_AddContent(x->currentTag,&check);
                        //here need to go back by 1 since we need to find '<' in the string in subsequent run
                        rfStringX_MoveBack(&x->s,1);
                        rfStringX_Deinit(&check);
                    }
                    else
                    {
                        rfXMLTag_AddContent(x->currentTag,&x->s);
                        rfStringX_Assign(&x->s,RFS_(""));
                    }
                }
                else//else depending on the '<' '</' found change the state
                {
                    if(clF != RF_FAILURE && clF <= opF && opF != RF_FAILURE)
                        parserState = TAG_CLOSING;
                    else
                        parserState = TAG_TITLE;
                }
            break;
            ///parser is expecting a tag to close
            case TAG_CLOSING:
                if(rfStringX_MoveAfterPair(&x->s,RFS_("</"),RFS_(">"),&check,0,0) != false)
                {
                    level --;
                    //if the root closed then we fail and go back to the root
                    if(rfString_Equal(&check,&x->root.name))
                    {
                        //destroy any children the root might have
                        while(x->root.children.size != 0)
                        {
                            rfXMLTag_RemoveChild(&x->root,0);
                        }
                        error = RE_XML_EOF;
                        goto cleanup3;
                    }
                    //if we find a closing tag we don't expect
                    if( rfString_Equal(&x->currentTag->name,&check) == false)
                    {
                        LOG_ERROR("Expected to find </%s> tag but found </%s>. Aborting XML file parsing.",RE_XML_PARSE_FAILURE,x->currentTag->name.bytes,rfString_Cstr(&check));
                        error = RE_XML_PARSE_FAILURE;
                        goto cleanup3;
                    }
                    ///if the level goes back to 0 or even negative we are done
                    if(level <=0)
                    {
                        ///a special case where the end of the parent is found. Means no more siblings so note it
                        ///and go to the parent's next sibling
                        if(level == -1 && x->lastTag != 0)
                        {
                            if(rfString_Equal(&x->lastTag->parent->name,&check))
                            {
                                RF_XMLTag* toDelete = x->currentTag;
                                x->currentTag = x->currentTag->parent;
                                //if the last tag pointer is a child of the ToDelete tag, zero it
                                RF_XMLTag* checkTag = x->lastTag;
                                while(checkTag->parent != 0)
                                {
                                    if(checkTag->parent == toDelete)
                                    {
                                        x->lastTag =0;
                                        break;
                                    }
                                    checkTag = checkTag->parent;
                                }
                                rfStringX_Deinit(&check);
                                //destroy it by removing it from its own parent and return RF_FAILURE, denoting we could
                                //find no other siblings and that we moved to the next sibling of the parent
                                rfXMLTag_RemoveChild(toDelete->parent,0);
                                x->level--;
                                rfStringX_Deinit(&copyS);
                                return RF_FAILURE;
                            }
                        }
                        ///success
                        //if there is a need to return it
                        if(t!= 0)
                        {
                            rfXMLTag_Copy_IN(t,x->currentTag);
                        }
                        rfStringX_Deinit(&check);
                        //keep this as the last tag
                        x->lastTag = x->currentTag;
                        //go one tag up
                        x->currentTag = x->currentTag->parent;
                        x->level--;
                        //finally delete the copy and success
                        rfStringX_Deinit(&copyS);
                        return RF_SUCCESS;
                    }
                    rfStringX_Deinit(&check);
                    //go one tag up
                    x->currentTag = x->currentTag->parent;
                    //and go to the start of the loop again
                    parserState = TAG_CONTENTS;
                    continue;
                }
                else
                {
                    LOG_ERROR("Expected to find </%s> but did not encounter it. Aborting XML file parsing.",RE_XML_PARSE_FAILURE,x->currentTag->name.bytes);
                    error = RE_XML_PARSE_FAILURE;
                    goto cleanup3;
                }
            break;
        }//parserstate switch closes
    }//end of main parsing loop
    ///if we get here then that is an error
    LOG_ERROR("Corrupt XML file, caused getting out of the parsing loop. XML Parsing failed.",RE_XML_PARSE_FAILURE);
    error = RE_XML_PARSE_FAILURE;

cleanup3: //Clean up the check string
    rfStringX_Deinit(&check);
cleanup2://Get the string back to its state as it was was before function call
    rfStringX_Deinit(&x->s);
    x->s = copyS;
//cleanup1://get the file position back to the previous sibling
    rfTextFile_GoToOffset(&x->f,prPos,SEEK_SET);
    x->currentTag = startTag;
    return error;
}

//Moves to a child under the current tag in the file
int32_t rfXML_GoIn_dsk(RF_XML* x,uint32_t i)
{
    int32_t parserState,error,level=0;
    uint32_t counter = 0;
    foff_rft prPos;
    RF_StringX contents,check,copyS;
    RF_XMLTag* startTag = x->currentTag;
    rfTextFile_GetOffset(&x->f,&prPos);
    //if the no childre flag is raised return failure
    if(RF_BITFLAG_ON(x->flags,XML_NOCHILDREN))
        return RF_FAILURE;
    //make a copy of the buffer string in case we need to restore it
    rfStringX_Copy_IN(&copyS,&x->s);
    ///start parsing
    parserState = TAG_CONTENTS;
    while(1)
    {
        RF_XMLTag* nTag;
        int32_t opF,clF;
        switch(parserState)
        {
            ///parser is expecting a tag's opening
            case TAG_TITLE:
                //get the title and if no tag name is found in this line
                if(rfStringX_MoveAfterPair(&x->s,RFS_("<"),RFS_(">"),&check,0,0) == false)
                {
                    //if there is nothing else in this line go to the next
                    if(rfString_Equal(&x->s,RFS_(""))== true)
                    {
                        RF_XML_READLINE_GOTO(x,"Failed to find a tag's opening",&x->s,error,cleanup3);
                        //back to the start of the loop
                        continue;
                    }
                }//end of if no tag was found in the line
                //keep the line of the opening of the last tag
                x->lastLine = x->f.line-1;//-1 is since ReadLine already moved it to the next line
                //increase indent level
                level ++;
                //if a space is found in the tag name it means that this tag has parameters
                if(rfString_Find(&check,RFS_(" "),0) != RF_FAILURE)
                {
                    char* until = "=\"";
                    RF_StringX attr,attrV;
                    //read the name. Whatever String is contained before space. No need for a failure check here since we know it does have " "
                    rfStringX_MoveAfter(&check,RFS_(" "),&contents,0);
                    //create the new tag with the name
                    nTag = rfXMLTag_Create(x->currentTag,&contents);
                    //get the attributes as long as we have any
                    while(rfStringX_MoveAfter(&check,RFS_(until),&attr,0) != RF_FAILURE)
                    {
                        until = "\"";
                        //read the values and add them to the tag
                        rfStringX_MoveAfter(&check,RFS_("\""),&attrV,0);
                        rfXMLTag_AddAttribute(nTag,&attr,&attrV);
                        //free the strings
                        rfStringX_Deinit(&attr);rfStringX_Deinit(&attrV);
                    }
                    //free string
                    rfStringX_Deinit(&contents);
                }
                else//just create the new tag
                {
                    nTag = rfXMLTag_Create(x->currentTag,&check);
                }
                //free one more string
                rfStringX_Deinit(&check);
                //make the new tag current and change state
                x->currentTag = nTag;
                parserState = TAG_CONTENTS;
                //if it's the child we seek , success
                if(counter == i)
                {
                    x->lastTag = x->currentTag;
                    rfStringX_Deinit(&copyS);
                    x->level++;
                    return RF_SUCCESS;
                }
                else
                {
                    //if it's a child tag at the level we seek increase child counter
                    if(level == 1)
                        counter++;
                }
            break;
            ///parser is expecting tag contents
            case TAG_CONTENTS:
                //if there is nothing else in this line go to the next
                if(rfString_Equal(&x->s,RFS_(""))==true )
                {
                    RF_XML_READLINE_GOTO(x,"Parsing failed while expecting tag contents",&x->s,error,cleanup2);
                }
                //trim space and tabs from the beginning and find if and where a tag opens and closes
                rfString_TrimStart(&x->s,RFS_(" \t"));
                opF = rfString_Find(&x->s,RFS_("<"),0);
                clF = rfString_Find(&x->s,RFS_("</"),0);
                //if a new tag is not opening right there
                if( opF != 0 && clF != 0)
                {
                    ///Will not be parsing content in this function
                    if(rfStringX_MoveAfter(&x->s,RFS_("<"),0,0) != RF_FAILURE)
                        //here need to go back by 1 since we need to find '<' in the string in subsequent run
                        rfStringX_MoveBack(&x->s,1);
                    else
                    {
                        //read a new line
                        rfStringX_Assign(&x->s,RFS_(""));
                    }
                }
                else//else depending on the '<' '</' found change the state
                {
                    if(clF != RF_FAILURE && clF <= opF && opF != RF_FAILURE)
                        parserState = TAG_CLOSING;
                    else
                        parserState = TAG_TITLE;
                }
            break;
            ///parser is expecting a tag to close
            case TAG_CLOSING:
                if(rfStringX_MoveAfterPair(&x->s,RFS_("</"),RFS_(">"),&check,0,0) != false)
                {
                    level --;
                    //if the root closed then we fail and go back to the root
                    if(rfString_Equal(&check,&x->root.name))
                    {
                        startTag = &x->root;//get tag back to root
                        error = RE_XML_EOF;
                        goto cleanup3;
                    }
                    //if we find a closing tag we don't expect
                    if( rfString_Equal(&x->currentTag->name,&check) == false)
                    {
                        LOG_ERROR("Expected to find </%s> tag but found </%s>. Aborting XML file parsing.",RE_XML_PARSE_FAILURE,x->currentTag->name.bytes,rfString_Cstr(&check));
                        error = RE_XML_PARSE_FAILURE;
                        goto cleanup3;
                    }
                    ///if the level goes back to -1 we can find no more children
                    if(level == -1)
                    {
                        //finally raise the no children flag and return failure
                        RF_BITFLAG_SET(x->flags,XML_NOCHILDREN);
                        error = RF_FAILURE;
                        goto cleanup3;
                    }
                    //go one tag up
                    x->currentTag = x->currentTag->parent;
                    //and go to the start of the loop again
                    parserState = TAG_CONTENTS;
                    continue;
                }
                else
                {
                    LOG_ERROR("Expected to find </%s> but did not encounter it. Aborting XML file parsing.",RE_XML_PARSE_FAILURE,x->currentTag->name.bytes);
                    error = RE_XML_PARSE_FAILURE;
                    goto cleanup3;
                }
            break;
        }//parserstate switch closes
    }//end of main parsing loop
    ///if we get here then that is an error
    LOG_ERROR("Corrupt XML file, caused getting out of the parsing loop. XML Parsing failed.",RE_XML_PARSE_FAILURE);
    error = RE_XML_PARSE_FAILURE;

cleanup3://Deinitialize the check string
    rfStringX_Deinit(&check);
cleanup2://Get the buffer string back to where it was before
    rfStringX_Deinit(&x->s);
    x->s = copyS;
//cleanup1://get the file position back tothe parent
    x->currentTag = startTag;
    rfTextFile_GoToOffset(&x->f,prPos,SEEK_SET);
    return error;
}

// Moves to the next sibling of the parent tag of the current tag in the file
int32_t rfXML_GoOut_dsk(RF_XML* x,char after)
{
    int32_t error;
    foff_rft prPos;
    RF_StringX copyS;
    rfTextFile_GetOffset(&x->f,&prPos);
    //if it's the root then the function fails
    if(x->currentTag == &x->root )
    {
        return RF_FAILURE;
    }
    if(x->currentTag->parent == &x->root)
    {//if it's a direct root child then the function gets you to the very beginning of the file, to the root
        x->lastTag = x->currentTag = &x->root;
        rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET);
        x->lastLine = x->f.line-1;
        rfStringX_Assign(&x->s,RFS_(""));
        x->level = 0;
        return RF_SUCCESS;
    }
    //make a copy of the string at function call
    rfStringX_Copy_IN(&copyS,&x->s);
    //lower the no children flag
    RF_BITFLAG_UNSET(x->flags,XML_NOCHILDREN);
    ///before case
    if(after == false)
        while(1)
        {
            //if the tag's parent opening tag is found in this line
            if(rfString_Find(&x->s,RFS_("<%s",x->currentTag->parent->name.bytes))!= RF_FAILURE)
            {

                x->currentTag = x->currentTag->parent;
                x->lastTag = x->currentTag;
                x->lastLine = x->f.line-1;
                //free the copy and succes
                rfStringX_Deinit(&copyS);
                return RF_SUCCESS;
            }
            else
            {//else move a line back
                if( (error = rfTextFile_MoveLines(&x->f,-2) != RF_SUCCESS))
                {
                    if(error == RF_FAILURE)
                        LOG_ERROR("While moving backwards in the XML file \"%s\" could not find the expected opening of tag <%s>",RE_XML_PARSE_FAILURE,x->f.name.bytes,x->currentTag->parent->name.bytes);
                    //else
                    LOG_ERROR("Could not traverse the XML file \"%s\" backwards",RE_XML_PARSE_FAILURE,x->f.name.bytes);
                    error = RE_XML_PARSE_FAILURE;
                    goto cleanup1;
                }
                rfTextFile_ReadLine2(&x->f,&x->s);
            }
        }
    else
    {///after case
        while(1)
        {
            //if the tag's parent closing tag is found in this line
            if(rfString_Find(&x->s,RFS_("</%s>",x->currentTag->parent->name.bytes))!= RF_FAILURE)
            {
                x->currentTag = x->currentTag->parent;
                x->lastTag = x->currentTag;
                x->lastLine = x->f.line-1;
                rfStringX_MoveAfter(&x->s,RFS_("</%s>",x->currentTag->name.bytes),0,0);
                //free the copy and success
                rfStringX_Deinit(&copyS);
                x->level--;
                return RF_SUCCESS;
            }
            else
            {
                if( (error = rfTextFile_ReadLine(&x->f,&x->s)) != RF_SUCCESS)
                {//if we got a file reading error
                    if(error == RE_FILE_EOF)
                        error = RE_XML_UNEXPECTED_EOF;
                    else
                        error = RE_XML_PARSE_FAILURE;
                    goto cleanup1;
                }
            }
        }//end of while loop
    }//end of the after case

cleanup1:
    rfTextFile_GoToOffset(&x->f,prPos,SEEK_SET);
    rfStringX_Deinit(&x->s);
    x->s = copyS;
    return error;
}
