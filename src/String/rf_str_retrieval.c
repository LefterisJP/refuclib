/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <String/rf_str_retrieval.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_core.h>
#include <String/rf_str_unicode.h> //rf_utf8_is_continuation_byte()
#include <String/rf_str_corex.h> //for rf_stringx_init_unsafe()
#include "rf_str_common.ph" //for required string private macros and functions
/*------------- Outside Module inclusion -------------*/
#include <Definitions/retcodes.h> //for bool
#include <Utils/bits.h>  //for RF_BITFLAG_ON
#include <Utils/localscope.h> //for the local scope macros
#include <Utils/memory.h> //for refu memory allocation
 #include <Utils/sanity.h> //for sanity macros
/*------------- libc inclusion --------------*/
#include <stdarg.h>
#include <limits.h> //for INT_MAX e.t.c.
#include <stdio.h> // for sscanf
/*------------- End of includes -------------*/

// Finds the length of the string in characters
uint32_t rf_string_length(const void* str)
{
    uint32_t length,i;
    RF_STRING_ITERATE_START(str, length, i)
    RF_STRING_ITERATE_END(length, i);
    return length;
}

bool rf_string_get_char(const void* str, uint32_t c, uint32_t* cp)
{
    uint32_t length, i;
    RF_STRING_ITERATE_START(str, length, i)
        if(length == c)
        {
            *cp = rf_string_bytepos_to_codepoint(str, i);
            return true;
        }
    RF_STRING_ITERATE_END(length, i)

    return false;
}

bool rf_string_substr(const void* s, uint32_t startPos,
                     uint32_t charsN, struct RFstring* ret)
{
    uint32_t charI,byteI,startI,endI;
    bool started = false, ended = false;
    startI = endI = 0;
    RF_CHECK_NOT_NULL_DEBUG_2(s, ret, return false);

    RF_STRING_ITERATE_START(s, charI, byteI)
        if(charI == startPos)
        {
            startI = byteI;
            started = true;
        }
        if(charI == startPos+charsN)
        {
            endI = byteI;
            ended = true;
            break;
        }
    RF_STRING_ITERATE_END(charI, byteI)
    if(!ended)
    {
        //if the start was found return from there until the end of the string
        if(started)
        {
            endI = byteI;
        }
        else//else the substring was not found
        {
            return false;
        }
    }
    /* else success and init the return string */
    return rf_string_init_unsafe_nnt(ret,
                                    rf_string_data(s) + startI,
                                    endI - startI
    );
 
}


int32_t rf_string_find(const void* tstr, const void* sstr,
                       enum RFstring_matching_options options)
{
    int32_t ret;
    RF_ENTER_LOCAL_SCOPE();

    if((ret = rf_string_find_byte_pos(tstr, sstr, options)) == RF_FAILURE)
    {
        ret = RF_FAILURE;
        goto cleanup;
    }

    ret = rf_string_bytepos_to_charpos(tstr, ret, false);
  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


int32_t rf_string_find_i(const void* thisstr, const void* sstr,
                         uint32_t startPos, uint32_t length,
                         enum RFstring_matching_options options)
{
    struct RFstring sub;
    int32_t ret = RF_FAILURE;//the return value
    RF_ENTER_LOCAL_SCOPE();
    //if the substring does not exist fail
    if(!rf_string_substr(thisstr, startPos, length, &sub))
    {
        goto cleanup1;
    }

    //now search for the sstr substring inside the sub substring defined by length
    if((ret=rf_string_find(&sub, sstr, options)) != RF_FAILURE)
    {
        ret += startPos;//return the proper position in the original string
    }
    rf_string_deinit(&sub);//free the sub substring and return

cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

unsigned int rf_string_count(const void* tstr, const void* sstr,
                             enum RFstring_matching_options options)
{
    int move, index;
    unsigned int n;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_2(tstr, sstr, goto cleanup);

    move = index = n = 0;
    //as long as the substring is found in the string
    while((move = rf_string_find_byte_pos(
               tstr, sstr, options)) != RF_FAILURE)
    {
        move += rf_string_length_bytes(sstr);
        //proceed searching inside the string and also increase the counter
        n++;
        rf_string_data(tstr) += move;
        index += move;
        rf_string_length_bytes(tstr) -= move;
    }

