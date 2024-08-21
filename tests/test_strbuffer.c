#include "utest.h"
#include "tests.h"

// Define a test case for initializing a StrBuffer
UTEST(StrBuffer, Init) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 10, NULL, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(strbuffer.buffer.itemcap, 11);
    ASSERT_EQ(strbuffer.length, 0);
    // Additional checks can be added here
}

// Define a test case for setting the capacity of a StrBuffer
UTEST(StrBuffer, SetCap) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 10, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_setcap(&strbuffer, 20, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(strbuffer.buffer.itemcap, 21);
    // Additional checks can be added here
}

// Define a test case for clearing a StrBuffer
UTEST(StrBuffer, Clear) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 10, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "Hello, World!", &err);
    ASSERT_EQ(err, 0);
    ms_clear(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(strbuffer.length, 0);
    // Additional checks can be added here
}

// Define a test case for getting the string from a StrBuffer
UTEST(StrBuffer, GetStr) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 10, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "Hello, World!", &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Hello, World!");
    // Additional checks can be added here
}

// Define a test case for concatenating strings to a StrBuffer
UTEST(StrBuffer, Cat) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 10, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "Hello", &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, ", World!", &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Hello, World!");
    // Additional checks can be added here
}

// Define a test case for concatenating formatted strings to a StrBuffer
UTEST(StrBuffer, CatFormatted) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 10, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "Number: %d", 42, &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Number: 42");
    // Additional checks can be added here
}

UTEST(StrBuffer, ClearNoInit) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_cat(&strbuffer, "Hello, World!", &err);
    ASSERT_EQ(err, 0);
    ms_clear(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(strbuffer.length, 0);
}

// Define a test case for getting the string from a StrBuffer without initialization
UTEST(StrBuffer, GetStrNoInit) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_cat(&strbuffer, "Hello, World!", &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Hello, World!");
}

// Define a test case for concatenating strings to a StrBuffer without initialization
UTEST(StrBuffer, CatNoInit) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_cat(&strbuffer, "Hello", &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, ", World!", &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Hello, World!");
}

// Define a test case for concatenating formatted strings to a StrBuffer without initialization
UTEST(StrBuffer, CatFormattedNoInit) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_cat(&strbuffer, "Number: %d", 42, &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Number: 42");
}

UTEST(StrBuffer, Trim) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 20, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "   Hello, World!   ", &err);
    ASSERT_EQ(err, 0);
    ms_trim(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&strbuffer, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "Hello, World!");
}

UTEST(StrBuffer, Substr) {
    m_StrBuffer strbuffer = {0};
    m_StrBuffer dest = {0};
    IErr err = 0;
    ms_init(&strbuffer, 20, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "Hello, World!", &err);
    ASSERT_EQ(err, 0);
    ms_substr(&strbuffer, 7, 5, &dest, &err);
    ASSERT_EQ(err, 0);
    char* str = ms_getstr(&dest, &err);
    ASSERT_EQ(err, 0);
    ASSERT_STREQ(str, "World");
}

UTEST(StrBuffer, Find) {
    m_StrBuffer strbuffer = {0};
    IErr err = 0;
    ms_init(&strbuffer, 20, NULL, &err);
    ASSERT_EQ(err, 0);
    ms_cat(&strbuffer, "Hello, World!", &err);
    ASSERT_EQ(err, 0);
    I32 index = ms_find(&strbuffer, "World", &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(index, 7);
}