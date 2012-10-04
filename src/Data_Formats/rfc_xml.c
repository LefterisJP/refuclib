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
**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> //for malloc




#include <Data_Formats/rfc_xml.h>
#include <rf_system.h>
#include <rf_utils.h> //for bitflags manipulations
#include <rf_setup.h>
#include "xml_private.h" //for the private xml functionality
#include <rf_io.h>
#include <String/rfc_stringx.h>


//! Defines the maximul buffer size for string read from a file
#define MAX_LINE_BUFFER_SIZE    128000

// Allocates and returns a new XML file handler
RF_XML* i_rfXML_Create(void* filename,char* openFlagP,char* encodingP)
{
    char encoding = *encodingP;
    char openFlag = *openFlagP;
    RF_XML* ret;
    RF_MALLOC(ret,sizeof (RF_XML));
    //initialize it
    if(rfXML_Init(ret,filename,openFlag,encoding)!= RF_SUCCESS)
    {
        free(ret);
        return 0;
    }
    //else success
    return ret;
}

// Initialize a new XML file
int32_t i_rfXML_Init(RF_XML* ret,void* filenameP,char* openFlagP,char* encodingP)
{
    int32_t error;
    char encoding,openFlag;
    RF_String* filename;
    RF_XMLTag* currentTag;
    RF_StringX copyS;
    encoding =  *encodingP;
    openFlag = *openFlagP;
    filename = (RF_String*)filenameP;
    // A pointer to the tag being currently parsed
    currentTag = 0;
    //initialize some data
    ret->version = 0.0;
    ret->flags=0;
    ret->rcs=0;
    ret->rootChildrenSearched = 0;
    ret->rootPos = 0;
    ret->currentTag = 0;
    ret->lastTag = 0;
    ret->lastLine = 0;
    ret->level = 0;
    //initialize the text buffer reading string
    rfStringX_Init_txtbuff(&ret->s,""); ///cleanup1 - clean this string
    //Some Strings that will be used during parsing
    RF_StringX check;
    ///depending on the type of opening the user requests
    if(RF_BITFLAG_ON(openFlag,XML_OPEN_IN_MEMORY) || RF_BITFLAG_ON(openFlag,XML_OPEN_IN_DISK))//existing file opening case
    {
        foff_rft prOff;
        if(RF_BITFLAG_ON(openFlag,XML_OPEN_IN_MEMORY))
            RF_BITFLAG_SET(ret->flags,XML_IN_MEMORY);

        if(rfTextFile_Init(&ret->f,filename,RF_FILE_READ,encoding) != RF_SUCCESS)
        {
            LOG_ERROR("XML File \"%s\" could not be opened, aborted object creation",RE_XML_OPEN_FAILURE,filename->bytes);
            error = RE_XML_OPEN_FAILURE;
            goto cleanup1;
        }
        ///read until we find the xml tag
        while(1)
        {
            //read line
            RF_XML_READLINE_GOTO(ret,"The XML opening tag could not be found",&ret->s,error,cleanup2)
            //if we don't find the xml tag in this line continue
            if(rfString_Find(&ret->s,RFS_("<?xml "),0) == RF_FAILURE)
            //no opening tag found continue to the next line
                continue;
            //if we get here means we found the tag so let's read the version
            if(rfString_ScanfAfter(&ret->s,RFS_("version=\""),"%f",&ret->version)==false)
            {
                LOG_ERROR("Error while parsing the XML file, no version attribute found in the header. Quitting parsing ...",RE_XML_PARSE_FAILURE);
                error = RE_XML_PARSE_FAILURE;
                goto cleanup2;
            }
            //with the version read break
            break;
        }
        ///read until root tag is found or EOF is reached
        while(1)
        {
            RF_XML_READLINE_GOTO(ret,"The root tag of the XML file could not be found",&ret->s,error,cleanup2);
            //get the tag name
            if( rfStringX_MoveAfterPair(&ret->s,RFS_("<"),RFS_(">"),&check,0,0) == true)
            {
                if((error = rfTextFile_GetOffset(&ret->f,&ret->rootPos)) != RF_SUCCESS)
                {
                    rfStringX_Deinit(&check);
                    goto cleanup2;
                }
                RF_BITFLAG_SET(ret->flags,XML_ROOT_INIT);
                //save it and break from the loop to go on building the xml tree
                rfXMLTag_Init(&ret->root,0,&check);
                currentTag = &ret->root;
                rfStringX_Deinit(&check);
                break;
            }
        }
        //perform a small check to see if there are tabs in the file or not
        rfStringX_Copy_IN(&copyS,&ret->s);
        if((error=rfTextFile_GetOffset(&ret->f,&prOff))!= RF_SUCCESS)
        {
            goto cleanup3;
        }
        rfTextFile_ReadLine2(&ret->f,&ret->s);
        if(rfString_Find(&ret->s,RFS_("\t"),0) == RF_FAILURE)
        {
            RF_BITFLAG_SET(ret->flags,XML_SPACES);
        }
        //now go back
        rfTextFile_GoToOffset(&ret->f,prOff,SEEK_SET);
        rfStringX_Deinit(&ret->s);
        ret->s = copyS;


        if(openFlag == XML_OPEN_IN_MEMORY)
        {
            ///Here there is a situation that we have to consider. The root tag might be without any contents and hence just closing right after being opened
            ///so we check for this situation here before proceeding
            if(rfString_Between(&ret->s,RFS_("</"),RFS_(">"),&check,0) == true)
            {
                //if the root tag closed we are done
                if( (currentTag == &ret->root) && rfString_Equal(&check,&ret->root.name))
                {
                    rfStringX_Deinit(&check);
                    //denote that the whole file is now in memory
                    RF_BITFLAG_SET(ret->flags,XML_IN_MEMORY);
                    return RF_SUCCESS;
                }//else just free the check string
                rfStringX_Deinit(&check);
            }
            //now parse the rest of the file and put it in memory if needed
            return i_rfXML_Parse(ret,currentTag);
        }//else we just opened the file in disk so success
        return RF_SUCCESS;

    }//end of existing file opening case
    else if(RF_BITFLAG_ON(openFlag,XML_NEW_IN_DISK) || RF_BITFLAG_ON(openFlag,XML_NEW_IN_MEMORY))///new file case
    {
        ret->version = 1;
        if(rfTextFile_Init(&ret->f,filename,RF_FILE_READWRITE_NEW,encoding) != RF_SUCCESS)
        {
            LOG_ERROR("XML File \"%s\" could not be opened, aborted object creation",RE_XML_OPEN_FAILURE,filename->bytes);
            error = RE_XML_OPEN_FAILURE;
            goto cleanup1;
        }
        if( RF_BITFLAG_ON(openFlag,XML_NEW_IN_MEMORY)) //if it should be in memory remember it
            RF_BITFLAG_SET(ret->flags,XML_IN_MEMORY);
        if(RF_BITFLAG_ON(openFlag,XML_SPACES)) //if it should have spaces instead of tabs for indentation remember it
            RF_BITFLAG_SET(ret->flags,XML_SPACES);
        //add the xml opening tag
        rfTextFile_Write(&ret->f,RFS_("<?xml version=\"1.0\"?>\n"));
        return RF_SUCCESS;
    }
    //else
    LOG_ERROR("Invalid opening flag provided during XML handler initialization",RE_XML_ILLEGAL_OPENFLAG);
    error = RE_INPUT;
    goto cleanup1;


cleanup3:
    rfStringX_Deinit(&copyS);
cleanup2:
    rfTextFile_Deinit(&ret->f);
cleanup1:
    rfStringX_Deinit(&ret->s);
    return error;
}

