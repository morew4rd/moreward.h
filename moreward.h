#ifndef _MOREWARD_H_
#define _MOREWARD_H_

/*

License (MIT) is at the EOF .

# moreward.h

## Overview

moreward.h is a lightweight, single-header C library designed to provide essential data structures and utility functions. It includes dynamic arrays (lists), key-value stores (dictionaries), string buffers, and a flexible logging system. The library supports custom memory allocators for precise control over memory management, making it suitable for performance-critical applications or resource-constrained environments.

## Setup

To use moreward.h, define MOREWARD_IMPL in exactly one C file before including the header to include the implementation:

```c
#define MOREWARD_IMPL
#include "moreward.h"
```

In all other C files, include the header normally without the definition:

```c
#include "moreward.h"
```

**Note:** Alternatively, you can use the mg.h (header) and mg.c (implementation) files from src directly. Include mg.c in your build process, and the rest of this documentation still applies.

## Types

### Primitive Types
- Signed Integers: `I8, I16, I32, I64` (8, 16, 32, 64 bits)
- Unsigned Integers: `U8, U16, U32, U64` (8, 16, 32, 64 bits)
- Floating-Point: `F32, F64` (32, 64 bits)
- Boolean: `Bool` (true/false)
- Strings: `Str` (mutable char*), `CStr` (immutable const char*)
- Sizes: `USize` (unsigned size_t), `ISize` (signed ptrdiff_t)
- Error Code: `IErr` (int32_t, 0 for success, non-zero for errors)

### Structures
- `m_Allocator`: Custom memory allocator with function pointers for malloc, realloc, free, and a userdata pointer for context.
- `m_Buffer`: Generic dynamic buffer for raw data, with item size and capacity.
- `m_List`: Dynamic array built on m_Buffer, with an optional item comparison function.
- `m_Dict`: Key-value store using two m_Lists—one for keys, one for values.
- `m_StrBuffer`: String buffer for efficient manipulation, built on m_Buffer.

### Enumerations
- `m_LogLevel`: Logging severity levels:
  - `M_LOG_TRACE`: Verbose debugging
  - `M_LOG_INFO`: General updates
  - `M_LOG_WARN`: Potential problems
  - `M_LOG_ERROR`: Recoverable errors
  - `M_LOG_FATAL`: Critical failures

## Memory Management

The library uses a global allocator for all memory operations, which can be customized for specific needs (e.g., memory pools or tracking).

- `m_set_allocator(m_Allocator* allocator)`: Sets a custom allocator.
- `m_reset_allocator()`: Reverts to the default allocator (standard malloc/realloc/free).
- `m_get_allocator()`: Retrieves the current allocator.

### Custom Allocator Example

```c
void* my_malloc(USize size, void* userdata) {  }
void* my_realloc(void* ptr, USize newsize, void* userdata) {  }
void my_free(void* ptr, void* userdata) {  }

m_Allocator custom = {
    .malloc = my_malloc,
    .realloc = my_realloc,
    .free = my_free,
    .userdata = NULL  // Optional context
};

m_set_allocator(&custom);
```

## Error Handling

Functions that may fail return an IErr code directly (0 indicates success, non-zero indicates an error). Check the return value to handle errors:

```c
IErr err = ml_push(&list, &item);
if (err != 0) {
    m_log_error("Failed to push item: error code %d", err);
}
```

## Data Structures

### Buffer (m_Buffer)
A low-level dynamic buffer for raw data.

-` m_Buffer mb_create(I32 itemsize, I32 itemcap)`: Creates a buffer with specified item size and capacity.
- `void mb_destroy(m_Buffer* buffer)`: Frees the buffer’s memory.
- `void mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap)`: Initializes an existing buffer.
- `IErr mb_setcap(m_Buffer* buffer, I32 newcap)`: Resizes the buffer’s capacity.

### List (m_List)
A dynamic array with flexible operations.

- `m_List ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer)`: Creates a list with an optional comparison function for sorting/finding.
- `void ml_destroy(m_List* list)`: Frees the list.
- `void ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer)`: Initializes an existing list.
- `IErr ml_push(m_List* list, void* item)`: Appends an item.
- `void* ml_pop(m_List* list)`: Removes and returns the last item.
- `I32 ml_count(m_List* list)`: Returns the number of items.
- `void* ml_get(m_List* list, I32 index)`: Retrieves an item by index.
- `IErr ml_insert(m_List* list, I32 index, void* item)`: Inserts an item at an index.
- `IErr ml_remove(m_List* list, I32 index)`: Removes an item at an index.
- `IErr ml_remove_range(m_List* list, I32 startindex, I32 count)`: Removes a range of items.
- `IErr ml_remove_swap(m_List* list, I32 index)`: Removes an item by swapping with the last (faster, unordered).
- `I32 ml_find(m_List* list, void* item)`: Returns the index of an item (or -1 if not found).
- `void ml_clear(m_List* list)`: Removes all items.
- `void ml_sort(m_List* list)`: Sorts the list using the provided comparer.

### Dictionary (m_Dict)
A key-value store implemented with two lists.

- `m_Dict md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer)`: Creates a dictionary.
- `void md_destroy(m_Dict* dict)`: Frees the dictionary.
- `void md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer)`: Initializes an existing dictionary.
- `void* md_get(m_Dict* dict, void* key)`: Retrieves a value by key (NULL if not found).
- `IErr md_put(m_Dict* dict, void* key, void* value)`: Adds or updates a key-value pair.
- `Bool md_has(m_Dict* dict, void* key)`: Checks if a key exists.
- `IErr md_remove(m_Dict* dict, void* key)`: Removes a key-value pair (unordered).
- `IErr md_remove_ordered(m_Dict* dict, void* key)`: Removes a key-value pair while preserving order.
- `void md_clear(m_Dict* dict)`: Removes all entries.

### String Buffer (m_StrBuffer)
A dynamic string buffer for efficient manipulation.

- `m_StrBuffer ms_create(I32 itemcap)`: Creates a string buffer with initial capacity.
- `void ms_destroy(m_StrBuffer* strbuffer)`: Frees the string buffer.
- `void ms_init(m_StrBuffer* strbuffer, I32 itemcap)`: Initializes an existing string buffer.
- `IErr ms_setcap(m_StrBuffer* strbuffer, I32 newcap)`: Resizes the capacity.
- `void ms_clear(m_StrBuffer* strbuffer)`: Clears the contents.
- `char* ms_getstr(m_StrBuffer* strbuffer)`: Returns the current string.
- `void ms_cat(m_StrBuffer* strbuffer, const char* format, ...)`: Appends a formatted string.
- `void ms_trim(m_StrBuffer* strbuffer)`: Trims leading/trailing whitespace.
- `IErr ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest)`: Extracts a substring into another buffer.
- `I32 ms_find(m_StrBuffer* strbuffer, const char* substring)`: Finds the position of a substring (or -1 if not found).

## Logging System

The logging system provides configurable severity levels for debugging and monitoring.

### Log Levels
- `M_LOG_TRACE`: Verbose debugging
- `M_LOG_INFO`: General updates
- `M_LOG_WARN`: Potential problems
- `M_LOG_ERROR`: Recoverable errors
- `M_LOG_FATAL`: Critical failures

### Functions and Macros
- `void m_set_loglevel(m_LogLevel level)`: Sets the minimum log level to display.
- `void m_log_raw(m_LogLevel level, const char* format, ...)`: Logs a raw message.
- Convenience Macros:
  - `m_log(format, ...)`: Logs at INFO level.
  - `m_log_trace(format, ...)`: Logs at TRACE level.
  - `m_log_info(format, ...)`: Logs at INFO level.
  - `m_log_warn(format, ...)`: Logs at WARN level.
  - `m_log_error(format, ...)`: Logs at ERROR level.
  - `m_log_fatal(format, ...)`: Logs at FATAL level.

## Other macros

- `m_alloc(n)`: Allocates n bytes using the current allocator.
- `m_realloc(p, n)`: Reallocates memory to n bytes.
- `m_free(p)`: Frees allocated memory.
- `m_countof(a)`: Returns the number of elements in a static array.
- `m_max(a, b)`: Returns the maximum of two values.
- `m_min(a, b)`: Returns the minimum of two values.


*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define m_assert(x)     assert(x)
#else
#define m_assert(x)
#endif

#define nil NULL

typedef void        Void;
typedef void*       VPtr;
typedef bool        Bool;
typedef int8_t      I8;
typedef int16_t     I16;
typedef int32_t     I32;
typedef int64_t     I64;
typedef uint8_t     U8;
typedef uint16_t    U16;
typedef uint32_t    U32;
typedef uint64_t    U64;
typedef float       F32;
typedef double      F64;
typedef char*       Str;
typedef const char* CStr;
typedef size_t      USize;
typedef ptrdiff_t   ISize;
typedef int32_t     IErr;

#define m_countof(a)                (size_t)(sizeof(a) / sizeof(*(a)))
#define m_max(a, b)                 ((a)>(b) ? (a) : (b))
#define m_min(a, b)                 ((a)<(b) ? (a) : (b))

#define m_alloc(n)                  m_get_allocator()->malloc((n), m_get_allocator()->userdata)
#define m_realloc(p,n)              m_get_allocator()->realloc((p), (n), m_get_allocator()->userdata)
#define m_free(p)                   m_get_allocator()->free((p), m_get_allocator()->userdata)

#define m_tracelog(level, file, line, format, ...) \
                                    m_log_raw(level, "[%s:%d] " format, file, line, ##__VA_ARGS__)
#define m_log(format, ...)          m_log_raw(M_LOG_INFO, format, ##__VA_ARGS__)
#define m_log_trace(format, ...)    m_tracelog(M_LOG_TRACE, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_info(format, ...)     m_tracelog(M_LOG_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_warn(format, ...)     m_tracelog(M_LOG_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_error(format, ...)    m_tracelog(M_LOG_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_fatal(format, ...)    m_tracelog(M_LOG_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

enum {
    M_ERR_NULL_POINTER = 1000000,
    M_ERR_OUT_OF_BOUNDS,
    M_ERR_INVALID_OPERATION,
};

typedef struct m_Allocator {
    void* (*malloc)(size_t size, void* userdata);
    void* (*realloc)(void* ptr, size_t new_size, void* userdata);
    void  (*free)(void* ptr, void* userdata);
    void* userdata;
} m_Allocator;

typedef struct m_Buffer {
    U8* data;
    I32 itemsize;
    I32 itemcap;
    m_Allocator* allocator;
} m_Buffer;

typedef I32 (*m_ItemComparer)(void* item1, void* item2);

typedef struct m_List {
    m_Buffer buffer;
    I32 count;
    m_ItemComparer comparer;
} m_List;

typedef struct m_Dict {
    m_List keys;
    m_List values;
} m_Dict;

typedef struct m_StrBuffer {
    m_Buffer buffer;
    I32 length;
} m_StrBuffer;

typedef enum m_LogLevel {
    M_LOG_TRACE,
    M_LOG_INFO,
    M_LOG_WARN,
    M_LOG_ERROR,
    M_LOG_FATAL
} m_LogLevel;

void m_set_loglevel(m_LogLevel level);
void m_log_raw(m_LogLevel level, const char* format, ...);

void m_set_allocator(m_Allocator *allocator);
void m_reset_allocator(void);
m_Allocator *m_get_allocator(void);

// Buffer functions
m_Buffer* mb_create(I32 itemsize, I32 itemcap);
void mb_destroy(m_Buffer* buffer);
void mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap);
void mb_setcap(m_Buffer* buffer, I32 newcap);

// List functions
m_List* ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer);
void ml_destroy(m_List* list);
void ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer);
void ml_setcap(m_List* list, I32 newcap);
void ml_clear(m_List* list);
IErr ml_push(m_List* list, void* item);
void* ml_pop(m_List* list);
void* ml_get(m_List* list, I32 index);
IErr ml_put(m_List* list, I32 index, void* item);
IErr ml_insert(m_List* list, I32 index, void* item);
IErr ml_remove(m_List* list, I32 index);
IErr ml_remove_range(m_List* list, I32 startindex, I32 count);
IErr ml_remove_swap(m_List* list, I32 index);
I32 ml_count(m_List* list);
I32 ml_find(m_List* list, void* item);
void ml_sort(m_List* list);

// Dictionary functions
m_Dict* md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer);
void md_destroy(m_Dict* dict);
void md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer);
void md_setcap(m_Dict* dict, I32 newcap);
void md_clear(m_Dict* dict);
void* md_get(m_Dict* dict, void* key);
IErr md_put(m_Dict* dict, void* key, void* value);
Bool md_has(m_Dict* dict, void* key);
IErr md_remove(m_Dict* dict, void* key);
IErr md_remove_ordered(m_Dict* dict, void* key);
I32 md_count(m_Dict* dict);

// String Buffer functions
m_StrBuffer* ms_create(I32 itemcap);
void ms_destroy(m_StrBuffer* strbuffer);
void ms_init(m_StrBuffer* strbuffer, I32 itemcap);
void ms_setcap(m_StrBuffer* strbuffer, I32 newcap);
void ms_clear(m_StrBuffer* strbuffer);
char* ms_getstr(m_StrBuffer* strbuffer);
void ms_cat(m_StrBuffer* strbuffer, const char* format, ...);
void ms_trim(m_StrBuffer* strbuffer);
IErr ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest);
I32 ms_find(m_StrBuffer* strbuffer, const char* substring);



/* IMPLEMENTATION */


