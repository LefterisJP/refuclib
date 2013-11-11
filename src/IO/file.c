/**
**      ==START OF REFU LICENSE==
**
** Copyright (c) 2011-2013, Karapetsas Eleftherios
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the distribution.
**  3. Neither the name of the Original Author of Refu nor the names of its contributors may be used to endorse or promote products derived from
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**      ==END OF REFU LICENSE==
**
*/
/*------------- Corrensponding Header inclusion -------------*/
#include <IO/file.h>
/*------------- Module related inclusion -------------*/
#include <IO/common.h> //for common I/O flags and definitions
#include "io.ph"//for private I/O macros
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
//for unicode related macro
    #include <String/unicode.h> //for rfUTF8_IsContinuationbyte
//for constant compare macros
    #include <Utils/constcmp.h> //for RF_HEXLE_US() macro and others
//for endianess functions
    #include <Utils/endianess.h>
/*------------- libc inclusion --------------*/
#include <string.h>//for memcpy e.t.c.
/*------------- End of includes -------------*/

// Reads a UTF-8 file descriptor until end of line or EOF is found and
// returns a UTF-8 byte buffer
bool rfFReadLine_UTF8(FILE* f, char eol, char** utf8,
                      uint32_t* byteLength,
                      uint32_t* bufferSize, char* eof)
{
    uint32_t bIndex=0;
    uint32_t bytesN;
    //allocate the utf8 buffer
    *bufferSize = RF_OPTION_FGETS_READ_BYTESN+4;
    RF_MALLOC(*utf8, *bufferSize, RE_MALLOC_FAILURE);
    *byteLength = 0;
    //read the start
    if(!rfFgets_UTF8(*utf8, RF_OPTION_FGETS_READ_BYTESN, f,
                     eof, eol, &bytesN))
    {
        free(*utf8);
        RF_ERROR("Failed to read a line from a UTF-8 file");
        return false;
    }
    (*byteLength) += bytesN;

    //if the last character was a newline we are done
    if(*((*utf8)+bytesN-1) == (char)RF_LF)
    {
        return true;
    }

    if (bytesN >= RF_OPTION_FGETS_READ_BYTESN &&
        (*eof) == false)//if the size does not fit in the buffer and
    { //if we did not reach the end of file
        //keep reading until we have read all until newline or EOF
        while(bytesN >= RF_OPTION_FGETS_READ_BYTESN && (*eof) == false)
        {
            if(*byteLength+RF_OPTION_FGETS_READ_BYTESN+4 >= *bufferSize)
            {
                *bufferSize = (*byteLength + 
                            RF_OPTION_FGETS_READ_BYTESN+4)*2;
                 RF_REALLOC(*utf8, char, *bufferSize,
                            false);
            }
            bIndex += bytesN;
            if(!rfFgets_UTF8((*utf8)+bIndex,
                             RF_OPTION_FGETS_READ_BYTESN,
                             f, eof, eol, &bytesN))
            {
                free(*utf8);
                RF_ERROR("Failed to read a line from a UTF-8 file");
                return false;
            }
            (*byteLength) += bytesN;

            //if the last character was a newline break
            if(*((*utf8) + bIndex + bytesN -1) == (char)RF_LF)
            {
                break;
            }
        }//end of reading loop
    }//end of size not fitting the initial buffer case

    return true;
}
//Reads a Little Endian UTF-16 file descriptor until end of line or EOF is found and returns a UTF-8 byte buffer
bool rfFReadLine_UTF16(FILE* f, char eol, char** utf8,
                         uint32_t* byteLength, char* eof,
                         uint32_t* bytes_read, int endianess)
{
    char buff[RF_OPTION_FGETS_READ_BYTESN+5], ret = true;
    uint32_t *codepoints, charsN, bytesN;
    uint32_t buffSize=RF_OPTION_FGETS_READ_BYTESN+5;
    char* tempBuff = 0,buffAllocated=false;
    *bytes_read = 0;

    if(!rfFgets_UTF16(buff, RF_OPTION_FGETS_READ_BYTESN, f, eof,
                       eol, &bytesN, endianess))
    {
        RF_ERROR("There was an error while readine a line from a UTF16 file "
                 "descriptor");
        return false;
    }
    *bytes_read = bytesN;
    tempBuff = &buff[0];//point the tempBuff to the initial buffer for now

    if(bytesN >= RF_OPTION_FGETS_READ_BYTESN &&
       (*eof) == false)//if the size does not fit in the buffer and if we did not reach the EOF
    {
        //allocate the temporary buffer and move the previous buffer's content inside it
        buffSize = buffSize * 2 + 5;
        RF_MALLOC(tempBuff, buffSize, false);
        memcpy(tempBuff ,buff ,bytesN);
        buffAllocated = true;
        //keep reading until we have read all until newline or EOF
        do
        {
            if(!rfFgets_UTF16(tempBuff + (*bytes_read),
                              RF_OPTION_FGETS_READ_BYTESN,
                              f, eof, eol, &bytesN, endianess))
            {
                RF_ERROR("There was an error while readine a line from a UTF16 file "
                         "descriptor");
                ret = false;
                goto cleanup2;
            }
            (*bytes_read) += bytesN;

            //realloc to have more space in the buffer for reading if needed
            if((*bytes_read) + RF_OPTION_FGETS_READ_BYTESN + 5 >= buffSize)
            {
                buffSize=((*bytes_read) + RF_OPTION_FGETS_READ_BYTESN + 5) * 2;
                RF_REALLOC(tempBuff, char, buffSize, false);
            }
            //if the last character was newline break off the loop
            if( *(uint16_t*)(tempBuff + (*bytes_read) - 2) == (uint16_t)RF_LF)
            {
                break;
            }
        }while(bytesN >= RF_OPTION_FGETS_READ_BYTESN && (*eof)==false);//end of reading loop
    }//end of size not fitting the initial buffer case
    if(bytesN >0)//determine the amount of bytes read
    {
        (*bytes_read) += bytesN;
    }
    //allocate the codepoints
    RF_MALLOC_JMP(codepoints, (*bytes_read + 5)*2, ret = false,
                  cleanup2);
    //decode it into codepoints
    if(!rfUTF16_Decode(tempBuff, &charsN, codepoints, (*bytes_read+5)*2))
    {

        RF_ERROR("Failed to Decode UTF-16 from a File Descriptor");
        ret = false;
        goto cleanup1;
    }
    //now encode these codepoints into UTF8
    RF_MALLOC_JMP(*utf8, charsN*4, ret = false, cleanup1);
    if(!rfUTF8_Encode(codepoints, charsN, byteLength, *utf8, charsN*4))
    {
        RF_ERROR("Failed to encode the File Descriptor's UTF-16 "
                 "bytestream to UTF-8");
        ret = false;
        free(*utf8);
        goto cleanup1;
    }

    //end
  cleanup1:
    free(codepoints);
  cleanup2:
    if(buffAllocated==true)
    {
        free(tempBuff);
    }
    return ret;
}


