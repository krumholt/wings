#ifndef clock_c
#define clock_c
#include "clock.h"
#include <time.h>

struct clock
{
    struct timespec time;
};


inline void
clock_initialise(struct clock *clock)
{
    unused(clock);
}

inline b32
clock_enable_high_resolution(void)
{
    return 0;
}

inline b32
clock_disable_high_resolution(void)
{
    return 0;
}

inline void
clock_start(struct clock *clock)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &clock->time);
}

inline
int64
clock_get_tick()
{
    struct timespec time = {0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    int64 nanoseconds = (time.tv_sec) * (int64)1e9 + time.tv_nsec;
    return(nanoseconds);
}

inline double
clock_elapsed_time_seconds(struct clock *clock)
{
    struct timespec time = {0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    int64 difference = (time.tv_sec - clock->time.tv_sec) * (long)1e9 + (time.tv_nsec - clock->time.tv_nsec);
    return(difference/1000.0f/1000.0f);
}

inline double
clock_elapsed_time_milliseconds(struct clock *clock)
{
    struct timespec time = {0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    int64 difference = (time.tv_sec - clock->time.tv_sec) * (long)1e9 + (time.tv_nsec - clock->time.tv_nsec);
    return(difference/1000.0f/1000.0f);
}

#endif
