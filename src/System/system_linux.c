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
/*------------- Corrensponding Header inclusion -------------*/
#include <System/rf_system.h>
/*------------- Outside Module inclusion -------------*/
#include <String/string_decl.h> //for RF_String
#include <String/stringx_decl.h> //for RF_StringX
#include <Definitions/defarg.h> //for the default argument declared string functions that are included below
#include <String/common.h> //for RFS_()
#include <String/corex.h> //for rfStringX_Init_buff() and others
#include <String/conversion.h> // for rfString_Cstr()
//for error logging
    #include <Utils/error.h>
//for memory allocation
    #include <Utils/memory.h> //for refu memory allocation
//for local scope macros
    #include <Utils/localscope.h>
//for IO/file function definitions
    #include <IO/common.h> //stat_rft definition
    #include <Definitions/inline.h> //for inline definitions
    #include <IO/common.h> //for stat_rft
    #include <IO/file.h> //rfStat() definition
/*------------- System specific inclusion --------------*/
#include <unistd.h> //for rmdir()
/*------------- libc inclusion --------------*/
#include <errno.h>
#include <dirent.h>
/*------------- system includes -------------*/
#include <unistd.h> //for syscall and tid
#include <sys/syscall.h> //for syscall and tid
/*------------- End of includes -------------*/


//Creates a directory
bool rfMakeDir(void* dirnameP,int mode)
{
    RF_String* dirname = (RF_String*)dirnameP;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(dirname == NULL)
    {
        RF_ERROR("Provided a NULL string for the path argument in "
                 "the directory creation function");
        ret = false;
        goto cleanup;
    }
#endif

    //make the directory
    if(mkdir(dirname->bytes,mode)!=0)
    {
        RF_ERROR("Creating a directory failed due to mkdir() "
                 "errno %d", errno);
        ret = false;
    }

#if RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes a directory and all its files
bool rfRemoveDir(void* dirnameP)
{
    RF_String* dirname = (RF_String*)dirnameP;
    bool ret = true;
    DIR* dir;
    struct dirent *entry;
    RF_StringX path;

    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUF
    if(dirname == NULL)
    {
        RF_ERROR("Null string given for the dirname argument");
        ret = false;
        goto cleanup1;
    }
#endif


    if(!rfStringX_Init_buff(&path, 1024, ""))
    {
        RF_ERROR("Failed to initialize a stringX buffer");
        ret = false;
        goto cleanup1;
    }
    //open the directory
    if((dir = opendir(dirname->bytes)) == NULL)
    {
        RF_ERROR("Failed to open the given directory due to "
                 "opendir() errno %d", errno);
        ret = false;
        goto cleanup2;
    }

    //keep the previous errno for  comparison
    int prErrno = errno;
    while((entry = readdir(dir)) != NULL)
    {
        //skip this and the parent dir
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            //create the full entry name
            if(!rfStringX_Assign(
                   &path,
                   RFS_("%S"RF_DIRSEP"%s",dirname,entry->d_name)))
            {
                RF_ERROR("Failure at assigning the directory name to the "
                         "path string");
                ret = false;
                goto cleanup2;
            }
            //if this directory entry is a directory itself, call the function recursively
            if (entry->d_type == DT_DIR)
            {
                if(!rfRemoveDir(&path))
                {
                    RF_ERROR("Calling rfRemoveDir on directory \"%S\" failed",
                             &path);
                    ret = false;
                    goto cleanup2;
                }
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }
            //if we get here this means it's a file that needs deletion
            if(unlink(path.INH_String.bytes)!=0)
            {
                RF_ERROR("Failed to remove file \"%S\" due to unlink() "
                         "errno %d", &path, errno);
                ret = false;
                goto cleanup2;
            }//end of check of succesful file removal
        }//end of the current and parent dir check
    }//end of directory entries iteration
    //check for readdir error
    if(errno != prErrno) //is this the best way to check?
    {
        RF_ERROR("Failure in reading the contents of a directory "
                 "due to readdir() errno %d", errno);
        ret = false;
        goto cleanup2;
    }
    if(closedir(dir) != 0)
    {
        RF_ERROR("Failure in closing a directory", "closedir");
        ret = false;
        goto cleanup2;
    }
    //free the path string
    rfStringX_Deinit(&path); // back to cleanup1
    //finally delete the directory itself
    if(rmdir(dirname->bytes) != 0)
    {
        RF_ERROR("Failed to remove directory \"%S\" due to rmdir() errno %d",
                 dirname, errno);
        ret = false;
    }

    //success
    goto cleanup1;

  cleanup2:
    //free the path string
    rfStringX_Deinit(&path);
cleanup1:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Deletes a file
bool rfDeleteFile(void* nameP)
{
    RF_String* name = (RF_String*)nameP;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(name == NULL)
    {
        RF_ERROR("Provided a null filename string");
        ret = false;
        goto cleanup;
    }
#endif

    //if we get here this means it's a file that needs deletion
    if(unlink(name->bytes) != 0)
    {
        RF_ERROR("Removing file \"%S\" failed due to unlink() errno %d",
                 name, errno);
        ret = false;
    }//end of check of succesful file removal

#if RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

// Renames a file
bool rfRenameFile(void* nameP, void* newNameP)
{
    RF_String* name = (RF_String*)nameP;
    RF_String* newName = (RF_String*)newNameP;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();

#if RF_OPTION_DEBUG
    if(name == NULL || newName == NULL)
    {
        RF_ERROR("Provided a null string for either the name or the newName "
                 "argument");
        ret = false;
        goto cleanup;
    }
#endif

    if(rename(name->bytes, newName->bytes)!= 0)
    {
        RF_ERROR("Renaming file \"%S\" to \"%S\" failed due to rename() "
                 "errno %d", name, newName, errno);
        ret = false;
    }//end of check for succesful renaming

#if RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}


bool rfFileExists(void* name)
{
    stat_rft sb;   
    RF_String* file_name = name;
    return (rfStat(file_name, &sb) == 0);
}

threadid_t rfSystem_GetThreadID()
{
    return syscall(SYS_gettid);
}
