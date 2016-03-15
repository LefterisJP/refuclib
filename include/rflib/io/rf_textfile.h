/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_TEXTFILE_H
#define RF_TEXTFILE_H

#include <rflib/io/rf_textfile_decl.h>

#include <rflib/defs/types.h>
#include <rflib/defs/imex.h>
#include <rflib/defs/defarg.h>
#include <rflib/defs/retcodes.h>
#include <rflib/string/xdecl.h>
#include <rflib/utils/array.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

//! @name Creation functions
//! @{

/**
 * @brief Initializes a new text file
 *
 * This function initializes a new textfile object. You have the option to
 * either open an already existing TextFile by using
 * either @c RF_FILE_READ , @c RF_FILE_WRITE or @c RF_FILE_READWRITE for the
 * @c mode argument. In that case the file shall also
 * be tested for the existence of a Byte Order Mark.
 *
 * To create a new file use either @c RF_FILE_NEW or @c RF_FILE_READWRITE_NEW.
 * A Byte Order Mark will be appened to the file
 * depending on the given @c encoding argument unless the library has been
 * compiled with the @c RF_OPTION_TEXTFILE_ADDBOM off.
 * Note that if a specific endianess for UTF-16 or UTF-32 is not given then
 * the file will be created in the system's endianess.
 * @lmsFunction
 * @param t                      The textfile to initialize
 * @param name                   The name of the file.
 *                               @inhtype{String,StringX} @tmpSTR
 * @param mode                   The mode with which to access the file.
 *                               For legal values take a look
 *                               at @ref RFtextfile_mode
 *
 *                               This parameter serves as but a hint of what
 *                               operation should follow the initialization.
 *                               The reason is that each of the functions that
 *                               operate on an @ref RFtextfile check the
 *                               current mode and if it is not the legal mode
 *                               for that operation the file is reopened with
 *                               the proper mode, or has its file pointer
 *                               rewinded and then put back if it is in
 *                               read/write mode.
 * @param endianess              The endianess of the file. Is useful only if
 *                               the encoding of the file is in UTF-32 or UTF-16.
 *                               Look at @ref RFendianess for possible values.
 *                               In the case or @ref RF_ENDIANESS_UNKNOWN
 *                               the file is scanned in an attempt to determine
 *                               its endianess
 * @param encoding               The encoding of the file. For legal values
 *                               take a look at @ref RFtext_encoding
 *
 * @param eol                    The End Of Line type that this textfile uses.
 *                               The value of @c RF_EOL_AUTO should not be given
 *                               if this is a newly created file. If that is done
 *                               an invalid argument error is logged and the file
 *                               is initialized with @c RF_EOL_LF. In addition
 *                               if the auto-detection fails for some reason
 *                               the default @c RF_EOL_DEFAULT will be used and
 *                               appropriate error logging will be performed.
 *                               For possible value look at @ref RFeol_mark
 *
 * @return                       Returns @c true in success
 */
i_DECLIMEX_ bool rf_textfile_init(struct RFtextfile* t,
                                  const void* name,
                                  enum RFtextfile_mode mode,
                                  enum RFendianess endianess,
                                  enum RFtext_encoding encoding,
                                  enum RFeol_mark eol
);

/**
 * @brief Creates a new text file
 *
 * Allocates and returns a new text file
 * @lmsFunction
 * Behaves exactly like @ref rf_textfile_init() so refer to it for the explanation
 * of the arguments
 */
i_DECLIMEX_ struct RFtextfile* rf_textfile_create(
    const void* name,
    enum RFtextfile_mode mode,
    enum RFendianess endianess,
    enum RFtext_encoding encoding,
    enum RFeol_mark eol
);


//! @}
//! @name Copying functions
//! @{

/**
 * @brief Copies one textfile handle into another
 *
 * Copies textfile @c src into textfile @c dst
 * This does not create a whole new textfile in disk. It just creates an identical
 * handler that points to the same textfile at the same position
 * @param dst The destination textfile
 * @param src The source textfile
 * @return Returns @c true for success and @c false otherwise
 *
 */
