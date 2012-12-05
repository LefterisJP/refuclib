#include <String/core.h>

#include <String/format.h> //for the String formatting function
#include "common.ph"//for private string iteration

#include <IO/rf_unicode.h> //for rfUTF8_VerifySequence()
#include <rf_localmem.h>//for local stack pushing
#include "../stdio.ph" //for the ioBuffer RF_StringX

#include <rf_error.h>
#include <rf_memory.h>
/*-------------------------------------------------------------------------Methods to create an RF_String-------------------------------------------------------------------------------*/

//Allocates and returns a string with the given characters a refu string with the given characters. Given characters have to be in UTF-8. A check for valide sequence of bytes is performed.
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
RF_String* rfString_Create(const char* s,...)
#else
RF_String* i_rfString_Create(const char* s,...)
#endif
{
    RF_String* ret;
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()

    va_start(args,s);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,s,args)!=RF_SUCCESS)
    {
        LOG_ERROR("String creation failure due to failing at reading the formatted string",RF_LastError);
        return 0;
    }
    va_end(args);

    RF_MALLOC(ret,sizeof(RF_String));
    //get length
    ret->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(ret->bytes,ret->byteLength+1);
    memcpy(ret->bytes,ioBuffer.INH_String.bytes,ret->byteLength+1);


    return ret;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
RF_String* i_NVrfString_Create(const char* s)
{
    RF_String* ret;
    uint32_t byteLength;

    //check for validity of the given sequence and get the character length
    if( rfUTF8_VerifySequence(s,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at String Allocation due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return 0;
    }
    RF_MALLOC(ret,sizeof(RF_String));
    //get length
    ret->byteLength = byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(ret->bytes,ret->byteLength+1);
    memcpy(ret->bytes,s,ret->byteLength+1);

    return ret;
}
#endif


// Initializes a string with the given characters. Given characters have to be in UTF-8. A check for valid sequence of bytes is performed.<b>Can't be used with RF_StringX</b>
#ifndef RF_OPTION_DEFAULT_ARGUMENTS
char rfString_Init(RF_String* str,const char* s,...)
#else
char i_rfString_Init(RF_String* str,const char* s,...)
#endif
{
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()

    va_start(args,s);
    //read the var args
    if(rfStringX_Formatv(&ioBuffer,s,args)!=RF_SUCCESS)
    {
        LOG_ERROR("String creation failure due to failing at reading the formatted string",RF_LastError);
        return false;
    }
    va_end(args);

    //get length
    str->byteLength = ioBuffer.INH_String.byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(str->bytes,str->byteLength+1);
    memcpy(str->bytes,ioBuffer.INH_String.bytes,str->byteLength+1);

    return true;
}
#ifdef RF_OPTION_DEFAULT_ARGUMENTS
char i_NVrfString_Init(RF_String* str,const char* s)
{
    //check for validity of the given sequence and get the character length
    uint32_t byteLength;
    if( rfUTF8_VerifySequence(s,&byteLength) != RF_SUCCESS)
    {
        LOG_ERROR("Error at String Initialization due to invalid UTF-8 byte sequence",RE_STRING_INIT_FAILURE);
        return false;
    }

    //get length
    str->byteLength = byteLength;
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(str->bytes,str->byteLength+1);
    memcpy(str->bytes,s,str->byteLength+1);

    return true;
}
#endif

//Allocates a String by turning a unicode code point in a String (encoded in UTF-8).
RF_String* rfString_Create_cp(uint32_t codepoint)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    if(rfString_Init_cp(ret,codepoint) == true)
    {
        return ret;
    }
    //failure
    free(ret);
    return 0;
}

//Initializes a string by turning a unicode code point in a String (encoded in UTF-8).
char rfString_Init_cp(RF_String* str, uint32_t codepoint)
{
    //alloc enough for a character
    RF_MALLOC(str->bytes,5)

    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        str->bytes[0] = codepoint;
        str->bytes[1] = '\0';
        str->byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->bytes[2] = '\0';
        str->byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->bytes[3] = '\0';
        str->byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->bytes[4] = '\0';
        str->byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into a string",RE_UTF8_INVALID_CODE_POINT);
        free(str->bytes);
        return false;
    }

    return true;
}


// Allocates and returns a string with the given integer
RF_String* rfString_Create_i(int32_t i)
{
    //the size of the int32_t buffer
    int32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%d",i);
    numLen = strlen(buff);

    //initialize the string and return it
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    ret->byteLength = numLen;
    RF_MALLOC(ret->bytes,numLen+1);
    strcpy(ret->bytes,buff);
    return ret;
}
// Initializes a string with the given integer.
char rfString_Init_i(RF_String* str, int32_t i)
{
    //the size of the int32_t buffer
    int32_t numLen;
    //put the int32_t into a buffer and turn it in a char*
    char buff[12];//max uint32_t is 4,294,967,295 in most environment so 12 chars will certainly fit it
    sprintf(buff,"%d",i);
    numLen = strlen(buff);


    str->byteLength = numLen;
    RF_MALLOC(str->bytes,numLen+1);
    strcpy(str->bytes,buff);

    return true;
}

