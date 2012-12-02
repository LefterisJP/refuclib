/*
** Copyright (c) 2011-2012, Karapetsas Eleftherios
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef RF_STRING_CORE_H
#define RF_STRING_CORE_H


#include <rf_setup.h> //for the macro flags
#include <String/string_decl.h>
#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif




/*-------------------------------------------------------------------------Methods to create an RF_String-------------------------------------------------------------------------------*/

//! @name Creating an RF_String
//! @{

//! @memberof RF_String
//! @opassign
//! @brief Allocates and returns a string with the given characters
//!
//! Given characters have to be in UTF-8. A check for valid sequence of bytes is performed. @notinherited{StringX}
//! @lmsFunction
//! @param s The sequence of bytes for the characters in UTF-8 (the default). Can also follow a printf-like format which will be formatted with
//! the variables that follow it. A check to see if it is a valid UTF-8 sequence is performed
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c slit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return Returns the initialized RF_string or null in case of failure to initialize, due to invalid utf-8 sequence
//! @see rfString_Init()
//! @see rfStringX_Create()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ RF_String* rfString_Create(const char* s,...);
#else
i_DECLIMEX_ RF_String* i_rfString_Create(const char* s,...);
i_DECLIMEX_ RF_String* i_NVrfString_Create(const char* s);
#define rfString_Create(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_CREATE,1,__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATE1(...) i_NVrfString_Create(__VA_ARGS__)
#define i_SELECT_RF_STRING_CREATE0(...) i_rfString_Create(__VA_ARGS__)
#endif




//! @memberof RF_String
//! @brief Initializes a string with the given characters.
//!
//! @notinherited{StringX}
//! Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
//! @lmsFunction
//! @param str The string to initialize
//! @param s The sequence of bytes for the characters in UTF-8 (the default).Can also follow a printf-like format which will be formatted with
//! the variables that follow it. A check to see if it is a valid UTF-8 sequence is performed
//! @param ... \rfoptional{nothing}  Depending on the string literal, the function may expect a sequence of additional arguments,
//! each containing one value to be inserted instead of each %-tag specified in the @c slit parameter, if any. There should be
//! the same number of these arguments as the number of %-tags that expect a value.
//! @return Returns true in case of correct initialization and false , due to invalid utf-8 sequence
//! @see rfString_Create()
//! @see rfStringX_Init()
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
i_DECLIMEX_ char rfString_Init(RF_String* str,const char* s,...);
#else
i_DECLIMEX_ char i_rfString_Init(RF_String* str,const char* s,...);
i_DECLIMEX_ char i_NVrfString_Init(RF_String* str,const char* s);
#define rfString_Init(...)  RP_SELECT_FUNC_IF_NARGIS(i_SELECT_RF_STRING_INIT,2,__VA_ARGS__)
#define i_SELECT_RF_STRING_INIT1(...) i_NVrfString_Init(__VA_ARGS__)
#define i_SELECT_RF_STRING_INIT0(...) i_rfString_Init(__VA_ARGS__)
#endif

//! @memberof RF_String
//! @cppnotctor
//! @brief Allocates a String by turning a unicode code point in a String (encoded in UTF-8).
//!
//! @notinherited{StringX}
//! @param code The unicode code point to encode
//! @return A String with the code point encoded in it or a null pointer in case of an illegal code point value
//! @see rfString_Init_cp()
//! @see rfStringX_Create_cp()
i_DECLIMEX_ RF_String* rfString_Create_cp(uint32_t code);
//! @memberof RF_String
//! @brief Initializes a string by turning a unicode code point in a String (encoded in UTF-8).
//!
//! @notinherited{StringX}
//! @param str The string to initialize
//! @param code The unicode code point to encode
//! @return Returns true in case of correct initialization and false , due to illegal code point value
//! @see rfString_Create_cp()
//! @see rfStringX_Init_cp()
i_DECLIMEX_ char rfString_Init_cp(RF_String* str,uint32_t code);



//! @memberof RF_String
//! @cppnotctor
//! @brief Allocates and returns a string with the given characters with no checking.
//!
//! @notinherited{StringX}
//! @warning NO VALID-UTF8 check is performed.
//! @param s The sequence of bytes for the characters in UTF-8 (the default).Can also follow a printf-like format which will be formatted with
//! the variables that follow it. No check for valid bytestream is performed
//! @return Returns the initialized RF_string or null in case of failure to initialize
//! @see rfString_Init_unsafe()
RF_String* rfString_Create_unsafe(const char* s);



