/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_BINARYARRAY_MODULE_HEADERS
#define RF_BINARYARRAY_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_BINARYARRAY//Binarry array module check

#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>//for fixed size data types
#include <Data_Structures/binaryarray_decl.h> //for RFbinarry_array
#include <Data_Structures/binaryarray.h>

#else
    #error Attempted to include RFbinary_array while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
