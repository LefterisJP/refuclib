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

#ifndef RFC_XML_H
#define RFC_XML_H

#include <rf_options.h>
#ifdef RF_MODULE_DF_XML //XML module check
#include <Data_Formats/rfc_xmltag.h>
#include <IO/rfc_textfile.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

///XML handler opening  flags
//For opening an existing XML file but all operations to it will be done in memory
#define XML_OPEN_IN_MEMORY  0x1
//For opening an existing XML file but all operations to it will be done directly in disk
#define XML_OPEN_IN_DISK    0x2
//For creating a new XML file but all operations to it will be done in memory
#define XML_NEW_IN_MEMORY   0x4
//For creating a new XML file but all operations to it will be done directly in disk
#define XML_NEW_IN_DISK     0x8
//If this is a new XML file specify if the indentation will be done by tabs or spaces. If this is given then it's spaces, if not it's tabs
#define XML_SPACES          0x10 ///DO NOT CHANGE THIS VALUE. SHOULD BE 0X10  since this is its value also in the internal flags

/**
** @internal
** @date 10/04/2011
** @author Lefteris
** @endinternal
** @brief This the handle of an XML file
**
** The XML struct acts as the handle for an XML file. You can
** load all of the file in memory to be read with the XML
** related functions
*/
typedef struct RF_XML
{
    //! The version of the xml file
    float version;
    //! A counter for the number of root children searched by any search done in the data. Used in resuming search from where it stopped
    int32_t rootChildrenSearched;
    //! Help count the root children searched
    int32_t rcs;
    //! The root tag of the xml data file
    RF_XMLTag root;
    //! A pointer to the current tag being parsed. Used by the parsing function
    RF_XMLTag* currentTag;
    //! A pointer to the last tag returned by a function
    RF_XMLTag* lastTag;
    //! @brief XML bitflag properties, 8 bits
    //!
    //! For various xml file handler properties
    //! + Bit 0: @b ON: Root initialized @b OFF: root not initialized
    //! + Bit 1: @b ON: Search is done   @b OFF: search is not done
    //! + Bit 2: @b ON: File read in memory @b OFF: File not read in memory
    //! + Bit 3: @b ON: During parsing no children flag raised @b OFF: not raised
    char flags;
    //! The xml text file handlers
    RF_TextFile f;
    //! The position of the root in the file
    foff_rft rootPos;
    //! The text buffer string to be used while parsing
    RF_StringX s;
    //! The last line that a tag was found, or moved to. Is used to figure out where to insert tags in the file
    uint64_t lastLine;
    //! The current indentation level during parsing
    uint32_t level;
}RF_XML;

//! @name XML Handler creation
//! @{

