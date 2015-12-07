/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_DLIB_H
#define RF_DLIB_H

#include <stdbool.h>

struct rf_dlib;
struct RFstring;

/**
 * Tries to find and load a dynamic library
 *
 * @param path           An optional path in which to find the library. If not
 *                       given then the standard system paths are searched.
 * @param name           The name of the library without the suffix.
 * @return               Returns the library handle if succesfull and NULL otherwise
 */
struct rf_dlib *rf_dlib_create(const struct RFstring *path, const struct RFstring *name);

/**
 * Frees up a previously created dlib.
 * @return true in succesfull freeing and false otherwise
 */
bool rf_dlib_destroy(struct rf_dlib *dl);

/**
 * Searches for and returns a symbol in a dlib
 *
 * @param dl            The dynamic library in which to search for the symbol
 * @param name          The name of the symbol to return
 * @return              A pointer to the symbol if found, and NULL otherwise.
 */
void *rf_dlib_symbol(struct rf_dlib *dl, const struct RFstring *name);

#endif
