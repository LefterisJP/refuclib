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
#ifndef RF_STDIO_H
#define RF_STDIO_H

#include <rf_setup.h> //for i_DECLIMEX_

#include <stdio.h> //for FILE*

//! Initializes the stdio buffer that is needed by all Refu stdio functions
char rfInitStdio();

//! @brief Prints a formatted string to the stdout
//!
//! For more information take a look at @ref rfStringX_Formatv()
//! @lmsFunction
//! @param format The formatted string to print
//! @param ... Extra arguments to output to the string
//! @return The number of @c bytes written to the stdout
i_DECLIMEX_ int rfPrintf(const char * format, ...);
//! @brief Prints a formatted string to the a file descriptor
//!
//! For more information take a look at @ref rfStringX_Formatv()
//! @lmsFunction
//! @param f The file descriptor into which to output the formatted string
//! @param format The formatted string to print
//! @param ... Extra arguments to output to the string
//! @return The number of @c bytes written to the @c
i_DECLIMEX_ int rfFPrintf(FILE* f,const char * format, ...);


#endif
