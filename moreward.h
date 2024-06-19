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


/* aliases */

#include <stddef.h>



#define countof(a)      (isize)(sizeof(a) / sizeof(*(a)))
#define max(a, b)       ((a)>(b) ? (a) : (b))
#define min(a, b)       ((a)<(b) ? (a) : (b))

#define nil NULL


typedef int bool;
#define true (1)
#define false (0)


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


enum __AllocError {
    ERR_ALLOC_NIL   = 11000,
};


typedef struct Alloc {
    void *udata;
    void * (*malloc)(isize size, void *udata);
    void * (*realloc)(void *ptr, isize newsize, void *udata);
    void (*free)(void *ptr, void *udata);
} Alloc;


Alloc *get_default_alloc(void);


/* c.h */
/* m.h */


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

Alloc *get_default_alloc(void) {
    return &_def_alloc;
}

#endif /* MOREWARD_H_IMPL */

/*

MIT license

*/

#endif /* _MOREWARD_H_ */
