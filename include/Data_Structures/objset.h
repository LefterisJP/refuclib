/* Taken from the CCAN project:
 * http://ccodearchive.net/index.html
 * https://github.com/rustyrussell/ccan
 *
 * Modified quite a bit by Lefteris Karapetsas<lefteris@refu.co>
 * Both adding functionality and fixing logic bugs.
 */
/*  Licensed under LGPLv2+ - see LICENSE file for details */
#ifndef RF_DATASTRUCTURES_OBJSET_H
#define RF_DATASTRUCTURES_OBJSET_H
#include <Data_Structures/htable_type.h>
#include <Utils/hash.h>
#include <Utils/tcon.h>
#include <stdlib.h>
#include <stdbool.h>

static inline const void *objset_key_(const void *elem)
{
	return elem;
}
static inline size_t objset_hashfn_(const void *elem)
{
	return hash_pointer(elem, 0);
}
static inline bool objset_eqfn_(const void *e1, const void *e2)
{
	return e1 == e2;
}
HTABLE_DEFINE_TYPE(void, objset_key_, objset_hashfn_, objset_eqfn_, objset_h);

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
#define OBJSET_MEMBERS(type)			\
	struct objset_h raw;			\
	TCON(type canary)

/**
 * rf_objset_init - initialize an empty objset
 * @set: the typed objset to initialize.
 *
 * Example:
 *	struct objset_int set;
 *
 *	rf_objset_init(&set);
 */
#define rf_objset_init(set) objset_h_init(&(set)->raw)

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
	struct objset_h_iter i;
	return objset_h_first(set, &i) == NULL;
}

/**
 * rf_objset_get - get a value from a set
 * @set: the typed objset to search.
 * @value: the value to search for.
 *
 * Returns the value, or NULL if it isn't in the set (and sets errno = ENOENT).
 *
 * Example:
 *	if (rf_objset_get(&set, val));
 *		printf("hello => %i\n", *val);
 */
#define rf_objset_get(set, member) \
	tcon_cast((set), canary, objset_h_get(&(set)->raw, (member)))

bool i_rf_objset_add(struct objset_h *set, void *value);
/**
 * rf_objset_add - place a member into the set.
 * @set: the typed objset to add to.
 * @value: the (non-NULL) object to place in the set.
 *
 * This returns false if we run out of memory and true if
 * all went fine or if the object was already in the set.
 *
 * Example:
 *	int *val;
 *
 *	val = malloc(sizeof *val);
 *	*val = 17;
 *	if (!rf_objset_add(&set, val))
 *		printf("error?\n");
 */
#define rf_objset_add(set, value)                                       \
	i_rf_objset_add(&tcon_check((set), canary, (value))->raw, (void *)(value))


/**
 * rf_objset_del - remove a member from the set.
 * @set: the typed objset to delete from.
 * @value: the value (non-NULL) to remove from the set
 *
 * This returns false NULL if @value was not in the set (and sets
 * errno = ENOENT).
 *
 * Example:
 *	if (!rf_objset_del(&set, val))
 *		printf("val was not in the set?\n");
 */
#define rf_objset_del(set, value)                           \
	objset_h_del(&tcon_check((set), canary, value)->raw,    \
		     (const void *)value)

/**
 * rf_objset_clear - remove every member from the set.
 * @set: the typed objset to clear.
 *
 * The set will be empty after this.
 *
 * Example:
 *	objset_clear(&set);
 */
#define rf_objset_clear(set) objset_h_clear(&(set)->raw)

/**
 * rf_objset_iter - iterator reference.
 *
 * This is valid for a particular set as long as the contents remain unchaged,
 * otherwise the effect is undefined.
 */
struct rf_objset_iter {
	struct objset_h_iter iter;
};

/**
 * rf_objset_first - get an element in the set
 * @set: the typed objset to iterate through.
 * @i: a struct objset_iter to use as an iterator.
 *
 * Example:
 *	struct rf_objset_iter i;
 *	int *v;
 *
 *	v = rf_objset_first(&set, &i);
 *	if (v)
 *		printf("One value is %i\n", *v);
 */
#define rf_objset_first(set, i) \
	tcon_cast((set), canary, objset_h_first(&(set)->raw, &(i)->iter))

/**
 * rf_objset_next - get the another element in the set
 * @set: the typed objset to iterate through.
 * @i: a struct objset_iter to use as an iterator.
 *
 * @i must have been set by a successful objset_first() or
 * objset_next() call.
 *
 * Example:
 *	while (v) {
 *		v = rf_objset_next(&set, &i);
 *		if (v)
 *			printf("Another value is %i\n", *v);
 *	}
 */
#define rf_objset_next(set, i)                                          \
	tcon_cast((set), canary, objset_h_next(&(set)->raw, &(i)->iter))

/**
 * rf_objset_foreach - iterate all the elements in the set
 * @set_:    the typed objset to iterate through
 * @value_:  A typed pointer to hold the value at each iteration
 * @it_:     an rf_objset_iter to use as an iterator
 *
 * added by Lefteris Karapetsas<lefteris@refu.co>
 */
#define rf_objset_foreach(set_, it_, value_)        \
    for (value_ = rf_objset_first((set_), (it_));   \
         value_;                                    \
         value_ = rf_objset_next((set_), (it_)))

/**
 * rf_objset_subset - Checks if set1 is a subset of set2
 * @set1_:    The left part of ⊆
 * @set2_:    The right part of ⊆
 * @return   True if all elements of set1 also exists in set2
 *
 * added by Lefteris Karapetsas<lefteris@refu.co>
 */
bool i_rf_objset_subset(struct objset_h *set1, struct objset_h *set2);
#define rf_objset_subset(set1_, set2_)                  \
    i_rf_objset_subset(&(set1_)->raw, &(set2_)->raw)
    

/**
 * rf_objset_subset - Checks if set1 is equal to set2
 * @set1_:    The left part of the equality
 * @set2_:    The right part of the equality
 * @return   True if all elements of set1 also exists in set2 and vice versa
 *
 * added by Lefteris Karapetsas<lefteris@refu.co>
 */
bool i_rf_objset_equal(struct objset_h *set1, struct objset_h *set2);
#define rf_objset_equal(set1_, set2_)                  \
    i_rf_objset_equal(&(set1_)->raw, &(set2_)->raw)

#endif /* CCAN_OBJSET_H */
