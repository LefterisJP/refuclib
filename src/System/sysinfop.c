//*---------------------Corresponding Header inclusion---------------------------------
#include <Definitions/types.h> //for the fixed size data types
    #ifdef REFU_LINUX_VERSION
    #include <time.h>  //for clockid_t used in the System info structure
    #endif
#include "info.ph"
//*---------------------Outside module inclusion----------------------------------------
#include <Definitions/imex.h> //for import export macro used in endianess.h
#include <Utils/endianess.h> //for endianess flags
//*----------------------------End of Includes------------------------------------------

//system info global definition
RF_SystemInfo rfSysInfo;

// A function which detects the system's endianess
void i_DetectEndianess()
{
    int32_t anint = (int32_t)0xdeadbeef;
    rfSysInfo.endianess = (*(char *)&anint == (char)0xef)? RF_LITTLE_ENDIAN : RF_BIG_ENDIAN;
}
