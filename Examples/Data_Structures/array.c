#include <Data_Structures/rfc_array.h>
#include <IO/rfc_textfile.h>

//! [PRINT_ARRAY]
void printArray(RF_ArrayV* array)
{
    uint32_t i;
    RF_String s;
    printf("\n\n\n");
    //print the string of the "i" index of the array if there is an element there
    for(i =0;i<array->size; i ++)
    {
        if(rfArrayV_Get(array,i,&s) == RF_SUCCESS)
            printf("Array[%d]=\"%s\"\n",i,rfString_ToCstr(&s));
    }
}
//! [PRINT_ARRAY]

int main()
{
    RF_TextFile f;
    uint32_t i = 0;
    RF_ArrayV array;
    RF_StringX buffer;
    RF_String addition;
    rfInit("errorLog","infoLog");

//! [INIT_STUFF]
    //initialize the text file
    rfTextFile_Init(&f,RFS_("testfile"),RF_FILE_READ,RF_UTF8);
    //initialize the array with 4 elements initial size since we know that the file has 4 lines
//! [ARRAY_INIT]
    rfArrayV_Init(&array,RF_String,4);
//! [ARRAY_INIT]
    //initialize the file reading StringX buffer
    rfStringX_Init_buff(&buffer,256,"");
    //initialize an addition string
    rfString_Init(&addition,"I am an addition string");
//! [INIT_STUFF]

//! [READ_LINES]
    //read each line of the file
    while(rfTextFile_ReadLine(&f,&buffer) == RF_SUCCESS)
    {
        //set a copy of the buffer string at the "i" index of the array
        rfArrayV_Set_copy(&array,i,&buffer,true);
        i++;
    }
    printArray(&array);
//! [READ_LINES]

//! [REMOVE]
    //showcasing removal from array
    rfArrayV_Remove(&array,0);
    rfArrayV_Remove(&array,3);
    printArray(&array);
//! [REMOVE]

//! [REALLOC]
    //let's say we need to add 2 more strings in indices bigger than 4
    //we can accomplish that by reallocating the array
    rfArrayV_Reallocate(&array,6);
    //set another string to the 5th and 6th array slot
    rfArrayV_Set(&array,4,&addition,true);
    //notice the setting of a copy to the 6th slot, because if the same string was set in that slot
    //the same string would be attempted to be destroyed twice later, resulting in undefined behaviour
    rfArrayV_Set_copy(&array,5,&addition,true);
    //print to see the result
    printArray(&array);
//! [REALLOC]

//! [CLEANUP]
    //finally cleanup the array and all the other stuff
    rfArrayV_Deinit(&array);
    rfStringX_Deinit(&buffer);
    rfTextFile_Deinit(&f);
//! [CLEANUP]
    return 0;
}
