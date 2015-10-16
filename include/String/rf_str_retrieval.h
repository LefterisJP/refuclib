/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_RETRIEVAL_H
#define RF_STRING_RETRIEVAL_H

/*------------- Module related inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstringX
#include <String/rf_str_flags.h> //for RFstring_matching_options
/*------------- Outside Module inclusion -------------*/
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/types.h> //for exact sized types
#include <Definitions/retcodes.h> //for bool
#include <Definitions/inline.h> //for inline
/*------------- End of includes -------------*/

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

struct RFarray;

//! @name String accessors
//! @{

/**
 * @brief Finds the length of the string in characters.
 * @isinherited{StringX}
 *
 * @param s      The string whose number of characters to find.
 *               @inhtype{String,StringX}
 * @return       Returns the length of the string in characters
 */
i_DECLIMEX_ uint32_t rf_string_length(const struct RFstring *s);


/**
 * @brief Gets the bytelength of the string
 * @warning IS a macro. Only safe past a typecheck
 * @isinherited{StringX}
 *
 * @param            s_ A string pointer
 * @return           The length of the string in bytes
 */
#define rf_string_length_bytes(s_) ((struct RFstring*)(s_))->length

/**
 * @brief Gets a pointer to the data of a string
 * @warning IS a macro. Only safe past a typecheck
 * @isinherited{StringX}
 *
 * @param s_      A string pointer
 * @return        A pointer to the string's data
 */
#define rf_string_data(s_) ((struct RFstring*)(s_))->data

/**
 * @brief Retrieves the unicode code point of the parameter character.
 *
 * @isinherited{StringX}
 * If the character position is out of bounds false is returned
 * @param thisstr      The string whose character code point we need.
 *                     @inhtype{String,StringX}
 * @param c            The character index whose unicode code point to return.
 *                     Must be a positive (including zero) integer.
 * @param cp           Will return the unicode codepoint if succesfull
 * @return             returns @c true for success and @c false for failure
 */
i_DECLIMEX_ bool rf_string_get_char(const struct RFstring *thisstr,
                                    uint32_t c,
                                    uint32_t *cp);
//! @}

//! @name String Retrieval
//! @{

/**
 * @brief Returns a substring of this string
 *
 * @isinherited{StringX}
 * Returns the substring of @c thisstr starting from @c startPos
 * and for @c charsN characters. It is returned inside @c ret.
 *
 * @param thisstr        The string whose substring we want
 *                       @inhtype{String,StringX}
 * @param startPos       The starting character where the substring
 *                       will begin from
 * @param charsN         The number of characters the substring will have.
 *                       If they exceed the end of the string then,
 *                       a substring only until the end of the string
 *                       shall be returned.
 * @param ret            Pass a reference to an RFstring here to receive
 *                       the resulting substring.
 * @return               Returns @c true if a substring exists and
 *                       @c false otherwise
 * @see rf_string_between()
 * @see rf_string_after()
 * @see rf_string_before()
 */
i_DECLIMEX_ bool rf_string_substr(const struct RFstring *thisstr,
                                  uint32_t startPos,
                                  uint32_t charsN,
                                  struct RFstring *ret);

/**
 * @brief Finds if a substring exists inside another string.
 *
 * @isinherited{StringX}
 * Finds the existence of String sstr inside this string with the given options.
 * You have the option to either match case or perform a case-insensitive search.
 * In addition you can search for the exact string and not it just
 * being a part of another string.
 * @lmsFunction
 *
 * @param thisstr          This string we want to search in
 *                         @inhtype{String,StringX}
 * @param sstr             The substring string we want to search for
 *                         @inhtype{String,StringX} @tmpSTR
 * @param options          @see rf_string_remove() for details on this option
 * @return                 Returns the character position of the found
 *                         substring or RF_FAILURE for not found
 * @see rf_string_find_i()
 */
i_DECLIMEX_ int32_t rf_string_find(const struct RFstring *thisstr,
                                   const struct RFstring *sstr,
                                   enum RFstring_matching_options options);

/**
 * @brief Finds if a substring exists inside a specific part of another string.
 *
 * @isinherited{StringX}
 * @lmsFunction
 * @see rf_string_find()
 */
i_DECLIMEX_ int32_t rf_string_find_i(const struct RFstring *thisstr,
                                     const struct RFstring *sstr,
                                     uint32_t startPos,
                                     uint32_t length,
                                     enum RFstring_matching_options options);


/**
 * @brief Finds if a String begins with a particular substring
 *
 * @isinherited{StringX}
 * Determines if the string begins with the @c sstr substring. You have the
 * option to either match case or perform a case-insensitive search.
 * In addition you can search for the exact string and not it just being
 * a part of another string.
 * @lmsFunction
 * @param thisstr       This string we want to search in
 *                      @inhtype{String,StringX}
 * @param sstr          The substring to check for in the beginning
 *                      @inhtype{String,StringX} @tmpSTR
 * @param options       For details @see rf_string_remove()
 * @return              Returns true if the string does begin with @c sstr
 *                      and false if not
 * @see rf_string_ends_with()
 */
