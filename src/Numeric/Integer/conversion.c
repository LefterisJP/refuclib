//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/inline.h> //for the inline declaration
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/retcodes.h> //for return codes and true/false definition
#include "rf_int.ph"
//*----------------------------End of Includes------------------------------------------



//force creation of external symbols for inline functions
i_INLINE_INS int intToStr(int64_t i,char* buff);
i_INLINE_INS int uintToStr(uint64_t i,char* buff);
i_INLINE_INS int uintToUHexStr(uint64_t i,char* buff);
i_INLINE_INS int uintToLHexStr(uint64_t i,char* buff);