    //return string to its original state
    rf_string_data(tstr) -= index;
    rf_string_length_bytes(tstr) += index;

#ifdef RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return n;
}


bool rf_string_scanf_after(const void* str, const void* astr,
                         const char* format, void* var)
{
    char *s;
    const char *found;
    bool ret = false;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_2(str, astr, goto cleanup);

    found = strstr_nnt(rf_string_data(str), rf_string_length_bytes(str),
                      rf_string_data(astr), rf_string_length_bytes(astr));
    if(!found)
    {
        goto cleanup;
    }
    //get a pointer to the start of the position where sscanf will be used
    s = rf_string_data(str) + (
        found - rf_string_data(str) + rf_string_length_bytes(astr)
    );

    //use sscanf
    if(sscanf(s, format, var) <= 0)
    {
        goto cleanup;
    }
    //success
    ret = true;
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


bool rf_string_between(const void* tstr, const void* lstr,
                       const void* rstr, void* result,
                       enum RFstring_matching_options options)
{
    int start, end;
    struct RFstring temp;
    bool ret = false;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_4(tstr, lstr, rstr, result, goto cleanup);

    //find the left substring
    if((start = rf_string_find_byte_pos(tstr, lstr, options)) == RF_FAILURE)
    {
        goto cleanup;
    }
    //get what is after it, no check since we already found it
    rf_string_after(tstr, lstr, &temp, options);
    //find the right substring in the remaining part
    if((end = rf_string_find_byte_pos(&temp, rstr, options)) == RF_FAILURE)
    {
        goto cleanup1;
    }

    //initialize the string to return
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rf_stringx_init_unsafe_nnt(
               result,
               rf_string_data(tstr) + start + rf_string_length_bytes(lstr),
               end))
        {
            goto cleanup1; 
        }
    }
    else
    {
        if(!rf_string_init_unsafe_nnt(
               result,
               rf_string_data(tstr) + start + rf_string_length_bytes(lstr),
               end))
        {
            goto cleanup1; 
        }
    }
    //success
    ret = true;


cleanup1:
    //free temp string
    rf_string_deinit(&temp);
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_beforev(const void* thisstr, void* result,
                       enum RFstring_matching_options options,
                       const unsigned char parN,  ...)
{
    const struct RFstring* s;
    int32_t i,minPos,thisPos;
    va_list argList;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_2(thisstr, result, ret = false; goto cleanup);

    //get the parameter characters
    va_start(argList, parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        s = (const struct RFstring*) va_arg(argList, struct RFstring*);
        if((thisPos= rf_string_find_byte_pos(thisstr, s, options))!= RF_FAILURE)
        {
            if(thisPos < minPos)      
            {
               minPos = thisPos;
            }
        }
    }
    va_end(argList);

    //if it is not found
    if(minPos == INT_MAX)
    {
        ret = false;
        goto cleanup;
    }
    //if it is found initialize the substring for return
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rf_stringx_init_unsafe_nnt(result, rf_string_data(thisstr), minPos))
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rf_string_init_unsafe_nnt(result, rf_string_data(thisstr), minPos))
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_before(const void* thisstr, const void* sstr,
                      void* result,
                      enum RFstring_matching_options options)
{
    int32_t rv;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_3(thisstr, sstr, result, ret = false; goto cleanup);

    //find the substring
    if((rv = rf_string_find_byte_pos(thisstr, sstr, options)) == RF_FAILURE)
    {
        ret = false;
        goto cleanup;
    }
    //if it is found initialize the substring for return
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rf_stringx_init_unsafe_nnt(result, rf_string_data(thisstr), rv))
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rf_string_init_unsafe_nnt(result, rf_string_data(thisstr), rv))
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

bool rf_string_after(const void* thisstr, const void* after,
                     void* result,
                     enum RFstring_matching_options options)
{
    int32_t bytePos;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_3(thisstr, after, result, ret = false;goto cleanup);

    //check for substring existence
    if((bytePos = rf_string_find_byte_pos(thisstr, after, options)) == RF_FAILURE)
    {
        ret = false;
        goto cleanup;
    }
    bytePos += rf_string_length_bytes(after);
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rf_stringx_init_unsafe_nnt(
               result,
               rf_string_data(thisstr) + bytePos,
               rf_string_length_bytes(thisstr) - bytePos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rf_string_init_unsafe_nnt(
               result,
               rf_string_data(thisstr) + bytePos,
               rf_string_length_bytes(thisstr) - bytePos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


bool rf_string_afterv(const void* thisstr, void* result,
                      enum RFstring_matching_options options,
                      const unsigned char parN, ...)
{
    const struct RFstring* s;
    int32_t i, minPos, thisPos;
    uint32_t minPosLength=0;
    //will keep the argument list
    va_list argList;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    RF_CHECK_NOT_NULL_DEBUG_2(thisstr, result, ret = false; goto cleanup);

    //get the parameter characters
    va_start(argList,parN);
    minPos = INT_MAX;
    for(i = 0; i < parN; i++)
    {
        s = (const struct RFstring*) va_arg(argList, struct RFstring*);
        if((thisPos= rf_string_find_byte_pos(thisstr, s, options))!= RF_FAILURE)
        {
            if(thisPos < minPos)
            {
                minPos = thisPos;
                minPosLength = rf_string_length_bytes(s);
            }
        }
    }
    va_end(argList);
    //if it is not found
    if(minPos == INT_MAX)
    {
        ret = false;
        goto cleanup;
    }
    //if it is found initialize the substring
    minPos += minPosLength;//go after the found substring
    if(options & RF_STRINGX_ARGUMENT)
    {
        if(!rf_stringx_init_unsafe_nnt(
               result,
               rf_string_data(thisstr) + minPos,
               rf_string_length_bytes(thisstr) - minPos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }
    else
    {
        if(!rf_string_init_unsafe_nnt(
               result,
               rf_string_data(thisstr) + minPos,
               rf_string_length_bytes(thisstr) - minPos)
           )
        {
            ret = false;
            goto cleanup; 
        }
    }

  cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}