//! @memberof RF_String
//! @brief Initializes a string with the given characters with no checking
//!
//! @notinherited{StringX}
//! @warning NO VALID-UTF8 check is performed.
//! @param str The string to initialize
//! @param s The sequence of bytes for the characters in UTF-8 (the default).Can also follow a printf-like format which will be formatted with
//! the variables that follow it. No check for valid bytestream is performed
//! @see rfString_Create_unsafe()
i_DECLIMEX_ void rfString_Init_unsafe(RF_String* str,const char* s);


//! @memberof RF_String
//! @opassign
//! @brief Allocates and returns a string with the given integer.
//!
//! @notinherited{StringX}
//! @param i The integer to turn into a string
//! @return Returns the initialized RF_string
//! @see rfString_Init_i()
i_DECLIMEX_ RF_String* rfString_Create_i(int32_t i);
//! @memberof RF_String
//! @brief Initializes a string with the given integer.
//!
//! @notinherited{StringX}
//! @param str The string to initialize
//! @param i The integer to turn into a string
//! @return Returns true in case of correct initialization and false otherwise
//! @see rfString_Create_i()
i_DECLIMEX_ char rfString_Init_i(RF_String* str,int32_t i);
//! @memberof RF_String
//! @opassign
//! @brief Allocates and returns a string with the given float.
//!
//! @notinherited{StringX}
//! @param f The float to turn into a string
//! @return Returns the initialized RF_string
//! @see rfString_Init_f()
i_DECLIMEX_ RF_String* rfString_Create_f(float f);
//! @memberof RF_String
//! @brief Initializes a string with the given float.
//!
//! @notinherited{StringX}
//! @param str The string to initialize
//! @param f The float to turn into a string
//! @return Returns true in case of correct initialization and false otherwise
//! @see rfString_Create_f()
i_DECLIMEX_ char rfString_Init_f(RF_String* str,float f);

//! @memberof RF_String
//! @brief Allocates and returns a string with the given UTF-16 byte sequence.
//!
//! @notinherited{StringX}
//! Given characters have to be in UTF-16 and in the endianess of the system. They also have to be null terminated.
//! @param s A buffer of 2-byte words representing the utf-16 byte sequence. Needs to be null terminated.
//! @return Returns the initialized RF_string or null in case of failure to initialize, due to invalid utf-16 sequence or illegal endianess value
//! @see rfString_Init_UTF16()
//! @see rfString_Create_fUTF16()
//! @see rfString_Init_UTF16()
i_DECLIMEX_ RF_String* rfString_Create_UTF16(const uint16_t* s);
//! @memberof RF_String
//! @brief Initializes a string with the given UTF-16 byte sequence.
//!
//! @notinherited{StringX}
//! Given characters have to be in UTF-16 and in the endianess of the system. They also have to be null terminated.
//! @param str The string to initialize
//! @param s A buffer of 2-byte words representing the utf-16 byte sequence. Needs to be null terminated.
//! @return Returns true for succesfull initialization and false otherwise due to invalid utf-16 sequence or illegal endianess value
//! @see rfString_Create_UTF16()
//! @see rfString_Create_fUTF16()
//! @see rfString_Init_UTF16()
i_DECLIMEX_ char rfString_Init_UTF16(RF_String* str,const uint16_t* s);

//! @memberof RF_String
//! @cppnotctor
//! @brief Allocates and returns a string with the given UTF-32 byte sequence.
//!
//! @notinherited{StringX}
//! Given characters have to be in UTF-32 and in the endianess of the system.
//! No endianess swapping occurs in the function
//! @param s A buffer of 4-byte words representing the utf-32 byte sequence. Needs to be null terminated.
//! @return Returns the initialized RF_string or null in case of failure to initialize
//! @see rfString_Init_UTF32()
//! @see rfString_Create_fUTF32()
//! @see rfString_Init_UTF32()
i_DECLIMEX_ RF_String* rfString_Create_UTF32(const uint32_t* s);
//! @memberof RF_String
//! @brief Initializes a string with the given UTF-32 byte sequence.
//!
//! @notinherited{StringX}
//! Given characters have to be in UTF-32 and in the endianess of the system.
//! No endianess swapping occurs in the function
//! @param str The string to initialize
//! @param s A buffer of 4-byte words representing the utf-32 byte sequence. Needs to be null terminated.
//! @return Returns true for successful initialization and false otherwise
//! @see rfString_Create_UTF32()
//! @see rfString_Create_fUTF32()
//! @see rfString_Init_UTF32()
i_DECLIMEX_ char rfString_Init_UTF32(RF_String* str,const uint32_t* s);

