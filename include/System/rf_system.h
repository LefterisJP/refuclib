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
**
** --System/rf_system.h
** This header declares functions and macros that act on the respective system, such as file creation, deletion e.t.c.
**
---------------------For internal library include make sure to have----------------------------
#include <Definitions/types.h> //for fixed size data types
#include <Definitions/imex.h>  //for import export macro
#include <System/rf_system.h>
---------------------For internal library include make sure to have----------------------------
*/
#ifndef RF_SYSTEM_H
#define RF_SYSTEM_H


/**
** @defgroup RF_SystemGRP System
** @addtogroup RF_SystemGRP
** @{
**/

//The directory separator depending on the user's system
#ifdef REFU_WIN32_VERSION
/**
 ** The directory separator for the underlying operating system
 **/
	#define RF_DIRSEP "\\"
  #define RF_CURDIR "."
#elif defined(REFU_LINUX_VERSION)
	#define RF_DIRSEP "/"
  #define RF_CURDIR "."
#else
	#error Unsupported OS detected
#endif

///--- The File Permission bits ---
#ifdef REFU_WIN32_VERSION  //in windows just defining them so they exist as macros
    #define RFP_IRUSR       0
    #define RFP_IREAD       0
    #define RFP_IWUSR       0
    #define RFP_IWRITE      0
    #define RFP_IXUSR       0
    #define RFP_IEXEC       0
    #define RFP_IRWXU       0
    #define RFP_IRGRP       0
    #define RFP_IWGRP       0
    #define RFP_IXGRP       0
    #define RFP_IRWXG       0
    #define RFP_IROTH       0
    #define RFP_IWOTH       0
    #define RFP_IXOTH       0
    #define RFP_IRWXO       0
    #define RFP_ISUID       0
    #define RFP_ISGID       0
    #define RFP_ISVTX       0
#else // the Refu wrappers of the permission bits
    #include <sys/stat.h> //this is where all permissions are defined in linux
    ///Read permission bit for the owner of the file
    #define RFP_IRUSR S_IRUSR
    #define RFP_IREAD S_IREAD
    ///Write permission bit for the owner of the file
    #define RFP_IWUSR S_IWUSR
    #define RFP_IWRITE S_IWRITE
    ///Execute (for ordinary files) or search (for directories) permission bit for the owner of the file
    #define RFP_IXUSR S_IXUSR
    #define RFP_IEXEC S_IEXEC
    /// This is equivalent to ‘(RFP_IRUSR | RFP_IWUSR | RFP_IXUSR)’.
    #define RFP_IRWXU S_IRWXU
    /// Reading Permission bit for the group owner of the file
    #define RFP_IRGRP S_IRGRP
    /// Write permission bit for the group owner of the file
    #define RFP_IWGRP S_IWGRP
    /// Execute or search permission bit for the group owner of the file
    #define RFP_IXGRP S_IXGRP
    /// This is equivalent to ‘(RFP_IRGRP | RFP_IWGRP | RFP_IXGRP)’.
    #define RFP_IRWXG S_IRWXG
    /// Read permission bit for other users.
    #define RFP_IROTH S_IROTH
    /// Write permission bit for other users.
    #define RFP_IWOTH S_IWOTH
    /// Execute or search permission bit for other users.
    #define RFP_IXOTH S_IXOTH
    /// This is equivalent to ‘(RFP_IROTH | RFP_IWOTH | RFP_IXOTH)’.
    #define RFP_IRWXO S_IRWXO
    /// This is the set-user-ID on execute bit
    #define RFP_ISUID S_ISUID
    /// This is the set-group-ID on execute bit
    #define RFP_ISGID S_ISGID
    /// This is the sticky bit
    #define RFP_ISVTX S_ISVTX
#endif

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif

