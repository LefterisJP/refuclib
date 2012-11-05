/*
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/** @defgroup RF_Preprocessor_DefaultArgsGRP Default Arguments Related Macros
** @addtogroup RF_Preprocessor_DefaultArgsGRP
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
//! @} //end of the RF_Preprocessor_DefaultArgsGRP group
