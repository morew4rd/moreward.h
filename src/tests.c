#include "utest.h"

#ifdef UNIT_TESTS
#include "mg.h"
#else
#define MOREWARD_IMPL
#include "../moreward.h"
#endif


UTEST(Scratch, Logging) {
    IErr errß = 0;
    m_log("hello %d", 10);
    m_log_info("hello again %d", 10);
    m_log_trace("hello (trace) you shouldn't see this %d", 10);
    m_set_loglevel(M_LOG_TRACE);
    m_log_trace("hello (trace) you see this %d", 10);
}


// Utility function to compare integers for list and dictionary operations
static I32 int_comparer(Void* item1, Void* item2) {
    I32 a = *(I32*)item1;
    I32 b = *(I32*)item2;
    return (a > b) - (a < b);
}

#pragma region Buffer Tests
// Tests for m_Buffer, the foundational dynamic array structure

UTEST(Buffer, CreationAndInitialization) {
    m_Buffer* buffer = mb_create(sizeof(I32), 10);
    ASSERT_NE(buffer, NULL);              // Ensure buffer is created
    ASSERT_EQ(buffer->itemsize, sizeof(I32)); // Verify item size
    ASSERT_EQ(buffer->itemcap, 10);       // Verify initial capacity
    mb_destroy(buffer);                   // Clean up
}

UTEST(Buffer, SetCapacity) {
    m_Buffer* buffer = mb_create(sizeof(I32), 10);
    mb_setcap(buffer, 20);                // Increase capacity
    ASSERT_EQ(buffer->itemcap, 20);       // Check new capacity
    mb_setcap(buffer, 0);                 // Decrease to zero
    ASSERT_EQ(buffer->data, NULL);        // Ensure data is freed
    mb_destroy(buffer);                   // Clean up
}
#pragma endregion

#pragma region List Tests
// Tests for m_List, a dynamic array with additional functionality

UTEST(List, CreationAndInitialization) {
    m_List* list = ml_create(sizeof(I32), 10, int_comparer);
    ASSERT_NE(list, NULL);                // Ensure list is created
    ASSERT_EQ(list->count, 0);            // Verify initial count
    ml_destroy(list);                     // Clean up
}

UTEST(List, PushAndGet) {
    m_List* list = ml_create(sizeof(I32), 10, int_comparer);
    I32 value = 42;
    ml_push(list, &value);                // Add an item
    ASSERT_EQ(list->count, 1);            // Check count
    I32* retrieved = (I32*)ml_get(list, 0);
    ASSERT_EQ(*retrieved, 42);            // Verify value
    ml_destroy(list);                     // Clean up
}

UTEST(List, Pop) {
    m_List* list = ml_create(sizeof(I32), 10, int_comparer);
    I32 value = 42;
    ml_push(list, &value);                // Add an item
    I32* popped = (I32*)ml_pop(list);     // Remove and retrieve item
    ASSERT_EQ(*popped, 42);               // Verify popped value
    ASSERT_EQ(list->count, 0);            // Check count
    ml_destroy(list);                     // Clean up
}

UTEST(List, InsertAndRemove) {
    m_List* list = ml_create(sizeof(I32), 10, int_comparer);
    I32 values[] = {1, 2, 3};
    for (I32 i = 0; i < 3; ++i) {
        ml_push(list, &values[i]);        // Add initial items
    }
    I32 insert_value = 42;
    ml_insert(list, 1, &insert_value);    // Insert at index 1
    ASSERT_EQ(list->count, 4);            // Verify count
    I32* item = (I32*)ml_get(list, 1);
    ASSERT_EQ(*item, 42);                 // Check inserted value
    ml_remove(list, 1);                   // Remove at index 1
    ASSERT_EQ(list->count, 3);            // Verify count
    item = (I32*)ml_get(list, 1);
    ASSERT_EQ(*item, 2);                  // Check remaining value
    ml_destroy(list);                     // Clean up
}
#pragma endregion

#pragma region Dictionary Tests
// Tests for m_Dict, a key-value store using two m_List instances

UTEST(Dict, CreationAndInitialization) {
    m_Dict* dict = md_create(sizeof(I32), sizeof(Str), 10, int_comparer);
    ASSERT_NE(dict, NULL);                // Ensure dictionary is created
    ASSERT_EQ(md_count(dict), 0);         // Verify initial count
    md_destroy(dict);                     // Clean up
}