i_DECLIMEX_ bool rf_textfile_copy_in(struct RFtextfile* dst,
                                     struct RFtextfile* src);
/**
 * @brief Allocates and returns a copy of an RFtextfile handler
 *
 * This does not create a whole new textfile in disk. It just creates an identical
 * handler that points to the same textfile at the same position
 * @param src The source textfile
 * @return A newly allocated copy of @c src or @c NULL for error
 *
 */
i_DECLIMEX_ struct RFtextfile* rf_textfile_copy_out(struct RFtextfile* src);

//!@}
//!@name Destruction functions
//!@{

/**
 * @brief Deinitalizes a text file
 *
 * Use it to cleanup text files created with @ref rf_textfile_init
 * @param t The text file to cleanup
 *
 */
i_DECLIMEX_ void rf_textfile_deinit(struct RFtextfile* t);
/**
 * @brief Destroys a text file
 *
 * Use it to cleanup text files created with @ref rf_textfile_create
 * @param t The text file to cleanup
 *
 */
i_DECLIMEX_ void rf_textfile_destroy(struct RFtextfile* t);

//! @}

//! @name Conversion Functions
//! @{

/**
 * @brief Changes the file access mode of the TextFile
 *
 * @param mode Give the new file access mode here. The only legal values are:
 * + @c RF_FILE_READ: So that the file stream goes into reading mode
 * + @c RF_FILE_WRITE: So that the file stream goes into writting mode
 * + @c RF_FILE_READWRITE: So that the file stream goes into read/write mode.
 *
 * If another value is given this function will fail.
 * @return Returns @c true for success and @c false for failure
 */
i_DECLIMEX_ bool rf_textfile_set_mode(struct RFtextfile* t,
                                      enum RFtextfile_mode mode);

//! @}

//! @name Traversal Functions
//! @{

/**
 * @brief Moves the file pointer by a given number of lines relative to
 * the current line
 *
 * Moves the file pointer at the beginning of the line at @c linesN forward if
 * @c linesN is a positive number or at the beginning of the line located
 * @c linesN behind the current line if the number is negative.
 * If the @c linesN number is negative and its absolute value is equal to or
 * greater to the current line then nothing happens and @c RF_FAILURE is returned.
 *
 * Also if the given @c linesN is negative and takes the line too far back and
 * going to that line would be faster through the beginning of the file then
 * the function understands that and starts moving to that line from the file's
 * beginning
 *
 * If @c linesN is 0 then the function will move the file pointer to the
 * beginning of the current line if it is not already there
 *
 * Should the function attempt to move further back than the first line of the
 * file then @c RF_FAILURE will be returned and the @c line will be the the same.
 *
 * @param[in] t The Text File to operator on
 * @param[in] linesN The number of lines to move the TextFile's internal pointer.
 * Can be either negative or positive number. If it is a positive number then
 * the pointer is moved forward and if it is negative it is moved backwards. If
 *  it is negative, its absolute value should be less than the current line.
 * Can't move further back than the first line. Should not be zero. If it is
 * zero, nothing happens.
 * @return Returns @c RF_SUCCESS for succesfull moving of the file pointer.
 * It can also return
 * + @c RF_FAILURE: If during moving a negative amount of lines, the start of
 * the file is encountered.
 * + @c RE_FILE_EOF: If the end of file is reached while attempting to move
 * the required amount of @c linesN
 * + A negative number for any other error
 */
i_DECLIMEX_ int32_t rf_textfile_move_lines(struct RFtextfile* t,
                                           int64_t linesN);


/**
 * @brief Moves the internal file pointer by a number of characters
 *
 * Moves the file pointer @c charsN characters forward if positive and
 * backwards if negative.
 * Should the function attempt to move further back than the first character
 * of the file then @c RF_FAILURE will be returned and the file pointer will
 * remain at the same position

 * @param[in] t The Text File to operate on
 * @param[in] charsN The number of characters to move. Can be either
 * negative or positive
 * @return Returns the number of characters moved in either direction
 *  or a negative number for failure
 */
