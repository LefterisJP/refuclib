/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_CORE_H
#define RF_STRING_CORE_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstring/X
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
#include <Utils/rf_unicode.h> //for rf_utf8_is_continutation_byte
/*------------- libc inclusion --------------*/
#include <string.h> //for size_t
#include <stdarg.h> //for va_XXX stuff
/*------------- End of includes -------------*/
#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


//! @name Creating an RFstring
//! @{

/**
 * Statically initialize a string
 */
#define RF_STRING_STATIC_INIT(s_) { sizeof(s_) - 1, (s_) }

/**
 * Shallow initialization of a string. If the source buffer,
 * gets overwritten then the data are lost.
 */
#define RF_STRING_SHALLOW_INIT(i_string, i_buff, i_len) \
    do {                                                \
        (i_string)->data = (i_buff);                    \
        (i_string)->length = (i_len);                   \
    } while(0)

/**
 * Shallow initialization of a string from a cstring
 */
#define RF_STRING_SHALLOW_INIT_CSTR(s_) {strlen(s_), (s_) }


/**
 * @brief Allocates and returns a string with the given characters
 *
 * Given characters have to be in UTF-8. A check for valid sequence
 * of bytes is performed. @notinherited{StringX}
 * @lmsFunction
 * @param s                      The sequence of bytes for the characters
 *                               in UTF-8 (the default).
 * @param ... \rfoptional{nothing}  Depending on the string literal,
 * the function may expect a sequence of additional arguments,
 * each containing one value to be inserted instead of each %-tag
 * specified in the @c slit parameter, if any. There should be
 * the same number of these arguments as the number of %-tags that expect a value.
 * @return Returns the initialized RF_string or null in case of failure to
 * initialize, due to invalid utf-8 sequence
 * @see rf_string_init()
 * @see rf_stringx_create()
 */
i_DECLIMEX_ struct RFstring* rf_string_create(const char *s);

/**
 * @brief Allocates and returns a string with the given characters (vararg)
 *
 * Given characters have to be in UTF-8. A check for valid sequence
 * of bytes is performed. @notinherited{StringX}
 * @lmsFunction
 * @param s                      The sequence of bytes for the characters
 *                               in UTF-8 (the default).
 * @param ...                    Depending on the string literal, the function
 *                               may expect additional arguments in a printf-like
 *                               fashion
 * @return                       Returns the initialized RF_string or null in
 *                               case of failure
 * @see rf_string_init()
 */
i_DECLIMEX_ struct RFstring *rf_string_createv(const char *s, ...);
i_DECLIMEX_ struct RFstring *rf_string_createvl(const char *s, va_list args);

/**
 * @brief Initializes a string with the given characters.
 *
 * @notinherited{StringX}
 * @lmsFunction
 * @see rf_string_create()
 *
 */
i_DECLIMEX_ bool rf_string_init(struct RFstring *str, const char *s);

/**
 * @brief Initializes a string with the given characters. (varargs)
 *
 * @notinherited{StringX}
 * @lmsFunction
 * @see rf_string_createv()
 *
 */
i_DECLIMEX_ bool rf_string_initv(struct RFstring *str, const char *s, ...);
i_DECLIMEX_ bool rf_string_initvl(struct RFstring *str,
                                  const char *s,
                                  va_list args);

/**
 * @brief Allocates a String by turning a unicode code point
 * in a String (encoded in UTF-8).
 *
 * @notinherited{StringX}
 * @param code The unicode code point to encode
 * @return A String with the code point encoded in it or a null
 * pointer in case of an illegal code point value
 * @see rf_string_init_cp()
 * @see rf_stringx_create_cp()
 */
i_DECLIMEX_ struct RFstring *rf_string_create_cp(uint32_t code);

/**
 * @brief Initializes a string by turning a unicode code point
 *        in a String (encoded in UTF-8).
 *
 * @notinherited{StringX}
 * @see rf_string_create_cp()
 * @see rf_stringx_init_cp()
 */
i_DECLIMEX_ bool rf_string_init_cp(struct RFstring *str, uint32_t code);

