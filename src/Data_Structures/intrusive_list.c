/**
 ** This file contains an intrusive linked list implementation inspired/copied by the ** ccan implementation of intrusive linked list which can be found here:
 ** http://ccodearchive.net/info/list.html
 ** The main changes I made are naming changes so that it is consistent with the 
 ** rest of the library. 
 **
 ** CCAN is in turn a port of the intrusive linked list found in the code of the 
 ** linux kernel from include/linux/list.h. Big thank you to the kernel developers!
 **
 ** The license for the list code can be found inside the ccan license file
 ** in the root directory of the library
 */

#include <stdio.h>
#include <stdlib.h>
#include <Data_Structures/intrusive_list.h>

static void *corrupt(const char *abortstr,
                     const struct RFilist_node *head,
                     const struct RFilist_node *node,
                     unsigned int count)
{
	if (abortstr) {
		fprintf(stderr,
			"%s: prev corrupt in node %p (%u) of %p\n",
			abortstr, node, count, head);
		abort();
	}
	return NULL;
}

struct RFilist_node *rf_ilist_check_node(const struct RFilist_node *node,
                                    const char *abortstr)
{
	const struct RFilist_node *p, *n;
	int count = 0;

	for (p = node, n = node->next; n != node; p = n, n = n->next) {
		count++;
		if (n->prev != p)
			return corrupt(abortstr, node, n, count);
	}
	/* Check prev on head node. */
	if (node->prev != p)
		return corrupt(abortstr, node, node, 0);

	return (struct RFilist_node *)node;
}

struct RFilist_head *rf_ilist_check(const struct RFilist_head *h,
                                    const char *abortstr)
{
	if (!rf_ilist_check_node(&h->n, abortstr))
		return NULL;
	return (struct RFilist_head *)h;
}

i_INLINE_INS void rf_ilist_head_init(struct RFilist_head *h);
i_INLINE_INS void rf_ilist_copy(struct RFilist_head *from, struct RFilist_head *to);
i_INLINE_INS void i_rf_ilist_add(struct RFilist_head *h, struct RFilist_node *n, const char *errstr);
i_INLINE_INS void i_rf_ilist_add_tail(struct RFilist_head *h, struct RFilist_node *n, const char *errstr);
i_INLINE_INS bool i_rf_ilist_is_empty(const struct RFilist_head *h, const char *errstr);
i_INLINE_INS void i_rf_ilist_delete(struct RFilist_node *n, const char *errstr);
i_INLINE_INS void rf_ilist_delete_from(struct RFilist_head *h, struct RFilist_node *n);
i_INLINE_INS const void *rf_ilist_top_(const struct RFilist_head *h, size_t off);
i_INLINE_INS const void *rf_ilist_pop_(const struct RFilist_head *h, size_t off);
i_INLINE_INS struct RFilist_node *rf_ilist_pop_raw(const struct RFilist_head *h);
i_INLINE_INS const void *rf_ilist_pop_back_(const struct RFilist_head *h, size_t off);
i_INLINE_INS void rf_ilist_pop_until(const struct RFilist_head *h, struct RFilist_node *n);
i_INLINE_INS void rf_ilist_pop_back_until(const struct RFilist_head *h, struct RFilist_node *n);
i_INLINE_INS struct RFilist_node *rf_ilist_pop_back_raw(const struct RFilist_head *h);
i_INLINE_INS const void *rf_ilist_tail_(const struct RFilist_head *h, size_t off);
i_INLINE_INS void i_rf_ilist_append_list(struct RFilist_head *to,
                                         struct RFilist_head *from,
                                         const char *errstr);
i_INLINE_INS void i_rf_ilist_prepend_list(struct RFilist_head *to,
                                        struct RFilist_head *from,
                                        const char *errstr);
i_INLINE_INS void *rf_ilist_node_to_off(const struct RFilist_node *node, size_t off);
i_INLINE_INS struct RFilist_node *rf_ilist_node_from_off(void *ptr, size_t off);
i_INLINE_INS bool rf_ilist_has(struct RFilist_head *l, const struct RFilist_node *n);
i_INLINE_INS struct RFilist_node *i_rf_ilist_at(struct RFilist_head *l, unsigned int idx);
i_INLINE_INS unsigned int i_rf_ilist_size(const struct RFilist_head *h, const char *errstr);
