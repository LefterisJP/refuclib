/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_DYNAMIC_ARRAY_MODULE_HEADERS
#define RF_DYNAMIC_ARRAY_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_DYNAMICARRAY //module check

#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/retcodes.h> //for error codes, bool
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens

#ifdef RF_MODULE_DYNAMICARRAY_EXTRA
#include <Data_Structures/dynamicarray_extra.h>
#endif

#else
    #error Attempted to include RFdynamic_array while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
