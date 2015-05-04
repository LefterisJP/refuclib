/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <String/rf_str_filesx.h>
/*------------- Module related inclusion -------------*/
#include <String/rf_str_common.h> // for RFS_()
#include <String/rf_str_manipulationx.h> //for rf_stringx_append and others
#include <String/rf_str_traversalx.h> //for rf_stringx_reset and others
#include "rf_str_files.ph" //for some macros
#include <String/rf_str_files.h> //for rf_string_from_file_init e.t.c.
#include <IO/rf_file.h> //for rfReadLine family  of functions
#include "rf_str_manipulation.ph" //for rf_stringxget_append()
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h> //for error logging macros
#include <Utils/memory.h> //for memory allocation
/*------------- Libc inclusion -------------*/
#include <stdio.h> //for FILE*
#include <string.h> //for memcpy()
/*------------- End of includes -------------*/




// Allocates and returns an RFstringx from a file descriptor
struct RFstringx* rf_stringx_from_file_create(
    FILE* f, char* eof,
    enum RFeol_mark eol,
    enum RFtext_encoding encoding,
    enum RFendianess endianess
)
{
    struct RFstringx* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_stringx_from_file_init(ret, f, eof, eol, encoding, endianess))
    {
        free(ret);
        ret = NULL;
    }
    return ret;
}

bool rf_stringx_from_file_init(struct RFstringx* str,
                               FILE* f, char* eof,
                               enum RFeol_mark eol,
                               enum RFtext_encoding encoding,
                               enum RFendianess endianess)
{
    if(!rf_string_from_file_init(&str->INH_String, f, eof, eol,
                                 encoding, endianess, &str->bSize))
    {
        return false;
    }
    //success
    str->bIndex = 0;
    return true;
}

bool rf_stringx_from_file_assign(struct RFstringx* str,
                                 FILE* f, char* eof,
                                 enum RFeol_mark eol,
                                 enum RFtext_encoding encoding,
                                 enum RFendianess endianess)
{
    RF_ASSERT(str, "got null string in function");
    UTF_FILE_READLINE(f, eol, eof, "assign")
    //success
    //assign it to the string
    RF_STRINGX_REALLOC(str, utf8ByteLength, ret = false; goto cleanup);
    memcpy(rf_string_data(str), utf8, utf8ByteLength);
    rf_string_length_bytes(str) = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

bool rf_stringx_from_file_append(struct RFstringx* str,
                                 FILE* f, char* eof,
                                 enum RFeol_mark eol,
                                 enum RFtext_encoding encoding,
                                 enum RFendianess endianess)
{
    RF_ASSERT(str, "got null string in function");
    UTF_FILE_READLINE(f, eol, eof, "append")
    //append the utf8 to the given string
    if(!rf_stringx_generic_append(str, utf8, utf8ByteLength))
    {
        RF_ERROR("Failed to append a utf8 buffer to a stringX");
        ret = false;
    }
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}
