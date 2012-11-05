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
**
**
** This file contains macros to easily declare functions with default arguments
** in header files using the C preprocessor. It is automatically generated
** by the python script gen_rf_xmacro_decl.py
*/
#include "rf_xmacro_sub.h"//to be able to do subtraction with the preprocessor
#include "rf_xmacro_gt.h"//to be able to do greater than comparisons with the preprocessor
#include "rf_xmacro_lt.h"//to be able to do less than comparisons with the preprocessor

#include "rf_xmacro_utils.h"//to use util macros for token pasting



///These macros here are used in order to pickup the correct function macro definition
#define i_RP_DECL_CHECK2(A__,B__)   RP_GT(B__,A__)
#define i_RP_DECL_CHECK1(A__,B__)   RP_LT(B__,A__)
#define i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,thisArgN__)  i_RP_DECL_CHECK1(RP_SUB(maxArgsN__,defArgsN__),thisArgN__)
#define i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,thisArgN__)  i_RP_DECL_CHECK2(RP_SUB(maxArgsN__,defArgsN__),thisArgN__)
#define i_RP_PICK_FUNC_DECL(funcmacro__,eqresult__,cmpresult__)  i_RP_PASTE3(funcmacro__,eqresult__,cmpresult__)


//! These macros are used when you want to declare a function in a header file with default arguments and want to avoid lots of typing



//! This macro is used to declare a function with 36 arguments in a header file
#define RF_DECLARE_DFUNC36(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)\
i_RP_DECLARE_FUNC36(funcNarf__,retType__,36,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)

#define i_RP_DECLARE_FUNC36(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC36_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,36),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,36))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)

#define i_RP_DECLARE_FUNC36_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)\
retType__ funcNarf__##36(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36);
#define i_RP_DECLARE_FUNC36_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)\
retType__ funcNarf__##36(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36);\
i_RP_DECLARE_FUNC35(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)
#define i_RP_DECLARE_FUNC36_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)
#define i_RP_DECLARE_FUNC36_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35,arg__36)



//! This macro is used to declare a function with 35 arguments in a header file
#define RF_DECLARE_DFUNC35(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)\
i_RP_DECLARE_FUNC35(funcNarf__,retType__,35,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)

#define i_RP_DECLARE_FUNC35(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC35_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,35),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,35))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)

#define i_RP_DECLARE_FUNC35_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)\
retType__ funcNarf__##35(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35);
#define i_RP_DECLARE_FUNC35_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)\
retType__ funcNarf__##35(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35);\
i_RP_DECLARE_FUNC34(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)
#define i_RP_DECLARE_FUNC35_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)
#define i_RP_DECLARE_FUNC35_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34,arg__35)



//! This macro is used to declare a function with 34 arguments in a header file
#define RF_DECLARE_DFUNC34(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)\
i_RP_DECLARE_FUNC34(funcNarf__,retType__,34,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)

#define i_RP_DECLARE_FUNC34(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC34_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,34),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,34))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)

#define i_RP_DECLARE_FUNC34_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)\
retType__ funcNarf__##34(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34);
#define i_RP_DECLARE_FUNC34_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)\
retType__ funcNarf__##34(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34);\
i_RP_DECLARE_FUNC33(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)
#define i_RP_DECLARE_FUNC34_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)
#define i_RP_DECLARE_FUNC34_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33,arg__34)



//! This macro is used to declare a function with 33 arguments in a header file
#define RF_DECLARE_DFUNC33(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)\
i_RP_DECLARE_FUNC33(funcNarf__,retType__,33,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)

#define i_RP_DECLARE_FUNC33(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC33_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,33),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,33))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)

#define i_RP_DECLARE_FUNC33_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)\
retType__ funcNarf__##33(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33);
#define i_RP_DECLARE_FUNC33_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)\
retType__ funcNarf__##33(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33);\
i_RP_DECLARE_FUNC32(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)
#define i_RP_DECLARE_FUNC33_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)
#define i_RP_DECLARE_FUNC33_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32,arg__33)



//! This macro is used to declare a function with 32 arguments in a header file
#define RF_DECLARE_DFUNC32(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)\
i_RP_DECLARE_FUNC32(funcNarf__,retType__,32,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)

#define i_RP_DECLARE_FUNC32(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC32_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,32),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,32))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)