i_DECLIMEX_ int32_t rf_textfile_move_chars(struct RFtextfile* t,
                                           int64_t charsN);

/**
 * @brief Moves the internal file pointer forward by an amount of characters
 *
 * If the End of file is encountered while attempting to move a character forward
 * then the function will return less than the requested number of characters
 * and the Textfile's EOF flag will be set.
 *
 * If a file reading error occurs at this function, unlike other functions of
 * RFtextfile this one will not attempt to reset the pointer back to its
 * original position
 * @param[in] t The Text File to operate on
 * @param[in] charsN The number of characters to move forward.
 * @return Returns the number of characters the file pointer was moved or a negative
 * number for error. If eof is reached then the returned number of characters
 * will be less than the requested number of characters and the textfile's
 * EOF flag will be set
 */
i_DECLIMEX_ int32_t rf_textfile_move_chars_f(struct RFtextfile* t,
                                             uint64_t charsN);
/**
 * @brief Moves the internal file pointer backward by an amount of characters
 *
 * @param[in] t The Text File to operate on
 * @param[in] charsN The number of characters to move backward
 * @return Returns the number of characters the file pointer was moved or a negative
 * number for error. If file's beginning is reached then the returned number
 * of characters will be less than the requested number of characters and
 * the textfile's EOF flag will be set
 */
i_DECLIMEX_ int32_t rf_textfile_move_chars_b(struct RFtextfile* t,
                                             uint64_t charsN);

/**
 * @brief Moves the file pointer at the start of the given line
 *
 * Moves the file pointer at the beginning of the line at @c lineN and it is
 * as if the line has not been read. The outcome of the next @ref
 * rf_textfile_read_line() will provide the contents of that line. Line Indexing
 * starts from @c 1.
 *
 * @param[in] t The Text File to operator on
 * @param[in] lineN The line at the beginning of which to move the file
 * pointer. If @c 0 is given it is an error
 * @return Returns @c RF_SUCCESS for succesfull moving of the file pointer.
 * In case of failure returns either negative number or @c RE_FILE_EOF if the
 * EOF has been encountered while searching for the line because the line does
 * not exist
 */
i_DECLIMEX_ int32_t rf_textfile_go_to_line(struct RFtextfile* t, uint64_t lineN);

/**
 * @brief Moves the file pointer at the given offset
 *
 * Moves the file pointer at the specified file offset also updating the
 * internal data of the text file. You can given the same flags given at
 * fseek() function except @c SEEK_END.
 *
 * A subsequent readline after this functions would read from the current
 *  file position until the next newline character is found.
 *
 * If invalid input is given then the function shall return error.
 * Invalid input constitute an illegal value at @c origin argument and illegal
 * combinations of @c offset and @c origin. For instance you can't expect to
 * have a negative offset with @c origin starting from the start of the file
 *
 * If the offset would take the file pointer to the end of the file or beyond
 * then @c RE_FILE_EOF is returned and the position is returned to where it
 * was before the function call.
 *
 * If @c offset would take the file pointer in between bytes that are not a
 * legal value in the corresponding encoding the file is in then the function
 * will actually move enough additional bytes forward (or backwards) to
 * encompass the whole unicode codepoint.
 *
 * @param[in] t The Text File to operate on
 * @param[in] offset The offset to move the internal file pointer to. Can be
 * either negative or positive and it depends on the position given at @c origin
 * @param[in] origin Just like fseek this denotes from where should the
 * offset moving be performed.
 * + @c SEEK_SET: From the beginning of the file
 * + @c SEEK_CUR: From the current file position
 *
 * Unlike fseek this function does not accept offset moving from the end of
 * the file. So @c SEEK_END is not a legal value
 * @return Returns @c RF_SUCCESS for succesfull moving of the file pointer.
 * If the EOF has been encountered while moving it returns RE_FILE_EOF while
 * in the case of any other error it returns a negative number
 */