//! @memberof RF_XML
//! @brief Creates a new XML handler
//!
//! Allocates and returns a new XML file handler.
//! @lmsFunction
//! @param filename A string with the name of the file to parse @inhtype{String,StringX} @tmpSTR
//! @param openFlag Opening bitflags indicating what type of XML file to create. Provide here one of:
//! + @c XML_OPEN_IN_MEMORY: For opening an existing XML file but all operations to it will be done in memory
//! + @c XML_OPEN_IN_DISK: For opening an existing XML file but all operations to it will be done directly in disk
//! + @c XML_NEW_IN_MEMORY: For creating a new XML file but all operations to it will be done in memory
//! + @c XML_NEW_IN_DISK: For creating a new XML file but all operations to it will be done directly in disk
//! and optionally add (with logical OR "|") the flag @c XML_SPACES if this is a new file and if you want
//! 3 spaces to be used for indentation. If not, then tabs will be used.
//! @param encoding The encoding of the file. Can be one of @c RF_UTF8, @c RF_UTF16 and @c RF_UTF32
//! @return An XML file handler or a null pointer in case of an error while the error itself is logged
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ RF_XML* rfXML_Create(void* filename,char openFlag,char encoding);
#else
i_DECLIMEX_ RF_XML* i_rfXML_Create(void* filename,char* openFlag,char* encoding);
#define rfXML_Create(i_FNAME_,i_OPENFLAG_,i_ENCODING_)  i_rfLMS_WRAP3(RF_XML*,i_rfXML_Create,i_FNAME_,i_RFI8_(i_OPENFLAG_),i_RFI8_(i_ENCODING_))
#endif
//! @memberof RF_XML
//! @brief Initializes an XML Handler
//!
//! Initializes an XML file handler according to the given arguments
//! @lmsFunction
//! @param xml The xml file handler to initialize
//! @param filename A string with the name of the file to parse @inhtype{String,StringX} @tmpSTR
//! @param openFlag Opening bitflags indicating what type of XML file to create. Provide here one of:
//! + @c XML_OPEN_IN_MEMORY: For opening an existing XML file but all operations to it will be done in memory
//! + @c XML_OPEN_IN_DISK: For opening an existing XML file but all operations to it will be done directly in disk
//! + @c XML_NEW_IN_MEMORY: For creating a new XML file but all operations to it will be done in memory
//! + @c XML_NEW_IN_DISK: For creating a new XML file but all operations to it will be done directly in disk
//! and optionally add (with logical OR "|") the flag @c XML_SPACES if this is a new file and if you want
//! 3 spaces to be used for indentation. If not, then tabs will be used.
//! @param encoding The encoding of the file. Can be one of @c RF_UTF8, @c RF_UTF16 and @c RF_UTF32
//! @return Returns RF_SUCCESS for success and an error code along with error logging in case of error.
//! The possible error values are any of those returnded by @ref rfTextFile_GetOffset() and also:
//! + @c RE_XML_OPEN_FAILURE: If the existing file failed to open or a new file failed to be created
//! + @c RE_INPUT: If an illegal value was given at @c openFlag
//! + @c RE_XML_STRING_FREAD: If at any point in the file a string could not be read succesfully.
//! + @c RE_XML_READLINE: If at any point in the file reading proccess a string could not be read from the file because the file is corrupt
//! + @c RE_XML_UTF_ENCODING: When a String is read and the unicode encoding does not match the expected encoding. For this function this means if it is not a proper UTF-8 encoding
//! + @c RE_XML_OPEN_TAG: If at any point in the file an expected tag opening could not be found. For more information look at the error log if this error occurs.
//! + @c RE_XML_PARSE_FAILURE: Generic parsing failure. We were expecting something from the xml file and it was not found. Such as the version tag.
//! + @c RE_XML_UNEXPECTED_EOF: If at some point in the file the End Of File is encountered prematurely. For more information and for where in the file the unexpected EOF was found check the error log
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfXML_Init(RF_XML* xml,void* filename,char openFlag,char encoding);
#else
i_DECLIMEX_ int32_t i_rfXML_Init(RF_XML* xml,void* filename,char* openFlag,char* encoding);
#define rfXML_Init(i_XML_,i_FNAME_,i_OPENFLAG_,i_ENCODING_)  i_rfLMSX_WRAP4(int32_t,i_rfXML_Init,i_XML_,i_FNAME_,i_RFI8_(i_OPENFLAG_),i_RFI8_(i_ENCODING_))
#endif



//! @}

//! @name Destruction functions
//! @{

//! @memberof RF_XML
//! @brief Deletes an XML file handler
//!
//! Use it to get rid of a handler made with _Create
//! @param x The xml file handler to destroy
i_DECLIMEX_ void rfXML_Destroy(RF_XML* x);
//! @memberof RF_XML
//! @brief Deinitializes an XML file handler
//!
//! Use it to get rid of a handler made with _Init
//! @param x The xml file handler to deinitialize
i_DECLIMEX_ void rfXML_Deinit(RF_XML* x);

//! @}

//! @name Outputting
//! @{

