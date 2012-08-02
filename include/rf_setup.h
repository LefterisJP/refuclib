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





/*******************************************************

*********************************************************/

#ifndef REFU_SETUP_H
#define REFU_SETUP_H

#include <stdio.h>
#include <float.h>
#include "Preprocessor/rf_xmacro_argcount.h" //for RF_NARG argument count macro
#include "Preprocessor/rf_xmacro_gt.h"    //for the greater than macro token comparison


#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#else
    //define true and false if we are calling from C
    #define true 1
    #define TRUE 1
    #define FALSE 0
    #define false 0
#endif


//include the options file that should have been created from building
#include "rf_options.h"

// Definition of the DLL/share library export macro
#ifdef REFU_WIN32_VERSION //windows case is the only one that needs special decl
    #ifdef REFU_DYNAMIC_LIB //export  case
        #define i_DECLIMEX_  __declspec(dllexport)
    #else//in import case or static library
        #ifndef REFU_STATIC_LIB //if this is not a static library
            #define i_DECLIMEX_  __declspec(dllimport)
        #else
			#define i_DECLIMEX_  //static means this should be empty
		#endif
    #endif
#else //in unix case or when testing
    #define i_DECLIMEX_
#endif

//include the exact c data types
#ifndef _MSC_VER
    #include <inttypes.h>
#else//msvc does not have the C99 standard header so we gotta define them explicitly here, since they do have some similar types
    typedef unsigned __int8 uint8_t;
    typedef __int8  int8_t;
    typedef unsigned __int16 uint16_t;
    typedef __int16 int16_t;
    typedef unsigned __int32 uint32_t;
    typedef __int32 int32_t;
    typedef unsigned __int64 uint64_t;
    typedef __int64 int64_t;
#endif

//! This is the last error code that occured
extern int32_t  i_DECLIMEX_ RF_LastError;
//! The standard log used by the library
extern i_DECLIMEX_ FILE* rfStdLog;
//! The standard error used by the library
extern i_DECLIMEX_ FILE* rfStdErr;


/**
** @defgroup RF_GeneralGRP Initialization function
** @addtogroup RF_GeneralGRP
** @{
**/

//! @brief Initializes the ReFu library
//!
//! This function needs to be called in the very beginning of any program using the library. It initializes everything that has to do with the library itself. Some of the things it does is:
//! + Initialize the error and info logging files
//! + Detect the endianess of the system and keep it so that the user can retrieve it with @ref rfUTILS_Endianess
//! + Initialize the local stack memory for the main thread
//! @param errstr \rfoptional{"refuErrorLog"} The filename of the error log to create and output the refu error log. You can provide any filename except for
//! @c "stdout" and @c "stderr" because they will be interpreted as special values for which the standard output stream or the standard error stream are used respectively
//! If no value is provided the default value is @c "refuErrorLog"
//! @param logstr \rfoptional{"refuInfoLog"} The filename of the info log to creeat and output the refu info log.You can provide any filename except for
//! @c "stdout" and @c "stderr" because they will be interpreted as special values for which the standard output stream or the standard error stream are used respectively
//! If no value is provided the default value is @c "refuInfoLog"
//! @param lmsSize \rfoptional{RF_OPTION_LOCALSTACKMEMORY_SIZE} The size of the main thread's local memory stack. This will be the size by which the
//! main thread's local memory stack will be initialized. It allows the usage of macros for temporary object creation such as @ref RFS_() or @ref RFXML_()
//! If no value is given the default value is to use the one provided at compile time by the Refu Builder and the
//! preprocessor directive @c RF_OPTION_LOCALSTACKMEMORY_SIZE.
#ifdef RF_IAMHERE_FOR_DOXYGEN
i_DECLIMEX_  void rfInit(char* errstr,char* logstr,uint64_t size);
#else
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
    i_DECLIMEX_  void i_rfInit(char* errstr,char* logstr,uint64_t size);
    #define rfInit(...) RF_SELECT_FUNC_IF_NARGGT(i_NPSELECT_RF_INIT,3,__VA_ARGS__)
    #define i_NPSELECT_RF_INIT1(...)  RF_COMPILE_ERROR("message \"Ileggal Arguments Number: Function rfStringInit() accepts from 0 to 3 arguments\"")
    #define i_NPSELECT_RF_INIT0(...)  RF_SELECT_FUNC(i_SELECT_RF_INIT,__VA_ARGS__)
    #define i_SELECT_RF_INIT3(...)  i_rfInit(__VA_ARGS__)
    #define i_SELECT_RF_INIT2(...)  i_rfInit(__VA_ARGS__,RF_OPTION_LOCALSTACKMEMORY_SIZE)
    #define i_SELECT_RF_INIT1(...)  i_rfInit(__VA_ARGS__,"refuInfoLog",RF_OPTION_LOCALSTACKMEMORY_SIZE)
    #define i_SELECT_RF_INIT0(...)  i_rfInit("refuErrorLog","refuInfoLog",RF_OPTION_LOCALSTACKMEMORY_SIZE)
