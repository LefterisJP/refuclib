/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_LIST_MODULE_HEADERS
#define RF_LIST_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_LIST // module check

#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens

#ifdef RF_MODULE_LIST_EXTRA
#include <Data_Structures/list_extra.h>
#endif

#else
    #error Attempted to include RFlist while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
