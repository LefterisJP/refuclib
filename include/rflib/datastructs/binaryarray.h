/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_BINARY_ARRAY_H
#define RF_BINARY_ARRAY_H

#include <rflib/defs/imex.h>
#include <rflib/defs/types.h>
#include <rflib/defs/retcodes.h>
#include <rflib/datastructs/binaryarray_decl.h>

#ifdef __cplusplus
extern "C"
{///opening bracket for calling from C++
#endif


/**
 * @memberof RFbinary_array
 * @brief Allocates and returns a binary array. All initial values are 0 (false)
 *
 * @param size The size of the array in values
 * @return Returns the initialized binary array
 */
i_DECLIMEX_ RFbinary_array *rf_binaryarray_create(uint32_t size);
/**
 * @memberof RFbinary_array
 * @brief Initializes a binary array. All initial values are 0 (false)
 *
 * @param arr The array to initialize
 * @param size The size of the array in values
 * @return Returns @c true in success and @c false for failure
 */
i_DECLIMEX_ bool rf_binaryarray_init(RFbinary_array *arr, uint32_t size);

/**
 * @memberof RFbinary_array
 * @brief Copies RFbinarry_array @c src into RFbinary_array @c dst
 *
 * @param dst The new binarry array copy to create
 * @param src The binary array to copy from
 * @return Returns @c true in success and @c false otherwise
 */
i_DECLIMEX_ bool rf_binaryarray_copy_in(RFbinary_array *dst,
                                        RFbinary_array *src);
/**
 * @memberof RFbinary_array
 * @brief Creates and returns an allocated copy of the given RFbinarry_array
 *
 * @param src The Binary array to copy
 * @return An allocated RFbinary_array copy of the given binary array,
 * or @c NULL in error
 */
i_DECLIMEX_ RFbinary_array *rf_binaryarray_copy_out(RFbinary_array *src);

/**
 * @memberof RFbinary_array
 * @brief Destroys a binary array freeing its memory
 *
 * Call this for arrays initialized with _Create
 * @param a The binary array to destroy
 */
i_DECLIMEX_ void rf_binaryarray_destroy(RFbinary_array *a);
/**
 * @memberof RFbinary_array
 * @brief Destroys a binary array but not freeing own memory
 *
 * Call this for arrays initialized with _Init
 * @param a The binary array to destroy
 */
i_DECLIMEX_ void rf_binaryarray_deinit(RFbinary_array *a);

/**
 * @memberof RFbinary_array
 * @brief Gets a specific value of the array
 * @param[in] a The binary array from where to get the value
 * @param[in] i The index of the value to get
 * @param[out] val Pass a reference to a char to get the value here
 * @return Returns true if the value exists and false if the requested index is out of bounds
 */
i_DECLIMEX_ bool rf_binaryarray_get(RFbinary_array *a, uint32_t i,
                                   char* val);

/**
 * @memberof RFbinary_array
 * @brief Gets a specific value of the array, no checking.
 *
 * This function gets a value without checking for array index out of bounds. If the index IS out of bounds the value can not be trusted and
 * a segmentation fault can also happen. For a safer function look at #rf_binaryarray_get that performs the check.
 * @warning  NEVER use this function if you are not sure wether the index is out of bounds or not
 * @param a The binary array from where to get the value
 * @param i The index of the value to get
 * @return Returns true if the value is set and false if it is not set
 *
 */
i_DECLIMEX_ bool rf_binaryarray_get_n_c(RFbinary_array *a, uint32_t i);

/**
 * @memberof RFbinary_array
 * @brief Sets a specific value of the binary array.
 * @param a The binary array from where to get the value
 * @param i The index of the value to set
 * @param val The value to give. Either 1 or 0.
 * @return Returns true if the value was succesfully set or false if the given index was out of bounds
 *
 */
i_DECLIMEX_ bool rf_binaryarray_set(RFbinary_array *a, uint32_t i, char val);

/**
 * @memberof RFbinary_array
 * @brief Sets a specific value of the binary array, no checking.
 *
 * This function sets a value without making an index out of bounds check. Use only if you are certain that the given index is not out of bounds!!!!
 * @warning  NEVER use this function if you are not sure wether the index is out of bounds or not
 * @param a The binary array from where to get the value
 * @param i The index of the value to set
 * @param val The value to give. Either 1 or 0.
 *
 */
i_DECLIMEX_ void rf_binaryarray_set_n_c(RFbinary_array *a, uint32_t i,
                                      char val);

/**
 * @memberof RFbinary_array
 * @brief Increases or decreases the size of a binary array.
 * @param a The binary array to reallocate
 * @param newSize The new size in values that the array should have.
 * @return Returns @c true for succesfull reallocation and @c false
 *  otherwise.
 *
 */
i_DECLIMEX_ bool rf_binaryarray_reallocate(RFbinary_array *a,
                                          uint32_t newSize);

#ifdef __cplusplus
}///closing bracket for calling from C++
#endif


#endif//include guards end
