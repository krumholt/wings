#ifndef WINGS_OS_LINUX_MEMORY_C_
#define WINGS_OS_LINUX_MEMORY_C_

#include "wings/base/types.c"
#include "wings/base/macros.c"

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef WINGS_OS_MEMORY_C_
enum memory_state
{
    memory_state_commited,
    memory_state_reserved,
    memory_state_free,
};

struct os_allocation
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
os_reserve_memory(struct os_allocation *block, u64 size)
{
	block->base = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block->base == MAP_FAILED)
	{
		return (1);
	}
	block->size = size;
    return (0);
}

error
os_commit_memory(struct os_allocation block)
{
	int result = mprotect(block.base, block.size, PROT_READ | PROT_WRITE);
    return (result == -1 ? 1 : 0);
}

error
os_reserve_and_commit_memory(struct os_allocation *block, u64 size)
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
os_release_memory(struct os_allocation block)
{
	int result = munmap(block.base, block.size);
	error error = 0;
	if (result == -1)
		error = 1;

    return (error);
}

error
os_decommit_memory(struct os_allocation block)
{
	int result = mprotect(block.base, block.size, PROT_NONE);
	error error = 0;
	if (result == -1)
		error = 1;
    return (error);
}

u64
os_get_page_size(void)
{
    u32 page_size = getpagesize();
    return (page_size);
}

u64
os_get_allocation_size(void)
{
    u32 allocation_size = getpagesize();
    return (allocation_size);
}

error
os_get_memory_info(struct memory_info *info, struct os_allocation block)
{
	UNUSED(info);
	UNUSED(block);
	WARN("os_get_memory_info not implemented in linux");
	return(1);
}

#endif
