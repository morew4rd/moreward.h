#include "m.h"

#include <string.h>

/* utility function for setting list capacity. only checks for the allocation size */
static ierr _l_setcap_nochecks(List *l, isize cap, Alloc *a) {
    ierr e = 0;
    isize bufsize = 0;

    bufsize = l->itemsize * cap;

    CHECK_ALLOC_SIZE(bufsize, &e, e);

    e = b_setsize(&l->buf, bufsize, a);

    if (e != 0) {
        return e;
    }

    l->cap = cap;

    if (l->len > l->cap) {
        l->len = l->cap;
    }

    return e;
}

static void _l_put_nochecks(List *l, void *item, isize index) {
    byte *storage = l->buf.data;
    storage += index * l->itemsize;
    memcpy(storage, item, l->itemsize);
}

static ierr _l_check_for_growth_and_grow(List *l, isize extra, Alloc *a) {
    ierr e = 0;
    if (l->len  >= l->cap + extra - M_LIST_MIN_EMPTY_SLOTS ) {
        e = _l_setcap_nochecks(l, l->cap * 2 + M_LIST_MIN_EMPTY_SLOTS,  a);
        if (e != 0) {
            return e;
        }
    }
    mg_assert(l->len < l->cap);
    return e;
}

ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);

    if (l->itemsize != 0) {
        e = ERR_LIST_ALREADY_INITIALIZED; /* already initialized */
        return e;
    }

    mg_assert(l->buf.data == nil);
    mg_assert(l->buf.size == 0);

    if (itemsize <= 0) { itemsize = M_LIST_DEFAULT_INITIAL_CAPACITY; }

    l->itemsize = itemsize;

    e = _l_setcap_nochecks(l, init_cap, a);

    return e;
}

ierr l_setcap(List *l, isize cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_ITEMSIZE(l, &e, e);

    e = _l_setcap_nochecks(l, cap, a);

    return e;
}

ierr l_push(List *l, void *item, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_ITEMSIZE(l, &e, e);
    CHECK_LIST_ITEM_PTR(item, &e, e);
    e = _l_check_for_growth_and_grow(l, 0, a);
    if (e != 0) {
        return e;
    }
    _l_put_nochecks(l, item, l->len);

    l->len += 1;
    return e;
}

ierr l_put(List *l, void *item, isize index) {
    ierr e = 0;
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_ITEMSIZE(l, &e, e);
    CHECK_LIST_ITEM_PTR(item, &e, e);
    CHECK_LIST_BOUNDS(l, index, &e, e);

    _l_put_nochecks(l, item, index);
    return e;
}

void *l_pop(List *l, ierr *errptr) {
    isize idx = l->len - 1;
    void *item = l_get(l, idx, errptr);
    if (*errptr == 0) {
        l->len -= 1;
    }
    return item;
}

void *l_get(List *l, isize index, ierr *errptr) {
    byte *storage = nil;
    CHECK_LIST_PTR(l, errptr, nil);
    CHECK_LIST_BOUNDS(l, index, errptr, nil);
    *errptr = 0;
    storage = l->buf.data;
    storage += l->itemsize * index;
    return storage;
}

ierr l_swap(List *l, isize i1, isize i2) {
    ierr e = 0;
    void *item1 = nil;
    void *item2 = nil;
    void *tmp = nil;
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_BOUNDS(l, i1, &e, e);
    CHECK_LIST_BOUNDS(l, i2, &e, e);

    item1 = l_get(l, i1, &e);
    if (e != 0) { return e; }
    item2 = l_get(l, i2, &e);
    if (e != 0) { return e; }

    mg_assert(l->cap > l->len);
    tmp = l->buf.data + l->len * l->itemsize;

    memcpy(tmp, item1, l->itemsize);
    memcpy(item1, item2, l->itemsize);
    memcpy(item2, tmp, l->itemsize);

    return e;
}


ierr l_clear(List *l) {
    ierr e = 0;
    CHECK_LIST_PTR(l, &e, e);
    l->len = 0;
    return e;
}

