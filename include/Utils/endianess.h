/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
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
 ** Swaps the endianness of the variable
 ** @warning the & 0xXXXX makes sure that in the signed case we DO NOT get a wrong number.
 ** (Because when you shift right >> a signed number the bits get filled by the sign and not zeros
 ** @param v The Variable whose endianness we wanna change
 **/
i_DECLIMEX_ void rf_swap_endianess_u16(uint16_t* v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **/
i_DECLIMEX_ void rf_swap_endianess_i16(int16_t* v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **
 **/
i_DECLIMEX_ void rf_swap_endianess_i32(int32_t*   v);
/**
 ** Swaps the endianness of the variable
 ** @param v The Variable whose endianness we wanna change
 **
 **/
i_DECLIMEX_ void rf_swap_endianess_u32(uint32_t*   v);




i_DECLIMEX_ void rf_process_byte_order_u16(uint16_t* v, int file_endianess);
i_DECLIMEX_ void rf_process_byte_order_u32(uint32_t* v, int file_endianess);
i_DECLIMEX_ void rf_process_byte_order_u16A(uint16_t* v, unsigned int length,
                           int file_endianess);

i_DECLIMEX_ void rf_process_byte_order_u32A(uint32_t* v, unsigned int length,
                           int file_endianess);


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
