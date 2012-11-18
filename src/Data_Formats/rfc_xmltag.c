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

#include <Data_Formats/rfc_xmltag.h>
#include <stdlib.h>//for malloc
#include <string.h> //for memmove
/********************************************************************START OF XML_TAG functions *************************************************************************/


//Allocates and returns an XML Tag
RF_XMLTag* rfXMLTag_Create2(RF_XMLTag* parent,void* name)
{
    RF_XMLTag* x;
    RF_MALLOC(x,sizeof(RF_XMLTag));
    //get the parent pointer and copy the name string
    x->parent = parent;
    rfString_Copy_IN(&x->name,name);
    //initialize the lists
    rfListP_Init(&x->children,RF_XMLTag,5,&rfXMLTag_Destroy,&rfXMLTag_Copy_OUT);//initial capacity of 5 children
    rfListP_Init(&x->attributes,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attributes
    rfListP_Init(&x->attribValues,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attribute values
    //initialize the contents string
    rfStringX_Init_buff(&x->contents,128,"");//initial capacity of 128 bytes for the content string
    //if the parent exists add this child to it
    if(parent!=0)
        rfXMLTag_AddTag_nocopy(parent,x);
    return x;
}
RF_XMLTag* rfXMLTag_Createv(RF_XMLTag* parent,void* name,void* content,unsigned char* attrNP,...)
{
    RF_XMLTag* x;
    uint32_t i;
    unsigned char attrN = * attrNP;
    RF_MALLOC(x,sizeof(RF_XMLTag));
    //get the parent pointer and copy the name string
    x->parent = parent;
    rfString_Copy_IN(&x->name,name);
    //initialize the lists
    rfListP_Init(&x->children,RF_XMLTag,5,&rfXMLTag_Destroy,&rfXMLTag_Copy_OUT);//initial capacity of 5 children
    rfListP_Init(&x->attributes,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attributes
    rfListP_Init(&x->attribValues,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attribute values
    //initialize the contents string
    rfStringX_Init_buff(&x->contents,128,"");//initial capacity of 128 bytes for the content string
    //if the parent exists add this child to it
    if(parent!=0)
        rfXMLTag_AddTag_nocopy(parent,x);
    //add the content
    if(content != 0 )
        rfXMLTag_AddContent(x,content);
    //get the attributes
    va_list argList;
    va_start(argList,attrNP);
    for(i = 0; i < attrN; i ++)
    {
       RF_String* attrName  = va_arg(argList,RF_String*);
       RF_String* attrValue = va_arg(argList,RF_String*);
       rfXMLTag_AddAttribute(x,attrName,attrValue);
    }
    va_end(argList);
    return x;
}
//Allocates and returns an XML Tag from local memory
RF_XMLTag* rfXMLTag_CreateLocalv(RF_XMLTag* parent,void* nameP,void* contentP,unsigned char attrN,...)
{
    RF_XMLTag* x;
    RF_String* name = (RF_String*)nameP;
    RF_String* content = (RF_String*)contentP;
    uint32_t i;
    x = rfLMS_Push(RF_LMS,sizeof(RF_XMLTag));
    if(x == 0)
    {
        LOG_ERROR("Memory allocation from the Local Memory Stack failed during XML tag allocation. \
                  Insufficient local memory stack space. Consider compiling the library with bigger stack space. Quitting proccess...",
                  RE_LOCALMEMSTACK_INSUFFICIENT);
        exit(RE_LOCALMEMSTACK_INSUFFICIENT);
    }
    ///get the parent pointer and copy the name string
    x->parent = parent;
    x->name.byteLength = name->byteLength;
    x->name.bytes = rfLMS_Push(RF_LMS,name->byteLength+1);
    memmove(x->name.bytes,name->bytes,name->byteLength+1);
    ///initialize the lists
    ///initialize the childrens list
    x->children.data = rfLMS_Push(RF_LMS,(sizeof(void*)*attrN)+1);
    x->children.bufferCapacity = sizeof(void*)+1;
    x->children.size = 0;
    x->children.ptr2Destroy = (void(*)(void*))rfString_Destroy;
    x->children.ptr2Copy = (void*(*)(void*))rfString_Copy_OUT;
    ///initialize attribute names list
    //allocate the data buffer and keep its size
    x->attributes.data = rfLMS_Push(RF_LMS,(sizeof(void*)*attrN)+1);
    x->attributes.bufferCapacity = attrN*sizeof(void*)+1;
    x->attributes.size = 0;
    x->attributes.ptr2Destroy =(void(*)(void*)) rfString_Destroy;
    x->attributes.ptr2Copy = (void*(*)(void*))rfString_Copy_OUT;
    ///initialize attribute values list
    //allocate the data buffer and keep its size
    x->attribValues.data = rfLMS_Push(RF_LMS,(sizeof(void*)*attrN)+1);
    x->attribValues.bufferCapacity = attrN*sizeof(void*)+1;
    x->attribValues.size = 0;
    x->attribValues.ptr2Destroy = (void(*)(void*))rfString_Destroy;
    x->attribValues.ptr2Copy = (void*(*)(void*))rfString_Copy_OUT;
    /// get and add the content
    if(content != 0 )
    {
        x->contents.INH_String.byteLength = content->byteLength;
        x->contents.bIndex = 0;
        x->contents.bSize = content->byteLength+1;
        x->contents.INH_String.bytes = rfLMS_Push(RF_LMS,content->byteLength+1);
        memmove(x->contents.INH_String.bytes,content->bytes,content->byteLength+1);
    }
    else
    {
        x->contents.INH_String.byteLength = 1;
        x->contents.bIndex = 0;
        x->contents.bSize = 1;
        x->contents.INH_String.bytes = rfLMS_Push(RF_LMS,1);
        x->contents.INH_String.bytes[0] = '\0';
    }

    //get the attributes
    va_list argList;
    va_start(argList,attrN);
    for(i = 0; i < attrN; i ++)
    {
       RF_String* attrNameTMP  = va_arg(argList,RF_String*);
       RF_String* attrValueTMP = va_arg(argList,RF_String*);
       RF_String* attrName,*attrValue;
       attrName = rfLMS_Push(RF_LMS,sizeof(RF_String));
       attrValue = rfLMS_Push(RF_LMS,sizeof(RF_String));
       rfString_Copy_IN(attrName,attrNameTMP);
       rfString_Copy_IN(attrValue,attrValueTMP);

       *(x->attributes.data+x->attributes.size) = attrName;
       x->attributes.size++;
       *(x->attribValues.data+x->attribValues.size) = attrValue;
       x->attribValues.size++;
       //give the value of the pointer's memory location to the appropriate member of the list
    }
    va_end(argList);
    return x;
}
//Initializes an XML Tag
char rfXMLTag_Init3(RF_XMLTag* x,RF_XMLTag* parent,void* name)
{
    x->parent = parent;
    rfString_Copy_IN(&x->name,name);
    //initialize the lists
    rfListP_Init(&x->children,RF_XMLTag,5,&rfXMLTag_Destroy,&rfXMLTag_Copy_OUT);//initial capacity of 5 children
    rfListP_Init(&x->attributes,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attributes
    rfListP_Init(&x->attribValues,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attribute values
    //initialize the contents string
    rfStringX_Init_buff(&x->contents,128,"");//initial capacity of 128 bytes for the content string
    //if the parent exists add this child to it
    if(parent!=0)
        rfXMLTag_AddTag_nocopy(parent,x);
    return true;
}
//Initializes an XML Tag
char rfXMLTag_Initv(RF_XMLTag* x,RF_XMLTag* parent,void* name,void* content,unsigned char* attrNP,...)
{
    uint32_t i;
    unsigned char attrN = * attrNP;
    x->parent = parent;
    rfString_Copy_IN(&x->name,name);
    //initialize the lists
    rfListP_Init(&x->children,RF_XMLTag,5,&rfXMLTag_Destroy,&rfXMLTag_Copy_OUT);//initial capacity of 5 children
    rfListP_Init(&x->attributes,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attributes
    rfListP_Init(&x->attribValues,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attribute values
    //initialize the contents string
    rfStringX_Init_buff(&x->contents,128,"");//initial capacity of 128 bytes for the content string
    //if the parent exists add this child to it
    if(parent!=0)
        rfXMLTag_AddTag_nocopy(parent,x);
    //add the content
    if(content != 0 )
        rfXMLTag_AddContent(x,content);
    //get the attributes
    va_list argList;
    va_start(argList,attrNP);
    for(i = 0; i < attrN; i ++)
    {
       RF_String* attrName  = va_arg(argList,RF_String*);
       RF_String* attrValue = va_arg(argList,RF_String*);
       rfXMLTag_AddAttribute(x,attrName,attrValue);
    }
    va_end(argList);
    return true;
}

// Copies XML Tag @c src to XML Tag @c dst
void rfXMLTag_Copy_IN(RF_XMLTag* dst,RF_XMLTag* src)
{
    dst->parent = src->parent;
    rfString_Copy_IN(&dst->name,&src->name);
    //copy the lists
    rfListP_Copy_IN(&dst->children,&src->children);
    rfListP_Copy_IN(&dst->attributes,&src->attributes);
    rfListP_Copy_IN(&dst->attribValues,&src->attribValues);
    rfStringX_Copy_IN(&dst->contents,&src->contents);
}
//Allocates and returns a copy of the given XML Tag
RF_XMLTag* rfXMLTag_Copy_OUT(RF_XMLTag* src)
{
    RF_XMLTag* dst;
    RF_MALLOC(dst,sizeof(RF_XMLTag));
    dst->parent = src->parent;
    rfString_Copy_IN(&dst->name,&src->name);
    //copy the lists
    rfListP_Copy_IN(&dst->children,&src->children);
    rfListP_Copy_IN(&dst->attributes,&src->attributes);
    rfListP_Copy_IN(&dst->attribValues,&src->attribValues);
    rfStringX_Copy_IN(&dst->contents,&src->contents);
    return dst;
}

//Destroys an XML tag
void rfXMLTag_Destroy(RF_XMLTag* t)
{
    //destroy all of the attributes and clear the list
    rfListP_Deinit(&t->attributes);
    //destroy all of the attributes values and clear the list
    rfListP_Deinit(&t->attribValues);
    //destroy all of the children and clear the list
    rfListP_Deinit(&t->children);
    //free the contents string
    rfStringX_Deinit(&t->contents);
    //free tag name
    rfString_Deinit(&t->name);
    //and finally free own memory
    free(t);
}
//Deinitializes an XML tag
void rfXMLTag_Deinit(RF_XMLTag* t)
{
    //destroy all of the attributes and clear the list
    rfListP_Deinit(&t->attributes);
    //destroy all of the attributes values and clear the list
    rfListP_Deinit(&t->attribValues);
    //destroy all of the children and clear the list
    rfListP_Deinit(&t->children);
    //free the contents string
    rfStringX_Deinit(&t->contents);
    //free tag name
    rfString_Deinit(&t->name);
}

//Adds a child tag by first making an internal copy of the tag
void rfXMLTag_AddTag(RF_XMLTag* t,RF_XMLTag* c)
{
    RF_XMLTag* child = malloc(sizeof(RF_XMLTag));
    rfXMLTag_Copy_IN(child,c);
    rfListP_Add(&t->children,child);
}
//Adds a child tag without making an internal copy
void rfXMLTag_AddTag_nocopy(RF_XMLTag* t,RF_XMLTag* c)
{
    rfListP_Add(&t->children,c);
}
// Adds an attribute to the tag
void i_rfXMLTag_AddAttribute(RF_XMLTag* t,void* attribName,void* attribValue)
{
    RF_String* name = rfString_Copy_OUT(attribName);
    RF_String* value = rfString_Copy_OUT(attribValue);
    rfListP_Add(&t->attributes,name);
    rfListP_Add(&t->attribValues,value);
}

//Adds content to the tag
void i_rfXMLTag_AddContent(RF_XMLTag* t,void* c)
{
    RF_String* content = rfString_Copy_OUT(c);
    rfStringX_Append(&t->contents,content);
}

//Gets the ith child tag of this tag
RF_XMLTag* rfXMLTag_GetChild(RF_XMLTag* t,uint32_t i)
{
    return rfListP_Get(&t->children,i);
}
// Removes a particular child tag
void rfXMLTag_RemoveChild(RF_XMLTag* t,uint32_t i)
{
    rfListP_Remove(&t->children,i);
}

// Gets the ith attribute name of this tag
char rfXMLTag_GetAttributeName(RF_XMLTag* t,uint32_t i,RF_String* name)
{
    RF_String* fromList = rfListP_Get(&t->attributes,i);
    if(fromList == 0)
        return false;
    rfString_Copy_IN(name,fromList);
    return true;
}

//Gets the ith attribute value of this tag
char rfXMLTag_GetAttributeValue(RF_XMLTag* t,uint32_t i,RF_String* value)
{
    RF_String* fromList = rfListP_Get(&t->attribValues,i);
    if(fromList == 0)
        return false;
    rfString_Copy_IN(value,fromList);
    return true;
}

//Gets the ith line of content of the tag
void rfXMLTag_GetContent(RF_XMLTag* t,RF_String* line)
{
    rfString_Copy_IN(line,&t->contents);
}


//! Runs through the tree recursively printing the tags
//! @param t The XML tag that we recursively print
//! @param f The textfile in which we print
//! @param strBuff An already initialized RF_StringX buffer
//! @param level The level of the tag in the xml tree. Is used for the /t tabs
//! @return Returns true for succesful writting of the @c t XML tag and false otherwise
char rfXMLTag_ToStr_internal(RF_XMLTag* t,RF_StringX* strBuff,uint32_t level)
{
    uint32_t i;
    //prepend the required number of tabs
    for(i = 0; i < level; i ++)
        rfStringX_Append(strBuff,RFS_("\t"));
    //create the tag string
    rfStringX_Append(strBuff,RFS_("<%s",t->name.bytes));
    for(i = 0; i < t->attributes.size; i ++)
    {
        rfStringX_Append(strBuff,RFS_(" %s=\"%s\"",((RF_String*)rfListP_Get(&t->attributes,i))->bytes,((RF_String*)rfListP_Get(&t->attribValues,i))->bytes));
    }
    rfStringX_Append(strBuff,RFS_(">\n"));
    //also print the tag's contents(if any)
    if(rfString_Equal(&t->contents,RFS_(""))!= true)
    {
        for(i= 0; i< level; i ++)
            rfStringX_Append(strBuff,RFS_("\t"));
        rfStringX_Append(strBuff,RFS_("%s\n",rfString_Cstr(&t->contents)));
    }
    //recursively call this function for all the children of this tag
    for(i=0;i < t->children.size; i++)
    {
        if(rfXMLTag_ToStr_internal((RF_XMLTag*)rfListP_Get(&t->children,i),strBuff,level+1) == false)
        {
            LOG_ERROR("Failed to convert tag's \"<%s>\" No. %d child to a String",RE_XML_TOSTR,t->name.bytes,i+1)
            return false;
        }
    }
    //when done write the closing of the tag
    //prepend the required number of tabs
    for(i = 0; i < level; i ++)
        rfStringX_Append(strBuff,RFS_("\t"));
    rfStringX_Append(strBuff,RFS_("</%s>\n",t->name.bytes));
    //success
    return true;
}
//Turns the tag into a string
char rfXMLTag_ToStr(RF_XMLTag *t ,RF_StringX* s)
{
    int level = 0;
    RF_StringX strBuff;
    //initialize the buffer string
    rfStringX_Init_buff(s,2048,"");
    if(rfXMLTag_ToStr_internal(t,s,level)==false)
    {
        rfStringX_Deinit(&strBuff);
        LOG_ERROR("Failed to represent an XML tag as a String",RE_XML_TOSTR);
        return false;
    }
    //success
    return true;
}
