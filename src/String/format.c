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
#include <stdarg.h>
#include <Definitions/types.h> //for fixed size types needed in various places
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include <Definitions/imex.h> //for the import export macro
#include <String/format.h>
/*------------- Module related inclusion -------------*/
#include <String/flags.h> //for string macro flags
#include <String/common.h> // for RFS()
#include <String/manipulationx.h> //for rfStringX_Append family of functions
#include "common.ph"//include the string private functions and macros
/*------------- Outside Module inclusion -------------*/
//for the error logging macros
    #include <stdio.h>//for FILE* used inside printf.h
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Threads/common.h> //for rfThread_GetID()
    #include <String/string_decl.h> //for RF_String
    #include <String/common.h> //for RFS_() macro
    #include <Utils/error.h>

#include <Definitions/retcodes.h> //for the return codes
//for the local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>
/*------------- End of includes -------------*/

//the specifiers that can appear in a format string
enum  legalCharacters
{
    SPECIFIER_DECIMAL_INT=1,
    SPECIFIER_DECIMAL_UINT,
    SPECIFIER_UOCTAL_INT,
    SPECIFIER_HEX_INT,
    SPECIFIER_FLOAT,
    SPECIFIER_SCIENTIFIC,
    SPECIFIER_FLOAT_SHORTEST,
    SPECIFIER_HEXFLOAT,
    SPECIFIER_CHAR,
    SPECIFIER_CSTRING,
    SPECIFIER_RFSTRING,//Refu String
    SPECIFIER_POINTER,
    SPECIFIER_NOTHING,//stores the number of characters written so far
    SPECIFIER_PERCENT,
    LENGTH_INT,
    LENGTH_CHAR,
    LENGTH_SHORT,
    LENGTH_LONG,
    LENGTH_LONGLONG,
    LENGTH_INTMAX,
    LENGTH_SIZET,
    LENGTH_PTRDIFF,
    LENGTH_LONGDOUBLE
};



//The different states that the format reader can be in
#define EXPECT_FLAG         5
#define EXPECT_WIDTH        4
#define EXPECT_PRECISION    3
#define EXPECT_LENGTH       2
#define EXPECT_SPECIFIER    1
#define END_FORMAT          0

//Width specific
#define WIDTH_NONE      -2
#define WIDTH_ARGUMENT  -1

//Precision specific
#define PRECISION_NONE          INT_MAX
#define PRECISION_ARGUMENT      -1

#define FLOAT_DEFAULT_PRECISION 6

typedef struct Argument_Format
{
    char flags;
    int width;
    int precision;
    char length;
    char specifier;
}Argument_Format;

/**
 ** Parses a Single argument format specifier
 **
 ** @param s A pointer to the format string at the position right after the '%'
 ** @param data A pointer to Argument_Format data type to populate
 ** @return The number of characters used after the '%' to specify the argument's format or 0 for failure
 **
 **/
