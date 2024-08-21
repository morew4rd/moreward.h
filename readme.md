## Overview

`moreward.h` is a header file designed to provide a set of common data structures and utility functions for C programming. It includes definitions for various types, memory allocation macros, and structures for lists, dictionaries, and string buffers. The file also provides logging capabilities and custom allocator support.

## Build

From only one `.c` file set `MOREWARD_IMPL` before including the library:

```c
#define MOREWARD_IMPL
#include "moreward.h"
```

From other `.c` files, you can include it normally:

```c
#include "moreward.h"
```

## Types

- **Primitive Types**:
  - `I8`, `I16`, `I32`, `I64`: Signed integers of 8, 16, 32, and 64 bits respectively.
  - `U8`, `U16`, `U32`, `U64`: Unsigned integers of 8, 16, 32, and 64 bits respectively.
  - `F32`, `F64`: Floating-point numbers of 32 and 64 bits respectively.
  - `Bool`: Boolean type.
  - `Str`, `CStr`: Strings and constant strings.
  - `USize`, `ISize`: Size types for unsigned and signed sizes.
  - `IErr`: Integer type for error codes.

- **Structures**:
  - `m_Allocator`: Defines a custom memory allocator with `malloc`, `realloc`, and `free` functions, along with a `userdata` pointer for context.
  - `m_Buffer`: Represents a generic buffer with a pointer to data, item size, capacity, and an allocator.
  - `m_List`: A dynamic array list with a buffer, count of items, and an item comparison function.
  - `m_Dict`: A dictionary implemented as two lists: one for keys and one for values.
  - `m_StrBuffer`: A string buffer built on top of `m_Buffer` with a length field.

- **Enumerations**:
  - `m_LogLevel`: Defines log levels for tracing, information, warnings, errors, and fatal messages.

## Functions

### Logging

- `m_set_loglevel(m_LogLevel level)`: Sets the logging level.
- `m_log_raw(m_LogLevel level, const char* format, ...)`: Logs a message with a specified level.
- Macros like `m_log`, `m_log_trace`, `m_log_info`, `m_log_warn`, `m_log_error`, and `m_log_fatal` provide convenient logging with file and line information.

### Memory Management

- `m_set_allocator(m_Allocator *allocator)`: Sets a custom allocator.
- `m_reset_allocator()`: Resets the allocator to the default.
- `m_get_allocator()`: Retrieves the current allocator.

#### Custom Allocator

The `m_Allocator` structure allows you to define custom memory allocation functions. This can be useful for tracking memory usage, using a custom memory pool, or integrating with a specific memory management system. To use a custom allocator, define an `m_Allocator` structure with your custom `malloc`, `realloc`, and `free` functions, and then call `m_set_allocator` with a pointer to this structure.

Example:

```c
m_Allocator custom_allocator = {
    .malloc = my_malloc,
    .realloc = my_realloc,
    .free = my_free,
    .userdata = NULL
};

m_set_allocator(&custom_allocator);
```

### Error Handling

The `IErr` type is used to return error codes from functions. Functions that can potentially fail will take a pointer to an `IErr` variable as an argument. If an error occurs, the function will set the value of this variable to a non-zero error code. You should check the value of this variable after calling such functions to determine if an error occurred.

Example:

```c
IErr err = 0;
mb_init(&buffer, itemsize, itemcap, allocator, &err);
if (err != 0) {
    // Handle error
}
```

### Buffer Management

- `mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap, m_Allocator* allocator, IErr* errptr)`: Initializes a buffer.
- `mb_setcap(m_Buffer* buffer, I32 newcap, IErr* errptr)`: Sets the capacity of a buffer.

### List Management