/* remove item. faster, but does not keep the order */
ierr l_rm_swap(List *l, isize index) {
    ierr e = 0;
    void *item = nil;
    void *last = nil;
    CHECK_LIST_BOUNDS(l, index, &e, e);
    if (l->len > 1) {
        /* if there's only 1 item, then no need to do a swap. otherwise overwrite the item with the last item */
        item = l->buf.data + index * l->itemsize;
        last = l->buf.data + (l->len - 1) * l->itemsize;
        memcpy(item, last, l->itemsize);
    }
    l->len -= 1;
    return e;
}

ierr l_rm_move(List *l, isize index) {
    ierr e = 0;
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_BOUNDS(l, index, &e, e);

    if (index < l->len - 1) {
        /* Shift all items after index to the left by one */
        memmove(
            l->buf.data + index * l->itemsize,
            l->buf.data + (index + 1) * l->itemsize,
            (l->len - index - 1) * l->itemsize
        );
    }

    l->len -= 1;
    return e;
}


ierr l_rm_move_n(List *l, isize index, isize n) {
    ierr e = 0;
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_BOUNDS(l, index, &e, e);
    if (n <= 0) { return ERR_LIST_OUT_OF_BOUNDS; }
    CHECK_LIST_BOUNDS(l, index + n - 1, &e, e);


    if (index + n >= l->len) {
        /* Remove from the end */
        l->len -= n;
    } else {
        /* Shift all items after index by n */
        memmove(
            l->buf.data + index * l->itemsize,
            l->buf.data + (index + n) * l->itemsize,
            (l->len - index - n) * l->itemsize
        );
        l->len -= n;
    }

    return e;
}




// AI written verify
ierr l_insert(List *l, isize index, void *item, Alloc *a) {
     ierr e = 0;
     CHECK_GET_ALLOCATOR(a);
     CHECK_LIST_PTR(l, &e, e);
     CHECK_LIST_ITEM_PTR(item, &e, e);

    if (index == l->len) {
        return l_push(l, item, a);
    }

     CHECK_LIST_BOUNDS(l, index, &e, e);

     e = _l_check_for_growth_and_grow(l, 0, a);
     if (e != 0) {
         return e;
     }

     if (index < l->len) {
         /* Shift all items after index to the right by one */
         memmove(
             l->buf.data + (index + 1) * l->itemsize,
             l->buf.data + index * l->itemsize,
             (l->len - index) * l->itemsize
         );
     }

     _l_put_nochecks(l, item, index);
     l->len += 1;

     return e;
}


/*  this function inserts n items at index by moving the other items away.
    newly allocated items are initialized to zero.
*/
ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);

    if (index != l->len) {
        // insert at the end OK
        CHECK_LIST_BOUNDS(l, index, &e, e);
    }

    if (n <= 0) { return ERR_LIST_OUT_OF_BOUNDS; }

    e = _l_check_for_growth_and_grow(l, n, a);
    if (e != 0) {
        return e;
    }

    if (index < l->len) {
        /* Shift all items after index to the right by n */
        memmove(
            l->buf.data + (index + n) * l->itemsize,
            l->buf.data + index * l->itemsize,
            (l->len - index) * l->itemsize
        );
    }

    /* Initialize new items to zero */
    memset(l->buf.data + index * l->itemsize, 0, n * l->itemsize);

    l->len += n;

    return e;
}


isize l_find(List *l, void *item, ierr *errptr) {
    isize i;
    CHECK_LIST_PTR(l, errptr, -1);
    CHECK_LIST_ITEM_PTR(item, errptr, -1);

    *errptr = 0;

    for (i = 0; i < l->len; i++) {
        void *current_item = l_get(l, i, errptr);
        if (memcmp(current_item, item, l->itemsize) == 0) {
            return i;
        }
    }

    *errptr = ERR_LIST_ITEM_NOT_FOUND;
    return -1;
}