/**
 ** @brief Creates a directory
 **
 ** This is a wrapper of the mkdir and _mkdir functions. Creates a new
 ** directory with the given name.
 ** @lmsFunction
 ** @param dirname A string with the name of the directory to create.
 ** @inhtype{String,StringX} @tmpSTR
 ** @param mode This is an optional parameter that only makes sense in Linux.
 ** In Windows this is ignored.
 ** In Linux pass here the permission bits for the newly created directory.
 ** The legal values comes from here and are just wrappers of these values so
 ** that they can also be visible in Windows.
 ** http://www.gnu.org/software/libc/manual/html_node/Creating-Directories.html
 ** Legal values can be a combination of:
 ** + @c RFP_IRUSR or @c RFP_IREAD: Read permission bit for the owner of the file
 ** + @c RFP_IWRITE or @c RFP_IWUSR: Write permission bit for the owner of the file
 ** + @c RFP_IXUSR or @c RFP_IEXEC: Execute (for ordinary files) or search
 ** (for directories) permission bit for the owner of the file
 ** + @c RFP_IRWXU: This is equivalent to
 ** ‘(@c RFP_IRUSR | @c RFP_IWUSR | @c RFP_IXUSR)’
 ** + @c RFP_IRGRP: Reading Permission bit for the group owner of the file
 ** + @c RFP_IWGRP: Write permission bit for the group owner of the file
 ** + @c RFP_IXGRP: Execute or search permission bit for the group owner of the file
 ** + @c RFP_IRWXG: This is equivalent to
 ** ‘(@c RFP_IRGRP | @c RFP_IWGRP | @c RFP_IXGRP)’.
 ** + @c RFP_IROTH: Read permission bit for other users
 ** + @c RFP_IWOTH: Write permission bit for other users
 ** + @c RFP_IXOTH: Execute/search permission bit for other users
 ** + @c RFP_IRWXO: This is equivalent to
 **  ‘(@c RFP_IROTH | @c RFP_IWOTH | @c RFP_IXOTH)’.
 ** + @c RFP_ISUID: This is the set-user-ID on execute bit
 ** + @c RFP_ISGID: This is the set-group-ID on execute bit
 ** + @c RFP_ISVTX: This is the sticky bit
 ** @return Returns @c true for success and @c false for error
 **/
i_DECLIMEX_ char rfMakeDir(void* dirname,int mode);

/**
 ** @brief Removes a directory and all its files recursively
 **
 ** This functions deletes all the files and child directories under the
 ** given directory and the directory itself.
 ** The calling proccess needs to have sufficient permissions.It is quite a
 ** dangerous function so be careful when using it since it can recursively
 ** delete everything.
 ** @lmsFunction
 ** @warning This function will permanently delete all the files and children
 ** directories of the parameter directory
 ** recursively. Do not use it if you are not sure of what you are doing.
 ** @param dirname A string of the name of the directory to delete
 ** @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true for success and @c false otherwise
 **/
i_DECLIMEX_ char rfRemoveDir(void* dirname);


/**
 ** @brief Deletes a file
 **
 ** This functions deletes a file from the system
 ** @lmsFunction
 ** @param name A string with the name and path of the file to delete
 ** @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true for success and @c false for failure
 **/
i_DECLIMEX_ char rfDeleteFile(void* name);


/**
 ** @brief Renames a file
 **
 ** This functions renames the file with name @c name to @c newName
 ** @lmsFunction
 ** @param name A string with the name and path of the file to rename
 ** @inhtype{String,StringX} @tmpSTR
 ** @param newName A string with the new name and path of the file
 ** @inhtype{String,StringX} @tmpSTR
 ** @return Returns @c true for success and @c false if an error occured
 **/
i_DECLIMEX_ char rfRenameFile(void* name, void* newName);


/**
 ** @brief Returns a unique address for the calling thread
 **
 ** For Linux it uses the gettid system call
 ** and for Windows it needs to be implemented as it's not
 ** currently supported.
 ** @return A unique id representing the calling thread
 **/
i_DECLIMEX_ uintptr_t rfSystem_GetThreadID();

//! @}
//end of system group

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end