#ifdef MOREWARD_IMPL

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h> // For isspace

// Default malloc function
static void* _default_malloc(size_t size, void* userdata) {
    return malloc(size);
}

// Default realloc function
static void* _default_realloc(void* ptr, size_t new_size, void* userdata) {
    return realloc(ptr, new_size);
}

// Default free function
static void _default_free(void* ptr, void* userdata) {
    free(ptr);
}

// Define the default allocator
static m_Allocator _default_allocator = {
    .malloc = _default_malloc,
    .realloc = _default_realloc,
    .free = _default_free,
    .userdata = NULL
};

static m_Allocator *_current_allocator = &_default_allocator;

// Allocator management functions
void m_set_allocator(m_Allocator *allocator) {
    _current_allocator = allocator;
}

void m_reset_allocator(void) {
    _current_allocator = &_default_allocator;
}

m_Allocator *m_get_allocator() {
    return _current_allocator;
}

// Helper function to free buffer data
static void _buffer_free_data(m_Buffer* buffer) {
    if (buffer->data) {
        buffer->allocator->free(buffer->data, buffer->allocator->userdata);
        buffer->data = NULL;
        buffer->itemcap = 0;
    }
}

// Buffer functions
m_Buffer* mb_create(I32 itemsize, I32 itemcap) {
    m_Buffer* buffer = (m_Buffer*)m_alloc(sizeof(m_Buffer));
    if (!buffer) {
        return nil;
    }
    mb_init(buffer, itemsize, itemcap);
    return buffer;
}

