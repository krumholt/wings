#ifndef WINGS_OS_PROCESS_C_
#define WINGS_OS_PROCESS_C_

#include "wings/base/macros.c"

#if defined(OS_WINDOWS)
#include "wings/os/windows/process.c"
#elif defined(OS_LINUX)
#include "wings/os/linux/process.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX. Yes FU Apple."
#endif

#endif
