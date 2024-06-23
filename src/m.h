#ifndef _M_H_INCLUDED_
#define _M_H_INCLUDED_
/* this is dev/test time header. in the final "amalgamated" version (moreward.h), this is on the top of the file. */

#include <stddef.h>

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define m_assert(x)     assert(x)
#else
#define m_assert(x)
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
    ERR_LIST_OUT_OF_BOUNDS          = 12022,    /* if index > len - 1.  l_get  */
};

#define CHECK_GET_ALLOCATOR(a)                  if (!a) { a = default_alloc; }
#define CHECK_ALLOC_SIZE(size, errptr, ret)     if (size < 0) { if (errptr) { *errptr = ERR_ALLOC_NEG_SIZE; } return ret; }
#define CHECK_BUFFER_PTR(b, errptr, ret)        if (!b) { if (errptr) { *errptr = ERR_BUFFER_PTR_NIL; } return ret; }
#define CHECK_LIST_PTR(l, errptr, ret)          if (!l) { if (errptr) { *errptr = ERR_LIST_PTR_NIL; } return ret; }
#define CHECK_LIST_ITEM_PTR(x, errptr, ret)     if (!x) { if (errptr) { *errptr = ERR_LIST_ITEM_PTR_NIL; } return ret; }
#define CHECK_LIST_ITEMSIZE(l, errptr, ret)     if (l->itemsize == 0) { if (errptr) { *errptr = ERR_LIST_ITEMSIZE_NOT_SET; } return ret; }
#define CHECK_LIST_BOUNDS(l, idx, errptr, ret)  if (idx < 0 || idx > l->len - 1) { *errptr = ERR_LIST_OUT_OF_BOUNDS; return ret; }

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
ierr l_push(List *l, void *item, Alloc *a);
ierr l_put(List *l, void *item, isize index);
void *l_pop(List *l, ierr *errptr);
void *l_get(List *l, isize index, ierr *errptr);
ierr l_clear(List *l) ;
ierr l_rm_swap(List *l, isize index);
ierr l_rm_move(List *l, isize index);
ierr l_rm_move_n(List *l, isize index, isize n);
ierr l_insert(List *l, isize index, void *item, Alloc *a);
ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) ;

#endif  /* _M_H_INCLUDED_ */