i_DECLIMEX_ int32_t rf_textfile_go_to_offset(struct RFtextfile* t,
                                             RFfile_offset offset,
                                             int origin);

//! @}

//! @name Retrieval Functions
//! @{

/**
 * @brief Reads the next line of the text file
 *
 * Reads the next line of the Text File and moves the file pointer to the
 * beginning of the next line.
 * If the user does not provide a @c characters value then the whole line is
 *  returned inside the string @c line. If a value is provided then only the
 * first @c characters characters are returned inside line. No matter the
 * amount of character returned in the string the file pointer moves a whole line.
 *
 * In order to use this function
 * you need to provide an <b>already initialized</b> RFstringx that will act
 *  as the reading buffer. Providing an uninitialized RFstringx can lead to
 *  a segmentation fault or worse. After you are done with processing each
 *  line of the file, then and only then should you call @ref rf_stringx_deinit() or
 * @ref rf_stringx_destroy() on the @c line string.
 *
 * @param[in] TextFile               A pointer to the text file
 * @param[in,out] line               Give an already initialized @ref RFstringx
 *                                   buffer that will act as the line buffer
 *                                   of the file. The reason this parameter is
 *                                   a StringX is due to the need for text
 *                                   processing.
 * @return                           Returns @c RF_SUCCESS for success,
 *                                   @c RE_FILE_EOF for the end of file
 *                                   encountered while reading and a negative
 *                                   number for error
 */
i_DECLIMEX_ int rf_textfile_read_line(struct RFtextfile* t,
                                      struct RFstringx* line);

/**
 * @brief Reads a specific number of characters from the next line
 *        @see rf_textfile_read_line() for more details
 *
 * @param[in] characters           Give the number of characters that you
 *                                 would like to read from the line into the
 *                                 String. If @c 0 is given the
 *                                 whole line is read into the string.
 *                                 @note No matter the choice of characters
 *                                 here the file pointer will still
 *                                 move to the start of the next line.
 */
i_DECLIMEX_ int rf_textfile_read_line_chars(struct RFtextfile* t,
                                            struct RFstringx* line,
                                            uint32_t characters);

/**
 * @brief Reads a specific number of lines from the file.
 *
 * @param t             The textfile from where to read the lines
 * @param lines         The number of lines to read from the file, counting
 *                      from the current file position. If @c 0 is given then
 *                      all lines are read.
 * @param str           An already initialized @c RFstringx to hold the
 *                      read lines
 * @param lines_pos     If not NULL, an array to put the positions in bytes
 *                      of each new line start. Starts counting from the very
 *                      first line, element 0 is always 0.
 *                      Check the return value of the function to see how many
 *                      uint32_t elements would be end up inside the passed
 *                      RFarray
 *
 * @return              The number of lines read or @c -1 for failure
 */
i_DECLIMEX_ int rf_textfile_read_lines(struct RFtextfile* t,
                                       unsigned int lines,
                                       struct RFstringx *str,
                                       struct RFarray *lines_pos);

/**
 * @brief Gets the current byte offset of the file
 *
 * @param[in] t The textfile whose offset to get
 * @param[out] offset Give a reference to an @c RFfile_offset type to receive the
 * file offset of the Textfile
 * @return Returns @c true for success and @c false for error
 */
i_DECLIMEX_ bool rf_textfile_get_offset(struct RFtextfile* t, RFfile_offset* offset);

