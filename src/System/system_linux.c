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
#include <String/retrieval.h> //for accessors
#include "../String/conversion.ph" //for the buffer Cstr() conversion
//for error logging
    #include <Utils/log.h>
//for memory allocation
    #include <Utils/memory.h> //for refu memory allocation
//for local scope macros
    #include <Utils/localscope.h>
//for IO/file function definitions
    #include <IO/common.h> //stat_rft definition
    #include <Definitions/inline.h> //for inline definitions
    #include <IO/common.h> //for stat_rft
    #include <IO/file.h> //rfStat() definition
//for Sanity macros
    #include <Utils/sanity.h>
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
bool rfMakeDir(void* dirname, int mode)
{
    char* cs;
    unsigned int index;
    bool ret = true;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(dirname, "directory name", ret = false; goto cleanup);

    if(!(cs = rfString_Cstr_ibuff_push(dirname, &index)))
    {
        ret = false;
        goto cleanup;
    }
    
    //make the directory
    if(mkdir(cs, mode) != 0)
    {
        RF_ERROR("Creating a directory failed due to mkdir() "
                 "errno %d", errno);
        ret = false;
    }


    rfString_Cstr_ibuff_pop(index);
cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Removes a directory and all its files
bool rfRemoveDir(void* dirname)
{
    bool ret = true;
    DIR* dir;
    struct dirent *entry;
    char *cs;
    unsigned int index;
    RF_StringX path;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(dirname, "directory name",
                      ret = false; goto cleanup_local);

    if(!(cs = rfString_Cstr_ibuff_push(dirname, &index)))
    {
        ret = false;
        goto cleanup_local;
    }

    if(!rfStringX_Init_buff(&path, 1024, ""))
    {
        RF_ERROR("Failed to initialize a stringX buffer");
        ret = false;
        goto cleanup_cstr_buff;
    }
    //open the directory
    if(!(dir = opendir(cs)))
    {
        RF_ERROR("Failed to open the given directory due to "
                 "opendir() errno %d", errno);
        ret = false;
        goto cleanup_path;
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
                   RFS_(RF_STR_PF_FMT RF_DIRSEP "%s",
                        RF_STR_PF_ARG(dirname), entry->d_name)))
            {
                RF_ERROR("Failure at assigning the directory name to the "
                         "path string");
                ret = false;
                goto cleanup_path;
            }
            //if this directory entry is a directory itself, call the function recursively
            if (entry->d_type == DT_DIR)
            {
                if(!rfRemoveDir(&path))
                {
                    RF_ERROR("Calling rfRemoveDir on directory "
                             "\""RF_STR_PF_FMT"\" failed",
                             RF_STR_PF_ARG(&path));
                    ret = false;
                    goto cleanup_path;
                }
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }
            //if we get here this means it's a file that needs deletion
            if(!rfDeleteFile(&path))
            {
                RF_ERROR("Failed to remove file \""RF_STR_PF_FMT"\""
                         " due to unlink() "
                         "errno %d", RF_STR_PF_ARG(&path), errno);
                ret = false;
                goto cleanup_path;
            }//end of check of succesful file removal
        }//end of the current and parent dir check
    }//end of directory entries iteration
    //check for readdir error
    if(errno != prErrno) //is this the best way to check?
    {
        RF_ERROR("Failure in reading the contents of a directory "
                 "due to readdir() errno %d", errno);
        ret = false;
        goto cleanup_path;
    }
    if(closedir(dir) != 0)
    {
        RF_ERROR("Failure in closing a directory", "closedir");
        ret = false;
        goto cleanup_path;
    }
    //finally delete the directory itself
    if(!rfRemoveDir(dirname))
    {
        RF_ERROR("Failed to remove directory \""RF_STR_PF_FMT"\" "
                 "due to rmdir() errno %d",
                 RF_STR_PF_ARG(dirname), errno);
        ret = false;
    }


cleanup_path:
    rfStringX_Deinit(&path);
cleanup_cstr_buff:
    rfString_Cstr_ibuff_pop(index);
cleanup_local:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Deletes a file
bool rfDeleteFile(void* name)
{
    bool ret = true;
    char *cs;
    unsigned int index;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_1(name, "file name", ret = false; goto cleanup);
    if(!(cs = rfString_Cstr_ibuff_push(name, &index)))
    {
        ret = false; goto cleanup;
    }        

    //if we get here this means it's a file that needs deletion
    if(unlink(cs) != 0)
    {
        RF_ERROR("Removing file \""RF_STR_PF_FMT"\" failed due"
                 " to unlink() errno %d",
                 RF_STR_PF_ARG(name), errno);
        ret = false;
    }//end of check of succesful file removal

    rfString_Cstr_ibuff_pop(index);
 cleanup:
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

// Renames a file
bool rfRenameFile(void* name, void* newName)
{
    bool ret = true;
    char *cs_name;
    char *cs_new_name;
    unsigned int index;
    RF_ENTER_LOCAL_SCOPE();
    i_NULLPTR_CHECK_2(name, newName, ret = false; goto cleanup);

    if(!(cs_name = rfString_Cstr_ibuff_push(name, &index)))
    {
        ret = false; goto cleanup;
    }    
    if(!(cs_new_name = rfString_Cstr_ibuff_push(newName, NULL)))
    {
        ret = false; goto cleanup1;
    }

    if(rename(cs_name, cs_new_name) != 0)
    {
        RF_ERROR("Renaming file \""RF_STR_PF_FMT"\" to "
                 "\""RF_STR_PF_FMT"\" failed due to rename() "
                 "errno %d", RF_STR_PF_ARG(name), RF_STR_PF_ARG(newName),
                 errno);
        ret = false;
    }//end of check for succesful renaming


  cleanup1:
    rfString_Cstr_ibuff_pop(index);
  cleanup:
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


FILE* rfFopen(void* name, const char* mode)
{
    unsigned int index;
    char* cs;
    FILE* ret;
    if(!(cs = rfString_Cstr_ibuff_push(name, &index)))
    {
        return NULL;
    }
    ret = fopen(cs, mode);
    rfString_Cstr_ibuff_pop(index);
    return ret;
}

FILE* rfFreopen(void* name, const char* mode, FILE* f)
{
    unsigned int index;
    char* cs;
    FILE* ret;
    if(!(cs = rfString_Cstr_ibuff_push(name, &index)))
    {
        return NULL;
    }
    ret = freopen(cs, mode, f);
    rfString_Cstr_ibuff_pop(index);
    return ret;
}
