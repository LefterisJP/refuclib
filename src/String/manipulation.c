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

/*------------- Corrensponding Header inclusion -------------*/
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/manipulation.h>
/*------------- Module related inclusion -------------*/
#include <String/stringx_decl.h> //for RF_StringX
#include <String/corex.h> //for rfStringX_Deinit() and rfStringX_FromString_IN
#include <Utils/constcmp.h> //for RF_HEXEQ_C() used in the iteration macros
#include <String/core.h> //for string iterations
#include <String/retrieval.h> //for rfString_Length()
#include "common.ph" //for required string private macros and functions
#include "manipulation.ph" //for manipulation specific common macros
#include "mod.ph" //for the submod initialization
#include "defines.ph" //for some defines
/*------------- Outside Module inclusion -------------*/
#include <String/unicode.h> //for unicode functions
//for bool
    #include <Definitions/retcodes.h>
//for error logging macros
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for local scope macros
    #include <Utils/localscope.h>
//for sanity macros
    #include <Utils/sanity.h>
//for internal thread specific buffer
    #include "../Internal/internal_mod.ph"
/*------------- libc inclusion --------------*/
#include <limits.h> //for UINT_MAX e.t.c.
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/


/**
 * A function used to fill in a buffer with characters of a string.
 ** Returns number of unicode characters the array was filled with
*/
static inline int fill_codepoints_from_string(const RF_String* s)
{
    unsigned int i = 0;
    uint32_t charValue, chars_num;
    chars_num = rfString_Length(s);
    if(rfString_Length(s) > rfBuffer_Size_u32(TSBUFFA))
    {
        if(rfBuffer_Increase_u32(TSBUFFA, chars_num * 2))
        {
            return -1;
        }

    }
    rfString_Iterate_Start(s, i, charValue)
        rfBuffer_Ptr_u32(TSBUFFA, i) = charValue;
    rfString_Iterate_End(i)
    return chars_num;
}


// Appends the parameter String to this one
bool rfString_Append(RF_String* thisstr, const void* other)
{
    unsigned int newLen;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "append", return false);

    newLen = rfString_ByteLength(thisstr) + rfString_ByteLength(other);
    //reallocate this string to fit the new addition
    RF_REALLOC_JMP(rfString_Data(thisstr), char, newLen ,
                   ret = false, cleanup);
    rfStringGEN_Append(thisstr, rfString_Data(other), rfString_ByteLength(other));
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_Append_int(RF_String* thisstr, const int32_t i)
{  
    int rc;
    //reallocate this string to fit the new addition
    RF_REALLOC(rfString_Data(thisstr), char,
               rfString_ByteLength(thisstr) + MAX_UINT32_STRING_CHAR_SIZE ,
               false
    );
    ADD_TYPE_TO_STRING(thisstr, "int", MAX_UINT32_STRING_CHAR_SIZE,
                       "%i", rc, i);
    return true;
}

bool rfString_Append_double(RF_String* thisstr, const double d)
{
    int rc;
    RF_REALLOC(rfString_Data(thisstr), char,
                   rfString_ByteLength(thisstr) + MAX_DOUBLE_STRING_CHAR_SIZE ,
                   false
    );
    ADD_TYPE_TO_STRING(thisstr, "double", MAX_DOUBLE_STRING_CHAR_SIZE,
                       "%f", rc, d);
    return true;
}

