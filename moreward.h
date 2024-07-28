#ifndef _MOREWARD_H_
#define _MOREWARD_H_

/*

License info is at the EOF .

moreward.h
==========

What?
-----

Reusable single-file header library for C projects. Provides dynamic lists (arrays), maps and strings. Custom allocators can be used with the API.

Almost all functions return an additional integer error value that is set to 0 on success.

HOW?
----

From exactly one .c file set `MOREWARD_H_IMPL` before including the library:
```c
#define MOREWARD_H_IMPL
#include "moreward.h"
```

Some default values can be overridden with defines. See the header source for details.

Other includes should be just:
```c
#include "moreward.h"
```

For some sample usage, see test cases.

API
---

- l_create
    creates a new list
- l_destroy
    destroys a list
- l_init
    initializes a list
- l_setcap
    sets the capacity of a list
- l_push
    adds an item to the end of a list
- l_put
    puts an item at a specific index in a list
- l_pop
    removes and returns the last item from a list
- l_get
    gets the item at a specific index in a list
- l_swap
    swaps two items in a list
- l_clear
    clears all items from a list
- l_rm_swap
    removes an item from a list by swapping with the last item
- l_rm_move
    removes an item from a list by moving subsequent items to fill the gap
- l_rm_move_n
    removes n items from a list starting at a specific index by moving subsequent items to fill the gaps
- l_insert
    inserts an item at a specific index in a list
- l_insert_empty_n
    inserts n empty items at a specific index in a list
- l_find
    finds the index of an item in a list
- l_len
    gets the number of items in a list
- l_cap
    gets the capacity of a list
- m_create
    creates a new map
- m_destroy
    destroys a map
- m_init
    initializes a map
- m_setcap
    sets the capacity of a map
- m_put
    puts a key-value pair into a map
- m_get
    gets the value associated with a key in a map
- m_rm
    removes a key-value pair from a map
- m_clear
    clears all key-value pairs from a map
- m_len
    gets the number of key-value pairs in a map
- m_cap
    gets the capacity of a map
- s_create
    creates a new string
- s_destroy
    destroys a string
- s_setcap
    sets the capacity of a string
- s_len
    gets the number of characters in a string
- s_cap
    gets the capacity of a string
- s_clear
    clears the contents of a string
- s_get
    gets the contents of a string
- s_cat
    concatenates a const C string to the end of a string
- s_cat_cstr
    concatenates a C string to the end of a string
- s_cat_char
    concatenates a character to the end of a string
- s_cat_f32
    concatenates a float to the end of a string
- s_cat_f64
    concatenates a double to the end of a string
- s_cat_i8
    concatenates an 8-bit integer to the end of a string
- s_cat_i16
    concatenates a 16-bit integer to the end of a string
- s_cat_i32
    concatenates a 32-bit integer to the end of a string
- s_cat_i64
    concatenates a 64-bit integer to the end of a string
- s_cat_isize
    concatenates a size_t to the end of a string
- s_cat_u8
    concatenates an 8-bit unsigned integer to the end of a string
- s_cat_u16
    concatenates a 16-bit unsigned integer to the end of a string
- s_cat_u32
    concatenates a 32-bit unsigned integer to the end of a string
- s_cat_u64
    concatenates a 64-bit unsigned integer to the end of a string
- s_cat_usize
    concatenates a usize to the end of a string


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

#ifndef M_STRING_NUMERIC_CAT_BUF_SIZE
#define M_STRING_NUMERIC_CAT_BUF_SIZE       32
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


#define TRUE    (1)
#define FALSE   (0)
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
    ERR_MAP_PTR_NIL                 = 13001,
    ERR_MAP_KEY_VAL_SIZE_NOT_SET    = 13004,
    ERR_STRING_PTR_NIL              = 14001,
};

#define CHECK_GET_ALLOCATOR(a)                  if (!(a)) { (a) = default_alloc; }
#define CHECK_ALLOC_SIZE(size, errptr, ret)     if (size < 0) { if (errptr) { *(errptr) = ERR_ALLOC_NEG_SIZE; } return ret; }
#define CHECK_BUFFER_PTR(b, errptr, ret)        if (!(b)) { if (errptr) { *(errptr) = ERR_BUFFER_PTR_NIL; } return ret; }
#define CHECK_LIST_PTR(l, errptr, ret)          if (!(l)) { if (errptr) { *(errptr) = ERR_LIST_PTR_NIL; } return ret; }
#define CHECK_LIST_ITEM_PTR(x, errptr, ret)     if (!(x)) { if (errptr) { *(errptr) = ERR_LIST_ITEM_PTR_NIL; } return ret; }
#define CHECK_LIST_ITEMSIZE(l, errptr, ret)     if ((l)->itemsize == 0) { if (errptr) { *(errptr) = ERR_LIST_ITEMSIZE_NOT_SET; } return ret; }
#define CHECK_LIST_BOUNDS(l, idx, errptr, ret)  if (idx < 0 || idx > (l)->len - 1) { *(errptr) = ERR_LIST_OUT_OF_BOUNDS; return ret; }
#define CHECK_MAP_PTR(m, errptr, ret)           if (!(m)) { if (errptr) { *(errptr) = ERR_MAP_PTR_NIL; } return ret; }
#define CHECK_MAP_KEY_VAL_SIZE(m, errptr, ret)  if ((m)->keys.itemsize == 0 || (m)->values.itemsize == 0) { if (errptr) { *(errptr) = ERR_MAP_KEY_VAL_SIZE_NOT_SET; } return ret; }
#define CHECK_STRING_PTR(s, errptr, ret)        if (!(s)) { if (errptr) { *(errptr) = ERR_STRING_PTR_NIL; } return ret; }

#define ZERO_ERRPTR(errptr) if (errptr) { *(errptr) = 0; }

typedef struct Alloc {
    void *udata;
    void * (*malloc)(isize size, void *udata);
    void * (*realloc)(void *ptr, isize newsize, void *udata);
    void (*free)(void *ptr, void *udata);
} Alloc;

typedef struct Buffer {
    isize size;  /* in bytes */
    u8 *data;
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