static int Parse_FormatSpecifier(const char* s, Argument_Format* data)
{
    int i;//the index to the format string after the '%' sign
    char state = EXPECT_FLAG;

    //initialize the Argument Format
    data->flags = 0;
    data->width = WIDTH_NONE;
    data->precision = PRECISION_NONE;
    data->length = LENGTH_INT;


    i=0;
    do
    {
    switch(s[i])
    {
        /* -- Flags Control Characters -- */
        case '-':
            if(state == EXPECT_FLAG)
            {
                data->flags |= RF_FORMATFLAG_LJUSTIFY;
            }
            else
            {
                RF_ERROR("During parsing a format string flag '-' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case '+':
            if(state == EXPECT_FLAG)
            {
                data->flags |= RF_FORMATFLAG_SIGN;
            }
            else
            {
                RF_ERROR("During parsing a format string flag '+' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case ' ':
            if(state == EXPECT_FLAG)
            {

                data->flags |= RF_FORMATFLAG_NOSIGN;
            }
            else
            {
                RF_ERROR("During parsing a format string flag ' ' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case '#':
            if(state == EXPECT_FLAG)
            {
                data->flags |= RF_FORMATFLAG_SHARP;
            }
            else
            {
                RF_ERROR("During parsing a format string flag '#' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case '0':
            if(state == EXPECT_FLAG)
            {
                data->flags |= RF_FORMATFLAG_PADZEROES;
            }
            else
            {
                RF_ERROR("During parsing a format string flag '0' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;

        /* -- Width Control Characters -- */
        case '*':
            if(state >=EXPECT_WIDTH)
            {
                state = EXPECT_PRECISION;
                data->width = WIDTH_ARGUMENT;
            }
            else
            {
                RF_ERROR("During parsing a format string, width flag '*' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;

        /* -- Precision Control Characters -- */
        case '.':
            if(state >= EXPECT_PRECISION)
            {
                i++;
                if(s[i] == '*')
                {
                    state = EXPECT_LENGTH;
                    data->precision = PRECISION_ARGUMENT;
                }
                else if(s[i] >= 49 && s[i] <=57)//else if it's a number 
                {                              //(not starting with zero)
                    char j = 0;
                    data->precision = 0;
                    //read the number
                    do
                    {
                        data->precision = data->precision*(10*j)+(s[i+j]-48);
                        j++;
                    }while(s[i+j] >= 48 && s[i+j] <=57);
                    i+=j-1;
                    state = EXPECT_LENGTH;
                }
                else
                {
                    RF_ERROR("During parsing a format string, after the "
                             "precision character '.' a non-numeric non '*' "
                             "character was encountered");
                    state = END_FORMAT;
                    i= -1;
                }
            }
            else
            {
                RF_ERROR("During parsing a format string, precision flag '.' "
                         "was encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;

        //number (should only exist for width)
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if(state >= EXPECT_WIDTH)
            {
                char j = 0;
                data->width = 0;
                //read the number
                do
                {
                    data->width = data->width*(10*j)+(s[i+j]-48);
                    j++;
                }while(s[i+j] >= 48 && s[i+j] <=57);
                i+=j-1;
                state = EXPECT_PRECISION;
            }
            else
            {
                RF_ERROR("During parsing a format string, a number was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;

        /* -- Length Control Characters -- */
        case 'h':
            if(state >= EXPECT_LENGTH)
            {
                state = EXPECT_SPECIFIER;
                data->length = LENGTH_SHORT;
                if(s[i+1] == 'h')
                {
                    i++;
                    data->length = LENGTH_CHAR;
                }
            }
            else
            {
                RF_ERROR("During parsing a format string, length flag 'h' "
                         "was encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case 'l':
            if(state >= EXPECT_LENGTH)
            {
                state = EXPECT_SPECIFIER;
                data->length = LENGTH_LONG;
                if(s[i+1] == 'l')
                {
                    i++;
                    data->length = LENGTH_LONGLONG;
                }
            }
            else
            {
                RF_ERROR("During parsing a format string, length flag 'l' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case 'j':
            if(state >= EXPECT_LENGTH)
            {
                state = EXPECT_SPECIFIER;
                data->length = LENGTH_INTMAX;
            }
            else
            {
                RF_ERROR("During parsing a format string, length flag 'j' "
                         "was encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case 'z':
            if(state >= EXPECT_LENGTH)
            {
                state = EXPECT_SPECIFIER;
                data->length = LENGTH_SIZET;
            }
            else
            {
                RF_ERROR("During parsing a format string, length flag 'z' was "
                         "encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case 't':
            if(state >= EXPECT_LENGTH)
            {
                state = EXPECT_SPECIFIER;
                data->length =  LENGTH_PTRDIFF;
            }
            else
            {
                RF_ERROR("During parsing a format string, length flag 't' "
                         "was encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;
        case 'L':
            if(state >= EXPECT_LENGTH)
            {
                state = EXPECT_SPECIFIER;
                data->length = LENGTH_LONGDOUBLE;
            }
            else
            {
                RF_ERROR("During parsing a format string, length flag 'L' "
                         "was encountered at an unexpected location");
                state = END_FORMAT;
                i= -1;
            }
        break;

        /* -- Specifier Control Characters -- */
        //no state checks from here and down since there are no subsequent states
        case 'd':
        case 'i':
            state = END_FORMAT;
            data->specifier = SPECIFIER_DECIMAL_INT;
        break;
        case 'u':
            state = END_FORMAT;
            data->specifier = SPECIFIER_DECIMAL_UINT;
        break;
        case 'o':
            state = END_FORMAT;
            data->specifier = SPECIFIER_UOCTAL_INT;
        break;
        case 'X':
            data->flags |= RF_FORMATFLAG_UPPER;
        case 'x':
            state = END_FORMAT;
            data->specifier = SPECIFIER_HEX_INT;
        break;
        case 'F':
            data->flags |= RF_FORMATFLAG_UPPER;
        case 'f':
            state = END_FORMAT;
            data->specifier = SPECIFIER_FLOAT;
        break;
        case 'E':
            data->flags |= RF_FORMATFLAG_UPPER;
        case 'e':
            state = END_FORMAT;
            data->specifier = SPECIFIER_SCIENTIFIC;
        break;
        case 'G':
            data->flags |= RF_FORMATFLAG_UPPER;
        case 'g':
            state = END_FORMAT;
            data->specifier = SPECIFIER_FLOAT_SHORTEST;
        break;
        case 'A':
            data->flags |= RF_FORMATFLAG_UPPER;
        case 'a':
            state = END_FORMAT;
            data->specifier = SPECIFIER_HEXFLOAT;
        break;
        case 'c':
            state = END_FORMAT;
            data->specifier = SPECIFIER_CHAR;
        break;
        case 's':
            state = END_FORMAT;
            data->specifier = SPECIFIER_CSTRING;
        break;
        case 'S': // Refu lib's extra specifier for RF_String
            state = END_FORMAT;
            data->specifier = SPECIFIER_RFSTRING;
        break;
        case 'p':
            state = END_FORMAT;
            data->specifier = SPECIFIER_POINTER;
        break;
        case 'n':
            state = END_FORMAT;
            data->specifier = SPECIFIER_NOTHING;
        break;
        case '%':
            state = END_FORMAT;
            data->specifier = SPECIFIER_PERCENT;
        break;
        default:
            state = END_FORMAT;
            RF_ERROR("During parsing a format string illegal control "
                     "character '%c' was encountered",
                     RE_FORMAT_ILLEGALCHAR,s[i]);
            i= -1;
        break;
    }//end of the huge switch
    i++;

    }while(state != END_FORMAT);

    return i;
}

//Formats an @ref RF_StringX according to the given format string and variable argument list
char rfStringX_Formatv(RF_StringX *ret, const char* format, va_list args)
{
    int i,extraChars;
    char rc = true;
    uint32_t formatLength,lastI;//format bytelength
    Argument_Format argData;

    //reset the stringX to an empty string, but keeping its buffer size
    ret->bIndex=0;
    ret->INH_String.bytes[0] = '\0';
    ret->INH_String.byteLength=0;


    RF_ENTER_LOCAL_SCOPE();
    lastI=0;

    formatLength = strlen(format);
    //begin reading the extra args


    //iterate the bytes of the format c string
    for(i=0; i < formatLength; i++)
    {
        if(format[i]=='%')
        {
            // append the part of the format string until before the '%'
            // to the return string
            if(!rfStringX_Append_bytes(ret, RFS_(format+lastI), i-lastI))
            {
                rc = false;
                goto cleanup1;
            }
            //read the format specifier to know what you have to read in now
            if( (extraChars = Parse_FormatSpecifier(format+i+1,
                                                    &argData)) == 0)
            {
                RF_ERROR("Error at parsing an argument's format specifier");
                rc = false;
                goto cleanup1;
            }
            //remember where to resume from the format string
            i += extraChars;
            lastI = i + 1;

            //check to see if we have an extra width argument
            if(argData.width == WIDTH_ARGUMENT)
            {
                argData.width = va_arg(args, int);
            }
            //check to see if we have an extra precision argument
            if(argData.precision == PRECISION_ARGUMENT)
            {
                argData.precision = va_arg(args, int);
            }

            //now depending on the specifier read the argument
            switch(argData.specifier)
            {
                /* %d or %i */
                case SPECIFIER_DECIMAL_INT:
                    switch(argData.length)
                    {
                        case LENGTH_CHAR:
                        case LENGTH_SHORT:
                        case LENGTH_INT:
                            if(!rfStringX_Append_int(
                                   ret,
                                   va_arg(args,int),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending an int to to a "
                                         "string during formatting");
                                goto cleanup1;
                            }
                        break;
                        case LENGTH_LONG:
                            if(!rfStringX_Append_int(
                                   ret,
                                   va_arg(args,long),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending an long int to "
                                         "a string during formatting");
                                goto cleanup1;
                            }
                        break;
                        case LENGTH_LONGLONG:
                            if(!rfStringX_Append_int(
                                   ret,
                                   va_arg(args,long long),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending a long long "
                                         "int to a string during formatting");
                                goto cleanup1;
                            }
                        break;

                    }
                break;//end of Decimal int

                /* %u */
                case SPECIFIER_DECIMAL_UINT:
                    switch(argData.length)
                    {
                        case LENGTH_CHAR:
                        case LENGTH_SHORT:
                        case LENGTH_INT:
                            if(!rfStringX_Append_uint(
                                   ret,
                                   va_arg(args,unsigned int),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending an unsigned "
                                         "int to a string during formatting");
                                goto cleanup1;
                            }
                        break;
                        case LENGTH_LONG:
                            if(!rfStringX_Append_uint(
                                   ret,
                                   va_arg(args,unsigned long),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending an unsigned "
                                         "long int to a string during formatting");
                                goto cleanup1;
                            }
                        break;
                        case LENGTH_LONGLONG:
                            if(!rfStringX_Append_uint(
                                   ret,
                                   va_arg(args,unsigned long long),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending an unsigned long "
                                         "long int to a string during formatting");
                                goto cleanup1;
                            }
                        break;
                    }
                break;//end of Decimal unsigned int
                /* %x or %X */
                case SPECIFIER_HEX_INT:
                    switch(argData.length)
                    {
                        case LENGTH_CHAR:
                        case LENGTH_SHORT:
                        case LENGTH_INT:
                            if(!rfStringX_Append_hex(
                                   ret,
                                   va_arg(args,unsigned int),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending a hex "
                                         "int to a string during formatting");
                                goto cleanup1;
                            }
                        break;
                        case LENGTH_LONG:
                            if(!rfStringX_Append_hex(
                                   ret,
                                   va_arg(args,unsigned long),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending a long hex "
                                         "int to a string during formatting");
                                goto cleanup1;
                            }
                        break;
                        case LENGTH_LONGLONG:
                            if(!rfStringX_Append_hex(
                                   ret,
                                   va_arg(args,unsigned long long),
                                   argData.width,
                                   argData.precision,
                                   argData.flags))
                            {
                                rc = false;
                                RF_ERROR("Failure at appending a long long "
                                         "hex int to a string during formatting");
                                goto cleanup1;
                            }
                        break;
                    }
                break;//end of Hexadecimal int

                /* %s */
                case SPECIFIER_CSTRING:
                    if(argData.precision == PRECISION_NONE)
                    {
                        if(!rfStringX_Append(
                               ret,
                               RFS_(va_arg(args,char*))))
                        {
                                rc = false;
                                RF_ERROR("Failure at appending a c string "
                                         "to a string during formatting");
                            goto cleanup1;
                        }
                    }
                    else
                    {
                        if(!rfStringX_Append_i(
                               ret,
                               RFS_(va_arg(args,char*)),
                               argData.precision))
                        {
                                rc = false;
                                RF_ERROR("Failure at appending a c string "
                                         "to a string during formatting");
                            goto cleanup1;
                        }
                    }
                break;
                /* %S */
                case SPECIFIER_RFSTRING:
                    if(argData.precision == PRECISION_NONE)
                    {
                        if(!rfStringX_Append(
                               ret,
                               va_arg(args,RF_String*)))
                        {
                            rc = false;
                            RF_ERROR("Failure at appending an RF_String "
                                     "to a string during formatting");
                            goto cleanup1;
                        }
                    }
                    else
                    {
                        if(!rfStringX_Append_i(
                               ret,
                               va_arg(args,RF_String*),
                               argData.precision))
                        {
                            rc = false;
                            RF_ERROR("Failure at appending an RF_String "
                                     "to a string during formatting");
                            goto cleanup1;
                        }
                    }
                break;

                /* %% */
                case SPECIFIER_PERCENT:
                    if(!rfStringX_Append_charutf8(ret,'%'))
                    {
                        rc = false;
                        RF_ERROR("Failure at appending '%' "
                                 "to a string during formatting");
                        goto cleanup1;
                    }
                break;

                /* %c */
                case SPECIFIER_CHAR:
                    if(!rfStringX_Append_charutf8(
                           ret,
                           va_arg(args,uint32_t)))
                    {
                        rc = false;
                        RF_ERROR("Failure at appending '%' "
                                 "to a string during formatting");
                        goto cleanup1;
                    }
                break;

                /* %f or %F */
                case SPECIFIER_FLOAT:
                    if(argData.precision == PRECISION_NONE)
                    {
                        argData.precision = FLOAT_DEFAULT_PRECISION;
                    }

                    if(!rfStringX_Append_double(
                           ret,
                           va_arg(args,double),
                           argData.width,
                           argData.precision,
                           argData.flags))
                    {
                        rc = false;
                        RF_ERROR("Failure at appending a float"
                                 "to a string during formatting");
                        goto cleanup1;
                    }
                break;
                /* %e or %E */
                case SPECIFIER_SCIENTIFIC:
                    if(argData.precision == PRECISION_NONE)
                    {
                        argData.precision = FLOAT_DEFAULT_PRECISION;
                    }
                    if(!rfStringX_Append_double_scientific(
                           ret,
                           va_arg(args,double),
                           argData.width,
                           argData.precision,
                           argData.flags))
                    {
                        rc = false;
                        RF_ERROR("Failure at appending a scientific float"
                                 "to a string during formatting");
                        goto cleanup1;
                    }
                break;


            }
        }//end of the case where a '%' is found
    }//end of parsing the format string

    //append the last bytes until the end the end
    if(!rfStringX_Append_bytes(ret,RFS_(format+lastI),i-lastI))
    {
        rc = false;
        RF_ERROR("Failure at appending the last bytes of the format string");
        //no going to cleanup1 since it's the end, if that changes
        //add a goto here
    }


cleanup1:

    RF_EXIT_LOCAL_SCOPE();
    return rc;
}



//Formats an @ref RF_StringX according to the given format string and
//variable arguments
char rfStringX_Format(RF_StringX *s, const char* format, ...)
{
    va_list args;//note that no local scope macros are needed here since the arguments get evaluated in rfStringX_Formatv()
    char ret;

    va_start(args,format);
    ret=rfStringX_Formatv(s,format,args);
    va_end(args);

    return ret;
}
