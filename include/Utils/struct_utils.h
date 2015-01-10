/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2015, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
 *
 * Macros to automatically create allocate, initialize and create functions
 * for a given struct. Also the corresponding deallocate, deinitialize and
 * destroy functions. Just for when you are lazy.
 */

#ifndef RF_STRUCT_UTILS_H
#define RF_STRUCT_UTILS_H

#include <stdbool.h>
#include <Utils/memory.h>
#include <Preprocessor/rf_xmacro_argcount.h>
#include <Preprocessor/rf_xmacro_keepargs.h>

/*-- Function Signatures --*/
#define RF_STRUCT_INIT_SIG(...)                                         \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRUCT_INIT_SIG, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_INIT_SIG1(i_name_)   \
    bool i_name_##_init(struct i_name_* this)
#define i_SELECT_RF_STRUCT_INIT_SIG0(i_name_, ...)          \
    bool i_name_##_init(struct i_name_* this, __VA_ARGS__)

#define RF_STRUCT_ALLOC_SIG(...)                                        \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRUCT_ALLOC_SIG, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_ALLOC_SIG1(i_name_)  \
    struct i_name_* i_name_##_alloc()
#define i_SELECT_RF_STRUCT_ALLOC_SIG0(i_name_, ...) \
    struct i_name_* i_name_##_alloc(__VA_ARGS__)

#define RF_STRUCT_CREATE_SIG(...)                                       \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRUCT_CREATE_SIG, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_CREATE_SIG1(i_name_) \
    struct i_name_* i_name_##_create()
#define i_SELECT_RF_STRUCT_CREATE_SIG0(i_name_, ...)    \
    struct i_name_* i_name_##_create(__VA_ARGS__)


#define RF_STRUCT_CONSTRUCTION_SIGS_NO_ALLOC(...)   \
    RF_STRUCT_INIT_SIG(__VA_ARGS__);                \
    RF_STRUCT_CREATE_SIG(__VA_ARGS__)

// This helper macro assumes no argument to alloc(). If there are arguments
// then you should use the ALLOC() macros separately
#define RF_STRUCT_CONSTRUCTION_SIGS_WITH_ALLOC(i_name_, ...)    \
    RF_STRUCT_INIT_SIG(i_name_, __VA_ARGS__);                   \
    RF_STRUCT_ALLOC_SIG(i_name_);                               \
    RF_STRUCT_CREATE_SIG(i_name_, __VA_ARGS__)
    


#define RF_STRUCT_DEINIT_SIG(...)                                       \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRUCT_DEINIT_SIG, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_DEINIT_SIG1(i_name_) \
    void i_name_##_deinit(struct i_name_* this)
#define i_SELECT_RF_STRUCT_DEINIT_SIG0(i_name_, ...)            \
    void i_name_##_deinit(struct i_name_* this, __VA_ARGS__)

#define RF_STRUCT_DEALLOC_SIG(...)                                      \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRUCT_DEALLOC_SIG, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_DEALLOC_SIG1(i_name_)    \
    void i_name_##_dealloc(struct i_name_* this)
#define i_SELECT_RF_STRUCT_DEALLOC_SIG0(i_name_, ...)           \
    void i_name_##_dealloc(struct i_name_* this, __VA_ARGS__)

#define RF_STRUCT_DESTROY_SIG(...)                                      \
    RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRUCT_DESTROY_SIG, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_DESTROY_SIG1(i_name_)    \
    void i_name_##_destroy(struct i_name_* this)
#define i_SELECT_RF_STRUCT_DESTROY_SIG0(i_name_, ...)           \
    void i_name_##_destroy(struct i_name_* this, __VA_ARGS__)


#define RF_STRUCT_DESTRUCTION_SIGS_NO_ALLOC(...)    \
    RF_STRUCT_DEINIT_SIG(__VA_ARGS__);              \
    RF_STRUCT_DESTROY_SIG(__VA_ARGS__)

#define RF_STRUCT_DESTRUCTION_SIGS_WITH_ALLOC(...)  \
    RF_STRUCT_DEINIT_SIG(__VA_ARGS__);              \
    RF_STRUCT_DEALLOC_SIG(__VA_ARGS__);             \
    RF_STRUCT_DESTROY_SIG(__VA_ARGS__)


// when using the common sigs macro, destruction functions get no additional arguments
// if you want to provide additional arguments use the RF_STRUCT_DESTRUCTION_SIGS separately
#define RF_STRUCT_COMMON_SIGS_NO_ALLOC(i_name_, ...)            \
    RF_STRUCT_CONSTRUCTION_SIGS_NO_ALLOC(i_name_, __VA_ARGS__); \
    RF_STRUCT_DESTRUCTION_SIGS_NO_ALLOC(i_name_)

#define RF_STRUCT_COMMON_SIGS_WITH_ALLOC(i_name_, ...)              \
    RF_STRUCT_CONSTRUCTION_SIGS_WITH_ALLOC(i_name_, __VA_ARGS__);   \
    RF_STRUCT_DESTRUCTION_SIGS_WITH_ALLOC(i_name_)



