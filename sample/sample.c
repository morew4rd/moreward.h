#define MOREWARD_IMPL
#include "moreward.h"
#include <stdio.h>

// Example: String Buffer Usage
void str_buffer_example(void) {
    m_log_info("--- StrBuffer Example ---");

    // Create a StrBuffer with capacity 100 using the global allocator
    m_StrBuffer* sb = ms_create(100);
    if (!sb) {
        m_log_error("Failed to create StrBuffer");
        return;
    }
    m_log_info("StrBuffer created with capacity 100");

    // Concatenate strings
    ms_cat(sb, "Hello, ");
    ms_cat(sb, "world!");
    m_log_info("Concatenated strings: %s", ms_getstr(sb));

    // Clear the StrBuffer
    ms_setcap(sb, 0);
    m_log_info("StrBuffer cleared");

    // Destroy the StrBuffer
    ms_destroy(sb);
    m_log_info("StrBuffer destroyed");
}

// Example: List Usage
void list_example(void) {
    m_log_info("--- List Example ---");

    // Create a List of integers with capacity 10
    m_List* list = ml_create(sizeof(I32), 10, NULL);
    if (!list) {
        m_log_error("Failed to create List");
        return;
    }
    m_log_info("List created with capacity 10");

    // Push items into the list
    I32 items[] = {1, 2, 3, 4, 5};
    for (I32 i = 0; i < m_countof(items); ++i) {
        if (ml_push(list, &items[i]) != 0) {
            m_log_error("Failed to push item %d", items[i]);
        }
    }
    m_log_info("Pushed items into the list");

    // Get and print items from the list
    for (I32 i = 0; i < ml_count(list); ++i) {
        I32* item = (I32*)ml_get(list, i);
        if (item) {
            m_log_info("List item at index %d: %d", i, *item);
        } else {
            m_log_warn("Item at index %d not found", i);
        }
    }

    // Destroy the list
    ml_destroy(list);
    m_log_info("List destroyed");
}

// Example: Dictionary Usage
void dict_example(void) {
    m_log_info("--- Dict Example ---");

    // Create a Dict with integer keys and string values, capacity 10
    m_Dict* dict = md_create(sizeof(I32), sizeof(Str), 10, NULL);
    if (!dict) {
        m_log_error("Failed to create Dict");
        return;
    }
    m_log_info("Dict created with capacity 10");

    // Put key-value pairs into the dict
    I32 keys[] = {1, 2, 3};
    Str values[] = {"one", "two", "three"};
    for (I32 i = 0; i < m_countof(keys); ++i) {
        if (md_put(dict, &keys[i], &values[i]) != 0) {
            m_log_error("Failed to put key %d", keys[i]);
        }
    }
    m_log_info("Put key-value pairs into the dict");

    // Get and print values from the dict
    for (I32 i = 0; i < m_countof(keys); ++i) {
        Str* value = (Str*)md_get(dict, &keys[i]);
        if (value) {
            m_log_info("Dict value for key %d: %s", keys[i], *value);
        } else {
            m_log_warn("Key %d not found", keys[i]);
        }
    }

    // Destroy the dict
    md_destroy(dict);
    m_log_info("Dict destroyed");
}

// Example: Logging Levels
void logging_example(void) {
    m_log_info("--- Logging Example ---");

    // Set log level to INFO
    m_set_loglevel(M_LOG_INFO);
    m_log_info("Log level set to INFO");

    // Log messages at different levels
    m_log_trace("This is a trace message"); // Won't be printed
    m_log_info("This is an info message");
    m_log_warn("This is a warning message");
    m_log_error("This is an error message");
    m_log_fatal("This is a fatal message");
}

// Custom allocator structure for a simple stack allocator
typedef struct {
    U8* stack_base;
    U8* stack_top;
    U8* stack_end;
} StackAllocator;

// Custom malloc function for the stack allocator
static void* stack_malloc(size_t size, void* userdata) {
    StackAllocator* stack = (StackAllocator*)userdata;
    if (stack->stack_top + size > stack->stack_end) {
        return NULL; // Out of memory
    }
    void* ptr = stack->stack_top;
    stack->stack_top += size;
    return ptr;
}

// Custom realloc function for the stack allocator (not supported)
static void* stack_realloc(void* ptr, size_t new_size, void* userdata) {
    return NULL; // Realloc not supported
}

// Custom free function for the stack allocator (not supported)
static void stack_free(void* ptr, void* userdata) {
    // Free not supported
}

// Initialize the stack allocator
void init_stack_allocator(StackAllocator* stack, U8* buffer, size_t size) {
    stack->stack_base = buffer;
    stack->stack_top = buffer;
    stack->stack_end = buffer + size;
}

// Reset the stack allocator
void reset_stack_allocator(StackAllocator* stack) {
    stack->stack_top = stack->stack_base;
}

// Example: Custom Allocator Usage
void custom_allocator_example(void) {
    m_log_info("--- Custom Allocator Example ---");

    // Create a stack allocator with a buffer of 101 bytes
    U8 buffer[101];
    StackAllocator stack = {0};
    init_stack_allocator(&stack, buffer, sizeof(buffer));

    // Create a custom allocator using the stack allocator
    m_Allocator custom_allocator = {
        .malloc = stack_malloc,
        .realloc = stack_realloc,
        .free = stack_free,
        .userdata = &stack
    };

    // Set the custom allocator
    m_set_allocator(&custom_allocator);
    m_log_info("Custom allocator set");

    // Create a StrBuffer using the custom allocator
    m_StrBuffer* sb = ms_create(50); // Anything larger would exceed stack size
    if (!sb) {
        m_log_error("Failed to create StrBuffer with custom allocator");
        return;
    }
    m_log_info("StrBuffer created using custom allocator");

    // Concatenate strings
    ms_cat(sb, "Hello, ");
    ms_cat(sb, "world!");
    m_log_info("Concatenated strings: %s", ms_getstr(sb));

    // Reset the stack allocator
    reset_stack_allocator(&stack);
    m_log_info("Stack allocator reset");

    // Destroy the StrBuffer
    ms_destroy(sb);
    m_log_info("StrBuffer destroyed");

    // Reset to the default allocator
    m_reset_allocator();
    m_log_info("Allocator reset to default");
}

I32 main(I32 argc, Str* argv) {
    str_buffer_example();
    list_example();
    dict_example();
    logging_example();
    custom_allocator_example();
    return 0;
}