/**
 * @cppnotctor
 * @brief Allocates and returns a string with the given characters
 *        without any checking.
 *
 * @notinherited{StringX}
 * @warning NO VALID-UTF8 check is performed.
 * @param s The sequence of bytes for the characters in UTF-8
 * (the default).Can also follow a printf-like format which will be formatted with
 * the variables that follow it. No check for valid bytestream is performed
 * @return Returns the initialized RF_string or null in case
 * of failure to initialize
 * @see rf_string_init_unsafe()
 */
struct RFstring* rf_string_create_unsafe(const char *s);

/**
 * @brief Initializes a string with the given characters with no checking
 *
 * @notinherited{StringX}
 * @see rf_string_create_unsafe()
 */
i_DECLIMEX_ bool rf_string_init_unsafe(struct RFstring *str, const char* s);

/**
 * @brief Initializes a string from a non-null terminated char* buffer
 * and for the given bytelength
 *
 * @notinherited{StringX}
 * @see rf_string_init_unsafe()
 */
i_DECLIMEX_ bool rf_string_init_unsafe_nnt(
    struct RFstring *str,
    const char *s,
    size_t length
);

/**
 * @opassign
 * @brief Allocates and returns a string with the given integer.
 *
 * @notinherited{StringX}
 * @param i The integer to turn into a string
 * @return Returns the initialized RF_string
 * @see rf_string_init_int()
 */
i_DECLIMEX_ struct RFstring *rf_string_create_int(int i);

/**
 * @brief Initializes a string with the given integer.
 *
 * @notinherited{StringX}
 * @see rf_string_create_int()
 */
i_DECLIMEX_ bool rf_string_init_int(struct RFstring* str, int i);

/**
 * @brief Allocates and returns a string with the given double.
 *
 * @notinherited{StringX}
 * @param f           The double to turn into a string
 * @param precision   The precision of the float. The number of characters
 *                    to add after the decimal point
 * @return Returns the initialized RF_string
 * @see rf_string_init_double()
 */
i_DECLIMEX_ struct RFstring *rf_string_create_double(
    double f,
    unsigned int precision
);

/**
 * @brief Initializes a string with the given double.
 *
 * @notinherited{StringX}
 * @see rf_string_create_double()
 */
i_DECLIMEX_ bool rf_string_init_double(
    struct RFstring *str,
    double f,
    unsigned int precision
);

/**
 * @brief Allocates and returns a string with the given UTF-16 byte sequence.
 *
 * @notinherited{StringX}
 * @param s A buffer of 2 bytes word representing the
 *  utf-16 byte sequence.
 * @param len The bytelength of @c s
 * @return Returns the initialized RF_string or null in case of failure to
 * initialize, due to invalid utf-16 sequence or illegal endianess value
 * @see rf_string_init_utf16()
 * @see rf_string_create_f_utf16()
 * @see rf_string_init_utf16()
 */
i_DECLIMEX_ struct RFstring *rf_string_create_utf16(
    const uint16_t *s,
    unsigned int len
);

/**
 * @brief Initializes a string with the given UTF-16 byte sequence.
 *
 * @notinherited{StringX}
 * @see rf_string_create_utf16()
 * @see rf_string_create_f_utf16()
 * @see rf_string_init_utf16()
 */
i_DECLIMEX_ bool rf_string_init_utf16(
    struct RFstring *str,
    const uint16_t *s,
    unsigned int len
);

/**
 * @brief Allocates and returns a string with the given UTF-32 byte sequence.
 *
 * @notinherited{StringX}
 * Given characters have to be in UTF-32 and in the endianess of the system.
 * No endianess swapping occurs in the function
 * @param s A buffer of 4-byte words representing the utf-32
 * byte sequence.
 * @param len The length of s in codepoints (unicode characters)
 * @return Returns the initialized RF_string or null in case of
 * failure to initialize
 * @see rf_string_init_utf32()
 * @see rf_string_create_f_utf32()
 * @see rf_string_init_utf32()
 */
