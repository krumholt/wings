#ifndef WINGS_OS_TIMER_C_
#define WINGS_OS_TIMER_C_

#if defined(OS_WINDOWS)
#include "windows/timer.c"
#elif defined(OS_LINUX)
#include "linux/timer.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
