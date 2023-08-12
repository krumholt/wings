#ifndef WINGS_OS_MEMORY_C_
#define WINGS_OS_MEMORY_C_

#include "wings/base/types.c"

enum memory_state
{
    memory_state_commited,
    memory_state_reserved,
    memory_state_free,
};

struct os_memory_block
{
	u8 *base;
	u64 size;
};

struct memory_info
{
    void             *base_adress;
    void             *allocation_adress;
    u64               region_size;
    enum memory_state state;
};

#if defined(OS_WINDOWS)
#include "wings/os/windows/memory.c"
#elif defined(OS_LINUX)
#include "wings/os/linux/memory.c"
#else
#error "Operating system not set #define OS_WINDOWS or OS_LINUX"
#endif

#endif