i_DECLIMEX_ struct RFstring *rf_string_create_utf32(
    const uint32_t *s,
    unsigned int len
);
/**
 * @brief Initializes a string with the given UTF-32 byte sequence.
 *
 * @notinherited{StringX}
 * @see rf_string_create_utf32()
 * @see rf_string_create_f_utf32()
 * @see rf_string_init_utf32()
 */
i_DECLIMEX_ bool rf_string_init_utf32(
    struct RFstring *str,
    const uint32_t *s,
    unsigned int len
);

//! @}

//! @name Assigning to a String
//! @{

/**
 * @brief Assigns the value of the source string to the destination.
 *
 * @notinherited{StringX}
 * @lmsFunction
 * Both strings should already be initialized and hold a value.
 * It is an error to give null parameters.
 * @param dest   The destination string, which should get assigned
 * @param source The source string, whose values to copy.
 *               @inhtype{String,StringX} @tmpSTR
 *
 * @return       Returns @c true for success and @c false otherwise
 * @see rf_string_assign_char()
 * @see rf_stringx_assign()
 */
i_DECLIMEX_ bool rf_string_assign(struct RFstring *dest, const struct RFstring *source);

/**
 * @brief Assigns to a string with the given format (varargs)
 *
 * @notinherited{StringX}
 * @lmsFunction
 *
 */
i_DECLIMEX_ bool rf_string_assignv(struct RFstring *str, const char *s, ...);
i_DECLIMEX_ bool rf_string_assignvl(
    struct RFstring *str,
    const char *s,
    va_list args
);

/**
 * @brief Assigns the value of a unicode character to the string
 *
 * @notinherited{StringX}
 * @param thisstr The string to assign to
 * @param character The unicode character codepoint to assign to the String
 * @return Returns @c true for succesfull assignment and @c false
 *         if the given @c character was not a valid unicode codepoint
 * @see rf_string_assign()
 * @see rf_stringx_assign_char()
 */
i_DECLIMEX_ bool rf_string_assign_char(struct RFstring *thisstr, uint32_t character);

/**
 * @brief Assigns to a string from a non-null terminated char* buffer
 * and for the given bytelength
 *
 * @notinherited{StringX}
 */
i_DECLIMEX_ bool rf_string_assign_unsafe_nnt(struct RFstring* str,
                                             const char *s,
                                             size_t length);

/**
 * @brief Nullifies a String
 * @warning Use null strings at your own risk. None of the RF_Sting/X
 * functions currently test for them
 * A safer and easier alternative is to assigg an empty string and
 * check for it with @ref rf_string_is_empty()
 */
#define rf_string_null(i_STRING) do{                        \
        rf_string_length_bytes(i_STRING) = 0;               \
        rf_string_data(i_STRING) = NULL;                    \
    }while(0)

//! @}

//! @name String Copying Functions
//! @{

/**
 * @brief Creates and returns an allocated copy of the given string
 *
 * @isinherited{StringX}
 * @param src The string to copy from. @inhtype{String,StringX}
 * @return Returns a string copied from the previous one or null
 * if the original string was null
 * @see rf_string_copy_in()
 * @see rf_string_copy_chars()
 */
i_DECLIMEX_ struct RFstring *rf_string_copy_out(const struct RFstring *src);

/**
 * @brief Copies all the contents of a string to another
 *
 * @isinherited{StringX}
 * @return Returns @c true for succesfull copying @c false otherwise
 * @see rf_string_copy_out()
 * @see rf_string_copy_chars()
 *
 */
i_DECLIMEX_ bool rf_string_copy_in(struct RFstring *dst, const struct RFstring *src);

/**
 * @brief Copies a certain number of characters from a string
 *
 * @isinherited{StringX}
 * Copies @c n characters from @c src String into the destination @c dst string.
 * @param dst The string to copy in
 * @param src The string to copy from. @inhtype{String,StringX}
 * @param n The number of characters to copy from the @c src string
 * If the value is bigger than the maximum number of characters
 * then still all characters are copied. If it is 0 then an empty
 * string is initialized as the copy.
 * @return Returns @c true for success and @c false otherwise
 * @see rf_string_copy_in()
 * @see rf_string_copy_out()
 *
 */
