/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_MODULE_HEADERS
#define RF_STRING_MODULE_HEADERS

#include <rf_options.h>
#ifdef RF_MODULE_STRINGS//check if the strings are included as a module


/*---------other needed headers-----*/
#include <Utils/rf_unicode.h>
/*---------RFstring headers--------*/
#include <String/rf_str_xdecl.h>
#include <String/rf_str_common.h>
#include <String/rf_str_core.h>
#include <String/rf_str_retrieval.h>
#include <String/rf_str_conversion.h>
#include <String/rf_str_manipulation.h>
#include <String/rf_str_files.h>
/*---------RFstringx headers--------*/
#include <String/rf_str_corex.h>
#include <String/rf_str_manipulationx.h>
#include <String/rf_str_traversalx.h>
#include <String/rf_str_filesx.h>
#include <stdarg.h> //needed for the va_list argument

#else //end of the strings module include
    #error Attempted to include Refu String manipulation without the proper flag
#endif

#endif //end OF ifndef guards
