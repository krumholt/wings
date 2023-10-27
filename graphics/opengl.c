#ifndef WINGS_GRAPHICS_OPENGL_C_
#define WINGS_GRAPHICS_OPENGL_C_

#include "wings/base/macros.c"

#if defined(OS_WINDOWS)
#include "wings/os/windows/file.c"
#elif defined(OS_LINUX)
#include "wings/os/linux/file.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
