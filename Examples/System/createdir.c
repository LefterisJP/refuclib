#include <RFsystem.h>
#include <RFstring.h>
#include <refu.h>


int main()
{
	rfInit();//init the library
//! [MAKE_DIR]
	//Create a new directory with user read,write and execute privileges
	if(rfMakeDir(RFS_("tempdir"),RFP_IRWXU) != RE_DIRECTORY_EXISTS)
    {
//! [MAKE_DIR]
        //create a new file in that directory
        FILE* f = fopen("tempdir"RF_DIRSEP"afile","w");
        fclose(f);
        //create a second file there
        f = fopen("tempdir"RF_DIRSEP"afile2","w");
        fclose(f);
        //create a third file there
        f = fopen("tempdir"RF_DIRSEP"afile3","w");
        fclose(f);
//! [RENAME_FILE]
        //rename the second file
        rfRenameFile(RFS_("tempdir"RF_DIRSEP"afile2"),RFS_("tempdir"RF_DIRSEP"renamedfile"));
//! [RENAME_FILE]
//! [DELETE_FILE]
        //delete the third file
        rfDeleteFile(RFS_("tempdir"RF_DIRSEP"afile3"));
//! [DELETE_FILE]
    }
//! [REMOVE_DIR]
    else//if the directory exists delete it, recursively destroying anything else residing inside it
    {
        rfRemoveDir(RFS_("tempdir"));
    }
//! [REMOVE_DIR]
    return 0;
}