#else
    i_DECLIMEX_  void rfInit(char* errstr,char* logstr,uint64_t size);
#endif
#endif
//! @}
//clsoing RF_GeneralGRP doxygen group





//! Sets all checks on
#define REFU_PEDANTIC_MODE
//! If this is on the String class is quite talkative and LOGS any undesired behaviour that might have happened. Helps with debugging
//! and in most cases should not be in a release library
#define VERBOSE_STRING
//! @def When this flag is on the Exceptions are also logging themselves on the error log
#define VERBOSE_EXCEPTION



//static const float MFLT_EPSILON= FLT_EPSILON;
//The system's epsilon tends to be too small some times so lets choose an arbirtrary small floating epsilon
//! Smallest floating point of the system, last known working value was 0.0001. Changed it to FLT_EPSILON to see if it works now.
//static const float FLOAT_EPSILON    = 0.0001;
static const float FLOAT_EPSILON    = FLT_EPSILON ;//from cfloat.h
static const float DOUBLE_EPSILON   = DBL_EPSILON;
//! Maximum finite value representable by a float
static const float FLOAT_MAX        =  FLT_MAX;
//! Minimum positive value representable by a float
static const float FLOAT_MIN        =  FLT_MIN;

//! This is a "trick" to produce compiler errors from macro
//! You have to write the error starting with message and with the
//! error message enclosed in ""
#ifdef __GNUC__
#define RF_COMPILE_ERROR(i_ERROR_STR)   ;_Pragma(i_ERROR_STR);\
                                        COMPILE_ERROR
#else
    #error Unsupported Compiler
#endif



//!  Returns true if float a > float b
#define FLCMP_G(a__,b__)  (a__)-(b__) > FLOAT_EPSILON
//!  Retuns true if float a >= float b
#define FLCMP_GE(a__,b__)   (a__)-(b__) >= 0
//!  Returns true if float a < float b
#define FLCMP_L(a__,b__)    (b__)-(a__) > FLOAT_EPSILON
//!  Returns true if float a <= float b
#define FLCMP_LE(a__,b__)   (b__)-(a__) >= 0
//!  Returns true if float a == float b
#define FLCMP_EQ(a__,b__)   fabs( (a__)-(b__))<FLOAT_EPSILON

//!  Returns true if float a > float b
#define DBLCMP_G(a__,b__)  a__-b__>DOUBLE_EPSILON
//!  Retuns true if float a >= float b
#define DBLCMP_GE(a__,b__)   a__-b__>=0
//!  Returns true if float a < float b
#define DBLCMP_L(a__,b__)    b__-a__>DOUBLE_EPSILON
//!  Returns true if float a <= float b
#define DBLCMP_LE(a__,b__)   b__-a__>=0
//!  Returns true if float a == float b
#define DBLCMP_EQ(a__,b__)   fabs(a__-b__)<DOUBLE_EPSILON


/**
** @defgroup RF_ErrorLoggingGRP Error Logging
** @addtogroup RF_ErrorLoggingGRP
** @{
**/

#ifndef RF_NO_C99
//! @brief Logs an error to the refu standard error stream
//!
//! @param i_msg_ A string literal containing the message to be logged for the error in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param i_errorCode_ The error code to log. Must be an int32_t value
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define LOG_ERROR(i_msg_,i_errorCode_,...)  i_PICK_LOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,RF_NARG(__VA_ARGS__),__VA_ARGS__)

//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,i_argN,...) i_PICK_LOG_ERROR_FUNC(i_msg_,i_errorCode_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_ERROR_FUNC(i_msg_,i_errorCode_,i_gt0,...) i_RP_PASTE2(i_LOG_ERROR,i_gt0) (i_msg_,i_errorCode_,__VA_ARGS__)

//Logs an error to the refu standard error stream with varargs
#define i_LOG_ERROR1(i_msg_,i_errorCode_, ...)  {fprintf(rfStdErr,"At \"%s\" >>> "i_msg_"\n",__FUNCTION__,__VA_ARGS__);RF_LastError = i_errorCode_;fflush(rfStdErr);}
//Logs an error to the refu standard error stream without varargs
#define i_LOG_ERROR0(i_msg_,i_errorCode_, ...)  {fprintf(rfStdErr,"At \"%s\" >>> "i_msg_"\n",__FUNCTION__);RF_LastError = i_errorCode_;fflush(rfStdErr);}


//! @brief Logs an error to the standard error stream in detail
//!
//! Detail means that it also saves the file and line number it occured
//! @param i_msg_ A string literal containing the message to be logged for the error in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param i_errorCode_ The error code to log. Must be an int32_t value
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define DLOG_ERROR(i_msg_,i_errorCode_, ...)  i_PICK_DLOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,RF_NARG(__VA_ARGS__),__VA_ARGS__)
//This macro picks up the correct error logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_DLOG_ERROR_FUNC_GT(i_msg_,i_errorCode_,i_argN,...) i_PICK_DLOG_ERROR_FUNC(i_msg_,i_errorCode_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct error logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_DLOG_ERROR_FUNC(i_msg_,i_errorCode_,i_gt0,...) i_RP_PASTE2(i_DLOG_ERROR,i_gt0) (i_msg_,i_errorCode_,__VA_ARGS__)