// Deletes an XML file handler
void rfXML_Destroy(RF_XML* x)
{
    //only if any data is contained
    if(RF_BITFLAG_ON(x->flags,XML_ROOT_INIT))
    {
        rfXMLTag_Deinit(&x->root);
    }
    //clean textfile and buffer reading string
    rfTextFile_Deinit(&x->f);
    rfStringX_Deinit(&x->s);
    //finally free own memory
    free(x);
}
// Deinitializes an XML file handler
void rfXML_Deinit(RF_XML* x)
{
    //only if any data is contained
    if(RF_BITFLAG_ON(x->flags,XML_ROOT_INIT))
    {
        rfXMLTag_Deinit(&x->root);
    }
    //clean textfile and buffer reading string
    rfTextFile_Deinit(&x->f);
    rfStringX_Deinit(&x->s);
}


//Outputs the XML to a file
int32_t i_rfXML_ToFile(RF_XML* x, void* nameP,char* encodingP)
{
    //check for file not read in memory
    if(RF_BITFLAG_ON(x->flags,XML_IN_MEMORY)==false)
        return RF_FAILURE;
    char tempFName[L_tmpnam+1], *tmpNamePtr;
    //open the output file
    char encoding = *encodingP, temp = false;
    RF_String* name = (RF_String*)nameP;
    int level = 0;
    RF_TextFile out;
    RF_StringX strBuff;
    //if an output name has been given
    if(name != 0)
    {
        if(rfTextFile_Init(&out,name,RF_FILE_NEW,encoding) != RF_SUCCESS)
        {
            LOG_ERROR("Failed to output XML to a file due to failure to open output file \"%s\" ",RE_XML_OPEN_FAILURE,name->bytes);
            return RE_XML_OPEN_FAILURE;
        }
    }
    else
    {
        //get a tempfile name
        temp=true;
        tmpNamePtr = tmpnam(tempFName);
        if(rfTextFile_Init(&out,RFS_(tmpNamePtr),RF_FILE_NEW,encoding) != RF_SUCCESS)
        {
            LOG_ERROR("Failed to output XML to a file due to failure to create a temporary output file",RE_XML_OPEN_FAILURE);
            return RE_XML_OPEN_FAILURE;
        }
    }
    //write the xml version and <xml> open tag
    rfTextFile_Write(&out,RFS_("<?xml version=\"%f\"?>\n",x->version));
    //initialize the buffer string
    rfStringX_Init_txtbuff(&strBuff,"");
    if(i_rfXMLTag_PrintToFile(&x->root,&out,&strBuff,level)==false)
    {
        LOG_ERROR("Failed to write to the output XML file",RE_XML_WRITE);
        rfStringX_Deinit(&strBuff);
        rfTextFile_Deinit(&out);
        return RE_XML_WRITE;
    }
    //get rid of the no longer needed stringx buffer
    rfStringX_Deinit(&strBuff);
    //if a temporary file was created
    if(temp == true)
    {
        //delete the old file
        fclose(x->f.f);
        if(rfDeleteFile(&x->f.name) != RF_SUCCESS)
        {
            rfTextFile_Deinit(&out);
            return RE_FILE_DELETE;
        }
        //rename the temp file to be the new file
        fclose(out.f);
        if(rfRenameFile(RFS_(tmpNamePtr),&x->f.name) != RF_SUCCESS)
        {
            rfTextFile_Deinit(&out);
            return RE_FILE_RENAME;
        }
        x->f.f = fopen(x->f.name.bytes,"r"i_PLUSB_WIN32"+");
        //also move to its beginning
        rfTextFile_GoToLine(&x->f,1);
        out.f = 0;
    }
    //success
    rfTextFile_Deinit(&out);
    return RF_SUCCESS;
}