#define i_RP_DECLARE_FUNC32_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)\
retType__ funcNarf__##32(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32);
#define i_RP_DECLARE_FUNC32_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)\
retType__ funcNarf__##32(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32);\
i_RP_DECLARE_FUNC31(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)
#define i_RP_DECLARE_FUNC32_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)
#define i_RP_DECLARE_FUNC32_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31,arg__32)



//! This macro is used to declare a function with 31 arguments in a header file
#define RF_DECLARE_DFUNC31(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)\
i_RP_DECLARE_FUNC31(funcNarf__,retType__,31,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)

#define i_RP_DECLARE_FUNC31(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC31_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,31),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,31))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)

#define i_RP_DECLARE_FUNC31_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)\
retType__ funcNarf__##31(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31);
#define i_RP_DECLARE_FUNC31_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)\
retType__ funcNarf__##31(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31);\
i_RP_DECLARE_FUNC30(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)
#define i_RP_DECLARE_FUNC31_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)
#define i_RP_DECLARE_FUNC31_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30,arg__31)



//! This macro is used to declare a function with 30 arguments in a header file
#define RF_DECLARE_DFUNC30(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)\
i_RP_DECLARE_FUNC30(funcNarf__,retType__,30,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)

#define i_RP_DECLARE_FUNC30(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC30_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,30),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,30))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)

#define i_RP_DECLARE_FUNC30_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)\
retType__ funcNarf__##30(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30);
#define i_RP_DECLARE_FUNC30_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)\
retType__ funcNarf__##30(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30);\
i_RP_DECLARE_FUNC29(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)
#define i_RP_DECLARE_FUNC30_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)
#define i_RP_DECLARE_FUNC30_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29,arg__30)



//! This macro is used to declare a function with 29 arguments in a header file
#define RF_DECLARE_DFUNC29(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)\
i_RP_DECLARE_FUNC29(funcNarf__,retType__,29,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)

#define i_RP_DECLARE_FUNC29(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC29_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,29),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,29))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)

#define i_RP_DECLARE_FUNC29_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)\
retType__ funcNarf__##29(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29);
#define i_RP_DECLARE_FUNC29_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)\
retType__ funcNarf__##29(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29);\
i_RP_DECLARE_FUNC28(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)
#define i_RP_DECLARE_FUNC29_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)
#define i_RP_DECLARE_FUNC29_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28,arg__29)



//! This macro is used to declare a function with 28 arguments in a header file
#define RF_DECLARE_DFUNC28(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)\
i_RP_DECLARE_FUNC28(funcNarf__,retType__,28,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)

#define i_RP_DECLARE_FUNC28(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC28_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,28),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,28))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)

#define i_RP_DECLARE_FUNC28_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)\
retType__ funcNarf__##28(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28);
#define i_RP_DECLARE_FUNC28_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)\
retType__ funcNarf__##28(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28);\
i_RP_DECLARE_FUNC27(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)
#define i_RP_DECLARE_FUNC28_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)
#define i_RP_DECLARE_FUNC28_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27,arg__28)



//! This macro is used to declare a function with 27 arguments in a header file
#define RF_DECLARE_DFUNC27(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)\
i_RP_DECLARE_FUNC27(funcNarf__,retType__,27,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)

#define i_RP_DECLARE_FUNC27(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC27_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,27),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,27))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)

#define i_RP_DECLARE_FUNC27_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)\
retType__ funcNarf__##27(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27);
#define i_RP_DECLARE_FUNC27_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)\
retType__ funcNarf__##27(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27);\
i_RP_DECLARE_FUNC26(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)
#define i_RP_DECLARE_FUNC27_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)
#define i_RP_DECLARE_FUNC27_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26,arg__27)



//! This macro is used to declare a function with 26 arguments in a header file
#define RF_DECLARE_DFUNC26(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)\
i_RP_DECLARE_FUNC26(funcNarf__,retType__,26,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)

#define i_RP_DECLARE_FUNC26(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC26_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,26),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,26))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)

#define i_RP_DECLARE_FUNC26_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)\
retType__ funcNarf__##26(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26);
#define i_RP_DECLARE_FUNC26_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)\
retType__ funcNarf__##26(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26);\
i_RP_DECLARE_FUNC25(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)
#define i_RP_DECLARE_FUNC26_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)
#define i_RP_DECLARE_FUNC26_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25,arg__26)



//! This macro is used to declare a function with 25 arguments in a header file
#define RF_DECLARE_DFUNC25(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)\
i_RP_DECLARE_FUNC25(funcNarf__,retType__,25,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)