// Allocates and returns a string with the given float
RF_String* rfString_Create_f(float f)
{
    //allocate a buffer for the float in characters
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);
    uint32_t len = strlen(buff);

    //initialize and return the string
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    ret->byteLength = len;
    RF_MALLOC(ret->bytes,len+1);
    strcpy(ret->bytes,buff);

    free(buff);

    return ret;
}
// Initializes a string with the given float
char rfString_Init_f(RF_String* str,float f)
{
    //allocate a buffer for the float in characters
    char* buff;
    RF_MALLOC(buff,128);
    sprintf(buff,"%f",f);
    uint32_t len = strlen(buff);


    str->byteLength = len;
    RF_MALLOC(str->bytes,len+1);
    strcpy(str->bytes,buff);
    free(buff);

    //success
    return true;
}

//Allocates and returns a string with the given UTF-16 byte sequence. Given characters have to be in UTF-16. A check for valid sequence of bytes is performed.<b>Can't be used with RF_StringX</b>
RF_String* rfString_Create_UTF16(const uint16_t* s)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    if(rfString_Init_UTF16(ret,s)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes a string with the given UTF-16 byte sequence. Given characters have to be in UTF-16. A check for valid sequence of bytes is performed.<b>Can't be used with RF_StringX</b>
char rfString_Init_UTF16(RF_String* str,const uint16_t* s)
{
    //decode the utf-16 and get the code points
    uint32_t* codepoints;
    uint32_t byteLength,characterLength,utf8ByteLength;
    char* utf8;
    byteLength = 0;
    while(s[byteLength]!= 0 || s[byteLength+1]!=0)
    {
        byteLength++;
    }
    byteLength+=3;//for the last utf-16 null termination character
    RF_MALLOC(codepoints,byteLength*2) //allocate the codepoints
    //parse the given byte stream normally since it has to be in the endianess of the system
    if(rfUTF16_Decode((char*)s,&characterLength,codepoints) != RF_SUCCESS)
    {
        free(codepoints);
        LOG_ERROR("String initialization failed due to invalide UTF-16 sequence",RE_STRING_INIT_FAILURE);
        return false;
    }
    //now encode these codepoints into UTF8
    if( (utf8 = rfUTF8_Encode(codepoints,characterLength,&utf8ByteLength))==0)
    {
        free(codepoints);
        return false;
    }
    //success
    free(codepoints);
    str->bytes = utf8;
    str->byteLength = utf8ByteLength;
    return true;

}

//Allocates and returns a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32.
RF_String* rfString_Create_UTF32(const uint32_t* s)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    if(rfString_Init_UTF32(ret,s)==false)
    {
        free(ret);
        return 0;
    }
    return ret;
}
//Initializes a string with the given UTF-32 byte sequence. Given characters have to be in UTF-32.
char rfString_Init_UTF32(RF_String* str,const uint32_t* codeBuffer)
{
    //find the length of the utf32 buffer in characters
    uint32_t length;
    rfUTF32_Length(codeBuffer,length);

    //turn the codepoints into a utf-8 encoded buffer
    char* utf8;uint32_t utf8ByteLength;
    if((utf8=rfUTF8_Encode(codeBuffer,length,&utf8ByteLength)) == 0)
    {
        LOG_ERROR("Could not properly encode the UTF32 buffer into UTF8",RE_UTF8_ENCODING);
        return false;
    }

    str->bytes = utf8;
    str->byteLength = utf8ByteLength;
    return true;
}

// Allocates and returns a string with the given characters. NO VALID-UTF8 check is performed
RF_String* rfString_Create_unsafe(const char* s)
{
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String))
    //get length
    ret->byteLength = strlen(s);
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(ret->bytes,ret->byteLength+1);
    memcpy(ret->bytes,s,ret->byteLength+1);
    return ret;
}


// Initializes a string with the given characters. NO VALID-UTF8 check is performed
void rfString_Init_unsafe(RF_String* str,const char* s)
{
    //get its length
    str->byteLength = strlen(s);
    //now that we know the length we can allocate the buffer and copy the bytes
    RF_MALLOC(str->bytes,str->byteLength+1);
    memcpy(str->bytes,s,str->byteLength+1);

}