// Logs an error to the refu standard error stream in detail with varargs
#define i_DLOG_ERROR1(i_msg_,i_errorCode_, ...)   {fprintf(rfStdErr,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__VA_ARGS__,__FILE__, __LINE__,__FUNCTION__ ); RF_LastError = i_errorCode_;fflush(rfStdErr);}
// Logs an error to the refu standard error stream in detail without varargs
#define i_DLOG_ERROR0(i_msg_,i_errorCode_, ...)   {fprintf(rfStdErr,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__FILE__, __LINE__,__FUNCTION__ );RF_LastError = i_errorCode_; fflush(rfStdErr);}


#ifdef REFU_WIN32_VERSION
//! @brief Gets Windows Last System error
//!
//! Gets Windows Last System error and turns it into a char* that later needs to be freed by the user using the @c LocalFree() function
//! @param i_STRBUFF The String buffer
#define RF_WIN32_GETSYSERROR(i_STRBUFF) \
/*The buffer to hold the string*/\
char* i_STRBUFF;\
/*The DWORD to hold the error code*/\
DWORD i_ERROR_CODE = GetLastError();\
FormatMessage(\
FORMAT_MESSAGE_ALLOCATE_BUFFER |\
FORMAT_MESSAGE_FROM_SYSTEM |\
FORMAT_MESSAGE_IGNORE_INSERTS,\
NULL,\
i_ERROR_CODE,\
MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
(LPTSTR) &i_STRBUFF,\
0, NULL );
#endif


//! @brief Logs something in the standard information log
//! @param i_msg_ A string literal containing the message to be logged in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define LOG_INFO(i_msg_,...)  i_PICK_LOG_INFO_FUNC_GT(i_msg_,RF_NARG(__VA_ARGS__),__VA_ARGS__)

//This macro picks up the correct logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_LOG_INFO_FUNC_GT(i_msg_,i_argN,...) i_PICK_LOG_INFO_FUNC(i_msg_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_LOG_INFO_FUNC(i_msg_,i_gt0,...) i_RP_PASTE2(i_LOG_INFO,i_gt0) (i_msg_,__VA_ARGS__)

//Logs something to the standard refu log with var_args
#define i_LOG_INFO1(i_msg_, ...)  {fprintf(rfStdLog,i_msg_"\n",__VA_ARGS__);fflush(rfStdLog);}
//Logs something to the standard refu log without var_args
#define i_LOG_INFO0(i_msg_, ...)   {fprintf(rfStdLog,i_msg_"\n");fflush(rfStdLog);}

//! @brief Logs something in the standard information log with details
//!
//! Detail means that it also saves the file and line number it occured
//! @param i_msg_ A string literal containing the message to be logged in the file. Can contain printf format specifiers which will be translated into values from the variables passed as further parameters to the macro
//! @param ... This is an optional input. If you have given any printf-style format specifiers in the string literal passes @c i_msg_ then here give the variables from which the value will be taken
#define DLOG_INFO(i_msg_,...)  i_PICK_DLOG_INFO_FUNC_GT(i_msg_,RF_NARG(__VA_ARGS__),__VA_ARGS__)

//This macro picks up the correct logging function, by taking the number of extra arguments and checking if it is greater than 0
#define i_PICK_DLOG_INFO_FUNC_GT(i_msg_,i_argN,...) i_PICK_DLOG_INFO_FUNC(i_msg_,RP_GT(i_argN,0),__VA_ARGS__)
//This macro picks up the correct logging function, by pasting the results of the greater than comparison together to select the correct macro
#define i_PICK_DLOG_INFO_FUNC(i_msg_,i_gt0,...) i_RP_PASTE2(i_DLOG_INFO,i_gt0) (i_msg_,__VA_ARGS__)

//Logs something in detail to the standard refu log with var_args
#define i_DLOG_INFO1(i_msg_, ...)   {fprintf(rfStdLog,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__VA_ARGS__,__FILE__, __LINE__,__FUNCTION__ ); fflush(rfStdLog);}
// Logs something in detail to the standard refu log without var_args
#define i_DLOG_INFO0(i_msg_, ...)   {fprintf(rfStdLog,i_msg_ "\nLogged At File=%s at line %d inside %s()function\n",__FILE__, __LINE__,__FUNCTION__ ); fflush(rfStdLog);}


#else// if the library is compiled with no C99 features on
///In this case the logs differ quite a lot since the function name and file and line can't be logged. So in MSVC's case this is simply reduced to error message logging
void LOG_ERROR(const char* msg,int32_t errorCode,...);
void DLOG_ERROR(const char* msg,int32_t errorCode,...);
void LOG_INFO(const char* msg,...);
void DLOG_INFO(const char* msg,...);
#endif

//! @brief Returns the error code of the last error that occured
#define RF_GETLASTERROR()  RF_LastError
//! @brief Returns the standard refu log file descriptor
#define RF_GET_LOG()      rfStdLog
//! @brief Returns the standard refu error file descriptor
#define RF_GET_ERR()      rfStdErr


