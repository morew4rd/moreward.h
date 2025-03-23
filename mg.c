
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h> // For isspace

#include "mg.h"

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

void m_set_allocator(m_Allocator *allocator) {
    _current_allocator = allocator;
}

void m_reset_allocator(void) {
    _current_allocator = &_default_allocator;
}

m_Allocator *m_get_allocator() {
    return _current_allocator;
}


static void _buffer_free_data(m_Buffer* buffer) {
    if (buffer->data) {
        buffer->allocator->free(buffer->data, buffer->allocator->userdata);
        buffer->data = NULL;
        buffer->itemcap = 0;
    }
}

m_Buffer* mb_create(I32 itemsize, I32 itemcap, m_Allocator* allocator, IErr* errptr) {
    m_Buffer* buffer = (m_Buffer*)m_alloc(sizeof(m_Buffer));
    if (!buffer) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return nil;
    }
    mb_init(buffer, itemsize, itemcap, allocator, errptr);
    if (errptr && *errptr != 0) {
        m_free(buffer);
        return nil;
    }
    return buffer;
}

void mb_destroy(m_Buffer* buffer, IErr* errptr) {
    if (!buffer) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return;
    }
    mb_setcap(buffer, 0, errptr);
    m_free(buffer);
}


void mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap, m_Allocator* allocator, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (allocator) {
        buffer->allocator = allocator;
    } else {
        buffer->allocator = NULL;
    }
    buffer->data = NULL;
    buffer->itemsize = itemsize;
    buffer->itemcap = 0;
    mb_setcap(buffer, itemcap, errptr);
}

void mb_setcap(m_Buffer* buffer, I32 newcap, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (!buffer->allocator) {
        buffer->allocator = m_get_allocator();
    }

    if (newcap == 0) {
        _buffer_free_data(buffer);
        return;
    }

    if (buffer->itemsize == 0) {
        buffer->itemsize = 1;  // 1 byte is assumed. mainly for string buffers without init
    }

    I32 new_size = buffer->itemsize * newcap;
    I32 old_size = buffer->itemsize * buffer->itemcap;

    if (buffer->data) {
        U8* new_data = (U8*)buffer->allocator->realloc(buffer->data, new_size, buffer->allocator->userdata);
        if (new_data) {
            buffer->data = new_data;
            buffer->itemcap = newcap;

            // Zero out the new area if the buffer is expanded
            if (new_size > old_size) {
                memset(buffer->data + old_size, 0, new_size - old_size);
            }
        } else {
            if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: realloc failed
        }
    } else {
        buffer->data = (U8*)buffer->allocator->malloc(new_size, buffer->allocator->userdata);
        if (buffer->data) {
            buffer->itemcap = newcap;

            // Zero out the entire newly allocated buffer
            memset(buffer->data, 0, new_size);
        } else {
            if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: malloc failed
        }
    }
}
m_List* ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr) {
    m_List* list = (m_List*)m_alloc(sizeof(m_List));
    if (!list) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return nil;
    }
    ml_init(list, itemsize, itemcap, comparer, allocator, errptr);
    if (errptr && *errptr != 0) {
        m_free(list);
        return nil;
    }
    return list;
}

void ml_destroy(m_List* list, IErr* errptr) {
    if (!list) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return;
    }
    ml_clear(list, errptr);
    mb_setcap(&list->buffer, 0, errptr);
    m_free(list);
}

static I32 _default_comparer(void* item1, void* item2) {
    if (item1 == item2) {
        return 0;
    } else if (item1 < item2) {
        return -1;
    } else {
        return 1;
    }
}

// TODO: this won't work
static I32 _int_comparer(void* item1, void* item2) {
    if (*(int*)item1 == *(int*)item2) {
        return 0;
    } else if (*(int*)item1 < *(int*)item2) {
        return -1;
    } else {
        return 1;
    }
}

void ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    mb_init(&list->buffer, itemsize, itemcap, allocator, errptr);
    list->count = 0;
    list->comparer = comparer ? comparer : _int_comparer; // TODO: not int! Use default comparer if none is provided
}

void ml_setcap(m_List* list, I32 newcap, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    mb_setcap(&list->buffer, newcap, errptr);
}

void ml_push(m_List* list, void* item, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (list->count >= list->buffer.itemcap) {
        // Double the capacity if the list is full
        ml_setcap(list, list->buffer.itemcap * 2, errptr);
        if (errptr && *errptr != 0) return; // Return if error occurred
    }
    memcpy(list->buffer.data + (list->count * list->buffer.itemsize), item, list->buffer.itemsize);
    list->count++;
}

void* ml_pop(m_List* list, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (list->count == 0) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: list is empty
        return NULL;
    }
    list->count--;
    return list->buffer.data + (list->count * list->buffer.itemsize);
}

I32 ml_count(m_List* list, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    return list->count;
}

void* ml_get(m_List* list, I32 index, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)
    if (index >= list->count) {

        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: index out of bounds
        return NULL;
    }
    return list->buffer.data + (index * list->buffer.itemsize);
}

void ml_put(m_List* list, I32 index, void* item, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (index >= list->count) {
        if (errptr) *errptr = M_ERR_OUT_OF_BOUNDS; // Error: index out of bounds
        return;
    }

    if (list->count >= list->buffer.itemcap) {
        // Double the capacity if the list is full
        ml_setcap(list, list->buffer.itemcap * 2, errptr);
        if (errptr && *errptr != 0) return; // Return if error occurred
    }

    // Copy the new item to the specified index
    memcpy(list->buffer.data + (index * list->buffer.itemsize), item, list->buffer.itemsize);
}

void ml_insert(m_List* list, I32 index, void* item, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (index > list->count) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: index out of bounds
        return;
    }
    if (list->count >= list->buffer.itemcap) {
        // Double the capacity if the list is full
        ml_setcap(list, list->buffer.itemcap * 2, errptr);
        if (errptr && *errptr != 0) return; // Return if error occurred
    }
    // Move elements to the right to make space for the new item
    memmove(list->buffer.data + ((index + 1) * list->buffer.itemsize), list->buffer.data + (index * list->buffer.itemsize), (list->count - index) * list->buffer.itemsize);
    memcpy(list->buffer.data + (index * list->buffer.itemsize), item, list->buffer.itemsize);
    list->count++;
}

void ml_remove(m_List* list, I32 index, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (index >= list->count) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: index out of bounds
        return;
    }
    // Move elements to the left to overwrite the removed item
    memmove(list->buffer.data + (index * list->buffer.itemsize), list->buffer.data + ((index + 1) * list->buffer.itemsize), (list->count - index - 1) * list->buffer.itemsize);
    list->count--;
}

void ml_remove_range(m_List* list, I32 startindex, I32 count, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (startindex >= list->count || count == 0) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: invalid range
        return;
    }
    I32 endindex = startindex + count;
    if (endindex > list->count) {
        endindex = list->count;
    }
    // Move elements to the left to overwrite the removed range
    memmove(list->buffer.data + (startindex * list->buffer.itemsize), list->buffer.data + (endindex * list->buffer.itemsize), (list->count - endindex) * list->buffer.itemsize);
    list->count -= (endindex - startindex);
}

void ml_remove_swap(m_List* list, I32 index, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (index >= list->count) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: index out of bounds
        return;
    }
    // Swap the item at index with the last item
    memcpy(list->buffer.data + (index * list->buffer.itemsize), list->buffer.data + ((list->count - 1) * list->buffer.itemsize), list->buffer.itemsize);
    list->count--;
}

I32 ml_find(m_List* list, void* item, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (list == NULL || item == NULL || list->comparer == NULL) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Error: invalid parameters
        return -1; // Return -1 if any parameter is NULL
    }

    for (I32 i = 0; i < list->count; ++i) {
        void* list_item = list->buffer.data + (i * list->buffer.itemsize);
        if (list->comparer(list_item, item) == 0) {
            return i; // Return the index of the matching item
        }
    }

    return -1; // Return -1 if no item matches
}

