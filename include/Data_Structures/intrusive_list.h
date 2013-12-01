/**
 ** This file contains an intrusive linked list implementation inspired/copied by the ** ccan implementation of intrusive linked list which can be found here:
 ** http://ccodearchive.net/info/list.html
 ** The main changes I made are naming changes so that it is consistent with the 
 ** rest of the library. 
 **
 ** CCAN is in turn a port of the intrusive linked list found in the code of the 
 ** linux kernel from include/linux/list.h. Big thank you to the kernel developers!
 **
 ** The license for the original list code can be found inside the ccan license file
 ** in the root directory of the library
 */

#ifndef RF_INTRUSIVE_LIST_H
#define RF_INTRUSIVE_LIST_H

#include <stdbool.h>
#include <assert.h>
#include <Utils/container_of.h>
#include <Utils/check_type.h>

/**
 * struct RF_ILNode - an entry in a doubly-linked list
 * @next: next entry (self if empty)
 * @prev: previous entry (self if empty)
 *
 * This is used as an entry in a linked list.
 * Example:
 *	struct child {
 *		const char *name;
 *		// Linked list of all us children.
 *		struct RF_ILNode list;
 *	};
 */
typedef struct RF_ILNode
{
	struct RF_ILNode *next, *prev;
}RF_ILNode;

/**
 * struct RF_ILHead - the head of a doubly-linked list
 * @h: the RF_ILHead (containing next and prev pointers)
 *
 * This is used as the head of a linked list.
 * Example:
 *	struct parent {
 *		const char *name;
 *		struct RF_ILHead children;
 *		unsigned int num_children;
 *	};
 */
typedef struct RF_ILHead
{
	struct RF_ILNode n;
}RF_ILHead;

/**
 * rf_ilist_check - check head of a list for consistency
 * @h: the RF_ILHead
 * @abortstr: the location to print on aborting, or NULL.
 *
 * Because RF_ILNodes have redundant information, consistency checking between
 * the back and forward links can be done.  This is useful as a debugging check.
 * If @abortstr is non-NULL, that will be printed in a diagnostic if the list
 * is inconsistent, and the function will abort.
 *
 * Returns the list head if the list is consistent, NULL if not (it
 * can never return NULL if @abortstr is set).
 *
 * See also: rf_ilist_check_node()
 *
 * Example:
 *	static void dump_parent(struct parent *p)
 *	{
 *		struct child *c;
 *
 *		printf("%s (%u children):\n", p->name, p->num_children);
 *		rf_ilist_check(&p->children, "bad child list");
 *		rf_ilist_for_each(&p->children, c, list)
 *		printf(" -> %s\n", c->name);
 *	}
 */
struct RF_ILHead *rf_ilist_check(const struct RF_ILHead *h, const char *abortstr);

/**
 * rf_ilist_check_node - check node of a list for consistency
 * @n: the RF_ILNode
 * @abortstr: the location to print on aborting, or NULL.
 *
 * Check consistency of the list node is in (it must be in one).
 *
 * See also: rf_ilist_check()
 *
 * Example:
 *	static void dump_child(const struct child *c)
 *	{
 *		rf_ilist_check_node(&c->list, "bad child list");
 *		printf("%s\n", c->name);
 *	}
 */
struct RF_ILNode *rf_ilist_check_node(const struct RF_ILNode *n,
                                     const char *abortstr);

#ifdef RF_OPTION_DEBUG
#define rf_ilist_debug(h) rf_ilist_check((h), __func__)
#define rf_ilist_debug_node(n) rf_ilist_check_node((n), __func__)
#else
#define rf_ilist_debug(h) (h)
#define rf_ilist_debug_node(n) (n)
#endif

/**
 * RF_ILHEAD_INIT - initializer for an empty RF_ILHead
 * @name: the name of the list.
 *
 * Explicit initializer for an empty list.
 *
 * See also:
 *	RF_ILHEAD, rf_ilhead_init()
 *
 * Example:
 *	static struct RF_ILHead my_list = RF_ILHEAD_INIT(my_list);
 */