//Reads a UTF-32 file descriptor until end of line or EOF is found and returns a UTF-8 byte buffer
bool rfFReadLine_UTF32(FILE* f, char eol, char** utf8,
                       uint32_t* byteLength, char* eof,
                       uint32_t* bytes_read, int endianess)
{
    char buff[RF_OPTION_FGETS_READ_BYTESN+7], ret = true;
    uint32_t *codepoints;
    uint32_t buffSize=RF_OPTION_FGETS_READ_BYTESN+7, bytesN = 0;
    char* tempBuff = 0,buffAllocated=false;
    *bytes_read = 0;

    if(!rfFgets_UTF32(buff, RF_OPTION_FGETS_READ_BYTESN, f, eof,
                      eol, &bytesN, endianess))
    {
        RF_ERROR("There was an error while reading a line from a UTF-32 "
                 "file descriptor");
        return false;
    }
    *bytes_read = bytesN;
    tempBuff = &buff[0];//point the tempBuff to the initial buffer for now

    if(bytesN >= RF_OPTION_FGETS_READ_BYTESN &&
       (*eof)==false)//if the size does not fit in the buffer and if we did not reach the EOF
    {
        //allocate the temporary buffer and move the previous buffer's content inside it
        buffSize = (buffSize * 2) + 7;
        RF_MALLOC(tempBuff, buffSize, false);
        memcpy(tempBuff, buff, bytesN);
        *bytes_read = bytesN;
        buffAllocated = true;
        //keep reading until we have read all until newline or EOF
        do
        {
            if(!rfFgets_UTF32(tempBuff + (*bytes_read),
                              RF_OPTION_FGETS_READ_BYTESN,
                              f, eof, eol, &bytesN, endianess))
            {
                RF_ERROR("There was an error while reading a line from a "
                         "UTF-32 file descriptor");
                ret = false;
                goto cleanup;
            }
            (*bytes_read) += bytesN;

            //realloc to have more space in the buffer for reading if needed
            if((*bytes_read) + RF_OPTION_FGETS_READ_BYTESN + 7 >= buffSize)
            {
                buffSize = ((*bytes_read) + RF_OPTION_FGETS_READ_BYTESN + 7) * 2;
                RF_REALLOC(tempBuff, char, buffSize, false);
            }
            //if the last character was newline break off the loop
            if( (*(uint32_t*)(tempBuff + (*bytes_read) - 4)) == (uint32_t)RF_LF)
            {
                break;
            }
        }while(bytesN >= RF_OPTION_FGETS_READ_BYTESN && (*eof)==false);//end of reading loop
    }//end of size not fitting the initial buffer case
    if(bytesN >0)//determine the amount of bytes read
    {
        (*bytes_read) += bytesN;
    }
    //utf-32 is actually codepoints
    codepoints = (uint32_t*)tempBuff;
    //now encode these codepoints into UTF8
    RF_MALLOC_JMP(*utf8, *bytes_read, ret = false, cleanup);
    if(!rfUTF8_Encode(codepoints, (*bytes_read)/4, byteLength, *utf8, *bytes_read))
    {
        RF_ERROR("Failed to encode the File Descriptor's UTF-32 "
                 "bytestream to UTF-8");
        ret = false;
        free(*utf8);
        goto cleanup;
    }


    //end
  cleanup:
    if(buffAllocated==true)
        free(tempBuff);

    return ret;
}


