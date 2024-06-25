#include "utest.h"

#ifdef TEST_UNIT
#include "m.h"
#else
#include "moreward.h"
#endif

UTEST(String, SetCapacity) {
    String s = {0};
    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.cap, 10);
}

UTEST(String, CatCStr) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_cstr(&s, "hello", nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 5);
    EXPECT_STREQ((char *)s.l.buf.data, "hello");
}


UTEST(String, CatCStr_twice) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_cstr(&s, "hello", nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 5);
    EXPECT_STREQ((char *)s.l.buf.data, "hello");
    e = s_cat_cstr(&s, "hello", nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 10);
    EXPECT_STREQ((char *)s.l.buf.data, "hellohello");
}


UTEST(String, CatChar) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_char(&s, 'a', nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 1);
    EXPECT_EQ(((char *)s.l.buf.data)[0], 'a');

    EXPECT_STREQ((char *)s.l.buf.data, "a");
}

UTEST(String, CatF32) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_f32(&s, 3.14f, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 8);
    EXPECT_STREQ((char *)s.l.buf.data, "3.140000");
}

UTEST(String, CatF64) {
    String s = {0};
    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_f64(&s, 3.14, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 8);
    EXPECT_STREQ((char *)s.l.buf.data, "3.140000");
}

UTEST(String, CatI8) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_i8(&s, -123, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 4);
    EXPECT_STREQ((char *)s.l.buf.data, "-123");
}

UTEST(String, CatI16) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_i16(&s, -12345, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 6);
    EXPECT_STREQ((char *)s.l.buf.data, "-12345");
}

UTEST(String, CatI32) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_i32(&s, -1234567, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 8);
    EXPECT_STREQ((char *)s.l.buf.data, "-1234567");
}

UTEST(String, CatI64) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_i64(&s, -123456789012345, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 16);
    EXPECT_STREQ((char *)s.l.buf.data, "-123456789012345");
}

UTEST(String, CatIsize) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_isize(&s, -123456789012345, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 16);
    EXPECT_STREQ((char *)s.l.buf.data, "-123456789012345");
}

UTEST(String, CatU8) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_u8(&s, 255, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 3);
    EXPECT_STREQ((char *)s.l.buf.data, "255");
}

UTEST(String, CatU16) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_u16(&s, 65535, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 5);
    EXPECT_STREQ((char *)s.l.buf.data, "65535");
}

UTEST(String, CatU32) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_u32(&s, 4294967295U, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 10);
    EXPECT_STREQ((char *)s.l.buf.data, "4294967295");
}

UTEST(String, CatU64) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_u64(&s, 18446744073709551615ULL, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 20);
    EXPECT_STREQ((char *)s.l.buf.data, "18446744073709551615");
}

UTEST(String, CatUsize) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_usize(&s, 18446744073709551615ULL, nil);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 20);
    EXPECT_STREQ((char *)s.l.buf.data, "18446744073709551615");
}

UTEST(String, Length) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_cstr(&s, "hello", nil);
    EXPECT_EQ(e, 0);
    isize len = s_len(&s, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(len, 5);}

UTEST(String, Capacity) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    isize cap = s_cap(&s, &e);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(cap, 10);
}

UTEST(String, Clear) {
    String s = {0};

    ierr e = s_setcap(&s, 10, nil);
    EXPECT_EQ(e, 0);
    e = s_cat_cstr(&s, "hello", nil);
    EXPECT_EQ(e, 0);
    e = s_clear(&s);
    EXPECT_EQ(e, 0);
    EXPECT_EQ(s.l.len, 0);
}