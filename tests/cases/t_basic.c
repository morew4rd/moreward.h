#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

UTEST(basic, alloc_dealloc) {
    Alloc *a = get_default_alloc();
    Alloc *ax = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
    void *test = nil;
    EXPECT_EQ(test, nil);
    test = a->malloc(100, a->udata);
}