//This is a function that's similar to c library fgets but it also returns the number of bytes read and works for UTF-32 encoded files
bool rfFgets_UTF32(char* buff, uint32_t num, FILE* f,
                   char* eofReached, char eol, uint32_t* bytes_read,
                   int endianess)
{
    uint32_t c;
    bool eolReached;
    //initialization
    *eofReached = eolReached = false;
    *bytes_read = 0;

#if RF_OPTION_DEBUG
    if(endianess != RF_LITTLE_ENDIAN || endianess != RF_BIG_ENDIAN)
    {
        RF_ERROR( "Illegal endianess type given");
        return false;
    }
#endif

    //if end of file or end of line is not found, keep reading
    do{
        if(rfFgetc_UTF32(f, (uint32_t*)(buff + (*bytes_read)),
                         endianess, eofReached) < 0)
        {
            if(*eofReached)
            {
                break;//EOF found
            }
            RF_ERROR("Reading error while reading from a "
                     "UTF-32 byte stream");
            return false;
        }


        *bytes_read += 4;
        //if we have read the number of characters requested by the function
        if(*bytes_read >= num)
        {
            break;
        }

        //newline check depending on the EOL pattern
        c = *(uint32_t*)(buff + (*bytes_read) - 4);
        switch(eol)
        {
            case RF_EOL_LF:
                if(c == RF_LF)
                {
                    eolReached = true;
                }
            break;
            case RF_EOL_CRLF:
                if(c == RF_LF)
                {
                    if( (*(uint32_t*)(buff + (*bytes_read) -8)) == RF_CR)
                    {
                        eolReached = true;
                        (*bytes_read) -= 4;
                        (*(uint32_t*)(buff + (*bytes_read) -4)) = '\n';
                    }
                }
            break;
            case RF_EOL_CR:
                if(c == RF_CR)
                {
                    eolReached = true;
                    (*(uint32_t*)(buff + (*bytes_read) -4)) = '\n';
                }
            break;
        }//end of EOL dependent newline check
    }while(c !=(uint32_t) EOF && !eolReached);
    //null terminate the buffer for UTF32
    buff[(*bytes_read)] =  buff[(*bytes_read) + 1] = buff[(*bytes_read)+2] = 
    buff[(*bytes_read)+3] = '\0';
    //finally check yet again for end of file right after the new line
    if(rfFgetc_UTF32(f,&c, endianess, eofReached) < 0 && !(*eofReached))
    {
            RF_ERROR("Reading error while reading from a "
                     "UTF-32 byte stream");
            return false;
    }

    if(!(*eofReached))
    {//unless it's EOF undo the peak ahead
        if(rfFseek(f,-4,SEEK_CUR) != 0)
        {
            RF_ERROR("Failed to undo the peek ahead of the file pointer"
                     "due to fseek() failure with errno %d", errno);
            return false;
        }
    }
    return true;
}

