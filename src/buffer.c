#include "m.h"
#include "i.h"

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