#define i_RP_DECLARE_FUNC25(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC25_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,25),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,25))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)

#define i_RP_DECLARE_FUNC25_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)\
retType__ funcNarf__##25(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25);
#define i_RP_DECLARE_FUNC25_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)\
retType__ funcNarf__##25(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25);\
i_RP_DECLARE_FUNC24(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)
#define i_RP_DECLARE_FUNC25_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)
#define i_RP_DECLARE_FUNC25_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24,arg__25)



//! This macro is used to declare a function with 24 arguments in a header file
#define RF_DECLARE_DFUNC24(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)\
i_RP_DECLARE_FUNC24(funcNarf__,retType__,24,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)

#define i_RP_DECLARE_FUNC24(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC24_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,24),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,24))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)

#define i_RP_DECLARE_FUNC24_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)\
retType__ funcNarf__##24(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24);
#define i_RP_DECLARE_FUNC24_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)\
retType__ funcNarf__##24(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24);\
i_RP_DECLARE_FUNC23(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)
#define i_RP_DECLARE_FUNC24_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)
#define i_RP_DECLARE_FUNC24_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23,arg__24)



//! This macro is used to declare a function with 23 arguments in a header file
#define RF_DECLARE_DFUNC23(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)\
i_RP_DECLARE_FUNC23(funcNarf__,retType__,23,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)

#define i_RP_DECLARE_FUNC23(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC23_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,23),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,23))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)

#define i_RP_DECLARE_FUNC23_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)\
retType__ funcNarf__##23(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23);
#define i_RP_DECLARE_FUNC23_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)\
retType__ funcNarf__##23(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23);\
i_RP_DECLARE_FUNC22(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)
#define i_RP_DECLARE_FUNC23_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)
#define i_RP_DECLARE_FUNC23_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22,arg__23)



//! This macro is used to declare a function with 22 arguments in a header file
#define RF_DECLARE_DFUNC22(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)\
i_RP_DECLARE_FUNC22(funcNarf__,retType__,22,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)

#define i_RP_DECLARE_FUNC22(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC22_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,22),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,22))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)

#define i_RP_DECLARE_FUNC22_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)\
retType__ funcNarf__##22(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22);
#define i_RP_DECLARE_FUNC22_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)\
retType__ funcNarf__##22(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22);\
i_RP_DECLARE_FUNC21(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)
#define i_RP_DECLARE_FUNC22_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)
#define i_RP_DECLARE_FUNC22_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21,arg__22)



//! This macro is used to declare a function with 21 arguments in a header file
#define RF_DECLARE_DFUNC21(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)\
i_RP_DECLARE_FUNC21(funcNarf__,retType__,21,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)

#define i_RP_DECLARE_FUNC21(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC21_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,21),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,21))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)

#define i_RP_DECLARE_FUNC21_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)\
retType__ funcNarf__##21(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21);
#define i_RP_DECLARE_FUNC21_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)\
retType__ funcNarf__##21(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21);\
i_RP_DECLARE_FUNC20(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)
#define i_RP_DECLARE_FUNC21_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)
#define i_RP_DECLARE_FUNC21_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20,arg__21)



//! This macro is used to declare a function with 20 arguments in a header file
#define RF_DECLARE_DFUNC20(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)\
i_RP_DECLARE_FUNC20(funcNarf__,retType__,20,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)

#define i_RP_DECLARE_FUNC20(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC20_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,20),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,20))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)

#define i_RP_DECLARE_FUNC20_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)\
retType__ funcNarf__##20(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20);
#define i_RP_DECLARE_FUNC20_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)\
retType__ funcNarf__##20(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20);\
i_RP_DECLARE_FUNC19(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)
#define i_RP_DECLARE_FUNC20_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)
#define i_RP_DECLARE_FUNC20_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19,arg__20)



//! This macro is used to declare a function with 19 arguments in a header file
#define RF_DECLARE_DFUNC19(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)\
i_RP_DECLARE_FUNC19(funcNarf__,retType__,19,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)

#define i_RP_DECLARE_FUNC19(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC19_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,19),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,19))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)

#define i_RP_DECLARE_FUNC19_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)\
retType__ funcNarf__##19(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19);
#define i_RP_DECLARE_FUNC19_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)\
retType__ funcNarf__##19(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19);\
i_RP_DECLARE_FUNC18(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)
#define i_RP_DECLARE_FUNC19_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)
#define i_RP_DECLARE_FUNC19_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18,arg__19)