//Gets a number of bytes from a UTF-16 file descriptor
bool rfFgets_UTF16(char* buff, uint32_t num, FILE* f,
                   char* eofReached, char eol, uint32_t* bytes_read,
                   int endianess)
{
    uint32_t c;
    int32_t bytesN;
    char eolReached;

#if RF_OPTION_DEBUG
    if(endianess != RF_LITTLE_ENDIAN || endianess != RF_BIG_ENDIAN)
    {
        RF_ERROR( "Illegal endianess type given");
        return false;
    }
#endif

    //initialization
    *eofReached = eolReached = false;
    *bytes_read = 0;
    //if end of file or end of line is not found, keep reading
    do{
        bytesN = rfFgetc_UTF16(f,(uint32_t*)(buff + (*bytes_read)), false,
                               endianess, eofReached);
        //error check
        if(bytesN < 0)
        {
            if(*eofReached)
            {
                break;//EOF found
            }
            else
            {
                RF_ERROR("An error was encountered while reading a stream "
                         "of bytes from a UTF-16 file descriptor");
                return false;
            }
        }
        (*bytes_read) += bytesN;
        //if we have read the number of characters requested by the function
        if((*bytes_read) >= num)
        {
            break;
        }
        //newline check depending on the EOL pattern
        c = *(uint32_t*)(buff + (*bytes_read) - bytesN);
        switch(eol)
        {
            case RF_EOL_LF:
                if(c == RF_LF)
                {
                    eolReached = true;
                }
            break;
            case RF_EOL_CRLF:
                if(c == RF_LF)
                {
                    if( (*(uint16_t*)(buff + (*bytes_read) - bytesN-2)) == RF_CR)
                    {
                        eolReached = true;
                        (*bytes_read) -= 2;
                        (*(uint16_t*)(buff + (*bytes_read) - 2)) = '\n';
                    }
                }
            break;
            case RF_EOL_CR:
                if(c == RF_CR)
                {
                    eolReached = true;
                    (*(uint16_t*)(buff + (*bytes_read) -2)) = '\n';
                }
            break;
        }//end of EOL dependent newline check
    }while(c !=(uint32_t) EOF && !eolReached);
    //null terminate the buffer for UTF16
    buff[*bytes_read] =  buff[(*bytes_read) + 1] = '\0';
    //finally check yet again for end of file right after the new line
    bytesN = rfFgetc_UTF16(f, &c, false, endianess, eofReached);
    if(bytesN < 0 && !(*eofReached))
    {
        RF_ERROR("An error was encountered while reading the end of "
                 "a stream of bytes from a UTF-16 file descriptor");
        return false;
    }
    
    if(!(*eofReached))
    {//unless it's EOF undo the peak ahead
        if(rfFseek(f,-bytesN,SEEK_CUR) != 0)
        {
            RF_ERROR("Failed to undo the peek ahead of the file pointer",
                     "due to fseek() failure with errno %d", errno);
            return false;
        }
    }
    return true;
}

//Gets a number of bytes from a UTF-8 file descriptor
bool rfFgets_UTF8(char* buff, uint32_t num, FILE* f,
                   char* eofReached, char eol, uint32_t* bytes_read)
{
    uint32_t c;
    int32_t bytesN;
    bool eolReached;
    //initialization
    *eofReached = eolReached = false;
    *bytes_read = 0;
    //if end of file or end of line is not found, keep reading
    do{
        bytesN = rfFgetc_UTF8(f, (uint32_t*)(buff + (*bytes_read)),
                              false, eofReached);
        if(*eofReached == true)
        {
            break;//EOF found
        }
        //error check
        if(bytesN < 0)
        {
            RF_ERROR("An error was encountered while reading a UTF8 line "
                     "from a file");
            return false;
        }

        *bytes_read = *bytes_read + bytesN;
        //if we have read the number of characters requested by the function
        if(*bytes_read >= num)
        {
            break;
        }

        //newline check depending on the EOL pattern
        c = *(uint32_t*)(buff + (*bytes_read) -bytesN);
        switch(eol)
        {
            case RF_EOL_LF:
                if(c == RF_LF)
                {
                    eolReached = true;
                }
            break;
            case RF_EOL_CRLF:
                if(c == RF_LF)
                {
                    if( buff[*bytes_read-bytesN-1] == RF_CR)
                    {
                        eolReached = true;
                        *bytes_read = *bytes_read-1;
                        buff[*bytes_read - 1] = '\n';
                    }
                }
            break;
            case RF_EOL_CR:
                if(c == RF_CR)
                {
                    eolReached = true;
                    buff[*bytes_read - 1] = '\n';
                }
            break;
        }//end of EOL dependent newline check
    }while(c != (uint32_t) EOF && !eolReached);
    //null terminate the buffer for UTF8
    buff[*bytes_read] = '\0';

    //finally check yet again for end of file right after the new line
    if( RF_HEXEQ_C(fgetc(f),EOF))
    {//check for error
        if(ferror(f) == 0)
        {
            *eofReached = true;
        }
        else
        {
           RF_ERROR("During reading a UTF-8 file there was a "
                    "read error due to fgetc() failing with errno %d");
           return -1;
        }
    }//undo the peek ahead of the file pointer
    else
    {
        if(rfFseek(f,-1,SEEK_CUR) != 0)
        {
            RF_ERROR("Failed to undo the peek ahead of the file pointer due "
                     "to fseek() failure with errno %d", errno);
            return false;
        }
    }
    return true;
}