#define RF_ILHEAD_INIT(name) { { &name.n, &name.n } }

/**
 * RF_ILHEAD - define and initialize an empty RF_ILHead
 * @name: the name of the list.
 *
 * The RF_ILHEAD macro defines a RF_ILHead and initializes it to an empty
 * list.  It can be prepended by "static" to define a static RF_ILHead.
 *
 * See also:
 *	RF_ILHEAD_INIT, rf_ilist_head_init()
 *
 * Example:
 *	static RF_ILHEAD(my_global_list);
 */
#define RF_ILHEAD(name) \
	struct RF_ILHead name = RF_ILHEAD_INIT(name)

/**
 * rf_ilist_head_init - initialize a RF_ILHead
 * @h: the RF_ILHead to set to the empty list
 *
 * Example:
 *	...
 *	struct parent *parent = malloc(sizeof(*parent));
 *
 *	rf_ilist_head_init(&parent->children);
 *	parent->num_children = 0;
 */
static inline void rf_ilist_head_init(struct RF_ILHead *h)
{
	h->n.next = h->n.prev = &h->n;
}

/**
 * rf_ilist_add - add an entry at the start of a linked list.
 * @h: the RF_ILHead to add the node to
 * @n: the RF_ILNode to add to the list.
 *
 * The RF_ILNode does not need to be initialized; it will be overwritten.
 * Example:
 *	struct child *child = malloc(sizeof(*child));
 *
 *	child->name = "marvin";
 *	rf_ilist_add(&parent->children, &child->list);
 *	parent->num_children++;
 */
static inline void rf_ilist_add(struct RF_ILHead *h, struct RF_ILNode *n)
{
	n->next = h->n.next;
	n->prev = &h->n;
	h->n.next->prev = n;
	h->n.next = n;
	(void)rf_ilist_debug(h);
}

/**
 * rf_ilist_add_tail - add an entry at the end of a linked list.
 * @h: the RF_ILHead to add the node to
 * @n: the RF_ILNode to add to the list.
 *
 * The RF_ILNode does not need to be initialized; it will be overwritten.
 * Example:
 *	rf_ilist_add_tail(&parent->children, &child->list);
 *	parent->num_children++;
 */
static inline void rf_ilist_add_tail(struct RF_ILHead *h, struct RF_ILNode *n)
{
	n->next = &h->n;
	n->prev = h->n.prev;
	h->n.prev->next = n;
	h->n.prev = n;
	(void)rf_ilist_debug(h);
}

/**
 * rf_ilist_is_empty - is a list empty?
 * @h: the RF_ILHead
 *
 * If the list is empty, returns true.
 *
 * Example:
 *	assert(rf_ilist_is_empty(&parent->children) == (parent->num_children == 0));
 */
static inline bool rf_ilist_is_empty(const struct RF_ILHead *h)
{
	(void)rf_ilist_debug(h);
	return h->n.next == &h->n;
}

/**
 * rf_ilist_delete - delete an entry from an (unknown) linked list.
 * @n: the RF_ILNode to delete from the list.
 *
 * Note that this leaves @n in an undefined state; it can be added to
 * another list, but not deleted again.
 *
 * See also:
 *	rf_ilist_delete_from()
 *
 * Example:
 *	rf_ilist_delete(&child->list);
 *	parent->num_children--;
 */
static inline void rf_ilist_delete(struct RF_ILNode *n)
{
	(void)rf_ilist_debug_node(n);
	n->next->prev = n->prev;
	n->prev->next = n->next;
#ifdef RF_OPTION_DEBUG
	/* Catch use-after-del. */
	n->next = n->prev = NULL;
#endif
}

/**
 * rf_ilist_delete_from - delete an entry from a known linked list.
 * @h: the RF_ILHead the node is in.
 * @n: the RF_ILNode to delete from the list.
 *
 * This explicitly indicates which list a node is expected to be in,
 * which is better documentation and can catch more bugs.
 *
 * See also: rf_ilist_delete()
 *
 * Example:
 *	rf_ilist_delete_from(&parent->children, &child->list);
 *	parent->num_children--;
 */
