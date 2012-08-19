
#include "../../include/rf_utils.h"

#include <string.h> //for memcpy

///Declaration of the utils globals
RF_UTILS rfUTILS;


// A function which detects system's endianess
void rfUTILS_DetectEndianess()
{
    int32_t anint = (int32_t)0xdeadbeef;
    rfUTILS.endianess = (*(char *)&anint == (char)0xef)? RF_LITTLE_ENDIAN : RF_BIG_ENDIAN;
}

//A function that returns the endianess of the system
char rfUTILS_Endianess()
{
    return rfUTILS.endianess;
}

// Swaps the endianness of the variable
void rfUTILS_SwapEndianUS(uint16_t* v)
{
    *v = (*v>>8)|(*v<<8);
}

//Swaps the endianness of the variable
void rfUTILS_SwapEndianS(int16_t* v)
{
    *v = ((*v&0xff00)>> 8) |((*v&0x00ff)<<8);
}

// Swaps the endianness of the variable
void rfUTILS_SwapEndianUI(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}
// Swaps the endianness of the variable
void rfUTILS_SwapEndianI(int32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}

// Swaps the endianness of the variable
void rfUTILS_SwapEndianUL(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}





