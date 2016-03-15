/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/string/files.h>

#include <rflib/string/conversion.h>
#include <rflib/string/manipulation.h>
#include <rflib/string/common.h>
#include "rf_str_files.ph"
#include "rf_str_common.ph"

#include <rflib/utils/log.h>
#include <rflib/utils/memory.h>
#include <rflib/io/rf_file.h>
#include <rflib/utils/sanity.h>

#include <stdio.h>

struct RFstring* rf_string_from_file_create(FILE* f, char* eof,
                                            enum RFeol_mark eol,
                                            enum RFtext_encoding encoding,
                                            enum RFendianess endianess,
                                            unsigned int* buff_size)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_string_from_file_init(ret, f, eof, eol, encoding, endianess, buff_size)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}

bool rf_string_from_file_init(struct RFstring* str, FILE* f, char* eof,
                              enum RFeol_mark eol,
                              enum RFtext_encoding encoding,
                              enum RFendianess endianess,
                              unsigned int* ret_buff_size)
{
    uint32_t buff_size;
    RF_ASSERT(str, "got null string in function");
    /* sanity checks for the rest of the arguments are performed further down */
    switch (encoding) {
        case RF_UTF8:
            if(!rf_file_read_line_utf8(f, eol, &rf_string_data(str),
                                 &rf_string_length_bytes(str), &buff_size, eof))
            {
                RF_ERROR("Failed to initialize String from a UTF-8 file");
                return false;
            }
            break;
        case RF_UTF16:
            if (!rf_file_read_line_utf16(f, eol, &rf_string_data(str),
                                         &rf_string_length_bytes(str), eof, &buff_size,
                                         endianess)) {
                RF_ERROR("Failure to initialize a String from reading"
                         " a UTF-16 file");
                return false;
            }
            break;
        case RF_UTF32:
            if (!rf_file_read_line_utf32(f, eol, &rf_string_data(str),
                                         &rf_string_length_bytes(str), eof,
                                         &buff_size, endianess))
            {
                RF_ERROR("Failure to initialize a String from reading"
                         " a UTF-32 file");
                return false;
            }
            break;
        default:
            RF_WARNING("Provided an illegal encoding value to function "
                     "rf_string_init_f()");
            return false;
            break;
    }
    if (ret_buff_size) {
        *ret_buff_size = buff_size;
    }
    return true;
}

bool rf_string_from_file_assign(struct RFstring* str,
                                FILE* f, char* eof,
                                enum RFeol_mark eol,
                                enum RFtext_encoding encoding,
                                enum RFendianess endianess)
{
    RF_ASSERT(str, "got null string in function");
    UTF_FILE_READLINE(f, eol, eof, "assign")
    //success
    //assign it to the string
    if (rf_string_length_bytes(str) <= utf8ByteLength) {
        RF_REALLOC(rf_string_data(str), char, utf8ByteLength,
                   ret = false; goto cleanup);
    }
    memcpy(rf_string_data(str), utf8, utf8ByteLength);
    rf_string_length_bytes(str) = utf8ByteLength;

  cleanup:
    //free the file's utf8 buffer
    free(utf8);
    return ret;
}

bool rf_string_from_file_append(struct RFstring* str,
                                FILE* f,
                                char* eof,
                                enum RFeol_mark eol,
                                enum RFtext_encoding encoding,
                                enum RFendianess endianess)
{
    UTF_FILE_READLINE(f, eol, eof, "append")
    RF_REALLOC(rf_string_data(str), char,
               rf_string_length_bytes(str) + utf8ByteLength,
               ret = false; goto cleanup
    );
    rf_string_generic_append(str, utf8, utf8ByteLength);

cleanup:
    //free the file's decoded utf8 buffer
    free(utf8);
    return ret;
}

//Writes a string to a file in the given encoding
bool rf_string_fwrite(const struct RFstring *s, FILE* f,
                      enum RFtext_encoding encoding,
                      enum RFendianess endianess)
{
    uint32_t *utf32,length;
    uint16_t* utf16;
    bool ret = true;
    RF_ASSERT(s, "got null string in function");

    //depending on the encoding
    switch(encoding)
    {
        case RF_UTF8:
            if(fwrite(
                   rf_string_data(s), 1,
                   rf_string_length_bytes(s), f) != rf_string_length_bytes(s))
            {
                break;//and go to error logging
            }
            goto cleanup1;//success
        break;
        case RF_UTF16:
            if((utf16 = rf_string_to_utf16(s, &length)) == NULL)
            {
                ret = false;
                RF_ERROR("Error in converting a string to UTF-16 LE");
                goto cleanup1;
            }
            rf_process_byte_order_u16A(utf16, length, endianess);
            if(fwrite(utf16, 2, length, f) != length)
            {
                free(utf16);
                break;//and go to error logging
            }
            free(utf16);
            goto cleanup1;//success
        break;
        case RF_UTF32:
            if((utf32 = rf_string_to_utf32(s, &length)) == NULL)
            {
                ret = false;
                RF_ERROR("Error in converting a string to LE UTF-32");
                goto cleanup1;
            }
            rf_process_byte_order_u32A(utf32, length, endianess);
            if(fwrite(utf32, 4, length, f) != length)
            {
                free(utf32);
                break;//and go to error logging
            }
            free(utf32);
            goto cleanup1;//success
        break;
        default:
            RF_ERROR("Illegal encoding value given to the function");
            ret = false;
            goto cleanup1;
            break;
    }
    //if we get here it means an error, and we log it
    RF_ERROR("Writting a string to a file failed due to "
             "fwrite errno %d", errno);
    ret = false;

cleanup1:
    return ret;
}
