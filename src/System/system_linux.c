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
#include <unistd.h> //for rmdir()
//*---------------------libc Headers inclusion------------------------------------------
#include <errno.h>
#include <dirent.h>
//*----------------------------End of Includes------------------------------------------


//Creates a directory
int32_t rfMakeDir(void* dirnameP,int mode)
{
    RF_String* dirname = (RF_String*)dirnameP;
    int32_t error = RF_SUCCESS;
    RF_ENTER_LOCAL_SCOPE()

    //make the directory
    if(mkdir(dirname->bytes,mode)!=0)
    {
        //if we fail find out why
        switch(errno)
        {
            case EEXIST:
                error = RE_DIRECTORY_EXISTS;
            break;
            case EACCES:
                LOG_ERROR("Failed to create directory \"%S\" because of insufficient permissions",RE_PERMISSION,dirname)
                error = RE_PERMISSION;
            break;
            case EMLINK:
                LOG_ERROR("Failed to create directory \"%S\" because the parent directory has too many entries",RE_DIRECTORY_PARENT_LINKS,dirname)
                error = RE_DIRECTORY_PARENT_LINKS;
            break;
            case ENOENT:
                LOG_ERROR("Failed to create directory \"%S\" because the given path was not found",RE_DIRECTORY_INVALID,dirname)
                error = RE_DIRECTORY_INVALID;
            break;
            case ENOSPC:
                LOG_ERROR("Failed to create directory \"%S\" because there is not enough space in the parent",RE_DIRECTORY_SPACE,dirname)
                error = RE_DIRECTORY_SPACE;
            break;
            case EROFS:
                LOG_ERROR("Failed to create directory \"%S\" because the parent directory is read only",RE_DIRECTORY_ROPARENT,dirname)
                error = RE_DIRECTORY_ROPARENT;
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
    int32_t ret = RF_SUCCESS;
    DIR* dir;
    struct dirent *entry;
    RF_StringX path;
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
            case ELOOP:
                LOG_ERROR("Too many symbolic links were encountered in resolving directory \"%S\"",RE_DIRECTORY_PARENT_LINKS,dirname)
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
            if (entry->d_type == DT_DIR)
            {
                if( (ret = rfRemoveDir(&path)) != RF_SUCCESS)
                    goto cleanup1;
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }
            //if we get here this means it's a file that needs deletion
            if(remove(path.INH_String.bytes)!=0)
            {
                switch(errno)
                {
                    case EACCES:
                    case EPERM:
                        LOG_ERROR("When attempting to delete file \"%S\" permission is denied on a component of the \
        path prefix, or write permission is denied on the parent directory of the file to be removed",RE_PERMISSION,&path)
                    break;
                    case EBUSY:
                        LOG_ERROR("When attempting to delete file \"%S\" permission is denied because it is currently\
        used by some other proccess or by the system",RE_FILE_BUSY,&path)
                    break;
                    case EINVAL:
                    case ENOENT:
                    case ENOTDIR:
                        LOG_ERROR("When attempting to delete file \"%S\" permission is denied because the path argument is invalid or it is not a file",RE_FILE_NOTFILE,&path)
                    break;
                    case EIO:
                        LOG_ERROR("When attempting to delete file \"%S\" a physical I/O error has occured",RE_FILE_IO,&path)
                    break;
                    case ENAMETOOLONG:
                        LOG_ERROR("When attempting to delete file \"%S\" the given name is too long",RE_FILE_NAMELENGTH,&path)
                    break;
                    case EROFS:
                        LOG_ERROR("When attempting to delete file \"%S\" the file resides in a read only file system",RE_FILE_RO,&path)
                    break;
                    default:
                        LOG_ERROR("Failed to delete file \"%S\"",RE_FILE_DELETE,&path)
                    break;
                }
                ret = RF_LastError;
                goto cleanup1;
            }//end of check of succesful file removal
        }//end of the .. and . check
    }//end of directory entries iteration
    closedir(dir);
    //free the path string
    rfStringX_Deinit(&path);
    //check if we finished iterating succesfully
    if(errno != prErrno)
    {
        switch(errno)
        {
            case EOVERFLOW:
                LOG_ERROR("During iteration of directory \"%S\" one of the values of the directory's file parameters can't be represented correctly",RE_DIRECTORY_FILES,dirname)
            break;
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
    if(rmdir(dirname->bytes)!=0)
    {
        switch(errno)
        {
            case EACCES:
            case EPERM:
                LOG_ERROR("When attempting to delete directory \"%S\" permission is denied on a component of the \
path prefix, or write permission is denied on the parent directory of the directory to be removed",RE_PERMISSION,dirname)
            break;
            case EBUSY:
                LOG_ERROR("When attempting to delete directory \"%S\" permission is denied because the directory is currently\
used by some other proccess or by the system",RE_PERMISSION,dirname)
            break;
            case EINVAL:
            case ENOENT:
            case ENOTDIR:
                LOG_ERROR("When attempting to delete directory \"%S\" permission is denied because the path argument is invalid or not a directory",RE_DIRECTORY_INVALID,dirname);
            break;
            case EIO:
                LOG_ERROR("When attempting to delete directory \"%S\" a physical I/O error has occured",RE_DIRECTORY_IO,dirname)
            break;
            case ENAMETOOLONG:
                LOG_ERROR("When attempting to delete directory \"%S\" the given name is too long",RE_DIRECTORY_NAMELENGTH,dirname)
            break;
            case EROFS:
                LOG_ERROR("When attempting to delete directory \"%S\" the directory resides in a read only file system",RE_DIRECTORY_ROPARENT,dirname)
            break;
            default:
                LOG_ERROR("Failed to delete directory \"%S\"",RE_DIRECTORY_DELETE,dirname)
            break;
        }
        ret = RF_LastError;
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

    //if we get here this means it's a file that needs deletion
    if(remove(name->bytes)!=0)
    {
        switch(errno)
        {
            case EACCES:
            case EPERM:
                LOG_ERROR("When attempting to delete file \"%S\" permission is denied on a component of the \
path prefix, or write permission is denied on the parent directory of the file to be removed",RE_PERMISSION,name)
            break;
            case EBUSY:
                LOG_ERROR("When attempting to delete file \"%S\" permission is denied because it is currently\
used by some other proccess or by the system",RE_FILE_BUSY,name)
            break;
            case EINVAL:
            case ENOENT:
            case ENOTDIR:
                LOG_ERROR("When attempting to delete file \"%S\" permission is denied because the path argument is invalid or it is not a file",RE_FILE_NOTFILE,name)
            break;
            case EIO:
                LOG_ERROR("When attempting to delete file \"%S\" a physical I/O error has occured",RE_FILE_IO,name)
            break;
            case ENAMETOOLONG:
                LOG_ERROR("When attempting to delete file \"%S\" the given name is too long",RE_FILE_NAMELENGTH,name)
            break;
            case EROFS:
                LOG_ERROR("When attempting to delete file \"%S\" the file resides in a read only file system",RE_FILE_RO,name)
            break;
            default:
                LOG_ERROR("Failed to delete file \"%S\"",RE_FILE_DELETE,name)
            break;
        }
        ret = RF_LastError;
    }//end of check of succesful file removal

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

    if(rename(name->bytes,newName->bytes)!= 0)
    {
        switch(errno)
        {
            case EACCES:
            case EPERM:
                LOG_ERROR("When attempting to rename file \"%S\" to \"%S\", permission is denied on a component of the \
path prefix, or write permission is denied on the parent directory of the file to be removed",RE_PERMISSION,name,newName)
            break;
            case EBUSY:
                LOG_ERROR("When attempting to rename file \"%S\" to \"%S\", permission is denied because it is currently\
used by some other proccess or by the system",RE_FILE_BUSY,name,newName)
            break;
            case EINVAL:
            case ENOENT:
            case ENOTDIR:
                LOG_ERROR("When attempting to rename file \"%S\" to \"%S\", permission is denied because the path argument is invalid or it is not a file",RE_FILE_NOTFILE,name,newName)
            break;
            case EIO:
                LOG_ERROR("When attempting to rename file \"%S\" to \"%S\", a physical I/O error has occured",RE_FILE_IO,name,newName);
            break;
            case ENAMETOOLONG:
                LOG_ERROR("When attempting to rename file \"%S\" to \"%S\", the given name is too long",RE_FILE_NAMELENGTH,name,newName)
            break;
            case EROFS:
                LOG_ERROR("When attempting to rename file \"%S\" to \"%S\", the file resides in a read only file system",RE_FILE_RO,name,newName)
            break;
            default:
                LOG_ERROR("Failed to rename file \"%S\" to \"%S\"",RE_FILE_DELETE,name,newName)
            break;
        }
        ret = RF_LastError;
    }//end of check for succesful renaming

    RF_EXIT_LOCAL_SCOPE()
    return ret;
}