//Gets a unicode character from a UTF-8 file descriptor
int rfFgetc_UTF8(FILE* f, uint32_t *ret, char cp, char* eof)
{
#define UTF8_FGETC_FAIL()                                               \
    do{                                                                 \
        if(ferror(f) == 0)                                              \
        {                                                               \
            *eof = true;                                                \
            RF_ERROR("While decoding a UTF-8 file byte stream, EOF"     \
                     " was encountered abruplty between bytes");        \
            return -1;                                                  \
        }                                                               \
        else                                                            \
        {                                                               \
            RF_ERROR(                                                   \
                "Failure when reading a character from a UTF8 file due to" \
                "fgetc() failing with errno %d", errno);                \
            return -1;                                                  \
        }                                                               \
    }while(0)

    char c,c2,c3,c4;
    *eof = false;
    if( (c = fgetc(f)) == EOF)
    {
        if(ferror(f) == 0)
        {
            *eof = true;
            return -1;
        }
        RF_ERROR("Failed to read from a UTF8 file due to fgetc() "
                 "failing with errno %d", errno);
        return -1;
    }
     //if the lead bit of the byte is 0 then range is : U+0000 to U+0007F (1 byte)
     if( ((c & 0x80)>>7) == 0 )
     {
        ///success
        if(cp == true)
        {
            *ret = c;
        }
        else
        {
            *ret = 0;
            char* cc = (char*) ret;
            cc[0] = c;
        }
        return 1;
     }
     else//we need more bytes
     {
        // if the leading bits are in the form of 0b110xxxxx then range is:
        // U+0080 to U+07FF (2 bytes)
        if( RF_HEXEQ_C( ( (~(c ^  0xC0))>>5), 0x7) )
        {
            //also remember bytes 0xC0 and 0xC1 are invalid and could possibly
            // be found in a starting byte of this type so check for them here
            if( RF_HEXEQ_C(c,0xC0) || RF_HEXEQ_C(c,0xC1))
            {
                RF_ERROR("While decoding a UTF-8 file byte "
                                 "stream, an invalid byte was "
                                 "encountered");
                return -1;
            }
            //so now read the next byte
            if( (c2 = fgetc(f)) == EOF)
            {
                UTF8_FGETC_FAIL();
            }
            //if this second byte is NOT a continuation byte
            if( !rfUTF8_IsContinuationByte(c2))
            {
                RF_ERROR("While decoding a UTF-8 file byte "
                          "stream, and expecting a continuation "
                          "byte, one was not found");
                return -1;
            }

            ///success
            if(cp == true)//return decoded codepoint
            {
                *ret = 0;
                //from the second byte take the first 6 bits
                *ret = (c2 & 0x3F) ;
                //from the first byte take the first 5 bits and put them in the start
                *ret |= ((c & 0x1F) << 6);
            }
            else
            {
                *ret = 0;
                char* cc = (char*)ret;
                cc[0] = c; cc[1] = c2;
            }
            return 2;
        }//end of the 2 bytes case
        //if the leading bits are in the form of 0b1110xxxx then range is 
        //U+0800 to U+FFFF  (3 bytes)
        else if( RF_HEXEQ_C( ( (~(c ^ 0xE0))>>4),0xF))
        {
            //so now read the next 2 bytes
            if( (c2 = fgetc(f)) == EOF || (c3 = fgetc(f)) == EOF)
            {
                UTF8_FGETC_FAIL();
            }
            //if the subsequent bytes are NOT  continuation bytes
            if( !rfUTF8_IsContinuationByte(c2) ||
                !rfUTF8_IsContinuationByte(c3))
            {
                RF_ERROR(
                         "While decoding a UTF-8 file byte stream, and "
                         "expecting a continuation byte, one was not found");
                return -1;
            }

            ///success
            if(cp == true)//if we need to decode the codepoint
            {
                *ret = 0;
                //from the third byte take the first 6 bits
                *ret = (c3 & 0x3F) ;
                //from the second byte take the first 6 bits and put them to
                // the left of the previous 6 bits
                *ret |= ((c2 & 0x3F) << 6);
                //from the first byte take the first 4 bits and put them to
                // the left of the previous 6 bits
                *ret |= ((c & 0xF) << 12);
            }
            else
            {
                *ret = 0;
                char* cc = (char*)ret;
                cc[0] = c; cc[1] = c2; cc[2] = c3;
            }
            return 3;
        }//end of 3 bytes case
        //if the leading bits are in the form of 0b11110xxx then range is
        // U+010000 to U+10FFFF (4 bytes)
        else if(RF_HEXEQ_C( ( (~(c ^ 0xF0))>>3), 0x1F))
        {
            //in this type of starting byte a number of invalid bytes can be encountered
            if(RF_HEXGE_C(c, 0xF5)) //invalid byte value are from 0xF5 to 0xFF
            {
                RF_ERROR(
                         "While decoding a UTF-8 file byte stream, an invalid"
                         " byte was encountered");
                return -1;
            }

            //so now read the next 3 bytes
            if((c2 = fgetc(f)) == EOF ||
               (c3 = fgetc(f)) == EOF ||
               (c4 = fgetc(f)) == EOF ) 
            {
                UTF8_FGETC_FAIL();
            }

            //if the subsequent bytes are NOT  continuation bytes
            if( !rfUTF8_IsContinuationByte(c2) ||
                !rfUTF8_IsContinuationByte(c3) ||
                !rfUTF8_IsContinuationByte(c4))
            {
                RF_ERROR(
                    0, "While decoding a UTF-8 file byte stream, and "
                    "expecting a continuation byte, one was not found");
                return -1;
            }

            ///success
            if(cp == true) //if we need to decode the codepoint
            {
                *ret = 0;
                //from the fourth byte take the first 6 bits
                *ret = (c4 & 0x3F) ;
                //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
                *ret |= ((c3 & 0x3F) << 6);
                //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
                *ret |= ((c2 & 0x3F) << 12);
                //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
                *ret |= ((c & 0x7) << 18);
            }
            else
            {
                *ret = 0;
                char* cc = (char*)ret;
                cc[0] = c; cc[1] = c2; cc[2] = c3; cc[3]=c4;
            }
            return 4;
        }//end of 4 bytes case
     }//end of needing more than 1 byte

    //if we get here means the 1st byte belonged to none of the 4 cases
    RF_ERROR(
        0, "While decoding a UTF-8 file byte stream, the first byte of a "
        "character was invalid UTF-8");
    return -1;

#undef UTF8_FGETC_FAIL
}

