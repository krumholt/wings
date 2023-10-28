#ifndef WINGS_OS_LINUX_TIMER_C_
#define WINGS_OS_LINUX_TIMER_C_

#include "wings/base/types.c"

#include <intrin.h>
#include <time.h>

u64
get_os_timer_frequency(void)
{
   return 1e9;
}

u64
get_os_timer(void)
{
   struct timespec time = { 0 };
   clock_gettime(CLOCK_MONOTONIC_RAW, &time);

   u64 nanoseconds = (time.tv_sec) * (u64)1e9 + time.tv_nsec;
   return (nanoseconds);
}

f64
get_os_timer_in_seconds(void)
{
   struct timespec time = { 0 };
   clock_gettime(CLOCK_MONOTONIC_RAW, &time);

   f64 nanoseconds = (time.tv_sec) + time.tv_nsec / (f64)1e9;
   return (nanoseconds);
}

u64
get_cpu_timer(void)
{
   return __rdtsc();
}

#endif
