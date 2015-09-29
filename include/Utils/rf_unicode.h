/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_UNICODE_H
#define RF_UNICODE_H

/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Utils/constcmp.h> //for RF_HEXEQ_C
#include <Definitions/retcodes.h> //for bool
/*------------- End of includes -------------*/

/**
 ** Represents the text encoding format of a file or byte stream.
 */
enum RFtext_encoding {
    RF_UTF8 = 1, /*!< Unicode UTF-8 encoding */
    RF_UTF16, /*!< Unicode UTF-16 encoding */
    RF_UTF32 /*!< Unicode UTF-32 encoding */
};

/* maximum bytes of a UTF8 char */
#define MAX_UTF8C_BYTES  5


/**
 **  Checks if a given  byte (must be char and not unsigned char)
 **/
#define rf_utf8_is_continuation_byte(b__)  \
    ( RF_HEXEQ_C( (  (~( (b__) ^ 0x80))>>6),0x3 ))

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
** @defgroup RFunicode_GRP Unicode
** @addtogroup RFunicode_GRP
** @{
**/

/**
 ** @brief Takes a buffer of unicode characters and turns them into a
 **  UTF-8 encoded non null terminated string
 **
 ** This function accepts a buffer of unicode codepoints in @c codepoints and
 **  also requires to know the size of that buffer in characters
 ** as the @c charsN argument.
 **
 ** If successfull it will return the number of bytes in the encoded utf8 buffer
 ** inside the variable given at argument @c utf8Length and return the
 ** encoded utf8 inside @c utf8.
 **
 ** User should make sure that @c utf8Length and @c utf8 arguments are always
 ** provided and are never NULL.
 **
 ** @warning This function allocates the UTF-8 buffer internally and the caller
 **  of this function either has to store it or free it after use.
 ** @param[in] codepoints The buffer of code points. Must be given as a
 ** buffer of @c uint32_t.
 ** @param[in] charsN The number of codepoints inside the buffer,
 **  that is how many characters are in there
 ** @param[out] utf8Length Pass a pointer to @c uint32_t to receive inside
 **  it the bytelength of the allocated utf8 buffer
 ** @param[in/out] utf8 Here the caller has to provide a byte buffer of size
 ** great enough to hold the encoded utf8. To be safe provide a buffer of
 ** size charsN*4. Does not get null terminated.
 ** @param buffSize The size of the buffer given at @c utf8 in bytes
 ** @return Returns @c true for success and @c false otherwise
 **
 **/
i_DECLIMEX_ bool rf_utf8_encode(const uint32_t* codepoints,
                                uint32_t charsN, uint32_t* utf8Length,
                                char* utf8, uint32_t buffSize);

/**
 ** @brief Takes a unicode codepoint and turns them into a UTF-8 byte
 **
 ** If successfull it will return the number of bytes in the encoded utf8 buffer
 ** and return the UTF-8 encoded byte buffer in @c utf8. Make sure that the
 ** pointer passed at @c utf8 is not null. The function does not check for it.
 **
 ** @param[in] codepoint The codepoint to encode. Must be given as a @c uint32_t
 ** @param[out] utf8 Pass a buffer of at least 5 bytes here to receive
 **             the utf8 bytes. Does not get null terminated. Should never be
 **             NULL.
 ** @return Returns the number of bytes used for the utf8 conversion for
 ** success or @c -1 for error at encoding with additional error logging
 **
 **/
i_DECLIMEX_ int rf_utf8_encode_single(const uint32_t codepoint, char* utf8);

/**
 ** @brief Takes a utf8 buffer and attempts to decode the first codepoint
 **
 **
 ** @param[in] utf8               The utf8 buffer
 ** @param[out] index             The next index in the buffer after the
 **                               decoded codepoint
 ** @param[out] cp                The decoded codepoint
 ** @return                       Returns @c true for success and
 **                               @c false otherwise
 **/
bool rf_utf8_decode_single(const char *s,
                           uint32_t *index,
                           uint32_t *cp);

/**
 ** @brief Takes a utf8 buffer and decodes it into unicode codepoints. It
 ** also verifies the validity of the utf8 byte stream. If the caller
 ** simply needs to verify the validity of the stream but not decode it
 ** then he can use @ref rf_utf8_verify()
 **
 **
 ** @param[in] utf8               The utf8 buffer
 ** @param[in] utf8_byte_length   The bytes length of the UTF8 buffer
 ** @param[out] chars_num         Pass a reference to an @c uint32_t here to receive
 **                               the number of unicode characters contained in
 **                               the @c utf8 and the returned codepoints buffer
 ** @param[in/out] code_points    Pass a buffer of @c uint32_t to receive the code
 **                               points of the decoded utf8 buffer here.
 **                               The buffer has to be allocated and
 **                               be of at least 4 times the length of the utf8
 **                               buffer to be safe that it fits
 **                               the decoded codepoints.
 ** @param cp_buff_size           The size of the buffer given at @c code_points.
 ** @return                       Returns @c true for success and
 **                               @c false otherwise
 ** @see rf_utf8_verify()
 **/
i_DECLIMEX_ bool rf_utf8_decode(const char* utf8, uint32_t utf8_byte_length,
                                uint32_t* chars_num, uint32_t* code_points,
                                uint32_t cp_buff_size);


/**
 ** Parses a utf-8 byte stream and verifies its validity. If it's a null terminated
 ** c string then you have to provide @c byteLength to get back its size.
 ** If it's not a null terminated buffer then you can provide its byte length
 ** in the third argument @c given_byte_length.
 **
 ** @param[in] bytes        A sequence of bytes encoded in the UTF-8 encoding
 ** @param[out] returned_byte_length
 **                         Pass a reference to an uint32_t to obtain the number
 **                         of bytes that make up the sequence. It can be NULL.
 **                         If this is null then the caller must provide a
 **                         relevant value to the @c given_byte_length field
 ** @param[in] given_byte_length
 **                         If you already know the size of @c bytes provide it
 **                         here
 ** @return                 Returns @c true for proper utf8 byte sequence
 **                         and @c false otherwise
 ** @see rf_utf8_decode()
 **/
i_DECLIMEX_ bool rf_utf8_verify(const char* bytes,
                                uint32_t* returned_byte_length,
                                uint32_t given_byte_length);

/**
 ** @brief Decodes a  UTF-16 byte stream into codepoints
 **
 ** Takes in a UTF-16 endoded byte stream  inside @c buff.
 ** Pass an int32_t in @c characterLength to receive the character length and
 ** an @c uint32_t buffer at @c codepoints with enough allocated size to fit
 ** any amount of characters that the UTF-16 stream could produce.
 ** The caller must make sure to never provide NULL to either the @c codepoint
 ** or the @c characterLength field.
 ** @param[in] buff             The UTF-16 byte stream
 ** @param[in] in_buff_length   The length of the input UTF-16 stream
 **                             in bytes
 ** @param[out] characterLength Give a @c uint32_t here to receive the
 **                             character length of the decoded codepoints
 ** @param[in/out] codepoints   Provide an already allocated uint32_t buffer to
 **                             hold the decoded unicode codepoints. The buffer
 **                             must have enough length
 **                             to hold any decoded codepoints that will come
 **                             out of the UTF-16. The maximum size you can
 **                             provide to be safe is 2 times the size of
 **                             the UTF-16 buffer length in bytes
 ** @param[in] buff_size        The size of the buffer at @c codepoints
 **                             given in bytes
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_utf16_decode(const char* buff, uint32_t in_buff_length,
                                 uint32_t* length, uint32_t* codepoints,
                                 uint32_t buff_size);

/**
 ** @brief Encodes a buffer of unicode codepoints into UTF-16.
 **
 ** The caller must make sure to never provide NULL to either the @c codepoints
 ** or the @c utf16length or the @c utf16 fields
 **
 ** @param[in] codepoints     Provides a buffer of unicode
 **                           codepoints for encoding
 ** @param[in] charsN         Provide the number of characters in the codepoints
 **                           buffer
 ** @param[out] utf16Length   Give a reference to a uint32_t to receive the
 **                           length in 16-bit words of the utf-16 buffer
 **                           that the function will return
 ** @param[out] utf16         Provide a buffer of big enough size here inside
 **                           which to receive the encoded UTF16 byte stream
 ** @param buff_size          Provide the size of the buffer given at @c utf16.
 **                           The size of the buffer must be big enough to fit
 **                           the encoded UTF-16. To be safe give a buffer the
 **                           same size in bytes as that of the codepoints
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ bool rf_utf16_encode(const uint32_t* codepoints,
                                 uint32_t charsN, uint32_t* utf16Length,
                                 uint16_t* utf16, uint32_t buff_size);


//! @}
//end of unicode doxygen group

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//end of include guards
