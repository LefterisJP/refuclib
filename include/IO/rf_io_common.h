/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_COMMON_FLAGS_H
#define RF_COMMON_FLAGS_H

#include <sys/types.h>

/**
 ** This is the type that represents the file offset
 **/
#if _FILE_OFFSET_BITS == 64

#ifdef _MSC_VER
typedef __int64 RFfile_offset;
#else
typedef off64_t RFfile_offset;
#endif

/* for now, don't have a stat_t equivalent in windows */
typedef struct stat64 stat_rft;

#else /* __ FILE_OFFSET_BITS != 64 */

#ifdef _MSC_VER
typedef __int32 RFfile_offset;
#else

typedef off_t RFfile_offset;
#endif

/* for now, don't have a stat_t equivalent in windows */
typedef struct stat stat_rft;

#endif

/**
 * New line feed
 *
 */
#define RF_LF   0xA
/**
 * Carriage Return
 *
 */
#define RF_CR   0xD

#ifdef REFU_WIN32_VERSION
    #define i_PLUSB_WIN32   "b"
#else
    #define i_PLUSB_WIN32   ""
#endif

/**
 * The different end of line mark types
 */
enum RFeol_mark {
    RF_EOL_AUTO  = 1,/*!< Used by functions that can autodetect the eol mark */
    RF_EOL_LF, /*!< Unix style line ending with '\n' */
    RF_EOL_CRLF, /*!< Windows style line ending with '\r\n' */
    RF_EOL_CR /*!< Macintosh style line ending with '\r' */
};
#define RF_EOL_DEFAULT  RF_EOL_LF//the default value is LF only (Unix-style)

#endif//include guards end
