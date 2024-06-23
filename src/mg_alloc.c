#include "m.h"

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
