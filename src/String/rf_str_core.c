/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <String/rf_str_core.h>
/*------------- Module related inclusion -------------*/
#include "rf_str_common.ph" //for private string iteration
#include "rf_str_defines.ph" //for some defines
/*------------- Outside Module inclusion -------------*/
#include <Utils/rf_unicode.h> //for unicode functions
#include <Definitions/retcodes.h> //for error codes
#include <Utils/log.h> //for error logging
#include <Utils/memory.h> //for refu memory allocation
#include <Utils/sanity.h> //for the sanity check macros
#include <Persistent/buffers.h> //for the internal persistent buffer
/*------------- libc inclusion --------------*/
#include <stdio.h> // for snprintf
/*------------- End of includes -------------*/



struct RFstring* rf_string_createv(const char* s, ...)
{
    struct RFstring *ret;
    va_list args;

    va_start(args, s);
    ret = rf_string_createvl(s, args);
    va_end(args);
    return ret;
}

struct RFstring *rf_string_createvl(const char* s, va_list args)
{
    struct RFstring* ret;

    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_string_initvl(ret, s, args)) {
        free(ret);
        return NULL;
    }
    return ret;
}

struct RFstring* rf_string_create(const char* s)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_string_init(ret, s)) {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_initv(struct RFstring* str, const char* s, ...)
{
    va_list args;
    bool ret;

    va_start(args, s);
    ret = rf_string_initvl(str, s, args);
    va_end(args);
    return ret;
}

bool rf_string_initvl(struct RFstring* str, const char* s, va_list args)
{
    unsigned int size, buff_index;
    bool ret = true;
    char *buff_ptr;
    RF_ASSERT(str, "got null string in function");

    if (!s) {
        RF_ERROR("String initialization failed due to null pointer input");
        return false;
    }

    //read the var args
    if (!fill_fmt_buffer(s, &size, &buff_ptr, &buff_index, args)) {
        RF_ERROR("String creation failure due to failing at reading the "
                 "formatted string");
        return false;
    }

    //get length
    rf_string_length_bytes(str) = size;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(rf_string_data(str), rf_string_length_bytes(str),
              ret = false; goto cleanup_buffer);
    memcpy(rf_string_data(str), buff_ptr, rf_string_length_bytes(str));

#ifdef RF_OPTION_DEBUG
cleanup_buffer:
#endif
    rf_buffer_set_index(TSBUFFA, buff_index, char);

    return ret;
}

bool rf_string_init(struct RFstring* str, const char* s)
{
    //check for validity of the given sequence and get the character length
    uint32_t byteLength;
    RF_ASSERT(str, "got null string in function");

    if (!s) {
        RF_ERROR("Attempted to initialize string with a null c string");
        return false;
    }
    if(!rf_utf8_verify(s, &byteLength, 0))
    {
        RF_ERROR("Error at String Initialization due to invalid UTF-8 "
                 "byte sequence");
        return false;
    }

    //get length
    rf_string_length_bytes(str) = byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(rf_string_data(str), rf_string_length_bytes(str), return false);
    memcpy(rf_string_data(str), s, rf_string_length_bytes(str));

    return true;
}


//Allocates a String by turning a unicode code point in a String (encoded in UTF-8).
struct RFstring* rf_string_create_cp(uint32_t codepoint)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(rf_string_init_cp(ret, codepoint))
    {
        return ret;
    }
    //failure
    free(ret);
    return NULL;
}

//Turns a unicode code point in a String (encoded in UTF-8).
bool rf_string_init_cp(struct RFstring* str, uint32_t codepoint)
{
    RF_ASSERT(str, "got null string in function");
    //alloc enough for a utf8 character
    RF_MALLOC(rf_string_data(str), MAX_UTF8C_BYTES, return false);
    rf_string_length_bytes(str) = rf_utf8_encode_single(
        codepoint, rf_string_data(str)
    );
    if (!rf_string_length_bytes(str)) {
        free(rf_string_data(str));
        return false;
    }
    return true;
}

struct RFstring* rf_string_create_int(int i)
{
    //initialize the string and return it
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_string_init_int(ret, i))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_init_int(struct RFstring* str, int i)
{
    //the size of the int32_t buffer
    int len;
    //put the int32_t into a buffer and turn it in a char*
    char buff[MAX_UINT32_STRING_CHAR_SIZE];
    RF_ASSERT(str, "got null string in function");

    len = snprintf(buff, MAX_UINT32_STRING_CHAR_SIZE, "%d", i);
    if(len < 0 || len >= MAX_UINT32_STRING_CHAR_SIZE)
    {
        RF_ERROR("String initialization from integer failed due to snprintf() "
                 "failing with errno %d", errno);
        return false;
    }
    rf_string_length_bytes(str) = len;
    RF_MALLOC(rf_string_data(str), len, return false);
    memcpy(rf_string_data(str), buff, len);

    return true;
}

