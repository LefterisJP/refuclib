/**
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
**/
//*---------------------Corrensponding Header inclusion---------------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h>  //for import export macro
#include <System/rf_system.h>
//*---------------------Outside module inclusion----------------------------------------
#include <String/string_decl.h> //for RF_String
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/defarg.h> //for the default argument declared string functions that are included below
#include <String/common.h> //for RFS_()
#include <String/corex.h> //for rfStringX_Init_buff() and others
//for error logging
    #include <stdio.h>//for FILE* used inside printf.h
    #include <IO/printf.h> //for rfFpintf() used in the error logging macros
    #include <Utils/error.h>
//for memory allocation
    #include <stdlib.h> //for malloc, calloc,realloc and exit()
    #include <Definitions/retcodes.h> //for error codes, logged in allocation failure
    #include <Utils/memory.h> //for refu memory allocation
//for local scope macros
    #include <Definitions/threadspecific.h> //for the thread specific attribute
    #include <Utils/localmem_decl.h> // for RF_LocalMemoryStack
    #include <string.h> //for memset()
    #include <limits.h> //for ULONG_MAX used in RF_ENTER_LOCAL_SCOPE() macro
    #include <Utils/localscope.h>
//*---------------------System Specific Headers inclusion-------------------------------
#include <windows.h>
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h>
#include <direct.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
//*----------------------------End of Includes------------------------------------------

//Creates a directory
int32_t rfMakeDir(void* dirnameP,int mode)
{
    RF_String* dirname = (RF_String*)dirnameP;
    int32_t error = RF_SUCCESS;
    RF_ENTER_LOCAL_SCOPE()

    //make the directory
    if(_mkdir(dirname->bytes)!=0)
    {
        //if we got an error
        switch(errno)
        {
            case EEXIST:
                error = RE_DIRECTORY_EXISTS;
            break;
            case ENOENT:
                LOG_ERROR("Failed to create directory \"%S\" because the given path was not found",RE_DIRECTORY_INVALID,dirname)
                error = RE_DIRECTORY_INVALID;
            break;
        }
    }

    RF_EXIT_LOCAL_SCOPE()
    return error;
}

//Removes a directory and all its files
int32_t rfRemoveDir(void* dirnameP)
{
    RF_String* dirname = (RF_String*)dirnameP;
    DIR* dir;
    struct dirent *entry;
    RF_StringX path;
    struct stat s;
    int32_t ret = RF_SUCCESS;
    RF_ENTER_LOCAL_SCOPE()

    rfStringX_Init_buff(&path,1024,"");
    //open the directory
    dir = opendir(dirname->bytes);
    if(dir == NULL)
    {
        switch(errno)
        {
            case EACCES:
                LOG_ERROR("Failed to access directory \"%S\" because of insufficient permissions",RE_PERMISSION,dirname)
            break;
            case ENAMETOOLONG:
                LOG_ERROR("Can't access directory \"%S\" because its name is too long for the filesystem",RE_DIRECTORY_NAMELENGTH,dirname)
            break;
            case ENOENT:
            case ENOTDIR:
                LOG_ERROR("Failed to access directory \"%S\" because the given path was not found or is not a directory",RE_DIRECTORY_INVALID,dirname)
            break;
            case ENFILE:
            case EMFILE:
                LOG_ERROR("Failed to access directory \"%S\" because too many files are currently open in the system",RE_DIRECTORY_TOOMANY_FILES,dirname)
            break;
            default:
                LOG_ERROR("Failed to access directory \"%S\" because of opendir() failure",RE_DIRECTORY_OPEN,dirname)
            break;
        }
        ret = RF_LastError;
        goto cleanup1;
    }
    //keep the previous errno for  comparison
    int prErrno = errno;
    while((entry = readdir(dir)) != NULL)
    {
        //skip this and the parent dir
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            //create the full entry name
            rfStringX_Assign(&path,RFS_("%S"RF_DIRSEP"%s",dirname,entry->d_name));
            //if this directory entry is a directory itself, call the function recursively
            stat(path.INH_String.bytes, &s);
            if (s.st_mode & S_IFDIR)
            {
                if( (ret = rfRemoveDir(&path)) != RF_SUCCESS)
                    goto cleanup1;
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }
            //if we get here this means it's a file that needs deletion
            if(DeleteFile(path.INH_String.bytes)==0)
            {
                RF_WIN32_GETSYSERROR(strBuff)
                LOG_ERROR("Recursive directory deletion of directory \"%S\" failed because of inability to delete file \"%S\". Windows error (%lu) was given:\n%s",
                          RE_FILE_DELETE,dirname,&path,i_ERROR_CODE,strBuff)
                LocalFree(strBuff);
                ret = RE_FILE_DELETE;
                goto cleanup1;
            }
        }

    }
    closedir(dir);
    //free the path string
    rfStringX_Deinit(&path);
    //check if we finished iterating succesfully
    if(errno != prErrno)
    {
        switch(errno)
        {
            case EBADF:
                LOG_ERROR("During iteration of directory \"%S\" the directory value was invalid",RE_DIRECTORY_INVALID,dirname)
            break;
            case ENOENT:
                LOG_ERROR("During iteration of directory \"%S\" the current position of the directory stream was invalid",RE_DIRECTORY_STREAMPOS,dirname)
            break;
            default:
                LOG_ERROR("During iteration of directory \"%S\" reading the directory failed",RE_DIRECTORY_READ,dirname)
            break;
        }
        ret = RF_LastError;
        goto cleanup1;
    }
    //finally delete the directory itself
    if(RemoveDirectory(dirname->bytes)==0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Recursive directory deletion of directory \"%S\" failed because of inability to delete the directory itself. Windows error (%lu) was given:\n%s",
                  RE_DIRECTORY_DELETE,dirname,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        ret = RE_DIRECTORY_DELETE;//no need to jump with goto here since it's right after
    }

cleanup1:
    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

//Deletes a file
int32_t rfDeleteFile(void* nameP)
{
    RF_String* name = (RF_String*)nameP;
    int32_t ret = RF_SUCCESS;
    RF_ENTER_LOCAL_SCOPE()

    if(DeleteFile(name->bytes)==0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Failed to delete file \"%S\" because of Windows error(%lu):\n%s",RE_FILE_DELETE,name,i_ERROR_CODE,strBuff)
        LocalFree(strBuff);
        ret = RE_FILE_DELETE;
    }

    RF_EXIT_LOCAL_SCOPE()
    return ret;
}
// Renames a file
int32_t rfRenameFile(void* nameP,void* newNameP)
{
    RF_String* name = (RF_String*)nameP;
    RF_String* newName = (RF_String*)newNameP;
    int32_t ret = RF_SUCCESS;
    RF_ENTER_LOCAL_SCOPE()

    if(MoveFile(name->bytes,newName->bytes) ==0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Failed to rename file \"%S\" to \"%S\" because of Windows error(%lu):\n:%s",RE_FILE_RENAME,name,newName,i_ERROR_CODE,strBuff)
        LocalFree(strBuff);
        ret = RE_FILE_RENAME;
    }

    RF_EXIT_LOCAL_SCOPE()
    return ret;
}

