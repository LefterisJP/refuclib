/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_TEXTFILE_DECL_H
#define RF_TEXTFILE_DECL_H

/*------------- Module Related inclusion -------------*/
#include <IO/rf_io_common.h> //for RFeol_mark
/*------------- Outside Module inclusion -------------*/
#include <Definitions/types.h> //for fixed size types
#include <Definitions/retcodes.h> //for bool
#include <String/rf_str_decl.h> //for RFstring
#include <Utils/rf_unicode.h> //for RFtext_encoding
#include <Utils/endianess.h> //for RFendianess
/*------------- libc inclusion --------------*/
#include <stdio.h> //for FILE*
/*------------- End of includes -------------*/


/**
 * The possible modes in which an RFtextfile
 * can be opened
 */
enum RFtextfile_mode {
    RF_FILE_READ = 1,/*!< The file is open for reading */
    RF_FILE_WRITE, /*!< The file is open for writting */
    RF_FILE_READWRITE, /*!< the file is open for both reading and writting */
    RF_FILE_NEW, /*!< The file opens for writting and if already existing
                   its contents are erased */
    RF_FILE_READWRITE_NEW, /*!< Creates a new file for reading and writting.
                            If it already exists its  contents are erased */
    RF_FILE_STDIN, /*!< Read from stdin */
};

/**
 * @brief TextFile handler
 *
 * This is a TextFile handler that allows for the manipulation of files that contain
 * text in any of the UTF encodings.
 *
 * It is made to work with files and not generic streams like pipes. It can also
 * work with stdin but it admittedly feels like a hack, added over old code.
 *
 * It allows for manipulation of a text file without placing the whole file in memory
 * and contains many different functions for moving inside the file in regards to specific line positions
 * or even retrieving said lines themselves inside an @ref RF_String.
 * For individual line parsing it is recommended to use the @ref RFstring and @ref RFstringx functions.
 */
struct RFtextfile {
    //! The file descriptor
    FILE* f;
    //! The name of the file
    struct RFstring name;
    //! The current line number. Basically represents the line that was last
    //! read by any of the file's functions
    uint64_t line;
    //! The encoding of the text file
    enum RFtext_encoding encoding;
    //! The endianess of the file
    enum RFendianess endianess;
    //! A boolean flag denoting if the end of file was reached
    char eof;
    //! the mode of textfile opening
    enum RFtextfile_mode mode;
    //! the previous i/o operation on the file. Can be one of @c RF_FILE_READ,
    //! @c RF_FILE_WRITE
    char previousOp;
    //! A boolean flag denoting if the TextFile has a Byte Order Mark in the beginning or not
    char hasBom;
    //! A flag denoting what kind of EOL pattern this particular text file observes
    enum RFeol_mark eol;
};


#endif//include guards end
