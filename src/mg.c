#include "mg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h> // For isspace

// Default malloc function
static Void* _default_malloc(Sz size, Void* userdata) {
    return malloc(size);
}

// Default realloc function
static Void* _default_realloc(Void* ptr, Sz new_size, Void* userdata) {
    return realloc(ptr, new_size);
}

// Default free function
static Void _default_free(Void* ptr, Void* userdata) {
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
Void m_set_allocator(m_Allocator *allocator) {
    _current_allocator = allocator;
}

Void m_reset_allocator(Void) {
    _current_allocator = &_default_allocator;
}

m_Allocator *m_get_allocator() {
    return _current_allocator;
}

// Helper function to free buffer data
static Void _buffer_free_data(m_Buffer* buffer) {
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
        return null;
    }
    IErr err = mb_init(buffer, itemsize, itemcap);
    if (err != 0) {
        m_free(buffer);
        return null;
    }
    return buffer;
}

Void mb_destroy(m_Buffer* buffer) {
    if (!buffer) {
        return;
    }
    mb_setcap(buffer, 0);
    m_free(buffer);
}

IErr mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap) {
    if (!buffer) {
        return M_ERR_NULL_POINTER;
    }
    buffer->allocator = m_get_allocator();
    buffer->data = NULL;
    buffer->itemsize = itemsize;
    buffer->itemcap = 0;
    return mb_setcap(buffer, itemcap);
}

IErr mb_setcap(m_Buffer* buffer, I32 newcap) {
    if (!buffer) {
        return M_ERR_NULL_POINTER;
    }

    if (newcap == 0) {
        _buffer_free_data(buffer);
        return 0;  // Success
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
            return 0;  // Success
        } else {
            m_log_error("mb_setcap: realloc failed");
            return M_ERR_ALLOCATION_FAILED;
        }
    } else {
        buffer->data = (U8*)buffer->allocator->malloc(new_size, buffer->allocator->userdata);
        if (buffer->data) {
            buffer->itemcap = newcap;
            memset(buffer->data, 0, new_size);
            return 0;  // Success
        } else {
            m_log_error("mb_setcap: malloc failed");
            return M_ERR_ALLOCATION_FAILED;
        }
    }
}

// List functions
m_List* ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer) {
    m_List* list = (m_List*)m_alloc(sizeof(m_List));
    if (!list) {
        return null;
    }
    IErr err = ml_init(list, itemsize, itemcap, comparer);
    if (err != 0) {
        m_free(list);
        return null;
    }
    return list;
}

Void ml_destroy(m_List* list) {
    if (!list) {
        return;
    }
    ml_clear(list);
    ml_setcap(list, 0);
    m_free(list);
}

static I32 _default_comparer(Void* item1, Void* item2) {
    if (item1 == item2) return 0;
    return (item1 < item2) ? -1 : 1;
}

IErr ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer) {
    if (!list) {
        return M_ERR_NULL_POINTER;
    }
    IErr err = mb_init(&list->buffer, itemsize, itemcap);
    if (err != 0) {
        return err;
    }
    list->count = 0;
    list->comparer = comparer ? comparer : _default_comparer;
    return 0;  // Success
}

IErr ml_setcap(m_List* list, I32 newcap) {
    if (!list) {
        return M_ERR_NULL_POINTER;
    }
    return mb_setcap(&list->buffer, newcap);
}

IErr ml_push(m_List* list, Void* item) {
    if (list->count >= list->buffer.itemcap) {
        I32 newcap = list->buffer.itemcap * 2 ?: 1; // Double or start at 1
        IErr err = ml_setcap(list, newcap);
        if (err != 0) {
            return err;
        }
    }
    memcpy(list->buffer.data + (list->count * list->buffer.itemsize), item, list->buffer.itemsize);
    list->count++;
    return 0; // Success
}

Void* ml_pop(m_List* list) {
    if (list->count == 0) {
        return NULL;
    }
    list->count--;
    return list->buffer.data + (list->count * list->buffer.itemsize);
}

I32 ml_count(m_List* list) {
    return list->count;
}

Void* ml_get(m_List* list, I32 index) {
    if (index < 0 || index >= list->count) {
        return NULL;
    }
    return list->buffer.data + (index * list->buffer.itemsize);
}

IErr ml_put(m_List* list, I32 index, Void* item) {
    if (index < 0 || index >= list->count) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    memcpy(list->buffer.data + (index * list->buffer.itemsize), item, list->buffer.itemsize);
    return 0; // Success
}

