//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/threadspecific.h> //for the thread specific macro
#include <Definitions/types.h> //for fixed size data types appearing in RF_String and RF_StringX
#include <String/string_decl.h>//for RF_String
#include <String/stringx_decl.h>//for RF_StringX
#include "buff.ph"
//*---------------------Outside module inclusion----------------------------------------
#include <Definitions/imex.h> //for the import export macro
#include <Definitions/defarg.h> //to enable default arguments for the corex functions
#include <String/corex.h> //for rfStringX_Init_buff()

#include <Definitions/retcodes.h>
//error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
    #include <Utils/error.h>



//the String to act as the ioBuffer for all the printf family of functions
#define STDIO_BUFF_SIZE 4096
i_THREAD__ RF_StringX ioBuffer;

//Initializes the stdio buffer that is needed by all Refu stdio functions
char rfInitStdio()
{
    if(!rfStringX_Init_buff(&ioBuffer,STDIO_BUFF_SIZE,""))
    {
        LOG_ERROR("Failed to initialize the refu stdio buffer",RE_STDIO_INIT)
        return false;
    }
    return true;
}