void mb_destroy(m_Buffer* buffer) {
    if (!buffer) {
        return;
    }
    mb_setcap(buffer, 0);
    m_free(buffer);
}

void mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap) {
    buffer->allocator = m_get_allocator();
    buffer->data = NULL;
    buffer->itemsize = itemsize;
    buffer->itemcap = 0;
    mb_setcap(buffer, itemcap);
}

void mb_setcap(m_Buffer* buffer, I32 newcap) {
    if (newcap == 0) {
        _buffer_free_data(buffer);
        return;
    }

    if (buffer->itemsize == 0) {
        buffer->itemsize = 1;  // Assume 1 byte if not set, mainly for string buffers
    }

    I32 new_size = buffer->itemsize * newcap;
    I32 old_size = buffer->itemsize * buffer->itemcap;

    if (buffer->data) {
        U8* new_data = (U8*)buffer->allocator->realloc(buffer->data, new_size, buffer->allocator->userdata);
        if (new_data) {
            buffer->data = new_data;
            buffer->itemcap = newcap;
            if (new_size > old_size) {
                memset(buffer->data + old_size, 0, new_size - old_size);
            }
        } else {
            m_log_error("mb_setcap: realloc failed");
        }
    } else {
        buffer->data = (U8*)buffer->allocator->malloc(new_size, buffer->allocator->userdata);
        if (buffer->data) {
            buffer->itemcap = newcap;
            memset(buffer->data, 0, new_size);
        } else {
            m_log_error("mb_setcap: malloc failed");
        }
    }
}

