/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#include <rflib/system/system.h>

#include <rflib/string/xdecl.h>
#include <rflib/string/common.h>
#include <rflib/string/corex.h>
#include <rflib/string/retrieval.h>
#include <rflib/utils/log.h>
#include <rflib/utils/memory.h>
#include <rflib/io/rf_file.h>
#include <rflib/utils/sanity.h>

#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pwd.h>

//Creates a directory
bool rf_system_make_dir(const struct RFstring *dirname, int mode)
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
            struct RFstring *full_entry_name = RFS(
                RFS_PF RF_DIRSEP "%s",
                RFS_PA(dirname),
                entry->d_name
            );
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
                    RF_ERROR(
                        "Calling rf_system_remove_dir on directory "
                        "\""RFS_PF"\" failed",
                        RFS_PA(&path)
                    );
                    ret = false;
                    goto cleanup_path;
                }
                //else we deleted that directory and we should go to the next entry of this directory
                continue;
            }

            //if we get here this means it's a file that needs deletion
            if (!rf_system_delete_file(&path)) {
                RF_ERROR(
                    "Failed to remove file \""RFS_PF"\" due to unlink errno %d",
                    RFS_PA(&path),
                    errno
                );
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
        RF_ERROR(
            "Failed to remove directory \""RFS_PF"\" due to rmdir() errno %d",
            RFS_PA(dirname),
            errno
        );
        ret = false;
    }


cleanup_path:
    rf_stringx_deinit(&path);
cleanup_cstr_buff:
    RFS_POP();
    return ret;
}

const struct RFstring *rf_homedir()
{
    char *home_dir = getenv("HOME");
	if (!home_dir || strlen(home_dir) == 0) {
		struct passwd *pwd = getpwuid(getuid());
		if (pwd) {
			home_dir = pwd->pw_dir;
        }
    }
    return RFS("%s", home_dir);
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
        RF_ERROR(
            "Removing file \""RFS_PF"\" failed due to unlink() errno %d",
            RFS_PA(name),
            errno
        );
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
        RF_ERROR(
            "Renaming file \""RFS_PF"\" to \""RFS_PF"\" failed due to rename()"
            " errno %d",
            RFS_PA(name),
            RFS_PA(new_name),
            errno
        );
        goto end_pop;
    }//end of check for succesful renaming
    ret = true;

  end_pop:
    RFS_POP();
    return ret;
}


bool rf_system_file_exists(const struct RFstring *name)
{
    stat_rft sb;
    return (rfStat(name, &sb) == 0);
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
    // get system endianess
    int32_t anint = (int32_t)0xdeadbeef;
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