//! @}

/*-------------------------------------------------------------------------Methods to assign to an RF_String-------------------------------------------------------------------------------*/
//! @name Assigning to a String
//! @{

//! @memberof RF_String
//! @brief Assigns the value of the source string to the destination.
//!
//! @notinherited{StringX}
//! @lmsFunction
//! Both strings should already be initialized and hold a value. It is an error to give null parameters.
//! @param dest The destination string, which should get assigned
//! @param source The source string, whose values to copy. @inhtype{String,StringX} @tmpSTR
//! @see rfString_Assign_char()
//! @see rfStringX_Assign()
i_DECLIMEX_ void rfString_Assign(RF_String* dest,const void* source);


//! @memberof RF_String
//! @brief Assigns the value of a unicode character to the string
//!
//! @notinherited{StringX}
//! @param thisstr The string to assign to
//! @param character The unicode character codepoint to assign to the String
//! @return Returns @c true for succesfull assignment and @c false if the given @c character was not a valid unicode codepoint
//! @see rfString_Assign()
//! @see rfStringX_Assign_char()
i_DECLIMEX_ char rfString_Assign_char(RF_String* thisstr,uint32_t character);

//! @}


/*------------------------------------------------------------------------ RF_String Copying functions-------------------------------------------------------------------------------*/
//! @name String Copying Functions
//! @{

//! @memberof RF_String
//! @cppignore
//! @brief Creates and returns an allocated copy of the given string
//!
//! @isinherited{StringX}
//! @note The Returned Substring needs to be freed by the user. BEWARE when assigning to a string using this function since if any previous string exists there IS NOT getting freed. You have to free it explicitly
//! @param src The string to copy from. @inhtype{String,StringX}
//! @return Returns a string copied from the previous one or null if the original string was null
//! @see rfString_Copy_IN()
//! @see rfString_Copy_chars()
i_DECLIMEX_ RF_String* rfString_Copy_OUT(const void* src);
//! @memberof RF_String
//! @cppignore
//! @brief Copies all the contents of a string to another
//!
//! @isinherited{StringX}
//! @param dst The string to copy in.
//! @param src The string to copy from. @inhtype{String,StringX}
//! If the value is bigger than the maximum number of characters then still all characters are copied.
//! @see rfString_Copy_OUT()
//! @see rfString_Copy_chars()
i_DECLIMEX_ void rfString_Copy_IN(RF_String* dst,const void* src);
//! @memberof RF_String
//! @brief Copies a certain number of characters from a string
//!
//! @isinherited{StringX}
//! Copies @c n characters from @c src String into the destination @c dst string.
//! @param dst The string to copy in
//! @param src The string to copy from. @inhtype{String,StringX}
//! @param n The number of characters to copy from the @c src string
//! If the value is bigger than the maximum number of characters then still all characters are copied.
//! @see rfString_Copy_IN()
//! @see rfString_Copy_OUT()
i_DECLIMEX_ void rfString_Copy_chars(RF_String* dst,const void* src,uint32_t n);

//! @}


/*-------------------------------------------------------------------------Methods to get rid of an RF_String-------------------------------------------------------------------------------*/
//! @name Getting rid of an RF_String
//! @{

//! @memberof RF_String
//! @cppignore
//! @brief Deletes a string object and also frees its pointer.
//!
//! @notinherited{StringX}
//! It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
//! Use it for strings made with _Create
//! @param s The string for deletion
//! @see rfString_Deinit()
i_DECLIMEX_ void rfString_Destroy(RF_String* s);
//! @memberof RF_String
//! @cppignore
//! @brief Deletes a string object only, not its memory.
//!
//! @notinherited{StringX}
//! It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
//! Use it for strings made with _Init
//! @param s The string for deletion
//! @see rfString_Destroy()
i_DECLIMEX_ void rfString_Deinit(RF_String* s);


//! @}
/*------------------------------------------------------------------------ RF_String Equality check -------------------------------------------------------------------------------*/
//! @name Equality check
//! @{

