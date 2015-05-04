/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_TEXTFILE_MODULE_HEADERS
#define RF_TEXTFILE_MODULE_HEADERS

#include <rf_options.h> // to include the options
#ifdef RF_MODULE_IO_TEXTFILE //module check

#include <IO/rf_textfile.h>
//for File manipulation
#include <System/rf_system.h>

#else
    #error Attempted to include RFtextfile while not having compiled the library with the appropriate module activated
#endif

#endif//include guards end