struct RFstring* rf_string_create_double(double f, unsigned int precision)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(rf_string_init_double(ret, f, precision) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_init_double(struct RFstring* str,
                           double f,
                           unsigned int precision)
{
    char buff[MAX_DOUBLE_STRING_CHAR_SIZE];
    int len;
    RF_ASSERT(str, "got null string in function");

    len = snprintf(buff, MAX_DOUBLE_STRING_CHAR_SIZE, "%.*f", precision, f);
    if(len < 0 || len >= MAX_DOUBLE_STRING_CHAR_SIZE)
    {
        RF_ERROR("String initialization from float failed due to snprintf() "
                 "failing with errno %d", errno);
        return false;
    }

    rf_string_length_bytes(str) = len;
    RF_MALLOC(rf_string_data(str), len, return false);
    memcpy(rf_string_data(str), buff, len);

    //success
    return true;
}

struct RFstring* rf_string_create_utf16(const uint16_t* s, unsigned int len)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_string_init_utf16(ret, s, len))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_init_utf16(struct RFstring* str, const uint16_t* s, unsigned int len)
{
    //decode the utf-16 and get the code points
    uint32_t* codepoints;
    uint32_t characterLength, utf8ByteLength;
    char* utf8;
    RF_ASSERT(str, "got null string in function");

    if (!s) {
        RF_ERROR("Provided NULL UTF-16 byte sequence");
        return false;
    }

    RF_MALLOC(codepoints, len * 2, return false); //allocate the codepoints
    //parse the given byte stream normally since it has to be in the
    //endianess of the system
    if(!rf_utf16_decode((const char*)s, len, &characterLength, codepoints,
                       len * 2))
    {
        free(codepoints);
        RF_ERROR("String initialization failed due to invalide UTF-16 "
                 "sequence");
        return false;
    }
    //now encode these codepoints into UTF8
    RF_MALLOC(utf8, characterLength * 4, return false);
    if(!rf_utf8_encode(codepoints, characterLength,
                      &utf8ByteLength, utf8, characterLength * 4))
    {
        RF_ERROR("String initialization failed during encoding in UTF8");
        free(codepoints);
        free(utf8);
        return false;
    }
    //success
    free(codepoints);
    rf_string_data(str) = utf8;
    rf_string_length_bytes(str) = utf8ByteLength;
    return true;
}

struct RFstring* rf_string_create_utf32(const uint32_t* s, unsigned int len)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(rf_string_init_utf32(ret, s, len) == false)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_init_utf32(struct RFstring* str, const uint32_t* codeBuffer,
                         unsigned int length)
{
    //find the length of the utf32 buffer in characters
    uint32_t utf8ByteLength;
    char *utf8;
    RF_ASSERT(str, "got null string in function");

    if (!codeBuffer) {
        RF_ERROR("Provided null pointer for the input byte stream");
        return false;
    }

    //turn the codepoints into a utf-8 encoded buffer
    RF_MALLOC(utf8, length * 4, return false);
    if(!rf_utf8_encode(codeBuffer, length, &utf8ByteLength, utf8, length * 4))
    {
        RF_ERROR("Could not properly encode a UTF32 buffer into UTF8");
        free(utf8);
        return false;
    }
    rf_string_data(str) = utf8;
    rf_string_length_bytes(str) = utf8ByteLength;
    return true;
}


struct RFstring* rf_string_create_unsafe(const char* s)
{
    struct RFstring* ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_string_init_unsafe(ret, s))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_init_unsafe(struct RFstring* str, const char* s)
{
    return rf_string_init_unsafe_nnt(str, s, strlen(s));
}


bool rf_string_init_unsafe_nnt(struct RFstring* str, const char* s, size_t length)
{
    RF_ASSERT(str, "got null string in function");
    RF_MALLOC(rf_string_data(str), length, return false);
    memcpy(rf_string_data(str), s, length);
    rf_string_length_bytes(str) = length;
    return true;
}


/* --- Assigning to a String --- */


bool rf_string_assign(struct RFstring* dst, const void* src)
{
    RF_ASSERT(dst, "got null string in function");

    if (!src) {
        return false;
    }
    //only if new string value won't fit in the buffer reallocate
    if(rf_string_length_bytes(src) > rf_string_length_bytes(dst))
    {
        RF_REALLOC(rf_string_data(dst), char, rf_string_length_bytes(src),
                   return false);
    }
    //now copy the value
    memcpy(rf_string_data(dst), rf_string_data(src), rf_string_length_bytes(src));
    //and fix the lengths
    rf_string_length_bytes(dst) = rf_string_length_bytes(src);

    return true;
}