void ml_clear(m_List* list, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    list->count = 0;
}

void ml_sort(m_List* list, IErr *errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    qsort(list->buffer.data, list->count, list->buffer.itemsize, (int (*)(const void*, const void*))list->comparer);
}

m_Dict* md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr) {
    m_Dict* dict = (m_Dict*)m_alloc(sizeof(m_Dict));
    if (!dict) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return nil;
    }
    md_init(dict, keysize, valuesize, itemcap, comparer, allocator, errptr);
    if (errptr && *errptr != 0) {
        m_free(dict);
        return nil;
    }
    return dict;
}

void md_destroy(m_Dict* dict, IErr* errptr) {
    if (!dict) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return;
    }
    md_clear(dict, errptr);
    ml_setcap(&dict->keys, 0, errptr);
    ml_setcap(&dict->values, 0, errptr);
    m_free(dict);
}

void md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    ml_init(&dict->keys, keysize, itemcap, comparer, allocator, errptr);
    ml_init(&dict->values, valuesize, itemcap, comparer, allocator, errptr);
}

void md_setcap(m_Dict* dict, I32 newcap, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    ml_setcap(&dict->keys, newcap, errptr);
    ml_setcap(&dict->values, newcap, errptr);
}

void* md_get(m_Dict* dict, void* key, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    I32 index = ml_find(&dict->keys, key, errptr);
    if (index >= 0) {
        return ml_get(&dict->values, index, errptr);
    }
    return NULL;
}

void md_put(m_Dict* dict, void* key, void* value, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    I32 index = ml_find(&dict->keys, key, errptr);
    if (index >= 0) {
        memcpy(ml_get(&dict->values, index, errptr), value, dict->values.buffer.itemsize);
    } else {
        ml_push(&dict->keys, key, errptr);
        ml_push(&dict->values, value, errptr);
    }
}

Bool md_has(m_Dict* dict, void* key, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    return ml_find(&dict->keys, key, errptr) >= 0;
}

void md_remove(m_Dict* dict, void* key, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    I32 index = ml_find(&dict->keys, key, errptr);
    if (index >= 0) {
        ml_remove_swap(&dict->keys, index, errptr);
        ml_remove_swap(&dict->values, index, errptr);
    }
}

void md_remove_ordered(m_Dict* dict, void* key, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    I32 index = ml_find(&dict->keys, key, errptr);
    if (index >= 0) {
        ml_remove(&dict->keys, index, errptr);
        ml_remove(&dict->values, index, errptr);
    }
}

void md_clear(m_Dict* dict, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    ml_clear(&dict->keys, errptr);
    ml_clear(&dict->values, errptr);
}

I32 md_count(m_Dict* dict, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    return dict->keys.count;
}

m_StrBuffer* ms_create(I32 itemcap, m_Allocator* allocator, IErr* errptr) {
    m_StrBuffer* strbuffer = (m_StrBuffer*)m_alloc(sizeof(m_StrBuffer));
    if (!strbuffer) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return nil;
    }
    ms_init(strbuffer, itemcap, allocator, errptr);
    if (errptr && *errptr != 0) {
        m_free(strbuffer);
        return nil;
    }
    return strbuffer;
}

void ms_destroy(m_StrBuffer* strbuffer, IErr* errptr) {
    if (!strbuffer) {
        if (errptr) *errptr = M_ERR_NULL_POINTER;
        return;
    }
    ms_clear(strbuffer, errptr);
    mb_setcap(&strbuffer->buffer, 0, errptr);
    m_free(strbuffer);
}

void ms_init(m_StrBuffer* strbuffer, I32 itemcap, m_Allocator* allocator, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    mb_init(&strbuffer->buffer, sizeof(char), itemcap + 1, allocator, errptr); // +1 for the null terminator
    if (errptr && *errptr != 0) return; // Return if error occurred

    strbuffer->length = 0;
    strbuffer->buffer.data[0] = '\0'; // Ensure the buffer is null-terminated
}

