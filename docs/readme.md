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
- Sizes: `Sz` (unsigned Sz), `ISz` (signed ptrdiff_t)
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
Void* my_malloc(Sz size, Void* userdata) {  }
Void* my_realloc(Void* ptr, Sz newsize, Void* userdata) {  }
my_free(Void* ptr, Void* userdata) {  }

m_Allocator custom = {
    .malloc = my_malloc,
    .realloc = my_realloc,
    .free = my_free,
    .userdata = NULL  // Optional context
};

m_set_allocator(&custom);
```

## Error Handling

Functions that may fail return an code directly (0 indicates success, non-zero indicates an error). Check the return value to handle errors:

```c
err = ml_push(&list, &item);
if (err != 0) {
    m_log_error("Failed to push item: error code %d", err);
}
```

## Data Structures

### Buffer (m_Buffer)
A low-level dynamic buffer for raw data.

-` m_Buffer mb_create(I32 itemsize, I32 itemcap)`: Creates a buffer with specified item size and capacity.
- `mb_destroy(m_Buffer* buffer)`: Frees the buffer’s memory.
- `mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap)`: Initializes an existing buffer.
- `mb_setcap(m_Buffer* buffer, I32 newcap)`: Resizes the buffer’s capacity.

### List (m_List)
A dynamic array with flexible operations.

- `m_List ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer)`: Creates a list with an optional comparison function for sorting/finding.
- `ml_destroy(m_List* list)`: Frees the list.
- `ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer)`: Initializes an existing list.
- `ml_push(m_List* list, Void* item)`: Appends an item.
- `Void* ml_pop(m_List* list)`: Removes and returns the last item.
- `I32 ml_count(m_List* list)`: Returns the number of items.
- `Void* ml_get(m_List* list, I32 index)`: Retrieves an item by index.
- `ml_insert(m_List* list, I32 index, Void* item)`: Inserts an item at an index.
- `ml_remove(m_List* list, I32 index)`: Removes an item at an index.
- `ml_remove_range(m_List* list, I32 startindex, I32 count)`: Removes a range of items.
- `ml_remove_swap(m_List* list, I32 index)`: Removes an item by swapping with the last (faster, unordered).
- `I32 ml_find(m_List* list, Void* item)`: Returns the index of an item (or -1 if not found).
- `ml_clear(m_List* list)`: Removes all items.
- `ml_sort(m_List* list)`: Sorts the list using the provided comparer.

### Dictionary (m_Dict)
A key-value store implemented with two lists.

- `m_Dict md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer)`: Creates a dictionary.
- `md_destroy(m_Dict* dict)`: Frees the dictionary.
- `md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer)`: Initializes an existing dictionary.
- `Void* md_get(m_Dict* dict, Void* key)`: Retrieves a value by key (NULL if not found).
- `md_put(m_Dict* dict, Void* key, Void* value)`: Adds or updates a key-value pair.
- `Bool md_has(m_Dict* dict, Void* key)`: Checks if a key exists.
- `md_remove(m_Dict* dict, Void* key)`: Removes a key-value pair (unordered).
- `md_remove_ordered(m_Dict* dict, Void* key)`: Removes a key-value pair while preserving order.
- `md_clear(m_Dict* dict)`: Removes all entries.

### String Buffer (m_StrBuffer)
A dynamic string buffer for efficient manipulation.

- `m_StrBuffer ms_create(I32 itemcap)`: Creates a string buffer with initial capacity.
- `ms_destroy(m_StrBuffer* strbuffer)`: Frees the string buffer.
- `ms_init(m_StrBuffer* strbuffer, I32 itemcap)`: Initializes an existing string buffer.
- `ms_setcap(m_StrBuffer* strbuffer, I32 newcap)`: Resizes the capacity.
- `ms_clear(m_StrBuffer* strbuffer)`: Clears the contents.
- `char* ms_getstr(m_StrBuffer* strbuffer)`: Returns the current string.
- `ms_cat(m_StrBuffer* strbuffer, const char* format, ...)`: Appends a formatted string.
- `ms_trim(m_StrBuffer* strbuffer)`: Trims leading/trailing whitespace.
- `ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest)`: Extracts a substring into another buffer.
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
- `m_set_loglevel(m_LogLevel level)`: Sets the minimum log level to display.
- `m_log_raw(m_LogLevel level, const char* format, ...)`: Logs a raw message.
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