// List functions
m_List* ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer) {
    m_List* list = (m_List*)m_alloc(sizeof(m_List));
    if (!list) {
        return nil;
    }
    ml_init(list, itemsize, itemcap, comparer);
    return list;
}

void ml_destroy(m_List* list) {
    if (!list) {
        return;
    }
    ml_clear(list);
    mb_setcap(&list->buffer, 0);
    m_free(list);
}

static I32 _default_comparer(void* item1, void* item2) {
    if (item1 == item2) return 0;
    return (item1 < item2) ? -1 : 1;
}

void ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer) {
    mb_init(&list->buffer, itemsize, itemcap);
    list->count = 0;
    list->comparer = comparer ? comparer : _default_comparer;
}

void ml_setcap(m_List* list, I32 newcap) {
    mb_setcap(&list->buffer, newcap);
}

IErr ml_push(m_List* list, void* item) {
    if (list->count >= list->buffer.itemcap) {
        I32 newcap = list->buffer.itemcap * 2 ?: 1; // Double or start at 1
        ml_setcap(list, newcap);
    }
    memcpy(list->buffer.data + (list->count * list->buffer.itemsize), item, list->buffer.itemsize);
    list->count++;
    return 0; // Success
}

void* ml_pop(m_List* list) {
    if (list->count == 0) {
        return NULL;
    }
    list->count--;
    return list->buffer.data + (list->count * list->buffer.itemsize);
}