// Inserts an XML tag in the XML file
int32_t i_rfXML_InsertTag(RF_XML* x, RF_XMLTag* t,char* flagsP)
{
    RF_StringX tempS;
    int32_t error;
    char flags = *flagsP;
    //get the tag's string form
    if(rfXMLTag_ToStr(t,&tempS)==false)
    {
        LOG_ERROR("Could not insert tag <%s> to the XML file because there was a failure in representing it as a String",RE_XML_TOSTR,t->name.bytes);
        return RE_XML_TOSTR;
    }
    //insert it into the file, as long as it's not the first tag ( the root)
    if(RF_BITFLAG_ON(x->flags,XML_ROOT_INIT))
    {
        error = rfXML_InsertStr(x,&tempS,flags);
    }
    else
    {//is the first tag, the root
        rfXMLTag_Copy_IN(&x->root,t);
        RF_BITFLAG_SET(x->flags,XML_ROOT_INIT);//set that we got the root
        //and just write it in the file
        x->lastLine = x->f.line;
        if((error = rfTextFile_GetOffset(&x->f,&x->rootPos)) != RF_SUCCESS)
        {
            LOG_ERROR("During inserting the root to XML file \"%s\" there was a problem when getting the current file position",error,x->f.name.bytes);
        }
        else
            error = rfTextFile_Write(&x->f,&tempS);
        x->rootPos-=1;
        x->lastTag = rfXMLTag_Copy_OUT(t);//make a copy of the given tag for the last tag since tag t is temporary
        x->currentTag = x->lastTag;
    }
    //if we need to keep it
    if(RF_BITFLAG_ON(flags,XML_GO))
    {
        x->lastTag = rfXMLTag_Copy_OUT(t);//make a copy of the given tag for the last tag since tag t is temporary
        x->currentTag = x->lastTag;
    }
    rfStringX_Deinit(&tempS);
    return error;
}

