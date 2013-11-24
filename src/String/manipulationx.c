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
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //for enabling default arguments
#include <String/manipulationx.h>
/*------------- Module related inclusion -------------*/
#include <String/flags.h> //for the flags
#include <String/core.h> //for rfString_Deinit
#include <String/retrieval.h> //for rfString_Between and others
#include <String/traversalx.h> //for rfStringX_MoveAfter() and other functions
#include "common.ph" //for private string functions and macros
#include "manipulation.ph" //for manipulation only stuff
/*------------- Outside Module inclusion -------------*/
#include <String/unicode.h> //for unicode functions
//for the integer int and uint conversions
    #include <Definitions/inline.h> //for the inline declaration
    #include <Definitions/retcodes.h> //for return codes and true/false definition
    #include "../Numeric/Integer/rf_int.ph" //for the integer int and uint conversions
//for the float conversions
    #include "../Numeric/Float/rf_float.ph" //for doubleToStr()
//for error logging macros
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for RF_HEXEQ_C() and other macros
    #include <Utils/constcmp.h> 
//for local scope macros
    #include <Utils/localscope.h>//for local scope macros
//for sanity macros
    #include <Utils/sanity.h>
/*------------- End of includes -------------*/

bool rfStringX_Append(RF_StringX* thisstr, const void* other)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "append", ret = false;goto cleanup);

    ret = rfStringXGEN_Append(thisstr, rfString_Data(other),
                              rfString_ByteLength(other));

#ifdef RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfStringX_Append_chars(RF_StringX* thisstr, const void* other, int chars)
{
    uint32_t i;
    uint32_t length = 0;
    unsigned end = 0;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "append", ret = false; goto cleanup);

    //find out the bytelength of the requested char position
    RF_STRING_ITERATE_START(other, length, i);
        if(length == chars)
        {
            end = i;
            break;
        }
    RF_STRING_ITERATE_END(length, i)
    //if the end is not found
    if(!end)
    {
        end = rfString_ByteLength(other);
    }
    ret = rfStringXGEN_Append(thisstr, rfString_Data(other), end);

#ifdef RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfStringX_Append_char(RF_StringX* thisstr, uint32_t unichar)
{
    char utf8[MAX_UTF8C_BYTES];
    int length;
    if((length = rfUTF8_Encode_single(unichar,utf8)) <= 0)
    {
        RF_ERROR("Encoding the given unicode codepoint to UTF8 failed");
        return false;
    }

    return rfStringXGEN_Append(thisstr, utf8, length);
}

bool rfStringX_Prepend(RF_StringX* thisstr, const void* other)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "prepend", ret=false;goto cleanup);

    //if the new string does not fit inside the buffer reallocate it
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        rfString_ByteLength(thisstr) + rfString_ByteLength(other),
        ret = false,
        cleanup
    );

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

