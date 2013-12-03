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
**
** --Utils/endianess.h
** This header declares functions and macros that deal with endianess
*/

#ifndef RF_UTILS_ENDIANESS_H
#define RF_UTILS_ENDIANESS_H

#include <Definitions/imex.h> //import export macro
#include <Definitions/types.h> //fixed size data types

/**
 ** Represents the endianess of a file or byte stream
 **/
enum RFendianess {
    RF_LITTLE_ENDIAN = 1, /*!< Little Endian byte order */
    RF_BIG_ENDIAN, /*!< Big Endian byte order */
    RF_ENDIANESS_UNKNOWN /*!< Signigies we are not sure of the byte order */
};


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/**
 ** @brief A function that returns the endianess of the system
 **
 ** @return The endianess of the system. Can be either @c RF_LITTLE_ENDIAN or @c RF_BIG_ENDIAN
 **/
i_DECLIMEX_ char rfEndianess();
/**
 ** @brief A function to return the endianess that is NOT used by the system 
 ** 
 **/ 
i_DECLIMEX_ char rfOtherEndianess();
/**
 ** Swaps the endianness of the variable
 ** @warning the & 0xXXXX makes sure that in the signed case we DO NOT get a wrong number.
 ** (Because when you shift right >> a signed number the bits get filled by the sign and not zeros
 ** @param v The Variable whose endianness we wanna change
 **/
i_DECLIMEX_ void rfSwapEndianUS(uint16_t* v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **/
i_DECLIMEX_ void rfSwapEndianS(int16_t* v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **
 **/
i_DECLIMEX_ void rfSwapEndianUI(uint32_t*   v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **
 **/
i_DECLIMEX_ void rfSwapEndianI(int32_t*   v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **
 **/
i_DECLIMEX_ void rfSwapEndianUL(uint32_t*   v);




i_DECLIMEX_ void rfProcessByteOrderUS(uint16_t* v, int file_endianess);
i_DECLIMEX_ void rfProcessByteOrderUI(uint32_t* v, int file_endianess);
i_DECLIMEX_ void rfProcessByteOrderUSA(uint16_t* v, unsigned int length,
                           int file_endianess);

i_DECLIMEX_ void rfProcessByteOrderUIA(uint32_t* v, unsigned int length,
                           int file_endianess);


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
