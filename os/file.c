#ifndef WINGS_OS_FILE_C_
#define WINGS_OS_FILE_C_

#include "../base/macros.c"

#if defined(OS_WINDOWS)
#include "windows/file.c"
#elif defined(OS_LINUX)
#include "linux/file.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
