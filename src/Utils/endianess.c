/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <Utils/endianess.h>
/*------------- End of includes -------------*/

// Swaps the endianness of the variable
void rf_swap_endianess_u16(uint16_t* v)
{
    *v = (*v>>8)|(*v<<8);
}

//Swaps the endianness of the variable
void rf_swap_endianess_i16(int16_t* v)
{
    *v = ((*v&0xff00)>> 8) |((*v&0x00ff)<<8);
}
// Swaps the endianness of the variable
void rf_swap_endianess_i32(int32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}
// Swaps the endianness of the variable
void rf_swap_endianess_u32(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}


void rf_process_byte_order_u32(uint32_t* v, int file_endianess)
{
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        rf_swap_endianess_u32(v);
    }
}

void rf_process_byte_order_u16(uint16_t* v, int file_endianess)
{
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        rf_swap_endianess_u16(v);
    }
}
void rf_process_byte_order_u16A(uint16_t* v, unsigned int length,
                           int file_endianess)
{
    unsigned int i;
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        for(i = 0; i < length; i ++)
        {
            rf_swap_endianess_u16(&v[i]);
        }
    }
}

void rf_process_byte_order_u32A(uint32_t* v, unsigned int length,
                           int file_endianess)
{
    unsigned int i;
    if(file_endianess == RF_LITTLE_ENDIAN)
    {
        for(i = 0; i < length; i ++)
        {
            rf_swap_endianess_u32(&v[i]);
        }
    }
}




