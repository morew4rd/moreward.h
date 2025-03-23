#include "mg.h"

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