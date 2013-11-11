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
#include <Utils/endianess.h>
/*------------- Outside Module inclusion -------------*/
    #ifdef REFU_LINUX_VERSION
    #include <time.h> //for clockid_t used in the System info structure
    #endif
#include "../System/info.ph" //for the System info private struct
/*------------- End of includes -------------*/


//A function that returns the endianess of the system
char rfEndianess()
{
    return rfSysInfo.endianess;
}

char rfOtherEndianess()
{
    if(rfSysInfo.endianess == RF_LITTLE_ENDIAN)
    {
        return RF_BIG_ENDIAN;
    }
    return RF_LITTLE_ENDIAN;
}

// Swaps the endianness of the variable
void rfSwapEndianUS(uint16_t* v)
{
    *v = (*v>>8)|(*v<<8);
}

//Swaps the endianness of the variable
void rfSwapEndianS(int16_t* v)
{
    *v = ((*v&0xff00)>> 8) |((*v&0x00ff)<<8);
}

// Swaps the endianness of the variable
void rfSwapEndianUI(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}
// Swaps the endianness of the variable
void rfSwapEndianI(int32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}

// Swaps the endianness of the variable
void rfSwapEndianUL(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}


void rfProcessByteOrderUI(uint32_t* v, int file_endianess)
{
    /* if(file_endianess != rfEndianess()) */
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        rfSwapEndianUI(v);
    }
}

void rfProcessByteOrderUS(uint16_t* v, int file_endianess)
{
    /* if(file_endianess != rfEndianess()) */
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        rfSwapEndianUS(v);
    }
}
void rfProcessByteOrderUSA(uint16_t* v, unsigned int length,
                           int file_endianess)
{
    unsigned int i;
    /* if(file_endianess != rfEndianess()) */
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        for(i = 0; i < length; i ++)
        {
            rfSwapEndianUS(&v[i]);
        }
    }
}

void rfProcessByteOrderUIA(uint32_t* v, unsigned int length,
                           int file_endianess)
{
    unsigned int i;
    /* if(file_endianess != rfEndianess()) */
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        for(i = 0; i < length; i ++)
        {
            rfSwapEndianUI(&v[i]);
        }
    }
}




