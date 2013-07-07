/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
*/

//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h> //for import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/string_decl.h> //for RF_String used in RF_XMLTag
#include <String/stringx_decl.h> //for RF_StringX used in RF_XMLTag
#include <Data_Structures/list_decl.h> //for RF_ListP used in RF_XMLTag
#include <Data_Formats/xmltag_decl.h>//for RF_XMLTag
#include <Data_Formats/xmltag.h>
//*---------------------Outside module inclusion----------------------------------------
#include <String/core.h> //for rfString_Copy_IN() and others
#include <String/corex.h> //for rfStringX_Init_buff() and others
#include <String/common.h> //for RFS_()
#include <String/manipulationx.h> //for rfStringX_Append() and others

#include <Data_Structures/list.h> //for rfListP_Init() and others
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation macros
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h>//for RF_MALLOC()
//for local memory functionality
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <string.h> //for size_t and memset() used in the Local scope macros
    #include <Utils/localmem_decl.h> //for local memory stack
    #include "../Utils/localmem.ph" //for the private local memory macros
//for local memory scope macros
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h> //for the local scope macros
//*---------------------libc Headers inclusion------------------------------------------
#include <stdarg.h> //for va_list() macros
//*----------------------------End of Includes------------------------------------------



#ifdef RF_OPTION_DEFAULT_ARGUMENTS
//Allocates and returns an XML Tag
RF_XMLTag* rfXMLTag_Create2(RF_XMLTag* parent,void* name)
{
    RF_XMLTag* x;
    RF_ENTER_LOCAL_SCOPE()
    RF_MALLOC(x,sizeof(RF_XMLTag));
    //get the parent pointer and copy the name string
    x->parent = parent;
    if(!rfString_Copy_IN(&x->name,name))
    {
        RF_EXIT_LOCAL_SCOPE()
        return NULL;
    }
    //initialize the lists
    rfListP_Init(&x->children,RF_XMLTag,5,&rfXMLTag_Destroy,&rfXMLTag_Copy_OUT);//initial capacity of 5 children
    rfListP_Init(&x->attributes,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attributes
    rfListP_Init(&x->attribValues,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attribute values
    //initialize the contents string
    rfStringX_Init_buff(&x->contents,128,"");//initial capacity of 128 bytes for the content string
    //if the parent exists add this child to it
    if(parent!=0)
        rfXMLTag_AddTag_nocopy(parent,x);

    RF_EXIT_LOCAL_SCOPE()
    return x;
}
#endif

#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_XMLTag* rfXMLTag_Create(RF_XMLTag* parent,void* name,void* content,unsigned char attrN,...)
#else
RF_XMLTag* rfXMLTag_Createv(RF_XMLTag* parent,void* name,void* content,unsigned char attrN,...)
#endif
{
    RF_XMLTag* x;
    uint32_t i;
    RF_ENTER_LOCAL_SCOPE()

    RF_MALLOC(x,sizeof(RF_XMLTag));
    //get the parent pointer and copy the name string
    x->parent = parent;
    if(!rfString_Copy_IN(&x->name,name))
    {
        RF_EXIT_LOCAL_SCOPE()
        return NULL;
    }
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
    va_start(argList,attrN);
    for(i = 0; i < attrN; i ++)
    {
       RF_String* attrName  = va_arg(argList,RF_String*);
       RF_String* attrValue = va_arg(argList,RF_String*);
       rfXMLTag_AddAttribute(x,attrName,attrValue);
    }
    va_end(argList);

    RF_EXIT_LOCAL_SCOPE()
    return x;
}
//Allocates and returns an XML Tag from local memory
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_XMLTag* rfXMLTag_CreateLocal(RF_XMLTag* parent,void* nameP,void* contentP,unsigned char attrN,...)
#else
RF_XMLTag* rfXMLTag_CreateLocalv(RF_XMLTag* parent,void* nameP,void* contentP,unsigned char attrN,...)
#endif
{
    RF_XMLTag* x;
    RF_String* name = (RF_String*)nameP;
    RF_String* content = (RF_String*)contentP;
    uint32_t i;

    //remember the stack pointer before this function
    i_rfLMS_ArgsEval()

    i_rfLMS_Push(x,sizeof(RF_XMLTag));
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
    i_rfLMS_Push(x->name.bytes,name->byteLength+1);
    memmove(x->name.bytes,name->bytes,name->byteLength+1);
    ///initialize the lists
    ///initialize the childrens list
    i_rfLMS_Push(x->children.data,(sizeof(void*)*attrN)+1);
    x->children.bufferCapacity = sizeof(void*)+1;
    x->children.size = 0;
    x->children.ptr2Destroy = (void(*)(void*))rfString_Destroy;
    x->children.ptr2Copy = (void*(*)(void*))rfString_Copy_OUT;
    ///initialize attribute names list
    //allocate the data buffer and keep its size
    i_rfLMS_Push(x->attributes.data,(sizeof(void*)*attrN)+1);
    x->attributes.bufferCapacity = attrN*sizeof(void*)+1;
    x->attributes.size = 0;
    x->attributes.ptr2Destroy =(void(*)(void*)) rfString_Destroy;
    x->attributes.ptr2Copy = (void*(*)(void*))rfString_Copy_OUT;
    ///initialize attribute values list
    //allocate the data buffer and keep its size
    i_rfLMS_Push(x->attribValues.data,(sizeof(void*)*attrN)+1);
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
        i_rfLMS_Push(x->contents.INH_String.bytes,content->byteLength+1);
        memmove(x->contents.INH_String.bytes,content->bytes,content->byteLength+1);
    }
    else
    {
        x->contents.INH_String.byteLength = 1;
        x->contents.bIndex = 0;
        x->contents.bSize = 1;
        i_rfLMS_Push(x->contents.INH_String.bytes,1);
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
       i_rfLMS_Push(attrName,sizeof(RF_String));
       i_rfLMS_Push(attrValue,sizeof(RF_String));
       if(!rfString_Copy_IN(attrName,attrNameTMP))
       {
           return NULL;
       }
       if(!rfString_Copy_IN(attrValue,attrValueTMP))
       {
           return NULL;
       }

       *(x->attributes.data+x->attributes.size) = attrName;
       x->attributes.size++;
       *(x->attribValues.data+x->attribValues.size) = attrValue;
       x->attribValues.size++;
       //give the value of the pointer's memory location to the appropriate member of the list
    }
    va_end(argList);
    return x;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
//Initializes an XML Tag
char rfXMLTag_Init3(RF_XMLTag* x,RF_XMLTag* parent,void* name)
{
    char ret = true;
    RF_ENTER_LOCAL_SCOPE()

    x->parent = parent;
    if(!rfString_Copy_IN(&x->name,name))
    {
        ret = false;
        goto cleanup;
    }
    //initialize the lists
    rfListP_Init(&x->children,RF_XMLTag,5,&rfXMLTag_Destroy,&rfXMLTag_Copy_OUT);//initial capacity of 5 children
    rfListP_Init(&x->attributes,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attributes
    rfListP_Init(&x->attribValues,RF_String,5,&rfString_Destroy,&rfString_Copy_OUT);//initial capacity of 5 attribute values
    //initialize the contents string
    rfStringX_Init_buff(&x->contents,128,"");//initial capacity of 128 bytes for the content string
    //if the parent exists add this child to it
    if(parent!=0)
        rfXMLTag_AddTag_nocopy(parent,x);

  cleanup:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}
#endif

//Initializes an XML Tag
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfXMLTag_Init(RF_XMLTag* x,RF_XMLTag* parent,void* name,void* content,unsigned char attrN,...)
#else
char rfXMLTag_Initv(RF_XMLTag* x,RF_XMLTag* parent,void* name,void* content,unsigned char attrN,...)
#endif
{
    uint32_t i;
    va_list argList;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE()

    x->parent = parent;
    if(!rfString_Copy_IN(&x->name,name))
    {
        ret = false;
        goto cleanup;
    }
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
    va_start(argList,attrN);
    for(i = 0; i < attrN; i ++)
    {
       RF_String* attrName  = va_arg(argList,RF_String*);
       RF_String* attrValue = va_arg(argList,RF_String*);
       rfXMLTag_AddAttribute(x,attrName,attrValue);
    }
    va_end(argList);

  cleanup:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

// Copies XML Tag @c src to XML Tag @c dst
char rfXMLTag_Copy_IN(RF_XMLTag* dst,RF_XMLTag* src)
{
    dst->parent = src->parent;
    if(rfString_Copy_IN(&dst->name,&src->name) &&
       rfListP_Copy_IN(&dst->children,&src->children) &&
       rfListP_Copy_IN(&dst->attributes,&src->attributes) &&
       rfListP_Copy_IN(&dst->attribValues,&src->attribValues) &&
       rfStringX_Copy_IN(&dst->contents,&src->contents) )
    {
        return true;
    }
    return false;
}
//Allocates and returns a copy of the given XML Tag
RF_XMLTag* rfXMLTag_Copy_OUT(RF_XMLTag* src)
{
    RF_XMLTag* dst;
    RF_MALLOC(dst,sizeof(RF_XMLTag), NULL);
    dst->parent = src->parent;
    if(rfString_Copy_IN(&dst->name,&src->name) &&
       rfListP_Copy_IN(&dst->children,&src->children) &&
       rfListP_Copy_IN(&dst->attributes,&src->attributes) &&
       rfListP_Copy_IN(&dst->attribValues,&src->attribValues) &&
       rfStringX_Copy_IN(&dst->contents,&src->contents) )
    {
        return dst;
    }
    return NULL;
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
void rfXMLTag_AddAttribute(RF_XMLTag* t,void* attribName,void* attribValue)
{
    RF_ENTER_LOCAL_SCOPE()
    RF_String* name = rfString_Copy_OUT(attribName);
    RF_String* value = rfString_Copy_OUT(attribValue);

    rfListP_Add(&t->attributes,name);
    rfListP_Add(&t->attribValues,value);

    RF_EXIT_LOCAL_SCOPE()
}

//Adds content to the tag
char rfXMLTag_AddContent(RF_XMLTag* t, void* c)
{
    RF_String* content;
    char ret = true;
    RF_ENTER_LOCAL_SCOPE()
    content = rfString_Copy_OUT(c);
    if( !content || 
        !rfStringX_Append(&t->contents, content))
    {
        ret = false;
    }
    RF_EXIT_LOCAL_SCOPE()
    return ret;
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
    RF_String* fromList = rfListP_Get(&t->attributes, i);
    if(fromList == 0 || !rfString_Copy_IN(name, fromList))
        return false;
    return true;
}

//Gets the ith attribute value of this tag
char rfXMLTag_GetAttributeValue(RF_XMLTag* t,uint32_t i,RF_String* value)
{
    RF_String* fromList = rfListP_Get(&t->attribValues, i);
    if(fromList == 0 || rfString_Copy_IN(value,fromList))
        return false;
    return true;
}

//Gets the ith line of content of the tag
char rfXMLTag_GetContent(RF_XMLTag* t,RF_String* line)
{
    return rfString_Copy_IN(line,&t->contents);
}


/**
 ** Runs through the tree recursively printing the tags
 ** @param t The XML tag that we recursively print
 ** @param f The textfile in which we print
 ** @param strBuff An already initialized RF_StringX buffer
 ** @param level The level of the tag in the xml tree. Is used for the /t tabs
 ** @return Returns true for succesful writting of the @c t XML tag and false otherwise
 **
 **/
char rfXMLTag_ToStr_internal(RF_XMLTag* t,RF_StringX* strBuff,uint32_t level)
{
    uint32_t i;
    //prepend the required number of tabs
    for(i = 0; i < level; i ++)
    {
        if(rfStringX_Append(strBuff,RFS_("\t")) == false)
        {
            return false;
        }
    }
    //create the tag string
    if(rfStringX_Append(strBuff,RFS_("<%S",&t->name)) == false)
    {
        return false;
    }
    for(i = 0; i < t->attributes.size; i ++)
    {
        if(rfStringX_Append(strBuff,
                            RFS_(" %S=\"%S\"",
                                 rfListP_Get(&t->attributes,i),
                                 rfListP_Get(&t->attribValues,i))) == false)
        {
            return false;
        }
    }
    if(!rfStringX_Append(strBuff, RFS_(">\n")))
    {
        return false;
    }
    //also print the tag's contents(if any)
    if(rfString_Equal(&t->contents,RFS_(""))!= true)
    {
        for(i= 0; i< level; i ++)
        {
            if(rfStringX_Append(strBuff, RFS_("\t")) == false)
            {
                return false;
            }
        }
        if(rfStringX_Append(strBuff, RFS_("%S\n", &t->contents)) == false)
        {
            return false;
        }
    }
    //recursively call this function for all the children of this tag
    for(i=0;i < t->children.size; i++)
    {
        if(rfXMLTag_ToStr_internal(
               (RF_XMLTag*)rfListP_Get(&t->children,i),
               strBuff,
               level+1) == false)
        {
            LOG_ERROR("Failed to convert tag's \"<%S>\" No. %d child to a"
                      " String", RE_XML_TOSTR, &t->name, i+1)
            return false;
        }
    }
    //when done write the closing of the tag
    //prepend the required number of tabs
    for(i = 0; i < level; i ++)
    {
        if(rfStringX_Append(strBuff,RFS_("\t")) == false)
        {
            return false;
        }
    }
    if(rfStringX_Append(strBuff,RFS_("</%S>\n",&t->name)) == false)
    {
        return false;
    }
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