//! @memberof RF_XML
//! @brief Outputs the XML file from memory to a file
//!
//! Prints the XML file that is currently in memory to a file. Should only be used if the
//! XML file has been loaded in memory.
//! If the XML file is not actually loaded in memory then the function
//! returns @c RF_FAILURE
//!
//! Note that any filename given at the @c name argument shall be created if it does not exist and if it does
//! exist then all its contents shall be overwritten
//! @lmsFunction
//! @param x The xml handler
//! @param name \rfoptional{same as the one at initialization} A string with the name of the output file.
//! If this parameter is omitted then the name of the output file will be the same as the one
//! given at the initial parsing, in effect putting back the XML in memory in the same file it was read from.
//! A temporary file is created until the writting to file is succesful so the data is not lost if something goes awry
//! @inhtype{String,StringX} @tmpSTR
//! @param encoding \rfoptional{RF_UTF8} The encoding of the output file. Can be one of @c RF_UTF8, @c RF_UTF16 and @c RF_UTF32. Can be ommitted and the default value is @c RF_UTF8
//! @return Returs either @c RF_SUCCESS for succesfull outputting of the XML tree the a file with @c name or one of the following errors:
//! + @c RF_FAILURE: If the XML file was not loaded in memory
//! + @c RE_XML_OPEN_FAILURE: If there was a problem opening the output file that we want to place the XML tree into
//! + @c RE_XML_WRITE: If there was a problem during writting to the output file. For more information if you get this error, check the error log.
//! + @c RE_FILE_DELETE: If no name has been given and the previous name is used, at some point during the outputting the previous file is deleted. This
//!      error means that the deleting failed.
//! + @c RE_FILE_RENAME: If no name has been given and the previous name is used, at some point during the outputting the a temporary file
//!      is renamed as the originally parsed file. This error means that the renaming failed
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfXML_ToFile(RF_XML* x, void* name,char encoding);
#else
i_DECLIMEX_ int32_t i_rfXML_ToFile(RF_XML* x, void* name,char* encoding);
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfXML_ToFile(...)   RF_SELECT_FUNC_IF_NARGGT(i_VSELECT_RF_XML_TOFILE,3,__VA_ARGS__)
        #define i_VSELECT_RF_XML_TOFILE1(...)   RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_ToFile() accepts from 1 to 3 arguments\"")
        #define i_VSELECT_RF_XML_TOFILE0(...)   RF_SELECT_FUNC(i_SELECT_RF_XML_TOFILE,__VA_ARGS__)
        #define i_SELECT_RF_XML_TOFILE3(i_XML_,i_NAME_,i_ENCODING_) i_rfLMSX_WRAP3(int32_t,i_rfXML_ToFile,i_XML_,i_NAME_,i_RFI8_(i_ENCODING_))
        #define i_SELECT_RF_XML_TOFILE2(i_XML_,i_NAME_)             i_rfLMSX_WRAP3(int32_t,i_rfXML_ToFile,i_XML_,i_NAME_,i_RFI8_(RF_UTF8))
        #define i_SELECT_RF_XML_TOFILE1(i_XML_)                     i_rfLMSX_WRAP3(int32_t,i_rfXML_ToFile,i_XML_,0,i_RFI8_(RF_UTF8))
        #define i_SELECT_RF_XML_TOFILE0(...)    RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfTextFile_ToFile() accepts from 1 to 3 arguments\"")
    #else
        #define rfXML_ToFile(i_XML_,i_NAME_,i_ENCODING_)    i_rfLMSX_WRAP3(int32_t,i_rfXML_ToFile,i_XML_,i_NAME_,i_RFI8_(i_ENCODING_))
    #endif
#endif

#define XML_TAG_END 0x1
#define XML_GO  0x2
//! @memberof RF_XML
//! @brief Inserts an XML tag in the XML file
//!
//! This function inserts tag @c t at the current file position.
//!
//! This function is intended to work with an XML file that has been initialized and opened in disk
//! with the whole file not having been loaded in memory. If
//! instead it is called on a handler in memory then @c RF_FAILURE will be returned.
//! @lmsFunction
//! @param x The xml file handler in which to insert the tag
//! @param t The tag to insert. @tmpXML
//! @param flags Bitflags indicating some options about the insertion. Possible values can be a combination of the below:
//! + @c XML_TAG_END: If so given then tag @c t is inserted at the end of the last tag. If this flag is not provided
//! it is inserted in the start
//! + @c XML_GO: If so given then the current file pointer is also moved to the start of the inserted tag and the last
//! tag is considered to be the inserted tag. If not then the last tag and the file pointer stay where they were
//! @return Returns @c RF_SUCCCESS for succesfull insertion or:
//! + @c RE_XML_TOSTR: For failure at representing XML Tag @c t as a string
//! Also this function can return any of the errors returned by @ref rfXML_InsertStr()
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfXML_InsertTag(RF_XML* x, RF_XMLTag* t,char flags);
#else
i_DECLIMEX_ int32_t i_rfXML_InsertTag(RF_XML* x, RF_XMLTag* t,char* flags);
#define rfXML_InsertTag(i_XML_,i_TAG_,i_FLAGS_) i_rfLMSX_WRAP3(int32_t,i_rfXML_InsertTag,i_XML_,i_TAG_,i_RFI8_(i_FLAGS_))
#endif