I32 ml_count(m_List* list) {
    return list->count;
}

void* ml_get(m_List* list, I32 index) {
    if (index < 0 || index >= list->count) {
        return NULL;
    }
    return list->buffer.data + (index * list->buffer.itemsize);
}

IErr ml_put(m_List* list, I32 index, void* item) {
    if (index < 0 || index >= list->count) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    memcpy(list->buffer.data + (index * list->buffer.itemsize), item, list->buffer.itemsize);
    return 0; // Success
}

IErr ml_insert(m_List* list, I32 index, void* item) {
    if (index < 0 || index > list->count) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    if (list->count >= list->buffer.itemcap) {
        I32 newcap = list->buffer.itemcap * 2 ?: 1;
        ml_setcap(list, newcap);
    }
    memmove(list->buffer.data + ((index + 1) * list->buffer.itemsize),
            list->buffer.data + (index * list->buffer.itemsize),
            (list->count - index) * list->buffer.itemsize);
    memcpy(list->buffer.data + (index * list->buffer.itemsize), item, list->buffer.itemsize);
    list->count++;
    return 0; // Success
}

IErr ml_remove(m_List* list, I32 index) {
    if (index < 0 || index >= list->count) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    memmove(list->buffer.data + (index * list->buffer.itemsize),
            list->buffer.data + ((index + 1) * list->buffer.itemsize),
            (list->count - index - 1) * list->buffer.itemsize);
    list->count--;
    return 0; // Success
}

