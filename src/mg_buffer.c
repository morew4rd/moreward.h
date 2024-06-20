#include "mg.h"


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