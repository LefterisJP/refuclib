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
**/
#ifndef RF_STRINGX_DECL_H
#define RF_STRINGX_DECL_H

#include <String/rf_str_decl.h>

/**
** @internal
** @date 16/05/2010
** @author Lefteris
** @endinternal
**
** @brief An extended version of the normal String which offers 
** a movable index and dynamic buffer allocation
**
** The Extended String is an extension over the simple @ref RFstring
** that adds functionalities that enable moving
** inside the String itself and allocating its size dynamically.
** This is accomplished by adding two more members to
** the structure:
**
**  <ul>
**  <li>A buffer index allowing operations of moving
**      inside the String to happen</li>
**  <li>A buffer size which keeps the size of the buffer allocated
**      for the string<
**  </li>
**  </ul>
**
** Whenever the size of this String gets over the buffer size, it gets increased.
** Some functions even decrease the size allocated for the String buffer.
**
** This String version has functions that extend it 
** greatly especially for the purposes
** of parsing files/streams and editable text. This String is not intended for being
** used en mass, saved in arrays/vectors e.t.c.
** For that use the original @ref RFstring
**
** As far as function calling is concerned if an extended version
** of the function exists (starting with rfStringX)
** then you should use that so that the string's members get updated correctly.
** If not then just using the normal
** function with an extended string as a parameter will suffice.
** All the normal String functions which have 
** <em class="rf_ability1">Can be used with StringX</em> on their description
**  can be used with extended strings safely, since no specific
** version of the function exists, or needs to exist to manipulate Extended Strings.
**
** @see RFstring
** @inherit RFstring
**/
struct RFstringx {
    //! The RFstring inherited members
    struct RFstring INH_String;
    //! The buffer index, denotes how far from the start of the buffer the start of the string has moved
    uint32_t bIndex;
    //! The size of the buffer allocated for this extended String in bytes.
    uint32_t bSize;
};



#endif //include guards end