bool rfString_Prepend(RF_String* thisstr, const void* other)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "prepend", ret=false;goto cleanup);

    //reallocate this string to fit the new addition
    RF_REALLOC_JMP(rfString_Data(thisstr), char,
                   rfString_ByteLength(thisstr) + rfString_ByteLength(other),
                   ret = false, cleanup);

    if(!rfStringGEN_Prepend(thisstr,
                            rfString_Data(other),
                            rfString_ByteLength(thisstr),
                            rfString_ByteLength(other)))
    {
        ret = false;
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

#ifndef RF_OPTION_DEFAULT_ARGUMENTS
bool rfString_Remove(void* thisstr, const void* rstr, uint32_t number,
                     const char options)
#else
bool i_rfString_Remove(void* thisstr, const void* rstr, uint32_t number,
                       const char options)
#endif
{
    uint32_t i,count,occurences=0;
    int32_t bytePos;
    char found = false, ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(rstr, "remove", ret=false;goto cleanup);

    //as long as we keep finding rstr in the string keep removing it
    do
    {   
        bytePos = rfString_FindBytePos(thisstr, rstr, options);
        //if the substring is not found
        if(bytePos == RF_FAILURE)
        {
            //if we have not even found it once , we fail
            if(!found)
            {
                ret = false;
                goto cleanup;
            }
            else 
            {
                //else we are done
                break;
            }
        }
        //substring found
        found = true;
        //move all of the string a position back
        count = 0;
        for(i = bytePos; i <= rfString_ByteLength(thisstr); i ++)
        {
            rfString_Data(thisstr)[i] = (
                rfString_Data(thisstr)[i + rfString_ByteLength(rstr)]
            );
            count++;
        }
        //now change the byte length
        rfString_ByteLength(thisstr) -= rfString_ByteLength(rstr);
        //count number of occurences, if we reached the required, stop
        occurences++;
        if(occurences == number)
        {
            break;
        }
    }while(bytePos != RF_FAILURE);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes all of the characters of the string except those specified
bool rfString_KeepOnly(void* thisstr, const void* keepstr)
{
    uint32_t keepLength,i, j, charValue;
    char unused[MAX_UTF8C_BYTES];
    char exists, charBLength;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();

    i_NULLPTR_CHECK_1(keepstr, "keep", ret=false;goto cleanup);
    //get all characters of the string in the array
    keepLength = fill_codepoints_from_string(keepstr);
    if(keepLength < 0 ) {return false;}

    //now iterate every character of this string
    i=0;
    rfString_Iterate_Start(thisstr, i, charValue)
        //for every character check if it exists in the keep str
        exists = false;
        for(j = 0; j < keepLength; j++)
        {
            if(rfBuffer_Ptr_u32(TSBUFFA, j) == charValue)
            {
                exists = true;
            }
        }
        //if it does not exist, move the string back to cover it so that it
        // effectively gets deleted
        if(!exists)
        {
            if((charBLength = rfUTF8_Encode_single(charValue, unused)) < 0)
            {
                RF_ERROR("Could not decode a codepoint into UTF-8");
                ret = false;
                goto cleanup;
            }
            //this is kind of a dirty way to do it. the rfString_Iterate_Start macro internally uses a byteIndex_ variable
            //we use that here to determine the current byteIndex_ of the string in the iteration and move the string backwards
            memmove(
                rfString_Data(thisstr) + byteIndex_,
                rfString_Data(thisstr) + byteIndex_ + charBLength,
                rfString_ByteLength(thisstr) - byteIndex_);
            rfString_ByteLength(thisstr) -= charBLength;
            continue;//by contiuing here we make sure that the current string position won't be moved to assure that we also check the newly move characters
        }
    rfString_Iterate_End(i)

cleanup:

    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_PruneStart(void* thisstr, uint32_t n)
{
    //iterate the characters of the string
    uint32_t i;
    uint32_t length = 0;
    unsigned nBytePos = 0;
    char found = false;
    RF_STRING_ITERATE_START(thisstr, length, i);
        //if we reach the number of characters passed as a parameter, note it
        if(length == n)
        {
            //remember that now i is the byte position we need
            nBytePos = i;
            found = true;
            break;
        }
    RF_STRING_ITERATE_END(length, i)

    //if the string does not have n chars to remove it becomes an empty string
    // and we return failure
    if(!found)
    {
        rfString_ByteLength(thisstr) = 0;
        return false;
    }

    //move the string back to cover the empty places
    for(i = 0; i < rfString_ByteLength(thisstr) - nBytePos; i++)
    {
        rfString_Data(thisstr)[i] = rfString_Data(thisstr)[i + nBytePos];
    }
    //get the new bytelength
    rfString_ByteLength(thisstr) -= nBytePos;

    return true;
}

bool rfString_PruneEnd(void* thisstr, uint32_t n)
{
    //start the iteration of the characters from the end of the string
    int32_t nBytePos = -1;
    uint32_t length,i;
    RF_STRING_ITERATEB_START(thisstr, length, i)
        //if we found the requested number of characters from the end
        if(length == n)
        {   //i is the byte position we need
            nBytePos = i;
            break;
        }
    RF_STRING_ITERATEB_END(length, i)

    //if the string does not have n chars to remove, fail
    if(nBytePos == -1)
    {
        rfString_Data(thisstr)[0] = '\0';
        return false;
    }

    //jut set the new byte length
    rfString_ByteLength(thisstr) -= (rfString_ByteLength(thisstr) - nBytePos);
    return true;
}

bool rfString_PruneMiddleB(void* thisstr, uint32_t p, uint32_t n)
{
    uint32_t i,length;
    int32_t pBytePos,nBytePos;
    //if we ask to remove more characters than possible
    if(n > p + 1)
    {
        return false;
    }
    //iterate the characters of the string

    pBytePos = nBytePos = -1;
    RF_STRING_ITERATE_START(thisstr, length, i)
        //if we reach the number of characters passed as a parameter, note it
        if(length == p +1 )
        {
            //we search for p+1  because we want to include all of the p character
            pBytePos = i;
            //also break since we don't care after position p
            break;
        }
        if(length == p - n + 1)//+1 is to make sure that indexing works from 0
            nBytePos = i;
    RF_STRING_ITERATE_END(length, i)

    //if the position was not found in the string do nothing
    if(pBytePos == -1 || nBytePos == -1)
    {
        return false;
    }
    /*
     *move the contents of the string to cover the removed
     *characters and change its length
     */
    memmove(
        rfString_Data(thisstr) + nBytePos,
        rfString_Data(thisstr) + pBytePos,
        rfString_ByteLength(thisstr) - pBytePos + 1);
    rfString_ByteLength(thisstr) -= (pBytePos - nBytePos);
    return true;
}

bool rfString_PruneMiddleF(void* thisstr, uint32_t p, uint32_t n)
{
    uint32_t j,i,length;
    int32_t pBytePos,nBytePos;

    //iterate the characters of the string
    pBytePos = nBytePos = -1;
    RF_STRING_ITERATE_START(thisstr, length, i)
        //if we reach the number of characters passed as a parameter, note it
        if(length == p)
        {
            pBytePos = i;
        }

        if(length == p + n)
        {
            nBytePos = i;
            break;//since we got all the data we needed
        }
    RF_STRING_ITERATE_END(length, i)

    //if the position was not found in the string do nothing
    if(pBytePos == -1)
    {
        return false;
    }

    /*
     * if we did not find the byte position of p+n then we remove
     * everything from pBytePos until the end of the string
     */
    if(nBytePos == -1)
    {
        rfString_ByteLength(thisstr) -= (
            rfString_ByteLength(thisstr) - pBytePos
        );
        return true;
    }

    //move the bytes in the buffer to remove the requested characters
    for(i=pBytePos, j=0;
        j<= rfString_ByteLength(thisstr) - nBytePos;
        i ++,j++) 
    {
        rfString_Data(thisstr)[i] = rfString_Data(thisstr)[nBytePos + j];
    }
    //find the new byte length
    rfString_ByteLength(thisstr) -= (nBytePos - pBytePos);
    return true;
}

bool rfString_TrimStart(void* thisstr, const void* sub)
{
    bool ret = false, noMatch;
    uint32_t i = 0, j, subLength, bytePos;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(sub, "substring", ret=false;goto cleanup);

    //get all the codepoints of the string
    subLength = fill_codepoints_from_string(sub);
    if(subLength < 0)
    {
        return false;
    }

    //iterate thisstring from the beginning
    i = 0;
    RF_STRING_ITERATE_START(thisstr,i,bytePos)
        noMatch = true;
        //for every substring character
        for(j = 0; j < subLength; j++)
        {
            //if we got a match
            if(rfString_BytePosToCodePoint(thisstr, bytePos) == 
               rfBuffer_Ptr_u32(TSBUFFA, j))
            {
                ret = true;
                noMatch = false;
                break;
            }
        }
        if(noMatch)
        {
            break;
        }
    RF_STRING_ITERATE_END(i, bytePos)

    //if we had any match
    if(ret)
    {
        //remove the characters
        for(i =0; i < rfString_ByteLength(thisstr) - bytePos; i++ )
        {
            rfString_Data(thisstr)[i] = rfString_Data(thisstr)[i + bytePos];
        }
        //also change bytelength
        rfString_ByteLength(thisstr) -= bytePos;
    }

#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION //only used for malloc fail
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_TrimEnd(void* thisstr, const void* sub)
{
    char ret = false,noMatch;
    uint32_t i = 0,j,subLength,bytePos,lastBytePos=0;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(sub, "substring", ret=false;goto cleanup);

    //get all the codepoints of the string
    subLength = fill_codepoints_from_string(sub);
    if(subLength < 0)
    {
        return false;
    }

    //iterate thisstring from the end
    i = 0;
    RF_STRING_ITERATEB_START(thisstr, i, bytePos)
        noMatch = true;
        //for every substring character
        for(j = 0; j < subLength; j++)
        {
            //if we got a match
            if(rfString_BytePosToCodePoint(thisstr, bytePos) == 
               rfBuffer_Ptr_u32(TSBUFFA, j))
            {
                ret = true;
                noMatch = false;
                lastBytePos = bytePos;
                break;
            }
        }
        if(noMatch)
        {
            break;
        }
    RF_STRING_ITERATEB_END(i, bytePos)

    //if we had any match
    if(ret)
    {
        rfString_ByteLength(thisstr) -= (
            rfString_ByteLength(thisstr) - lastBytePos
        );
    }

#ifdef RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfString_Trim(void* thisstrP, const void* subP)
{
    RF_ENTER_LOCAL_SCOPE();
    char res1 = rfString_TrimStart(thisstrP,subP);
    char res2 = rfString_TrimEnd(thisstrP,subP);
    RF_EXIT_LOCAL_SCOPE();
    return res1|res2;
}


#ifndef RF_OPTION_DEFAULT_ARGUMENTS
bool rfString_Replace(RF_String* thisstr, const void* sstr,
                      const void* rstr, const uint32_t num,
                      const char options)
#else
bool i_rfString_Replace(RF_String* thisstr, const void* sstr,
                        const void* rstr, const uint32_t num,
                        const char options)
#endif
{
    //will keep the number of given instances to find
    uint32_t number = num;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_2(sstr, rstr, ret=false;goto cleanup);

    if(!replace_intro(thisstr, &number, sstr, options))
    {
        ret = false;
        goto cleanup;
    }

    //act depending on the size difference of rstr and sstr
    if(rfString_ByteLength(rstr)> rfString_ByteLength(sstr))
    {
        RF_REALLOC_JMP(
            rfString_Data(thisstr), char,
            rfString_ByteLength(thisstr) + number * (
                rfString_ByteLength(rstr) - rfString_ByteLength(sstr)
            ),
            ret = false, cleanup
        );
        replace_greater(thisstr, number, sstr, rstr);
    }
    else if(rfString_ByteLength(rstr) < rfString_ByteLength(sstr))
    {
        replace_lesser(thisstr, number, sstr, rstr);
    }
    else //replace and remove strings are equal
    {
        replace_equal(thisstr, number, rstr);
    }
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


/* module related inline object insertion*/

i_INLINE_INS bool rfStringGEN_Prepend(void* thisstr, const char* other,
                                      unsigned int orig_size,
                                      unsigned int other_size);
i_INLINE_INS bool replace_intro(void* s, uint32_t* number, const void* sstr,
                                const char options);
i_INLINE_INS void replace_greater(void* s, uint32_t number,
                                  const void* sstr, const void* rstr);
i_INLINE_INS void replace_lesser(void* s, uint32_t number,
                                 const void* sstr, const void* rstr);
i_INLINE_INS void replace_equal(void* s, uint32_t number, const void* rstr);
