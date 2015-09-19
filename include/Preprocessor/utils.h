/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_PREPROCESSOR_UTILS_H
#define RF_PREPROCESSOR_UTILS_H

/**
 * Turn any C-expression into a string literal
 */
#define RF_STRINGIFY(i_x_) i_rf_stringify(i_x_)
#define i_rf_stringify(i_x_) #i_x_

#endif
