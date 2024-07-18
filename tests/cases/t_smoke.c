#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

UTEST(smoke_tests_shared, smoke) {
    Alloc *a = get_default_alloc();
    EXPECT_EQ(a->udata, nil);
}

UTEST(smoke_tests_shared, string_basic) {
    String s = {0};
    ierr e = 0;
    e = s_setcap(&s, 100, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_cap(&s, nil), 100);
    EXPECT_EQ(s_len(&s, nil), 0);
    EXPECT_STREQ(s_get(&s, nil), "");

    e = s_cat(&s, "hi", nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_len(&s, nil), 2);
    EXPECT_STREQ(s_get(&s, nil), "hi");

    e = s_cat_f64(&s, 3.14, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_len(&s, nil), 10);
    EXPECT_STREQ(s_get(&s, nil), "hi3.140000");

    e = s_clear(&s);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_len(&s, nil), 0);
    EXPECT_STREQ(s_get(&s, nil), "");
}

UTEST(smoke_tests_shared, string_create_destroy) {
    String *s = nil;
    ierr e = 0;

    s = s_create(100, nil, &e);
    EXPECT_EQ(e, 0);
    EXPECT_NE((void *)s, nil);

    e = s_setcap(s, 100, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_cap(s, nil), 100);
    EXPECT_EQ(s_len(s, nil), 0);
    EXPECT_STREQ(s_get(s, nil), "");

    e = s_cat(s, "hi", nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_len(s, nil), 2);
    EXPECT_STREQ(s_get(s, nil), "hi");

    e = s_cat_f64(s, 3.14, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_len(s, nil), 10);
    EXPECT_STREQ(s_get(s, nil), "hi3.140000");

    e = s_clear(s);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_len(s, nil), 0);
    EXPECT_EQ(s_cap(s, nil), 100);
    EXPECT_STREQ(s_get(s, nil), "");

    e = s_destroy(s, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s_cap(s, nil), 0);
}

UTEST(smoke_tests_shared, string_nil_ptr) {
    String *s = nil; // not created

    ierr e = 0;
    e = s_setcap(s, 100, nil);
    EXPECT_EQ(e, ERR_STRING_PTR_NIL);
    EXPECT_EQ(s_cap(s, nil), -1);
    EXPECT_EQ(s_len(s, nil), -1);

    e = s_cat(s, "hi", nil);
    EXPECT_EQ(e, ERR_STRING_PTR_NIL);
    EXPECT_EQ(s_len(s, nil), -1);

    e = s_cat_f64(s, 3.14, nil);
    EXPECT_EQ(e, ERR_STRING_PTR_NIL);
    EXPECT_EQ(s_len(s, nil), -1);

    e = s_clear(s);
    EXPECT_EQ(e, ERR_STRING_PTR_NIL);
    EXPECT_EQ(s_len(s, nil), -1);
}
