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

/* basic macros  */

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define m_assert(x)     assert(x)
#else
#define m_assert(x)
#endif

#define countof(a)      (isize)(sizeof(a) / sizeof(*(a)))
#define max(a, b)       ((a)>(b) ? (a) : (b))
#define min(a, b)       ((a)<(b) ? (a) : (b))


#define nil NULL

/*  c89 bool type  */

typedef int bool;
#define true (1)
#define false (0)


/*  shortcuts  */

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




#ifndef M_LIST_DEFAULT_INITIAL_CAPACITY
#define M_LIST_DEFAULT_INITIAL_CAPACITY     8
#endif

/* alloc and buffer errors */

enum {
    ERR_ALLOC_NEG_SIZE          = 10001,

    ERR_BUFFER_PTR_NIL          = 11001,

    ERR_LIST_PTR_NIL            = 12001,
    ERR_LIST_IS_INITIALIZED     = 12002,    /* if l_init is called on an initialized list */
    ERR_LIST_IS_UNINITIALIZED   = 12003,    /* if l_setcap is called on an uninitialized list */
};


#define CHECK_GET_ALLOCATOR(a)                  if (!a) { a = default_alloc; }
#define CHECK_ALLOC_SIZE(size, errptr, ret)     if (size < 0) { if (errptr) { *errptr = ERR_ALLOC_NEG_SIZE; } return ret; }
#define CHECK_BUFFER_PTR(b, errptr, ret)        if (!b) { if (errptr) { *errptr = ERR_BUFFER_PTR_NIL; } return ret; }
#define CHECK_LIST_PTR(l, errptr, ret)          if (!l) { if (errptr) { *errptr = ERR_LIST_PTR_NIL; } return ret; }


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


extern Alloc *default_alloc;

Alloc *get_default_alloc(void);

ierr b_setsize(Buffer *b, isize size, Alloc *a);

ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a);
ierr l_setcap(List *l, isize cap, Alloc *a);






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
            m_assert(!b->data);   /* data should be nil */
        } else {
            /* free buffer */
            m_assert(b->data);   /* data should not be nil */
            a->free(b->data, a->udata);
            b->data = nil;
            b->size = 0;
        }
    } else if (size == b->size) {
        /* do nothing */
        m_assert(b->data);   /* data should not be nil */
    } else if (b->size == 0) {
        /* malloc */
        m_assert(!b->data);   /* data should be nil */
        b->data = a->malloc(size, a->udata);
        m_assert(b->data);   /* data should not be nil */
        b->size = size;
    } else {
        /* realloc */
        m_assert(b->data);   /* data should not be nil */
        b->data = a->realloc(b->data, size, a->udata);
        m_assert(b->data);   /* data should not be nil */
        b->size = size;
    }

    return e;
}
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
#endif /* MOREWARD_H_IMPL */

/*

MIT license

*/

#endif /* _MOREWARD_H_ */