bool rf_string_assignv(struct RFstring* str, const char* s, ...)
{
    va_list args;
    bool ret;

    va_start(args, s);
    ret = rf_string_assignvl(str, s, args);
    va_end(args);
    return ret;
}
bool rf_string_assignvl(struct RFstring* str,
                        const char* s,
                        va_list args)
{
    unsigned int size, buff_index;
    char *buff_ptr;
    RF_ASSERT(str, "got null string in function");

    if (!s) {
        RF_ERROR("String assignment failed due to null pointer input");
        return false;
    }
    //read the var args
    if (!fill_fmt_buffer(s, &size, &buff_ptr, &buff_index, args)) {
        RF_ERROR("String assignment failure due to failing at reading the "
                 "formatted string");
        return false;
    }

    rf_buffer_set_index(TSBUFFA, buff_index, char);

    if (rf_string_length_bytes(str) < size) {
        RF_REALLOC(rf_string_data(str), char, size, return false);
    }
    //get length
    rf_string_length_bytes(str) = size;
    memcpy(rf_string_data(str), buff_ptr, rf_string_length_bytes(str));
    
    return true;
}

//Assigns the value of a unicode character to the string
bool rf_string_assign_char(struct RFstring* str,uint32_t codepoint)
{
    RF_ASSERT(str, "got null string in function");
    //realloc if needed
    if (str->length < 5) {
        RF_REALLOC(rf_string_data(str), char, 5, return false);
    }
    rf_string_length_bytes(str) = rf_utf8_encode_single(codepoint,
                                                    rf_string_data(str));
    if(!rf_string_length_bytes(str))
    {
        return false;
    }
    return true;
}

bool rf_string_assign_unsafe_nnt(struct RFstring* str, const char* s,
                                 size_t length)
{
    bool ret = true;
    RF_ASSERT(str, "got null string in function");
    if (!s) {
        RF_WARNING("Provided null pointer for assignment");
        return false;
    }
    if(length > rf_string_length_bytes(str))
    {
        RF_REALLOC(rf_string_data(str), char, length,
                   ret = false; goto cleanup);
    }

    //now copy the value
    memcpy(rf_string_data(str), s, length);
    //and fix the lengths
    rf_string_length_bytes(str) = length;

  cleanup:
    return ret;
}

/*--- RFstring copying functions ---*/

