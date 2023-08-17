#ifndef WINGS_BASE_TYPES_C_
#define WINGS_BASE_TYPES_C_

#include <stdint.h>

typedef char          s8;
typedef unsigned char u8;
typedef int16_t       s16;
typedef uint16_t      u16;
typedef int32_t       s32;
typedef uint32_t      u32;
typedef int64_t       s64;
typedef uint64_t      u64;
typedef float         f32;
typedef double        f64;

typedef uint32_t b32;
typedef uint32_t error;
typedef error    merror;

const error NO_ERROR = 0;

struct buffer
{
    u8 *data;
    u64 size;
};

#endif