/**
 * @brief Gets a specific line from a Text File starting from the beginning
 * of the file
 *
 * Searches the text file for line @c lineN with the first line being @c 1.
 * When found it returns the line by assigning @c line's contents to the
 * given string. The user is responsible of freeing that string later himself.
 *
 * The function starts a search for the appropriate line from the beginning
 * of the file retrieving the lines one by one until it reaches the required
 * line at @c lineN. For a function that deduces how to search for the line
 * by comparing the current line with the requested one take a look
 * at @ref rf_textfile_get_line()
 *
 * If no line @c lineN exists and the file ends before that line is found then
 * the function returns @c RE_FILE_EOF and nothing happens to the passed
 * string at @c line.
 *
 * An important thing to note about the usage of this function is that when a
 * line is found the internal file pointer and the current line are not
 * changed. This just retrieves the lines without moving the file pointer or
 * changing the line.
 * @param[in] t The textfile to get the line from
 * @param[in] lineN The number of line to retrieve. Indexing starts from @c 1
 * for the first line. Do not give 0 here. If 0 is given then error is returned
 * @param[out] line Here give an @ref RFstringx that will receive the read
 * line. The string must <b>already be initialized</b> before being passed to
 * the function.
 * @return Returns @c RF_SUCCESS if everything went all right. If there was a
 * failure it returns a negative number. Can also return @c RE_FILE_EOF if the
 * line does not exist
 */
i_DECLIMEX_ int32_t rf_textfile_get_line_begin(struct RFtextfile* t,
                                               uint64_t lineN,
                                               struct RFstringx* line);
/**
 * @brief Gets a specific line from a Text File
 *
 * Searches the text file for line @c lineN with the first line being @c 1.
 * When found it returns the line by assigning @c line's contents to the
 * given string.
 *
 * The function compares the requested line with the line that the file pointer is
 * currently in. Then it deduces which method to use to find it. Either search
 * from the beginning of the file or search from the current line until the
 * given line is found.
 *
 * If no line @c lineN exists and the file ends before that line is found then
 * the functions returns @c RE_FILE_EOF and nothing happens to the passed
 * string at @c line.
 *
 * An important thing to note about the usage of this function is that when a
 * line is found the internal file pointer and the current line are not changed.
 * This just retrieves the lines without moving the file pointer or changing
 * the line.
 * @param[in] t The textfile to get the line from
 * @param[in] lineN The number of line to retrieve. Indexing starts from @c 1.
 *  Do not give 0 here. If 0 is given then @c RE_INPUT error is returned
 * @param[out] line Here give an @ref RFstringx that will receive the read
 * line. The string must <b>already be initialized</b> before being passed to
 *  the function.
 * @return Returns @c RF_SUCCESS if everything went all right. If there was a
 * failure it returns a negative number. Can also return @c RE_FILE_EOF if the
 * line does not exist
 *
 */
i_DECLIMEX_ int32_t rf_textfile_get_line(struct RFtextfile* t,
                                         uint64_t lineN,
                                         struct RFstringx* line);

//! @}

//! @name Writing Functions
//! @{

/**
 * @brief Writes an RFstring to the current file position
 *
 * Writes the given @c string at the current file position
 *
 * The given string may contain   new lines inside and hence
 * write more than one line with a single function call.
 * The given string's newlines must be only represented
 * by the '\n' character. The function shall handle the file line endings
 * separately by editing the string before inputting it into the file if so needed.
 * @lmsFunction
 * @note In this function if the given @c string does not end with a newline then one is not
 * automatically appended by the function.
 * @param t The textfile to write to
 * @param string The string to add to the end of the textfile
 *                @inhtype{String,StringX} @tmpSTR
 * @return Returns @c true for succesfull writting and @c false if an
 * error happened
 */
i_DECLIMEX_ bool rf_textfile_write(struct RFtextfile *t, const struct RFstring *string);


