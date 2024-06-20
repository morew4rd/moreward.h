#include "mg.h"

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

    return e;
}


ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);

    if (l->itemsize != 0) {
        e = ERR_LIST_IS_INITIALIZED; /* already initialized */
        return e;
    }

    m_assert(l->buf.data == nil);
    m_assert(l->buf.size == 0);


    l->itemsize = itemsize;

    e = _l_setcap_nochecks(l, init_cap, a);

    return e;
}

ierr l_setcap(List *l, isize cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);

    if (l->itemsize == 0) {
        e = ERR_LIST_IS_UNINITIALIZED; /* list not initialized yet, so we don't have an item size*/
        return e;
    }

    e = _l_setcap_nochecks(l, cap, a);

    return e;
}