IErr ml_remove_range(m_List* list, I32 startindex, I32 count) {
    if (startindex < 0 || startindex >= list->count || count <= 0) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    I32 endindex = startindex + count;
    if (endindex > list->count) {
        endindex = list->count;
    }
    memmove(list->buffer.data + (startindex * list->buffer.itemsize),
            list->buffer.data + (endindex * list->buffer.itemsize),
            (list->count - endindex) * list->buffer.itemsize);
    list->count -= (endindex - startindex);
    return 0; // Success
}

IErr ml_remove_swap(m_List* list, I32 index) {
    if (index < 0 || index >= list->count) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    memcpy(list->buffer.data + (index * list->buffer.itemsize),
           list->buffer.data + ((list->count - 1) * list->buffer.itemsize),
           list->buffer.itemsize);
    list->count--;
    return 0; // Success
}

I32 ml_find(m_List* list, void* item) {
    if (!list || !item || !list->comparer) {
        return -1;
    }
    for (I32 i = 0; i < list->count; ++i) {
        void* list_item = list->buffer.data + (i * list->buffer.itemsize);
        if (list->comparer(list_item, item) == 0) {
            return i;
        }
    }
    return -1;
}

void ml_clear(m_List* list) {
    list->count = 0;
}

void ml_sort(m_List* list) {
    qsort(list->buffer.data, list->count, list->buffer.itemsize,
          (int (*)(const void*, const void*))list->comparer);
}

// Dictionary functions
m_Dict* md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer) {
    m_Dict* dict = (m_Dict*)m_alloc(sizeof(m_Dict));
    if (!dict) {
        return nil;
    }
    md_init(dict, keysize, valuesize, itemcap, comparer);
    return dict;
}

void md_destroy(m_Dict* dict) {
    if (!dict) {
        return;
    }
    md_clear(dict);
    ml_setcap(&dict->keys, 0);
    ml_setcap(&dict->values, 0);
    m_free(dict);
}

void md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer) {
    ml_init(&dict->keys, keysize, itemcap, comparer);
    ml_init(&dict->values, valuesize, itemcap, NULL); // Values don’t need a comparer
}

void md_setcap(m_Dict* dict, I32 newcap) {
    ml_setcap(&dict->keys, newcap);
    ml_setcap(&dict->values, newcap);
}

void* md_get(m_Dict* dict, void* key) {
    I32 index = ml_find(&dict->keys, key);
    return (index >= 0) ? ml_get(&dict->values, index) : NULL;
}

IErr md_put(m_Dict* dict, void* key, void* value) {
    I32 index = ml_find(&dict->keys, key);
    if (index >= 0) {
        return ml_put(&dict->values, index, value);
    }
    IErr err = ml_push(&dict->keys, key);
    if (err != 0) return err;
    err = ml_push(&dict->values, value);
    if (err != 0) {
        ml_remove(&dict->keys, ml_count(&dict->keys) - 1);
        return err;
    }
    return 0; // Success
}

Bool md_has(m_Dict* dict, void* key) {
    return ml_find(&dict->keys, key) >= 0;
}

IErr md_remove(m_Dict* dict, void* key) {
    I32 index = ml_find(&dict->keys, key);
    if (index >= 0) {
        IErr err = ml_remove_swap(&dict->keys, index);
        if (err != 0) return err;
        return ml_remove_swap(&dict->values, index);
    }
    return 0; // No key found, still success
}

IErr md_remove_ordered(m_Dict* dict, void* key) {
    I32 index = ml_find(&dict->keys, key);
    if (index >= 0) {
        IErr err = ml_remove(&dict->keys, index);
        if (err != 0) return err;
        return ml_remove(&dict->values, index);
    }
    return 0; // No key found, still success
}

void md_clear(m_Dict* dict) {
    ml_clear(&dict->keys);
    ml_clear(&dict->values);
}

I32 md_count(m_Dict* dict) {
    return dict->keys.count;
}

// String buffer functions
m_StrBuffer* ms_create(I32 itemcap) {
    m_StrBuffer* strbuffer = (m_StrBuffer*)m_alloc(sizeof(m_StrBuffer));
    if (!strbuffer) {
        return nil;
    }
    ms_init(strbuffer, itemcap);
    return strbuffer;
}