//Gets a unicode character from a UTF-16 file descriptor
int rfFgetc_UTF16(FILE* f, uint32_t *c, char cp, int endianess, char* eof)
{
    uint16_t v1,v2;
    *eof = false;
#if RF_OPTION_DEBUG
    if(endianess != RF_LITTLE_ENDIAN || endianess != RF_BIG_ENDIAN)
    {
        RF_ERROR( "Illegal endianess type given");
        return -1;
    }
#endif
    //read the first 2 bytes
    if(fread(&v1, 2, 1, f) != 1)
    {
        if(ferror(f) == 0)
        {
            *eof = true;
            return -1;
        }
        //else error
        RF_ERROR("Error while reading from a UTF16 byte stream"
                 "due to fread() failing with errno %d", errno);
        return -1;
    }
    rfProcessByteOrderUS(&v1, endianess);

    /*If the value is in the surrogate area*/
    if(RF_HEXGE_US(v1,0xD800) && RF_HEXLE_US(v1,0xDFFF))
    {
        if(RF_HEXL_US(v1,0xD800) || RF_HEXG_US(v1,0xDBFF))
        {
            RF_ERROR("While reading a little endian UTF-16 file stream the "
                "first byte encountered held an illegal value");
            return -1;
        }

        //then we also need to read its surrogate pair
        if(fread(&v2, 2, 1, f) != 1)
        {
            if(ferror(f) == 0)
            {
                RF_ERROR("While decoding a UTF-16 file byte "
                         "stream, EOF was encountered abruplty when expecting"
                         " a surrogate pair");
                return -1;
            }
            //else read error
            RF_ERROR(
                "Error while reading from a UTF-16 byte stream"
                "due to fread() failing with errno %d", errno);
            return -1;
        }
        rfProcessByteOrderUS(&v2, endianess);
        if(RF_HEXL_US(v2, 0xDC00) || RF_HEXG_US(v2, 0xDFFF))
        {
            RF_ERROR(
                     "While reading a little endian UTF-16 file stream the "
                     "surrogate pair encountered held an illegal value");
            return -1;
        }

        if(cp == true)//if the user wants the decoded codepoint
        {
            *c = 0;
            *c = v2&0x3ff;
            *c |= (10<<v1&0x3ff);
            *c += 0x10000;
            return 4;
        }//else
        *c = 0;
        uint16_t* cc = (uint16_t*)c;
        cc[0] = v1; cc[1] = v2;
        return 4;
    }//end of surrogate pair existence case
    //else no surrogate pair exists so v1 is all we need
    *c = v1;
    return 2;
}
//Gets a unicode character from a UTF-32 Little Endian file descriptor
int rfFgetc_UTF32(FILE* f, uint32_t *c, int endianess, char* eof)
{
    *eof = false;
#if RF_OPTION_DEBUG
    if(endianess != RF_LITTLE_ENDIAN || endianess != RF_BIG_ENDIAN)
    {
        RF_ERROR( "Illegal endianess type given");
        return -1;
    }
#endif
    //read the next 4 bytes
    if(fread(c,4,1,f) != 1)
    {
        if(ferror(f) == 0)
        {
            *eof = true;
            return -1;
        }
        //else error
        RF_ERROR("Error while reading from a UTF32  byte"
                 "due to fread() failing with errno %d",errno);
        return -1;
    }
    //check if we need to be swapping
    rfProcessByteOrderUI(c, endianess);
    return 4;
}



