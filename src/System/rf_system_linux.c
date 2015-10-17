/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <System/rf_system.h>
/*------------- Outside Module inclusion -------------*/
#include <String/rf_str_xdecl.h> //for RFstringx and RF_String
#include <String/rf_str_common.h> //for RFS_()
#include <String/rf_str_corex.h> //for rf_stringx_init_buff() and others
#include <String/rf_str_retrieval.h> //for accessors
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
bool rf_system_make_dir(void *dirname, int mode)
{
    char *cstr;
    bool ret = false;
    RF_NULLCHECK1(dirname, "directory name", return false);

    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(dirname))) {
        goto end_pop;
    }

    //make the directory
    if (mkdir(cstr, mode) != 0) {
        RF_ERROR("Creating a directory failed due to mkdir() "
                 "errno %d", errno);
        goto end_pop;
    }
    ret = true;

end_pop:
    RFS_POP();
    return ret;
}

//Removes a directory and all its files
bool rf_system_remove_dir(void *dirname)
{
    bool ret = true;
    DIR *dir;
    struct dirent *entry;
    char *cstr;
    struct RFstringx path;
    RF_NULLCHECK1(dirname, "directory name", return false);

    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(dirname))) {
        ret = false;
        goto cleanup_cstr_buff;
    }

    if (!rf_stringx_init_buff(&path, 1024, "")) {
        RF_ERROR("Failed to initialize a stringX buffer");
        ret = false;
        goto cleanup_cstr_buff;
    }
    //open the directory
    if (!(dir = opendir(cstr))) {
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
            RFS_PUSH();
            struct RFstring *full_entry_name = RFS(RF_STR_PF_FMT RF_DIRSEP "%s",
                                                   RF_STR_PF_ARG(dirname),
                                                   entry->d_name);
            if (!full_entry_name || !rf_stringx_assign(&path, full_entry_name)) {
                RF_ERROR("Failure at assigning the directory name to the "
                         "path string");
                ret = false;
                RFS_POP();
                goto cleanup_path;
            }
            RFS_POP();

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
    RFS_POP();
    return ret;
}

//Deletes a file
bool rf_system_delete_file(const void *name)
{
    char *cstr;
    bool ret = false;
    RF_NULLCHECK1(name, "file name", return false);
    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(name))) {
        goto end_pop;
    }

    //if we get here this means it's a file that needs deletion
    if (unlink(cstr) != 0) {
        RF_ERROR("Removing file \""RF_STR_PF_FMT"\" failed due"
                 " to unlink() errno %d",
                 RF_STR_PF_ARG(name), errno);
        goto end_pop;
    }//end of check of succesful file removal
    ret = true;

end_pop:
    RFS_POP();
    return ret;
}

// Renames a file
bool rf_system_rename_file(void *name, void *new_name)
{
    bool ret = false;
    char *cs_name;
    char *cs_new_name;
    RF_NULLCHECK2(name, new_name, return false);

    RFS_PUSH();
    if (!(cs_name = rf_string_cstr_from_buff(name))) {
        goto end_pop;
    }
    if (!(cs_new_name = rf_string_cstr_from_buff(new_name))) {
        goto end_pop;
    }

    if (rename(cs_name, cs_new_name) != 0) {
        RF_ERROR("Renaming file \""RF_STR_PF_FMT"\" to "
                 "\""RF_STR_PF_FMT"\" failed due to rename() "
                 "errno %d", RF_STR_PF_ARG(name), RF_STR_PF_ARG(new_name),
                 errno);
        goto end_pop;
    }//end of check for succesful renaming
    ret = true;

  end_pop:
    RFS_POP();
    return ret;
}


bool rf_system_file_exists(void *name)
{
    stat_rft sb;
    struct RFstring *file_name = name;
    return (rfStat(file_name, &sb) == 0);
}

RFthread_id rf_system_get_thread_id()
{
    return syscall(SYS_gettid);
}

FILE *rf_fopen(const struct RFstring *n, const char *mode)
{
    FILE *ret;
    RFS_PUSH();
    ret = fopen(rf_string_cstr_from_buff_or_die(n), mode);
    RFS_POP();
    return ret;
}

FILE *rf_freopen(const void *name, const char *mode, FILE *f)
{
    char *cstr;
    FILE *ret = NULL;
    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(name))) {
        goto end_pop;
    }
    ret = freopen(cstr, mode, f);

end_pop:
    RFS_POP();
    return ret;
}

FILE *rf_popen(const void *command, const char *mode)
{
    FILE *ret = NULL;
    char *cstr;


    if (strcmp(mode,"r") != 0 && strcmp(mode,"w") != 0) {
        RF_ERROR("Invalid mode argument provided to rf_popen()");
        return NULL;
    }

    RFS_PUSH();
    if (!(cstr = rf_string_cstr_from_buff(command))) {
        goto end_pop;
    }
    ret = popen(cstr, mode);

end_pop:
    RFS_POP();
    return ret;
}

int rf_pclose(FILE *stream)
{
    return pclose(stream);
}

bool rf_system_activate()
{
    int32_t anint;

    // get system endianess
    anint= (int32_t)0xdeadbeef;
    g_sys_info.endianess = (*(char *)&anint == (char)0xef)?
    RF_LITTLE_ENDIAN : RF_BIG_ENDIAN;

    // see if we can have high res timer
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
