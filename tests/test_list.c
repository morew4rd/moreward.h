#include "utest.h"
#include "tests.h"
#include <string.h>

// Define a comparer function for integer items
static I32 int_comparer(void* item1, void* item2) {
    int* a = (int*)item1;
    int* b = (int*)item2;
    return *a - *b;
}

// Test initialization of the list
UTEST(List, Init) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 10, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(list.buffer.itemsize, sizeof(int));
    ASSERT_EQ(list.buffer.itemcap, 10);
    ASSERT_EQ(list.count, 0);
    ASSERT_NE(list.buffer.data, NULL);
    ml_setcap(&list, 0, &err);
}

// Test pushing elements into the list
UTEST(List, Push) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(ml_count(&list, &err), 1);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(ml_count(&list, &err), 2);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(list.buffer.itemcap, 2); // Capacity should double
    ml_setcap(&list, 0, &err);
}

// Test popping elements from the list
UTEST(List, Pop) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    int* popped = (int*)ml_pop(&list, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*popped, item2);
    ASSERT_EQ(ml_count(&list, &err), 1);
    ASSERT_EQ(err, 0);
    popped = (int*)ml_pop(&list, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*popped, item1);
    ASSERT_EQ(ml_count(&list, &err), 0);
    ASSERT_EQ(err, 0);
    ml_setcap(&list, 0, &err);
}

// Test setting capacity of the list
UTEST(List, SetCap) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    ml_setcap(&list, 20, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(list.buffer.itemcap, 20);
    ml_setcap(&list, 0, &err);
}

// Test getting elements from the list
UTEST(List, Get) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    int* get1 = (int*)ml_get(&list, 0, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get1, item1);
    int* get2 = (int*)ml_get(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get2, item2);
    ml_setcap(&list, 0, &err);
}

// Test inserting elements into the list
UTEST(List, Insert) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    ml_insert(&list, 1, &item3, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(ml_count(&list, &err), 3);
    ASSERT_EQ(err, 0);
    int* get1 = (int*)ml_get(&list, 0, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get1, item1);
    int* get2 = (int*)ml_get(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get2, item3);
    int* get3 = (int*)ml_get(&list, 2, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get3, item2);
    ml_setcap(&list, 0, &err);
}

// Test removing elements from the list
UTEST(List, Remove) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item3, &err);
    ASSERT_EQ(err, 0);
    ml_remove(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(ml_count(&list, &err), 2);
    ASSERT_EQ(err, 0);
    int* get1 = (int*)ml_get(&list, 0, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get1, item1);
    int* get2 = (int*)ml_get(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get2, item3);
    ml_setcap(&list, 0, &err);
}

// Test removing elements by swapping from the list
UTEST(List, RemoveSwap) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item3, &err);
    ASSERT_EQ(err, 0);
    ml_remove_swap(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(ml_count(&list, &err), 2);
    ASSERT_EQ(err, 0);
    int* get1 = (int*)ml_get(&list, 0, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get1, item1);
    int* get2 = (int*)ml_get(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get2, item3);
    ml_setcap(&list, 0, &err);
}

// Test finding elements in the list
UTEST(List, Find) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 1;
    int item2 = 2;
    int item3 = 3;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item3, &err);
    ASSERT_EQ(err, 0);
    int key = 2;
    I32 index = ml_find(&list, &key, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(index, 1);
    key = 4;
    index = ml_find(&list, &key, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(index, -1);
    ml_setcap(&list, 0, &err);
}

UTEST(List, Sort) {
    m_List list;
    IErr err = 0;
    ml_init(&list, sizeof(int), 1, int_comparer, NULL, &err);
    ASSERT_EQ(err, 0);
    int item1 = 3;
    int item2 = 1;
    int item3 = 2;
    ml_push(&list, &item1, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item2, &err);
    ASSERT_EQ(err, 0);
    ml_push(&list, &item3, &err);
    ASSERT_EQ(err, 0);
    ml_sort(&list, &err);
    ASSERT_EQ(err, 0);
    int* get1 = (int*)ml_get(&list, 0, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get1, item2);
    int* get2 = (int*)ml_get(&list, 1, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get2, item3);
    int* get3 = (int*)ml_get(&list, 2, &err);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(*get3, item1);
    ml_setcap(&list, 0, &err);
}