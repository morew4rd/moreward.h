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

#ifndef M_LIST_MIN_EMPTY_SLOTS
#define M_LIST_MIN_EMPTY_SLOTS              4
#endif


/* alloc and buffer errors */

enum {
    ERR_ALLOC_NEG_SIZE              = 10001,
    ERR_BUFFER_PTR_NIL              = 11001,
    ERR_LIST_PTR_NIL                = 12001,
    ERR_LIST_ALREADY_INITIALIZED    = 12002,    /* if l_init is called on an initialized list */
    ERR_LIST_ITEMSIZE_NOT_SET       = 12003,    /* if l_setcap is called on an uninitialized list */
    ERR_LIST_OUT_OF_BOUNDS          = 12004,    /* if index > len - 1.  l_get  */
};


#define CHECK_GET_ALLOCATOR(a)                  if (!a) { a = default_alloc; }
#define CHECK_ALLOC_SIZE(size, errptr, ret)     if (size < 0) { if (errptr) { *errptr = ERR_ALLOC_NEG_SIZE; } return ret; }
#define CHECK_BUFFER_PTR(b, errptr, ret)        if (!b) { if (errptr) { *errptr = ERR_BUFFER_PTR_NIL; } return ret; }
#define CHECK_LIST_PTR(l, errptr, ret)          if (!l) { if (errptr) { *errptr = ERR_LIST_PTR_NIL; } return ret; }
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
ierr l_rm_swap(List *l, isize index);
ierr l_rm_move(List *l, isize index);
ierr l_rm_move_n(List *l, isize index, isize n);
ierr l_clear(List *l) ;
ierr l_insert(List *l, isize index, void *item, Alloc *a);
ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) ;







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

