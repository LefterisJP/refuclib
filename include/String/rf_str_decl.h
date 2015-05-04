/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_STRING_DECL_H
#define RF_STRING_DECL_H

#include <Definitions/types.h> //for uin32_t definition

/**
** @internal
** @author Lefteris
** @date 09/12/2010
** @endinternal
** @brief A unicode String with UTF-8 internal representation
**
** The Refu String is a Unicode String that has two versions. One is this and for
** the other check @ref RFstringx to see what operations can be performed
** on extended Strings.
** Functions to convert to and from all UTF encoding exists but the internal
** representation is always at UTF-8. Once a String has been created it is 
** always assumed that the stream of bytes inside it is valid UTF-8 since
** every function performs a UTF-8 check unless otherwise specified.
**
** All the functions which have @isinherited{StringX} on their
** description can be used with extended strings safely, since no specific
** version of the function exists, or needs to exist to manipulate 
** Extended Strings. To make the documentation even clearer the 
** functions that should not
** be used with the extended string are marked with @notinherited{StringX}
**
** @see RFstringx
** @internal
** @cppcode
** //default constructor
** String(){this->i_StringCHandle = rf_string_create("");}
** @endcpp
** @endinternal
*/
struct RFstring
{
    //! The string's data
    char* data;
    //! String's length in bytes
    uint32_t length;
};

#endif//include guards end
