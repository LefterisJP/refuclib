/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_SLEEP_MODULE_HEADERS
#define RF_SLEEP_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_TIME_TIMER //module check

#include <Definitions/types.h> //for the fixed size data types
#include <Definitions/imex.h> //for the import export macro
#include <Time/sleep.h>

#else
    #error Attempted to include RFdate while not having compiled the library with the appropriate module activated
#endif

#endif //include guards end

