#include "m.h"

#include <string.h>
#include <stdio.h>


ierr s_setcap(String *s, isize cap, Alloc *a) {
    ierr e = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    s->l.itemsize = 1;
    e = l_setcap(&s->l, cap, a);
    if (s->l.cap > 0) {
        if (s->l.len >= s->l.cap) {
            s->l.len = s->l.cap - 1;
        }
        memset(s->l.buf.data + s->l.len, 0, s->l.cap - s->l.len); // ensure zeros at the end "\0"
    }
    return e;
}

static ierr _check_len_add_cap(String *s, isize newlen, Alloc *a) {
    if (s->l.cap < newlen) {
        return s_setcap(s, newlen, a);
    }
    return 0;
}

isize s_len(String *s, ierr *errptr) {
    CHECK_STRING_PTR(s, errptr, -1);
    return s->l.len;
}

isize s_cap(String *s, ierr *errptr) {
    CHECK_STRING_PTR(s, errptr, -1);
    return s->l.cap;
}

ierr s_clear(String *s) {
    ierr e = l_clear(&s->l);
    CHECK_STRING_PTR(s, &e, e);
    *(char *)(s->l.buf.data + s->l.len * s->l.itemsize) = '\0';
    return e;
}

char *s_get(String *s, ierr *errptr) {
    CHECK_STRING_PTR(s, errptr, nil);
    return (char *)s->l.buf.data;
}

ierr s_cat(String *s, const char *cstr, Alloc *a) {
    ierr e = 0;
    isize cstr_len = 0;
    isize new_len = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    CHECK_STRING_PTR(cstr, &e, e);

    cstr_len = strlen(cstr);
    // TODO: MAX check?
    new_len = s->l.len + cstr_len;

    e = _check_len_add_cap(s, new_len, a);
    if (e != 0) {
        return e;
    }

    memcpy(s->l.buf.data + s->l.len * s->l.itemsize, cstr, cstr_len);
    s->l.len += cstr_len;

    return e;
}

ierr s_cat_cstr(String *s, char *cstr, Alloc *a) {
    ierr e = 0;
    isize cstr_len = 0;
    isize new_len = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);
    CHECK_STRING_PTR(cstr, &e, e);

    cstr_len = strlen(cstr);
    // TODO: MAX check?
    new_len = s->l.len + cstr_len;

    e = _check_len_add_cap(s, new_len, a);
    if (e != 0) {
        return e;
    }

    memcpy(s->l.buf.data + s->l.len * s->l.itemsize, cstr, cstr_len);
    s->l.len += cstr_len;

    return e;
}

ierr s_cat_char(String *s, char c, Alloc *a) {
    ierr e = 0;
    isize new_len = 0;
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    new_len = s->l.len + 1;

    e = _check_len_add_cap(s, new_len, a);
    if (e != 0) {
        return e;
    }

    *(char *)(s->l.buf.data + s->l.len * s->l.itemsize) = c;
    s->l.len += 1;

    return e;
}

ierr s_cat_f32(String *s, f32 f, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%f", f);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_f64(String *s, f64 f, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%lf", f);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i8(String *s, i8 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%d", (int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i16(String *s, i16 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%d", (int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i32(String *s, i32 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%d", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_i64(String *s, i64 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%ld", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_isize(String *s, isize i, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%ld", i);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u8(String *s, u8 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%u", (unsigned int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u16(String *s, u16 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%u", (unsigned int)x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u32(String *s, u32 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%u", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_u64(String *s, u64 x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%lu", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}

ierr s_cat_usize(String *s, usize x, Alloc *a) {
    ierr e = 0;
    char buffer [M_STRING_NUMERIC_CAT_BUF_SIZE];
    int len; (void)len; len = sprintf(buffer, "%zu", x);
    CHECK_GET_ALLOCATOR(a);
    CHECK_STRING_PTR(s, &e, e);

    e = s_cat_cstr(s, buffer, a);

    return e;
}