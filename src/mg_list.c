#include "mg.h"


ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a) {
    ierr e = 0;
    isize bufsize = 0;
    CHECK_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);
    bufsize = itemsize * init_cap;
    CHECK_ALLOC_SIZE(bufsize, &e, e);

    m_assert("TBD");

    return e;
}

ierr l_setcap(List *l, isize cap, Alloc *a) {
    ierr e = 0;
    isize bufsize = 0;
    CHECK_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);
    bufsize = l->itemsize * cap;
    CHECK_ALLOC_SIZE(bufsize, &e, e);

    m_assert("TBD");

    return e;
}