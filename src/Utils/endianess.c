//*---------------------Corresponding Header inclusion---------------------------------
#include <Definitions/imex.h> //import export macro
#include <Definitions/types.h> //fixed size data types
#include <Utils/endianess.h>
//*---------------------Outside module inclusion----------------------------------------
    #ifdef REFU_LINUX_VERSION
    #include <time.h> //for clockid_t used in the System info structure
    #endif
#include "../System/info.ph" //for the System info private struct
//*----------------------------End of Includes------------------------------------------


//A function that returns the endianess of the system
char rfEndianess()
{
    return rfSysInfo.endianess;
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





