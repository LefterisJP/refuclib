#include <RFthread.h>
#include <RFstring.h>
#include <RFtextfile.h>
#include <refu.h>

char condition = false;
/**
 ** [THREAD_DATA]
 **
 **/
//Packing all the data to give as input to the Thread in a structure
typedef struct i_Thread_Data
{
    //The textfile handle for this thread
    RF_TextFile f;
    //Just a number to give to the thread for the printfs
    uint8_t num;
    //The line until which this thread will read
    uint64_t until;
    //Something for the string to return. For this example's sake assume it's a string that we seek
    //from inside the text file
    RF_String result;
} i_Thread_Data;
/**
 ** [THREAD_DATA]
 **
 **/

/**
 ** [THREAD_FUNCTION]
 **
 **/
void* singleThreadQuery(void* dataP)
{
    //get the data for this thread
    i_Thread_Data* data = (i_Thread_Data*)dataP;
    printf("Got in thread's %d function\n",data->num);
    //init a big enough StringX
    RF_StringX line;
    rfStringX_Init_buff(&line,4097,"");
    //keep reading lines from the file until either EOF or until we pass the lines that were requested to be read
    while(rfTextFile_ReadLine(&data->f,&line) == RF_SUCCESS &&  data->f.line < data->until)
    {
        //if we get to a specific part of the file that satisfies a given condition get the result
        //(for the example the condition is always false), just to keep the size small
        if(condition)
            rfString_After(&line,RFS_("\"result:\""),&data->result);
    }
    //finished parsing
    rfStringX_Deinit(&line);
    printf("Got out of thread's %d function\n",data->num);
    return 0;
}
/**
 ** [THREAD_FUNCTION]
 **
 **/

int main()
{
    int i;
    rfInit("errorLog","infoLog");
/**
 ** [INIT_THREAD_DATA]
 **
 **/
    //assume that we already parsed the file before and divided it into 6 equal parts
    i_Thread_Data data[6];
    RF_Thread threads[6];
    //initialize the 6 different files and take their respective file pointers to the 6 specific
    // positions. We assumed we divided it beforehand so we know the file offset and line for each position (e.g.: line 100000 -> offset 54565573)
    rfTextFile_Init(&data[0].f,RFS_("Big File"),RF_FILE_READ,RF_UTF8);
    data[0].until = 100000;
    rfTextFile_Init(&data[1].f,RFS_("Big File"),RF_FILE_READ,RF_UTF8);
    data[1].until = 200000; rfTextFile_GoToOffset(&data[1].f,54565573,SEEK_SET);
    rfTextFile_Init(&data[2].f,RFS_("Big File"),RF_FILE_READ,RF_UTF8);
    data[2].until = 300000; rfTextFile_GoToOffset(&data[2].f,110317746,SEEK_SET);
    rfTextFile_Init(&data[3].f,RFS_("Big File"),RF_FILE_READ,RF_UTF8);
    data[3].until = 400000; rfTextFile_GoToOffset(&data[3].f,166363980,SEEK_SET);
    rfTextFile_Init(&data[4].f,RFS_("Big File"),RF_FILE_READ,RF_UTF8);
    data[4].until = 500000; rfTextFile_GoToOffset(&data[4].f,223935430,SEEK_SET);
    rfTextFile_Init(&data[5].f,RFS_("Big File"),RF_FILE_READ,RF_UTF8);
    data[5].until = 600000; rfTextFile_GoToOffset(&data[5].f,82055537,SEEK_SET);
/**
 ** [INIT_THREAD_DATA]
 **
 **/

/**
 ** [INIT_THREADS]
 **
 **/
    //initialize the threads with the function
    for(i =0; i < 6; i++)
    {
        data[i].num = i+1;
        rfThread_Init(&threads[i],0,&singleThreadQuery,&data[i],100);
    }
/**
 ** [INIT_THREADS]
 **
 **/

/**
 ** [MAIN_THREAD_WAIT]
 **
 **/
    //now the main thread should wait until all of the other threads are done
    for(i =0; i < 6; i ++)
        if(rfThread_Join(&threads[i]) != RF_SUCCESS)
            exit(-1); //error
/**
 ** [MAIN_THREAD_WAIT]
 **
 **/
    printf("Main thread succesfully waited for all the working threads\n");
    //clean the thread data
    for(i = 0; i < 6; i++)
        rfTextFile_Deinit(&data[i].f);

    return 0;
}