IErr ml_insert(m_List* list, I32 index, Void* item) {
    if (index < 0 || index > list->count) {
        return M_ERR_OUT_OF_BOUNDS;
    }
    if (list->count >= list->buffer.itemcap) {
        I32 newcap = list->buffer.itemcap * 2 ?: 1;
        IErr err = ml_setcap(list, newcap);
        if (err != 0) {
            return err;
        }
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

I32 ml_find(m_List* list, Void* item) {
    if (!list || !item || !list->comparer) {
        return -1;
    }
    for (I32 i = 0; i < list->count; ++i) {
        Void* list_item = list->buffer.data + (i * list->buffer.itemsize);
        if (list->comparer(list_item, item) == 0) {
            return i;
        }
    }
    return -1;
}

Void ml_clear(m_List* list) {
    list->count = 0;
}

Void ml_sort(m_List* list) {
    qsort(list->buffer.data, list->count, list->buffer.itemsize,
          (int (*)(const Void*, const Void*))list->comparer);
}

// Dictionary functions
m_Dict* md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer) {
    m_Dict* dict = (m_Dict*)m_alloc(sizeof(m_Dict));
    if (!dict) {
        return null;
    }
    IErr err = md_init(dict, keysize, valuesize, itemcap, comparer);
    if (err != 0) {
        m_free(dict);
        return null;
    }
    return dict;
}

Void md_destroy(m_Dict* dict) {
    if (!dict) {
        return;
    }
    md_clear(dict);
    ml_setcap(&dict->keys, 0);
    ml_setcap(&dict->values, 0);
    m_free(dict);
}

IErr md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer) {
    if (!dict) {
        return M_ERR_NULL_POINTER;
    }
    IErr err = ml_init(&dict->keys, keysize, itemcap, comparer);
    if (err != 0) {
        return err;
    }
    err = ml_init(&dict->values, valuesize, itemcap, NULL);
    if (err != 0) {
        ml_destroy(&dict->keys);
        return err;
    }
    return 0;  // Success
}

IErr md_setcap(m_Dict* dict, I32 newcap) {
    if (!dict) {
        return M_ERR_NULL_POINTER;
    }
    IErr err = ml_setcap(&dict->keys, newcap);
    if (err != 0) {
        return err;
    }
    err = ml_setcap(&dict->values, newcap);
    if (err != 0) {
        return err;
    }
    return 0;  // Success
}

Void* md_get(m_Dict* dict, Void* key) {
    I32 index = ml_find(&dict->keys, key);
    return (index >= 0) ? ml_get(&dict->values, index) : NULL;
}

IErr md_put(m_Dict* dict, Void* key, Void* value) {
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

Bool md_has(m_Dict* dict, Void* key) {
    return ml_find(&dict->keys, key) >= 0;
}

IErr md_remove(m_Dict* dict, Void* key) {
    I32 index = ml_find(&dict->keys, key);
    if (index >= 0) {
        IErr err = ml_remove_swap(&dict->keys, index);
        if (err != 0) return err;
        return ml_remove_swap(&dict->values, index);
    }
    return 0; // No key found, still success
}

IErr md_remove_ordered(m_Dict* dict, Void* key) {
    I32 index = ml_find(&dict->keys, key);
    if (index >= 0) {
        IErr err = ml_remove(&dict->keys, index);
        if (err != 0) return err;
        return ml_remove(&dict->values, index);
    }
    return 0; // No key found, still success
}

Void md_clear(m_Dict* dict) {
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
        return null;
    }
    IErr err = ms_init(strbuffer, itemcap);
    if (err != 0) {
        m_free(strbuffer);
        return null;
    }
    return strbuffer;
}

Void ms_destroy(m_StrBuffer* strbuffer) {
    if (!strbuffer) {
        return;
    }
    ms_clear(strbuffer);
    mb_setcap(&strbuffer->buffer, 0);
    m_free(strbuffer);
}

IErr ms_init(m_StrBuffer* strbuffer, I32 itemcap) {
    if (!strbuffer) {
        return M_ERR_NULL_POINTER;
    }
    IErr err = mb_init(&strbuffer->buffer, sizeof(char), itemcap + 1);
    if (err != 0) {
        return err;
    }
    strbuffer->length = 0;
    strbuffer->buffer.data[0] = '\0';
    return 0;  // Success
}

IErr ms_setcap(m_StrBuffer* strbuffer, I32 newcap) {
    if (!strbuffer) {
        return M_ERR_NULL_POINTER;
    }
    IErr err = mb_setcap(&strbuffer->buffer, newcap + 1);
    if (err != 0) {
        return err;
    }
    if (strbuffer->length > newcap) {
        strbuffer->length = newcap;
    }
    strbuffer->buffer.data[strbuffer->length] = '\0';
    return 0;  // Success
}

Void ms_clear(m_StrBuffer* strbuffer) {
    strbuffer->length = 0;
    strbuffer->buffer.data[0] = '\0';
}

char* ms_getstr(m_StrBuffer* strbuffer) {
    return (char*)strbuffer->buffer.data;
}

IErr ms_cat(m_StrBuffer* strbuffer, const char* format, ...) {
    if (!strbuffer) {
        return M_ERR_NULL_POINTER;
    }

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    I32 length = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    I32 newlength = strbuffer->length + length;
    if (newlength >= strbuffer->buffer.itemcap - 1) {
        IErr err = ms_setcap(strbuffer, newlength + 1);
        if (err != 0) {
            va_end(args);
            return err;
        }
    }

    vsprintf((char*)strbuffer->buffer.data + strbuffer->length, format, args);
    strbuffer->length = newlength;
    strbuffer->buffer.data[strbuffer->length] = '\0';

    va_end(args);
    return 0;  // Success
}

Void ms_trim(m_StrBuffer* strbuffer) {
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

    IErr err = ms_init(dest, actual_length);
    if (err != 0) {
        return err;
    }
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

Void m_set_loglevel(m_LogLevel level) {
    current_log_level = level;
}

Void m_log_raw(m_LogLevel level, const char* format, ...) {
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