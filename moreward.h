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

#define M_DISABLE_ASSERTS

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define m_assert(x)     assert(x)
#else
#define m_assert(x)
#endif

#define countof(a)      (isize)(sizeof(a) / sizeof(*(a)))
#define max(a, b)       ((a)>(b) ? (a) : (b))
#define min(a, b)       ((a)<(b) ? (a) : (b))


/*  c89 bool type  */

typedef int bool;
#define true (1)
#define false (0)


/*  shortcuts  */

#define nil NULL

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



/* base interface */

#ifndef M_LIST_DEFAULT_INITIAL_CAPACITY
#define M_LIST_DEFAULT_INITIAL_CAPACITY     8
#endif

/* alloc and buffer errors */

enum {
    ERR_ALLOC_NEG_SIZE          = 10001,

    ERR_BUFFER_PTR_NIL          = 11001,

    ERR_LIST_PTR_NIL            = 12001,
};


#define CHECK_ALLOCATOR(a)                      if (!a) { a = default_alloc; }
#define CHECK_ALLOC_SIZE(size, errptr, ret)     if (size < 0) { if (errptr) { *errptr = ERR_ALLOC_NEG_SIZE; } return ret; }
#define CHECK_BUFFER_PTR(b, errptr, ret)        if (!b) { if (errptr) { *errptr = ERR_BUFFER_PTR_NIL; } return ret; }


#define CHECK_LIST_PTR(l, errptr, ret)           if (!l) { if (errptr) { *errptr = ERR_LIST_PTR_NIL; } return ret; }


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
    Buffer *buf;
} List;




extern Alloc *default_alloc;

Alloc *get_default_alloc(void);

ierr b_setsize(Buffer *b, isize size, Alloc *a);

ierr l_init(List *l, isize itemsize, isize init_cap, Alloc *a);
ierr l_setcap(List *l, isize cap, Alloc *a);



/* IMPLEMENTATION */


#ifdef MOREWARD_H_IMPL


#include <stdlib.h>

#ifndef M_MALLOC
#define M_MALLOC    malloc
#endif
#ifndef M_REALLOC
#define M_REALLOC   realloc
#endif
#ifndef M_FREE
#define M_FREE      free
#endif

static void *_def_malloc(isize size, void *udata) {
    (void)udata;
    return M_MALLOC(size);
}

static void * _def_realloc(void *ptr, isize newsize, void *udata) {
    (void)udata;
    return M_REALLOC(ptr, newsize);
}

static void _def_free(void *ptr, void *udata) {
    (void)udata;
    M_FREE(ptr);
}

static Alloc _def_alloc = {nil, _def_malloc, _def_realloc, _def_free};


Alloc *default_alloc = &_def_alloc;


Alloc *get_default_alloc(void) {
    return default_alloc;
}


ierr b_setsize(Buffer *b, isize size, Alloc *a) {
    ierr e = 0;
    CHECK_ALLOCATOR(a);
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
#endif /* MOREWARD_H_IMPL */

/*

MIT license

*/

#endif /* _MOREWARD_H_ */
