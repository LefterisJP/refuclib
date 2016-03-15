/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_UTILS_BITS_H
#define RF_UTILS_BITS_H


/**
 * Checks if the bit @c BIT_ is set in the variable @c VAR_
 */
#define RF_BIT_ON(VAR_,BIT_)    ((VAR_) & (1<<(BIT_)))
/**
 * Checks if the bitflag @c FLAG_ is set in the variable @c VAR_
 */
#define RF_BITFLAG_ON(VAR_, FLAG_)    (((VAR_) & (FLAG_)) != 0)
/**
 * Sets the bitflag @c FLAG_ in the variable @c VAR_
 */
#define RF_BITFLAG_SET(VAR_, FLAG_)  (VAR_) |= (FLAG_)
/**
 * Unsets the bitflag @c FLAG_ in the variable @c VAR_
 */
#define RF_BITFLAG_UNSET(VAR_, FLAG_)  (VAR_) &= (~(FLAG_))


//this is not used anywhere yet (why is it here?)
/**
 * Gets the value (0) or (1) of the __n__ bit of __byte__
 *
 */
#define RF_GET_BIT(__byte__,__n__)  ((__byte__& (0x01<<__n__) )>>__n__ )


#endif//include guards end