//! This macro is used to declare a function with 18 arguments in a header file
#define RF_DECLARE_DFUNC18(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)\
i_RP_DECLARE_FUNC18(funcNarf__,retType__,18,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)

#define i_RP_DECLARE_FUNC18(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC18_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,18),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,18))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)

#define i_RP_DECLARE_FUNC18_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)\
retType__ funcNarf__##18(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18);
#define i_RP_DECLARE_FUNC18_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)\
retType__ funcNarf__##18(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18);\
i_RP_DECLARE_FUNC17(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)
#define i_RP_DECLARE_FUNC18_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)
#define i_RP_DECLARE_FUNC18_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17,arg__18)



//! This macro is used to declare a function with 17 arguments in a header file
#define RF_DECLARE_DFUNC17(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)\
i_RP_DECLARE_FUNC17(funcNarf__,retType__,17,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)

#define i_RP_DECLARE_FUNC17(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC17_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,17),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,17))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)

#define i_RP_DECLARE_FUNC17_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)\
retType__ funcNarf__##17(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17);
#define i_RP_DECLARE_FUNC17_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)\
retType__ funcNarf__##17(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17);\
i_RP_DECLARE_FUNC16(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)
#define i_RP_DECLARE_FUNC17_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)
#define i_RP_DECLARE_FUNC17_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16,arg__17)



//! This macro is used to declare a function with 16 arguments in a header file
#define RF_DECLARE_DFUNC16(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)\
i_RP_DECLARE_FUNC16(funcNarf__,retType__,16,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)

#define i_RP_DECLARE_FUNC16(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC16_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,16),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,16))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)

#define i_RP_DECLARE_FUNC16_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)\
retType__ funcNarf__##16(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16);
#define i_RP_DECLARE_FUNC16_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)\
retType__ funcNarf__##16(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16);\
i_RP_DECLARE_FUNC15(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)
#define i_RP_DECLARE_FUNC16_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)
#define i_RP_DECLARE_FUNC16_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15,arg__16)



//! This macro is used to declare a function with 15 arguments in a header file
#define RF_DECLARE_DFUNC15(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)\
i_RP_DECLARE_FUNC15(funcNarf__,retType__,15,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)

#define i_RP_DECLARE_FUNC15(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC15_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,15),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,15))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)

#define i_RP_DECLARE_FUNC15_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)\
retType__ funcNarf__##15(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15);
#define i_RP_DECLARE_FUNC15_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)\
retType__ funcNarf__##15(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15);\
i_RP_DECLARE_FUNC14(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)
#define i_RP_DECLARE_FUNC15_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)
#define i_RP_DECLARE_FUNC15_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14,arg__15)



//! This macro is used to declare a function with 14 arguments in a header file
#define RF_DECLARE_DFUNC14(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)\
i_RP_DECLARE_FUNC14(funcNarf__,retType__,14,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)

#define i_RP_DECLARE_FUNC14(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC14_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,14),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,14))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)

#define i_RP_DECLARE_FUNC14_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)\
retType__ funcNarf__##14(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14);
#define i_RP_DECLARE_FUNC14_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)\
retType__ funcNarf__##14(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14);\
i_RP_DECLARE_FUNC13(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)
#define i_RP_DECLARE_FUNC14_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)
#define i_RP_DECLARE_FUNC14_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13,arg__14)



//! This macro is used to declare a function with 13 arguments in a header file
#define RF_DECLARE_DFUNC13(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)\
i_RP_DECLARE_FUNC13(funcNarf__,retType__,13,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)

#define i_RP_DECLARE_FUNC13(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC13_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,13),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,13))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)

#define i_RP_DECLARE_FUNC13_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)\
retType__ funcNarf__##13(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13);
#define i_RP_DECLARE_FUNC13_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)\
retType__ funcNarf__##13(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13);\
i_RP_DECLARE_FUNC12(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)
#define i_RP_DECLARE_FUNC13_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)
#define i_RP_DECLARE_FUNC13_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12,arg__13)



//! This macro is used to declare a function with 12 arguments in a header file
#define RF_DECLARE_DFUNC12(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)\
i_RP_DECLARE_FUNC12(funcNarf__,retType__,12,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)

#define i_RP_DECLARE_FUNC12(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC12_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,12),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,12))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)