//Inserts a string in the XML file
int32_t i_rfXML_InsertStr(RF_XML* x,void* sP,char* flagsP)
{
    int32_t error;
    uint32_t i;
    uint64_t lineN;
    RF_StringX strBuff,newLine,attributes,nS,tabs;
    RF_String* s = (RF_String*)sP;
    char flags = *flagsP;
    //if the file is open in memory (and hence not just on disk)
    if(RF_BITFLAG_ON(x->flags,XML_IN_MEMORY))
    {
        LOG_ERROR("Attempted to insert a string in an XML file with the handler not being opened as a disk only handler",RE_XML_NOT_INDISK)
        return RF_FAILURE;
    }
    //if the last tag has not been initialized by an operation, and we do have a root
    if(x->lastTag == 0)
    {
        LOG_ERROR("Attempted to insert a string in the XML file without having performed an operation to properly place the file pointer",RE_XML_NOFILEOP)
        return RE_XML_NOFILEOP;
    }
    //init the buffer string
    rfStringX_Init_txtbuff(&strBuff,""); ///cleanup 1 -- deinit this string
    //get the previous line at the position
    //rfTextFile_GetLine(&x->f,x->lastLine,&strBuff);
    if((error=rfTextFile_GoToLine(&x->f,x->lastLine)) != RF_SUCCESS)
    {
        LOG_ERROR("Could not move to the beginning of the last tag's line",error);
        goto cleanup1;
    }
    if((error=rfTextFile_ReadLine(&x->f,&strBuff)) != RF_SUCCESS)
    {
        LOG_ERROR("Could not read the last tag's line",error);
        goto cleanup1;
    }
    //keep the line number of the added tag that will become the last line if all goes well
    lineN = x->lastLine+1;

    //create a new insertion string since it may be initialized in the local stack so can't replace anything in the original
    rfStringX_FromString_IN(&nS,s);
    //change all the newlines to newlines plus the proper amount of tabs +1 or spaces + 3 so that they are indented properly
    rfStringX_Init(&tabs,"");///cleanup 2 - deinit these strings
    if(RF_BITFLAG_ON(x->flags,XML_SPACES))
    {
        for(i=0;i<x->level;i++)
            rfStringX_Append(&tabs,RFS_("   "));
        rfStringX_Replace(&nS,RFS_("\n"),RFS_("\n%s   ",rfString_Cstr(&tabs)),0,0);
    }
    else
    {
        for(i=0;i<x->level;i++)
            rfStringX_Append(&tabs,RFS_("\t"));
        rfStringX_Replace(&nS,RFS_("\n"),RFS_("\n%s\t",rfString_Cstr(&tabs)),0,0);
    }

    if(RF_BITFLAG_ON(flags,XML_TAG_END))///insertion at end of tag
    {
        RF_String before;
        //read until you get to the line that the tag closes
        while(rfString_Find(&strBuff,RFS_("</%s>",x->lastTag->name.bytes),0) == RF_FAILURE)
        {
            if((error=rfTextFile_ReadLine(&x->f,&strBuff)) != RF_SUCCESS)
            {
                if(error==RE_FILE_EOF)
                    LOG_ERROR("During attempting to find the closing of tag </%s> end of file was encountered",error,x->lastTag->name.bytes)
                else
                    LOG_ERROR("During attempting to find the closing of tag </%s> a read error happened",error,x->lastTag->name.bytes)
                goto cleanup2;
            }
        }
        //move after the tag closing if found
        rfString_Before(&strBuff,RFS_("</%s>",x->lastTag->name.bytes),&before,0);
        if(rfStringX_MoveAfter(&strBuff,RFS_("</%s>",x->lastTag->name.bytes),0,0) == RF_FAILURE)
        {
            LOG_ERROR("The opening of the expected XML tag <%s> was not found in the current XML line",RE_XML_OPEN_TAG,x->lastTag->name.bytes)
            error = RE_XML_OPEN_TAG;
            rfString_Deinit(&before);
            goto cleanup2;
        }
        rfString_TrimEnd(&nS,RFS_("\n\t "));
        //create the new line that will replace the previous line, and also add the same level of tabs as the previous one
        if(RF_BITFLAG_ON(x->flags,XML_SPACES))///cleanup3 - the newline string
            rfStringX_Init(&newLine,"%s   %s\n%s</%s>%s",before.bytes,rfString_Cstr(&nS),rfString_Cstr(&tabs),x->lastTag->name.bytes,rfString_Cstr(&strBuff));
        else
            rfStringX_Init(&newLine,"%s\t%s\n%s</%s>%s",before.bytes,rfString_Cstr(&nS),rfString_Cstr(&tabs),x->lastTag->name.bytes,rfString_Cstr(&strBuff));
        rfString_TrimEnd(&newLine,RFS_("\n\t"));
        if((error=rfTextFile_Replace(&x->f,x->f.line-1,&newLine)) != RF_SUCCESS)
        {
            LOG_ERROR("Failed to replace a line of the XML file \"%s\" with the requested string",error,x->f.name.bytes);
            rfString_Deinit(&before);
            goto cleanup3;
        }
    }//end of the "end of tag" case
    else///insertion at start of tag
    {
        RF_StringX before;
        //move after the tag opening if found
        if(rfStringX_MoveAfter(&strBuff,RFS_("<%s",x->lastTag->name.bytes),&before,0) == RF_FAILURE)
        {
            LOG_ERROR("The opening of the expected XML tag <%s> was not found in the current XML line",RE_XML_OPEN_TAG,x->lastTag->name.bytes)
            error = RE_XML_OPEN_TAG;
            goto cleanup2;
        }
        if(rfStringX_MoveAfter(&strBuff,RFS_(">"),&attributes,0)==RF_FAILURE)
        {
            LOG_ERROR("The opening tag of the XML tag <%s> does not have a closing bracket",RE_XML_PARSE_FAILURE,x->lastTag->name.bytes)
            error = RE_XML_PARSE_FAILURE;
            rfStringX_Deinit(&before);
            goto cleanup2;
        }
        //create the new line that will replace the previous line, and also add the same level of tabs as the previous one
        if(RF_BITFLAG_ON(x->flags,XML_SPACES)) ///cleanup3 - the newline string
            rfStringX_Init(&newLine,"%s<%s%s>\n%s   %s%s",rfString_Cstr(&before),x->lastTag->name.bytes,rfString_Cstr(&attributes),rfString_Cstr(&tabs),rfString_Cstr(&nS),rfString_Cstr(&strBuff));
        else
            rfStringX_Init(&newLine,"%s<%s%s>\n%s\t%s%s",rfString_Cstr(&before),x->lastTag->name.bytes,rfString_Cstr(&attributes),rfString_Cstr(&tabs),rfString_Cstr(&nS),rfString_Cstr(&strBuff));
        rfString_TrimEnd(&newLine,RFS_("\n\t "));
        if((error=rfTextFile_Replace(&x->f,x->lastLine,&newLine)) != RF_SUCCESS)
        {
            LOG_ERROR("Failed to replace a line of the XML file \"%s\" with the requested string",error,x->f.name.bytes);
            rfStringX_Deinit(&attributes);
            rfStringX_Deinit(&before);
            goto cleanup3;
        }
    }//end of the if for putting it either in start or end of tag
    ///success (error would have RF_SUCCESS) here if all went well

    if(RF_BITFLAG_ON(flags,XML_GO))
    {
        x->lastLine = x->f.line;
        rfTextFile_GoToLine(&x->f,x->lastLine);
        x->level+=1;
    }
    else //stay on the same parent
    {
        x->lastLine = lineN-1;
        rfTextFile_GoToLine(&x->f,x->lastLine);
    }


    error = RF_SUCCESS;

//errors
cleanup3:
    rfStringX_Deinit(&newLine);
cleanup2:
    rfStringX_Deinit(&tabs);
    rfStringX_Deinit(&nS);
cleanup1:
    rfStringX_Deinit(&strBuff);
    return error;
}



