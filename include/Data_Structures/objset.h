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
    struct htable_iter it;
};

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
 * Define a specific objset for a type. Heavily influenced by htable_type.h
 * @ref HTABLE_DEFINE_TYPE(). Same arguments apply but different/less functions
 * are generated
 */
#define OBJSET_DEFINE_TYPE(name, type, keyof, hashfn, eqfn)             \
    struct rf_objset_##name { OBJSET_MEMBERS(type *); };                \
	static inline size_t objset_##name##_hash(const void *elem, void *priv) \
	{                                                                   \
		return hashfn(keyof((const type *)elem));                       \
	}                                                                   \
	static inline void objset_##name##_init(struct objset_h *set)       \
	{                                                                   \
		htable_init(&set->ht, objset_##name##_hash, NULL);              \
	}                                                                   \
	static inline bool objset_##name##_del(struct objset_h *set, const type *elem) \
	{                                                                   \
		return htable_del(&set->ht, hashfn(keyof(elem)), elem);         \
	}                                                                   \
	static inline type *objset_##name##_get(const struct objset_h *set, \
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
	static inline bool objset_##name##_add(struct objset_h *set, const type *elem) \
	{                                                                   \
        type *e = objset_##name##_get(set, elem);                       \
		return e || htable_add(&set->ht, hashfn(keyof(elem)), elem);    \
	}                                                                   \
	static inline bool objset_##name##_delkey(struct objset_h *set,     \
                                              const HTABLE_KTYPE(keyof) k) \
	{                                                                   \
		type *elem = objset_##name##_get(set, k);                       \
		if (elem)                                                       \
			return objset_##name##_del(set, elem);                      \
		return false;                                                   \
	}                                                                   \
    static inline bool objset_##name##_subset(const struct objset_h *set1, \
                                              const struct objset_h *set2) \
    {                                                                   \
        struct htable_iter it1;                                         \
        type *elem;                                                     \
        htable_foreach(&set1->ht, &it1, elem) {                         \
            if (!objset_##name##_get(set2, elem)) {                     \
                return false;                                           \
            }                                                           \
        }                                                               \
        return true;                                                    \
    }

/**
 * Initialize an rf_objset
 *
 * @param set_        The set_ to initialize. Must be of a type containing OBJSET_MEMBERS
 * @param name_       Give the name with which you described the type of the pointer
 *                    when calling OBJSET_DEFINE_TYPE()
 */
#define rf_objset_init(set_, name_) objset_##name_##_init(&(set_)->raw)
#define rf_objset_init_default(set_) objset_void_init(&(set_)->raw)

/**
 * Check if the set is empty
 * @param set_     the typed objset to check.
 * @return         True if the set has no elements and false otherwise
 */
#define rf_objset_empty(set) i_objset_empty_(&(set)->raw)
bool i_objset_empty_(const struct objset_h *set);

/**
 * Get the number of elements in the set
 * It can get expensive since it iterates over the set
 * @param set_    The set whose number of elements to get
 * @return        The number of elements stored in the set
 */
#define rf_objset_size(set_) i_objset_size_(&(set_)->raw)
unsigned int i_objset_size_(const struct objset_h *set);


/**
 * Get a value from a set
 * @param set_        The set_ to get from. Must be of a type containing OBJSET_MEMBERS
 * @param name_       Give the name with which you described the type of the pointer
 *                    when calling OBJSET_DEFINE_TYPE()
 * @param value_      the (non-NULL) object to search for
 * @return            the value, or NULL if it isn't in the set
 */
#define rf_objset_get(set_, name_, value_)                              \
	tcon_cast((set_), canary, objset_##name_##_get(&(set_)->raw, (value_)))
#define rf_objset_get_default(set_, value_)                             \
	tcon_cast((set_), canary, objset_void_get(&(set_)->raw, (value_)))


bool i_rf_objset_add(struct objset_h *set, void *value);
/**
 * Add a member to the set
 *
 * @param set_        The set_ to add to. Must be of a type containing OBJSET_MEMBERS
 * @param name_       Give the name with which you described the type of the pointer
 *                    when calling OBJSET_DEFINE_TYPE()
 * @param value_      the (non-NULL) object to place in the set.
 * @return            false if we run out of memory and true if all went fine
 *                    or if the object was already in the set.
 */
#define rf_objset_add(set_, name_, value_)                              \
    objset_##name_##_add(&tcon_check((set_), canary, (value_))->raw, (void *)(value_))
#define rf_objset_add_default(set_, value_)                             \
    objset_void_add(&tcon_check((set_), canary, (value_))->raw, (void *)(value_))


/**
 * Remove a member from the set.
 *
 * @param set_        The set to remove from. Must be of a type containing OBJSET_MEMBERS
 * @param name_       Give the name with which you described the type of the pointer
 *                    when calling OBJSET_DEFINE_TYPE()
 * @param value_      the (non-NULL) object to find and remove from the set.
 * @return            false if @value was not in the set
 */
#define rf_objset_del(set_, name_, value_)                            \
	objset_##name_##_del(&tcon_check((set_), canary, value_)->raw,    \
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
	tcon_cast((set_), canary, htable_first(&(set_)->raw.ht, &(it_)->it))

/**
 * Get another element in the set using a previously initialized iterator
 * @param set_     the typed objset to iterate through.
 *                 Must be of a type containing OBJSET_MEMBERS
 * @param it_      a struct rf_objset_iter to use as an iterator.
 *                 Must have been set by a successful rf_objset_first() or
 *                 rf_objset_next() call.
 */
#define rf_objset_next(set_, it_)                                       \
	tcon_cast((set_), canary, htable_next(&(set_)->raw.ht, &(it_)->it))

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
 * @param name_       Give the name with which you described the type of the pointer
 *                    when calling OBJSET_DEFINE_TYPE()
 * @return         True if all elements of set1 also exists in set2. That means
 *                 for the empty set we will always get true.
 */
#define rf_objset_subset(set1_, set2_, name_)               \
    objset_##name_##_subset(&(set1_)->raw, &(set2_)->raw)
#define rf_objset_subset_default(set1_, set2_)          \
    objset_void_subset(&(set1_)->raw, &(set2_)->raw)

/**
 * Checks if two sets are equal
 * @param set1_    The left part of equality
 * @param set2_    The right part of equality
 * @param name_       Give the name with which you described the type of the pointer
 *                    when calling OBJSET_DEFINE_TYPE()
 * @return         True if all elements of set1 also exists in set2 and vice versa
 */
#define rf_objset_equal(set1_, set2_, name_)    \
    (rf_objset_subset(set1_, set2_, name_) &&   \
     rf_objset_subset(set2_, set1_, name_))
#define rf_objset_equal_default(set1_, set2_)   \
    (rf_objset_subset_default(set1_, set2_) &&  \
     rf_objset_subset_default(set2_, set1_))



// define functions for the the default set type which is simple pointer comparison
OBJSET_DEFINE_TYPE(void,
                   void,
                   default_objset_key_,
                   default_objset_hashfn_,
                   default_objset_eqfn_)

#endif /* DATA_STRUCTURES_OBJSET_H */
