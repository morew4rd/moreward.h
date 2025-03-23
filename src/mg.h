#ifndef _MG_H
#define _MG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define m_assert(x)     assert(x)
#else
#define m_assert(x)
#endif

#define null NULL

typedef void        Void;
typedef bool        Bool;
typedef int8_t      I8;
typedef int16_t     I16;
typedef int32_t     I32;
typedef int64_t     I64;
typedef uint8_t     U8;
typedef uint16_t    U16;
typedef uint32_t    U32;
typedef uint64_t    U64;
typedef float       F32;
typedef double      F64;
typedef char       *Str;
typedef const char *CStr;
typedef size_t      Sz;
typedef ptrdiff_t   ISz;
typedef int         IErr;

#define m_countof(a)                (Sz)(sizeof(a) / sizeof(*(a)))
#define m_max(a, b)                 ((a)>(b) ? (a) : (b))
#define m_min(a, b)                 ((a)<(b) ? (a) : (b))

#define m_alloc(n)                  m_get_allocator()->malloc((n), m_get_allocator()->userdata)
#define m_realloc(p,n)              m_get_allocator()->realloc((p), (n), m_get_allocator()->userdata)
#define m_free(p)                   m_get_allocator()->free((p), m_get_allocator()->userdata)

#define m_tracelog(level, file, line, format, ...) \
                                    m_log_raw(level, "[%s:%d] " format, file, line, ##__VA_ARGS__)
#define m_log(format, ...)          m_log_raw(M_LOG_INFO, format, ##__VA_ARGS__)
#define m_log_trace(format, ...)    m_tracelog(M_LOG_TRACE, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_info(format, ...)     m_tracelog(M_LOG_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_warn(format, ...)     m_tracelog(M_LOG_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_error(format, ...)    m_tracelog(M_LOG_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_fatal(format, ...)    m_tracelog(M_LOG_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

enum {
    M_OK = 0,
    M_ERR_NULL_POINTER = 1000000,
    M_ERR_ALLOCATION_FAILED,
    M_ERR_OUT_OF_BOUNDS,
    M_ERR_INVALID_OPERATION,
};

typedef struct m_Allocator {
    Void* (*malloc)(Sz size, Void* userdata);
    Void* (*realloc)(Void* ptr, Sz new_size, Void* userdata);
    Void  (*free)(Void* ptr, Void* userdata);
    Void* userdata;
} m_Allocator;

typedef struct m_Buffer {
    U8* data;
    I32 itemsize;
    I32 itemcap;
    m_Allocator* allocator;
} m_Buffer;

typedef I32 (*m_ItemComparer)(Void* item1, Void* item2);

typedef struct m_List {
    m_Buffer buffer;
    I32 count;
    m_ItemComparer comparer;
} m_List;

typedef struct m_Dict {
    m_List keys;
    m_List values;
} m_Dict;

typedef struct m_StrBuffer {
    m_Buffer buffer;
    I32 length;
} m_StrBuffer;

typedef enum m_LogLevel {
    M_LOG_TRACE,
    M_LOG_INFO,
    M_LOG_WARN,
    M_LOG_ERROR,
    M_LOG_FATAL
} m_LogLevel;

Void m_set_loglevel(m_LogLevel level);
Void m_log_raw(m_LogLevel level, CStr format, ...);

Void m_set_allocator(m_Allocator *allocator);
Void m_reset_allocator(Void);
m_Allocator *m_get_allocator(Void);

// Buffer functions
m_Buffer* mb_create(I32 itemsize, I32 itemcap);
Void mb_destroy(m_Buffer* buffer);
IErr mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap);
IErr mb_setcap(m_Buffer* buffer, I32 newcap);

// List functions
m_List* ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer);
Void ml_destroy(m_List* list);
IErr ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer);
IErr ml_setcap(m_List* list, I32 newcap);
Void ml_clear(m_List* list);
IErr ml_push(m_List* list, Void* item);
Void* ml_pop(m_List* list);
Void* ml_get(m_List* list, I32 index);
IErr ml_put(m_List* list, I32 index, Void* item);
IErr ml_insert(m_List* list, I32 index, Void* item);
IErr ml_remove(m_List* list, I32 index);
IErr ml_remove_range(m_List* list, I32 startindex, I32 count);
IErr ml_remove_swap(m_List* list, I32 index);
I32 ml_count(m_List* list);
I32 ml_find(m_List* list, Void* item);
Void ml_sort(m_List* list);

// Dictionary functions
m_Dict* md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer);
Void md_destroy(m_Dict* dict);
IErr md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer);
IErr md_setcap(m_Dict* dict, I32 newcap);
Void md_clear(m_Dict* dict);
Void* md_get(m_Dict* dict, Void* key);
IErr md_put(m_Dict* dict, Void* key, Void* value);
Bool md_has(m_Dict* dict, Void* key);
IErr md_remove(m_Dict* dict, Void* key);
IErr md_remove_ordered(m_Dict* dict, Void* key);
I32 md_count(m_Dict* dict);

// String Buffer functions
m_StrBuffer* ms_create(I32 itemcap);
Void ms_destroy(m_StrBuffer* strbuffer);
IErr ms_init(m_StrBuffer* strbuffer, I32 itemcap);
IErr ms_setcap(m_StrBuffer* strbuffer, I32 newcap);
Void ms_clear(m_StrBuffer* strbuffer);
Str ms_getstr(m_StrBuffer* strbuffer);
IErr ms_cat(m_StrBuffer* strbuffer, CStr format, ...);
Void ms_trim(m_StrBuffer* strbuffer);
IErr ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest);
I32 ms_find(m_StrBuffer* strbuffer, CStr substring);

#endif /* _MG_H */