//Moves a unicode character backwards in a UTF-32 file stream
int rfFback_UTF32(FILE* f, uint32_t *c, int endianess)
{
#if RF_OPTION_DEBUG
    if(endianess != RF_LITTLE_ENDIAN || endianess != RF_BIG_ENDIAN)
    {
        RF_ERROR( "Illegal endianess type given");
        return -1;
    }
#endif

    //go back and read the last 4 bytes
    if(rfFseek(f, -4, SEEK_CUR) != 0)
    {
        RF_ERROR("Going backwards in a UTF-32 file stream failed due to "
                 "fseek() fail with errno %d", errno);
        return -1;
    }
    if(fread(c, 4, 1, f) != 1)
    {
        if(ferror(f) == 0)
        {
            RF_ERROR("While reading four bytes backwards in a UTF-32 "
                     "byte stream EOF was encountered");
            return -1;
        } 
        RF_ERROR("Reading four bytes backwards in a "
                 "UTF-32 file stream failed due to fread() failing with "
                 "errno %d", errno);
        return -1;
    }
    if(rfFseek(f, -4, SEEK_CUR) != 0)
    {
        RF_ERROR("Going backwards in a UTF-32 file stream failed due to fseek() failing with errno %d", errno);
        return -1;
    }
    //check if we need to be swapping
    rfProcessByteOrderUI(c, endianess);
    return 4;
}


