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

#include <rf_system.h>
#include <errno.h>
#include <direct.h>
#include <unistd.h>
#include <dirent.h>
#include <rf_string.h>
#include <sys/stat.h>
#include <windows.h>


//Creates a directory
int32_t i_rfMakeDir(void* dirnameP,int* modeP)
{
    RF_String* dirName = (RF_String*)dirnameP;
    int mode = *modeP;
    //make the directory
    if(_mkdir(dirName->bytes)==0)
        return RF_SUCCESS;
    //if we got an error
    switch(errno)
    {
        case EEXIST:
            return RE_DIRECTORY_EXISTS;
        break;
        case ENOENT:
            LOG_ERROR("Failed to create directory \"%s\" because the given path was not found",RE_DIRECTORY_INVALID,dirName->bytes);
            return RE_DIRECTORY_INVALID;
        break;
    }
    //failure
    return RE_DIRECTORY_CREATE;
}

//Removes a directory and all its files
int32_t i_rfRemoveDir(void* dirnameP)
{
    RF_String* dirname = (RF_String*)dirnameP;
    DIR* dir;
    struct dirent *entry;
    RF_StringX path;
    struct stat s;
    rfStringX_Init_buff(&path,1024,"");
    //open the directory
    dir = opendir(dirname->bytes);
    if(dir == NULL)
    {
        switch(errno)
        {
            case EACCES:
                LOG_ERROR("Failed to access directory \"%s\" because of insufficient permissions",RE_PERMISSION,dirname->bytes);
                return RE_PERMISSION;
            break;
            case ENAMETOOLONG:
                LOG_ERROR("Can't access directory \"%s\" because its name is too long for the filesystem",RE_DIRECTORY_NAMELENGTH,dirname->bytes);
                return RE_DIRECTORY_NAMELENGTH;
            break;
            case ENOENT:
            case ENOTDIR:
                LOG_ERROR("Failed to access directory \"%s\" because the given path was not found or is not a directory",RE_DIRECTORY_INVALID,dirname->bytes);
                return RE_DIRECTORY_INVALID;
            break;
            case ENFILE:
            case EMFILE:
                LOG_ERROR("Failed to access directory \"%s\" because too many files are currently open in the system",RE_DIRECTORY_TOOMANY_FILES,dirname->bytes);
                return RE_DIRECTORY_TOOMANY_FILES;
            break;
            default:
                LOG_ERROR("Failed to access directory \"%s\" because of opendir() failure",RE_DIRECTORY_OPEN,dirname->bytes);
                return RE_DIRECTORY_OPEN;
            break;
        }
    }
    //keep the previous errno for  comparison
    int prErrno = errno;
    while((entry = readdir(dir)) != NULL)
    {
        //skip this and the parent dir
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            //create the full entry name
            rfStringX_Assign(&path,RFS_("%s"RF_DIRSEP"%s",dirname->bytes,entry->d_name));
            //if this directory entry is a directory itself, call the function recursively
            stat(path.INH_String.bytes, &s);
            if (s.st_mode & S_IFDIR)
            {
                int ret;
                if( (ret = rfRemoveDir(&path)) != RF_SUCCESS)
                    return ret;
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }
            //if we get here this means it's a file that needs deletion
            if(DeleteFile(path.INH_String.bytes)==0)
            {
                RF_WIN32_GETSYSERROR(strBuff)
                LOG_ERROR("Recursive directory deletion of directory \"%s\" failed because of inability to delete file \"%s\". Windows error (%lu) was given:\n%s",
                          RE_FILE_DELETE,dirname->bytes,path.INH_String.bytes,i_ERROR_CODE,strBuff)
                LocalFree(strBuff);
                return RE_FILE_DELETE;
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
                LOG_ERROR("During iteration of directory \"%s\" the directory value was invalid",RE_DIRECTORY_INVALID,dirname->bytes)
                return RE_DIRECTORY_INVALID;
            break;
            case ENOENT:
                LOG_ERROR("During iteration of directory \"%s\" the current position of the directory stream was invalid",RE_DIRECTORY_STREAMPOS,dirname->bytes)
                return RE_DIRECTORY_STREAMPOS;
            break;
            default:
                LOG_ERROR("During iteration of directory \"%s\" reading the directory failed",RE_DIRECTORY_READ,dirname->bytes)
                return RE_DIRECTORY_READ;
            break;
        }
    }
    //finally delete the directory itself
    if(RemoveDirectory(dirname->bytes)==0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Recursive directory deletion of directory \"%s\" failed because of inability to delete the directory itself. Windows error (%lu) was given:\n%s",
                  RE_DIRECTORY_DELETE,dirname->bytes,i_ERROR_CODE,strBuff);
        LocalFree(strBuff);
        return RE_DIRECTORY_DELETE;
    }
    return RF_SUCCESS;
}

//Deletes a file
int32_t i_rfDeleteFile(void* nameP)
{
    RF_String* name = (RF_String*)nameP;
    if(DeleteFile(name->bytes)==0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Failed to delete file \"%s\" because of Windows error(%lu):\n%s",RE_FILE_DELETE,name->bytes,i_ERROR_CODE,strBuff)
        LocalFree(strBuff);
        return RE_FILE_DELETE;
    }
    return RF_SUCCESS;
}
// Renames a file
int32_t i_rfRenameFile(void* nameP,void* newNameP)
{
    RF_String* name = (RF_String*)nameP;
    RF_String* newName = (RF_String*)newNameP;
    if(MoveFile(name->bytes,newName->bytes) ==0)
    {
        RF_WIN32_GETSYSERROR(strBuff)
        LOG_ERROR("Failed to rename file \"%s\" to \"%s\" because of Windows error(%lu):\n:%s",RE_FILE_RENAME,name->bytes,newName->bytes,i_ERROR_CODE,strBuff)
        LocalFree(strBuff);
        return RE_FILE_RENAME;
    }
    return RF_SUCCESS;
}

