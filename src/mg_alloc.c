#include "mg.h"

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
