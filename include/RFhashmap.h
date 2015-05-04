/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_HASHMAP_MODULE_HEADERS
#define RF_HASHMAP_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_HASHMAP // module check

#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/defarg.h> //for enabling default arguments
#include <Preprocessor/rf_tokens.h>//for the defined library tokens
#include <String/rf_str_decl.h> //for RFstring
#include <Definitions/retcodes.h> //for error codes, bool

#ifdef RF_MODULE_HASHMAP_EXTRA
#include <Data_Structures/hashmap_extra.h>
#endif

#else
    #error Attempted to include RFhash_map while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