// Moves a unicode character backwards in a UTF-16 file stream
int rfFback_UTF16(FILE* f, uint32_t *c, int endianess)
{
    uint16_t v1,v2;
#if RF_OPTION_DEBUG
    if(endianess != RF_LITTLE_ENDIAN || endianess != RF_BIG_ENDIAN)
    {
        RF_ERROR( "Illegal endianess type given");
        return -1;
    }
#endif
    //go back and read the last 2 bytes
    if(rfFseek(f,-2,SEEK_CUR) != 0)
    {
        RF_ERROR("Going backwards in a UTF-16 file stream failed due "
                 "to fseek() with errno %d", errno);
        return -1;
    }
    if(fread(&v1, 2, 1, f) != 1)
    {
        if(ferror(f) == 0)
        {
            RF_ERROR("While reading two bytes backwards in a UTF-16 "
                     "byte stream EOF was encountered");
            return -1;
        } 
        RF_ERROR("Reading two bytes backwards in a "
                 "UTF-16 file stream failed due to fread() fail with "
                 "errno %d", errno);
        return -1;
    }
    if(rfFseek(f, -2, SEEK_CUR) != 0)
    {
        RF_ERROR("Going backwards in a UTF-16 file stream failed due to "
                 "fseek() with errno %d", errno);
        return -1;
    }
    rfProcessByteOrderUS(&v1, endianess);

    /*If the word is a surrogate pair*/
    if(RF_HEXGE_US(v1,0xDC00) && RF_HEXLE_US(v1,0xDFFF))
    {
        //go back and read 2 more bytes
        if(rfFseek(f, -2, SEEK_CUR) != 0)
        {
            RF_ERROR("Going backwards in a UTF-16 file stream failed due "
                     "to fseek() with errno %d", errno);
            return -1;
        }
        if(fread(&v2, 2, 1, f) != 1)
        {
            if(ferror(f) == 0)
            {
                RF_ERROR("While reading two bytes backwards in a UTF-16 "
                         "byte stream EOF was encountered");
                return -1;
            } 
            RF_ERROR("Reading two bytes backwards in a "
                     "UTF-16 file stream failed due to fread() failure "
                     "with errno %d", errno);
            return -1;
        }
        if(rfFseek(f, -2, SEEK_CUR) != 0)
        {
            RF_ERROR("Going backwards in a UTF-16 file stream failed "
                     "due to fseek() with errno %d", errno);
            return -1;
        }
        rfProcessByteOrderUS(&v2, endianess);

        if(RF_HEXL_US(v2,0xD800) || RF_HEXG_US(v2,0xDBFF))
        {
            RF_ERROR(
                     "While reading bytes backwards in a UTF-16"
                     " file stream the encountered byte was supposed to be a"
                     " surrogate pair but its pair is of illegal value");
            return -1;
        }
        //get the codepoint
        *c = 0;
        *c = v1&0x3ff;
        *c |= (10<<v2&0x3ff);
        *c += 0x10000;
        return 4;
    }//end of surrogate pair case
    //getting here means this word is what we seek. Let's confirm
    if(RF_HEXL_US(v1,0xD800) || RF_HEXG_US(v1,0xDFFF))
    {
        //get the codepoint
         *c = 0;
        uint16_t* cc = (uint16_t*) c;
        cc[0] = v1;
        return 2;
    }
    //else invald sequence
    RF_ERROR(
             "While reading bytes backwards in a UTF-16 file "
             "stream the encountered byte had an illegal value");
    return -1;
}

//Moves a unicode character backwards in a UTF-8 file stream
int rfFback_UTF8(FILE* f, uint32_t *c)
{
    //read one byte before the current
    int i = 0;
    char bytes[4];
    do
    {
        if(rfFseek(f, -1, SEEK_CUR) != 0)
        {
            RF_ERROR("Going backwards in a UTF-8 file failed due to "
                     "fseek() with errno %d", errno);
            return -1;
        }
        if((bytes[i] = fgetc(f)) == EOF)
        {
            if(ferror(f) == 0)
            {
                RF_ERROR("The EOF was encountered going backwards in "
                         "a UTF-8 file. Confused");
                return -1;
            }
            RF_ERROR("Reading a byte backwards in a UTF-8 file failed due "
                     "to fgetc() returning errno %d", errno);
            return -1;
        }
        if(rfFseek(f, -1, SEEK_CUR) != 0)
        {
            RF_ERROR("Going backwards in a UTF-8 file failed due to fseek() "
                     "with errno %d", errno);
            return -1;
        }
        i++;
    }while(rfUTF8_IsContinuationByte(bytes[i-1]));

    switch(i)//depending on the number of bytes read backwards
    {
        case 4:
            *c = 0;
            //from the fourth byte take the first 6 bits
            *c = (bytes[0] & 0x3F) ;
            //from the third byte take the first 6 bits and put them to the left of the previous 6 bits
            *c |= ((bytes[1] & 0x3F) << 6);
            //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
            *c |= ((bytes[2] & 0x3F) << 12);
            //from the first byte take the first 3 bits and put them to the left of the previous 6 bits
            *c |= ((bytes[3] & 0x7) << 18);
        break;
        case 3:
            *c = 0;
            //from the third byte take the first 6 bits
            *c = (bytes[0] & 0x3F) ;
            //from the second byte take the first 6 bits and put them to the left of the previous 6 bits
            *c |= ((bytes[1] & 0x3F) << 6);
            //from the first byte take the first 4 bits and put them to the left of the previous 6 bits
            *c |= ((bytes[2] & 0xF) << 12);
        break;
        case 2:
            *c = 0;
            //from the second byte take the first 6 bits
            *c = (bytes[0] & 0x3F) ;
            //from the first byte take the first 5 bits and put them in the start
            *c |= ((bytes[1] & 0x1F) << 6);
        break;
        case 1:
            *c = bytes[0];
        break;
        default:
            RF_ERROR("During moving one unicode character back in a UTF-8 "
                     "filestream moved an abnormal number of bytes");
            return -1;
        break;
    }
    return i;
}


//for creation of external symbol
i_INLINE_INS int rfStat(RF_String* f, stat_rft* buffer);