i_DECLIMEX_ bool rf_string_copy_chars(
    struct RFstring *dst,
    const struct RFstring *src,
    uint32_t n
);

/**
 * @brief Shallow initialization of an RFstring from an RFstringx
 */
#define RF_STRING_SHALLOW_COPY(i_dst, i_src)                            \
    do {                                                                \
        rf_string_data(i_dst) = rf_string_data(i_src);                  \
        rf_string_length_bytes(i_dst) = rf_string_length_bytes(i_src);  \
    } while(0)

//! @}

//! @name Getting rid of an RFstring
//! @{

/**
 * @brief Deletes a string object and also frees its pointer.
 *
 * @notinherited{StringX}
 * Use it for strings made with _Create
 * @param s The string for deletion
 * @see rf_string_deinit()
 */
i_DECLIMEX_ void rf_string_destroy(struct RFstring *s);

/**
 * @cppignore
 * @brief Deletes a string object only, not its memory.
 *
 * @notinherited{StringX}
 * @see rf_string_destroy()
 *
 */
i_DECLIMEX_ void rf_string_deinit(struct RFstring *s);


//! @}

//! @name Equality check
//! @{

/**
 * @brief Compares two Strings and returns true
 *        if they are equal and false otherwise
 *
 * @isinherited{StringX}
 * @lmsFunction
 * @param s1 The first string to compare @inhtype{String,StringX} @tmpSTR
 * @param s2 The second string to compare @inhtype{String,StringX} @tmpSTR
 * @return True in case the strings are equal and false otherwise
 */
i_DECLIMEX_ bool rf_string_equal(
    const struct RFstring *s1,
    const struct RFstring *s2
);

/**
 * Compares a string with a cstr
 * @param str  The string to compare
 * @param cstr The c-string to compare with
 * @return     True for equality, false otherwise
 */
i_DECLIMEX_ bool rf_string_equal_cstr(const struct RFstring *str, const char *cstr);

/**
 * @brief Checks that a string is null
 */
#define rf_string_is_null(i_STRING)                                     \
    (rf_string_length_bytes(i_STRING) == 0 && rf_string_data(i_STRING) == NULL)


/**
 * @isinherited{StringX}
 * @brief Checks that a string is empty.
 */
#define rf_string_is_empty(i_STRING)            \
    (rf_string_length_bytes(i_STRING) == 0)

/**
 * @return a pointer to the empty string
 */
i_DECLIMEX_ const struct RFstring *rf_string_empty_get();
//! @}


//! @name String Iteration
//! @{


/*
  The following 2 functions are exposed here only
  because they are used in the iteration macros
*/

/**
 * @internal
 * @brief Retrieves the unicode code point of the parameter
 * bytepos of the string.
 *
 * @isinherited{StringX}
 * This is an internal function, there is no need to use it. The reason
 * it is exposed here is that it is utilized in the iteration macros.
 * @warning DO NOT use this function unless you know what you are doing
 * @param thisstr The string whose byte position code point we need.
 *                @inhtype{String,StringX}
 * @param bytepos The byte position of the string from
 *                 where to get the code point.
 * @warning If this is out of bounds then nothing can detect it and
 *          at best it will cause a SEG FAULT. Moreover no check to see
 *          if this is not a continutation byte is made. All the checks
 *          must have been made before calling the function.
 * @return Returns the code point of the byte position as an uint32_t
 * @endinternal
 */
i_DECLIMEX_ uint32_t rf_string_bytepos_to_codepoint(
    const struct RFstring *thisstr,
    uint32_t bytepos
);

