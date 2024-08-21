#include "m.h"
#include "i.h"

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
