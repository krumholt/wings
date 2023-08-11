#ifndef OS_MEMORY_C
#define OS_MEMORY_C

#include "base/types.h"

enum memory_state
{
    memory_state_commited,
    memory_state_reserved,
    memory_state_free,
};

struct memory_info
{
    void             *base_adress;
    void             *allocation_adress;
    u64               region_size;
    enum memory_state state;
};

#if defined(OS_WINDOWS)
#include "os/windows_os_memory.c"
#elif defined(OS_LINUX)
#error "OS_LINUX not implemented"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