//!@}
//end of the Doxygen RF_ErrorLoggingGRP group

//Here are some macro wrappers of malloc,calloc and realloc that depending on the flag RF_OPTION_SAFE_MEMORY_ALLOCATION check their return value or not

//for realloc I check no matter what since it's a bit more complicated case than the other twoo
//! Wrapper macro of the malloc() function that does check for memory allocation failure
//! @param REALLOC_RETURN_ Give the pointer that you need to realloc here
//! @param TYPE_ give the type of the pointer here
//! @param SIZE_ Give the size parameter of realloc here
#define RF_REALLOC(REALLOC_RETURN_,TYPE_,SIZE_)  { TYPE_* i_TEMPPTR_ = realloc( (REALLOC_RETURN_),(SIZE_));\
                                                  if(i_TEMPPTR_ == NULL)\
                                                  {\
                                                    DLOG_ERROR("realloc() failure",RE_REALLOC_FAILURE);\
                                                    exit(RE_REALLOC_FAILURE);\
                                                  }\
                                                  REALLOC_RETURN_ = i_TEMPPTR_;\
                                                  }

#ifdef RF_OPTION_SAFE_MEMORY_ALLOCATION
//! Wrapper macro of the malloc() function that does check for memory allocation failure
//! @param MALLOC_RETURN_ Give the pointer of that you want to point to malloc's return here
//! @param MALLOC_SIZE_   Give the size parameter of malloc here
    #define RF_MALLOC(MALLOC_RETURN_,MALLOC_SIZE_)  { MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) );\
                                                     if(MALLOC_RETURN_ == NULL)\
                                                     {\
                                                        DLOG_ERROR(" malloc() failure",RE_MALLOC_FAILURE);\
                                                        exit(RE_MALLOC_FAILURE);\
                                                     } }

//! Wrapper macro of the calloc() function that does check for memory allocation failure
//! @param CALLOC_RETURN_ Give the pointer of that you want to point to calloc's return here
//! @param CALLOC_NUM_    Give the number parameter of calloc here
//! @param MALLOC_SIZE_   Give the size parameter of calloc here
    #define RF_CALLOC(CALLOC_RETURN_,CALLOC_NUM_,CALLOC_SIZE_)  { CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) );\
                                                     if(CALLOC_RETURN_ == NULL)\
                                                     {\
                                                        DLOG_ERROR(" calloc() failure",RE_CALLOC_FAILURE);\
                                                        exit(RE_CALLOC_FAILURE);\
                                                     } }

#else
//! Wrapper macro of the malloc() function that does NOT check for memory allocation failure
//! @param MALLOC_RETURN_ Give the pointer of that you want to point to malloc's return here
//! @param MALLOC_SIZE_   Give the size parameter of malloc here
    #define RF_MALLOC(MALLOC_RETURN_,MALLOC_SIZE_)  { MALLOC_RETURN_ = malloc( (MALLOC_SIZE_) );}

//! Wrapper macro of the calloc() function that does NOT check for memory allocation failure
//! @param CALLOC_RETURN_ Give the pointer of that you want to point to calloc's return here
//! @param CALLOC_NUM_    Give the number parameter of calloc here
//! @param MALLOC_SIZE_   Give the size parameter of calloc here
    #define RF_CALLOC(CALLOC_RETURN_,CALLOC_NUM_,CALLOC_SIZE_)  { CALLOC_RETURN_ = calloc( (CALLOC_NUM_), (CALLOC_SIZE_) );}
#endif

    /** Comparisons with hex values macros. USE THEM. They make sure that a value becomes what you expect it to be, depending on the macro. They simply do a cast for you, nothing more.
        Main reason they were created is because I wanted the to become practise so that I always do safe comparisons from now on by using them and never simple comparsions**/


///chars

//! A convenience macro to allow us to compare easily char bytes with hex values for equality.
#define RF_HEXEQ_C(c__,h__)    (c__==(char)h__)
//! A convenience macro to allow us to compare easily unsigned char bytes with hex values for equality.
#define RF_HEXEQ_UC(c__,h__)    (c__==(unsigned char)h__)

//! A convenience macro to allow us to compare easily char bytes with hex values for greater or equal.
#define RF_HEXGE_C(c__,h__)    (c__>=(char)h__)
//! A convenience macro to allow us to compare easily unsigned char bytes with hex values for greater or equal.
#define RF_HEXGE_UC(c__,h__)    (c__>=(unsigned char)h__)

//! A convenience macro to allow us to compare easily char bytes with hex values for less or equal.
#define RF_HEXLE_C(c__,h__)    (c__<=(char)h__)
//! A convenience macro to allow us to compare easily unsigned char bytes with hex values for less or equal.
#define RF_HEXLE_UC(c__,h__)    (c__<=(unsigned char)h__)

//! A convenience macro to allow us to compare easily char bytes with hex values for greater.
#define RF_HEXG_C(c__,h__)    (c__>(char)h__)
//! A convenience macro to allow us to compare easily unsigned char bytes with hex values for greater.
#define RF_HEXG_UC(c__,h__)    (c__>(unsigned char)h__)