//! @memberof RF_String
//! @opcmpeq
//! @brief Compares two Strings and returns true if they are equal and false otherwise
//!
//! @isinherited{StringX}
//! @lmsFunction
//! @param s1 The first string to compare @inhtype{String,StringX} @tmpSTR
//! @param s2 The second string to compare @inhtype{String,StringX} @tmpSTR
//! @return True in case the strings are equal and false otherwise
i_DECLIMEX_ char rfString_Equal(const void* s1,const void* s2);

//! @}


/*------------------------------------------------------------------------ RF_String iteration functions-------------------------------------------------------------------------------*/
//! @name String Iteration
//! @{


// **The following 2 functions are exposed here only because they are used in the iteration macros** //
//! @internal
//! @memberof RF_String
//! @cppignore
//! @brief Retrieves the unicode code point of the parameter bytepos of the string.
//!
//! @isinherited{StringX}
//! This is an internal function, there is no need to use it. The reason it is exposed here is that it is utilized in the iteration macros.
//! @warning DO NOT use this function unless you know what you are doing
//! @param thisstr The string whose byte position code point we need. @inhtype{String,StringX}
//! @param bytepos The byte position of the string from where to get the code point.
//! @warning If this is out of bounds then nothing can detect it and at best it will cause a SEG FAULT.
//!                 Moreover no check to see if this is not a continutation byte is made. All the checks must have been made before calling the function.
//! @return Returns the code point of the byte position as an uint32_t
//! @endinternal
i_DECLIMEX_ uint32_t rfString_BytePosToCodePoint(const void* thisstr,uint32_t bytepos);
//! @internal
//! @memberof RF_String
//! @cppignore
//! @brief Retrieves character position of a byte position
//!
//! @isinherited{StringX}
//! This is an internal function, there is no need to use it. It attempts to retrieve character position from a byte position. If the byte
//! position is a continutation byte and does not constitute the start of a character then depending on the option the function will find
//! either the next character or the previous character position from this byte position
//!
//! @warning DO NOT use this function unless you know what you are doing
//! @param thisstr The string whose byte position code point we need. @inhtype{String,StringX}
//! @param bytepos The byte position of the string from where to get the character position
//! @param before A boolean flag denoting the behaviour in case this byte position is a continutation byte. If @c before is true then
//! the function will retrieve the first character position before the byte. If it is false, it will retrieve the first character position
//! after the continuation byte.
//! @endinternal
i_DECLIMEX_ uint32_t rfString_BytePosToCharPos(const void* thisstr,uint32_t bytepos,char before);


// Checks if a given byte is a continuation byte
#define rfUTF8_IsContinuationByte2(b__)  ( b__ >= 0x80 && b__<= 0xBF )

