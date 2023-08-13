#ifndef WINGS_OS_WINDOWS_MEMORY_C_
#define WINGS_OS_WINDOWS_MEMORY_C_

#include "wings/base/macros.c"
#include "wings/base/types.c"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#endif
#include <Windows.h>

enum os_memory_error
{
	os_memory_error_NO_ERROR,
	os_memory_error_FAILED_TO_ALLOCATE,
	os_memory_error_QUERY_UNKNOWN_STATE,
	os_memory_error_QUERY_FAILED,
	os_memory_error_ENUM_LENGTH,
};

#ifndef WINGS_OS_MEMORY_C_
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

#endif

error
os_reserve_memory(struct os_memory_block *block, u64 size)
{
    block->base = VirtualAlloc(0,
                               size,
                               MEM_RESERVE,
                               PAGE_READWRITE);
    if (block->base == 0)
        return (os_memory_error_FAILED_TO_ALLOCATE);
    block->size = size;

    return (0);
}

error
os_commit_memory(struct os_memory_block block)
{
    void *result = VirtualAlloc(block.base,
                                block.size,
                                MEM_COMMIT,
                                PAGE_READWRITE);
    return (block.base == result ? NO_ERROR : os_memory_error_FAILED_TO_ALLOCATE);
}

error
os_reserve_and_commit_memory(struct os_memory_block *block, u64 size)
{
    error error = 0;
    error       = os_reserve_memory(block, size);
    if (error)
        return error;

    error = os_commit_memory(*block);
    if (error)
        return error;

    return (NO_ERROR);
}

error
os_release_memory(struct os_memory_block block)
{
    b32 result = VirtualFree(block.base, 0, MEM_RELEASE);
    return (result == 0 ? os_memory_error_FAILED_TO_ALLOCATE : NO_ERROR);
}

error
os_decommit_memory(struct os_memory_block block)
{
    b32 result = VirtualFree(block.base, block.size, MEM_DECOMMIT);
    return (result == 0 ? os_memory_error_FAILED_TO_ALLOCATE : NO_ERROR);
}

error
os_get_memory_info(struct memory_info *info, struct os_memory_block block)
{
    MEMORY_BASIC_INFORMATION basic_info = { 0 };

    u64 query_size = VirtualQuery(block.base, &basic_info, sizeof(MEMORY_BASIC_INFORMATION));
    if (query_size != sizeof(MEMORY_BASIC_INFORMATION))
        return (os_memory_error_QUERY_FAILED);

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
        return (os_memory_error_QUERY_UNKNOWN_STATE);
    }
    info->state             = state;
    info->base_adress       = basic_info.BaseAddress;
    info->allocation_adress = basic_info.AllocationBase;
    info->region_size       = basic_info.RegionSize;

    return (NO_ERROR);
}

#endif
