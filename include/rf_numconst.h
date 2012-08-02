/**
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
**/

#ifndef RF_NUMCONST_H
#define RF_NUMCONST_H
#include <rf_setup.h>

//! @brief Saves an int8_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFI8_(int8_t i);
//! @brief Saves a uint8_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFUI8_(uint8_t i);
//! @brief Saves an int16_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFI16_(int16_t i);
//! @brief Saves a uint16_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFUI16_(uint16_t i);
//! @brief Saves an int32_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFI32_(int32_t i);
//! @brief Saves a uint32_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFUI32_(uint32_t i);
//! @brief Saves an int64_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFI64_(int64_t i);
//! @brief Saves a uint64_t into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFUI64_(uint64_t i);

//! @brief Saves an @c int type integer into the local stack memory space
//!
//! This is an internal function and is not intended to be used by the user
//! @param i The integer to save
//! @return A pointer to the position of the integer
i_DECLIMEX_ void* i_RFI_(int i);

#endif