static inline void rf_ilist_delete_from(struct RF_ILHead *h, struct RF_ILNode *n)
{
#ifdef RF_OPTION_DEBUG
	{
		/* Thorough check: make sure it was in list! */
		struct RF_ILNode *i;
		for (i = h->n.next; i != n; i = i->next)
			assert(i != &h->n);
	}
#endif

	/* Quick test that catches a surprising number of bugs. */
	assert(!rf_ilist_is_empty(h));
	rf_ilist_delete(n);
}

/**
 * rf_ilist_entry - convert a RF_ILNode back into the structure containing it.
 * @n: the RF_ILNode
 * @type: the type of the entry
 * @member: the RF_ILNode member of the type
 *
 * Example:
 *	// First list entry is children.next; convert back to child.
 *	child = rf_ilist_entry(parent->children.n.next, struct child, list);
 *
 * See Also:
 *	rf_ilist_top(), rf_ilist_for_each()
 */
#define rf_ilist_entry(n, type, member) container_of(n, type, member)

/**
 * rf_ilist_top - get the first entry in a list
 * @h: the RF_ILHead
 * @type: the type of the entry
 * @member: the RF_ILNode member of the type
 *
 * If the list is empty, returns NULL.
 *
 * Example:
 *	struct child *first;
 *	first = rf_ilist_top(&parent->children, struct child, list);
 *	if (!first)
 *		printf("Empty list!\n");
 */
#define rf_ilist_top(h, type, member)					\
	((type *)rf_ilist_top_((h), rf_ilist_off(type, member)))

static inline const void *rf_ilist_top_(const struct RF_ILHead *h, size_t off)
{
	if (rf_ilist_is_empty(h))
		return NULL;
	return (const char *)h->n.next - off;
}

/**
 * rf_ilist_pop - remove the first entry in a list
 * @h: the RF_ILHead
 * @type: the type of the entry
 * @member: the RF_ILNode member of the type
 *
 * If the list is empty, returns NULL.
 *
 * Example:
 *	struct child *one;
 *	one = rf_ilist_pop(&parent->children, struct child, list);
 *	if (!one)
 *		printf("Empty list!\n");
 */
#define rf_ilist_pop(h, type, member)					\
	((type *)rf_ilist_pop_((h), rf_ilist_off(type, member)))

static inline const void *rf_ilist_pop_(const struct RF_ILHead *h, size_t off)
{
	struct RF_ILNode *n;

	if (rf_ilist_is_empty(h))
		return NULL;
	n = h->n.next;
	rf_ilist_delete(n);
	return (const char *)n - off;
}

/**
 * rf_ilist_tail - get the last entry in a list
 * @h: the RF_ILHead
 * @type: the type of the entry
 * @member: the RF_ILNode member of the type
 *
 * If the list is empty, returns NULL.
 *
 * Example:
 *	struct child *last;
 *	last = rf_ilist_tail(&parent->children, struct child, list);
 *	if (!last)
 *		printf("Empty list!\n");
 */
#define rf_ilist_tail(h, type, member) \
	((type *)rf_ilist_tail_((h), rf_ilist_off(type, member)))

static inline const void *rf_ilist_tail_(const struct RF_ILHead *h, size_t off)
{
	if (rf_ilist_is_empty(h))
		return NULL;
	return (const char *)h->n.prev - off;
}

/**
 * rf_ilist_for_each - iterate through a list.
 * @h: the RF_ILHead (warning: evaluated multiple times!)
 * @i: the structure containing the RF_ILNode
 * @member: the RF_ILNode member of the structure
 *
 * This is a convenient wrapper to iterate @i over the entire list.  It's
 * a for loop, so you can break and continue as normal.
 *
 * Example:
 *	rf_ilist_for_each(&parent->children, child, list)
 *		printf("Name: %s\n", child->name);
 */
#define rf_ilist_for_each(h, i, member)					\
	rf_ilist_for_each_off(h, i, rf_ilist_off_var(i, member))