- `ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr)`: Initializes a list.
- `ml_setcap(m_List* list, I32 newcap, IErr* errptr)`: Sets the capacity of a list.
- `ml_push(m_List* list, void* item, IErr* errptr)`: Adds an item to the list.
- `void* ml_pop(m_List* list, IErr* errptr)`: Removes and returns the last item from the list.
- `I32 ml_count(m_List* list, IErr* errptr)`: Returns the number of items in the list.
- `void* ml_get(m_List* list, I32 index, IErr* errptr)`: Retrieves an item at a specified index.
- `ml_insert(m_List* list, I32 index, void* item, IErr* errptr)`: Inserts an item at a specified index.
- `ml_remove(m_List* list, I32 index, IErr* errptr)`: Removes an item at a specified index.
- `ml_remove_range(m_List* list, I32 startindex, I32 count, IErr* errptr)`: Removes a range of items.
- `ml_remove_swap(m_List* list, I32 index, IErr* errptr)`: Removes an item by swapping it with the last item.
- `I32 ml_find(m_List* list, void* item, IErr* errptr)`: Finds the index of an item.
- `ml_clear(m_List* list, IErr* errptr)`: Clears the list.
- `ml_sort(m_List* list, IErr* errptr)`: Sorts the list.

### Dictionary Management

- `md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr)`: Initializes a dictionary.
- `md_setcap(m_Dict* dict, I32 newcap, IErr* errptr)`: Sets the capacity of a dictionary.
- `void* md_get(m_Dict* dict, void* key, IErr* errptr)`: Retrieves a value by key.
- `md_put(m_Dict* dict, void* key, void* value, IErr* errptr)`: Adds or updates a key-value pair.
- `Bool md_has(m_Dict* dict, void* key, IErr* errptr)`: Checks if a key exists.
- `md_remove(m_Dict* dict, void* key, IErr* errptr)`: Removes a key-value pair.
- `md_remove_ordered(m_Dict* dict, void* key, IErr* errptr)`: Removes a key-value pair while maintaining order.
- `md_clear(m_Dict* dict, IErr* errptr)`: Clears the dictionary.

### String Buffer Management

- `ms_init(m_StrBuffer* strbuffer, I32 itemcap, m_Allocator* allocator, IErr* errptr)`: Initializes a string buffer.
- `ms_setcap(m_StrBuffer* strbuffer, I32 newcap, IErr* errptr)`: Sets the capacity of a string buffer.
- `ms_clear(m_StrBuffer* strbuffer, IErr* errptr)`: Clears the string buffer.
- `char* ms_getstr(m_StrBuffer* strbuffer, IErr* errptr)`: Retrieves the string from the buffer.
- `ms_cat(m_StrBuffer* strbuffer, const char* format, ...)`: Appends formatted text to the string buffer.
- `ms_trim(m_StrBuffer* strbuffer, IErr* errptr)`: Trims the string buffer.
- `ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest, IErr* errptr)`: Extracts a substring.
- `I32 ms_find(m_StrBuffer* strbuffer, const char* substring, IErr* errptr)`: Finds the index of a substring.

### Logging System

The logging system in `moreward.h` allows you to log messages at different levels of severity. The available log levels are:

- `M_LOG_TRACE`: Detailed trace messages, typically used for debugging.
- `M_LOG_INFO`: Informational messages that highlight the progress of the application.
- `M_LOG_WARN`: Warning messages that indicate potential issues.
- `M_LOG_ERROR`: Error messages that indicate issues that need attention.
- `M_LOG_FATAL`: Fatal error messages that indicate critical issues requiring immediate attention.

To set the logging level, use the `m_set_loglevel` function:

```c
m_set_loglevel(M_LOG_INFO);
```

To log messages, use the `m_log_raw` function or the convenience macros:

```c
m_log_info("This is an informational message.");
m_log_error("This is an error message.");
```

### Macros

- `m_alloc(n)`: Allocates memory using the current allocator.
- `m_realloc(p, n)`: Reallocates memory using the current allocator.
- `m_free(p)`: Frees memory using the current allocator.
- `m_log(format, ...)`: Logs an informational message.
- `m_tracelog(level, file, line, format, ...)`: Logs a message with file and line information.
- `m_log_trace(format, ...)`: Logs a trace message.
- `m_log_info(format, ...)`: Logs an informational message.
- `m_log_warn(format, ...)`: Logs a warning message.
- `m_log_error(format, ...)`: Logs an error message.
- `m_log_fatal(format, ...)`: Logs a fatal error message.
- `m_try(expr, errptr)`: Executes an expression and checks for errors.
- `m_try_ret(expr, errptr, retval)`: Executes an expression, checks for errors, and returns a value if an error occurs.

This guide provides a brief overview of the types, functions, and macros available in `moreward.h`. For detailed usage and implementation, refer to the source code and comments within the file.