UTEST(Dict, PutAndGet) {
    m_Dict* dict = md_create(sizeof(I32), sizeof(Str), 10, int_comparer);
    I32 key = 1;
    Str value = "one";
    md_put(dict, &key, &value);           // Add key-value pair
    Str* retrieved = (Str*)md_get(dict, &key);
    ASSERT_NE(retrieved, NULL);           // Ensure value exists
    ASSERT_EQ(*retrieved, "one");         // Verify value
    md_destroy(dict);                     // Clean up
}

UTEST(Dict, HasAndRemove) {
    m_Dict* dict = md_create(sizeof(I32), sizeof(Str), 10, int_comparer);
    I32 key = 1;
    Str value = "one";
    md_put(dict, &key, &value);           // Add key-value pair
    ASSERT_TRUE(md_has(dict, &key));      // Check key exists
    md_remove(dict, &key);                // Remove key
    ASSERT_FALSE(md_has(dict, &key));     // Verify key is removed
    md_destroy(dict);                     // Clean up
}

UTEST(Dict, UpdateExistingKey) {
    m_Dict* dict = md_create(sizeof(I32), sizeof(Str), 10, int_comparer);
    I32 key = 1;
    Str value1 = "one";
    Str value2 = "uno";
    md_put(dict, &key, &value1);          // Add initial value
    md_put(dict, &key, &value2);          // Update value for same key
    Str* retrieved = (Str*)md_get(dict, &key);
    ASSERT_EQ(*retrieved, "uno");         // Verify updated value
    md_destroy(dict);                     // Clean up
}
#pragma endregion

#pragma region String Buffer Tests
// Tests for m_StrBuffer, a specialized string handling structure

UTEST(StrBuffer, CreationAndInitialization) {
    m_StrBuffer* sb = ms_create(100);
    ASSERT_NE(sb, NULL);                  // Ensure string buffer is created
    ASSERT_EQ(sb->length, 0);             // Verify initial length
    ms_destroy(sb);                       // Clean up
}

UTEST(StrBuffer, Concatenation) {
    m_StrBuffer* sb = ms_create(10);
    ms_cat(sb, "Hello");                  // Add first string
    ms_cat(sb, ", world!");               // Add second string
    ASSERT_EQ(strcmp(ms_getstr(sb), "Hello, world!"), 0); // Verify result
    ms_destroy(sb);                       // Clean up
}

UTEST(StrBuffer, Trim) {
    m_StrBuffer* sb = ms_create(100);
    ms_cat(sb, "   Hello, world!   ");    // Add string with whitespace
    ms_trim(sb);                          // Trim whitespace
    ASSERT_EQ(strcmp(ms_getstr(sb), "Hello, world!"), 0); // Verify result
    ms_destroy(sb);                       // Clean up
}

UTEST(StrBuffer, FindSubstring) {
    m_StrBuffer* sb = ms_create(100);
    ms_cat(sb, "Hello, world!");          // Set string
    I32 pos = ms_find(sb, "world");       // Find substring
    ASSERT_EQ(pos, 7);                    // Verify position
    pos = ms_find(sb, "notfound");        // Find non-existent substring
    ASSERT_EQ(pos, -1);                   // Verify not found
    ms_destroy(sb);                       // Clean up
}
#pragma endregion

#pragma region Logging Tests
// Tests for the logging system

UTEST(Logging, LogLevels) {
    m_set_loglevel(M_LOG_INFO);           // Set level to INFO
    m_log_trace("Trace: shouldn't see this %d", 10); // Should not log
    m_log_info("Info: should see this %d", 10);      // Should log
    m_set_loglevel(M_LOG_TRACE);          // Set level to TRACE
    m_log_trace("Trace: should see this now %d", 10); // Should log now
    // Note: Output verification requires capturing stdout, omitted here
}

UTEST(Logging, AllLevels) {
    m_set_loglevel(M_LOG_TRACE);          // Set to lowest level
    m_log_trace("Trace message %d", 1);
    m_log_info("Info message %d", 2);
    m_log_warn("Warn message %d", 3);
    m_log_error("Error message %d", 4);
    m_log_fatal("Fatal message %d", 5);
    // Output assumed correct if functions execute
}
#pragma endregion

UTEST_MAIN();