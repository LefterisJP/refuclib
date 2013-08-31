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
		     const struct RF_ILNode *head,
		     const struct RF_ILNode *node,
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

struct RF_ILNode *rfIList_CheckNode(const struct RF_ILNode *node,
                                    const char *abortstr)
{
	const struct RF_ILNode *p, *n;
	int count = 0;

	for (p = node, n = node->next; n != node; p = n, n = n->next) {
		count++;
		if (n->prev != p)
			return corrupt(abortstr, node, n, count);
	}
	/* Check prev on head node. */
	if (node->prev != p)
		return corrupt(abortstr, node, node, 0);

	return (struct RF_ILNode *)node;
}

struct RF_ILHead *rfIList_Check(const struct RF_ILHead *h, const char *abortstr)
{
	if (!rfIList_CheckNode(&h->n, abortstr))
		return NULL;
	return (struct RF_ILHead *)h;
}
