/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <Data_Structures/binaryarray.h>
/*------------- Outside Module inclusion -------------*/
//for error logging
    #include <Utils/log.h>
//for memory allocation macros
    #include <Utils/memory.h> //for refu memory allocation
/*------------- libc inclusion --------------*/
#include <string.h> //for memcpy
/*------------- End of includes -------------*/

// Allocates and returns a binary array
RFbinary_array *rf_binaryarray_create(uint32_t size)
{
    RFbinary_array *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (rf_binaryarray_init(ret, size) == false) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
// Initializes a binary array
bool rf_binaryarray_init(RFbinary_array *arr,uint32_t size)
{
    //callocate enough bytes to encompass all of the values.
    RF_CALLOC(arr->data, size/8 + 1, 1, return false);
    arr->size = size;
    return true;
}

//Copies RFbinarry_array @c src into RFbinary_array @c dst
bool rf_binaryarray_copy_in(RFbinary_array *dst, RFbinary_array *src)
{
    dst->size = src->size;
    RF_CALLOC(dst->data, dst->size/8 + 1, 1, return false);
    memcpy(dst->data, src->data, dst->size/8 + 1);
    return true;
}

//Creates and returns an allocated copy of the given RFbinarry_array
RFbinary_array *rf_binaryarray_copy_out(RFbinary_array *src)
{
    RFbinary_array *dst;
    RF_MALLOC(dst, sizeof(*dst), return NULL);
    if (!rf_binaryarray_copy_in(dst, src)) {
        free(dst);
        dst = NULL;
    }
    return dst;
}

// Destroys a binary array freeing its memory
void rf_binaryarray_destroy(RFbinary_array *a)
{
    free(a->data);
    free(a);
}
// Destroys a binary array but without freeing its memory
void rf_binaryarray_deinit(RFbinary_array *a)
{
    free(a->data);
}

// Gets a specific value of the array
bool rf_binaryarray_get(RFbinary_array *a, uint32_t i, char* val)
{
    char b;
    //check for out of bounds index
    if (i >= a->size) {
        RF_ERROR(
                 "Attempted to retrieve a value from a BinaryArray with an "
                 "index out of bounds");
        return false;
    }

    //the specific bit of the byte we wanna check for
    b = 0x1 << (i%8);
    //check if the bit is set or not
    *val = (b == (a->data[i/8] & b));
    //success
    return true;
}

// Gets a specific value of the array, without checking for array index out of bounds. If the index IS out of bounds the value can not be trusted. For a safer function look at #rf_binaryarray_get
bool rf_binaryarray_get_n_c(RFbinary_array *a, uint32_t i)
{
    //the specific bit of the byte we wanna check for
    char b = 0x1 << (i%8);
    //return the value of the bit
    return (b == (a->data[i/8] & b));
}

// Sets a specific value of the binary array.
bool rf_binaryarray_set(RFbinary_array *a, uint32_t i, char val)
{
    //check for out of bounds index
    if(i >= a->size) {
        RF_ERROR(
            "Attempted to set a value of a BinaryArray with an index out "
            "of bounds");
        return false;
    }

    //if the given value is true set the bit
    if (val) {
        a->data[i/8] |= (0x1 << (i%8));
        return true;
    }

    //else unset the bit
    a->data[i/8] &= ~(0x1 << (i%8));
    return true;
}

// Sets a specific value of the binary array without making an index out of bounds check. Use only if you are certain that the given index is not out of bounds!!!!
void rf_binaryarray_set_n_c(RFbinary_array *a, uint32_t i, char val)
{
    //if the given value is true set the bit
    if (val == true) {
        a->data[i/8] |= (0x1 << (i%8));
        return ;
    }

    //else unset the bit
    a->data[i/8] &= ~(0x1 << (i%8));
}


// Increases the size of a binary array
bool rf_binaryarray_reallocate(RFbinary_array *a, uint32_t newSize)
{
    //attempt to realloc
    RF_REALLOC(a->data, char, newSize/8 + 1, return false);
    //check to see if the array got reallocated upwards
    if (newSize > a->size) {
        ///if it did we have to initialize all the new values to 0
        unsigned char j;
        unsigned char sBit = a->size%8;
        //get the difference in size
        uint32_t diff = newSize-a->size;

        //set the remaining bits of the byte to 0 (if any)
        for (j = sBit ;j < 8; j ++) {
            a->data[a->size/8] &= ~(0x1 << (j%8));
        }

        //find the remaining bits of the byte and check if the difference in size exceeds them
        if (diff > (unsigned char)(8-sBit)) {
            //then we also have additional bytes which we need to make sure are equal to 0
            for (j = a->size/8 + 1; j <= newSize/8; j++ ) {
                a->data[j] = 0;
            }
        }
    }

    //finally save the new size
    a->size = newSize;

    return true;
}