//! A convenience macro to allow us to compare easily char bytes with hex values for less.
#define RF_HEXL_C(c__,h__)    (c__<(char)h__)
//! A convenience macro to allow us to compare easily unsigned char bytes with hex values for less.
#define RF_HEXL_UC(c__,h__)    (c__<(unsigned char)h__)

///shorts

//! A convenience macro to allow us to compare easily shorts with hex values for equality.
#define RF_HEXEQ_S(c__,h__)    (c__==(int16_t)h__)
//! A convenience macro to allow us to compare easily shorts with hex values for equality.
#define RF_HEXEQ_US(c__,h__)    (c__==(uint16_t)h__)

//! A convenience macro to allow us to compare easily shorts with hex values for greater or equal.
#define RF_HEXGE_S(c__,h__)    (c__>=(int16_t)h__)
//! A convenience macro to allow us to compare easily shorts with hex values for greater or equal.
#define RF_HEXGE_US(c__,h__)    (c__>=(uint16_t)h__)

//! A convenience macro to allow us to compare easily shorts with hex values for less or equal.
#define RF_HEXLE_S(c__,h__)    (c__<=(int16_t)h__)
//! A convenience macro to allow us to compare easily shorts with hex values for less or equal.
#define RF_HEXLE_US(c__,h__)    (c__<=(uint16_t)h__)

//! A convenience macro to allow us to compare easily shorts with hex values for greater.
#define RF_HEXG_S(c__,h__)    (c__>(int16_t)h__)
//! A convenience macro to allow us to compare easily shorts bytes with hex values for greater.
#define RF_HEXG_US(c__,h__)    (c__>(uint16_t)h__)

//! A convenience macro to allow us to compare easily shorts with hex values for less.
#define RF_HEXL_S(c__,h__)    (c__<(int16_t)h__)
//! A convenience macro to allow us to compare easily shorts with hex values for less.
#define RF_HEXL_US(c__,h__)    (c__<(uint16_t)h__)


///ints

//! A convenience macro to allow us to compare easily ints with hex values for equality.
#define RF_HEXEQ_I(c__,h__)    (c__==(int32_t)h__)
//! A convenience macro to allow us to compare easily ints with hex values for equality.
#define RF_HEXEQ_UI(c__,h__)    (c__==(uint32_t)h__)

//! A convenience macro to allow us to compare easily ints with hex values for greater or equal.
#define RF_HEXGE_I(c__,h__)    (c__>=(int32_t)h__)
//! A convenience macro to allow us to compare easily ints with hex values for greater or equal.
#define RF_HEXGE_UI(c__,h__)    (c__>=(uint32_t)h__)

//! A convenience macro to allow us to compare easily ints with hex values for less or equal.
#define RF_HEXLE_I(c__,h__)    (c__<=(int32_t)h__)
//! A convenience macro to allow us to compare easily ints with hex values for less or equal.
#define RF_HEXLE_UI(c__,h__)    (c__<=(uint32_t)h__)

//! A convenience macro to allow us to compare easily ints with hex values for greater.
#define RF_HEXG_I(c__,h__)    (c__>(int32_t)h__)
//! A convenience macro to allow us to compare easily ints bytes with hex values for greater.
#define RF_HEXG_UI(c__,h__)    (c__>(uint32_t)h__)

//! A convenience macro to allow us to compare easily ints with hex values for less.
#define RF_HEXL_I(c__,h__)    (c__<(int32_t)h__)
//! A convenience macro to allow us to compare easily ints with hex values for less.
#define RF_HEXL_UI(c__,h__)    (c__<(uint32_t)h__)




//! Flag denoting that any argument is sacceptable
#define RF_ANY  0
//! Flag denoting that no value should be changed.
#define RF_NONE 0
// Return codes These should always be considered as 0 for success and negative for some very generic error. More specific error codes are declared for the functiosn they are found in.
// These error codes should always fit in a char
#define RF_SUCCESS                      0
#define RF_FAILURE                      -1

//! This macro defines the highest error value used by the refu library.
//! As a user it is is safe to define errors with values more negative
//! than this
#define RF_HIGHEST_ERROR    RE_STRING_INIT_FAILURE
/// These are the various error codes of the refu library. Should be returned inside an int32_t. They are all guaranteed to be negative while RF_SUCCESS is 0 so in most cases.
/// Notice that in contrast with almost anything else in the library the error codes start with RE_ for REFU ERROR instead of RF_
/// A check of negative value can showcase if there was an error or not. Not all of these errors are actually returned by a function. Only those that can be handled or need to be given
/// to the user of the function may be returned by a function. The rest are logged using the logging functions.
// LIST OF REFU LIBRARY ERRORS
enum {

