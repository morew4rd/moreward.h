#ifndef _M_H_INCLUDED_
#define _M_H_INCLUDED_
/* this is dev/test time header. in the final "amalgamated" version (moreward.h), this is on the top of the file. */

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

typedef struct String {
    List l;
} String;

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


#endif  /* _M_H_INCLUDED_ */