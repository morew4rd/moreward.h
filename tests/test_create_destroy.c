#include "utest.h"
#include "tests.h"


// Test for m_Buffer create and destroy
UTEST(m_Buffer, create_destroy) {
    IErr err = 0;
    m_Buffer* buffer = mb_create(sizeof(I32), 10, NULL, &err);
    ASSERT_EQ(0, err);
    ASSERT_NE(NULL, buffer);
    mb_destroy(buffer, &err);
    ASSERT_EQ(0, err);
}

// Test for m_List create and destroy
UTEST(m_List, create_destroy) {
    IErr err = 0;
    m_List* list = ml_create(sizeof(I32), 10, NULL, NULL, &err);
    ASSERT_EQ(0, err);
    ASSERT_NE(NULL, list);
    ml_destroy(list, &err);
    ASSERT_EQ(0, err);
}

// Test for m_Dict create and destroy
UTEST(m_Dict, create_destroy) {
    IErr err = 0;
    m_Dict* dict = md_create(sizeof(I32), sizeof(I32), 10, NULL, NULL, &err);
    ASSERT_EQ(0, err);
    ASSERT_NE(NULL, dict);
    md_destroy(dict, &err);
    ASSERT_EQ(0, err);
}

// Test for m_StrBuffer create and destroy
UTEST(m_StrBuffer, create_destroy) {
    IErr err = 0;
    m_StrBuffer* strbuffer = ms_create(10, NULL, &err);
    ASSERT_EQ(0, err);
    ASSERT_NE(NULL, strbuffer);
    ms_destroy(strbuffer, &err);
    ASSERT_EQ(0, err);
}