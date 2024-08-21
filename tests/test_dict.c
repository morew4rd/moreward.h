#include "utest.h"
#include "tests.h"
#include <string.h>

// Define a comparer function for integer items
static I32 int_comparer(void* item1, void* item2) {
    int* a = (int*)item1;
    int* b = (int*)item2;
    return *a - *b;
}

// Test initialization of the dictionary
UTEST(Dict, Init) {
    m_Dict dict;
    IErr err = 0;
    md_init(&dict, sizeof(int), sizeof(int), 10, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(dict.keys.buffer.itemsize, sizeof(int));
    ASSERT_EQ(dict.values.buffer.itemsize, sizeof(int));
    ASSERT_EQ(dict.keys.buffer.itemcap, 10);
    ASSERT_EQ(dict.values.buffer.itemcap, 10);
    ASSERT_EQ(dict.keys.count, 0);
    ASSERT_EQ(dict.values.count, 0);
    ASSERT_NE(dict.keys.buffer.data, NULL);
    ASSERT_NE(dict.values.buffer.data, NULL);
    md_setcap(&dict, 0, &err);
}

// Test putting elements into the dictionary
UTEST(Dict, Put) {
    m_Dict dict;
    IErr err = 0;
    md_init(&dict, sizeof(int), sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int key1 = 1;
    int value1 = 10;
    int key2 = 2;
    int value2 = 20;
    md_put(&dict, &key1, &value1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(dict.keys.count, 1);
    ASSERT_EQ(dict.values.count, 1);
    md_put(&dict, &key2, &value2, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(dict.keys.count, 2);
    ASSERT_EQ(dict.values.count, 2);
    ASSERT_EQ(dict.keys.buffer.itemcap, 2); // Capacity should double
    ASSERT_EQ(dict.values.buffer.itemcap, 2); // Capacity should double
    md_setcap(&dict, 0, &err);
}

// Test getting elements from the dictionary
UTEST(Dict, Get) {
    m_Dict dict;
    IErr err = 0;
    md_init(&dict, sizeof(int), sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int key1 = 1;
    int value1 = 10;
    int key2 = 2;
    int value2 = 20;
    md_put(&dict, &key1, &value1, &err);
    ASSERT_EQ(err, 0);
    md_put(&dict, &key2, &value2, &err);
    ASSERT_EQ(err, 0);
    int* get1 = (int*)md_get(&dict, &key1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get1, value1);
    int* get2 = (int*)md_get(&dict, &key2, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get2, value2);
    md_setcap(&dict, 0, &err);
}

// Test checking if the dictionary has a key
UTEST(Dict, Has) {
    m_Dict dict;
    IErr err = 0;
    md_init(&dict, sizeof(int), sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int key1 = 1;
    int value1 = 10;
    int key2 = 2;
    int value2 = 20;
    md_put(&dict, &key1, &value1, &err);
    ASSERT_EQ(err, 0);
    md_put(&dict, &key2, &value2, &err);
    ASSERT_EQ(err, 0);
    ASSERT_TRUE(md_has(&dict, &key1, &err));
    ASSERT_EQ(err, 0);
    ASSERT_TRUE(md_has(&dict, &key2, &err));
    ASSERT_EQ(err, 0);
    int key3 = 3;
    ASSERT_FALSE(md_has(&dict, &key3, &err));
    ASSERT_EQ(err, 0);
    md_setcap(&dict, 0, &err);
}

// Test removing elements from the dictionary
UTEST(Dict, Remove) {
    m_Dict dict;
    IErr err = 0;
    md_init(&dict, sizeof(int), sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int key1 = 1;
    int value1 = 10;
    int key2 = 2;
    int value2 = 20;
    md_put(&dict, &key1, &value1, &err);
    ASSERT_EQ(err, 0);
    md_put(&dict, &key2, &value2, &err);
    ASSERT_EQ(err, 0);
    md_remove(&dict, &key1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(dict.keys.count, 1);
    ASSERT_EQ(dict.values.count, 1);
    ASSERT_FALSE(md_has(&dict, &key1, &err));
    ASSERT_EQ(err, 0);
    ASSERT_TRUE(md_has(&dict, &key2, &err));
    ASSERT_EQ(err, 0);
    md_setcap(&dict, 0, &err);
}

// Test removing elements (keeping the order)
UTEST(Dict, RemoveOrdered) {
    m_Dict dict;
    IErr err = 0;
    md_init(&dict, sizeof(int), sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int key1 = 1;
    int value1 = 10;
    int key2 = 2;
    int value2 = 20;
    md_put(&dict, &key1, &value1, &err);
    ASSERT_EQ(err, 0);
    md_put(&dict, &key2, &value2, &err);
    ASSERT_EQ(err, 0);
    md_remove_ordered(&dict, &key1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(dict.keys.count, 1);
    ASSERT_EQ(dict.values.count, 1);
    ASSERT_FALSE(md_has(&dict, &key1, &err));
    ASSERT_EQ(err, 0);
    ASSERT_TRUE(md_has(&dict, &key2, &err));
    ASSERT_EQ(err, 0);
    md_setcap(&dict, 0, &err);
}