/**
 * rf_ilist_for_each_rev - iterate through a list backwards.
 * @h: the RF_ILHead
 * @i: the structure containing the RF_ILNode
 * @member: the RF_ILNode member of the structure
 *
 * This is a convenient wrapper to iterate @i over the entire list.  It's
 * a for loop, so you can break and continue as normal.
 *
 * Example:
 *	rf_ilist_for_each_rev(&parent->children, child, list)
 *		printf("Name: %s\n", child->name);
 */
#define rf_ilist_for_each_rev(h, i, member)					\
	for (i = container_of_var(rf_ilist_debug(h)->n.prev, i, member);	\
	     &i->member != &(h)->n;					\
	     i = container_of_var(i->member.prev, i, member))

/**
 * rf_ilist_for_each_safe - iterate through a list, maybe during deletion
 * @h: the RF_ILHead
 * @i: the structure containing the RF_ILNode
 * @nxt: the structure containing the RF_ILNode
 * @member: the RF_ILNode member of the structure
 *
 * This is a convenient wrapper to iterate @i over the entire list.  It's
 * a for loop, so you can break and continue as normal.  The extra variable
 * @nxt is used to hold the next element, so you can delete @i from the list.
 *
 * Example:
 *	struct child *next;
 *	rf_ilist_for_each_safe(&parent->children, child, next, list) {
 *		rf_ilist_delete(&child->list);
 *		parent->num_children--;
 *	}
 */
#define rf_ilist_for_each_safe(h, i, nxt, member)				\
	rf_ilist_for_each_safe_off(h, i, nxt, rf_ilist_off_var(i, member))

/**
 * rf_ilist_append_list - empty one list onto the end of another.
 * @to: the list to append into
 * @from: the list to empty.
 *
 * This takes the entire contents of @from and moves it to the end of
 * @to.  After this @from will be empty.
 *
 * Example:
 *	struct RF_ILHead adopter;
 *
 *	rf_ilist_append_list(&adopter, &parent->children);
 *	assert(rf_ilist_is_empty(&parent->children));
 *	parent->num_children = 0;
 */
static inline void rf_ilist_append_list(struct RF_ILHead *to,
                                       struct RF_ILHead *from)
{
	struct RF_ILNode *from_tail = rf_ilist_debug(from)->n.prev;
	struct RF_ILNode *to_tail = rf_ilist_debug(to)->n.prev;

	/* Sew in head and entire list. */
	to->n.prev = from_tail;
	from_tail->next = &to->n;
	to_tail->next = &from->n;
	from->n.prev = to_tail;

	/* Now remove head. */
	rf_ilist_delete(&from->n);
	rf_ilist_head_init(from);
}

/**
 * rf_ilist_prepend_list - empty one list into the start of another.
 * @to: the list to prepend into
 * @from: the list to empty.
 *
 * This takes the entire contents of @from and moves it to the start
 * of @to.  After this @from will be empty.
 *
 * Example:
 *	rf_ilist_prepend_list(&adopter, &parent->children);
 *	assert(rf_ilist_is_empty(&parent->children));
 *	parent->num_children = 0;
 */
static inline void rf_ilist_prepend_list(struct RF_ILHead *to,
                                       struct RF_ILHead *from)
{
	struct RF_ILNode *from_tail = rf_ilist_debug(from)->n.prev;
	struct RF_ILNode *to_head = rf_ilist_debug(to)->n.next;

	/* Sew in head and entire list. */
	to->n.next = &from->n;
	from->n.prev = &to->n;
	to_head->prev = from_tail;
	from_tail->next = to_head;

	/* Now remove head. */
	rf_ilist_delete(&from->n);
	rf_ilist_head_init(from);
}

