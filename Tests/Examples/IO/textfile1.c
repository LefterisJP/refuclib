#include <IO/rfc_textfile.h>

int main()
{
    rfInit("errorLog","infoLog");
    RF_TextFile f;
    RF_StringX lineBuffer;
    int32_t i;
//! [FILE_INIT]
    //let's initialize the file handler and start with writing
    rfTextFile_Init(&f,RFS_("testfile"),RF_FILE_READWRITE_NEW);
//! [FILE_INIT]
//! [FILE_WRITE]
    //write a few lines to the file
    for(i=0;i<5;i++)
        rfTextFile_Write(&f,RFS_("This is line number %d of testfile\n",i));
//! [FILE_WRITE]
//! [FILE_RESET]
    //reset the file pointer to the file's beginning
    rfTextFile_GoToLine(&f,1);
//! [FILE_RESET]
//! [READ_LINES]
    //now read each line of the file and print it
    //while reading a file's lines they will be stored in a StringX line buffer which we initialize here
    rfStringX_Init_txtbuff(&lineBuffer,"");
    while(rfTextFile_ReadLine(&f,&lineBuffer) == RF_SUCCESS)
    {
        printf("%s\n",rfString_ToCstr(&lineBuffer));
    }
//! [READ_LINES]
//! [CLEANUP]
    //finally close the text file and the stringx line buffer
    rfTextFile_Deinit(&f);
    rfStringX_Deinit(&lineBuffer);
//! [CLEANUP]

	return 0;
}