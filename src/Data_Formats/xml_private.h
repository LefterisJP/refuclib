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
** This is a private header defining only functions
** and macros that are private and only accessible
** to the XML functions
**/



#include <rf_setup.h>

// --------------------- //
//      XML Bitflags    //
// -------------------- //
//! Root is initialized or not
#define XML_ROOT_INIT       0x1
//! Denotes if search is complete or not
#define XML_SEARCH_DONE     0x2
//! Denotes if the whole file has been read in memory or not
#define XML_IN_MEMORY       0x4
//! Denotes that there are no more children to go into
#define XML_NOCHILDREN      0x8
//! Already defined in xml.h , if it changes there it should also make sure to not replace any of the flags here
//#define XML_SPACES        0x10

enum parserStates { TAG_TITLE=1,TAG_ATTRIBUTES,TAG_CONTENTS, TAG_CLOSING};








///=== Private XML macros ===///

//! This is a macro that just reads an xml file line and does error checking and error handling.
//! Is to be used ONLY INSIDE rfXML_Init. Could as well be a function but the macro works fine and has been tested thoroughly. No need to add the overhead of function calling
//! @param i_XML_ The xml handler (pointer)
//! @param i_ERRORSTR The error string to prepend to the error message
//! @param i_BUFF_ The string buffer to keep the read lines (pointer)
#define RF_XML_READLINE(i_XML_,i_ERRORSTR,i_BUFF_)   {\
        /*If there is an error in line reading*/\
        if( (error = rfTextFile_ReadLine2(&(i_XML_)->f,(i_BUFF_))) != RF_SUCCESS)\
        {\
            if(error == RE_FILE_EOF)\
            {\
                LOG_ERROR(i_ERRORSTR" because premature End of File was found",RE_XML_UNEXPECTED_EOF)\
                rfStringX_Deinit((i_BUFF_));\
                return RE_XML_UNEXPECTED_EOF;\
            }\
            LOG_ERROR(i_ERRORSTR" because of error in Line Reading",RE_XML_READLINE)\
            rfStringX_Deinit((i_BUFF_));\
            return RE_XML_READLINE;\
        }}

//! This is a version of RF_XML_READLINE() macro that goes to cleanup instead of handling errors itself
#define RF_XML_READLINE_GOTO(i_XML_,i_ERRORSTR,i_BUFF_,i_ERROR_,i_GOTO_)   {\
        /*If there is an error in line reading*/\
        if( (i_ERROR_ = rfTextFile_ReadLine2(&(i_XML_)->f,(i_BUFF_))) != RF_SUCCESS)\
        {\
            if(i_ERROR_ == RE_FILE_EOF)\
            {\
                LOG_ERROR(i_ERRORSTR" because premature End of File was found",RE_XML_UNEXPECTED_EOF)\
                i_ERROR_ = RE_XML_UNEXPECTED_EOF;\
                goto i_GOTO_;\
            }\
            LOG_ERROR(i_ERRORSTR" because of error in Line Reading",RE_XML_READLINE)\
            i_ERROR_ = RE_XML_READLINE;\
            goto i_GOTO_;\
        }}

//! This is a macro that just reads an xml file line and does error checking and error handling but does not check for eof.
//! Is to be used ONLY INSIDE rfXML_Init. Could as well be a function but the macro works fine and has been tested thoroughly. No need to add the overhead of function calling
//! @param i_XML_ The xml handler (pointer)
//! @param i_ERRORSTR The error string to prepend to the error message
//! @param i_BUFF_ The string buffer to keep the read lines (pointer)
#define RF_XML_READLINE_NO_EOFCHECK(i_XML_,i_ERRORSTR,i_BUFF_)   {\
        /*If there is an error in line reading*/\
        if( (error = rfTextFile_ReadLine2(&(i_XML_)->f,(i_BUFF_))) != RF_SUCCESS)\
        {\
            LOG_ERROR(i_ERRORSTR" because of error in Line Reading",RE_XML_READLINE)\
            rfStringX_Deinit((i_BUFF_));\
            return RE_XML_READLINE;\
        }}
//! This is a version of the RF_XML_READLINE_NO_EOFCHECK macro that does not do its own error handling but instead
//! used goto to jump to a cleanup section
#define RF_XML_READLINE_NO_EOFCHECK_GOTO(i_XML_,i_ERRORSTR,i_BUFF_,i_ERROR_,i_GOTO_)   {\
        /*If there is an error in line reading*/\
        if( (i_ERROR_ = rfTextFile_ReadLine2(&(i_XML_)->f,(i_BUFF_))) != RF_SUCCESS)\
        {\
            LOG_ERROR(i_ERRORSTR" because of error in Line Reading",RE_XML_READLINE)\
            i_ERROR_ = RE_XML_READLINE;\
            goto i_GOTO_;\
        }}

///=== Private XML functions ===///

struct RF_XML;
struct RF_XMLTag;
struct RF_String;
struct RF_StringX;
struct RF_TextFile;

//! Runs through the xml tree searching for a tag recursively by name and/or contents
//! @param x The XML file handler
//! @param t The currently searching tag
//! @param tName The name of the tag we are searching for
//! @param contents The contents of the tag we are searching for. Can be zero.
//! @return Returns the tag if it was found or NULL if not
struct RF_XMLTag* rfXML_SearchSmall(struct RF_XML* x,struct RF_XMLTag* t,void* tName,void* contents);

