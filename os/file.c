#ifndef WINGS_OS_FILE_C_
#define WINGS_OS_FILE_C_


#if !defined(OS_WINDOWS) && !defined(OS_LINUX)
#define OS_WINDOWS
#endif
#if defined(OS_WINDOWS)
#include "windows/file.c"
#elif defined(OS_LINUX)
#include "linux/file.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
