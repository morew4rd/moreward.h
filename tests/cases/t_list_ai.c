#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

/*
    Tests in this file were generated by DeepSeek V2 lite and lightly massaged by me.
*/


UTEST(aigen, alloc_dealloc) {
    Alloc *a = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
    void *test = nil;
    EXPECT_EQ(test, nil);
    test = a->malloc(100, a->udata);
    a->free(test, a->udata);
}

UTEST(aigen, simple_buffer) {
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

UTEST(aigen, simple_list) {
    ierr e = 0;
    List l = {0};
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.data, nil);

    /* init list */
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  10 * 100);

    /* set_cap */
    e = l_setcap(&l, 200, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  10 * 200);

    /* set_cap to 0 (cleanup) */
    e = l_setcap(&l, 0, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.size,  0);
}

UTEST(aigen, list_init_invalid_item_size) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, -10, 100, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.itemsize, M_LIST_DEFAULT_INITIAL_CAPACITY);
    EXPECT_NE(l.buf.data, nil);
    EXPECT_EQ(l.buf.size, 100*M_LIST_DEFAULT_INITIAL_CAPACITY);
}

UTEST(aigen, list_init_negative_initial_capacity) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, -100, nil);
    EXPECT_EQ(e, ERR_ALLOC_NEG_SIZE);
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.size, 0);
}

UTEST(aigen, list_put_nil_empty_list) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 2*10, 100, nil);
    EXPECT_EQ(e, 0);
    char item[10] = {0};
    e = l_put(&l, &item, 0);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_put_negative_index) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    char item[10] = {0};
    e = l_put(&l, &item, -1);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_put_index_out_of_bounds) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    char item[10] = {0};
    e = l_put(&l, &item, 100);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}


UTEST(aigen, list_pop_empty_list) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    l_setcap(&l, 0, nil);
    void *item = l_pop(&l, &e);
    EXPECT_EQ(item, nil);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_get_empty_list) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    void *item = l_get(&l, 0, &e);
    EXPECT_EQ(item, nil);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_get_negative_index) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    void *item = l_get(&l, -1, &e);
    EXPECT_EQ(item, nil);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_get_index_out_of_bounds) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    void *item = l_get(&l, 100, &e);
    EXPECT_EQ(item, nil);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_clear_nil_allocator) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_clear(&l);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.len, 0);
}

UTEST(aigen, list_rm_swap_negative_index) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_swap(&l, -1);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_swap_index_out_of_bounds) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_swap(&l, 100);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_move_negative_index) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_move(&l, -1);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_move_index_out_of_bounds) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_move(&l, 100);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_move_n_negative_index) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_move_n(&l, -1, 1);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_move_n_index_out_of_bounds) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_move_n(&l, 100, 1);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_move_n_non_positive_n) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_move_n(&l, 0, -1);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}

UTEST(aigen, list_rm_move_n_index_out_of_bounds_for_n) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_rm_move_n(&l, 0, 101);
    EXPECT_EQ(e, ERR_LIST_OUT_OF_BOUNDS);
}


UTEST(aigen, list_setcap_negative_capacity) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_setcap(&l, -200, nil);
    EXPECT_EQ(e, ERR_ALLOC_NEG_SIZE);
}

UTEST(aigen, list_setcap_zero_capacity) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_setcap(&l, 0, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(l.buf.data, nil);
    EXPECT_EQ(l.buf.size, 0);
}

UTEST(aigen, list_setcap_valid_capacity) {
    ierr e = 0;
    List l = {0};
    e = l_init(&l, 10, 100, nil);
    EXPECT_EQ(e, 0);
    e = l_setcap(&l, 200, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE(l.buf.data, nil);
    EXPECT_EQ(l.buf.size, 10 * 200);
}


UTEST(aigen, test_l_swap) {
    ierr e = 0;
    List l = {0};
    int a = 1, b = 2, c = 3;

    e = l_init(&l, sizeof(int), 3, nil);
    EXPECT_EQ(e, 0);

    e = l_push(&l, &a, nil);
    EXPECT_EQ(e, 0);
    e = l_push(&l, &b, nil);
    EXPECT_EQ(e, 0);
    e = l_push(&l, &c, nil);
    EXPECT_EQ(e, 0);

    e = l_swap(&l, 0, 1);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*(int*)l_get(&l, 0, &e), 2);
    EXPECT_EQ(*(int*)l_get(&l, 1, &e), 1);

    e = l_swap(&l, 1, 2);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*(int*)l_get(&l, 1, &e), 3);
    EXPECT_EQ(*(int*)l_get(&l, 2, &e), 1);

    e = l_swap(&l, 0, 2);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(*(int*)l_get(&l, 0, &e), 1);
    EXPECT_EQ(*(int*)l_get(&l, 2, &e), 2);

    e = l_clear(&l);
    EXPECT_EQ(e, 0);
}