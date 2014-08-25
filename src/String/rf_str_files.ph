#ifndef RF_FILES_PH
#define RF_FILES_PH


/* Macros commonly used in string file related functions */
#define UTF_FILE_READLINE(f, eol, eof, errstr)                          \
    uint32_t utf8ByteLength, bytes_read;                                \
    char* utf8 = 0, ret = true;                                         \
    switch(encoding)                                                    \
    {                                                                   \
        case RF_UTF8:                                                   \
            if(!rf_file_read_line_utf8(f, eol, &utf8,                   \
                                       &utf8ByteLength, &bytes_read, eof)) \
            {                                                           \
                RF_ERROR("UTF-8 string "errstr" failed");               \
                return false;                                           \
            }                                                           \
            break;                                                      \
        case RF_UTF16:                                                  \
            if(!rf_file_read_line_utf16(f, eol, &utf8, &utf8ByteLength, eof, \
                                        &bytes_read, endianess))        \
            {                                                           \
                RF_ERROR("UTF-16 string "errstr" failed");               \
                return false;                                           \
            }                                                           \
            break;                                                      \
        case RF_UTF32:                                                  \
            if(!rf_file_read_line_utf32(f, eol, &utf8, &utf8ByteLength, eof, \
                                        &bytes_read, endianess))        \
            {                                                           \
                RF_ERROR("UTF-32 string "errstr" failed");              \
                return false;                                           \
            }                                                           \
            break;                                                      \
    default:                                                            \
        RF_WARNING("Illegal encoding argument");                        \
        return false;                                                   \
        break;                                                          \
    }


#endif //include guards end
