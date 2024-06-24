#ifndef _MOREWARD_H_
#define _MOREWARD_H_

/*

License info is at the end.

moreward.h
==========

What?
-----

Reusable single-file header library for C projects.

Guide
------

Guide is TBD

*/


#include <stddef.h>

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define mg_assert(x)     assert(x)
#else
#define mg_assert(x)
#endif

#ifndef M_LIST_DEFAULT_INITIAL_CAPACITY
#define M_LIST_DEFAULT_INITIAL_CAPACITY     8
#endif

#ifndef M_LIST_MIN_EMPTY_SLOTS
#define M_LIST_MIN_EMPTY_SLOTS              4
#endif

#define countof(a)      (isize)(sizeof(a) / sizeof(*(a)))
#define max(a, b)       ((a)>(b) ? (a) : (b))
#define min(a, b)       ((a)<(b) ? (a) : (b))

typedef float           f32;
typedef double          f64;
typedef signed char     i8;
typedef unsigned char   u8;
typedef signed short    i16;
typedef unsigned short  u16;
typedef signed int      i32;
typedef unsigned int    u32;
typedef signed long     i64;
typedef unsigned long   u64;
typedef u8              byte;
typedef size_t          usize;
typedef ptrdiff_t       isize;
typedef u32             uint;
typedef u64             uptr;
typedef int             ierr;
typedef int             ibool;

#define True    (1)
#define False   (0)
#define nil     NULL

enum {
    ERR_ALLOC_NEG_SIZE              = 10001,
    ERR_BUFFER_PTR_NIL              = 11001,
    ERR_LIST_PTR_NIL                = 12001,
    ERR_LIST_ITEM_PTR_NIL           = 12002,
    ERR_LIST_ALREADY_INITIALIZED    = 12003,    /* if l_init is called on an initialized list */
    ERR_LIST_ITEMSIZE_NOT_SET       = 12004,    /* if l_setcap is called on an uninitialized list */
    ERR_LIST_NEG_ITEMSIZE           = 12005,    /* if l_setcap is called on an uninitialized list */
    ERR_LIST_ITEM_NOT_FOUND         = 12006,    /* if l_find() can't find the given item. it also returns -1 as index. */
    ERR_LIST_OUT_OF_BOUNDS          = 12022,    /* if index > len - 1.  l_get  */
};

#define CHECK_GET_ALLOCATOR(a)                  if (!(a)) { a = default_alloc; }
#define CHECK_ALLOC_SIZE(size, errptr, ret)     if (size < 0) { if (errptr) { *errptr = ERR_ALLOC_NEG_SIZE; } return ret; }
#define CHECK_BUFFER_PTR(b, errptr, ret)        if (!(b)) { if (errptr) { *errptr = ERR_BUFFER_PTR_NIL; } return ret; }
#define CHECK_LIST_PTR(l, errptr, ret)          if (!(l)) { if (errptr) { *errptr = ERR_LIST_PTR_NIL; } return ret; }
#define CHECK_LIST_ITEM_PTR(x, errptr, ret)     if (!(x)) { if (errptr) { *errptr = ERR_LIST_ITEM_PTR_NIL; } return ret; }
#define CHECK_LIST_ITEMSIZE(l, errptr, ret)     if ((l)->itemsize == 0) { if (errptr) { *errptr = ERR_LIST_ITEMSIZE_NOT_SET; } return ret; }
#define CHECK_LIST_BOUNDS(l, idx, errptr, ret)  if (idx < 0 || idx > (l)->len - 1) { *errptr = ERR_LIST_OUT_OF_BOUNDS; return ret; }

typedef struct Alloc {
    void *udata;
    void * (*malloc)(isize size, void *udata);
    void * (*realloc)(void *ptr, isize newsize, void *udata);
    void (*free)(void *ptr, void *udata);
} Alloc;

typedef struct Buffer {
    isize size;  /* in bytes */
    void *data;
} Buffer;

typedef struct List {
    isize itemsize;
    isize len;  /* number of items */
    isize cap;  /* capacity (value = buffer.size / itemsize) */
    Buffer buf;
} List;

typedef struct Map {
    List keys;
    List values;
} Map;

// typedef struct String {
//     List l;
// } String;

extern Alloc *default_alloc;

Alloc *get_default_alloc(void);

ierr b_setsize(Buffer *b, isize size, Alloc *a);

ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a);
ierr l_setcap(List *l, isize cap, Alloc *a);
ierr l_push(List *l, void *item, Alloc *a);
ierr l_put(List *l, void *item, isize index);
void *l_pop(List *l, ierr *errptr);
void *l_get(List *l, isize index, ierr *errptr);
ierr l_swap(List *l, isize i1, isize i2);
ierr l_clear(List *l) ;
ierr l_rm_swap(List *l, isize index);
ierr l_rm_move(List *l, isize index);
ierr l_rm_move_n(List *l, isize index, isize n);
ierr l_insert(List *l, isize index, void *item, Alloc *a);
ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) ;
isize l_find(List *l, void *item, ierr *errptr);


