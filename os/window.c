#ifndef WINGS_OS_WINDOW_C_
#define WINGS_OS_WINDOW_C_


#if defined(OS_WINDOWS)
#include "wings/os/windows/window.c"
#elif defined(OS_LINUX)
#include "wings/os/linux/window.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
