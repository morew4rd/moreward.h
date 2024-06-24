#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

typedef struct Pt {
    i32 x, y;
    struct {
        u8 r, g, b;
    } color;
} Pt;


UTEST(Map, Init) {
    Map m = {0};
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(m.keys.itemsize, (isize)sizeof(int));
    EXPECT_EQ(m.values.itemsize, (isize)sizeof(int));
    EXPECT_EQ(m.keys.len, 0);
    EXPECT_EQ(m.values.len, 0);
    EXPECT_EQ(m.keys.cap, 8);
    EXPECT_EQ(m.values.cap, 8);
}

UTEST(Map, SetCap) {
    Map m = {0};
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    EXPECT_EQ(e, 0);
    e = m_setcap(&m, 16, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(m.keys.cap, 16);
    EXPECT_EQ(m.values.cap, 16);
}

UTEST(Map, PutAndGet) {
    Map m = {0};
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    int value = 0;
    EXPECT_EQ(e, 0);
    e = m_put(&m, &(int){1}, &(int){2}, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(m.keys.len, 1);
    EXPECT_EQ(m.values.len, 1);
    value = *(int *)m_get(&m, &(int){1}, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(value, 2);
}

UTEST(Map, GetNonExistent) {
    Map m = {0};
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    EXPECT_EQ(e, 0);
    int *value = (int *)m_get(&m, &(int){1}, nil);
    EXPECT_EQ((void*)value, nil);
    EXPECT_EQ(m.keys.len, 0);
    EXPECT_EQ(m.values.len, 0);
}

UTEST(Map, Remove) {
    Map m = {0};
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    EXPECT_EQ(e, 0);
    e = m_put(&m, &(int){1}, &(int){2}, nil);
    EXPECT_EQ(e, 0);
    e = m_rm(&m, &(int){1}, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(m.keys.len, 0);
    EXPECT_EQ(m.values.len, 0);
}

UTEST(Map, Clear) {
    Map m = {0};
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    EXPECT_EQ(e, 0);
    e = m_put(&m, &(int){1}, &(int){2}, nil);
    EXPECT_EQ(e, 0);
    e = m_clear(&m);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(m.keys.len, 0);
    EXPECT_EQ(m.values.len, 0);
}

UTEST(Map, ErrorHandling) {
    Map m = {0};
    int *ptr = nil;
    ierr e = m_init(&m, sizeof(int), sizeof(int), 8, nil);
    EXPECT_EQ(e, 0);
    e = m_put(&m, nil, &(int){2}, nil);
    EXPECT_EQ(e, ERR_LIST_ITEM_PTR_NIL);
    ptr =  m_get(&m, nil, &e);
    (void)ptr;
    EXPECT_EQ(e, ERR_LIST_ITEM_PTR_NIL);
    e = m_rm(&m, nil, nil);
    EXPECT_EQ(e, ERR_LIST_ITEM_PTR_NIL);
}