#define i_RP_DECLARE_FUNC12_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)\
retType__ funcNarf__##12(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12);
#define i_RP_DECLARE_FUNC12_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)\
retType__ funcNarf__##12(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12);\
i_RP_DECLARE_FUNC11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)
#define i_RP_DECLARE_FUNC12_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)
#define i_RP_DECLARE_FUNC12_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11,arg__12)



//! This macro is used to declare a function with 11 arguments in a header file
#define RF_DECLARE_DFUNC11(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)\
i_RP_DECLARE_FUNC11(funcNarf__,retType__,11,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)

#define i_RP_DECLARE_FUNC11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC11_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,11),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,11))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)

#define i_RP_DECLARE_FUNC11_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)\
retType__ funcNarf__##11(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11);
#define i_RP_DECLARE_FUNC11_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)\
retType__ funcNarf__##11(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11);\
i_RP_DECLARE_FUNC10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)
#define i_RP_DECLARE_FUNC11_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)
#define i_RP_DECLARE_FUNC11_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10,arg__11)



//! This macro is used to declare a function with 10 arguments in a header file
#define RF_DECLARE_DFUNC10(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)\
i_RP_DECLARE_FUNC10(funcNarf__,retType__,10,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)

#define i_RP_DECLARE_FUNC10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC10_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,10),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,10))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)

#define i_RP_DECLARE_FUNC10_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)\
retType__ funcNarf__##10(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10);
#define i_RP_DECLARE_FUNC10_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)\
retType__ funcNarf__##10(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10);\
i_RP_DECLARE_FUNC9(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)
#define i_RP_DECLARE_FUNC10_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)
#define i_RP_DECLARE_FUNC10_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9,arg__10)



//! This macro is used to declare a function with 9 arguments in a header file
#define RF_DECLARE_DFUNC9(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)\
i_RP_DECLARE_FUNC9(funcNarf__,retType__,9,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)

#define i_RP_DECLARE_FUNC9(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC9_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,9),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,9))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)

#define i_RP_DECLARE_FUNC9_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)\
retType__ funcNarf__##9(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9);
#define i_RP_DECLARE_FUNC9_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)\
retType__ funcNarf__##9(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9);\
i_RP_DECLARE_FUNC8(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)
#define i_RP_DECLARE_FUNC9_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)
#define i_RP_DECLARE_FUNC9_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8,arg__9)



//! This macro is used to declare a function with 8 arguments in a header file
#define RF_DECLARE_DFUNC8(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)\
i_RP_DECLARE_FUNC8(funcNarf__,retType__,8,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)

#define i_RP_DECLARE_FUNC8(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC8_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,8),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,8))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)

#define i_RP_DECLARE_FUNC8_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)\
retType__ funcNarf__##8(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8);
#define i_RP_DECLARE_FUNC8_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)\
retType__ funcNarf__##8(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8);\
i_RP_DECLARE_FUNC7(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)
#define i_RP_DECLARE_FUNC8_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)
#define i_RP_DECLARE_FUNC8_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7,arg__8)



//! This macro is used to declare a function with 7 arguments in a header file
#define RF_DECLARE_DFUNC7(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)\
i_RP_DECLARE_FUNC7(funcNarf__,retType__,7,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)

#define i_RP_DECLARE_FUNC7(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC7_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,7),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,7))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)

#define i_RP_DECLARE_FUNC7_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)\
retType__ funcNarf__##7(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7);
#define i_RP_DECLARE_FUNC7_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)\
retType__ funcNarf__##7(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7);\
i_RP_DECLARE_FUNC6(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)
#define i_RP_DECLARE_FUNC7_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)
#define i_RP_DECLARE_FUNC7_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6,arg__7)



//! This macro is used to declare a function with 6 arguments in a header file
#define RF_DECLARE_DFUNC6(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)\
i_RP_DECLARE_FUNC6(funcNarf__,retType__,6,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)

#define i_RP_DECLARE_FUNC6(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC6_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,6),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,6))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)

#define i_RP_DECLARE_FUNC6_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)\
retType__ funcNarf__##6(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6);
#define i_RP_DECLARE_FUNC6_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)\
retType__ funcNarf__##6(arg__1,arg__2,arg__3,arg__4,arg__5,arg__6);\
i_RP_DECLARE_FUNC5(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)
#define i_RP_DECLARE_FUNC6_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)
#define i_RP_DECLARE_FUNC6_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5,arg__6)



//! This macro is used to declare a function with 5 arguments in a header file
#define RF_DECLARE_DFUNC5(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)\
i_RP_DECLARE_FUNC5(funcNarf__,retType__,5,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)

