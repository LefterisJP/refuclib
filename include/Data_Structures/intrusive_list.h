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
#include <Definitions/inline.h>

/**
 * struct RFilist_node - an entry in a doubly-linked list
 * @next: next entry (self if empty)
 * @prev: previous entry (self if empty)
 *
 * This is used as an entry in a linked list.
 * Example:
 *	struct child {
 *		const char *name;
 *		// Linked list of all us children.
 *		struct RFilist_node list;
 *	};
 */
typedef struct RFilist_node
{
	struct RFilist_node *next, *prev;
}RFilist_node;

/**
 * struct RFilist_head - the head of a doubly-linked list
 * @h: the RFilist_head (containing next and prev pointers)
 *
 * This is used as the head of a linked list.
 * Example:
 *	struct parent {
 *		const char *name;
 *		struct RFilist_head children;
 *		unsigned int num_children;
 *	};
 */
typedef struct RFilist_head
{
	struct RFilist_node n;
}RFilist_head;

/**
 * rf_ilist_check - check head of a list for consistency
 * @h: the RFilist_head
 * @abortstr: the location to print on aborting, or NULL.
 *
 * Because RFilist_nodes have redundant information, consistency checking between
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
struct RFilist_head *rf_ilist_check(const struct RFilist_head *h, const char *abortstr);

/**
 * rf_ilist_check_node - check node of a list for consistency
 * @n: the RFilist_node
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
struct RFilist_node *rf_ilist_check_node(const struct RFilist_node *n,
                                     const char *abortstr);

#ifdef RF_OPTION_DEBUG
#define rf_ilist_debug(h) rf_ilist_check((h), __func__)
#define rf_ilist_debug_node(n) rf_ilist_check_node((n), __func__)
#else
#define rf_ilist_debug(h) (h)
#define rf_ilist_debug_node(n) (n)
#endif

/**
 * RF_ILHEAD_INIT - initializer for an empty RFilist_head
 * @name: the name of the list.
 *
 * Explicit initializer for an empty list.
 *
 * See also:
 *	RF_ILHEAD, rf_ilhead_init()
 *
 * Example:
 *	static struct RFilist_head my_list = RF_ILHEAD_INIT(my_list);
 */
#define RF_ILHEAD_INIT(name) { { &name.n, &name.n } }

/**
 * RF_ILHEAD - define and initialize an empty RFilist_head
 * @name: the name of the list.
 *
 * The RF_ILHEAD macro defines a RFilist_head and initializes it to an empty
 * list.  It can be prepended by "static" to define a static RFilist_head.
 *
 * See also:
 *	RF_ILHEAD_INIT, rf_ilist_head_init()
 *
 * Example:
 *	static RF_ILHEAD(my_global_list);
 */
#define RF_ILHEAD(name) \
	struct RFilist_head name = RF_ILHEAD_INIT(name)

/**
 * rf_ilist_head_init - initialize a RFilist_head
 * @h: the RFilist_head to set to the empty list
 *
 * Example:
 *	...
 *	struct parent *parent = malloc(sizeof(*parent));
 *
 *	rf_ilist_head_init(&parent->children);
 *	parent->num_children = 0;
 */
i_INLINE_DECL void rf_ilist_head_init(struct RFilist_head *h)
{
	h->n.next = h->n.prev = &h->n;
}

i_INLINE_DECL void rf_ilist_copy(struct RFilist_head *from, struct RFilist_head *to)
{
    if (from->n.prev != &from->n) {
        from->n.prev->next = &to->n;
        to->n.prev = from->n.prev;
    } else { // copying uninitialized
        to->n.prev = &to->n;
    }

    if (from->n.next != &from->n) {
        from->n.next->prev = &to->n;
        to->n.next = from->n.next;
    } else { // copying uninitialized
        to->n.next = &to->n;
    }
}

/**
 * rf_ilist_add - add an entry at the start of a linked list.
 * @h: the RFilist_head to add the node to
 * @n: the RFilist_node to add to the list.
 *
 * The RFilist_node does not need to be initialized; it will be overwritten.
 * Example:
 *	struct child *child = malloc(sizeof(*child));
 *
 *	child->name = "marvin";
 *	rf_ilist_add(&parent->children, &child->list);
 *	parent->num_children++;
 */
i_INLINE_DECL void rf_ilist_add(struct RFilist_head *h, struct RFilist_node *n)
{
	n->next = h->n.next;
	n->prev = &h->n;
	h->n.next->prev = n;
	h->n.next = n;
	(void)rf_ilist_debug(h);
}

/**
 * rf_ilist_add_tail - add an entry at the end of a linked list.
 * @h: the RFilist_head to add the node to
 * @n: the RFilist_node to add to the list.
 *
 * The RFilist_node does not need to be initialized; it will be overwritten.
 * Example:
 *	rf_ilist_add_tail(&parent->children, &child->list);
 *	parent->num_children++;
 */
i_INLINE_DECL void rf_ilist_add_tail(struct RFilist_head *h, struct RFilist_node *n)
{
	n->next = &h->n;
	n->prev = h->n.prev;
	h->n.prev->next = n;
	h->n.prev = n;
	(void)rf_ilist_debug(h);
}

/**
 * rf_ilist_is_empty - is a list empty?
 * @h: the RFilist_head
 *
 * If the list is empty, returns true.
 *
 * Example:
 *	assert(rf_ilist_is_empty(&parent->children) == (parent->num_children == 0));
 */
i_INLINE_DECL bool rf_ilist_is_empty(const struct RFilist_head *h)
{
	(void)rf_ilist_debug(h);
	return h->n.next == &h->n;
}

/**
 * rf_ilist_delete - delete an entry from an (unknown) linked list.
 * @n: the RFilist_node to delete from the list.
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
i_INLINE_DECL void rf_ilist_delete(struct RFilist_node *n)
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
 * @h: the RFilist_head the node is in.
 * @n: the RFilist_node to delete from the list.
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
i_INLINE_DECL void rf_ilist_delete_from(struct RFilist_head *h, struct RFilist_node *n)
{
#ifdef RF_OPTION_DEBUG
	{
		/* Thorough check: make sure it was in list! */
		struct RFilist_node *i;
		for (i = h->n.next; i != n; i = i->next)
			assert(i != &h->n);
	}
#endif

	/* Quick test that catches a surprising number of bugs. */
	assert(!rf_ilist_is_empty(h));
	rf_ilist_delete(n);
}

/**
 * rf_ilist_entry - convert a RFilist_node back into the structure containing it.
 * @n: the RFilist_node
 * @type: the type of the entry
 * @member: the RFilist_node member of the type
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
 * @h: the RFilist_head
 * @type: the type of the entry
 * @member: the RFilist_node member of the type
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

i_INLINE_DECL const void *rf_ilist_top_(const struct RFilist_head *h, size_t off)
{
	if (rf_ilist_is_empty(h))
		return NULL;
	return (const char *)h->n.next - off;
}

/**
 * rf_ilist_pop - remove the first entry in a list
 * @h: the RFilist_head
 * @type: the type of the entry
 * @member: the RFilist_node member of the type
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

i_INLINE_DECL const void *rf_ilist_pop_(const struct RFilist_head *h, size_t off)
{
	struct RFilist_node *n;

	if (rf_ilist_is_empty(h))
		return NULL;
	n = h->n.next;
	rf_ilist_delete(n);
	return (const char *)n - off;
}

/**
 * rf_ilist_tail - get the last entry in a list
 * @h: the RFilist_head
 * @type: the type of the entry
 * @member: the RFilist_node member of the type
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

i_INLINE_DECL const void *rf_ilist_tail_(const struct RFilist_head *h, size_t off)
{
	if (rf_ilist_is_empty(h))
		return NULL;
	return (const char *)h->n.prev - off;
}

/**
 * rf_ilist_for_each - iterate through a list.
 * @h: the RFilist_head (warning: evaluated multiple times!)
 * @i: the structure containing the RFilist_node
 * @member: the RFilist_node member of the structure
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
 * @h: the RFilist_head
 * @i: the structure containing the RFilist_node
 * @member: the RFilist_node member of the structure
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
 * @h: the RFilist_head
 * @i: the structure containing the RFilist_node
 * @nxt: the structure containing the RFilist_node
 * @member: the RFilist_node member of the structure
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
 *	struct RFilist_head adopter;
 *
 *	rf_ilist_append_list(&adopter, &parent->children);
 *	assert(rf_ilist_is_empty(&parent->children));
 *	parent->num_children = 0;
 */
i_INLINE_DECL void rf_ilist_append_list(struct RFilist_head *to,
                                       struct RFilist_head *from)
{
	struct RFilist_node *from_tail = rf_ilist_debug(from)->n.prev;
	struct RFilist_node *to_tail = rf_ilist_debug(to)->n.prev;

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
i_INLINE_DECL void rf_ilist_prepend_list(struct RFilist_head *to,
                                         struct RFilist_head *from)
{
	struct RFilist_node *from_tail = rf_ilist_debug(from)->n.prev;
	struct RFilist_node *to_head = rf_ilist_debug(to)->n.next;

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
 * @h: the RFilist_head
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
 * is operation on opaque types with known offset for `struct RFilist_node'
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
 * @h: the RFilist_head
 * @i: the pointer to a memory region wich contains list node data.
 * @nxt: the structure containing the RFilist_node
 * @off: offset(relative to @i) at which list node data resides.
 *
 * For details see `rf_ilist_for_each_off' and `rf_ilist_for_each_safe'
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
i_INLINE_DECL void *rf_ilist_node_to_off(const struct RFilist_node *node, size_t off)
{
	return (void *)((char *)node - off);
}
i_INLINE_DECL struct RFilist_node *rf_ilist_node_from_off(void *ptr, size_t off)
{
	return (struct RFilist_node *)((char *)ptr + off);
}

/* Get the offset of the member, but make sure it's a RFilist_node. */
#define rf_ilist_off(type, member)      \
    (container_off(type, member) +                    \
     check_type(((type *)0)->member, struct RFilist_node))

#define rf_ilist_off_var(var, member)           \
    (container_off_var(var, member) +           \
     check_type(var->member, struct RFilist_node))

/**
 * rf_ilist_has - a simple search in the linked list for a particular object
 *
 * @l: The list to search in
 * @n: The RFlist_node to search for in the list
 * @return true if @n is found and false otherwise
 */
i_INLINE_DECL bool rf_ilist_has(struct RFilist_head *l, const struct RFilist_node *n)
{
    struct RFilist_node *i;
    rf_ilist_for_each_off(l, i, 0) {
        if (i == n) {
            return true;
        }
    }
    return false;
}

#endif // include guards end