        /* String Related errors */
        RE_STRING_INIT_FAILURE=-200000,//Happens when there is a string initialization failure
        RE_STRING_FINIT_FAILURE ,//Happens when there is a string initialization from file failure
        RE_STRING_FREAD_FAILURE,//Happens when a string read a line from an open file and there is an error at read
        RE_STRING_ASSIGNMENT,//Error for assignment of one string to another
        RE_STRING_FREAD_BUFFER_SIZE,//Error in reading a String from a file due to the buffer size not big enough to fit it
        RE_STRING_TOFLOAT_RANGE, //Error in converting a string to float. The number is out of the representable range
        RE_STRING_TOFLOAT,  //Error in converting a string to a float. The string seems to not represent a float
        RE_STRING_TOFLOAT_UNDERFLOW,// While converting a string to a float the represented value caused underflow
        RE_STRING_VSPRINTF, // Inside a string function vsprintf failed

        /*General Errors*/
        RE_INPUT, //When a function has been given illegal input
        RE_INDEX_OUT_OF_BOUNDS, //When a given index for an operation is out of bounds

        /*OS related errors */
        RE_INTERRUPT, //Means that there was an interrupt signal
        RE_INSUFFICIENT_RESOURCES,//Insufficient system resources to perform the operaiton
        RE_INSUFFICIENT_MEMORY, //Insufficient memory to perforn the operation
        RE_PERMISSION, //The caller does not have sufficient permission to perform the operation
        RE_UNSUPPORTED, //The system does not support the operation

        /* Memory allocation errors */
        RE_REALLOC_FAILURE,//Happens when realloc can not work due to insufficient memory or segmentation. A pretty bad error code
        RE_MALLOC_FAILURE,//Error during memory allocation using malloc()
        RE_CALLOC_FAILURE,//Error during memory allocation using calloc()

        /* XML Related errors */
        RE_XML_OPEN_FAILURE,//Happens when the xml file can not be opened, or created
        RE_XML_PARSE_FAILURE,//Happens when there is a general parsing error of an XML file
        RE_XML_STRING_FREAD,//Error during reading a line of the XML file into a String. fread failed
        RE_XML_UTF_ENCODING, //Error during reading  a line of the XML file into a string. Unexpected UTF encoding
        RE_XML_OPEN_TAG,//Error during XML parsing if failing to find an xml opening tag
        RE_XML_UNEXPECTED_EOF,//Error during XML parsing. Unexcepted End of file
        RE_XML_EOF,//Error during traversing the XML file in the disk. Just denotes that we reached the closing of the root tag
        RE_XML_READLINE,//Error in XML parsing, a line of the file is corrupted
        RE_XML_WRITE,//Error in XML output writting, failed to write a line of the file
        RE_XML_ROOT_NOCHILDREN, //if the root tag has no children and an operation was requested
        RE_XML_TOSTR, // if during conversion of a tag to a string there was an error
        RE_XML_NOFILEOP, //if no XML file traversal functions have been called, yet an operation that needs them attempted to occur
        RE_XML_NOT_INDISK, //if the xml handler in question should have been opened as an in-disk only handler but is not
        RE_XML_ILLEGAL_OPENFLAG, // if an invalid XML opening flag was given during creation of an XML handler

        /* Date and Time errors */
        RE_TIMER_ILLEGAL_RESOLUTION,//Error initializing/querying a timer, with an illegal resolution
        RE_TIMER_HIGHRES_UNSUPPORTED,//Error initializing a timer, the system does not support high resolution performance counter
        RE_TIMER_POSIX,//Error occured while using a posix timer function.
        RE_TIMER_QPC,//Error occured while using Windows Performance Counter function
        RE_DATE_ILLEGAL_PARAM,//Illegal RF_Date parameters given
        RE_DATE_SYSTEM_ERROR,//Error during initialization of a Date in Linux. The system call returned an error
        RE_DATE_SET_SYSTEMTIME,//Error during setting the sytem time
        RE_DATE_ILLEGAL_MONTH, //Occurs when an illegal month value is encountered

        /* Data Structure Errors */
        RE_LIST_INIT_FAILURE,//Happens when there is a failure at list initialization. Mostly because of 0 size given.
        RE_ARRAY_INIT_FAILURE,//Happens when there is a failure at array initialization. Mostly because of 0 size given.
        RE_ARRAY_UNINITIALIZED_OBJECT,//Uninitialized array object requested
        RE_ARRAY_UNINITIALIZED_POINTER,//Uninitialized array pointer requested
        RE_FUNCGROUP_INIT_FAILURE,//Failure at the initialization of a function group
        RE_FUNCGROUP_ADD_FAILURE,//Failure at adding an element in a function group

        /* Math Errors */
        RE_MATH_RANDOM_BETWEEN_RANGE,//Error for when using a call to random numbers in a range with the max being smaller than the min

        /* Util Errors */
        RE_UTILS_COPYBITS_BETWEEN_RANGE,//Error for when trying to copy bits from an array of bytes to another due to giving a smaller end to the range than the start
        RE_UTILS_COPYBITS_STARTBIT_ILLEGAL,//Error for when setting an illegal start bit for bit copying. Bits start counting from 1 until 8 for a single byte (including 8), unlike 0 - 7 as is usual
        RE_UTILS_BYTES_TO_FLOAT_NAN,//Error for when trying to convert bytes to float and get NaN