typedef struct String {
    List l;
} String;

extern Alloc *default_alloc;

Alloc *get_default_alloc(void);

ierr b_setsize(Buffer *b, isize size, Alloc *a);

List *l_create(isize itemsize, isize init_cap, Alloc *a, ierr *errptr);
ierr l_destroy(List *l, Alloc *a);
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
isize l_len(List *l, ierr *errptr);
isize l_cap(List *l, ierr *errptr);

Map *m_create(isize key_size, isize value_size, isize init_cap, Alloc *a, ierr *errptr);
ierr m_destroy(Map *l, Alloc *a);
ierr m_init(Map *m, isize key_size, isize value_size, isize init_cap, Alloc *a);
ierr m_setcap(Map *m, isize cap, Alloc *a);
ierr m_put(Map *m, void *key, void *value, Alloc *a);
void *m_get(Map *m, void *key, ierr *errptr);
ierr m_rm(Map *m, void *key, Alloc *a);
ierr m_clear(Map *m);
isize m_len(Map *m, ierr *errptr);
isize m_cap(Map *m, ierr *errptr);

String *s_create(isize init_cap, Alloc *a, ierr *errptr);
ierr s_destroy(String *s, Alloc *a);
ierr s_setcap(String *s, isize cap, Alloc *a);
isize s_len(String *s, ierr *errptr);
isize s_cap(String *s, ierr *errptr);
ierr s_clear(String *s);
char *s_get(String *s, ierr *errptr);
ierr s_cat(String *s, const char *cstr, Alloc *a);
ierr s_cat_cstr(String *s, char *cstr, Alloc *a);
ierr s_cat_char(String *s, char c, Alloc *a);
ierr s_cat_f32(String *s, f32 f, Alloc *a);
ierr s_cat_f64(String *s, f64 f, Alloc *a);
ierr s_cat_i8(String *s, i8 x, Alloc *a);
ierr s_cat_i16(String *s, i16 x, Alloc *a);
ierr s_cat_i32(String *s, i32 x, Alloc *a);
ierr s_cat_i64(String *s, i64 x, Alloc *a);
ierr s_cat_isize(String *s, isize i, Alloc *a);
ierr s_cat_u8(String *s, u8 x, Alloc *a);
ierr s_cat_u16(String *s, u16 x, Alloc *a);
ierr s_cat_u32(String *s, u32 x, Alloc *a);
ierr s_cat_u64(String *s, u64 x, Alloc *a);
ierr s_cat_usize(String *s, usize x, Alloc *a);




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
    int e = 0;
    isize idx = l->len - 1;
    void *item = l_get(l, idx, &e);
    if (e == 0) {
        l->len -= 1;
    }
    if (errptr) { *errptr = e; }
    return item;
}

void *l_get(List *l, isize index, ierr *errptr) {
    byte *storage = nil;
    CHECK_LIST_PTR(l, errptr, nil);
    CHECK_LIST_BOUNDS(l, index, errptr, nil);
    ZERO_ERRPTR(errptr);
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

    ZERO_ERRPTR(errptr);

    for (i = 0; i < l->len; i++) {
        void *current_item = l_get(l, i, errptr);
        if (memcmp(current_item, item, l->itemsize) == 0) {
            return i;
        }
    }

    if (errptr) { *errptr = ERR_LIST_ITEM_NOT_FOUND; }
    return -1;
}