/**
 * @internal
 * @brief Retrieves character position of a byte position
 *
 * @isinherited{StringX}
 * This is an internal function, there is no need to use it. It attempts
 * to retrieve character position from a byte position. If the byte
 * position is a continutation byte and does not constitute the start
 * of a character then depending on the option the function will find
 * either the next character or the previous character position from
 * this byte position
 *
 * @warning DO NOT use this function unless you know what you are doing
 * @param thisstr The string whose byte position code point we need.
 *                @inhtype{String,StringX}
 * @param bytepos The byte position of the string from where to get
 *                the character position
 * @param before A boolean flag denoting the behaviour in case this byte
 *               position is a continutation byte. If @c before is true then
 *               the function will retrieve the first character position
 *               before the byte. If it is false, it will retrieve the
 *               first character position after the continuation byte.
 * @endinternal
 */
i_DECLIMEX_ uint32_t rf_string_bytepos_to_charpos(
    const struct RFstring *thisstr,
    uint32_t bytepos,
    bool before
);


// Checks if a given byte is a continuation byte
#define rf_utf8_is_continuation_byte2(b__)  ( b__ >= 0x80 && b__<= 0xBF )

/**
 * An iterator allowing iteration over an RFstring
 */
struct RFstring_iterator {
    char *sp;
    char *ep;
    char *p;
    uint32_t character_pos;
};

/**
 ** Returns an iterator for the given string
 */
i_DECLIMEX_ void rf_string_get_iter(
    const struct RFstring *thisstr,
    struct RFstring_iterator *ret
);

/**
 ** Iterates the characters of a string
 */
bool rf_string_iterator_next(struct RFstring_iterator *it, uint32_t *value);


/*
  Two macros to accomplish iteration of an RFstring from any given
  character going forwards. This macro should be used with its end pair.
  We take advantage of the fact that an RFstring is always guaranteed
  to contain a valid UTF-8 sequence and thus no checks are performed.
*/

/**
 * @brief Starts an RFstring forward iteration scope.
 *
 * @isinherited{StringX}
 * Use this macro to iterate every character inside an RFstring or RFstringx
 * Must be used with its pair macro #rfString_Iterate_End.
 * As an example consider this code that iterates every character
 * of a string from the start to finish
 * @code
 * uint32_t i = 0;
 * uint32_t charValue;
 * RFstring foo;rf_string_init(&foo,"I am a String");
 * rf_string_iterate_start(&foo,i,charValue)
 *      //for every character in the string,let's print it
 *      printf("Character at index %d is %c\n",i,charValue);
 * rf_string_iterate_end(i)
 * @endcode
 * @param[in] string_ The string to iterate. Must be a pointer to string
 * @param[in,out] startCharacterPos_ Here give an uint32_t which will
 *                be the character position from which to start the iteration.
 *                In each iteration this will hold the character index.
 *                If the given position is out of bounds then the
 *                iteration does not happen
 * @param[in,out] characterUnicodeValue_ Here pass an uint32_t which in
 *                each iteration will hold the unicode code point of the
 *                character at position startCharacterPos_
 * @see rf_string_iterate_end()
 * @see rf_string_iterate_b_start()
 */
