#ifndef WINGS_BASE_UNITS_C_
#define WINGS_BASE_UNITS_C_

#ifndef WINGS_BASE_TYPES_C_
#include "wings/base/types.c"
#endif

u64
kibibytes(u64 n)
{
    u64 size = { 1024 * n };
    return (size);
}

u64
mebibytes(u64 n)
{
    u64 size = { 1024 * 1024 * n };
    return (size);
}

u64
gibibyte(u64 n)
{
    u64 size = { 1024 * 1024 * 1024 * n };
    return (size);
}

f64
bytes_to_mebibytes(u64 n)
{
    return (n / 1024.0f / 1024.0f);
}

const char *
set_to_closest_memory_unit(f64 *size)
{
    const char *units[] = {
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

f64
seconds_to_nanoseconds(f64 time)
{
    return (time * (1000.0 * 1000.0));
}

#endif
