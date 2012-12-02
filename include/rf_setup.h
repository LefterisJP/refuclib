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





/*******************************************************

*********************************************************/

#ifndef REFU_SETUP_H
#define REFU_SETUP_H


#include <float.h> //for FLOAT_EPSILON, used in the safe floats comparisons
//both the includes below are used in all the default arguments cases
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

/* ---------------------------------------------------------- The needed export-import macros ------------------------------------------------------------ */
#ifdef REFU_WIN32_VERSION //windows case is the only one that needs special decl
    #ifdef REFU_DYNAMIC_LIB //export  case
        #define i_DECLIMEX_  __declspec(dllexport)
    #else//in import case or static library
        #if !( defined(REFU_STATIC_LIB) || defined(REFU_TEST)) //if this is not a static library
            #define i_DECLIMEX_  __declspec(dllimport)
        #else
			#define i_DECLIMEX_  //static means this should be empty
		#endif
    #endif
#else //in unix case or when testing
    #define i_DECLIMEX_
#endif

/* ------------------------------------------------------------ Exact data types ------------------------------------------------------------ */
#ifndef _MSC_VER
    #define __STDC_FORMAT_MACROS //also request the printf format macros
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

/* ------------------------------------------------------------ Macro Flags for System endianess ------------------------------------------------------------ */
//! Defines little endian system
#define RF_LITTLE_ENDIAN    0
//! Defines big endian system
#define RF_BIG_ENDIAN       1


/* ------------------------------------------------------------ Library initialization ------------------------------------------------------------ */

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
//closing RF_GeneralGRP doxygen group

/* ------------------------------------------------ A macro that hopefully generates a compile error  ------------------------------------------------------- */
#if defined(__GNUC__) || defined(__TINYC__) //GCC and TCC
    #define RF_COMPILE_ERROR(i_ERROR_STR)   ;_Pragma(i_ERROR_STR);\
                                        COMPILE_ERROR
#elif _MSC_VER //MSVC
    #define __STR2__(x) #x
    #define __STR1__(x) __STR2__(x)
    #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Error E0000: #pragma VSERROR: "
    #define RF_COMPILE_ERROR(x)  message(__LOC__ x)
#else
    #error Unsupported Compiler
#endif

/* ------------------------------------------------ Thread specific keyword  ------------------------------------------------------- */
#if defined(__GNUC__) || defined(__TINYC__) //GCC and TCC
    #define i_THREAD__ __thread
#elif _MSC_VER //MSVC
    #define i_THREAD__ __declspec(thread)
#else
    #error Unsupported Compiler
#endif

/* ------------------------------------------------ Inline functions handling ------------------------------------------------------- */
//kudos to this SO answer by Jens Gustedt http://stackoverflow.com/questions/5229343/how-to-declare-an-inline-function-in-c99-multi-file-project
#ifdef __GNUC__ //GCC
    #if !__GNUC_STDC_INLINE__ //old way to inline
        #define i_INLINE_DECL   static inline
        #define i_INLINE_INS    static inline
    #else //C99 way to inline
        #define i_INLINE_DECL   inline
        #define i_INLINE_INS    extern inline
    #endif
#else //GCC-end
    #error todo
#endif

/* --------------------------------------------- Safe Float and Double comparisons ------------------------------------------------------- */
// Smallest floating point of the system, last known working value was 0.0001. Changed it to FLT_EPSILON to see if it works now.
static const float FLOAT_EPSILON    = FLT_EPSILON ;//from cfloat.h
static const float DOUBLE_EPSILON   = DBL_EPSILON;
// Maximum finite value representable by a float
static const float FLOAT_MAX        =  FLT_MAX;
// Minimum positive value representable by a float
static const float FLOAT_MIN        =  FLT_MIN;
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


/* --------------------------------------------- Safe Hex Literal comparisons ------------------------------------------------------- */
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




/* --------------------------------------------- Function Return codes ------------------------------------------------------- */
// Return codes These should always be considered as 0 for success and negative for some very generic error. More specific error codes are declared for the functiosn they are found in.
// These error codes should always fit in an int32_t
#define RF_SUCCESS                      0
#define RF_FAILURE                      -1

// This macro defines the highest error value used by the refu library.
// As a user it is is safe to define errors with values more negative
// than this
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

        /* String Format errors */
        RE_FORMAT_UNEXPECTEDCHAR, //When during parsing the format string a legal control character is encountered at an unexpected position
        RE_FORMAT_ILLEGALCHAR,  // Occurs when during parsing the format string an illegal control character is encountered
        RE_FORMAT_ILLEGALPRECISION, //Occurs if after the precision character '.' an unexpected character is encountered
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

        /* stdio Errors */
        RE_STDIO_INIT, //Failure to initialize the stdio

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

        /* TextFile Errors */
        RE_TEXTFILE_EOL_DETECT, //Occurs if there was a failure to auto-detect the End Of Line pattern during the initialization of a TextFile


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

#endif//include guards end
