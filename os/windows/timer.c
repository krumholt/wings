#ifndef WINGS_OS_WINDOWS_TIMER_C_
#define WINGS_OS_WINDOWS_TIMER_C_

#include "wings/base/types.h"

#include <intrin.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

u64
get_os_timer_frequency(void)
{
   LARGE_INTEGER frequency;
   QueryPerformanceFrequency(&frequency);
   return frequency.QuadPart;
}

u64
get_os_timer(void)
{
   LARGE_INTEGER value;
   QueryPerformanceCounter(&value);
   return value.QuadPart;
}

f64
get_os_timer_in_seconds(void)
{
   LARGE_INTEGER value;
   QueryPerformanceCounter(&value);
   return value.QuadPart / (f64)get_os_timer_frequency();
}

u64
get_cpu_timer(void)
{
   return __rdtsc();
}

#endif