#define rf_string_begins_with(i_THISSTR_,i_SEARCHSTR_,i_OPTIONS_)     \
    (rf_string_find((i_THISSTR_),(i_SEARCHSTR_),(i_OPTIONS_)) == 0)


/**
 * @brief Finds if a String ends with a particular substring
 *
 * @isinherited{StringX}
 * Determines if the string ends with the @c sstr substring. You have the
 * option to either match case or perform a case-insensitive search.
 * In addition you can search for the exact string and not it just
 * being a part of another string.
 * @lmsFunction
 * @param thisstr        This string we want to search in
 *                       @inhtype{String,StringX}
 * @param sstr           The substring to check for in the end of the
 *                       string @inhtype{String,StringX} @tmpSTR
 * @param options        For details @see rf_string_remove()
 * @return               Returns true if the string does end with
 *                       @c sstr and false if not
 * @see rf_string_begins_with()
 *
 */
i_INLINE_DECL bool rf_string_ends_with(const struct RFstring *thisstr,
                                       const struct RFstring *searchstr,
                                       enum RFstring_matching_options options)
{
    int32_t res = rf_string_find(thisstr, searchstr, options);
    return res >= 0 &&
        (unsigned int)res == (rf_string_length(thisstr) - rf_string_length(searchstr));
}

/**
 * @brief Checks if a string begins with any of the specified characters
 *
 * @isinherited{StringX}
 * @lmsFunction
 *
 * @param thisstr        This string we want to search in
 *                       @inhtype{String,StringX}
 * @param chars          The string containing all the characters to check
 *                       @inhtype{String,StringX}
 * @param limit          If not NULL, a pointer to a position inside
 *                       @c thisstr after which to stop checking. This
 *                       position will be included in the search.
 * @param[out] bytes     If given, then bytes will contain the number of bytes
 *                       of the beginning of the string that are part of the
 *                       @c chars string
 * @return               How many character of the beginning of the string
 *                       are part of the @c chars string
 */
unsigned int rf_string_begins_with_any(const struct RFstring *thisstr,
                                       const struct RFstring *chars,
                                       const char *limit,
                                       unsigned int *bytes);


/**
 * @brief Counts how many times a substring occurs inside the string.
 *
 * @isinherited{StringX}
 * @lmsFunction
 * @param thisstr       The string inside which to count.
 *                      @inhtype{String,StringX}
 * @param sstr          The substring for which to search.
 *                      @inhtype{String,StringX} @tmpSTR
 * @param bytes         If not 0, then this is the number of bytes into
 *                      the string to search for the substring
 * @param positions     If not NULL then this will contain the byte positions
 *                      of the occurences of @c sstr. User will need to take
 *                      care of the freeing later.
 * @param options       Reserved for future use. Not used at the moment in
 *                      function
 * @return              Returns the number of times cstr exists inside the
 *                      string. In case it is not found at all 0 is returned.
 *                      Note that -1 is returned  in case of error such as
 *                      invalid input(null pointers).
 */
i_DECLIMEX_ int rf_string_count(
    const struct RFstring *thisstr,
    const struct RFstring *sstr,
    unsigned int bytes,
    struct RFarray *positions,
    enum RFstring_matching_options options
);

//! @}

//! @name Positional String Retrieval Functions
//! @{

/**
 * @brief Applies a limited version of sscanf after the specified substring
 *
 * @isinherited{StringX}
 * @lmsFunction
 * @param thisstr           The current string. @inhtype{String,StringX}
 * @param afterstr          The substring after which to apply sscanf.
 *                          @inhtype{String,StringX} @tmpSTR
 * @param format            The tokens parameter which give the format of scanf
 * @param var               A void* to pass in any variable we need
 *                          to get a value
 * @return                  Returns @c true if a value was read and @c false
 *                          otherwise, substring not being found in the string
 *                          or sscanf unable to read into the variable
 */
i_DECLIMEX_ bool rf_string_scanf_after(const struct RFstring *thisstr,
                                       const struct RFstring *afterstr,
                                       const char *format,
                                       void *var);

/**
 * @brief Initializes the first substring, between two given strings
 *
 * @isinherited{StringX}
 * Initializes the given string as the first substring existing
 * between the left and right parameter substrings
 * @lmsFunction
 * @note The Returned Substring needs to be deinitialized by the user.
 * @param[in] thisstr         This current string. @inhtype{String,StringX}
 * @param[in] lstr            The left substring that will define the new
 *                            substring. @inhtype{String,StringX} @tmpSTR
 * @param[in] rstr            The right substring that will define the new
 *                            substring. @inhtype{String,StringX} @tmpSTR
 * @param[out] result         Pass a pointer to a String type to receive
 *                            the string between @c lstr and @c rstr.
 *                            If the passed pointer is of RFstringx type
 *                            also pass the @c RF_STRINGX_ARGUMENT bitflag
 *                            argument in the @c options argument.
 *                            The caller is responsible for
 *                            initializing the string but the function
 *                            will assign to it
 *                            and also reallocate its buffer if needed
 *                            This should NOT ever be null.
 *                            @inhtype{String,StringX}
 * @param options             For details @see rf_stringx_move_after()
 * @return                    Returns @c true if the substring is found and
 *                            initialized and @c false otherwise
 * @see rf_string_before()
 * @see rf_string_after()
 * @see rf_stringx_move_after_pair()
 */