// Two macros to accomplish iteration of an RF_String from any given character going forwards. This macro should be used with its end pair.
// We take advantage of the fact that an RF_String is always guaranteed to contain a valid UTF-8 sequence and thus no checks are performed.
/**
** @memberof RF_String
** @cppignore
** @brief Starts an RF_String forward iteration scope.
**
** @isinherited{StringX}
** Use this macro to iterate every character inside an RF_String or RF_StringX\n
** Must be used with its pair macro #rfString_Iterate_End.\n
** As an example consider this code that iterates every character of a string from the start to finish
** @code
** uint32_t i = 0;
** uint32_t charValue;
** RF_String foo;rfString_Init(&foo,"I am a String");
** rfString_Iterate_Start(&foo,i,charValue)
**      //for every character in the string,let's print it
**      printf("Character at index %d is %c\n",i,charValue);
** rfString_Iterate_End(i)
** @endcode
** @param[in] string_ The string to iterate. Must be a pointer to string
** @param[in,out] startCharacterPos_ Here give an uint32_t which will be the character position from which to start the iteration. In each iteration this will hold the character index. If the given position is out of bounds then the iteration does not happen
** @param[in,out] characterUnicodeValue_ Here pass an uint32_t which in each iteration will hold the unicode code point of the character at position startCharacterPos_
** @see rfString_Iterate_End()
** @see rfString_IterateB_Start()
**/
#define rfString_Iterate_Start(string_,startCharacterPos_,characterUnicodeValue_)     {\
            /* b index sec is the byte index and j the character index*/\
            uint32_t byteIndex_ = 0;uint32_t j_=0;\
            /*iterate until we find the character position requested and its equivalent byte position*/\
            while(j_!=startCharacterPos_)\
            {\
                if( rfUTF8_IsContinuationByte( ((RF_String*)(string_))->bytes[byteIndex_]) ==false)\
                {\
                    j_++;\
                }\
                byteIndex_++;\
            }\
            /*now start the requested iteration*/\
            while( ((RF_String*)(string_))->bytes[byteIndex_]!='\0')\
            {\
                /*if it's a character*/\
                if( rfUTF8_IsContinuationByte( ((RF_String*)(string_))->bytes[byteIndex_]) ==false)\
                {/*Give the character value to the user*/\
                    characterUnicodeValue_ = rfString_BytePosToCodePoint( (string_),byteIndex_);

//! @memberof RF_String
//! @cppignore
//! @brief Ends an RF_String/RF_StringX forward iteration scope.
//!
//! @isinherited{StringX}
//! Look at #rfString_Iterate_Start for an example usage
//! @param[in,out] startCharacterPos_ Here give the uint32_t given to #rfString_Iterate_Start
//! @see rfString_Iterate_Start()
#define rfString_Iterate_End(startCharacterPos_)  startCharacterPos_++;}byteIndex_++;}}

//Two macros to accomplish iteration of an RF_String from any given character going backwards. This macro should be used with its end pair.
// We take advantage of the fact that an RF_String is always guaranteed to contain a valid UTF-8 sequence and thus no checks are performed.

/**
** @memberof RF_String
** @cppignore
** @brief Starts an RF_String backward iteration scope.
**
** @isinherited{StringX}
** Use this macro to iterate every character inside an RF_String or RF_StringX going backwards\n
** Must be used with its pair macro #rfString_IterateB_End.\n
**
** As an example consider this code that iterates every character of a string from the start to finish
** @code
** uint32_t charValue;
** RF_String foo;rfString_Init(&foo,"I am a String");
** uint32_t i = rfString_Length(&foo)-1;//-1 is since String indexing starts from 0
** rfString_IterateB_Start(&foo,i,charValue)
**      //for every character in the string,let's print it
**      printf("Character at index %d is %c\n",i,charValue);
** rfString_IterateB_End(i)
** @endcode
** @param[in] string_ The string to iterate. Must be a pointer to string
** @param[in,out] characterPos_ Here give an uint32_t which will be the character position from which to start the iteration. In each iteration this will hold the character index. If the given position is out of bounds then the iteration does not happen
** @param[in,out] characterUnicodeValue_ Here pass an uint32_t which in each iteration will hold the unicode code point of the character at position characterPos_
** @see rfString_IterateB_End()
** @see rfString_Iterate_Start()
**/
#define rfString_IterateB_Start(string_,characterPos_,characterUnicodeValue_)     {\
            /* b index is the byte index and j the character index*/\
            uint32_t b_index_ = 0;uint32_t j_=0;\
            /* c index sec is another signed copy of the character index (and is int64_t so that it can cater for any situation). Reason is cause going backwards we gotta have -1 too */\
            int64_t c_index_ = characterPos_;\
            /*iterate until we find the character position requested and its equivalent byte position*/\
            while(j_!=characterPos_)\
            {\
                if( rfUTF8_IsContinuationByte( ((RF_String*)(string_))->bytes[b_index_]) ==false)\
                {\
                    j_++;\
                }\
                b_index_++;\
            }\
            /*now start the requested iteration - notice that the end condition is to reach the first character position*/\
            while(c_index_!=-1)\
            {\
                /*if it's a character*/\
                if( rfUTF8_IsContinuationByte( ((RF_String*)(string_))->bytes[b_index_]) ==false)\
                {/*Give the character value to the user*/\
                    characterUnicodeValue_ = rfString_BytePosToCodePoint( (string_),b_index_);

//! @memberof RF_String
//! @cppignore
//! @brief Ends an RF_String/RF_StringX backward iteration scope.
//!
//! @isinherited{StringX}
//! Look at #rfString_IterateB_Start for an example usage
//! @param[in,out] characterPos_ Here give the uint32_t given to #rfString_IterateB_Start
//! @see rfString_IteraB_Start()
#define rfString_IterateB_End(characterPos_)  c_index_-- ;characterPos_--;}b_index_--;}}

//! @}


#ifdef __cplusplus
}///closing bracket for calling from C++
#endif

#endif//include guards end