//Adds the root tag in an XML file initialized in memory
char i_rfXML_AddRoot(RF_XML* x,RF_XMLTag* t)
{
    if(RF_BITFLAG_ON(x->flags,XML_ROOT_INIT))
        return false;
    if(!RF_BITFLAG_ON(x->flags,XML_IN_MEMORY))
        return false;
    //copy the root
    rfXMLTag_Copy_IN(&x->root,t);
    //denote it's initialized now
    RF_BITFLAG_SET(x->flags,XML_ROOT_INIT);
    return true;
}

//Provides a pointer to the XML handle's root
const RF_XMLTag* rfXML_GetRoot(RF_XML* x)
{
    return &x->root;
}

//Returns a tag if it exists in the xml data either in the disk or in memory. Scans the xml tree recursively searching for the first tag matching the given name,attributes and contents
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_XMLTag* rfXML_GetTag(RF_XML* x,void* tName,void* contents, uint32_t* attrNP, ...)
#else
RF_XMLTag* i_rfXML_GetTag(RF_XML* x,void* tName,void* contents, uint32_t* attrNP, ...)
#endif
{
    uint32_t i;
    int32_t error;
    RF_String** sAttribs=0;
    RF_String** sAttribValues=0;
    RF_XMLTag* tempTag;//the tag to return
    char ffwhile;
    uint32_t attrN = *attrNP;
    RF_BITFLAG_UNSET(x->flags,XML_SEARCH_DONE);
    x->rcs = 0;
    //allocate the variable number of attributes array
    if(attrN != 0)
    {
        RF_MALLOC(sAttribs,sizeof(RF_String*)*attrN)
        RF_MALLOC(sAttribValues,sizeof(RF_String*)*attrN)
    }
    //get variable number of attributes
    va_list argList;
    va_start(argList,attrNP);
    for(i = 0; i < attrN; i ++)
    {
       sAttribs[i]      = va_arg(argList,RF_String*);
       sAttribValues[i] = va_arg(argList,RF_String*);
    }
    va_end(argList);
    ///==-If the file is loaded in memory===///
    if(RF_BITFLAG_ON(x->flags,XML_IN_MEMORY))
    {
        //search for the tag with the given name starting from the root
        tempTag = rfXML_SearchFull(x,&x->root,tName,contents,attrN,sAttribs,sAttribValues);
    }
    else///===The file is only on disk===///
    {//go to the where the root is
        x->level = 0;
        if((error=rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET)) != RF_SUCCESS)
        {
            LOG_ERROR("There was an error while attempting to go to the position of the root of the XML file\"%s\"",error,x->f.name.bytes);
            return 0;
        }
        //check if the root is the one we seek
        rfTextFile_ReadLine(&x->f,&x->s);
        if(rfString_Find(&x->s,RFS_("<%s",((RF_String*)tName)->bytes),0) != RF_FAILURE)
        {
            x->lastTag = &x->root;
            x->lastLine = x->f.line-1;
            return rfXMLTag_Copy_OUT(&x->root);
        }
        else//go back to start the search
        {
            if((error=rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET)) != RF_SUCCESS)
            {
                LOG_ERROR("There was an error while attempting to go to the position of the root of the XML file\"%s\"",error,x->f.name.bytes);
                return 0;
            }
        }
        //allocate the temporary tag that will serve as the return value
        RF_MALLOC(tempTag,sizeof(RF_XMLTag));
        ///start the search
        while(1)
        {
            while(rfXML_GoIn_dsk(x,0)==RF_SUCCESS)
            {
                ffwhile = false;
                ///check the retrieved tag
                //name check
                if(rfString_Equal(&x->lastTag->name,tName) == false)
                    continue;
                //attributes check
                if(attrN != 0)
                {
                    char match = true;
                    for(i=0;i<attrN; i++)
                    {
                        if( rfString_Equal(  ((RF_String*) (rfListP_Get(&x->lastTag->attributes,i))),sAttribs[i])      == false ||  //attribute name does not match
                            rfString_Equal( ((RF_String*)(rfListP_Get(&x->lastTag->attribValues,i))),sAttribValues[i]) == false  )  //attribute value does not match
                        {
                            match = false;
                            break;
                        }
                    }//if it's not a match
                    if(match == false)
                    {
                        continue;
                    }
                }//end of attributes check
                //if the name and attributes match then we should make sure to have read all of the tag so that we can match contents
                error = rfXML_GoNext_dsk(x,tempTag);
                if(error != RF_SUCCESS)
                {
                    free(tempTag);//notice freeing and not deinitializing here
                    tempTag = 0;
                    //reached the end of the file and found nothing
                    if(error != RE_XML_EOF)
                    {
                        LOG_ERROR("XML Parsing error occured during attempting to retrieve tag <%s> from file \"%s\"",RE_XML_PARSE_FAILURE,((RF_String*)tName)->bytes,x->f.name.bytes);
                    }
                    ffwhile = true;//just to get to the end of the function
                    break;
                }
                //contents check
                if(contents!=0)
                {
                    if(rfString_Find(&x->lastTag->contents,contents,0) == RF_FAILURE)
                        continue;
                }
                ///MATCH
                ffwhile = true;
                //break off the loop and go to return and cleanup
                break;
            }
            if(ffwhile == true)
                break;

            //#2# Read the deepest tag and go to the next
            if((error = rfXML_GoNext_dsk(x,tempTag))== RF_SUCCESS)
            {
                ///check the retrieved tag
                //name check
                if(rfString_Equal(&tempTag->name,tName) == false)
                {
                    rfXMLTag_Deinit(tempTag);//deiniting it here because I want to keep the allocated memory on the heap (to be returned to the user)
                    continue;
                }
                //contents check
                if(contents!=0)
                {
                    if(rfString_Find(&tempTag->contents,contents,0) == RF_FAILURE)
                    {
                        rfXMLTag_Deinit(tempTag);//deiniting it here because I want to keep the allocated memory on the heap (to be returned to the user)
                        continue;
                    }
                }
                //attributes check
                if(attrN != 0)
                {
                    char match = true;
                    for(i=0;i<attrN; i++)
                    {
                        if( rfString_Equal(  ((RF_String*) (rfListP_Get(&tempTag->attributes,i))),sAttribs[i])      == false ||  //attribute name does not match
                            rfString_Equal( ((RF_String*)(rfListP_Get(&tempTag->attribValues,i))),sAttribValues[i]) == false  )  //attribute value does not match
                        {
                            match = false;
                            break;
                        }
                    }//if it's not a match
                    if(match == false)
                    {
                        rfXMLTag_Deinit(tempTag);//deiniting it here because I want to keep the allocated memory on the heap (to be returned to the user)
                        continue;
                    }
                }
                ///MATCH
                break;//break of the loop with a fully initialized temp Tag
            }
            //reached the end of the file and found nothing
            else if(error == RE_XML_EOF)
            {
                free(tempTag);//notice freeing and not deinitializing here
                tempTag = 0;
                break; //break off the loop to go to the end
            }
            else
            {
                LOG_ERROR("XML Parsing error occured during attempting to retrieve tag <%s> from file \"%s\"",RE_XML_PARSE_FAILURE,((RF_String*)tName)->bytes,x->f.name.bytes);
                free(tempTag);//notice freeing and not deinitializing here
                tempTag = 0;
                break;
            }
        }//search loop
    }//end of the in disk only case
    //free the attributes array before returning
    if(attrN != 0)
    {
        free(sAttribs);
        free(sAttribValues);
    }
    //temporary test to see if the level should be -1 of what it is after this function
    if(x->level !=0)
        x->level--;
    return tempTag;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_XMLTag* i_NVrfXML_GetTag(RF_XML* x,void* tName,void* contents)
{
    char ffwhile;
    int32_t error;
    RF_XMLTag* tempTag;
    if(RF_BITFLAG_ON(x->flags,XML_IN_MEMORY))
    {
        RF_BITFLAG_UNSET(x->flags,XML_SEARCH_DONE);
        x->rcs =0;
        //search for the tag with the given name starting from the root
        return rfXML_SearchSmall(x,&x->root,tName,contents);
    }//end of the only in memory case
    else
    {//go to the where the root is
        x->level = 0;
        if((error=rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET)) != RF_SUCCESS)
        {
            LOG_ERROR("There was an error while attempting to go to the position of the root of the XML file\"%s\"",error,x->f.name.bytes);
            return 0;
        }
        //check if the root is the one we seek
        rfTextFile_ReadLine(&x->f,&x->s);
        if(rfString_Find(&x->s,RFS_("<%s",((RF_String*)tName)->bytes),0) != RF_FAILURE)
        {
            x->lastTag = &x->root;
            x->lastLine = x->f.line-1;
            return rfXMLTag_Copy_OUT(&x->root);
        }
        else//go back to start the search
        {
            if((error=rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET)) != RF_SUCCESS)
            {
                LOG_ERROR("There was an error while attempting to go to the position of the root of the XML file\"%s\"",error,x->f.name.bytes);
                return 0;
            }
        }
        //allocate the temporary tag that will serve as the return value
        RF_MALLOC(tempTag,sizeof(RF_XMLTag));
        ///start the search
        while(1)
        {
            //#1# Go as deep in this tag as you can
            while(rfXML_GoIn_dsk(x,0)==RF_SUCCESS)
            {
                ffwhile = false;
                ///check the retrieved tag
                //name check
                if(rfString_Equal(&x->lastTag->name,tName) == false)
                    continue;
                //if the name matches then we should make sure to have read all of the tag so that we can match contents
                error = rfXML_GoNext_dsk(x,tempTag);
                if(error != RF_SUCCESS)
                {
                    free(tempTag);//notice freeing and not deinitializing here
                    tempTag = 0;
                    //reached the end of the file and found nothing
                    if(error != RE_XML_EOF)
                    {
                        LOG_ERROR("XML Parsing error occured during attempting to retrieve tag <%s> from file \"%s\"",RE_XML_PARSE_FAILURE,((RF_String*)tName)->bytes,x->f.name.bytes);
                    }
                    ffwhile = true;
                    break;
                }
                //contents check
                if(contents!=0)
                {
                    if(rfString_Find(&x->lastTag->contents,contents,0) == RF_FAILURE)
                        continue;
                }
                ///MATCH
                ffwhile = true;
                //break off the loop and go to return and cleanup
                break;
            }
            if(ffwhile == true)
                break;
            //#2# Read the deepest tag and go to the next
            if((error = rfXML_GoNext_dsk(x,tempTag))== RF_SUCCESS)
            {
                ///check the retrieved tag
                //name check
                if(rfString_Equal(&tempTag->name,tName) == false)
                {
                    rfXMLTag_Deinit(tempTag);//deiniting it here because I want to keep the allocated memory on the heap (to be returned to the user)
                    continue;
                }
                //contents check
                if(contents!=0)
                {
                    if(rfString_Find(&tempTag->contents,contents,0) == RF_FAILURE)
                    {
                        rfXMLTag_Deinit(tempTag);//deiniting it here because I want to keep the allocated memory on the heap (to be returned to the user)
                        continue;
                    }
                }
                ///MATCH
                break;//break of the loop with a fully initialized temp Tag
            }
            //reached the end of the file and found nothing
            else if(error == RE_XML_EOF)
            {
                free(tempTag);//notice freeing and not deinitializing here
                tempTag = 0;
                break; //break off the loop to go to the end
            }
            else
            {
                LOG_ERROR("XML Parsing error occured during attempting to retrieve tag <%s> from file \"%s\"",RE_XML_PARSE_FAILURE,((RF_String*)tName)->bytes,x->f.name.bytes);
                free(tempTag);//notice freeing and not deinitializing here
                tempTag = 0;
                break;
            }
        }//search loop

        return tempTag;
    }//end of the only in disk case
}
#endif
//Returns a tag if it exists in the xml data. Scans the xml tree recursively searching for the first tag matching tName having childName child somewhere in its children
RF_XMLTag* i_rfXML_GetTag_child(RF_XML* x,void* tName,void* childName,char* directP,char* rParentP)
{
    char direct,rParent;
    //if the handle is not loaded in memory return 0
    if(!RF_BITFLAG_ON(x->flags,XML_IN_MEMORY))
        return 0;
    direct = *directP;
    rParent = *rParentP;
    RF_BITFLAG_UNSET(x->flags,XML_SEARCH_DONE);
    x->rcs = 0;
    return i_rfXML_SearchChild(x,&x->root,tName,childName,direct,rParent);
}



