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

UTEST(list, list_push_check_cap) {
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
    EXPECT_EQ(l.cap, 2 + M_LIST_MIN_EMPTY_SLOTS);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);
    EXPECT_EQ(l.cap, 2 + M_LIST_MIN_EMPTY_SLOTS);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 3);
    EXPECT_EQ(l.cap,  (2 + M_LIST_MIN_EMPTY_SLOTS) * 2 + M_LIST_MIN_EMPTY_SLOTS);
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

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);
    EXPECT_EQ(rec->x, 222);
    EXPECT_EQ(rec->y, 111);

    rec = l_pop(&l, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);
    EXPECT_EQ(rec->x, 222);
    EXPECT_EQ(rec->y, 111);
}


UTEST(list, list_push_get_put) {
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

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);
    EXPECT_EQ(rec->x, 222);
    EXPECT_EQ(rec->y, 111);
    pt.x = 4000;
    pt.y = 2000;
    EXPECT_EQ(rec->x, 222);
    EXPECT_EQ(rec->y, 111);

    e = l_put(&l, &pt, 0);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);

    rec = l_pop(&l, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);
    EXPECT_EQ(rec->x, 4000);
    EXPECT_EQ(rec->y, 2000);
}

UTEST(list, list_push_remove_only) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);
    Pt pt = {0};

    pt.x = 222;
    pt.y = 111;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);

    e = l_rm_swap(&l, 0);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);
}

UTEST(list, list_push_3_remove_first) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);
    Pt pt = {0};
    Pt *rec = nil;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    pt.x = 1;
    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);

    pt.x = 2;
    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    pt.x = 3;
    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 3);

    e = l_rm_swap(&l, 0);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 3);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 2);

}

UTEST(list, list_push_3_remove_second) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(Pt);
    Pt pt = {0};
    Pt *rec = nil;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    pt.x = 1;
    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 1);

    pt.x = 2;
    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    pt.x = 3;
    e = l_push(&l, &pt, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 3);

    e = l_rm_swap(&l, 1);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 1);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 3);

}