i_DECLIMEX_ bool rf_string_between(const struct RFstring *thisstr,
                                   const struct RFstring *lstr,
                                   const struct RFstring *rstr,
                                   void *result,
                                   enum RFstring_matching_options options);


/**
 * @brief Initializes the given string as the substring from the
 * start until any of the given Strings are found
 *
 * @isinherited{StringX}
 * The parameters that have to be given as variable argument
 * <b>must</b> be of type RFstring* or RFstringx* or even
 * string initialized with the temporary string macro
 * @lmsFunction
 * @param thisstr            The string to operate in. @inhtype{String,StringX}
 * @param[out] result        For details @see rf_string_between()
 * @param options            For details @see rf_stringx_move_after()
 * @param parN               The number of strings to search for
 * @param ...                The strings to search for.
 *                           @inhtype{String,StringX} @tmpSTR
 * @return                   Returns true if the substring was initialized and
 *                           false if none of the parameters were found or an
 *                           invalid UTF-8 sequence was given. In the latter case
 *                           an error is also logged.
 * @see rf_string_before()
 * @see rf_string_afterv()
 */
i_DECLIMEX_ bool rf_string_beforev(const struct RFstring *thisstr,
                                   void *result,
                                   enum RFstring_matching_options options,
                                   const unsigned char parN,
                                   ...);


/**
 * @brief Initializes the given string as the substring from the start
 * until the given string is found
 *
 * @isinherited{StringX}
 * @lmsFunction
 * @param thisstr            The string to operate in.
 *                           @inhtype{String,StringX}
 * @param sstr               The substring that we want to find inside the
 *                           string @inhtype{String,StringX} @tmpSTR
 * @param[out] result        For details @see rf_string_between()
 * @param options            For details @see rf_stringx_move_after()
 * @return                   Returns @c true if the substring was initialized
 *                           and false if none of the parameters were found or
 *                           an invalid UTF-8 sequence was given.
 *                           In the latter case an error is also logged.
 * @see rf_string_beforev()
 * @see rf_string_after()
 */
i_DECLIMEX_ bool rf_string_before(const struct RFstring *thisstr,
                                  const struct RFstring *sstr,
                                  void *result,
                                  enum RFstring_matching_options options);

/**
 * @brief Initialize a string after a given substring
 *
 * @isinherited{StringX}
 * Initializes the given String with the substring located after
 * (and not including) the after substring inside the parameter string.
 * If the substring is not located the function returns false.
 * @note The given String needs to be deinitialized by the user
 * @lmsFunction
 * @param[in] thisstr          The parameter string from which the substring
 *                             will be formed. @inhtype{String,StringX}
 * @param[in] after            The substring to search for inside the parameter
 *                             string. @inhtype{String,StringX} @tmpSTR
 * @param[out] result          For details @see rf_string_between()
 * @param options              For details @see rf_stringx_move_after()
 * @return                     Returns true for success and false if the
 *                             substring is not found in the parameter string.
 * @see rf_string_afterv()
 * @see rf_string_before()
 * @see rf_stringx_move_after()
 */
i_DECLIMEX_ bool rf_string_after(const struct RFstring *thisstr,
                                 const struct RFstring *after,
                                 void *result,
                                 enum RFstring_matching_options options);

/**
 * @brief Initialize a string after the first of the given substrings found
 *
 * @isinherited{StringX}
 * Initializes the given String with the substring located after
 * (and not including) the after substring inside the parameter string.
 * If the substring is not located the function returns false.
 * The parameters that have to be given as variable argument <b>must</b> be
 * of type RFstring* or RFstringx* or even string initializers
 * with the temporary string macro
 * @lmsFunction
 * @param[in] thisstr            The parameter string from which the substring
 *                               will be formed. @inhtype{String,StringX}
 * @param[out] result            For details @see rf_string_between()
 * @param options                For details @see rf_stringx_move_after()
 * @param parN                   The number of substrings to search for.
 * @param ...                    The substrings to search for.
 *                               @inhtype{String,StringX} @tmpSTR
 * @return                       Returns @c true for success and @c false if
 *                               the substring is not found in the parameter
 *                               string or in error
 * @see rf_string_after()
 * @see rf_string_beforev()
 */
i_DECLIMEX_ bool rf_string_afterv(const struct RFstring *thisstr,
                                  void *result,
                                  enum RFstring_matching_options options,
                                  const unsigned char parN,
                                  ...);


//! @}

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif
