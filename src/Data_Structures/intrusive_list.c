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