ierr m_init(Map *m, isize key_size, isize value_size, isize init_cap, Alloc *a);
ierr m_setcap(Map *m, isize cap, Alloc *a);
ierr m_put(Map *m, void *key, void *value, Alloc *a);
void *m_get(Map *m, void *key, ierr *errptr);
ierr m_rm(Map *m, void *key, Alloc *a);
ierr m_clear(Map *m);

// ierr s_setcap(String *s, isize cap, Alloc *a);
// ierr s_cat(String *s, const char *cstr, Alloc *a);
// ierr s_cat_format(String *s, const char *fmt, ...);
// ierr s_cat_len(String *s, const char *cstr, isize len, Alloc *a);
// ierr s_cat_str(String *s, String *s2, Alloc *a);
// isize s_len(String *s, ierr *errptr);
// isize s_cap(String *s, ierr *errptr);
// ierr s_clear(String *s);




/* IMPLEMENTATION */


#ifdef MOREWARD_H_IMPL


#include <stdlib.h>

#ifndef M_MALLOC_FN
#define M_MALLOC_FN    malloc
#endif
#ifndef M_REALLOC_FN
#define M_REALLOC_FN   realloc
#endif
#ifndef M_FREE_FN
#define M_FREE_FN      free
#endif

static void *_def_malloc(isize size, void *udata) {
    (void)udata;
    return M_MALLOC_FN(size);
}

static void * _def_realloc(void *ptr, isize newsize, void *udata) {
    (void)udata;
    return M_REALLOC_FN(ptr, newsize);
}

static void _def_free(void *ptr, void *udata) {
    (void)udata;
    M_FREE_FN(ptr);
}

static Alloc _def_alloc = {nil, _def_malloc, _def_realloc, _def_free};


Alloc *default_alloc = &_def_alloc;


Alloc *get_default_alloc(void) {
    return default_alloc;
}


ierr b_setsize(Buffer *b, isize size, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_BUFFER_PTR(b, &e, e);
    CHECK_ALLOC_SIZE(size, &e, e);

    if (size == 0) {
        if (b->size == 0) {
            /* nothing. buffer already empty */
            mg_assert(!b->data);   /* data should be nil */
        } else {
            /* free buffer */
            mg_assert(b->data);   /* data should not be nil */
            a->free(b->data, a->udata);
            b->data = nil;
            b->size = 0;
        }
    } else if (size == b->size) {
        /* do nothing */
        mg_assert(b->data);   /* data should not be nil */
    } else if (b->size == 0) {
        /* malloc */
        mg_assert(!b->data);   /* data should be nil */
        b->data = a->malloc(size, a->udata);
        mg_assert(b->data);   /* data should not be nil */
        b->size = size;
    } else {
        /* realloc */
        mg_assert(b->data);   /* data should not be nil */
        b->data = a->realloc(b->data, size, a->udata);
        mg_assert(b->data);   /* data should not be nil */
        b->size = size;
    }

    return e;
}
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
#include <string.h>

ierr m_init(Map *m, isize key_size, isize value_size, isize init_cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(&m->keys, &e, e);
    CHECK_LIST_PTR(&m->values, &e, e);

    e = l_init(&m->keys, key_size, init_cap, a);
    if (e != 0) { return e; }
    e = l_init(&m->values, value_size, init_cap, a);
    if (e != 0) { return e; }

    return e;
}

ierr m_setcap(Map *m, isize cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_LIST_PTR(&m->keys, &e, e);
    CHECK_LIST_PTR(&m->values, &e, e);
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
    CHECK_LIST_PTR(&m->keys, &e, e);
    CHECK_LIST_PTR(&m->values, &e, e);
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
    CHECK_LIST_PTR(&m->keys, errptr, nil);
    CHECK_LIST_PTR(&m->values, errptr, nil);
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
    CHECK_LIST_PTR(&m->keys, &e, e);
    CHECK_LIST_PTR(&m->values, &e, e);
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
    CHECK_LIST_PTR(&m->keys, &e, e);
    CHECK_LIST_PTR(&m->values, &e, e);

    e = l_clear(&m->keys);
    if (e == 0) {
        e = l_clear(&m->values);
    }
    mg_assert(m->keys.len == m->values.len);
    return e;
}
#endif /* MOREWARD_H_IMPL */

/*

MIT license

*/

#endif /* _MOREWARD_H_ */