//! Runs through the xml tree searching for a tag recursively by name ,attributes and contents
//! @param t The currently searching tag
//! @param tName The name of the tag we are searching for
//! @param contents The contents we want the tag to have
//! @param attrN The number of attributes
//! @return Returns the tag if it was found or NULL if not
struct RF_XMLTag* rfXML_SearchFull(struct RF_XML* x,struct RF_XMLTag* t,void* tNameP,void* contentsP,uint32_t attrN,struct RF_String** sAttribs,struct RF_String** sAttribValues);


//! Runs through the tree searching for a tag tName with a child childName somewhere in its children
//! @param x The XML handler
//! @param t The currently searching tag
//! @param tName The name of the tag we are searching for
//! @param childName The name of the child tag we want the tag we search for to have
//! @param direct Boolean flag denoting whether childName neeeds to be direct child (=true) or anywhere in its childre(=false)
//! @param rParent Boolean flag denoting whether we want to return the parent or the child.
//! @return Returns the tag if it was found or NULL if not
struct RF_XMLTag* i_rfXML_SearchChild(struct RF_XML* x,struct RF_XMLTag* t,void* tNameP,void* childNameP,char direct,char rParent);


//! Runs through the tree recursively printing the tags
//! @param t The XML tag that we recursively print
//! @param f The textfile in which we print
//! @param strBuff An already initialize RF_StringX buffer
//! @param level The level of the tag in the xml tree. Is used for the /t tabs
//! @return Returns true for succesful writting of the @c t XML tag and false otherwise
char i_rfXMLTag_PrintToFile(struct RF_XMLTag* t,struct RF_TextFile* f,struct RF_StringX* strBuff,uint32_t level);


//internal full file parsing function, intended to be used only from the start of the file to parse the entirety of the file
int32_t i_rfXML_Parse(struct RF_XML* x,struct RF_XMLTag* currentTag);



//! @memberof RF_XML
//! @brief Moves to the next sibling tag in disk
//!
//! Retrieves the next sibling tag at the current position
//! of the file. If the position before the function call
//! is the root tag then this function shall return its first child.
//!
//! If no other tag exists as a sibling then the function will
//! return @c RF_FAILURE and position the file position at the next sibling of its parent
//!
//! Also if during failing to find a sibling the root tag closes then
//! the function will return @c RE_XML_EOF
//! @param[in] x The xml handler for which to move
//! @param[out] t \rfoptional{0} If the tag is found then you have the option to return it by passing
//! a pointer to an @ref RF_XMLTag to be initialized by the function. If this is 0 or omitted then
//! the function moves the file pointer to the tag but does not return it. If returned the user must free it by
//! calling @ref rfXMLTag_Deinit() on it later.
//! @return Returns RF_SUCCESS on success and otherwise it can return one of
//! + @c RF_FAILURE: If no other sibling tag exists
//! + @c RE_XML_EOF: If the root tag closed, so it's impossible to return another tag
//! + @c RE_XML_UNEXPECTED_EOF: If the end of file was unexpectedly encountered during parsing
//! + @c RE_XML_PARSE_FAILURE: If something unexpected was found during file parsing
int32_t rfXML_GoNext_dsk(struct RF_XML* x,struct RF_XMLTag* t);

//! @memberof RF_XML
//! @brief Moves to a child under the current tag in the file
//!
//! @param[in] x The xml handler for which to move
//! @param[in] i \rfoptional{0} The child number of the current
//! tag to move to. If omitted the first child is assumed. Indexing starts from 0.
//! @return Returns RF_SUCCESS on success and otherwise it can return one of
//! + @c RF_FAILURE: If no @c i child exists or if the parent closed. The file stays at the same position before the function call
//! + @c RE_XML_EOF: If the root tag closed, so it's impossible to return another tag
//! + @c RE_XML_UNEXPECTED_EOF: If the end of file was unexpectedly encountered during parsing
//! + @c RE_XML_PARSE_FAILURE: If something unexpected was found during file parsing
int32_t rfXML_GoIn_dsk(struct RF_XML* x,uint32_t i);

//! @memberof RF_XML
//! @brief Moves out of the current tag's position in the file
//!
//! Using this function you have a choice to move either at the beginning of the parent or at the beginning of the parent's next tag.
//! If the parent is the root and we attempt to use this function then basically it returns the file pointer to part of the file
//! in the beginning where the root tag opens
//!
//! If an error occurs then the function attempts to position the file pointer
//! to the same position it was before the function call. That is not guaranteed to work at all times.
//! @param[in] x The xml handler for which to move
//! @param[in] after A boolean value. If @c true then when going out of the current tag the file pointer is positioned after
//! its parent and if @c false, right at the beginning of its parent
//! @return Returns RF_SUCCESS on success and otherwise it can return one of:
//! + @c RF_FAILURE: If the current tag is the root
//! + @c RE_XML_EOF: If the root tag closed, so it's impossible to return another tag
//! + @c RE_XML_UNEXPECTED_EOF: If the end of file was unexpectedly encountered during parsing
//! + @c RE_XML_PARSE_FAILURE: If something unexpected was found during file parsing
int32_t rfXML_GoOut_dsk(struct RF_XML* x,char after);


