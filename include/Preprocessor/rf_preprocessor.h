/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */

/** @defgroup RFpreprocessor_default_args_g_r_p Default Arguments Related Macros
** @addtogroup RFpreprocessor_default_args_g_r_p
** @{
** @def RF_DECLARE_DFUNCXX(retType,funcN,defArgsN,...)
** @brief Declares a function with default arguments
**
** Replace the XX with the number of arguments of your function. Currently the maximum number of supported arguments is 36
** @param retType The return type of the function
** @param funcN The name of the function
** @param defArgsN How many of the functions arguments will have default values
** @param ... Give the type of each argument here. So give argument1Type,argument2Type,...,argumentXXType
**/
#define RF_DECLARE_DFUNCXX(retType,funcN,defArgsN,...) //just so doxygen can parse it...temporary until I find another wayn



/**
** @def RF_DEFINE_DFUNCXX(retType,funcN,defArgsN,...)
** @brief Defines a function with default arguments
**
** Replace the XX with the number of arguments of your function. Currently the maximum number of supported arguments is 36
** @param retType The return type of the function
** @param funcN The name of the function
** @param ... Here give in order: 1)the type of each variable 2)the default arguments value. For example arg1Type,arg2Type,arg3Types,defValue1,defValue2
**/
#define RF_DEFINE_DFUNCXX(retType,funcN,...) //just so doxygen can parse it...temporary until I find another way
//! @} //end of the RFpreprocessor_default_args_g_r_p group
