#define MOREWARD_IMPL
#include "moreward.h"
#include <stdio.h>

void str_buffer_example(void) {
    m_log_info("--- StrBuffer Example ---");

    // Creating a StrBuffer without initialization
    m_StrBuffer sb = {0};
    m_log_info("StrBuffer created without initialization");

    // // Initializing the StrBuffer with a capacity of 100
    // ms_init(&sb, 100, NULL, NULL);
    // m_log_info("StrBuffer initialized with capacity 100");

    // Concatenating strings
    ms_cat(&sb, "Hello, ");
    ms_cat(&sb, "world!");
    m_log_info("Concatenated strings: %s", ms_getstr(&sb, NULL));

    // Clearing the StrBuffer
    ms_clear(&sb, NULL);
    m_log_info("StrBuffer cleared");
}

void list_example(void) {
    m_log_info("--- List Example ---");

    // Creating a List of integers
    m_List* list = ml_create(sizeof(I32), 10, NULL, NULL, NULL);
    m_log_info("List created with capacity 10");

    // Pushing items into the list
    I32 items[] = {1, 2, 3, 4, 5};
    for (I32 i = 0; i < m_countof(items); ++i) {
        ml_push(list, &items[i], NULL);
    }
    m_log_info("Pushed items into the list");

    // Getting and printing items from the list
    for (I32 i = 0; i < ml_count(list, NULL); ++i) {
        I32* item = (I32*)ml_get(list, i, NULL);
        m_log_info("List item at index %d: %d", i, *item);
    }

    // Destroying the list
    ml_destroy(list, NULL);
    m_log_info("List destroyed");
}

void dict_example(void) {
    m_log_info("--- Dict Example ---");

    // Creating a Dict with integer keys and string values
    m_Dict* dict = md_create(sizeof(I32), sizeof(Str), 10, NULL, NULL, NULL);
    m_log_info("Dict created with capacity 10");

    // Putting key-value pairs into the dict
    I32 keys[] = {1, 2, 3};
    Str values[] = {"one", "two", "three"};
    for (I32 i = 0; i < m_countof(keys); ++i) {
        md_put(dict, &keys[i], &values[i], NULL);
    }
    m_log_info("Put key-value pairs into the dict");

    // Getting and printing values from the dict
    for (I32 i = 0; i < m_countof(keys); ++i) {
        Str* value = (Str*)md_get(dict, &keys[i], NULL);
        m_log_info("Dict value for key %d: %s", keys[i], *value);
    }

    // Destroying the dict
    md_destroy(dict, NULL);
    m_log_info("Dict destroyed");
}

void logging_example(void) {
    m_log_info("--- Logging Example ---");

    // Setting the log level to INFO
    m_set_loglevel(M_LOG_INFO);
    m_log_info("Log level set to INFO");

    // Logging messages at different levels
    m_log_trace("This is a trace message");
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

// Custom realloc function for the stack allocator
static void* stack_realloc(void* ptr, size_t new_size, void* userdata) {
    // Stack allocator does not support realloc, so we just return NULL
    return NULL;
}

// Custom free function for the stack allocator
static void stack_free(void* ptr, void* userdata) {
    // Stack allocator does not support free, so we do nothing
}

// Function to initialize the stack allocator
void init_stack_allocator(StackAllocator* stack, U8* buffer, size_t size) {
    stack->stack_base = buffer;
    stack->stack_top = buffer;
    stack->stack_end = buffer + size;
}

// Function to reset the stack allocator
void reset_stack_allocator(StackAllocator* stack) {
    stack->stack_top = stack->stack_base;
}

void custom_allocator_example(void) {
    m_log_info("--- Custom Allocator Example ---");

    // Creating a stack allocator with a buffer of 101 bytes
    U8 buffer[101];
    StackAllocator stack;
    init_stack_allocator(&stack, buffer, sizeof(buffer));

    // Creating a custom allocator using the stack allocator
    m_Allocator custom_allocator = {
        .malloc = stack_malloc,
        .realloc = stack_realloc,
        .free = stack_free,
        .userdata = &stack
    };

    // Setting the custom allocator
    m_set_allocator(&custom_allocator);
    m_log_info("Custom allocator set");

    // Creating a StrBuffer using the custom allocator
    m_StrBuffer sb = {0};
    ms_init(&sb, 100, &custom_allocator, NULL); // anything larger would crash the app.
    m_log_info("StrBuffer created using custom allocator");

    // Concatenating strings
    ms_cat(&sb, "Hello, ");
    ms_cat(&sb, "world!");
    m_log_info("Concatenated strings: %s", ms_getstr(&sb, NULL));

    // Resetting the stack allocator
    reset_stack_allocator(&stack);
    m_log_info("Stack allocator reset");

    // Destroying the StrBuffer
    ms_destroy(&sb, NULL);
    m_log_info("StrBuffer destroyed");

    // Resetting the allocator to the default
    m_reset_allocator();
    m_log_info("Allocator reset to default");
}

I32 main(I32 argc, Str *argv) {
    str_buffer_example();
    list_example();
    dict_example();
    logging_example();
    custom_allocator_example();

    return 0;
}