//! @memberof RF_XML
//! @brief Inserts a string in the XML file
//!
//! This function inserts string @c s at the current file position
//!
//! It is the caller's responsibility to assure that the string is a proper
//! xml file snippet and makes sense being inserted into the current file position
//! For a safer function take a look at: @ref rfXML_Insert_tag().
//!
//! This function is intended to work with an XML file that has been initialized and opened in disk
//! with the whole file not having been loaded in memory. If
//! instead it is called on a handler in memory then @c RF_FAILURE will be returned.
//! @lmsFunction
//! @param x The xml file handler in which to insert the string
//! @param s A string to enter into the XML file. @inhtype{String,StringX} @tmpSTR
//! + @c XML_TAG_END: If so given then tag @c t is inserted at the end of the last tag. If this flag is not provided
//! it is inserted in the start
//! + @c XML_GO: If so given then the current file pointer is also moved to the start of the inserted tag and the last
//! tag is considered to be the inserted tag. If not then the last tag and the file pointer stay where they were
//! @return Returns either @c RF_SUCCESS for succesful insertion to the file or one of the following:
//! + @c RF_FAILURE: If the xml handler @c x is not opened with @ref rfXML_Init_dsk but is instead loaded in memory
//! + @c RE_XML_NOFILEOP: If no operation has been done to the file itself so the file pointer has not been moved at all or if the file is only in memory
//! + @c RE_XML_OPEN_TAG: The opening of the expected XML tag was not found in the XML line where the insertion should happen
//! + @c RE_XML_PARSE_FAILURE: If the current xml file line is not properly formed as it should be, so no insertion can happen
//! Also this function can return any of the errors returned by the function @ref rfTextFile_Replace()
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ int32_t rfXML_InsertStr(RF_XML* x, void* s,char flags);
#else
i_DECLIMEX_ int32_t i_rfXML_InsertStr(RF_XML* x, void* s,char* flags);
#define rfXML_InsertStr(i_XML_,i_STR_,i_FLAGS_) i_rfLMSX_WRAP3(int32_t,i_rfXML_InsertStr,i_XML_,i_STR_,i_RFI8_(i_FLAGS_))
#endif


//! @memberof RF_XML
//! @brief Adds the root tag in an XML file initialized in memory
//!
//! This function is used to add the root tag in an XML file initialized in memory and not for any
//! other type of XML file handler. If the root tag is already initialized this function shall return
//! @c false.
//! @lmsFunction
//! @param x The xml handle for the operation
//! @param t The tag to add. @tmpXML
//! @return Returns @c true for succesfull root insertion or @c false if the root was already initialized or this
//! is not a handler in memory
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_ char rfXML_AddRoot(RF_XML* x,RF_XMLTag* t);
#else
i_DECLIMEX_ char i_rfXML_AddRoot(RF_XML* x,RF_XMLTag* t);
#define rfXML_AddRoot(i_XML_,i_TAG_)    i_rfLMSX_WRAP2(char,i_rfXML_AddRoot,i_XML_,i_TAG_)
#endif

//! @memberof RF_XML
//! @brief Provides a pointer to the XML handle's root
//!
//! The returned pointer is a pointer to the actual root of the XML tree
//! so no modification should be attempted to it without copying.
//! @param x The xml handle for the operation
//! @return Returns a pointer to the root tag of the XML tree if succesful and 0 if
//! no root has so far been initialized. Do not attempt to free this pointer, since it's a
//! pointer to the actual root of the xml file
i_DECLIMEX_ const RF_XMLTag* rfXML_GetRoot(RF_XML* x);

//! @}
//outputting functions end

//! @name Tag Retrieval
//! @{

