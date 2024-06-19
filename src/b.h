#ifndef _B_H_INCLUDED_
#define _B_H_INCLUDED_
#include "a.h"

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


#endif  /* _B_H_INCLUDED_ */