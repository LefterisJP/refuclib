/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */

/**
 * An atomic int
 */
typedef struct atomic
{
    int val;
}RFatomic;

#define RF_ATOMIC_INIT(a_) {a_}

static inline int rf_atomic_inc_fetch(RFatomic* a)
{
    return __sync_add_and_fetch(a->val, 1);
}

static inline void rf_atomic_inc(RFAtomic* a)
{
    __sync_add_and_fetch(a->val, 1);
}

static inline int rf_atomic_dec_fetch(RFAtomic* a)
{
    return __sync_sub_and_fetch(a->val, 1);
}

static inline void rf_atomic_dec(RFatomic* a)
{
    __sync_sub_and_fetch(a->val, 1);
}