//! @memberof RF_XML
//! @brief Searches the xml file data for a given tag
//!
//! Scans the xml file either recursively if it is in memory or by reading all the lines of the disk file if not,
//! searching for the first tag matching the given name.
//! Also if additional data such as attributes or content is also given the search takes them also into account.
//!
//! This function starts searching from the beginning of the file if the file handler is only in disk and from the
//! root of the file tree if it is in memory.
//!
//! By omitting all arguments after @c contents at function call a more lightweight version of the function is called
//! If the library is compiled with no default arguments on
//! then this function can be called in a limited fashion where there is only one
//! version of the function called under the hood.
//! @rfNoDefArgsWarn1
//! @warning IF the library is compiled with @c DEFAULT_ARGUMENTS off then the argument @c attrN is
//! actually a pointer to a @c uint32_t
//! @lmsFunction
//! @param x The XML handler
//! @param tName The tag name of the tag we need.  @inhtype{String,StringX} @tmpSTR
//! @param contents \rfoptional{0} The contents we want search for in the tag.
//! The tag contents are not searched for as an exact match but are instead searched to see if they exists inside
//! the tag. Can also be @c 0 which means that no specific content is going to be searched.
//! By omitting all arguments after this at function call a much more lightweight version of the function is called @inhtype{String,StringX} @tmpSTR
//! @param attrN \rfoptional{0} The number of attributes to pass to the function, default is 0 which means no attributes should be passed.
//! The maximum number of extra arguments that can be given is @varFuncLimit and that means that this number divided by 2
//! is the number of attributes this function can accept.
//! @param ... \rfoptional{nothing} The name and values of attributes to follow. Depending on the number given at
//! @c attrN the function expects @c attrN pairs of attributes and values here. @inhtype{String,StringX} @tmpSTR
//! @extraVarArgLim
//! @return The tag object requested or 0 if it is not found.
//! If this is called when the handler has the file loaded in memory this will be a pointer to a Tag in the tree in
//! memory so it should not be freed. If on the other hand it is on the disk this is a newly allocated tag that is returned
//! just to show to the user that the Tag was found, so in that case freeing should be performed after use with @ref rfXMLTag_Destroy()
#if defined(RF_IAMHERE_FOR_DOXYGEN)
i_DECLIMEX_ RF_XMLTag* rfXML_GetTag(RF_XML* x,void* tName,void* contents, uint32_t attrN, ...);
#else
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        i_DECLIMEX_ RF_XMLTag* i_rfXML_GetTag(RF_XML* x,void* tName,void* contents, uint32_t* attrN, ...);
        i_DECLIMEX_ RF_XMLTag* i_NVrfXML_GetTag(RF_XML* x,void* tName,void* contents);
        #define rfXML_GetTag(...)   RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_XML_GETTAG,4,__VA_ARGS__)
        #define i_NPSELECT_RF_XML_GETTAG1(...) RF_SELECT_FUNC_IF_NARGGT2(i_LIMSELECT_RF_XML_GETTAG,18,__VA_ARGS__)
        #define i_NPSELECT_RF_XML_GETTAG0(...) RF_SELECT_FUNC(i_SELECT_RF_XML_GETTAG,__VA_ARGS__)
        #define i_LIMSELECT_RF_XML_GETTAG1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received more extra arguments than the limit permits\"")
        #define i_LIMSELECT_RF_XML_GETTAG0(...) RF_SELECT_FUNC(i_SELECT_RF_XML_GETTAG,__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG18(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) i_rfLMS_WRAP18(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG17(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received odd number of extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG16(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) i_rfLMS_WRAP16(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG15(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received odd number of extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG14(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) i_rfLMS_WRAP14(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG13(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received odd number of extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG12(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) i_rfLMS_WRAP12(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG11(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received odd number of extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG10(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) i_rfLMS_WRAP10(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG9(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received odd number of extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG8(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) i_rfLMS_WRAP8(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG7(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received odd number of extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG6(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
             i_rfLMS_WRAP6(RF_XMLTag*,i_rfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG5(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() received 1 extra argument. Only even number can be given \"")
        #define i_SELECT_RF_XML_GETTAG4(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfXML_GetTag() accepts from 2 to N arguments. But if 4 arguments are given then at least 1 more must follow\"")
        #define i_SELECT_RF_XML_GETTAG3(i_THISTAG_,i_TAGNAME_,i_CONTENTS_) i_rfLMS_WRAP3(RF_XMLTag*,i_NVrfXML_GetTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_)
        #define i_SELECT_RF_XML_GETTAG2(i_THISTAG_,i_TAGNAME_) i_rfLMS_WRAP3(RF_XMLTag*,i_NVrfXML_GetTag,i_THISTAG_,i_TAGNAME_,0)
        #define i_SELECT_RF_XML_GETTAG1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() accepts from 2 to N arguments.\"")
        #define i_SELECT_RF_XML_GETTAG0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag() accepts from 2 to N arguments.\"")
    #else
        i_DECLIMEX_ RF_XMLTag* rfXML_GetTag(RF_XML* x,void* tName,void* contents, uint32_t* attrN, ...);
    #endif
#endif

//! @memberof RF_XML
//! @brief Searches the XML file in disk for a given tag and moves to it
//!
//! Scans the xml file by reading all the lines of the disk file. If the file is not only on the disk
//! and is loaded in memory it returns @c RF_FAILURE.
//! searching for the first tag matching the given name.
//! Also if additional data such as attributes or content is also given the search takes them also into account.
//!
//! This function starts searching from the beginning of the file.
//!
//! @rfNoDefArgsWarn1
//! @warning IF the library is compiled with @c DEFAULT_ARGUMENTS off then the argument @c attrN is
//! actually a pointer to a @c uint32_t
//! @lmsFunction
//! @param x The XML handler
//! @param tName The tag name of the tag we need.  @inhtype{String,StringX} @tmpSTR
//! @param contents \rfoptional{0} The contents we want search for in the tag.
//! The tag contents are not searched for as an exact match but are instead searched to see if they exists inside
//! the tag. Can also be @c 0 which means that no specific content is going to be searched.
//! By omitting all arguments after this at function call a much more lightweight version of the function is called @inhtype{String,StringX} @tmpSTR
//! @param attrN \rfoptional{0} The number of attributes to pass to the function, default is 0 which means no attributes should be passed.
//! The maximum number of extra arguments that can be given is @varFuncLimit and that means that this number divided by 2
//! is the number of attributes this function can accept.
//! @param ... \rfoptional{nothing} The name and values of attributes to follow. Depending on the number given at
//! @c attrN the function expects @c attrN pairs of attributes and values here. @inhtype{String,StringX} @tmpSTR
//! @extraVarArgLim
//! @return Returns @c RF_SUCCESS for succesfull moving of the file pointer to the given tag and error otherwise.
//! + @c RF_FAILURE: If the handler has the XML file open only in disk
//! + @c RE_XML_EOF: If while searchign for the given tag the End of file was reached
//! + All the errors that @ref fTextFile_GoToOffset() and @ref fTextFile_ReadLine() can return
#if defined(RF_IAMHERE_FOR_DOXYGEN)
i_DECLIMEX_ int32_t rfXML_GoToTag(RF_XML* x,void* tName,void* contents, uint32_t attrN, ...);
#else
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        i_DECLIMEX_ int32_t i_rfXML_GoToTag(RF_XML* x,void* tName,void* contents, uint32_t* attrN, ...);
        #define rfXML_GoToTag(...)   RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_XML_GOTOTAG,4,__VA_ARGS__)
        #define i_NPSELECT_RF_XML_GOTOTAG1(...) RF_SELECT_FUNC_IF_NARGGT2(i_LIMSELECT_RF_XML_GOTOTAG,8,__VA_ARGS__)
        #define i_NPSELECT_RF_XML_GOTOTAG0(...) RF_SELECT_FUNC(i_SELECT_RF_XML_GOTOTAG,__VA_ARGS__)
        #define i_LIMSELECT_RF_XML_GOTOTAG1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GoToTag() received more extra arguments than the limit permits\"")
        #define i_LIMSELECT_RF_XML_GOTOTAG0(...) RF_SELECT_FUNC(i_SELECT_RF_XML_GOTOTAG,__VA_ARGS__)
        #define i_SELECT_RF_XML_GOTOTAG8(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
            i_rfLMSX_WRAP8(int32_t,i_rfXML_GoToTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GOTOTAG7(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GoToTag() received 3 extra arguments. Only even number can be given \"")
        #define i_SELECT_RF_XML_GOTOTAG6(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) \
             i_rfLMSX_WRAP6(int32_t,i_rfXML_GoToTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(i_ATTRN_),__VA_ARGS__)
        #define i_SELECT_RF_XML_GOTOTAG5(i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_ATTRN_,...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GoToTag() received 1 extra argument. Only even number can be given \"")
        #define i_SELECT_RF_XML_GOTOTAG4(...) RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfXML_GoToTag() accepts from 2 to N arguments. But if 4 arguments are given then at least 1 more must follow\"")
        #define i_SELECT_RF_XML_GOTOTAG3(i_THISTAG_,i_TAGNAME_,i_CONTENTS_) i_rfLMSX_WRAP4(int32_t,i_rfXML_GoToTag,i_THISTAG_,i_TAGNAME_,i_CONTENTS_,i_RFUI32_(0))
        #define i_SELECT_RF_XML_GOTOTAG2(i_THISTAG_,i_TAGNAME_) i_rfLMSX_WRAP4(int32_t,i_rfXML_GoToTag,i_THISTAG_,i_TAGNAME_,0,i_RFUI32_(0))
        #define i_SELECT_RF_XML_GOTOTAG1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GoToTag() accepts from 2 to N arguments.\"")
        #define i_SELECT_RF_XML_GOTOTAG0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GoToTag() accepts from 2 to N arguments.\"")
    #else
        i_DECLIMEX_ int32_t rfXML_GoToTag(RF_XML* x,void* tName,void* contents, uint32_t* attrN, ...);
    #endif
#endif


//! @memberof RF_XML
//! @brief Searches the xml data for a tag by name and child name
//!
//! Scans the xml tree recursively searching for the first tag matching @c tName having a @c childName child somewhere in its children
//! This can only work for the XML file in memory.
//! @lmsFunction
//! @param x The XML handler
//! @param tName The tag name of the tag we need. @inhtype{String,StringX} @tmpSTR
//! @param childName The name of a tag we require for the tag to have in its children. @inhtype{String,StringX} @tmpSTR
//! @param direct \rfoptional{true} Boolean flag denoting whether childName neeeds to be direct child (= @c true) or anywhere in its children(= @c false)
//! @param rParent \rfoptional{false} Boolean flag denoting whether we want to return the parent or the child. Default is @c true, meaning the parent.
//! @return A pointer to the tag object requested, or 0 if the tag can not be found or if the XML handle is not loaded in memory
#if defined(RF_IAMHERE_FOR_DOXYGEN)
i_DECLIMEX_ RF_XMLTag* rfXML_GetTag_child(RF_XML* x,void* tName,void* childName,char direct,char rParent);
#else
i_DECLIMEX_ RF_XMLTag* i_rfXML_GetTag_child(RF_XML* x,void* tName,void* childName,char* direct,char* rParent);
    #ifdef RF_OPTION_DEFAULT_ARGUMENTS
        #define rfXML_GetTag_child(...) RF_SELECT_FUNC_IF_NARGGT(i_VSELECT_RF_XML_GETTAG_CHILD,5,__VA_ARGS__)
        #define i_VSELECT_RF_XML_GETTAG_CHILD1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag_child() accepts from 3 to 5 arguments\"")
        #define i_VSELECT_RF_XML_GETTAG_CHILD0(...) RF_SELECT_FUNC(i_SELECT_RF_XML_GETTAG_CHILD,__VA_ARGS__)
        #define i_SELECT_RF_XML_GETTAG_CHILD5(i_XML_,i_NAME_,i_CHILD_,i_DIRECT_,i_RPARENT_) \
            i_rfLMS_WRAP5(RF_XMLTag*,i_rfXML_GetTag_child,i_XML_,i_NAME_,i_CHILD_,i_RFI8_(i_DIRECT_),i_RFI8_(i_RPARENT_))
        #define i_SELECT_RF_XML_GETTAG_CHILD4(i_XML_,i_NAME_,i_CHILD_,i_DIRECT_) \
            i_rfLMS_WRAP5(RF_XMLTag*,i_rfXML_GetTag_child,i_XML_,i_NAME_,i_CHILD_,i_RFI8_(i_DIRECT_),i_RFI8_(0))
        #define i_SELECT_RF_XML_GETTAG_CHILD3(i_XML_,i_NAME_,i_CHILD_) \
            i_rfLMS_WRAP5(RF_XMLTag*,i_rfXML_GetTag_child,i_XML_,i_NAME_,i_CHILD_,i_RFI8_(0),i_RFI8_(0))
        #define i_SELECT_RF_XML_GETTAG_CHILD2(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag_child() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_XML_GETTAG_CHILD1(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag_child() accepts from 3 to 5 arguments\"")
        #define i_SELECT_RF_XML_GETTAG_CHILD0(...) RF_COMPILE_ERROR("message \"Illegal Arguments Number: Function rfXML_GetTag_child() accepts from 3 to 5 arguments\"")
    #else
        #define rfXML_GetTag_child(i_XML_,i_NAME_,i_CHILD_,i_DIRECT_,i_RPARENT_) \
             i_rfLMS_WRAP5(RF_XMLTag*,i_rfXML_GetTag_child,i_XML_,i_NAME_,i_CHILD_,i_RFI8_(i_DIRECT_),i_RFI8_(i_RPARENT_))
    #endif
#endif

//! @}
//tag retrieval functions end


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#else
    #error Attempted to include Refu XML module without the proper module flag on. If you need to use it build the library with that option.
#endif//XML module check stop

#endif//include guards end
