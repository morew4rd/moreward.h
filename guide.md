moreward.h
==========

What?
-----

Reusable single-file header library for C projects. Provides dynamic lists (arrays), maps and strings. Custom allocators can be used with the API.

Almost all functions return an additional integer error value that is set to 0 on success.

HOW?
----

From exactly one .c file set `MOREWARD_H_IMPL` before including the library:
```c
#define MOREWARD_H_IMPL
#include "moreward.h"
```

Some default values can be overridden with defines. See the header source for details.

Other includes should be just:
```c
#include "moreward.h"
```

For some sample usage, see test cases.

API
---

- l_create
    creates a new list
- l_destroy
    destroys a list
- l_init
    initializes a list
- l_setcap
    sets the capacity of a list
- l_push
    adds an item to the end of a list
- l_put
    puts an item at a specific index in a list
- l_pop
    removes and returns the last item from a list
 l_get
    gets the item at a specific index in a list
- l_swap
    swaps two items in a list
- l_clear
    clears all items from a list
- l_rm_swap
    removes an item from a list by swapping with the last item
- l_rm_move
    removes an item from a list by moving subsequent items to fill the gap
- l_rm_move_n
    removes n items from a list starting at a specific index by moving subsequent items to fill the gaps
- l_insert
    inserts an item at a specific index in a list
- l_insert_empty_n
    inserts n empty items at a specific index in a list
- l_find
    finds the index of an item in a list
- l_len
    gets the number of items in a list
- l_cap
    gets the capacity of a list
- m_create
    creates a new map
- m_destroy
    destroys a map
- m_init
    initializes a map
- m_setcap
    sets the capacity of a map
- m_put
    puts a key-value pair into a map
- m_get
    gets the value associated with a key in a map
- m_rm
    removes a key-value pair from a map
- m_clear
    clears all key-value pairs from a map
- m_len
    gets the number of key-value pairs in a map
- m_cap
    gets the capacity of a map
- s_create
    creates a new string
- s_destroy
    destroys a string
- s_setcap
    sets the capacity of a string
- s_len
    gets the number of characters in a string
- s_cap
    gets the capacity of a string
- s_clear
    clears the contents of a string
- s_get
    gets the contents of a string
- s_cat
    concatenates a const C string to the end of a string
- s_cat_cstr
    concatenates a C string to the end of a string
- s_cat_char
    concatenates a character to the end of a string
- s_cat_f32
    concatenates a float to the end of a string
- s_cat_f64
    concatenates a double to the end of a string
- s_cat_i8
    concatenates an 8-bit integer to the end of a string
- s_cat_i16
    concatenates a 16-bit integer to the end of a string
- s_cat_i32
    concatenates a 32-bit integer to the end of a string
- s_cat_i64
    concatenates a 64-bit integer to the end of a string
- s_cat_isize
    concatenates a size_t to the end of a string
- s_cat_u8
    concatenates an 8-bit unsigned integer to the end of a string
- s_cat_u16
    concatenates a 16-bit unsigned integer to the end of a string
- s_cat_u32
    concatenates a 32-bit unsigned integer to the end of a string
- s_cat_u64
    concatenates a 64-bit unsigned integer to the end of a string
- s_cat_usize
    concatenates a usize to the end of a string

