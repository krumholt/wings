#ifndef LINUX_OS_MEMORY_C
#define LINUX_OS_MEMORY_C

#include "base/types.h"

#include <sys/mman.h>

error
os_reserve_memory(void **memory, u64 size)
{
    *memory = VirtualAlloc(0,
                           size,
                           MEM_RESERVE,
                           PAGE_READWRITE);
    if (memory == 0)
        return (1);

    return (0);
}

error
os_commit_memory(void *memory, u64 size)
{
    void *result = VirtualAlloc(memory,
                                size,
                                MEM_COMMIT,
                                PAGE_READWRITE);
    return (memory == result ? 0 : 1);
}

error
os_release_memory(void *memory)
{
    b32 result = VirtualFree(memory, 0, MEM_RELEASE);
    return (result == 0 ? 1 : 0);
}

error
os_decommit_memory(void *memory, u64 size)
{
    b32 result = VirtualFree(memory, size, MEM_DECOMMIT);
    return (result == 0 ? 1 : 0);
}

error
os_get_memory_info(struct memory_info *info, void *memory)
{
    MEMORY_BASIC_INFORMATION basic_info = { 0 };

    u64 query_size = VirtualQuery(memory, &basic_info, sizeof(MEMORY_BASIC_INFORMATION));
    if (query_size != sizeof(MEMORY_BASIC_INFORMATION))
        return (1);

    enum memory_state state = { 0 };
    switch (basic_info.State)
    {
    case MEM_RESERVE:
        state = memory_state_reserved;
        break;
    case MEM_FREE:
        state = memory_state_free;
        break;
    case MEM_COMMIT:
        state = memory_state_commited;
        break;
    default:
        return (2);
    }
    info->state             = state;
    info->base_adress       = basic_info.BaseAddress;
    info->allocation_adress = basic_info.AllocationBase;
    info->region_size       = basic_info.RegionSize;

    return (0);
}

#endif
