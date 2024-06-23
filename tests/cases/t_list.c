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

UTEST(list, list_push_3_remove_first__swap) {
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

UTEST(list, list_push_3_remove_second__swap) {
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





UTEST(list, list_push_3_remove_first__move) {
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

    e = l_rm_move(&l, 0);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 2);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 3);

}

UTEST(list, list_push_3_remove_second__move) {
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

    e = l_rm_move(&l, 1);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 1);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(rec->x, 3);
}

UTEST(list, list_clear){
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);
    e = l_clear(&l);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);
}

UTEST(list, list_push_remove_range) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);

    /* remove 3 items starting from index 1 */
    e = l_rm_move_n(&l, 1, 3);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    /* verify the remaining items */
    int *rec = nil;
    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 1);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 5);
}



UTEST(list, list_push_remove_range__beginning) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);


    /* remove 3 items starting from index 0 */
    e = l_rm_move_n(&l, 0, 3);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    /* verify the remaining items */
    int *rec = nil;
    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 4);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 5);
}


UTEST(list, list_push_remove_range__end) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);


    /* remove 3 items starting from index 2 */
    e = l_rm_move_n(&l, 2, 3);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 2);

    /* verify the remaining items */
    int *rec = nil;
    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 1);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 2);
}


UTEST(list, list_push_remove_range__end_extra) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);


    /* attempt to remove 4 items starting from index 2 */
    e = l_rm_move_n(&l, 2, 4);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
    EXPECT_EQ(l.len, 5);

    /* verify the remaining items */
    int *rec = nil;
    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 1);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 2);
}

UTEST(list, list_push_remove_range__all) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);


    /* remove 5 items starting from index 0 */
    e = l_rm_move_n(&l, 0, 5);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

}






UTEST(list, list_insert_tests) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;
    int val;
    int *rec = nil;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);

    val = 111;
    e = l_insert(&l, 0, &val, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 6);


    rec = l_get(&l, 0, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 111);

    rec = l_get(&l, 1, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 1);

    val = 222;
    e = l_insert(&l, 6, &val, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 7);

    rec = l_get(&l, 6, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 222);

    val = 333;
    e = l_insert(&l, 4, &val, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 8);

    rec = l_get(&l, 4, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 333);
}




UTEST(list, list_insert_n__middle) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;
    int *rec = nil;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 1; i <= 5; ++i) {
        pt = i;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 5);


    e = l_insert_empty_n(&l, 1, 3, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 8);

    rec = l_get(&l, 0, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 1);

    rec = l_get(&l, 1, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 0);

    rec = l_get(&l, 3, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 0);

    rec = l_get(&l, 4, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 2);

    rec = l_get(&l, 7, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 5);
}



UTEST(list, list_insert_n__head) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;
    int *rec = nil;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 0 ; i < 3; ++i) {
        pt = i + 10;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 3);

   e = l_insert_empty_n(&l, 0, 2, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 5);

    rec = l_get(&l, 0, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 0);

    rec = l_get(&l, 1, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 0);

    rec = l_get(&l, 2, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 10);

    rec = l_get(&l, 3, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 11);

    rec = l_get(&l, 4, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 12);

}



UTEST(list, list_insert_n__end) {
    ierr e = 0;
    List l = {0};
    isize itemsize = sizeof(int);
    int pt = 0;
    int i;
    int *rec = nil;

    /* init list */
    e = l_init(&l, itemsize, 1, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);

    /* push 5 items */
    for (i = 0 ; i < 3; ++i) {
        pt = i + 10;
        e = l_push(&l, &pt, nil);
        EXPECT_EQ(e, 0);
    }
    EXPECT_EQ(l.len, 3);

   e = l_insert_empty_n(&l, 3, 2, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 5);

    rec = l_get(&l, 0, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 10);

    rec = l_get(&l, 1, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 11);

    rec = l_get(&l, 2, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 12);

    rec = l_get(&l, 3, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 0);

    rec = l_get(&l, 4, &e); EXPECT_EQ(e, 0);
    EXPECT_EQ(*rec, 0);

}
