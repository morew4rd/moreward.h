#ifndef  _M_H_
#define  _M_H_
/* this is dev/test time header. in the final "amalgamated" version (moreward.h), this is on the top of the file. */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef M_DISABLE_ASSERTS
#include <assert.h>
#define m_assert(x)     assert(x)
#else
#define m_assert(x)
#endif

#define nil NULL

typedef void        Void;
typedef void*       VPtr;
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
typedef char*       Str;
typedef const char* CStr;
typedef size_t      USize;
typedef ptrdiff_t   ISize;
typedef int32_t     IErr;

#define m_countof(a)                (size_t)(sizeof(a) / sizeof(*(a)))
#define m_max(a, b)                 ((a)>(b) ? (a) : (b))
#define m_min(a, b)                 ((a)<(b) ? (a) : (b))

#define m_alloc(n)                  m_get_allocator()->malloc((n), m_get_allocator()->userdata);
#define m_realloc(p,n)              m_get_allocator()->realloc((p), (n), m_get_allocator()->userdata);
#define m_free(p)                   m_get_allocator()->free((p), m_get_allocator()->userdata)

#define m_tracelog(level, file, line, format, ...) \
                                    m_log_raw(level, "[%s:%d] " format, file, line, ##__VA_ARGS__)
#define m_log(format, ...)          m_log_raw(M_LOG_INFO, format, ##__VA_ARGS__)
#define m_log_trace(format, ...)    m_tracelog(M_LOG_TRACE, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_info(format, ...)     m_tracelog(M_LOG_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_warn(format, ...)     m_tracelog(M_LOG_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_error(format, ...)    m_tracelog(M_LOG_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define m_log_fatal(format, ...)    m_tracelog(M_LOG_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define m_try(expr, errptr) \
    do { \
        expr; \
        if (errptr && *(errptr) != 0) { \
            m_log_error("Error in %s: %d", __func__, *(errptr)); \
            return; \
        } \
    } while (0)

#define m_try_ret(expr, errptr, retval) \
    do { \
        expr; \
        if (errptr && *(errptr) != 0) { \
            m_log_error("Error in %s: %d", __func__, *(errptr)); \
            return (retval); \
        } \
    } while (0)

enum {
    M_ERR_NULL_POINTER = -1,
};

typedef struct m_Allocator {
    void* (*malloc)(size_t size, void* userdata);
    void* (*realloc)(void* ptr, size_t new_size, void* userdata);
    void  (*free)(void* ptr, void* userdata);
    void* userdata;
} m_Allocator;

typedef struct m_Buffer {
    U8* data;
    I32 itemsize;
    I32 itemcap;
    m_Allocator* allocator;
} m_Buffer;

typedef I32 (*m_ItemComparer)(void* item1, void* item2);

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

void m_set_loglevel(m_LogLevel level);
void m_log_raw(m_LogLevel level, const char* format, ...);

void m_set_allocator(m_Allocator *allocator);
void m_reset_allocator(void);
m_Allocator *m_get_allocator(void);

m_Buffer* mb_create(I32 itemsize, I32 itemcap, m_Allocator* allocator, IErr* errptr);
void mb_destroy(m_Buffer* buffer, IErr* errptr);
m_List* ml_create(I32 itemsize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr);
void ml_destroy(m_List* list, IErr* errptr);
m_Dict* md_create(I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr);
void md_destroy(m_Dict* dict, IErr* errptr);
m_StrBuffer* ms_create(I32 itemcap, m_Allocator* allocator, IErr* errptr);
void ms_destroy(m_StrBuffer* strbuffer, IErr* errptr);

void mb_init(m_Buffer* buffer, I32 itemsize, I32 itemcap, m_Allocator* allocator, IErr* errptr);
void mb_setcap(m_Buffer* buffer, I32 newcap, IErr* errptr);

void ml_init(m_List* list, I32 itemsize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr);
void ml_setcap(m_List* list, I32 newcap, IErr* errptr);
void ml_clear(m_List* list, IErr* errptr);
void ml_push(m_List* list, void* item, IErr* errptr);
void* ml_pop(m_List* list, IErr* errptr);
I32 ml_count(m_List* list, IErr* errptr);
void* ml_get(m_List* list, I32 index, IErr* errptr);
void ml_insert(m_List* list, I32 index, void* item, IErr* errptr);
void ml_remove(m_List* list, I32 index, IErr* errptr);
void ml_remove_range(m_List* list, I32 startindex, I32 count, IErr* errptr);
void ml_remove_swap(m_List* list, I32 index, IErr* errptr);
I32 ml_find(m_List* list, void* item, IErr* errptr);
void ml_sort(m_List* list, IErr* errptr);

void md_init(m_Dict* dict, I32 keysize, I32 valuesize, I32 itemcap, m_ItemComparer comparer, m_Allocator* allocator, IErr* errptr);
void md_setcap(m_Dict* dict, I32 newcap, IErr* errptr);
void md_clear(m_Dict* dict, IErr* errptr);
void* md_get(m_Dict* dict, void* key, IErr* errptr);
void md_put(m_Dict* dict, void* key, void* value, IErr* errptr);
Bool md_has(m_Dict* dict, void* key, IErr* errptr);
void md_remove(m_Dict* dict, void* key, IErr* errptr);
void md_remove_ordered(m_Dict* dict, void* key, IErr* errptr);

void ms_init(m_StrBuffer* strbuffer, I32 itemcap, m_Allocator* allocator, IErr* errptr);
void ms_setcap(m_StrBuffer* strbuffer, I32 newcap, IErr* errptr);
void ms_clear(m_StrBuffer* strbuffer, IErr* errptr);
char* ms_getstr(m_StrBuffer* strbuffer, IErr* errptr);
void ms_cat(m_StrBuffer* strbuffer, const char* format, ...);
void ms_trim(m_StrBuffer* strbuffer, IErr* errptr);
void ms_substr(m_StrBuffer* strbuffer, I32 start, I32 length, m_StrBuffer* dest, IErr* errptr);
I32 ms_find(m_StrBuffer* strbuffer, const char* substring, IErr* errptr);

#endif  // _M_H_