/*-- Function Definitions --*/
#define RF_STRUCT_CREATE_DEF_NO_ALLOC(...)                              \
    RP_SELECT_FUNC_IF_NARGIS2(i_SELECT_RF_STRUCT_CREATE_DEF_NO_ALLOC, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_CREATE_DEF_NO_ALLOC1(i_name_)    \
    i_SELECT_RF_STRUCT_CREATE_SIG1(i_name_)                 \
    {                                                       \
        struct i_name_ *ret;                                \
        RF_MALLOC(ret, sizeof(*ret), return NULL);          \
        if (!i_name_##_init(ret)) {                         \
            free(ret);                                      \
            ret = NULL;                                     \
        }                                                   \
        return ret;                                         \
    }
#define i_SELECT_RF_STRUCT_CREATE_DEF_NO_ALLOC0(i_name_, ...)           \
    i_SELECT_RF_STRUCT_CREATE_SIG0(i_name_, RP_COMBINE_EVERY_TWO_ARGUMENTS(__VA_ARGS__)) \
    {                                                                   \
        struct i_name_* ret;                                            \
        RF_MALLOC(ret, sizeof(*ret), return NULL);                      \
        if (!i_name_##_init(ret, RP_KEEP_ODD_ARGUMENTS(__VA_ARGS__))) { \
            free(ret);                                                  \
            ret = NULL;                                                 \
        }                                                               \
        return ret;                                                     \
    }

#define RF_STRUCT_CREATE_DEF_WITH_ALLOC(...)                            \
    RP_SELECT_FUNC_IF_NARGIS2(i_SELECT_RF_STRUCT_CREATE_DEF_WITH_ALLOC, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_CREATE_DEF_WITH_ALLOC1(i_name_)  \
    i_SELECT_RF_STRUCT_CREATE_SIG1(i_name_)                 \
    {                                                       \
        struct i_name_ *ret;                                \
        ret = i_name_##_alloc();                            \
        if (!ret) {                                         \
            return NULL;                                    \
        }                                                   \
        if (!i_name_##_init(ret)) {                         \
            free(ret);                                      \
            ret = NULL;                                     \
        }                                                   \
        return ret;                                         \
    }
#define i_SELECT_RF_STRUCT_CREATE_DEF_WITH_ALLOC0(i_name_, ...)         \
    i_SELECT_RF_STRUCT_CREATE_SIG0(i_name_, RP_COMBINE_EVERY_TWO_ARGUMENTS(__VA_ARGS__)) \
    {                                                                   \
        struct i_name_* ret;                                            \
        ret = i_name_##_alloc();                                        \
        if (!ret) {                                                     \
            return NULL;                                                \
        }                                                               \
        if (!i_name_##_init(ret, RP_KEEP_ODD_ARGUMENTS(__VA_ARGS__))) { \
            free(ret);                                                  \
            ret = NULL;                                                 \
        }                                                               \
        return ret;                                                     \
    }



#define RF_STRUCT_DESTROY_DEF_NO_ALLOC(...)                             \
    RP_SELECT_FUNC_IF_NARGIS2(i_SELECT_RF_STRUCT_DESTROY_DEF_NO_ALLOC, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_DESTROY_DEF_NO_ALLOC1(i_name_)   \
    i_SELECT_RF_STRUCT_DESTROY_SIG1(i_name_)                \
    {                                                       \
        i_name_##_deinit(this);                             \
        free(this);                                         \
    }
#define i_SELECT_RF_STRUCT_DESTROY_DEF_NO_ALLOC0(i_name_, ...)          \
    i_SELECT_RF_STRUCT_DESTROY_SIG0(i_name_, RP_COMBINE_EVERY_TWO_ARGUMENTS(__VA_ARGS__)) \
    {                                                                   \
        i_name_##_deinit(this, RP_KEEP_ODD_ARGUMENTS(__VA_ARGS__));     \
        free(this);                                                     \
    }

#define RF_STRUCT_DESTROY_DEF_WITH_ALLOC(...)                           \
    RP_SELECT_FUNC_IF_NARGIS2(i_SELECT_RF_STRUCT_DESTROY_DEF_WITH_ALLOC, 1, __VA_ARGS__)
#define i_SELECT_RF_STRUCT_DESTROY_DEF_WITH_ALLOC1(i_name_) \
    i_SELECT_RF_STRUCT_DESTROY_SIG1(i_name_)                \
    {                                                       \
        i_name_##_deinit(this);                             \
        i_name_##_dealloc(this);                            \
    }
#define i_SELECT_RF_STRUCT_DESTROY_DEF_WITH_ALLOC0(i_name_, ...)        \
    i_SELECT_RF_STRUCT_DESTROY_SIG0(i_name_, RP_COMBINE_EVERY_TWO_ARGUMENTS(__VA_ARGS__)) \
    {                                                                   \
        i_name_##_deinit(this, RP_KEEP_ODD_ARGUMENTS(__VA_ARGS__));     \
        i_name_##_dealloc(this);                                        \
    }


// when using the common defs macro, destruction functions get no additional arguments
// if you want to provide additional arguments use the RF_STRUCT_DESTROY_DEF separately
#define RF_STRUCT_COMMON_DEFS_NO_ALLOC(i_name_, ...)    \
    RF_STRUCT_CREATE_DEF_NO_ALLOC(i_name_, __VA_ARGS__) \
    RF_STRUCT_DESTROY_DEF_NO_ALLOC(i_name_)

#define RF_STRUCT_COMMON_DEFS_WITH_ALLOC(i_name_, ...)      \
    RF_STRUCT_CREATE_DEF_WITH_ALLOC(i_name_, __VA_ARGS__)   \
    RF_STRUCT_DESTROY_DEF_WITH_ALLOC(i_name_)

#endif