bool rfStringX_Insert(RF_StringX* thisstr, uint32_t pos,
                      const void* other)
{
    uint32_t length, bytePos, size, i;
    bool found = false, ret = false;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "insert", goto cleanup);


    //keep the original byte length here
    size = rfString_ByteLength(thisstr);
    //get the new byte length
    rfString_ByteLength(thisstr) += rfString_ByteLength(other);
    //check if the new string fits in the buffer and if not reallocate it
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        rfString_ByteLength(thisstr),
        ;,
        cleanup);
    //iterate this string to find the byte position of the character position
    RF_STRING_ITERATE_START(thisstr, length, i)
        if(length == pos)
        {
            //found the position. Is saved in bytePos
            bytePos = i;
            found = true;
            break;
        }
    RF_STRING_ITERATE_END(length, i);
    //if the position is found in the string then insert
    if(found)
    {
        ret = true;
        //move the string's contents to make room for the extra string insertion
        memmove(
            rfString_Data(thisstr) + rfString_ByteLength(other) + bytePos,
            rfString_Data(thisstr) + bytePos, size - bytePos);
        //now insert the new string
        memcpy(rfString_Data(thisstr) + bytePos,
               rfString_Data(other),
               rfString_ByteLength(other));
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

/*--- RF_StringX unsafe appending functions---*/

bool rfStringX_Append_bytes(RF_StringX* thisstr, const void* other,
                            const int32_t bytes)
{
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(other, "append_bytes", ret=false;goto cleanup);

    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC_JMP(
        thisstr,
        rfString_ByteLength(thisstr) + bytes,
        ret = false,
        cleanup);
    ret = rfStringXGEN_Append(thisstr, rfString_Data(other), bytes);

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rfStringX_Append_cstr(RF_StringX* thisstr, const char* cstr)
{
    size_t len = strlen(cstr);
    i_NULLPTR_CHECK_1(other, "append_cstr", ret=false;goto cleanup);
    //if it does not fit inside the remaining size, reallocate the buffer
    RF_STRINGX_REALLOC(
        thisstr,
        rfString_ByteLength(thisstr) + len,
        false);
    
    return rfStringXGEN_Append(thisstr, cstr, len);
}

/*--- RF_StringX replacing functions---*/

#ifndef RF_OPTION_DEFAULT_ARGUMENTS
bool rfStringX_Replace(RF_StringX* thisstr, const void* sstr,
                       const void* rstr, uint32_t num,
                       const char options)
#else
bool i_rfStringX_Replace(RF_StringX* thisstr, const void* sstr,
                         const void* rstr, uint32_t num,
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
        //reallocate the string if needed
        RF_STRINGX_REALLOC_JMP(
            thisstr,
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

#ifndef RF_OPTION_DEFAULT_ARGUMENTS
bool rfStringX_ReplaceBetween(RF_StringX* thisstr, const void* left,
                              const void* right, const void* rstr,
                              char options, uint32_t i)
#else
bool i_rfStringX_ReplaceBetween(RF_StringX* thisstr, const void* left,
                                const void* right, const void* rstr,
                                char options, uint32_t i)
#endif
{
    uint32_t j,move,start = thisstr->bIndex;
    bool found = false,ret=false;
    RF_String ss;
    RF_ENTER_LOCAL_SCOPE();

    i_NULLPTR_CHECK_3(left, right, rstr, ret=false; goto cleanup);

    if(i==0)//if we want all occurences replaced
    {
        //while we have occurences of the pair
        while(!rfString_Between(thisstr, left, right, &ss, options))
        {
            found = true;
            /*
             * move the internal pointer right after the left part of the pair
             * and replace what was found there. No of the functions
             * below fail since the while condition is true
             */
            rfStringX_MoveAfter(thisstr, left, 0, options);
            if(rfStringX_Replace(thisstr, &ss, rstr, 1, options))
            {
                goto cleanup1;
            }
            //also move after the right
            move = rfString_ByteLength(rstr) + rfString_ByteLength(right);
            thisstr->bIndex += move;
            rfString_Data(thisstr) += move;
            rfString_ByteLength(thisstr) -= move;
            //free temp string before next iteration
            rfString_Deinit(&ss);
        }

        if(found)//if we replaced at least 1 occurence
        {
            move = thisstr->bIndex - start;
            rfString_Data(thisstr) -= move;
            thisstr->bIndex = start;
            rfString_ByteLength(thisstr) += move;
            //success
            ret = true;
        }
        goto cleanup1;
    }

    ///if we check for a specific instance
    for(j = 1; j < i; j++)
    {
        //move after the pair of the 'j' inbetween substrings
        if(!rfStringX_MoveAfterPair(thisstr, left, right, 0, options))
        {
            //and if the occurence does not exist
            move = thisstr->bIndex - start;
            rfString_Data(thisstr) -= move;
            thisstr->bIndex = start;
            //and increase its bytelength
            rfString_ByteLength(thisstr) += move;
            goto cleanup1;//falure
        }
    }
    //move after the pair of the inbetween substrings we actually want
    if(!rfString_Between(thisstr, left, right, &ss, options))
    {
        //and if the occurence does not exist
        move = thisstr->bIndex-start;
        rfString_Data(thisstr) -= move;
        thisstr->bIndex = start;
        //and increase its bytelength
        rfString_ByteLength(thisstr) += move;
        goto cleanup1;//falure
    }
    ///success
    //move after the left part of the pair
    rfStringX_MoveAfter(thisstr, left, 0, options);
    //and then replace the occurence
    if(!rfStringX_Replace(thisstr, &ss, rstr, 1, options))
    {
        //failure
        goto cleanup1;
    }
    //now we are done and should go back
    rfString_Deinit(&ss);
    move = thisstr->bIndex - start;
    rfString_Data(thisstr) -= move;
    thisstr->bIndex = start;
    rfString_ByteLength(thisstr) += move;

    //if we get here success
    ret = true;
cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}




i_INLINE_INS bool rfStringXGEN_Append(RF_StringX* s,
                                      const char* other,
                                      unsigned int bytes_to_copy);