static ierr _l_check_for_growth_and_grow(List *l, Alloc *a) {
    ierr e = 0;
    if (l->len  >= l->cap - M_LIST_MIN_EMPTY_SLOTS ) {
        e = _l_setcap_nochecks(l, l->cap * 2 + M_LIST_MIN_EMPTY_SLOTS,  a);
        if (e != 0) {
            return e;
        }
    }
    m_assert(l->len < l->cap);
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
    m_assert(l->buf.data == nil);
    m_assert(l->buf.size == 0);

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
    e = _l_check_for_growth_and_grow(l, a);
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


ierr l_insert(List *l, isize index, void *item, Alloc *a) {

}
ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {

}


















// ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
//     ierr e = 0;
//     CHECK_GET_ALLOCATOR(a);
//     CHECK_LIST_PTR(l, &e, e);
//     CHECK_LIST_ITEMSIZE(l, &e, e);
//     CHECK_LIST_BOUNDS(l, index, &e, e);

//     if (l->len == 0) {
//         e = _l_setcap_nochecks(l, n, a);
//         if (e != 0) {
//             return e;
//         }
//         for (isize i = 0; i < n; ++i) {
//             _l_put_nochecks(l, nil, index + i);
//         }
//         l->len = n;
//         return e;
//     } else {
//         e = _l_check_for_growth_and_grow(l, a);
//         if (e != 0) {
//             return e;
//         }
//         /* Move items to the right to make space for the new items */
//         byte *dst = l->buf.data + (index + n) * l->itemsize;
//         byte *src = l->buf.data + index * l->itemsize;
//         isize bytes_to_move = (l->len - index) * l->itemsize;
//         memmove(dst, src, bytes_to_move);
//         /* Fill the new empty slots with the new items */
//         for (isize i = 0; i < n; ++i) {
//             _l_put_nochecks(l, nil, index + i);
//         }
//         l->len += n;
//         return e;
//     }
// }

// ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
//     ierr e = 0;
//     CHECK_LIST_PTR(l, &e, e);
//     CHECK_LIST_ITEMSIZE(l, &e, e);
//     // CHECK_LIST_BOUNDS(l, index, &e, e);

//     if (n <= 0) return e;

//     if (l->len + n > l->cap) {
//         /* Make room by resizing the buffer */
//         e = _l_setcap_nochecks(l, max(l->cap * 2, l->cap + n), a);
//         if (e != 0) {
//             return e;
//         }
//     }

//     /* Shift items to the right to make space for the new slots */
//     memmove(
//         l->buf.data + (index + n) * l->itemsize,
//         l->buf.data + index * l->itemsize,
//         (l->len - index) * l->itemsize
//     );

//     /* Update the buffer size and length */
//     l->buf.size += n * l->itemsize;
//     l->len += n;

//     return e;
// }







// ierr l_rm_move(List *l, isize index) {
//     ierr e = 0;
//     void *item = nil;
//     CHECK_LIST_BOUNDS(l, index, &e, e);
//     if (e != 0) {
//         return e;
//     }
//     item = l_get(l, index, &e);
//     if (e != 0) {
//         return e;
//     }
//     if (index < l->len - 1) {
//         memmove(item, l->buf.data + (index + 1) * l->itemsize, (l->len - index - 1) * l->itemsize);
//     }
//     l->len -= 1;
//     return e;
// }

// ierr l_rm_move_n(List *l, isize index, isize n) {
//     ierr e = 0;
//     CHECK_LIST_BOUNDS(l, index, &e, e);
//     CHECK_LIST_BOUNDS(l, index + n - 1, &e, e);
//     if (e != 0) {
//         return e;
//     }
//     if (index < l->len - n) {
//         memmove(l->buf.data + index * l->itemsize, l->buf.data + (index + n) * l->itemsize, (l->len - index - n) * l->itemsize);
//     }
//     l->len -= n;
//     return e;
// }

// ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
//     ierr e = 0;
//     CHECK_GET_ALLOCATOR(a);
//     CHECK_LIST_PTR(l, &e, e);
//     CHECK_LIST_BOUNDS(l, index, &e, e*100);
//     e = _l_check_for_growth_and_grow(l, a);
//     if (e != 0) {
//         return e;
//     }
//     if (index < l->len) {
//         memmove(l->buf.data + (index + n) * l->itemsize, l->buf.data + index * l->itemsize, (l->len - index) * l->itemsize);
//     }
//     l->len += n;
//     return e;
// }

// ierr l_clear(List *l) {
//     ierr e = 0;
//     CHECK_LIST_PTR(l, &e, e);
//     CHECK_LIST_ITEMSIZE(l, &e, e);
//     l->len = 0;
//     b_setsize(&l->buf, 0, default_alloc);
//     return e;
// }



// ierr l_rm_move(List *l, isize index) {
//     ierr e = 0;
//     void *item = nil;
//     CHECK_LIST_BOUNDS(l, index, &e, e);
//     if (l->len > 1) {
//         item = l->buf.data + index * l->itemsize;
//         memcpy(item, l->buf.data + (index + 1) * l->itemsize, l->itemsize);
//     }
//     l->len -= 1;
//     return e;
// }

// ierr l_rm_move_n(List *l, isize index, isize n) {
//     ierr e = 0;
//     CHECK_LIST_BOUNDS(l, index, &e, e);
//     CHECK_LIST_BOUNDS(l, index + n - 1, &e, e);
//     if (n <= 0) {
//         return e;
//     }
//     if (l->len > index + n) {
//         memcpy(l->buf.data + index * l->itemsize, l->buf.data + (index + n) * l->itemsize, (l->len - (index + n)) * l->itemsize);
//     }
//     l->len -= n;
//     return e;
// }

// ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
//     ierr e = 0;
//     CHECK_GET_ALLOCATOR(a);
//     CHECK_LIST_PTR(l, &e, e);
//     CHECK_LIST_ITEMSIZE(l, &e, e);
//     // CHECK_LIST_BOUNDS(l, index, &e, e);
//     if (n <= 0) {
//         return e;
//     }
//     e = _l_check_for_growth_and_grow(l, a);
//     if (e != 0) {
//         return e;
//     }
//     if (l->len - index < n) {
//         n = l->len - index;
//     }
//     if (l->len >= l->cap) {
//         e = _l_setcap_nochecks(l, l->cap * 2, a);
//         if (e != 0) {
//             return e;
//         }
//     }
//     memmove(l->buf.data + (index + n) * l->itemsize, l->buf.data + index * l->itemsize, (l->len - index) * l->itemsize);
//     memset(l->buf.data + index * l->itemsize, 0, n * l->itemsize);
//     l->len += n;
//     return e;
// }

// ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
//     ierr e = 0;
//     CHECK_GET_ALLOCATOR(a);
//     CHECK_LIST_PTR(l, &e, e);
//     CHECK_LIST_ITEMSIZE(l, &e, e);
//     CHECK_LIST_BOUNDS(l, index, &e, e);
//     if (n <= 0) {
//         return e;
//     }
//     e = _l_check_for_growth_and_grow(l, a);
//     if (e != 0) {
//         return e;
//     }
//     if (l->len - index < n) {
//         n = l->len - index;
//     }
//     memcpy(l->buf.data + (index + n) * l->itemsize, l->buf.data + index * l->itemsize, (l->len - index) * l->itemsize);
//     memset(l->buf.data + index * l->itemsize, 0, n * l->itemsize);
//     l->len += n;
//     return e;
// }

// ierr l_clear(List *l) {
//     ierr e = 0;
//     CHECK_LIST_PTR(l, &e, e);
//     l->len = 0;
//     return e;
// }






/* remove item. moves other items. slow, but keeps the order

ierr l_rm_move(List *l, isize index) {
    ierr e = 0;
    CHECK_LIST_BOUNDS(l, index, &e, e);
    if (l->len > 1) {
        // Move items after the index to the left by one position
        memmove(
            l->buf.data + index * l->itemsize,
            l->buf.data + (index + 1) * l->itemsize,
            (l->len - index - 1) * l->itemsize
        );
        l->len -= 1;
    } else if (l->len == 1) {
        // If there's only one item, simply set the length to 0
        l->len = 0;
    } else {
        // If the list is empty, there's nothing to do
        // This should be handled by the CHECK_LIST_BOUNDS macro, but just in case
        e = ERR_LIST_OUT_OF_BOUNDS;
    }
    return e;
}


ierr l_rm_move_n(List *l, isize index, isize n) {
    ierr e = 0;
    CHECK_LIST_BOUNDS(l, index, &e, e);
    if (n <= 0) {
        return e; // No items to remove
    }
    if (index + n > l->len) {
        return ERR_LIST_OUT_OF_BOUNDS; // Not enough items to remove
    }
    if (l->len > 1) {
        // Move items after the index to the left by `n` positions
        memmove(l->buf.data + index * l->itemsize, l->buf.data + (index + n) * l->itemsize, (l->len - index - n) * l->itemsize);
        l->len -= n;
    } else if (l->len == n) {
        // If the list length is equal to `n`, set the length to 0
        l->len = 0;
    } else {
        // If the list is empty, there's nothing to do
        e = ERR_LIST_OUT_OF_BOUNDS;
    }
    return e;
}



ierr l_insert_empty_n(List *l, isize index, isize n, Alloc *a) {
    ierr e = 0;
    CHECK_LIST_PTR(l, &e, e);
    CHECK_ALLOC_SIZE(n, &e, e); // Check the size of the items to insert
    CHECK_LIST_BOUNDS(l, index, &e, e);
    if (n <= 0) {
        return e; // No space to insert
    }
    if (index < 0 || index > l->cap) {
        return ERR_LIST_OUT_OF_BOUNDS; // Invalid index
    }

    // Check if we need to increase the capacity
    if (l->len + n > l->cap) {
        e = l_setcap(l, max(l->cap * 2, l->len + n), a);
        if (e != 0) {
            return e;
        }
    }

    // Move items after the index to the right by `n` positions
    if (l->len > index) {
        memmove(l->buf.data + (index + n) * l->itemsize, l->buf.data + index * l->itemsize, (l->len - index) * l->itemsize);
    }

    // Set the new items to NULL (empty)
    byte *storage = l->buf.data;
    storage += index * l->itemsize;
    memset(storage, 0, n * l->itemsize); // Fill new space with zeros

    l->len += n;
    return e;
}



ierr l_clear(List *l) {
    ierr e = 0;
    CHECK_LIST_PTR(l, &e, e);
    l->len = 0;
    return e;
}









*/





#endif /* MOREWARD_H_IMPL */

/*

MIT license

*/

#endif /* _MOREWARD_H_ */
