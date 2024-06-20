#include "mg.h"

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


ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);

    if (l->itemsize != 0) {
        e = ERR_LIST_ALREADY_INITIALIZED; /* already initialized */
        return e;
    }
    m_assert(l->buf.data == nil);
    m_assert(l->buf.size == 0);

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
    byte *storage = nil;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);
    CHECK_LIST_ITEMSIZE(l, &e, e);

    if (l->len == l->cap) {
        e = _l_setcap_nochecks(l, l->cap * 2,  a);
        if (e != 0) {
            return e;
        }
    }
    m_assert(l->len < l->cap);
    storage = l->buf.data;
    storage += l->len * l->itemsize;
    memcpy(storage, item, l->itemsize);
    l->len += 1;
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
    if (index < 0 || index > l->len - 1) {
        *errptr = ERR_LIST_OUT_OF_BOUNDS;
        return nil;
    }
    storage = l->buf.data;
    storage += l->itemsize * index;
    return storage;
}


































