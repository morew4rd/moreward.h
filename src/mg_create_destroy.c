#include "m.h"

#include <string.h>

List *l_create(isize itemsize, isize init_cap, Alloc *a, ierr *errptr) {
    ierr e = 0;
    List *l = nil;
    CHECK_GET_ALLOCATOR(a);
    l = (List *)a->malloc(sizeof(List), a->udata);
    if (!l) {
        if (errptr) { *errptr = ERR_LIST_PTR_NIL; }
        return nil;
    }
    memset(l, 0, sizeof(List));
    e = l_init(l, itemsize, init_cap, a);
    if (e != 0) {
        a->free(l, a->udata);
        if (errptr) { *errptr = e; }
        return nil;
    }
    if (errptr) { *errptr = e; }
    return l;
}

ierr l_destroy(List *l, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(l, &e, e);
    e = l_setcap(l, 0, a);
    a->free(l, a->udata);
    return e;
}

Map *m_create(isize key_size, isize value_size, isize init_cap, Alloc *a, ierr *errptr) {
    ierr e = 0;
    Map *m = nil;
    CHECK_GET_ALLOCATOR(a);
    m = (Map *)a->malloc(sizeof(Map), a->udata);
    if (!m) {
        if (errptr) { *errptr = ERR_MAP_PTR_NIL; }
        return nil;
    }
    memset(m, 0, sizeof(Map));
    e = m_init(m, key_size, value_size, init_cap, a);
    if (e != 0) {
        a->free(m, a->udata);
        if (errptr) { *errptr = e; }
        return nil;
    }
    if (errptr) { *errptr = e; }
    return m;
}

ierr m_destroy(Map *m, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_MAP_PTR(m, &e, e);
    e = m_setcap(m, 0, a);;
    a->free(m, a->udata);
    return e;
}

String *s_create(isize init_cap, Alloc *a, ierr *errptr) {
    ierr e = 0;
    String *s = nil;
    CHECK_GET_ALLOCATOR(a);
    s = (String *)a->malloc(sizeof(String), a->udata);
    if (!s) {
        if (errptr) { *errptr = ERR_STRING_PTR_NIL; }
        return nil;
    }
    memset(s, 0, sizeof(String));
    e = l_init(&s->l, 1, init_cap, a);
    if (e != 0) {
        a->free(s, a->udata);
        if (errptr) { *errptr = e; }
        return nil;
    }
    if (errptr) { *errptr = e; }
    return s;
}

ierr s_destroy(String *s, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    e = s_setcap(s, 0, a);
    a->free(s, a->udata);
    return e;
}