#define i_RP_DECLARE_FUNC5(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC5_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,5),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,5))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)

#define i_RP_DECLARE_FUNC5_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)\
retType__ funcNarf__##5(arg__1,arg__2,arg__3,arg__4,arg__5);
#define i_RP_DECLARE_FUNC5_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)\
retType__ funcNarf__##5(arg__1,arg__2,arg__3,arg__4,arg__5);\
i_RP_DECLARE_FUNC4(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)
#define i_RP_DECLARE_FUNC5_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)
#define i_RP_DECLARE_FUNC5_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4,arg__5)



//! This macro is used to declare a function with 4 arguments in a header file
#define RF_DECLARE_DFUNC4(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3,arg__4)\
i_RP_DECLARE_FUNC4(funcNarf__,retType__,4,defArgsN__,arg__1,arg__2,arg__3,arg__4)

#define i_RP_DECLARE_FUNC4(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC4_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,4),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,4))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)

#define i_RP_DECLARE_FUNC4_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)\
retType__ funcNarf__##4(arg__1,arg__2,arg__3,arg__4);
#define i_RP_DECLARE_FUNC4_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)\
retType__ funcNarf__##4(arg__1,arg__2,arg__3,arg__4);\
i_RP_DECLARE_FUNC3(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)
#define i_RP_DECLARE_FUNC4_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)
#define i_RP_DECLARE_FUNC4_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3,arg__4)



//! This macro is used to declare a function with 3 arguments in a header file
#define RF_DECLARE_DFUNC3(retType__,funcNarf__,defArgsN__,arg__1,arg__2,arg__3)\
i_RP_DECLARE_FUNC3(funcNarf__,retType__,3,defArgsN__,arg__1,arg__2,arg__3)

#define i_RP_DECLARE_FUNC3(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC3_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,3),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,3))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)

#define i_RP_DECLARE_FUNC3_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)\
retType__ funcNarf__##3(arg__1,arg__2,arg__3);
#define i_RP_DECLARE_FUNC3_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)\
retType__ funcNarf__##3(arg__1,arg__2,arg__3);\
i_RP_DECLARE_FUNC2(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)
#define i_RP_DECLARE_FUNC3_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)
#define i_RP_DECLARE_FUNC3_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2,arg__3)



//! This macro is used to declare a function with 2 arguments in a header file
#define RF_DECLARE_DFUNC2(retType__,funcNarf__,defArgsN__,arg__1,arg__2)\
i_RP_DECLARE_FUNC2(funcNarf__,retType__,2,defArgsN__,arg__1,arg__2)

#define i_RP_DECLARE_FUNC2(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC2_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,2),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,2))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)

#define i_RP_DECLARE_FUNC2_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)\
retType__ funcNarf__##2(arg__1,arg__2);
#define i_RP_DECLARE_FUNC2_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)\
retType__ funcNarf__##2(arg__1,arg__2);\
i_RP_DECLARE_FUNC1(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)
#define i_RP_DECLARE_FUNC2_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)
#define i_RP_DECLARE_FUNC2_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1,arg__2)



//! This macro is used to declare a function with 1 arguments in a header file
#define RF_DECLARE_DFUNC1(retType__,funcNarf__,defArgsN__,arg__1)\
i_RP_DECLARE_FUNC1(funcNarf__,retType__,1,defArgsN__,arg__1)

#define i_RP_DECLARE_FUNC1(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC1_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,1),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,1))(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)

#define i_RP_DECLARE_FUNC1_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)\
retType__ funcNarf__##1(arg__1);
#define i_RP_DECLARE_FUNC1_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)\
retType__ funcNarf__##1(arg__1);\
i_RP_DECLARE_FUNC0(funcNarf__,retType__,maxArgsN__,defArgsN__)
#define i_RP_DECLARE_FUNC1_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)
#define i_RP_DECLARE_FUNC1_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,arg__1)


//! This macro is used to declare a function with 0 arguments in a header file
#define i_RP_DECLARE_FUNC0(funcNarf__,retType__,maxArgsN__,defArgsN__)\
i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC0_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,0),i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,0))(funcNarf__,retType__,maxArgsN__,defArgsN__)
#define i_RP_DECLARE_FUNC0_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__)  retType__  funcNarf__##0();
#define i_RP_DECLARE_FUNC0_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__)
#define i_RP_DECLARE_FUNC0_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__)
#define i_RP_DECLARE_FUNC0_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__)
