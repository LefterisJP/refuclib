/*
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
*/
#ifndef RF_XML_DECL_H
#define RF_XML_DECL_H

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

#endif //include guards end
