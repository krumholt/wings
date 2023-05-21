#ifndef types_h
#define types_h

#include <stdint.h>

typedef char          int8;
typedef unsigned char uint8;
typedef int16_t       int16;
typedef uint16_t      uint16;
typedef int32_t       int32;
typedef uint32_t      uint32;
typedef int64_t       int64;
typedef uint64_t      uint64;
typedef float         float32;
typedef double        float64;
typedef uint32_t      bool32;

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
typedef uint32_t      b32;

#define SET_CONSOLE_RED   "\x1b[31m"
#define SET_CONSOLE_GREEN "\x1b[32m"
#define RESET_CONSOLE     "\x1b[m"

#ifndef unused
#define unused(x)                               \
    (void)(x)
#endif

#define STR(...) STR_(__VA_ARGS__)
#define STR_(...) # __VA_ARGS__

#ifndef array_length
#define array_length(x)     \
    (sizeof(x)/sizeof(x[0]))
#endif

uint64 kibibyte(uint64 n)
{
    uint64 size = {1024 * n};
    return(size);
}

uint64 mebibyte(uint64 n)
{
    uint64 size = {1024 * 1024 * n};
    return(size);
}

float to_mebibyte(uint64 n)
{
    return(n / 1024.0f / 1024.0f);
}

uint64 gibibyte(uint64 n)
{
    uint64 size = {1024 * 1024 * 1024 * n};
    return(size);
}

char *
set_to_closest_unit(f64 *size)
{
    char *units[] = {
        "B",
        "KB",
        "MB",
        "GB",
        "TB",
        "PB",
        "EB",
        "ZB",
        "YB",
        "RB",
        "QB",
    };
    s32 unit_index_memory_used = 0;
    while (*size > 1024.0)
    {
        *size = *size / 1024.0;
        ++unit_index_memory_used;
        if (unit_index_memory_used == 10)
            break;
    }
    return units[unit_index_memory_used];
}

#ifndef max
int32
maxii(int32 a, int32 b)
{
    return(a > b ? a : b);
}

float
maxff(float a, float b)
{
    return(a > b ? a : b);
}
#endif


#ifndef min
int32
min_ii(int32 a, int32 b)
{
    return(a < b ? a : b);
}
float
min_ff(float a, float b)
{
    return(a < b ? a : b);
}
#endif

#endif

