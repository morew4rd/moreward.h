#include "m.h"
#include "i.h"

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

