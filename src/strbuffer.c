#include "m.h"
#include "i.h"

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