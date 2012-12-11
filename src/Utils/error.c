//*---------------------Corrensponding Header inclusion---------------------------------
#include <stdio.h>//for FILE* used inside printf.h
#include <Definitions/imex.h> //for import export macro
#include <Definitions/types.h>
#include <IO/printf.h> //for rfFpintf() used in the error logging macros
#include <Definitions/defarg.h> //since LOG_ERROR macros use argument counting
#include <Utils/error.h>
//*----------------------------End of Includes------------------------------------------


//definitions
int32_t  RF_LastError;
FILE* rfStdLog;
FILE* rfStdErr;


#ifdef RF_NO_C99 // if the library is compiled with no C99 features on
///In this case the logs differ quite a lot since the function name and file and line can't be logged. So in MSVC's case this is simply reduced to error message logging
void LOG_ERROR(const char* msg,int32_t errorCode,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
    //remember the set error
    RF_LastError = errorCode;
}
void DLOG_ERROR(const char* msg,int32_t errorCode,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
    //remember the set error
    RF_LastError = errorCode;
}
void LOG_INFO(const char* msg,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
}
void DLOG_INFO(const char* msg,...)
{
    //get the variable arguments list and print the given formatted message
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(rfStdErr, msg, argptr);
    va_end(argptr);
    //close with a newline
    fprintf(rfStdErr,"\n");
    fflush(rfStdErr);
}
#endif
