#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

#define TEST_ITEM_SIZE 4
#define TEST_INIT_CAP 8

UTEST(create_destroy, test_list_create){
    List *l = nil;
    ierr e = 0;

    l = l_create(TEST_ITEM_SIZE, TEST_INIT_CAP, &e, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE((void *)l, nil);
    EXPECT_EQ(l->itemsize, TEST_ITEM_SIZE);
    EXPECT_EQ(l->len, 0);
    EXPECT_EQ(l->cap, TEST_INIT_CAP);
    EXPECT_NE(l->buf.data, nil);
    EXPECT_EQ(l->buf.size, TEST_ITEM_SIZE * TEST_INIT_CAP);

    e = l_destroy(l, nil);
    EXPECT_EQ(e, 0);
}

UTEST(create_destroy, test_map_create){
    Map *m = nil;
    ierr e = 0;

    m = m_create(TEST_ITEM_SIZE, TEST_ITEM_SIZE, TEST_INIT_CAP, &e, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE((void *)m, nil);
    EXPECT_EQ(m->keys.itemsize, TEST_ITEM_SIZE);
    EXPECT_EQ(m->keys.len, 0);
    EXPECT_EQ(m->keys.cap, TEST_INIT_CAP);
    EXPECT_NE(m->keys.buf.data, nil);
    EXPECT_EQ(m->keys.buf.size, TEST_ITEM_SIZE * TEST_INIT_CAP);
    EXPECT_EQ(m->values.itemsize, TEST_ITEM_SIZE);
    EXPECT_EQ(m->values.len, 0);
    EXPECT_EQ(m->values.cap, TEST_INIT_CAP);
    EXPECT_NE(m->values.buf.data, nil);
    EXPECT_EQ(m->values.buf.size, TEST_ITEM_SIZE * TEST_INIT_CAP);

    e = m_destroy(m, nil);
    EXPECT_EQ(e, 0);
}

UTEST(create_destroy, test_string_create){
    String *s = nil;
    ierr e = 0;

    s = s_create(TEST_INIT_CAP, &e, nil);
    EXPECT_EQ(e, 0);
    EXPECT_NE((void *)s, nil);
    EXPECT_EQ(s->l.itemsize, 1);
    EXPECT_EQ(s->l.len, 0);
    EXPECT_EQ(s->l.cap, TEST_INIT_CAP);
    EXPECT_NE(s->l.buf.data, nil);
    EXPECT_EQ(s->l.buf.size, TEST_INIT_CAP);

    e = s_destroy(s, nil);
    EXPECT_EQ(e, 0);
}
