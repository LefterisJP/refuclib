/* The original idea was taken from the CCAN project
 * http://ccodearchive.net/index.html
 * https://github.com/rustyrussell/ccan
 *
 * The original code was Licensed under LGPLv2+ - see LICENSE file for details 
 *
 * Modified quite a bit by Lefteris Karapetsas<lefteris@refu.co> to a point that
 * it's quite a different API. All additions are licensed under the same license
 * as the rest of the project (BSD3)
 */

#ifndef RF_DATASTRUCTURES_OBJSET_H
#define RF_DATASTRUCTURES_OBJSET_H
#include <Data_Structures/htable_type.h>
#include <Utils/hash.h>
#include <Utils/tcon.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static inline const void *default_objset_key_(const void *elem)
{
	return elem;
}
static inline size_t default_objset_hashfn_(const void *elem)
{
	return hash_pointer(elem, 0);
}
static inline bool default_objset_eqfn_(const void *e1, const void *e2)
{
	return e1 == e2;
}


/**
 * This is a an objset handler. Just a wrapper around hashtable
 */
struct objset_h {
    struct htable ht;
};

/**
 * rf_objset_iter - iterator reference.
 *
 * This is valid for a particular set as long as the contents remain unchaged,
 * otherwise the effect is undefined.
 */
struct rf_objset_iter {
    struct htable_iter i;
};

/**
 * Define a specific objset for a type. Heavily influenced by htable_type.h
 * @ref HTABLE_DEFINE_TYPE(). Same arguments apply but different/less functions
 * are generated
 */
#define OBJSET_DEFINE_TYPE(type, keyof, hashfn, eqfn)                   \
	static inline size_t objset_##type##_hash(const void *elem, void *priv) \
	{                                                                   \
		return hashfn(keyof((const type *)elem));                       \
	}                                                                   \
	static inline void objset_##type##_init(struct objset_h *set)       \
	{                                                                   \
		htable_init(&set->ht, objset_##type##_hash, NULL);              \
	}                                                                   \
	static inline bool objset_##type##_add(struct objset_h *set, const type *elem) \
	{                                                                   \
		return htable_add(&set->ht, hashfn(keyof(elem)), elem);         \
	}                                                                   \
	static inline bool objset_##type##_del(struct objset_h *set, const type *elem) \
	{                                                                   \
		return htable_del(&set->ht, hashfn(keyof(elem)), elem);         \
	}                                                                   \
	static inline type *objset_##type##_get(const struct objset_h *set, \
                                            const HTABLE_KTYPE(keyof) k) \
	{                                                                   \
		/* Typecheck for eqfn */                                        \
		(void)sizeof(eqfn((const type *)NULL,                           \
                          keyof((const type *)NULL)));                  \
		return htable_get(&set->ht,                                     \
                          hashfn(k),                                    \
                          (bool (*)(const void *, void *))(eqfn),       \
                          k);                                           \
	}                                                                   \
	static inline bool objset_##type##_delkey(struct objset_h *set,     \
                                              const HTABLE_KTYPE(keyof) k) \
	{                                                                   \
		type *elem = objset_##type##_get(set, k);                       \
		if (elem)                                                       \
			return objset_##type##_del(set, elem);                      \
		return false;                                                   \
	}                                                                   \
    static inline bool objset_##type##_subset(struct objset_h *set1,    \
                                              struct objset_h *set2)    \
    {                                                                   \
        struct htable_iter it1;                                         \
        type *elem;                                                     \
        htable_foreach(&set1->ht, &it1, elem) {                         \
            if (!objset_##type##_get(set2, elem)) {                     \
                return false;                                           \
            }                                                           \
        }                                                               \
        return true;                                                    \
    }


/**
 * OBJSET_MEMBERS - declare members for a type-specific unordered objset.
 * @type: type for this set's values, or void * for any pointer.
 *
 * You use this to create your own typed objset for a particular type.
 * You can use an integer type, *but* remember you can't use "0" as a
 * value!
 *
 * Example:
 *	struct objset_int {
 *		OBJSET_MEMBERS(int *);
 *	};
 */
#define OBJSET_MEMBERS(type)        \
	struct objset_h raw;            \
	TCON(type canary)

/**
 * Initialize an rf_objset
 *
 * @param set_        The set_ to initialize. Must be of a type containing OBJSET_MEMBERS
 * @param type_       Give the type of pointer objects the set is holding
 */
#define rf_objset_init(set_, type_) objset_##type_##_init(&(set_)->raw)
#define rf_objset_init_default(set_) objset_void_init(&(set_)->raw)

/**
 * rf_objset_empty - is this set empty?
 * @set: the typed objset to check.
 *
 * Example:
 *	if (!rf_objset_empty(&set))
 *		abort();
 */
#define rf_objset_empty(set) objset_empty_(&(set)->raw)
static inline bool objset_empty_(const struct objset_h *set)
{
    struct htable_iter it;
	return htable_first(&set->ht, &it) == NULL;
}