/**
 * rf_ilist_for_each_off - iterate through a list of memory regions.
 * @h: the RF_ILHead
 * @i: the pointer to a memory region wich contains list node data.
 * @off: offset(relative to @i) at which list node data resides.
 *
 * This is a low-level wrapper to iterate @i over the entire list, used to
 * implement all over, more high-level, for-each constructs. It's a for loop,
 * so you can break and continue as normal.
 *
 * WARNING! Being the low-level macro that it is, this wrapper doesn't know
 * nor care about the type of @i. The only assumtion made is that @i points
 * to a chunk of memory that at some @offset, relative to @i, contains a
 * properly filled `struct node_list' which in turn contains pointers to
 * memory chunks and it's turtles all the way down. Whith all that in mind
 * remember that given the wrong pointer/offset couple this macro will
 * happilly churn all you memory untill SEGFAULT stops it, in other words
 * caveat emptor.
 *
 * It is worth mentioning that one of legitimate use-cases for that wrapper
 * is operation on opaque types with known offset for `struct RF_ILNode'
 * member(preferably 0), because it allows you not to disclose the type of
 * @i.
 *
 * Example:
 *	rf_ilist_for_each_off(&parent->children, child,
 *				offsetof(struct child, list))
 *		printf("Name: %s\n", child->name);
 */
#define rf_ilist_for_each_off(h, i, off)                                 \
    for (i = rf_ilist_node_to_off(rf_ilist_debug(h)->n.next, (off));     \
         rf_ilist_node_from_off((void *)i, (off)) != &(h)->n;           \
         i = rf_ilist_node_to_off(rf_ilist_node_from_off((void *)i, (off))->next, \
                                  (off)))

/**
 * rf_ilist_for_each_safe_off - iterate through a list of memory regions, maybe
 * during deletion
 * @h: the RF_ILHead
 * @i: the pointer to a memory region wich contains list node data.
 * @nxt: the structure containing the RF_ILNode
 * @off: offset(relative to @i) at which list node data resides.
 *
 * For details see `rfIList_ForEach_off' and `rfIList_ForEach_safe'
 * descriptions.
 *
 * Example:
 *	rf_ilist_for_each_safe_off(&parent->children, child,
 *		next, offsetof(struct child, list))
 *		printf("Name: %s\n", child->name);
 */
#define rf_ilist_for_each_safe_off(h, i, nxt, off)                       \
    for (i = rf_ilist_node_to_off(rf_ilist_debug(h)->n.next, (off)),     \
         nxt = rf_ilist_node_to_off(rf_ilist_node_from_off(i, (off))->next, \
                                    (off));                             \
         rf_ilist_node_from_off(i, (off)) != &(h)->n;                   \
         i = nxt,                                                       \
         nxt = rf_ilist_node_to_off(rf_ilist_node_from_off(i, (off))->next, \
                                    (off)))


/* Other -off variants. */
#define rf_ilist_entry_off(n, type, off)		\
	((type *)rf_ilist_node_from_off((n), (off)))

#define rf_ilist_head_off(h, type, off)		\
	((type *)rf_ilist_head_off((h), (off)))

#define rf_ilist_tail_off(h, type, off)		\
	((type *)rf_ilist_tail((h), (off)))

#define rf_ilist_add_off(h, n, off)                      \
    rf_ilist_add((h), rf_ilist_node_from_off((n), (off)))

#define rf_ilist_delete_off(n, off)                     \
    rf_ilist_delete(rf_ilist_node_from_off((n), (off)))

#define rf_ilist_delete_from_off(h, n, off)                     \
    rf_ilist_delete_from(h, rf_ilist_node_from_off((n), (off)))

/* Offset helper functions so we only single-evaluate. */
static inline void *rf_ilist_node_to_off(struct RF_ILNode *node, size_t off)
{
	return (void *)((char *)node - off);
}
static inline struct RF_ILNode *rf_ilist_node_from_off(void *ptr, size_t off)
{
	return (struct RF_ILNode *)((char *)ptr + off);
}

/* Get the offset of the member, but make sure it's a RF_ILNode. */
#define rf_ilist_off(type, member)      \
    (container_off(type, member) +                    \
     check_type(((type *)0)->member, struct RF_ILNode))

#define rf_ilist_off_var(var, member)           \
    (container_off_var(var, member) +           \
     check_type(var->member, struct RF_ILNode))

#endif // include guards end