        /* Threading Related Errors */
        RE_THREAD_NULL_EXECUTION_FUNCTION,//Error for when a null function pointer is passed for thread's execution
        RE_THREAD_CREATION,//Error during thread creation
        RE_WAITFORSINGLEOBJECT,//Error during Win32 call of WaitForSingleObject
        RE_THREAD_KILL,//Error during thread killing
        RE_THREAD_INVALID, //The thread handle is invalid
        RE_THREAD_NOTJOINABLE,//Error during attempting to join a thread because it is not joinable
        RE_THREAD_JOIN,//Generic Thread Join error
        RE_THREAD_DEADLOCK,//A Deadlock was detected on the thread

        /* Mutex Related Errors */
        RE_MUTEX_INIT,//Error encountered during initialization of a Mutex object
        RE_MUTEX_UNLOCK,//Error during unlocking of a mutex
        RE_MUTEX_LOCK,//Error during locking a mutex
        RE_MUTEX_LOCK_INVALID, //Error during locking a mutex because the mutex value is invalid, or the priorities don't allow it to get locked
        RE_MUTEX_LOCK_NUM, //Error during locking a posix mutex because the maximum number of recursive locks for the mutex has been reached.
        RE_MUTEX_DEADLOCK, // Error during locking a mutex because a deadlock was detected.
        RE_MUTEX_INVALID,//Error because the mutex value is invalid
        RE_MUTEX_BUSY, // The mutex is already locked/occupied
        RE_MUTEX_TIMEOUT, // The timeout given for the waiting for the mutex expired
        RE_MUTEX_DESTRUCTION,//Error during mutex destruction

        /* Semaphore Related Errors */
        RE_SEMAPHORE_INIT,//Error encountered during initialization of a Semaphore object
        RE_SEMAPHORE_MAXCOUNT, // Error because the given initial count exceeds the maximum semaphore count of the system
        RE_SEMAPHORE_DESTRUCTION,//Error during semaphore destruction
        RE_SEMAPHORE_WAIT,//Error during semaphore waiting
        RE_SEMAPHORE_WAIT_INVALID, //Error during waiting for a semaphore because its value is invalid, or the priorities don't allow it to get locked
        RE_SEMAPHORE_TIMEOUT, //During a timed waiting for a semaphore the waiting time has expired
        RE_SEMAPHORE_BUSY, //During a trywaiting for a semaphore the target semaphore's count is zero
        RE_SEMAPHORE_POST,//Error during attempting to unlock/release a semaphore
        RE_SEMAPHORE_POST_INVALID, //Error during posting a semaphore because its value is invalid, or the priorities don't allow it to get locked
        RE_SEMAPHORE_DEADLOCK, //Error during a semaphore operation because of deadlock detection

        /* UTF8 Byte stream processing errors */
        RE_UTF8_INVALID_SEQUENCE_END,//Error for encountering an unexpected End of string character in a UTF-8 byte sequence
        RE_UTF8_INVALID_SEQUENCE_CONBYTE,//Error for expecting a continuation byte in a UTF-8 byte sequence and not getting it
        RE_UTF8_INVALID_SEQUENCE_INVALID_BYTE,//Error for encountering a byte that is invalid in UTF-8
        RE_UTF8_INVALID_SEQUENCE,//General error for invalid UTF-8 byte sequence
        RE_UTF8_INVALID_CODE_POINT,//Error for attempting to encode an illegal unicode code point into UTF-8
        RE_UTF8_ENCODING, //Error during encoding a UTF-8 sequence

        /* Other unicode and unicode file reading errors */
        RE_UTF16_ENCODING, // Error while encoding unicode codepoints into UTF-16
        RE_UTF16_INVALID_SEQUENCE,//Error while attempting to decode UTF-16 byte sequence
        RE_UTF16_NO_SURRPAIR, //Error while attempting to decode UTF-16 and not finding a surrogate pair
        RE_FUTF32_INSUFFICIENT_BUFFER_SIZE,//Insufficient buffer size given as parameter in UTF32 file reading
        RE_FUTF16_INSUFFICIENT_BUFFER_SIZE,//Insufficient buffer size given as parameter in UTF16 file reading
        RE_FUTF8_INSUFFICIENT_BUFFER_SIZE,//Insufficient buffer size given as parameter in UTF8 file reading
        RE_FUTF16_FILE_READING,//Error encountered during file reading of a UTF-16 file
        RE_FUTF32_FILE_READING,//Errror encountered during file reading of a UTF-32 file
        RE_FUTF8_FILE_READING,//Errror encountered during file reading of a UTF-8 file

        /* Networking Errors */
        RE_SOCKET_WIN32_INIT,//Failure at windows socket initialization
        RE_SOCKET_GETADDRINFO,//Failure at getaddrinfo
        RE_HTTP_BAD_ADDRESS,//Bad Address parameter at an http request
        RE_HTTP_RECEIVE,//Error at receiving an http response from the server
        RE_HTTP_RECEIVE_SERVER_CLOSED,//Error at receiving an http response from the server because the server closes the connection
        RE_SOCKET_SETSOCKOPT,//Error at setsockopt
        RE_SOCKET_BIND_PORT,//Error at binding port for listening
        RE_NET_LISTEN,//Error at listening at a specific port

