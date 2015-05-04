/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_SYSTEM_MODULE_HEADERS
#define RF_SYSTEM_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_SYSTEM//module check

#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h>  //for import export macro
#include <Definitions/retcodes.h> //for booleans
#include <System/rf_system.h>

#else
    #error Attempted to include System functions while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
