#include "m.h"
#include "i.h"

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