void ms_setcap(m_StrBuffer* strbuffer, I32 newcap, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    mb_setcap(&strbuffer->buffer, newcap + 1, errptr); // +1 for the null terminator
    if (errptr && *errptr != 0) return; // Return if error occurred

    if (strbuffer->length > newcap) {
        strbuffer->length = newcap;
    }
    strbuffer->buffer.data[strbuffer->length] = '\0'; // Ensure the buffer is null-terminated
}

void ms_clear(m_StrBuffer* strbuffer, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    strbuffer->length = 0;
    strbuffer->buffer.data[0] = '\0'; // Ensure the buffer is null-terminated
}

char* ms_getstr(m_StrBuffer* strbuffer, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    return (char*)strbuffer->buffer.data;
}

void ms_cat(m_StrBuffer* strbuffer, const char* format, ...) {
    IErr e = 0;
    va_list args;
    va_start(args, format);

    // Calculate the length of the new string
    va_list args_copy;
    va_copy(args_copy, args);
    I32 length = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    // Ensure there's enough space in the buffer
    I32 newlength = strbuffer->length + length;
    if (newlength >= strbuffer->buffer.itemcap - 1) {
        ms_setcap(strbuffer, newlength + 1, &e);
    }

    // Append the new string
    vsprintf((char*)strbuffer->buffer.data + strbuffer->length, format, args);
    strbuffer->length = newlength;
    strbuffer->buffer.data[strbuffer->length] = '\0'; // Ensure the buffer is null-terminated

    va_end(args);
}


void ms_trim(m_StrBuffer* strbuffer, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (strbuffer->length == 0) return; // Nothing to trim

    // Trim leading whitespace
    U8* start = strbuffer->buffer.data;
    while (isspace(*start)) start++;

    // Trim trailing whitespace
    U8* end = strbuffer->buffer.data + strbuffer->length - 1;
    while (end > start && isspace(*end)) end--;

    // Calculate new length
    strbuffer->length = end - start + 1;
    memmove(strbuffer->buffer.data, start, strbuffer->length);
    strbuffer->buffer.data[strbuffer->length] = '\0'; // Ensure null-termination
}

void ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (start < 0 || start >= strbuffer->length || length <= 0) {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Invalid start or length
        return;
    }

    I32 actual_length = length;
    if (start + length > strbuffer->length) {
        actual_length = strbuffer->length - start;
    }

    ms_init(dest, actual_length, strbuffer->buffer.allocator, errptr);
    if (errptr && *errptr != 0) return; // Return if error occurred

    strncpy((char*)dest->buffer.data, (char*)strbuffer->buffer.data + start, actual_length);
    dest->length = actual_length;
    dest->buffer.data[dest->length] = '\0'; // Ensure null-termination
}

I32 ms_find(m_StrBuffer* strbuffer, const char* substring, IErr* errptr) {
    if (errptr) *errptr = 0; // Initialize error code to 0 (no error)

    if (strbuffer->length == 0 || !substring || *substring == '\0') {
        if (errptr) *errptr = M_ERR_NULL_POINTER; // Invalid input
        return -1;
    }

    char* found = strstr((char*)strbuffer->buffer.data, substring);
    if (found) {
        return found - (char*)strbuffer->buffer.data;
    } else {
        return -1;
    }
}

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
        case M_LOG_TRACE:
            fprintf(stdout, "[TRACE] ");
            break;
        case M_LOG_INFO:
            fprintf(stdout, "[INFO] ");
            break;
        case M_LOG_WARN:
            fprintf(stdout, "[WARN] ");
            break;
        case M_LOG_ERROR:
            fprintf(stdout, "[ERROR] ");
            break;
        case M_LOG_FATAL:
            fprintf(stdout, "[FATAL] ");
            break;
    }

    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
}