//Returns a tag if it exists in the xml data either in the disk or in memory. Scans the xml tree recursively searching for the first tag matching the given name,attributes and contents
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
int32_t rfXML_GoToTag(RF_XML* x,void* tName,void* contents, uint32_t* attrNP, ...)
#else
int32_t i_rfXML_GoToTag(RF_XML* x,void* tName,void* contents, uint32_t* attrNP, ...)
#endif
{
    uint32_t i;
    int32_t error;
    RF_String** sAttribs=0;
    RF_String** sAttribValues=0;
    RF_XMLTag t;
    char ffwhile;
    uint32_t attrN = *attrNP;
    //do a check at the very beginning for if the handler is in disk and not in memory
    if(RF_BITFLAG_ON(x->flags,XML_IN_MEMORY))
    {
        LOG_ERROR("Attempted to move to a specific tag in the file while the XML handler was in memory and not in disk",RE_XML_NOT_INDISK);
        return RF_FAILURE;
    }
    //allocate the variable number of attributes array
    if(attrN != 0)
    {
        RF_MALLOC(sAttribs,sizeof(RF_String*)*attrN)
        RF_MALLOC(sAttribValues,sizeof(RF_String*)*attrN)
    }
    //get variable number of attributes
    va_list argList;
    va_start(argList,attrNP);
    for(i = 0; i < attrN; i ++)
    {
       sAttribs[i]      = va_arg(argList,RF_String*);
       sAttribValues[i] = va_arg(argList,RF_String*);
    }
    va_end(argList);

    x->level = 0;
    //go to the where the root is
    if((error=rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET)) != RF_SUCCESS)
    {
        LOG_ERROR("There was an error while attempting to go to the position of the root of the XML file\"%s\"",error,x->f.name.bytes);
        return error;
    }
    //check if the root is the one we seek
    rfTextFile_ReadLine(&x->f,&x->s);
    if(rfString_Find(&x->s,RFS_("<%s",((RF_String*)tName)->bytes),0) != RF_FAILURE)
    {
        x->lastTag = &x->root;
        x->lastLine = x->f.line-1;
        return RF_SUCCESS;
    }
    else//go back to begin the search
    {
        if((error=rfTextFile_GoToOffset(&x->f,x->rootPos,SEEK_SET)) != RF_SUCCESS)
        {
            LOG_ERROR("There was an error while attempting to go to the position of the root of the XML file\"%s\"",error,x->f.name.bytes);
            return error;
        }
    }
    ///start the search
    while(1)
    {
        while(rfXML_GoIn_dsk(x,0)==RF_SUCCESS)
        {
            ffwhile = false;
            ///check the retrieved tag
            //name check
            if(rfString_Equal(&x->lastTag->name,tName) == false)
                continue;
            //attributes check
            if(attrN != 0)
            {
                char match = true;
                for(i=0;i<attrN; i++)
                {
                    if( rfString_Equal(  ((RF_String*) (rfListP_Get(&x->lastTag->attributes,i))),sAttribs[i])      == false ||  //attribute name does not match
                        rfString_Equal( ((RF_String*)(rfListP_Get(&x->lastTag->attribValues,i))),sAttribValues[i]) == false  )  //attribute value does not match
                    {
                        match = false;
                        break;
                    }
                }//if it's not a match
                if(match == false)
                {
                    continue;
                }
            }//end of attributes check
            //if the name and attributes match then we should make sure to have read all of the tag so that we can match contents
            error = rfXML_GoNext_dsk(x,&t);
            if(error != RF_SUCCESS)
            {
                //reached the end of the file and found nothing
                if(error != RE_XML_EOF)
                {
                    LOG_ERROR("XML Parsing error occured during attempting to retrieve tag <%s> from file \"%s\"",RE_XML_PARSE_FAILURE,((RF_String*)tName)->bytes,x->f.name.bytes);
                }
                ffwhile = true;//just to get to the end of the function
                break;
            }
            //contents check
            if(contents!=0)
            {
                if(rfString_Find(&x->lastTag->contents,contents,0) == RF_FAILURE)
                    continue;
            }
            ///MATCH
            ffwhile = true;
            //break off the loop and go to return and cleanup
            break;
        }
        if(ffwhile == true)
            break;


        //#2# Read the deepest tag and go to the next
        if((error = rfXML_GoNext_dsk(x,&t))== RF_SUCCESS)
        {
            ///check the retrieved tag
            //name check
            if(rfString_Equal(&t.name,tName) == false)
            {
                rfXMLTag_Deinit(&t);
                continue;
            }
            //contents check
            if(contents!=0)
            {
                if(rfString_Find(&t.contents,contents,0) == RF_FAILURE)
                {
                    rfXMLTag_Deinit(&t);
                    continue;
                }
            }
            //attributes check
            if(attrN != 0)
            {
                char match = true;
                for(i=0;i<attrN; i++)
                {
                    if( rfString_Equal(  ((RF_String*) (rfListP_Get(&t.attributes,i))),sAttribs[i])      == false ||  //attribute name does not match
                        rfString_Equal( ((RF_String*)(rfListP_Get(&t.attribValues,i))),sAttribValues[i]) == false  )  //attribute value does not match
                    {
                        match = false;
                        break;
                    }
                }//if it's not a match
                if(match == false)
                {
                    rfXMLTag_Deinit(&t);
                    continue;
                }
            }
            ///MATCH
            //deinit the temp tag and break off the loop
            rfXMLTag_Deinit(&t);
            error = RF_SUCCESS;
            break;
        }
        //reached the end of the file and found nothing
        else if(error == RE_XML_EOF)
        {
            break; //break off the loop to go to the end
        }
        else
        {
            LOG_ERROR("XML Parsing error occured during attempting to retrieve tag <%s> from file \"%s\"",RE_XML_PARSE_FAILURE,((RF_String*)tName)->bytes,x->f.name.bytes);
            break;
        }
    }//search loop
    //free the attributes array before returning
    if(attrN != 0)
    {
        free(sAttribs);
        free(sAttribValues);
    }
    //temporary test to see if the level should be -1 of what it is after this function
    if(x->level !=0)
        x->level--;
    return error;
}


