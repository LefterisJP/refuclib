/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
**
** --Definitions/retcodes.h
** This header contains the library's return codes
*/
#ifndef RF_RETURNCODES_H
#define RF_RETURNCODES_H


// Return codes These should always be considered as 0 for success and negative for some very generic error. More specific error codes are declared for the functiosn they are found in.
// These error codes should always fit in an int32_t
#define RF_SUCCESS                      0
#define RF_FAILURE                      -1

/* defining true/false/bool */
#ifndef __cplusplus

/* for C99 code assume we have stdbool.h */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <stdbool.h> 
#else
    #define true 1
    #define TRUE 1
    #define FALSE 0
    #define false 0
    typedef int bool;
#endif

#endif//end of not c++

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
        RE_STRING_APPEND, // Appending to a string failed

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
        RE_CALLOC_FAILURE,//Error during memory allocation using calloc()r

        /* Date and Time errors */
        RE_TIMER_ILLEGAL_RESOLUTION,//Error initializing/querying a timer, with an illegal resolution
        RE_TIMER_HIGHRES_UNSUPPORTED,//Error initializing a timer, the system does not support high resolution performance counter
        RE_TIMER_POSIX,//Error occured while using a posix timer function.
        RE_TIMER_QPC,//Error occured while using Windows Performance Counter function

        /* Data Structure Errors */
        RE_LIST_INIT_FAILURE,//Happens when there is a failure at list initialization. Mostly because of 0 size given.
        RE_ARRAY_INIT_FAILURE,//Happens when there is a failure at array initialization. Mostly because of 0 size given.
        RE_ARRAY_UNINITIALIZED_OBJECT,//Uninitialized array object requested
        RE_ARRAY_UNINITIALIZED_POINTER,//Uninitialized array pointer requested
        RE_FUNCGROUP_INIT_FAILURE,//Failure at the initialization of a function group
        RE_FUNCGROUP_ADD_FAILURE,//Failure at adding an element in a function group
        RE_LINKEDLIST_INIT_FAILURE, //Failure to initialize a linked list

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
        RE_UTF8_DECODING, // Errpr during decoding a UTF-8 sequence

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
        RE_POPEN_INVALID_MODE, //Occurs if an invalid mode argument was passed to rf_popen
        RE_FILE_BUSY,// Occurs if the operation to be done to the file failed because the file was accessed by another process or by the system
        RE_FILE_RO,// Occurs if the operation to be done to the file failed because it is a read only file
        RE_FILE_MOUNT,// Occurs if the operation happens between two different mounting points
        RE_FILE_MODE_CHANGE, //Occures if an attempt to change the file handler's opening mode, from writting to reading or vice versa failed
        RE_FILE_MOVEPOS, //Occurs if an illegal movement of the internal file pointer is attempted

        /* TextFile Errors */
        RE_TEXTFILE_EOL_DETECT, //Occurs if there was a failure to auto-detect the End Of Line pattern during the initialization of a TextFile
        RE_TEXTFILE_COPY, //Occurs when there is an error during copying a text file

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
        RE_LOCALMEMSTACK_INIT, // Initialization of local memory stack failed
        RE_LOCALMEMSTACK_PUSH, // Attempted to push more memory into the local memory stack than it can handle
        RE_LOCALMEMSTACK_POP, // Attempted to pop (free) more memory from the local memory stack than it currently has
        RE_LOCALMEMSTACK_INSUFFICIENT, // The Local Memory Stack remaining is insufficient for an operation
        RE_LOCALMEMSTACK_MACROEVALS, // More macros that work with the local stack have been evaluated than the limit set by RF_MAX_FUNC_ARGS
};


#endif//include guards end