void ms_destroy(m_StrBuffer* strbuffer) {
    if (!strbuffer) {
        return;
    }
    ms_clear(strbuffer);
    mb_setcap(&strbuffer->buffer, 0);
    m_free(strbuffer);
}

void ms_init(m_StrBuffer* strbuffer, I32 itemcap) {
    mb_init(&strbuffer->buffer, sizeof(char), itemcap + 1); // +1 for null terminator
    strbuffer->length = 0;
    strbuffer->buffer.data[0] = '\0';
}

void ms_setcap(m_StrBuffer* strbuffer, I32 newcap) {
    mb_setcap(&strbuffer->buffer, newcap + 1); // +1 for null terminator
    if (strbuffer->length > newcap) {
        strbuffer->length = newcap;
    }
    strbuffer->buffer.data[strbuffer->length] = '\0';
}

void ms_clear(m_StrBuffer* strbuffer) {
    strbuffer->length = 0;
    strbuffer->buffer.data[0] = '\0';
}

char* ms_getstr(m_StrBuffer* strbuffer) {
    return (char*)strbuffer->buffer.data;
}

void ms_cat(m_StrBuffer* strbuffer, const char* format, ...) {
    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    I32 length = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    I32 newlength = strbuffer->length + length;
    if (newlength >= strbuffer->buffer.itemcap - 1) {
        ms_setcap(strbuffer, newlength + 1);
    }

    vsprintf((char*)strbuffer->buffer.data + strbuffer->length, format, args);
    strbuffer->length = newlength;
    strbuffer->buffer.data[strbuffer->length] = '\0';

    va_end(args);
}

void ms_trim(m_StrBuffer* strbuffer) {
    if (strbuffer->length == 0) return;

    U8* start = strbuffer->buffer.data;
    while (isspace(*start)) start++;

    U8* end = strbuffer->buffer.data + strbuffer->length - 1;
    while (end > start && isspace(*end)) end--;

    strbuffer->length = end - start + 1;
    memmove(strbuffer->buffer.data, start, strbuffer->length);
    strbuffer->buffer.data[strbuffer->length] = '\0';
}

IErr ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest) {
    if (start < 0 || start >= strbuffer->length || length <= 0) {
        return M_ERR_OUT_OF_BOUNDS;
    }

    I32 actual_length = (start + length > strbuffer->length) ? strbuffer->length - start : length;

    ms_init(dest, actual_length);
    strncpy((char*)dest->buffer.data, (char*)strbuffer->buffer.data + start, actual_length);
    dest->length = actual_length;
    dest->buffer.data[dest->length] = '\0';
    return 0; // Success
}

I32 ms_find(m_StrBuffer* strbuffer, const char* substring) {
    if (strbuffer->length == 0 || !substring || *substring == '\0') {
        return -1;
    }

    char* found = strstr((char*)strbuffer->buffer.data, substring);
    return found ? (found - (char*)strbuffer->buffer.data) : -1;
}

// Logging functions
static m_LogLevel current_log_level = M_LOG_INFO;

void m_set_loglevel(m_LogLevel level) {
    current_log_level = level;
}

void m_log_raw(m_LogLevel level, const char* format, ...) {
    if (level < current_log_level) {
        return;
    }

    va_list args;
    va_start(args, format);

    switch (level) {
        case M_LOG_TRACE: fprintf(stdout, "[TRACE] "); break;
        case M_LOG_INFO:  fprintf(stdout, "[INFO] ");  break;
        case M_LOG_WARN:  fprintf(stdout, "[WARN] ");  break;
        case M_LOG_ERROR: fprintf(stdout, "[ERROR] "); break;
        case M_LOG_FATAL: fprintf(stdout, "[FATAL] "); break;
    }

    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
}

#endif /* MOREWARD_IMPL */

/*

Copyright (c) 2024 moreward

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

*/

#endif /* _MOREWARD_H_ */
