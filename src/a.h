#ifndef _A_H_INCLUDED_
#define _A_H_INCLUDED_
/* line will be cut */

/* aliases */

#include <stddef.h>



#define countof(a)      (isize)(sizeof(a) / sizeof(*(a)))
#define max(a, b)       ((a)>(b) ? (a) : (b))
#define min(a, b)       ((a)<(b) ? (a) : (b))

#define nil NULL


typedef int bool;
#define true (1)
#define false (0)


typedef float           f32;
typedef double          f64;

typedef signed char     i8;
typedef unsigned char   u8;
typedef signed short    i16;
typedef unsigned short  u16;
typedef signed int      i32;
typedef unsigned int    u32;
typedef signed long     i64;
typedef unsigned long   u64;
typedef u8              byte;

typedef size_t          usize;
typedef ptrdiff_t       isize;
typedef u32             uint;
typedef u64             uptr;

typedef int             ierr;


#endif  /* _A_H_INCLUDED_ */