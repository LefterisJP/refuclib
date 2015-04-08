#include <Data_Structures/objset.h>

unsigned int i_objset_size_(const struct objset_h *set)
{
    struct htable_iter it;
    void *elem;
    unsigned int size = 0;
    htable_foreach(&set->ht, &it, elem) {
        ++size;
    }
    return size;
}

bool i_objset_empty_(const struct objset_h *set)
{
    struct htable_iter it;
	return htable_first(&set->ht, &it) == NULL;
}
