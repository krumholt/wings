#ifndef types_h
#define types_h

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
typedef uint32_t      b32;

#define SET_CONSOLE_RED   "\x1b[31m"
#define SET_CONSOLE_GREEN "\x1b[32m"
#define RESET_CONSOLE     "\x1b[m"

#ifndef unused
#define unused(x) \
    (void)(x)
#endif

#define STR(...)  STR_(__VA_ARGS__)
#define STR_(...) #__VA_ARGS__

#ifndef array_length
#define array_length(x) \
    (sizeof(x) / sizeof(x[0]))
#endif

u64 kibibyte(u64 n)
{
    u64 size = { 1024 * n };
    return (size);
}

u64 mebibyte(u64 n)
{
    u64 size = { 1024 * 1024 * n };
    return (size);
}

float to_mebibyte(u64 n)
{
    return (n / 1024.0f / 1024.0f);
}

u64 gibibyte(u64 n)
{
    u64 size = { 1024 * 1024 * 1024 * n };
    return (size);
}

char *
set_to_closest_memory_unit(f64 *size)
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
    s32 unit_index = 0;
    while (*size > 1024.0)
    {
        *size = *size / 1024.0;
        ++unit_index;
        if (unit_index == 10)
            break;
    }
    return units[unit_index];
}

char *
set_to_closest_time_unit(f64 *time_in_ns)
{
    f64   time    = *time_in_ns;
    char *units[] = {
        "ns",
        "ms",
        "s",
        "m",
        "h",
        "d",
    };
    f64 increase[] = {
        1000.0,
        1000.0,
        60.0,
        60.0,
        24.0,
    };
    s32 unit_index = 0;
    while (time > increase[unit_index])
    {
        time = time / increase[unit_index];
        ++unit_index;
        if (unit_index == 5)
            break;
    }
    *time_in_ns = time;
    return units[unit_index];
}

f64 seconds_to_nanoseconds(f64 time)
{
    return (time * (1000.0 * 1000.0));
}

#ifndef max
s32 maxii(s32 a, s32 b)
{
    return (a > b ? a : b);
}

float maxff(float a, float b)
{
    return (a > b ? a : b);
}
#endif

#ifndef min
s32 min_ii(s32 a, s32 b)
{
    return (a < b ? a : b);
}
float min_ff(float a, float b)
{
    return (a < b ? a : b);
}
#endif

#endif
