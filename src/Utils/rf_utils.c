
#include "../../include/rf_utils.h"

#include <string.h> //for memcpy

///Declaration of the utils globals
RF_UTILS rfUTILS;


// A function which detects system's endianess
void rfUTILS_DetectEndianess()
{
    int32_t anint = (int32_t)0xdeadbeef;
    rfUTILS.endianess = (*(char *)&anint == (char)0xef)? RF_LITTLE_ENDIAN : RF_BIG_ENDIAN;
}

//A function that returns the endianess of the system
char rfUTILS_Endianess()
{
    return rfUTILS.endianess;
}

// Swaps the endianness of the variable
void rfUTILS_SwapEndianUS(uint16_t* v)
{
    *v = (*v>>8)|(*v<<8);
}

//Swaps the endianness of the variable
void rfUTILS_SwapEndianS(int16_t* v)
{
    *v = ((*v&0xff00)>> 8) |((*v&0x00ff)<<8);
}

// Swaps the endianness of the variable
void rfUTILS_SwapEndianUI(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}
// Swaps the endianness of the variable
void rfUTILS_SwapEndianI(int32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}

// Swaps the endianness of the variable
void rfUTILS_SwapEndianUL(uint32_t*   v)
{
    *v = (*v>>24) | ((*v<<8) & 0x00FF0000) |
    ((*v>>8) & 0x0000FF00) | (*v<<24);
}


//Performs bubblesort to an array of bytes
void rfBubbleSort_c(char* arr,int32_t size)
{
    char done;
    int32_t i;
    do
    {
        done = true;
        for(i = 0; i < size; i ++)
        {
            //swap them if their i position is wrong
            if(arr[i]>arr[i+1])
            {
                char temp=arr[i];
                arr[i] = arr[i+1];
                arr[i+1]=temp;
                done = false;
            }
        }
    }while(done==false);
}

//Performs bubblesort to an array of unsigned shorts, sorting the array starting from the lowest value to the highest
void rfBubbleSort_us(uint16_t* arr,int32_t size)
{
    char done;
    int32_t i;
    do
    {
        done = true;
        for(i = 0; i < size; i ++)
        {
            //swap them if their i position is wrong
            if(arr[i]>arr[i+1])
            {
                uint16_t temp=arr[i];
                arr[i] = arr[i+1];
                arr[i+1]=temp;
                done = false;
            }
        }
    }while(done==false);
}

// Copies from the source to the destination from the start to the end bits. Make sure that the destination AND the source have a size of at least end/8 + 1 bytes
void rfCopyBits_Between(char* src,char* dest,uint16_t start,uint16_t end,uint16_t byteLength)
{
    int32_t i,startByte;
    char copyingStarted;
#ifdef REFU_PEDANTIC_MODE
    if(end < start)
    {
        uint16_t temp;
        LOG_ERROR("Tried to copy bits from a byte to another with illegal range. The end of the range is smaller than the start. Swapped the end\
	    with the start to make sense but please do check your code",RE_UTILS_COPYBITS_BETWEEN_RANGE);
        temp = start;
        start = end;
        end = temp;
    }
#endif

    //find the byte where the copying should begin
    startByte = (start-1)/8;
    copyingStarted = false;

    for(i = startByte; i < byteLength; i ++)
    {
        //if this the starting byte where we take bits from
        if(copyingStarted == false)
        {
            //the bits to copy for this byte. The +1 is to shift for correct amounts, since we start counting from the 1 and not from 0 as usually
            uint16_t destShift = (i*8+8)-start + 1; // the number of bits by which to right shift the destination byte
            uint16_t srcShift = 8 - destShift; // the number of bits by which to left shift the source byte

            //! @todo this applies only for little endian, gotta have an if clause here. Maybe move the makoto endianess detection from the utils to the refu utils
            dest[i] = (dest[i] & (0xFF>>destShift)) | (src[i] & (0xFF<<(srcShift) ));

            copyingStarted = true; //make sure we do not get in this if ever again
            //go to the next byte
            continue;
        }
        //if it's the last byte where copying must occur
        if((i*8)+8 >= end)
        {
            //the number of the bits to NOT copy.
            uint16_t srcShift = (i*8)+8-end; // the number of bits by which to right shift the source byte
            uint16_t destShift = 8 - srcShift; //the number of bits by which to left shift the destination byte


            //! @todo this applies only for little endian, gotta have an if clause here. Maybe move the makoto endianess detection from the utils to the refu utils
            dest[i] = (dest[i] & (0xFF<<(destShift))) | (src[i] & (0xFF>>(srcShift) ));

            //we are done, do not continue down to the rest
            break;
        }

        //else in any other case copy the whole byte
        memcpy(dest+i,src+i,sizeof(char));
    }
}



