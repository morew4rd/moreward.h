#include "utest.h"

#ifdef TEST_UNIT
#include "mg.h"
#else
#include "moreward.h"
#endif

UTEST(basic, alloc_dealloc) {
    Alloc *a = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
    void *test = nil;
    EXPECT_EQ(test, nil);
    test = a->malloc(100, a->udata);
}


UTEST(basic, simple_buffer) {
    ierr e = 0;
    Buffer b = {0};
    EXPECT_EQ(b.size, 0);
    EXPECT_EQ(b.data, nil);

    e = b_setsize(&b, 128, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(b.size, 128);
    EXPECT_NE(b.data, nil);

    e = b_setsize(&b, 32, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(b.size, 32);
    EXPECT_NE(b.data, nil);

    e = b_setsize(&b, 0, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(b.size, 0);
    EXPECT_EQ(b.data, nil);
}


UTEST(basic, simple_list) {
    ierr e = 0;
    List l = {0};
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.data, nil);
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE(l.buf.data, nil);
}