// Assigns the value of the source string to the destination.Both strings should already be initialized and hold a value. It is an error to give null parameters.
void rfString_Assign(RF_String* dest,const void* sourceP)
{
    const RF_String* source = (const RF_String*)sourceP;
    RF_ENTER_LOCAL_SCOPE()
    //only if the new string value won't fit in the buffer reallocate the buffer (let's avoid unecessary reallocs)
    if(source->byteLength > dest->byteLength)
    {
        RF_REALLOC(dest->bytes,char,source->byteLength+1);
    }
    //now copy the value
    memcpy(dest->bytes,source->bytes,source->byteLength+1);
    //and fix the lengths
    dest->byteLength = source->byteLength;
    RF_EXIT_LOCAL_SCOPE()
}

//Assigns the value of a unicode character to the string
char rfString_Assign_char(RF_String* str,uint32_t codepoint)
{
    //realloc if needed
    if(str->byteLength <5)
    {
        RF_REALLOC(str->bytes,char,5);
    }
    //if we only need a byte to encode it
    if(RF_HEXLE_UI(codepoint,0x007f))
    {
        str->bytes[0] = codepoint;
        str->bytes[1] = '\0';
        str->byteLength = 1;
    }
    //if we need 2 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0080) && RF_HEXLE_UI(codepoint,0x07ff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[1] = (codepoint & 0x3F)|(0x02<<6);
        //get the 5 following bits and encode them in the second byte
        str->bytes[0] = ((codepoint & 0x7C0) >> 6)  | (0x6<<5);
        str->bytes[2] = '\0';
        str->byteLength = 2;
    }
    //if we need 3 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x0800) && RF_HEXLE_UI(codepoint,0x0ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[2] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[1] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 4 following bits and encode them in the third byte
        str->bytes[0] = (((codepoint & 0xF000))>>12) | (0xE<<4);
        str->bytes[3] = '\0';
        str->byteLength = 3;
    }
    //if we need 4 bytes to encode it
    else if( RF_HEXGE_UI(codepoint,0x10000) && RF_HEXLE_UI(codepoint,0x10ffff))
    {
        //get the first bits of the first byte and encode them to the first byte
        str->bytes[3] = (codepoint & 0x3F)|(0x02<<6);
        //get the 6 following bits and encode them in the second byte
        str->bytes[2] = ((codepoint & 0xFC0) >> 6)  | (0x02<<6);
        //get the 6 following bits and encode them in the third byte
        str->bytes[1] = (((codepoint & 0x3F000))>>12) | (0x02<<6);
        //get the 3 following bits and encode them in the fourth byte
        str->bytes[0] = (((codepoint & 0x1C0000))>>18) | (0x1E<<3);
        str->bytes[4] = '\0';
        str->byteLength = 4;
    }
    else
    {
        LOG_ERROR("Attempted to encode an invalid unicode code point into a string",RE_UTF8_INVALID_CODE_POINT);
        return false;
    }

    return true;
}

/*------------------------------------------------------------------------ RF_String copying functions-------------------------------------------------------------------------------*/


//Creates and returns an allocated copy of the given string
RF_String* rfString_Copy_OUT(const void* srcP)
{
    const RF_String* src = (const RF_String*)srcP;
    //create the new string
    RF_String* ret;
    RF_MALLOC(ret,sizeof(RF_String));
    //get the length
    ret->byteLength = src->byteLength;
    //copy the bytes
    RF_MALLOC(ret->bytes,ret->byteLength+1);
    memcpy(ret->bytes,src->bytes,ret->byteLength+1);
    return ret;

}
// Copies all the contents of a string to another
void rfString_Copy_IN(RF_String* dst,const void* srcP)
{
    const RF_String* src = (const RF_String*)srcP;
    //get the length
    dst->byteLength = src->byteLength;
    //copy the bytes
    RF_MALLOC(dst->bytes,src->byteLength+1);
    memcpy(dst->bytes,src->bytes,dst->byteLength+1);
    return;

}
//Copies a certain number of characters from a string
void rfString_Copy_chars(RF_String* dst,const void* srcP,uint32_t charsN)
{
    uint32_t i = 0,bytePos;
    const RF_String* src = (const RF_String*)srcP;

    //find the byte position until which we need to copy
    RF_STRING_ITERATE_START(src,i,bytePos)
        if(i == charsN)
            break;
    RF_STRING_ITERATE_END(i,bytePos)
    dst->byteLength = bytePos;
    RF_MALLOC(dst->bytes,dst->byteLength+1);
    memcpy(dst->bytes,src->bytes,dst->byteLength+1);
    dst->bytes[dst->byteLength] = '\0';//null terminate it
}


/*-------------------------------------------------------------------------Methods to get rid of an RF_String-------------------------------------------------------------------------------*/

