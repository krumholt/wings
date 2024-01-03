#ifndef WINGS_BASE_TYPES_H_
#define WINGS_BASE_TYPES_H_

#include <stdint.h>

typedef signed char   s8;
typedef unsigned char u8;
typedef signed char   i8;
typedef int16_t       s16;
typedef int16_t       i16;
typedef uint16_t      u16;
typedef int32_t       s32;
typedef int32_t       i32;
typedef uint32_t      u32;
typedef int64_t       s64;
typedef int64_t       i64;
typedef uint64_t      u64;
typedef float         f32;
typedef double        f64;

typedef char     b8;
typedef uint32_t b32;
typedef uint32_t error;
typedef error    merror;

struct buffer
{
   u8   *base;
   u64   used;
   u64   size;
};

struct string
{
   char  *first;
   u64    length;
};


#endif
