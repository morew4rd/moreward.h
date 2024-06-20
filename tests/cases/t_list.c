#include "utest.h"

#ifdef TEST_UNIT
#include "mg.h"
#else
#include "moreward.h"
#endif

typedef struct Pt {
    i32 x, y;
    struct {
        u8 r, g, b;
    } color;
} Pt;


UTEST(list, list_struct_size) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);

    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.data, nil);

    /* init list */
    e = l_init(&l, itemsize, 10, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  10 * itemsize);

    /* set_cap */
    e = l_setcap(&l, 200, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  200 * itemsize);

    /* set_cap to 0 (cleanup) */
    e = l_setcap(&l, 0, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  0);
}

UTEST(list, list_neg_cap) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);

    /* init list */
    e = l_init(&l, itemsize, -10, nil);
    EXPECT_EQ(e, ERR_ALLOC_NEG_SIZE);
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  0);
}

UTEST(list, list_push) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);
    Pt pt = {0};

    pt.x = 567;
    pt.y = 123;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);
    EXPECT_EQ(l.cap, 1);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);
    EXPECT_EQ(l.cap, 2);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 3);
    EXPECT_EQ(l.cap, 4);
}


UTEST(list, list_push_get_pop) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);
    Pt pt = {0};
    Pt *rec = nil;

    pt.x = 222;
    pt.y = 111;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);
    EXPECT_EQ(l.cap, 1);

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);
    EXPECT_EQ(l.cap, 1);
    EXPECT_EQ(rec->x, 222);
    EXPECT_EQ(rec->y, 111);

    rec = l_pop(&l, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);
    EXPECT_EQ(l.cap, 1);
    EXPECT_EQ(rec->x, 222);
    EXPECT_EQ(rec->y, 111);
}