// Deletes a string object and also frees its pointer.It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
void rfString_Destroy(RF_String* s)
{
    free(s->bytes);
    free(s);
}
// Deletes a string object only, not its memory.It is an error to give a NULL(0x0) string for deleting. Will most probably lead to a segmentation fault
void rfString_Deinit(RF_String* s)
{
    free(s->bytes);
}

/*-------------------------------------------------------------------------Equality check-------------------------------------------------------------------------------*/

// Compares two Strings and returns true if they are equal and false otherwise
char rfString_Equal(const void* s1P,const void* s2P)
{
    const RF_String* s1 = (const RF_String*)s1P;
    const RF_String* s2 = (const RF_String*)s2P;
    char ret = false;
    RF_ENTER_LOCAL_SCOPE()
    if( strcmp(s1->bytes,s2->bytes)==0)
        ret = true;

    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

/*-------------------------------------------------------------------------2 functions used in the iteration macros-------------------------------------------------------------------------------*/

//Retrieves the unicode code point of the parameter bytepos of the string. If the byte position is not the start of a character 0 is returned. This is an internal function, there is no need to use it. <i>Can be used with StringX</i>
uint32_t rfString_BytePosToCodePoint(const void* str,uint32_t i)
{
    uint32_t codePoint=0;
    const RF_String* thisstr = (const RF_String*)str;
    ///Here I am not checking if byte position 'i' is withing bounds and especially if it is a start of a character
    /// This is assumed to have been checked or to be known beforehand by the programmer. That's one of the reasons
    /// why this is an internal function and should not be used unless you know what you are doing
    //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
    if( ((thisstr->bytes[i] & 0x80)>>7) == 0 )
    {
        //and the code point is this whole byte only
        codePoint = thisstr->bytes[i];
    }
    //if the leading bits are in the form of 0b110xxxxx then range is: U+0080 to U+07FF (2 bytes)
    else if ( RF_HEXEQ_C( ( (~(thisstr->bytes[i] ^  0xC0))>>5),0x7) )
    {
        codePoint =0;
        //from the second byte take the first 6 bits
        codePoint = (thisstr->bytes[i+1] & 0x3F) ;
        //from the first byte take the first 5 bits and put them in the start
        codePoint |= ((thisstr->bytes[i] & 0x1F) << 6);
    }
    //if the leading bits are in the form of 0b1110xxxx then range is U+0800 to U+FFFF  (3 bytes)
    else if( RF_HEXEQ_C( ( (~(thisstr->bytes[i] ^ 0xE0))>>4),0xF) )
    {
        codePoint = 0;
        //from the third byte take the first 6 bits
        codePoint = (thisstr->bytes[i+2] & 0x3F) ;
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i+1] & 0x3F) << 6);
        //from the first byte take the first 4 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i] & 0xF) << 12);
    }
    //if the leading bits are in the form of 0b11110xxx then range is U+010000 to U+10FFFF (4 bytes)
    else if( RF_HEXEQ_C( ( (~(thisstr->bytes[i] ^ 0xF0))>>3), 0x1F))
    {
        codePoint = 0;
        //from the fourth byte take the first 6 bits
        codePoint = (thisstr->bytes[i+3] & 0x3F) ;
        //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i+2] & 0x3F) << 6);
        //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i+1] & 0x3F) << 12);
        //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
        codePoint |= ((thisstr->bytes[i] & 0x7) << 18);
    }

    return codePoint;
}
//Retrieves character position of a byte position
uint32_t rfString_BytePosToCharPos(const void* thisstrP,uint32_t bytepos,char before)
{
    ///here there is no check if this is actually a byte pos inside the string's
    ///byte buffer. The programmer should have made sure it is before hand. This is why it is
    /// an internal function and should only be used if you know what you are doing
    const RF_String* thisstr = (const RF_String*)thisstrP;
    uint32_t charPos = 0;
    uint32_t byteI = 0;
    //iterate the string's bytes until you get to the required byte
    //if it is not a continuation byte, return the position
    if(rfUTF8_IsContinuationByte(thisstr->bytes[bytepos])==false)
    {
        RF_STRING_ITERATE_START(thisstr,charPos,byteI)
            if(byteI == bytepos)
                return charPos;
        RF_STRING_ITERATE_END(charPos,byteI)
    }
    //else  iterate the string's bytes until you get anything bigger than the required byte
    RF_STRING_ITERATE_START(thisstr,charPos,byteI)
        if(byteI > bytepos)
            break;
    RF_STRING_ITERATE_END(charPos,byteI)
    //if we need the previous one return it
    if(before == true)
        return charPos-1;
    //else return this
    return charPos;
}