#define rf_string_iterate_start(string_, startCharacterPos_,             \
                               characterUnicodeValue_)                  \
    {                                                                   \
    /* b index sec is the byte index and j the character index*/        \
    uint32_t byteIndex_ = 0;uint32_t j_=0;                              \
    /*                                                                  \
      iterate until we find the character position requested and        \
      its equivalent byte position                                      \
    */                                                                  \
    while(j_ != startCharacterPos_)                                     \
    {                                                                   \
        if(!rf_utf8_is_continuation_byte(rf_string_data(string_)[byteIndex_])) \
        {                                                               \
            j_++;                                                       \
        }                                                               \
        byteIndex_++;                                                   \
    }                                                                   \
    /*now start the requested iteration*/                               \
    while(byteIndex_ < rf_string_length_bytes(string_))                    \
    {                                                                   \
    /*if it's a character*/                                             \
        if(!rf_utf8_is_continuation_byte(rf_string_data(string_)[byteIndex_])) \
        {/*Give the character value to the user*/                       \
            characterUnicodeValue_ = rf_string_bytepos_to_codepoint((string_), \
                                                                 byteIndex_);

/**
 * @brief Ends an RF_String/RFstringx forward iteration scope.
 *
 * @isinherited{StringX}
 * Look at #rf_string_iterate_start for an example usage
 * @param[in,out] startCharacterPos_ Here give the uint32_t given to #rf_string_iterate_start
 * @see rf_string_iterate_start()
 *
 */
#define rf_string_iterate_end(startCharacterPos_)  startCharacterPos_++;}byteIndex_++;}}

/*
  Two macros to accomplish iteration of an RFstring from any given
  character going backwards. This macro should be used with its end pair.
  We take advantage of the fact that an RFstring is always guaranteed to contain
  a valid UTF-8 sequence and thus no checks are performed.
*/

/**
 * @brief Starts an RFstring backward iteration scope.
 *
 * @isinherited{StringX}
 * Use this macro to iterate every character inside an RFstring or
 * RFstringx going backwards
 * Must be used with its pair macro #rfString_IterateB_End.
 * As an example consider this code that iterates every character of a
 * string from the start to finish
 * @code
 * uint32_t charValue;
 * RFstring foo;rf_string_init(&foo,"I am a String");
 * uint32_t i = rf_string_length(&foo)-1;//-1 is since String indexing starts from 0
 * rf_string_iterate_b_start(&foo,i,charValue)
 *      //for every character in the string,let's print it
 *      printf("Character at index %d is %c\n",i,charValue);
 * rf_string_iterate_b_end(i)
 * @endcode
 * @param[in] string_ The string to iterate. Must be a pointer to string
 * @param[in,out] characterPos_ Here give an uint32_t which will be the
 *                character position from which to start the iteration.
 *                In each iteration this will hold the character index.
 *                If the given position is out of bounds then the iteration
 *                does not happen
 * @param[in,out] characterUnicodeValue_ Here pass an uint32_t which in
 *                each iteration will hold the unicode code point of the
 *                character at position characterPos_
 * @see rf_string_iterate_b_end()
 * @see rf_string_iterate_start()
 */
#define rf_string_iterate_b_start(string_,                              \
                                  characterPos_,                        \
                                  characterUnicodeValue_)     {         \
        /* b index is the byte index and j the character index*/        \
        uint32_t b_index_ = 0;                                          \
        uint32_t j_=0;                                                  \
        /*                                                              \
         * c index sec is another signed copy of the character index    \
         * (and is int64_t so that it can cater for any situation). Reason \
         * is cause going backwards we gotta have -1 too                \
         */                                                             \
        int64_t c_index_ = characterPos_;                               \
        /*                                                              \
         * iterate until we find the character position requested and   \
         * its equivalent byte position                                 \
         */                                                             \
        while (b_index_ < rf_string_length_bytes(string_) && j_ != characterPos_) { \
            if(!rf_utf8_is_continuation_byte(rf_string_data(string_)[b_index_])) \
            {                                                           \
                j_++;                                                   \
            }                                                           \
            b_index_++;                                                 \
        }                                                               \
        /* make sure that we don't stop at continuation byte */         \
        while (b_index_ < rf_string_length_bytes(string_) &&            \
               rf_utf8_is_continuation_byte(rf_string_data(string_)[b_index_])) { \
            b_index_++;                                                 \
        }                                                               \
        /*                                                              \
         * now start the requested iteration - notice that the          \
         * end condition is to reach the first character position       \
         */                                                             \
        while (c_index_ != -1)                                             { \
            /*if it's a character*/                                     \
            if(!rf_utf8_is_continuation_byte(rf_string_data(string_)[b_index_])) \
            {/*Give the character value to the user*/                   \
                characterUnicodeValue_ = rf_string_bytepos_to_codepoint((string_), \
                                                                        b_index_);

/**
 * @cppignore
 * @brief Ends an RF_String/RFstringx backward iteration scope.
 *
 * @isinherited{StringX}
 * Look at #rf_string_iterate_b_start for an example usage
 * @param[in,out] characterPos_ Here give the uint32_t given
 * to #rf_string_iterate_b_start
 * @see rf_string_itera_b_start()
 *
 */
#define rf_string_iterate_b_end(characterPos_)  c_index_-- ;characterPos_--;}b_index_--;}}

//! @}


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