/**
 * Get a value from a set
 * @param set_        The set_ to get from. Must be of a type containing OBJSET_MEMBERS
 * @param type_       Give the type of pointer objects the set is holding
 * @param value_      the (non-NULL) object to search for
 *
 * Returns the value, or NULL if it isn't in the set (and sets errno = ENOENT).
 */
#define rf_objset_get(set_, type_, value_)                              \
	tcon_cast((set_), canary, objset_##type_##_get(&(set_)->raw, (value_)))
#define rf_objset_get_default(set_, value_)                             \
	tcon_cast((set_), canary, ojset_void_get(&(set_)->raw, (value_)))


bool i_rf_objset_add(struct objset_h *set, void *value);
/**
 * Add a member to the set
 *
 * @param set_        The set_ to add to. Must be of a type containing OBJSET_MEMBERS
 * @param type_       Give the type of pointer objects the set is holding
 * @param value_      the (non-NULL) object to place in the set.
 *
 * This returns false if we run out of memory and true if
 * all went fine or if the object was already in the set.
 */
#define rf_objset_add(set_, type_, value_)                              \
    objset_##type_##add(&tcon_check((set_), canary, (value_))->raw, (void *)(value_))
#define rf_objset_add_default(set_, value_)                             \
    objset_void_add(&tcon_check((set_), canary, (value_))->raw, (void *)(value_))


/**
 * Remove a member from the set.
 *
 * @param set_        The set to remove from. Must be of a type containing OBJSET_MEMBERS
 * @param type_       Give the type of pointer objects the set is holding
 * @param value_      the (non-NULL) object to find and remove from the set.
 *
 * This returns false NULL if @value was not in the set (and sets
 * errno = ENOENT).
 */
#define rf_objset_del(set_, type_, value_)                            \
	objset_##type_##_del(&tcon_check((set_), canary, value_)->raw,    \
                         (const void *)value_)
#define rf_objset_del_default(set_, value_)                           \
	objset_void_del(&tcon_check((set_), canary, value_)->raw,         \
                    (const void *)value_)

/**
 * Remove all members from the set and clear its memory
 *
 * @param set_     The set_ to clear
 */
#define rf_objset_clear(set) htable_clear(&(set)->raw.ht)


/**
 * Get the first element in the set and initialize an iterator
 *
 * @param set_     the typed objset to iterate through.
 *                 Must be of a type containing OBJSET_MEMBERS
 * @param it_      a struct rf_objset_iter to use as an iterator.
 *
 */
#define rf_objset_first(set_, it_) \
	tcon_cast((set_), canary, htable_first(&(set_)->raw.ht, &(it_)->iter))

/**
 * Get another element in the set using a previously initialized iterator
 * @param set_     the typed objset to iterate through.
 *                 Must be of a type containing OBJSET_MEMBERS
 * @param it_      a struct rf_objset_iter to use as an iterator.
 *                 Must have been set by a successful rf_objset_first() or
 *                 rf_objset_next() call.
 */
#define rf_objset_next(set_, it_)                                       \
	tcon_cast((set_), canary, htable_next(&(set)->raw.ht, &(it_)->iter))

/**
 * Iiterate all the elements in the set
 *
 * @param set_     the typed objset to iterate through.
 *                 Must be of a type containing OBJSET_MEMBERS
 * @param it_      a struct rf_objset_iter to use as an iterator.
 * @value_:        A typed pointer to hold the value at each iteration
 *
 */
#define rf_objset_foreach(set_, it_, value_)        \
    for (value_ = rf_objset_first((set_), (it_));   \
         value_;                                    \
         value_ = rf_objset_next((set_), (it_)))

/**
 * Checks if a set is a subset of another
 * @param set1_    The left part of ⊆
 * @param set2_    The right part of ⊆
 * @param type_    Give the type of pointer objects the set is holding
 * @return         True if all elements of set1 also exists in set2
 */
#define rf_objset_subset(set1_, set2_, type_)           \
    objset_##type_##_subset(&(set1)->raw, &(set2)->raw)
#define rf_objset_subset_default(set1_, set2_)          \
    objset_void_subset(&(set1)->raw, &(set2)->raw)

/**
 * Checks if two sets are equal
 * @param set1_    The left part of equality
 * @param set2_    The right part of equality
 * @param type_    Give the type of pointer objects the set is holding
 * @return         True if all elements of set1 also exists in set2 and vice versa
 */
#define rf_objset_equal(set1_, set2_, type_)                \
    (objset_##type_##_subset(&(set1)->raw, &(set2)->raw) && \
     objset_##type_##_subset(&(set2)->raw, &(set1)->raw))
#define rf_objset_equal_default(set1_, set2_)                \
    (rf_objset_subset_default(&(set1)->raw, &(set2)->raw) && \
     rf_objset_subset_default(&(set2)->raw, &(set1)->raw))



// define functions for the the default set type which is simple pointer comparison
OBJSET_DEFINE_TYPE(void,
                   default_objset_key_,
                   default_objset_hashfn_,
                   default_objset_eqfn_)

#endif /* DATA_STRUCTURES_OBJSET_H */
