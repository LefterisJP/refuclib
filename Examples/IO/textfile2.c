#include <RFtextfile.h>
#include <RFstring.h>
#include <refu.h>

//! [PRINT_LINES]
//A function to print all the lines of a text file
void printLines(RF_TextFile* t,RF_StringX* lineBuffer,const char* text)
{

    printf("\n||%s||\n",text);
    //move back to the beginning of the file
    rfTextFile_GoToLine(t,1);
    //explicitly changing the mode to read for the readlines to follow. not really needed
    rfTextFile_SetMode(t,RF_FILE_READ);
    //read and print all the file's lines
    while(rfTextFile_ReadLine(t,lineBuffer) == RF_SUCCESS)
    {
        printf("%s\n",rfString_ToCstr(lineBuffer));
    }
}
//! [PRINT_LINES]

int main()
{
    rfInit("errorLog","infoLog");
    RF_TextFile f;
    RF_StringX lineBuffer;
    int32_t i;

    //let's initialize the file handler
    rfTextFile_Init(&f,RFS_("testfile"),RF_FILE_NEW);
    //also initialize the buffer string
    rfStringX_Init_txtbuff(&lineBuffer,"");
    //write a few lines to the file
    for(i=0;i<5;i++)
        rfTextFile_Write(&f,RFS_("This is line number %d of testfile\n",i+1));

//! [FILE_INSERT]
    //insert 2 lines
    rfTextFile_Insert(&f,3,RFS_("I am inserted by \"rfTextFile_Insert()\" after line 3"),true);
    rfTextFile_Insert(&f,2,RFS_("I am inserted by \"rfTextFile_Insert()\" before line 2"),false);
    printLines(&f,&lineBuffer,"After inserting 2 lines");
//! [FILE_INSERT]

//! [FILE_REMOVE]
    //remove the first line of the file
    rfTextFile_Remove(&f,1);
    printLines(&f,&lineBuffer,"After removing the first line");
//! [FILE_REMOVE]

//! [FILE_REPLACE]
    //replace the two lines. Specifically they are the two inserted by insert
    rfTextFile_Replace(&f,1,RFS_("I am the replacement of line 1"));
    rfTextFile_Replace(&f,4,RFS_("I am the replacement of line 4"));
    printLines(&f,&lineBuffer,"After replacing two lines");
//! [FILE_REPLACE]

//! [FILE_MOVE]
    //move 3 lines back from the current position (end of file) and read 2 lines from there
    rfTextFile_MoveLines(&f,-3);
    printf("\n||Moved 3 lines back||\n");
    rfTextFile_ReadLine(&f,&lineBuffer);
    printf("\nReadLine():\"%s\"",rfString_ToCstr(&lineBuffer));
    rfTextFile_ReadLine(&f,&lineBuffer);
    printf("\nReadLine():\"%s\"",rfString_ToCstr(&lineBuffer));
//! [FILE_MOVE]

//! [CLEANUP]
    //finally close the text file and free the stringx line buffer
    rfTextFile_Deinit(&f);
    rfStringX_Deinit(&lineBuffer);
//! [CLEANUP]
    return 0;
}