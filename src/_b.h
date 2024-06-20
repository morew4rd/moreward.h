#ifndef _B_H_INCLUDED_
#define _B_H_INCLUDED_
#include "_a.h"

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

#endif  /* _B_H_INCLUDED_ */