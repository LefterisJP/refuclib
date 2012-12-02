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
**
**
**
** -- Numeric/Integer/rf_int.ph
** This file contains some functions that deal with integer
** numbers and are attempting to replace the clib equivalents
**
** It's a private header since there is no need to expose this functionality to the user as
** things currently stand plus, if exposed to the user, the inlining would have to go away
** since Dynamic libraries can't have inline functions if I am not mistaken.
*/
#ifndef RF_NUMERIC_INT_PH
#define RF_NUMERIC_INT_PH

#include <rf_setup.h> //for int64_t and inline declaration

//xor swapping
#define XOR_SWAP(X,Y)\
{\
    X^=Y;\
    Y^=X;\
    X^=Y;\
}\

i_INLINE_DECL int intToStr(int64_t i,char* buff)
{
    int64_t num = i;
    char hasSign=0;
    int j = 0,k=0;
    if(i<0)
    {
        num*=-1;
        buff[0] = '-';
        j=1;
        k=1;
        hasSign = true;
    }
    //turn the string into a number
    while(num != 0)
    {
        buff[j] = (num%10)+48;
        num = num/10;
        j++;
    }
    buff[j] = '\0';//null terminate it

    for(;k<j/2+hasSign;k++)
        XOR_SWAP(buff[k],buff[j-k-1+hasSign])

    return j;
}

i_INLINE_DECL int uintToStr(uint64_t i,char* buff)
{
    uint64_t num = i;
    int j = 0,k=0;
    //turn the string into a number
    while(num != 0)
    {
        buff[j] = (num%10)+48;
        num = num/10;
        j++;
    }
    for(;k<j/2;k++)
        XOR_SWAP(buff[k],buff[j-k-1])
    buff[j] ='\0';
    return j;
}

static char hexU [] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' ,'A', 'B', 'C', 'D', 'E', 'F' };
static char hexL [] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' ,'a', 'b', 'c', 'd', 'e', 'f' };

i_INLINE_DECL int uintToUHexStr(uint64_t i,char* buff)
{
    int len=0,k=0;
    do
    {
        buff[len] = hexU[i&0xF];
        len++;
        i>>=4;
    }while(i!=0);
    for(;k<len/2;k++)
        XOR_SWAP(buff[k],buff[len-k-1])
    buff[len]='\0';
    return len;
}

i_INLINE_DECL int uintToLHexStr(uint64_t i,char* buff)
{
    int len=0,k=0;
    do
    {
        buff[len] = hexL[i&0xF];
        len++;
        i>>=4;
    }while(i!=0);
    for(;k<len/2;k++)
        XOR_SWAP(buff[k],buff[len-k-1])
    buff[len]='\0';
    return len;
}


#endif//include guards end