/**
 * @brief Inserts a line into a specific part of the Text File
 *
 * Inserts the contents of @c string inside the text file  either right after
 * @c lineN line if @c after is @c true or right before it if it is @c false.
 *
 * This function inserts a line, so it will append a newline character at the end
 * of the @c string before inserting. The given string may contain many new
 * lines inside and hence replace one line with multiple one. The given
 * string's newlines must be only represented by the '\n' character. The
 * function shall handle the file line endings separately by editing
 * the string before inputting it into the file if so needed.
 * The given string must not end with newline, it is automatically appended
 * by the function.
 *
 * If the End Of File is encountered before the line is found then the function
 * returns error
 *
 * @lmsFunction
 * @param t                      The TextFile to operate on
 * @param lineN                  Give the line number before of after which
 *                               the @c string will be placed. Line indexing
 *                               starts from @c 1 and up until the last line
 *                               of the file. If a bigger line than the file's
 *                               last line is given then an error will occur
 *                               since the requested line will not be found.
 *                               Do not give 0 here. If 0 is given the
 *                               function shall return error
 * @param string                 @inhtype{String,StringX} @tmpSTR
 *                               The string to insert to the textfile either
 *                               after or before @c lineN. A newline character
 *                               will be appended by this function inside the
 *                               string to make it a line but you can also
 *                               include other newline characters inside,
 *                               in essence entering multiple lines to the file
 * @param after                  A boolean denoting where we want to place
 *                               the new line. If @c true then the new line is
 *                               placed after the line at @c lineN and if false
 *                               before
 * @return                       Returns @c true for success @c false for error
 */
i_DECLIMEX_ bool rf_textfile_insert(struct RFtextfile* t,
                                    uint64_t lineN,
                                    const void* string,
                                    bool after);


/**
 * @brief Removes a specific line from the text file
 *
 * Searches the text file for line @c lineN and removes the line
 * if it manages to find it.
 *
 * If the End Of File is encountered before the line is found then the
 * function returns error
 *
 * @param t The TextFile to operate on
 * @param lineN Give the number of the line to remove. Indexing starts from @c 1.
 * If 0 is given the function shall return error.
 * @return Returns @c true for success and @c false for error
 */
i_DECLIMEX_ bool rf_textfile_remove(struct RFtextfile* t, uint64_t lineN);

/**
 * @brief Replaces a line of the textfile with another one
 *
 * Searches the text file for line @c lineN and replaces it with
 * @c string.
 *
 * The given string may contain many new lines inside and hence
 * replace one line with multiple one. The given string's newlines must be
 * only represented by the '\n' character. The function shall handle the file
 * line endings separately by editing the string before inputting it into the
 * file if so needed.  The given string must not end with newline, it is
 * automatically appended by the function.
 *
 * If the End Of File is encountered before the line is found then the
 * function returns error
 *
 * @lmsFunction
 * @param t The TextFile to operate on
 * @param lineN Give the number of the line to replace. Indexing starts from @c 1.
 * Do not give 0 here. If 0 is given the function shall return error
 * @param string The string to replace the line located at @c lineN.
 * A newline character will be appended by this function inside the string to
 * make it a line but you can also have other newline characters inside, in
 * essence replacing the line with multiple lines
 * @inhtype{String,StringX} @tmpSTR
 * @return Returns @c true for success and @c false for error
 */
i_DECLIMEX_ bool rf_textfile_replace(struct RFtextfile* t,
                                     uint64_t lineN,
                                     void* string);

/**
 * Convenience functions to parse a file into a string and optionally return
 * lines number and positions array.
 *
 * Essentially a wrapper over rf_textfile_init() and rf_textfile_read_lines().
 *
 * @param name            The full path to the file to open or "stdin" if we want
 *                        to read from stdin
 * @param lines           An optional unsigned int to contain the number of lines
 *                        found in the file's string
 * @param lines_pos       Optionally pass an array just like in rf_textfile_read_lines()
 *                        to get newline positions.
 * @param strbuff_in      Only for the XXX_in version of the function, provide an
 *                        already allocated stringx which to create the buffer from.
 * @return                An allocated Stringx with the contents of the entire
 *                        file, or NULL for failure.
 */
i_DECLIMEX_ struct RFstringx *rf_textfile_tostr(const struct RFstring *name,
                                                unsigned int *lines,
                                                struct RFarray *lines_pos);
i_DECLIMEX_ bool rf_textfile_tostr_in(const struct RFstring *name,
                                      unsigned int *lines,
                                      struct RFarray *lines_pos,
                                      struct RFstringx *strbuff_in);

//! @}


#ifdef __cplusplus
}//closing bracket for calling from C++
#endif


#endif //include guards end
