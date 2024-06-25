#include "m.h"

#include <string.h>

ierr m_init(Map *m, isize key_size, isize value_size, isize init_cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_MAP_PTR(m, &e, e);

    e = l_init(&m->keys, key_size, init_cap, a);
    if (e != 0) { return e; }
    e = l_init(&m->values, value_size, init_cap, a);
    if (e != 0) { return e; }

    return e;
}

ierr m_setcap(Map *m, isize cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_MAP_PTR(m, &e, e);
    CHECK_MAP_KEY_VAL_SIZE(m, &e, e);
    CHECK_LIST_ITEMSIZE(&m->keys, &e, e);
    CHECK_LIST_ITEMSIZE(&m->values, &e, e);

    e = l_setcap(&m->keys, cap, a);
    if (e != 0) { return e; }
    e = l_setcap(&m->values, cap, a);
    if (e != 0) { return e; }

    return e;
}

ierr m_put(Map *m, void *key, void *value, Alloc *a) {
    ierr e = 0;
    isize idx = 0;
    void *key_storage = nil;
    CHECK_GET_ALLOCATOR(a);
    CHECK_MAP_PTR(m, &e, e);
    CHECK_MAP_KEY_VAL_SIZE(m, &e, e);
    CHECK_LIST_ITEM_PTR(key, &e, e);
    CHECK_LIST_ITEM_PTR(value, &e, e);

    idx = l_find(&m->keys, key, &e);
    if (e == 0) {
        /* update */
        key_storage = l_get(&m->keys, idx, &e);
        if (e == 0) {
            memcpy(key_storage, key, m->keys.itemsize);
            key_storage = l_get(&m->values, idx, &e);
            if (e == 0) {
                memcpy(key_storage, value, m->values.itemsize);
            }
            mg_assert(m->keys.len == m->values.len);
        }
    } else {
        /* insert */
        e = l_push(&m->keys, key, a);
        if (e == 0) {
            e = l_push(&m->values, value, a);
        }
        mg_assert(m->keys.len == m->values.len);
    }

    return e;
}

void *m_get(Map *m, void *key, ierr *errptr) {
    isize idx = 0;
    void *value = nil;
    ierr e = 0;
    CHECK_MAP_PTR(m, errptr, nil);
    CHECK_MAP_KEY_VAL_SIZE(m, errptr, nil);
    CHECK_LIST_ITEM_PTR(key, errptr, nil);

    idx = l_find(&m->keys, key, &e);
    if (e == 0) {
        value = l_get(&m->values, idx, errptr);
    } else {
        if (errptr) { *errptr = e; }
    }

    return value;
}

ierr m_rm(Map *m, void *key, Alloc *a) {
    ierr e = 0;
    isize idx = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_MAP_PTR(m, &e, e);
    CHECK_MAP_KEY_VAL_SIZE(m, &e, e);
    CHECK_LIST_ITEM_PTR(key, &e, e);

    idx = l_find(&m->keys, key, &e);
    if (e == 0) {
        e = l_rm_move(&m->keys, idx);
        if (e == 0) {
            e = l_rm_move(&m->values, idx);
        }
        mg_assert(m->keys.len == m->values.len);
    }

    return e;
}

ierr m_clear(Map *m) {
    ierr e = 0;
    CHECK_MAP_PTR(m, &e, e);
    CHECK_MAP_KEY_VAL_SIZE(m, &e, e);

    e = l_clear(&m->keys);
    if (e == 0) {
        e = l_clear(&m->values);
    }
    mg_assert(m->keys.len == m->values.len);
    return e;
}