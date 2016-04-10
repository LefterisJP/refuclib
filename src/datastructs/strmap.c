#include <rflib/datastructs/strmap.h>

#include <rflib/utils/log.h>
#include <rflib/string/retrieval.h>
#include <rflib/string/core.h>
#include <rflib/math/ilog.h>

#include <assert.h>
#include <stdlib.h>
#include <errno.h>

struct node {
    /* These point to strings or nodes. */
    struct strmap child[2];
    /* The byte number where first bit differs. */
    size_t byte_num;
    /* The bit where these children differ. */
    uint8_t bit_num;
};

/* Closest member to this in a non-empty map. */
static struct strmap *closest(struct strmap *n, const struct RFstring *member)
{
    const uint8_t *bytes = (const uint8_t *)rf_string_data(member);

    /* Anything with NULL value is a node. */
    while (!n->v) {
        uint8_t direction = 0;

        if (n->u.n->byte_num < rf_string_length_bytes(member)) {
            uint8_t c = bytes[n->u.n->byte_num];
            direction = (c >> n->u.n->bit_num) & 1;
        }
        n = &n->u.n->child[direction];
    }
    return n;
}

void *strmap_get_(const struct strmap *map, const struct RFstring *member)
{
    struct strmap *n;

    /* Not empty map? */
    if (map->u.n) {
        n = closest((struct strmap *)map, member);
        if (rf_string_equal(member, n->u.s))
            return n->v;
    }
    errno = ENOENT;
    return NULL;
}

bool strmap_add_(struct strmap *map, const struct RFstring *member, const void *value)
{
    size_t len = rf_string_length_bytes(member);
    uint8_t *arg_bytes = (uint8_t *)rf_string_data(member);
    struct strmap *n;
    struct node *newn;
    size_t byte_num = 0;
    uint8_t bit_num;
    uint8_t new_dir;

    assert(value);

    /* Empty map? */
    if (!map->u.n) {
        map->u.s = member;
        map->v = (void *)value;
        return true;
    }

    /* Find closest existing member. */
    n = closest(map, member);

    /* Find where they differ. */
    const uint8_t *close_bytes = (const uint8_t*)rf_string_data(n->u.s);
    unsigned int n_len = rf_string_length_bytes(n->u.s);
    unsigned int smlen = len < n_len ? len : n_len;
    for (byte_num = 0; byte_num != smlen && close_bytes[byte_num] == arg_bytes[byte_num]; byte_num++) {
        ;
    }
    if (byte_num == len && len == n_len) {
        /* All identical! */
        errno = EEXIST;
        return false;
    }

    uint8_t arg_b = byte_num >= len ? 0 : arg_bytes[byte_num];
    uint8_t close_b = byte_num >= n_len ? 0 : close_bytes[byte_num];

    /* Find which bit differs (if we had ilog8, we'd use it) */
    bit_num = ilog32_nz((uint8_t)close_b ^ arg_b) - 1;
    assert(bit_num < CHAR_BIT);

    /* Which direction do we go at this bit? */
    new_dir = ((arg_b) >> bit_num) & 1;

    /* Allocate new node. */
    newn = malloc(sizeof(*newn));
    if (!newn) {
        errno = ENOMEM;
        return false;
    }
    newn->byte_num = byte_num;
    newn->bit_num = bit_num;
    newn->child[new_dir].v = (void *)value;
    newn->child[new_dir].u.s = member;

    /* Find where to insert: not closest, but first which differs! */
    n = map;
    while (!n->v) {
        uint8_t direction = 0;

        if (n->u.n->byte_num > byte_num)
            break;
        /* Subtle: bit numbers are "backwards" for comparison */
        if (n->u.n->byte_num == byte_num && n->u.n->bit_num < bit_num)
            break;

        if (n->u.n->byte_num < len) {
            uint8_t c = arg_bytes[n->u.n->byte_num];
            direction = (c >> n->u.n->bit_num) & 1;
        }
        n = &n->u.n->child[direction];
    }

    newn->child[!new_dir] = *n;
    n->u.n = newn;
    n->v = NULL;
    return true;
}

struct RFstring *strmap_del_(struct strmap *map, const struct RFstring *member, void **valuep)
{
    size_t len = rf_string_length_bytes(member);
    const uint8_t *bytes = (const uint8_t *)rf_string_data(member);
    struct strmap *n;
    struct strmap *parent = NULL;
    const struct RFstring *ret = NULL;
    uint8_t direction = 0; /* prevent bogus gcc warning. */

    /* Empty map? */
    if (!map->u.n) {
        errno = ENOENT;
        return NULL;
    }

    /* Find closest, but keep track of parent. */
    n = map;
    /* Anything with NULL value is a node. */
    while (!n->v) {
        uint8_t c = 0;

        parent = n;
        if (n->u.n->byte_num < len) {
            c = bytes[n->u.n->byte_num];
            direction = (c >> n->u.n->bit_num) & 1;
        } else {
            direction = 0;
        }
        n = &n->u.n->child[direction];
    }

    /* Did we find it? */
    if (!rf_string_equal(member, n->u.s)) {
        errno = ENOENT;
        return NULL;
    }

    ret = n->u.s;
    if (valuep) {
        *valuep = n->v;
    }

    if (!parent) {
        /* We deleted last node. */
        map->u.n = NULL;
    } else {
        struct node *old = parent->u.n;
        /* Raise other node to parent. */
        *parent = old->child[!direction];
        free(old);
    }

    return (struct RFstring *)ret;
}

static bool iterate(struct strmap n,
                    bool (*handle)(const struct RFstring *, void *, void *),
                    const void *data)
{
    if (n.v)
        return handle(n.u.s, n.v, (void *)data);

    return iterate(n.u.n->child[0], handle, data)
        && iterate(n.u.n->child[1], handle, data);
}

void strmap_iterate_(const struct strmap *map, strmap_it_cb cb, const void *data)
{
    /* Empty map? */
    if (!map->u.n)
        return;

    iterate(*map, cb, data);
}

const struct strmap *strmap_prefix_(const struct strmap *map,
                                    const struct RFstring *prefix)
{
    const struct strmap *n;
    const struct strmap *top;
    const uint8_t *bytes = (const uint8_t *)rf_string_data(prefix);

    /* Empty map -> return empty map. */
    if (!map->u.n) {
        return map;
    }

    top = n = map;

    /* We walk to find the top, but keep going to check prefix matches. */
    while (!n->v) {
        uint8_t direction;
        uint8_t c = 0;

        if (n->u.n->byte_num < rf_string_length_bytes(prefix))
            c = bytes[n->u.n->byte_num];

        direction = (c >> n->u.n->bit_num) & 1;
        n = &n->u.n->child[direction];
        if (c) {
            top = n;
        }
    }

    if (!rf_string_begins_with(n->u.s, prefix, 0)) {
        /* Convenient return for prefixes which do not appear in map. */
        static const struct strmap empty_map;
        return &empty_map;
    }

    return top;
}

static void clear(struct strmap n)
{
    if (!n.v) {
        clear(n.u.n->child[0]);
        clear(n.u.n->child[1]);
        free(n.u.n);
    }
}

void strmap_clear_(struct strmap *map)
{
    if (map->u.n)
        clear(*map);
    map->u.n = NULL;
}
