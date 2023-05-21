#ifndef clock_c
#define clock_c
#include "types.h"
#pragma warning(push, 0)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define NOMINMAX
#include <windows.h>
#include <timeapi.h>
#pragma warning(pop)

struct clock
{
    u64 frequency_in_seconds;
    u64 _start_tick;
};

inline void
clock_initialise(struct clock *clock)
{
    LARGE_INTEGER time = {0};
    QueryPerformanceFrequency(&time);
    clock->frequency_in_seconds = (u64)time.QuadPart;
}

inline b32
clock_enable_high_resolution(void)
{
    b32 error = (timeBeginPeriod(1) == TIMERR_NOERROR);
    return error;
}

inline b32
clock_disable_high_resolution(void)
{
    b32 error = (timeEndPeriod(1) == TIMERR_NOERROR);
    return error;
}

inline void
clock_start(struct clock *clock)
{
    LARGE_INTEGER time = {0};
    QueryPerformanceCounter(&time);
    clock->_start_tick = (uint64)time.QuadPart;
}

inline
s64
clock_get_tick(void)
{
    LARGE_INTEGER time = {0};
    QueryPerformanceCounter(&time);
    return(time.QuadPart);
}

inline f64
clock_elapsed_time_seconds(struct clock *clock)
{
    LARGE_INTEGER Time = {0};
    QueryPerformanceCounter(&Time);
    return((f64)(Time.QuadPart - clock->_start_tick) / (f64)clock->frequency_in_seconds);
}

inline f64
clock_elapsed_time_milliseconds(struct clock *clock)
{
    LARGE_INTEGER Time = {0};
    QueryPerformanceCounter(&Time);
    return(((f64)(Time.QuadPart - clock->_start_tick) / (f64)clock->frequency_in_seconds)*1000.0);
}

#endif