isize l_len(List *l, ierr *errptr) {
    isize len = 0;
    CHECK_LIST_PTR(l, errptr, -1);
    len = l->len;
    return len;
}

isize l_cap(List *l, ierr *errptr) {
    isize cap = 0;
    CHECK_LIST_PTR(l, errptr, -1);
    cap = l->cap;
    return cap;
}


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


isize m_len(Map *m, ierr *errptr) {
    isize len = 0;
    CHECK_MAP_PTR(m, errptr, -1);
    len = m->keys.len;
    return len;
}

isize m_cap(Map *m, ierr *errptr) {
    isize cap = 0;
    CHECK_MAP_PTR(m, errptr, -1);
    cap = m->keys.cap;
    return cap;
}


#include <string.h>
#include <stdio.h>


ierr s_setcap(String *s, isize cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    s->l.itemsize = 1;
    e = l_setcap(&s->l, cap, a);
    if (s->l.cap > 0) {
        if (s->l.len >= s->l.cap) {
            s->l.len = s->l.cap - 1;
        }
        memset(s->l.buf.data + s->l.len, 0, s->l.cap - s->l.len); // ensure zeros at the end "\0"
    }
    return e;
}

static ierr _check_len_add_cap(String *s, isize newlen, Alloc *a) {
    ierr e = 0;
    if (s->l.cap < newlen + 1) {
        e = s_setcap(s, newlen, a);
        if (e) { return e; }
        s->l.buf.data[newlen] = 0;

    }
    return 0;
}

isize s_len(String *s, ierr *errptr) {
    CHECK_STRING_PTR(s, errptr, -1);
    return s->l.len;
}

isize s_cap(String *s, ierr *errptr) {
    CHECK_STRING_PTR(s, errptr, -1);
    return s->l.cap;
}

ierr s_clear(String *s) {
    ierr e = l_clear(&s->l);
    CHECK_STRING_PTR(s, &e, e);
    *(char *)(s->l.buf.data + s->l.len * s->l.itemsize) = '\0';
    return e;
}

char *s_get(String *s, ierr *errptr) {
    CHECK_STRING_PTR(s, errptr, nil);
    return (char *)s->l.buf.data;
}

ierr s_cat(String *s, const char *cstr, Alloc *a) {
    ierr e = 0;
    isize cstr_len = 0;
    isize new_len = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    CHECK_STRING_PTR(cstr, &e, e);

    cstr_len = strlen(cstr);
    // TODO: MAX check?
    new_len = s->l.len + cstr_len;

    e = _check_len_add_cap(s, new_len, a);
    if (e != 0) {
        return e;
    }

    memcpy(s->l.buf.data + s->l.len * s->l.itemsize, cstr, cstr_len);
    s->l.len += cstr_len;
    s->l.buf.data[s->l.len] = 0;

    return e;
}

ierr s_cat_cstr(String *s, char *cstr, Alloc *a) {
    ierr e = 0;
    isize cstr_len = 0;
    isize new_len = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    CHECK_STRING_PTR(cstr, &e, e);

    cstr_len = strlen(cstr);
    // TODO: MAX check?
    new_len = s->l.len + cstr_len;

    e = _check_len_add_cap(s, new_len, a);
    if (e != 0) {
        return e;
    }

    memcpy(s->l.buf.data + s->l.len * s->l.itemsize, cstr, cstr_len);
    s->l.len += cstr_len;
    s->l.buf.data[s->l.len] = 0;

    return e;
}

ierr s_cat_char(String *s, char c, Alloc *a) {
    ierr e = 0;
    isize new_len = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    new_len = s->l.len + 1;

    e = _check_len_add_cap(s, new_len, a);
    if (e != 0) {
        return e;
    }

    *(char *)(s->l.buf.data + s->l.len * s->l.itemsize) = c;
    s->l.len += 1;
    s->l.buf.data[s->l.len] = 0;

    return e;
}

ierr s_cat_f32(String *s, f32 f, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%f", f);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_f64(String *s, f64 f, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%lf", f);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i8(String *s, i8 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%d", (int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i16(String *s, i16 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%d", (int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i32(String *s, i32 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%d", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i64(String *s, i64 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%ld", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_isize(String *s, isize i, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%ld", i);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u8(String *s, u8 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%u", (unsigned int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u16(String *s, u16 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%u", (unsigned int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u32(String *s, u32 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%u", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u64(String *s, u64 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%lu", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_usize(String *s, usize x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%zu", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}
#endif /* MOREWARD_H_IMPL */

/*

Copyright (c) 2024 moreward

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

*/

#endif /* _MOREWARD_H_ */