struct RFstring* rf_string_copy_out(const void* src)
{
    struct RFstring* ret;
    //create the new string
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if(!rf_string_copy_in(ret, src))
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool rf_string_copy_in(struct RFstring* dst, const void* src)
{
    if (!src) {
        RF_WARNING("Provided null source string");
        return false;
    }
    return rf_string_init_unsafe_nnt(dst,
                                    rf_string_data(src),
                                    rf_string_length_bytes(src));
}
//Copies a certain number of characters from a string
bool rf_string_copy_chars(struct RFstring* dst, const void* src, uint32_t charsN)
{
    uint32_t i = 0,bytePos;
    if (!src) {
        RF_WARNING("Gave null source pointer");
        return false;
    }
    //find the byte position until which we need to copy
    RF_STRING_ITERATE_START(src, i, bytePos)
        if(i == charsN) { break;}
    RF_STRING_ITERATE_END(i, bytePos)

    return rf_string_init_unsafe_nnt(dst, rf_string_data(src), bytePos);
}


/*--- Methods to get rid of an RFstring ---*/

// Deletes a string object and also frees its pointer.
void rf_string_destroy(struct RFstring* s)
{
    if(s != 0)
    {
        free(rf_string_data(s));
        free(s);
    }
}
// Deletes a string object only, not its memory.
void rf_string_deinit(struct RFstring* s)
{
    if(s != 0)
    {
        free(rf_string_data(s));
    }
}

/*--- Equality check ---*/

bool rf_string_equal(const void* s1, const void* s2)
{
    bool ret;
    RF_ASSERT(s1, "got null 1st string in function");
    RF_ASSERT(s2, "got null 2nd string in function");
    ret = strcmp_nnt(rf_string_data(s1), rf_string_length_bytes(s1),
                     rf_string_data(s2), rf_string_length_bytes(s2));
    return ret;
}

/*--- 2 functions used in the iteration macros ---*/


uint32_t rf_string_bytepos_to_codepoint(const void* str, uint32_t i)
{
    uint32_t codePoint=0;
    /*
      Here I am not checking if byte position 'i' is withing bounds
      and especially if it is a start of a character. This is assumed
      to have been checked or to be known beforehand by the programmer.
      That's one of the reasons why this is an internal function and
      should not be used unless you know what you are doing
    */
    //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
    if( ((rf_string_data(str)[i] & 0x80)>>7) == 0 )
    {
        //and the code point is this whole byte only
        codePoint = rf_string_data(str)[i];
    }
    //if the leading bits are in the form of 0b110xxxxx then range is: U+0080 to U+07FF (2 bytes)
    else if ( RF_HEXEQ_C( ( (~(rf_string_data(str)[i] ^  0xC0))>>5),0x7) )
    {
        codePoint =0;
        //from the second byte take the first 6 bits
        codePoint = (rf_string_data(str)[i+1] & 0x3F) ;
        //from the first byte take the first 5 bits and put them in the start
        codePoint |= ((rf_string_data(str)[i] & 0x1F) << 6);
    }
    //if the leading bits are in the form of 0b1110xxxx then range is U+0800 to U+FFFF  (3 bytes)
    else if( RF_HEXEQ_C( ( (~(rf_string_data(str)[i] ^ 0xE0))>>4),0xF) )
    {
        codePoint = 0;
        //from the third byte take the first 6 bits
        codePoint = (rf_string_data(str)[i+2] & 0x3F) ;
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((rf_string_data(str)[i+1] & 0x3F) << 6);
        //from the first byte take the first 4 bits and put them to the left of the previous 6 bits
        codePoint |= ((rf_string_data(str)[i] & 0xF) << 12);
    }
    //if the leading bits are in the form of 0b11110xxx then range is U+010000 to U+10FFFF (4 bytes)
    else if( RF_HEXEQ_C( ( (~(rf_string_data(str)[i] ^ 0xF0))>>3), 0x1F))
    {
        codePoint = 0;
        //from the fourth byte take the first 6 bits
        codePoint = (rf_string_data(str)[i+3] & 0x3F) ;
        //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((rf_string_data(str)[i+2] & 0x3F) << 6);
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((rf_string_data(str)[i+1] & 0x3F) << 12);
        //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
        codePoint |= ((rf_string_data(str)[i] & 0x7) << 18);
    }

    return codePoint;
}
//Retrieves character position of a byte position
uint32_t rf_string_bytepos_to_charpos(const void* str, uint32_t bytepos,
                                   bool before)
{
    /*
      here there is no check if this is actually a byte pos inside the string's
      byte buffer. The programmer should have made sure it is before hand.
      This is why it is an internal function and should only be used if
      you know what you are doing
    */
    uint32_t charPos = 0;
    uint32_t byteI = 0;
    //iterate the string's bytes until you get to the required byte
    //if it is not a continuation byte, return the position
    if(!rf_utf8_is_continuation_byte(rf_string_data(str)[bytepos]))
    {
        RF_STRING_ITERATE_START(str, charPos, byteI)
            if(byteI == bytepos) {return charPos;}
        RF_STRING_ITERATE_END(charPos, byteI)
    }
    //else iterate the string's bytes until you get sth bigger than the required byte
    RF_STRING_ITERATE_START(str, charPos, byteI)
        if(byteI > bytepos)
            break;
    RF_STRING_ITERATE_END(charPos, byteI)
    //if we need the previous one return it
    if(before)
    {
        return charPos - 1;
    }
    //else return this
    return charPos;
}

/* -- iteration related -- */
void rf_string_get_iter(const void *thisstr,
                        struct RFstring_iterator *ret)
{
    RF_ASSERT(thisstr, "got null string in function");
    ret->p = rf_string_data(thisstr);
    ret->sp = ret->p;
    ret->ep = ret->sp + rf_string_length_bytes(thisstr);
    ret->character_pos = 0;
}

bool rf_string_iterator_next(struct RFstring_iterator *it,
                             uint32_t *value)
{
    uint32_t index;
    if (it->p > it->ep) {
        return false;
    }

    while (rf_utf8_is_continuation_byte(*it->p)) {
        it->p = it->p + 1;
        if (it->p > it->ep) {
            RF_ERROR("String ending with continuation byte during iteration");
            return false;
        }
    }

    if (!rf_utf8_decode_single(it->p, it->ep, &index, value)) {
        RF_ERROR("Error at utf-8 decoding during string iteration");
        return false;
    }

    if (it->p != it->sp) { /* increase character pos unless it's the 1st one */
        it->character_pos += 1;
    }
    it->p = it->p + index;
    return true;
}