        /* I/O and File Errors */
        RE_FILE_INIT, //Failure at creating or intiializing a file
        RE_FILE_MODE, //Failure at file creation due to illegal mode given at a function or a mode that does not match the current operation
        RE_FILE_PERMISSION,//Failure to perform an operation on a file due to insufficient permissions for said operation
        RE_FILE_NOTFILE, //Means that the given file name is not actually a file. Either wrong format or belongs to a directory or is a socket/pipe/FIFO descriptor. In writes this means that the device holding it is non existent.
        RE_FILE_MAXFILES, //Occurs if the process has reached the maximum number of open files/filedescriptors allowed by the system
        RE_FILE_NAMELENGTH,// Occurs if the name of the file or the directory the file is in exceeds the maximum name length allowed by the system
        RE_FILE_NOSPACE, // Occurs if there is no space for the requested operation on the file
        RE_FILE_TOOBIG, // Occurs if the file is too big for us to operate on
        RE_FILE_ILLEGAL_ENCODING, //Occurs if the programmer entered an illegal value for the encoding parameter
        RE_FILE_ENCODING_ERROR, //Occurs if the process is reading a file and the byte stream does not match the expected encoding
        RE_FILE_POS_OVERFLOW, //Occurs if at an attempt to get the file position the current position is not representable by an fpos_t
        RE_FILE_GETFILEPOS, // Occurs if fgetpos() failed for unknown reason
        RE_FILE_EOF,  //Occurs if the end of file has been reached
        RE_FILE_WRITE, // Occurs if there is a problem with writing to a file
        RE_FILE_WRITE_BLOCK, //Occurs if the write operation failed due to the file descriptor being occupied by some other thread and the O_NONBLOCK flag been set
        RE_FILE_READ, // Occurs if there is a problem with reading from a file
        RE_FILE_READ_BLOCK, //Occurs if the read operation failed due to the file descriptor being occupied by some other thread and the O_NONBLOCK flag is set
        RE_FILE_BAD, // Occurs if the file we are trying to read/write has an illegal file descriptor
        RE_FILE_IO, // Occurs if there was a physical I/O error during the file operation
        RE_FILE_DELETE,//Occurs if there was an inability to delete a file from the file system
        RE_FILE_RENAME,//Occurs if there was an inability to rename a file from the file system
        RE_POPEN_INVALID_MODE, //Occurs if an invalid mode argument was passed to rfPopen
        RE_FILE_BUSY,// Occurs if the operation to be done to the file failed because the file was accessed by another process or by the system
        RE_FILE_RO,// Occurs if the operation to be done to the file failed because it is a read only file
        RE_FILE_MODE_CHANGE, //Occures if an attempt to change the file handler's opening mode, from writting to reading or vice versa failed
        RE_FILE_MOVEPOS, //Occurs if an illegal movement of the internal file pointer is attempted

        /* System Errors */
        RE_DIRECTORY_EXISTS, //Failure at creating a directory because it already exists
        RE_DIRECTORY_INVALID, // Failure to create a directory because the path was invalid
        RE_DIRECTORY_CREATE, // Generic directory creation error
        RE_DIRECTORY_PARENT_LINKS, // Very rare to occur. Can only happen if the parent directory has too many links
        RE_DIRECTORY_SPACE, // There is not enough space in the parent for the creation of a new directory
        RE_DIRECTORY_ROPARENT, //Parent directory is read only and can not be modified
        RE_DIRECTORY_NAMELENGTH,//Directory can't be accessed cause the name exceeds the filesystem's name length
        RE_DIRECTORY_TOOMANY_FILES,//Directory can't be accessed because too many files/file descriptors are open in the system
        RE_DIRECTORY_FILES,//Directory iteration failed because one of the values of the directory's file structure could not be represented correctly
        RE_DIRECTORY_STREAMPOS, // Current position of the directory stream is invalid
        RE_DIRECTORY_DELETE, //The directory itself could not be deleted
        RE_DIRECTORY_OPEN, //The directory itself could not be opened due to opendir() failure
        RE_DIRECTORY_READ, //The directory could not be read due to readdir() failure
        RE_DIRECTORY_BUSY, //The directory is currently in use by the system or some process and the implementation considers this to be an error
        RE_DIRECTORY_IO, // Occurs if there was a physical I/O error during the directory operation

        /* Local Memory Stack errors */
        RE_LOCALMEMSTACK_PUSH, // Attempted to push more memory into the local memory stack than it can handle
        RE_LOCALMEMSTACK_POP, // Attempted to pop (free) more memory from the local memory stack than it currently has
        RE_LOCALMEMSTACK_INSUFFICIENT, // The Local Memory Stack remaining is insufficient for an operation
        RE_LOCALMEMSTACK_MACROEVALS, // More macros that work with the local stack have been evaluated than the limit set by RF_MAX_FUNC_ARGS
};



#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif
