/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <System/rf_system.h>
/*------------- Outside Module inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstringx and RF_String
#include <String/rf_str_common.h> //for RFS_()
#include <String/rf_str_corex.h> //for rf_stringx_init_buff() and others
#include <String/rf_str_retrieval.h> //for accessors
#include "../String/rf_str_conversion.ph" //for the buffer Cstr() conversion
#include <Utils/log.h> //for error logging
#include <Utils/memory.h> //for refu memory allocation
#include <IO/rf_file.h> //rfStat() definition
#include <Utils/sanity.h> //for Sanity macros
/*------------- libc inclusion --------------*/
#include <errno.h>
#include <dirent.h>
/*------------- system includes -------------*/
#include <unistd.h> //for syscall and tid
#include <sys/syscall.h> //for syscall and tid
/*------------- End of includes -------------*/


//Creates a directory
bool rf_system_make_dir(void* dirname, int mode)
{
    char* cs;
    unsigned int index;
    RF_NULLCHECK1(dirname, "directory name", return false);

    if (!(cs = rf_string_cstr_ibuff_push(dirname, &index))) {
        return false;
    }
    
    //make the directory
    if (mkdir(cs, mode) != 0) {
        RF_ERROR("Creating a directory failed due to mkdir() "
                 "errno %d", errno);
        return false;
    }

    rf_string_cstr_ibuff_pop(index);
    return true;
}

//Removes a directory and all its files
bool rf_system_remove_dir(void* dirname)
{
    bool ret = true;
    DIR* dir;
    struct dirent *entry;
    char *cs;
    unsigned int index;
    struct RFstringx path;
    RF_NULLCHECK1(dirname, "directory name", return false);

    if (!(cs = rf_string_cstr_ibuff_push(dirname, &index))) {
        return false;
    }

    if (!rf_stringx_init_buff(&path, 1024, "")) {
        RF_ERROR("Failed to initialize a stringX buffer");
        ret = false;
        goto cleanup_cstr_buff;
    }
    //open the directory
    if (!(dir = opendir(cs))) {
        RF_ERROR("Failed to open the given directory due to "
                 "opendir() errno %d", errno);
        ret = false;
        goto cleanup_path;
    }

    //keep the previous errno for  comparison
    int prErrno = errno;
    while ((entry = readdir(dir)) != NULL) {
        
        //skip this and the parent dir
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            
            //create the full entry name
            RFS_buffer_push();
            if (!rf_stringx_assign(
                    &path,
                    RFS_(RF_STR_PF_FMT RF_DIRSEP "%s",
                         RF_STR_PF_ARG(dirname), entry->d_name))) {

                RF_ERROR("Failure at assigning the directory name to the "
                         "path string");
                ret = false;
                RFS_buffer_pop();
                goto cleanup_path;
            }
            RFS_buffer_pop();
            
            //if this directory entry is a directory itself, call the function recursively
            if (entry->d_type == DT_DIR) {
                if (!rf_system_remove_dir(&path)) {
                    RF_ERROR("Calling rf_system_remove_dir on directory "
                             "\""RF_STR_PF_FMT"\" failed",
                             RF_STR_PF_ARG(&path));
                    ret = false;
                    goto cleanup_path;
                }
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }
            
            //if we get here this means it's a file that needs deletion
            if (!rf_system_delete_file(&path)) {
                RF_ERROR("Failed to remove file \""RF_STR_PF_FMT"\""
                         " due to unlink() "
                         "errno %d", RF_STR_PF_ARG(&path), errno);
                ret = false;
                goto cleanup_path;
            }//end of check of succesful file removal
        }//end of the current and parent dir check
    }//end of directory entries iteration
    
    //check for readdir error
    if (errno != prErrno) { //is this the best way to check?
        RF_ERROR("Failure in reading the contents of a directory "
                 "due to readdir() errno %d", errno);
        ret = false;
        goto cleanup_path;
    }
    
    if (closedir(dir) != 0) {
        RF_ERROR("Failure in closing a directory", "closedir");
        ret = false;
        goto cleanup_path;
    }
    //finally delete the directory itself
    if (!rf_system_remove_dir(dirname)) {
        RF_ERROR("Failed to remove directory \""RF_STR_PF_FMT"\" "
                 "due to rmdir() errno %d",
                 RF_STR_PF_ARG(dirname), errno);
        ret = false;
    }


cleanup_path:
    rf_stringx_deinit(&path);
cleanup_cstr_buff:
    rf_string_cstr_ibuff_pop(index);
    return ret;
}

//Deletes a file
bool rf_system_delete_file(const void* name)
{
    char *cs;
    unsigned int index;
    RF_NULLCHECK1(name, "file name", return false);
    if (!(cs = rf_string_cstr_ibuff_push(name, &index))) {
        return false;
    }        

    //if we get here this means it's a file that needs deletion
    if (unlink(cs) != 0) {
        RF_ERROR("Removing file \""RF_STR_PF_FMT"\" failed due"
                 " to unlink() errno %d",
                 RF_STR_PF_ARG(name), errno);
        return false;
    }//end of check of succesful file removal

    rf_string_cstr_ibuff_pop(index);
    return true;
}

// Renames a file
bool rf_system_rename_file(void* name, void* newName)
{
    bool ret = true;
    char *cs_name;
    char *cs_new_name;
    unsigned int index;
    RF_NULLCHECK2(name, newName, return false);

    if (!(cs_name = rf_string_cstr_ibuff_push(name, &index))) {
        return false;
    }    
    if (!(cs_new_name = rf_string_cstr_ibuff_push(newName, NULL))) {
        ret = false; goto cleanup1;
    }

    if (rename(cs_name, cs_new_name) != 0) {
        RF_ERROR("Renaming file \""RF_STR_PF_FMT"\" to "
                 "\""RF_STR_PF_FMT"\" failed due to rename() "
                 "errno %d", RF_STR_PF_ARG(name), RF_STR_PF_ARG(newName),
                 errno);
        ret = false;
    }//end of check for succesful renaming


  cleanup1:
    rf_string_cstr_ibuff_pop(index);
    return ret;
}


bool rf_system_file_exists(void* name)
{
    stat_rft sb;   
    struct RFstring* file_name = name;
    return (rfStat(file_name, &sb) == 0);
}

RFthread_id rf_system_get_thread_id()
{
    return syscall(SYS_gettid);
}


FILE* rf_fopen(const void* name, const char* mode)
{
    unsigned int index;
    char* cs;
    FILE* ret;
    if(!(cs = rf_string_cstr_ibuff_push(name, &index)))
    {
        return NULL;
    }
    ret = fopen(cs, mode);
    rf_string_cstr_ibuff_pop(index);
    return ret;
}

FILE* rf_freopen(const void* name, const char* mode, FILE* f)
{
    unsigned int index;
    char* cs;
    FILE* ret;
    if(!(cs = rf_string_cstr_ibuff_push(name, &index)))
    {
        return NULL;
    }
    ret = freopen(cs, mode, f);
    rf_string_cstr_ibuff_pop(index);
    return ret;
}

FILE* rf_popen(void* command, const char* mode)
{
    FILE* ret = NULL;
    unsigned int index;
    char *cs;


    if (strcmp(mode,"r") != 0 && strcmp(mode,"w") != 0) {
        RF_ERROR("Invalid mode argument provided to rf_popen()");
        return NULL;
    }

    if(!(cs = rf_string_cstr_ibuff_push(command, &index))) {
        return NULL;
    }

    ret = popen(cs, mode);

    rf_string_cstr_ibuff_pop(index);
    return ret;
}

int rf_pclose(FILE* stream)
{
    return pclose(stream);
}

bool rf_system_activate()
{
    int32_t anint;
    
    /* get system endianess */
    anint= (int32_t)0xdeadbeef;
    g_sys_info.endianess = (*(char *)&anint == (char)0xef)?
    RF_LITTLE_ENDIAN : RF_BIG_ENDIAN;

    /* see if we can have high res timer */
    g_sys_info.has_high_res_timer = true;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, 0) == -1) {
        if (clock_getres(CLOCK_MONOTONIC, 0) == -1) {
            if (clock_getres(CLOCK_REALTIME, 0) == -1) {
                RF_ERROR("No high resolution timer is supported. Even "
                         "CLOCK_REALTIME initialization failed.");
                g_sys_info.has_high_res_timer = false;
            } else {
                g_sys_info.timerType = CLOCK_REALTIME;
            }
        } else {
            g_sys_info.timerType = CLOCK_MONOTONIC;
        }
    } else {
        g_sys_info.timerType = CLOCK_PROCESS_CPUTIME_ID;
    }
    return true;
}

void rf_system_deactivate()
{
    //nothing needed for now
}
