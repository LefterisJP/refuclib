/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_INTRUSIVE_LIST_MODULE_HEADERS
#define RF_INTRUSIVE_LIST_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_INTRUSIVE_LIST // module check

#include <Data_Structures/intrusive_list.h>

#else
    #error Attempted to include RFlist while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
